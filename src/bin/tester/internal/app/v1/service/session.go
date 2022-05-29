package service

import (
	"context"
	"crypto/sha1"
	"errors"
	"fmt"
	"tester/internal/app/v1/repository"
	"tester/models"
	"tester/pkg/config"
	apperr "tester/pkg/error"
	"tester/pkg/logger"
	"tester/pkg/tokenmanager"
)

type AuthService struct {
	rep          repository.Repository
	tokenManager tokenmanager.TokenManager
}

func NewSessionService(rep repository.Repository) (*AuthService, error) {
	if rep == nil {
		return nil, errors.New("expected repository, got nil")
	}

	tm, err := tokenmanager.NewManager(config.Cfg.SigningAccessKey)
	if err != nil {
		return nil, err
	}
	return &AuthService{
		rep:          rep,
		tokenManager: tm,
	}, nil
}

func (a AuthService) SignUpTeacher(ctx context.Context, reg models.Registration, userAgent string) (models.AuthorizationResponse, error) {
	tx, err := a.rep.Begin()
	if err != nil {
		return models.AuthorizationResponse{}, err
	}
	defer processingEndTx(ctx, tx, &err)

	a.rep = tx

	// Проверяем корректен ли преподавательский ключ
	if reg.TeacherKey != config.Cfg.TeacherKey {
		return models.AuthorizationResponse{}, errors.New("bad teacher key")
	}

	// Общая регистрация
	auth, err := a.signUp(ctx, reg, userAgent)

	return auth, err
}

func (a AuthService) SignUpStudent(ctx context.Context, reg models.Registration, userAgent string) (models.AuthorizationResponse, error) {
	tx, err := a.rep.Begin()
	if err != nil {
		return models.AuthorizationResponse{}, err
	}
	defer processingEndTx(ctx, tx, &err)
	a.rep = tx

	g, err := a.rep.SelectGroupByName(ctx, reg.Name)
	if err != nil {
		return models.AuthorizationResponse{}, err
	}

	if g == (models.Group{}) {
		return models.AuthorizationResponse{}, fmt.Errorf("group not exists")
	}

	// Общая регистрация
	authResp, err := a.signUp(ctx, reg, userAgent)

	if err != nil {
		return models.AuthorizationResponse{}, err
	}
	// Создаем связть профиля с грруппой
	p := authResp.Prof.(models.Profile)
	err = a.rep.CreateProfileGroupRelation(ctx, p.ID, g.ID)
	if err != nil {
		return models.AuthorizationResponse{}, apperr.NewServiceError(err)
	}

	authResp.Prof = models.ProfileWithGroup{
		Profile: p,
		Group:   g.Name,
	}

	return authResp, nil
}

func (a AuthService) signUp(ctx context.Context, reg models.Registration, userAgent string) (models.AuthorizationResponse, error) {
	tx, err := a.rep.Begin()
	if err != nil {
		return models.AuthorizationResponse{}, err
	}
	defer processingEndTx(ctx, tx, &err)
	a.rep = tx

	// Проверяем корректность роли (учитель/ученик)
	if reg.Role[0] != models.TeacherRole && reg.Role[0] != models.StudentRole {
		return models.AuthorizationResponse{}, apperr.BadRoleError
	}

	// Записываем профиль в базу
	p, err := a.rep.CreateProfile(ctx, reg.Profile, generatePasswordHash(reg.Password))
	if err != nil {
		return models.AuthorizationResponse{}, apperr.NewServiceError(err)
	}

	pg := models.ProfileWithGroup{
		Group:   reg.Group,
		Profile: p,
	}
	access, _, err := a.tokenManager.NewAccessJWT(
		models.AccessToken{
			ProfileWithGroup: pg,
		}, config.Cfg.AccessTokenTTL)

	if err != nil {
		return models.AuthorizationResponse{}, apperr.NewServiceError(err)
	}

	// Записываем рефреш токен в базу
	refresh, err := a.rep.CreateSession(ctx, p.ID, userAgent)
	if err != nil {
		logger.LoggingError(ctx, logger.ServiceLocation, errors.New("refresh token not created"))
		return models.AuthorizationResponse{}, apperr.NewServiceError(err)
	}

	return models.AuthorizationResponse{
		Prof:   p,
		Access: access,
		UUID:   refresh.UUID,
	}, nil
}

func (a AuthService) SignIn(ctx context.Context, auth models.AuthorizationWithUserAgent) (models.AuthorizationResponse, error) {
	tx, err := a.rep.Begin()
	if err != nil {
		return models.AuthorizationResponse{}, err
	}
	defer processingEndTx(ctx, tx, &err)
	a.rep = tx
	// Достаем профиль с паролем из базы
	p, err := a.rep.GetProfileWithPassByLogin(ctx, auth.Login)
	if err != nil {
		return models.AuthorizationResponse{}, apperr.NewServiceError(err)
	}

	// Проверяем существует ли профиль с таким логином
	if p.Login == "" {
		logger.LoggingDebug(ctx, logger.ServiceLocation, fmt.Sprintf("profile with login %s not found", auth.Login))
		return models.AuthorizationResponse{}, apperr.RecordNotFound
	}

	// Проверяем правильность пароля
	if passEqual(auth.Password, p.Pass) == false {
		return models.AuthorizationResponse{}, apperr.BadPasswordError
	}

	// Достаем группу из базы если студент
	group := models.Group{}
	if p.Role[0] == models.StudentRole {
		group, err = a.rep.SelectGroupByProfileId(ctx, p.ID)
		if err != nil {
			return models.AuthorizationResponse{}, err
		}
	}

	// Пробуем достать из базы рефреш токен с присланными profile_id и user_agent
	refresh, err := a.rep.CreateSession(ctx, p.ID, auth.UserAgent)
	if err != nil {
		return models.AuthorizationResponse{}, apperr.NewServiceError(err)
	}
	pg := models.ProfileWithGroup{
		Group:   group.Name,
		Profile: p.Profile,
	}
	access, _, err := a.tokenManager.NewAccessJWT(
		models.AccessToken{
			ProfileWithGroup: pg,
		}, config.Cfg.AccessTokenTTL)

	if err != nil {
		return models.AuthorizationResponse{}, apperr.NewServiceError(err)
	}

	return models.AuthorizationResponse{
		Prof:   pg,
		Access: access,
		UUID:   refresh.UUID,
	}, nil
}

func (a AuthService) Identity(ctx context.Context, accessToken string) (models.AccessToken, error) {
	at, err := a.tokenManager.ParseAccessToken(accessToken)
	if err != nil && err != apperr.TokenExpired {
		logger.LoggingError(ctx, logger.ServiceLocation, err)
		return models.AccessToken{}, apperr.NewServiceError(err)
	}

	if err == apperr.TokenExpired {
		return models.AccessToken{}, err
	}

	return at, nil
}

func (a AuthService) Refresh(ctx context.Context, UUID string, userAgent string) (string, string, error) {
	tx, err := a.rep.Begin()
	if err != nil {
		return "", "", err
	}
	defer processingEndTx(ctx, tx, &err)
	a.rep = tx
	session, err := a.rep.SelectSessionByUUID(ctx, UUID, userAgent)
	if err != nil {
		return "", "", err
	}

	p, err := a.rep.GetProfileById(ctx, session.ProfileID)
	if err != nil {
		return "", "", err
	}

	access, _, err := a.tokenManager.NewAccessJWT(
		models.AccessToken{
			ProfileWithGroup: p,
		}, config.Cfg.AccessTokenTTL)

	if err != nil {
		return "", "", err
	}

	refresh, err := a.rep.CreateSession(ctx, p.ID, userAgent)

	if err != nil {
		return "", "", err
	}

	return access, refresh.UUID, nil
}

func generatePasswordHash(password string) string {
	hash := sha1.New()
	hash.Write([]byte(password))

	return fmt.Sprintf("%x", hash.Sum([]byte(config.Cfg.Salt)))
}

func passEqual(pass string, dbPass string) bool {
	pass = generatePasswordHash(pass)
	return pass == dbPass
}
