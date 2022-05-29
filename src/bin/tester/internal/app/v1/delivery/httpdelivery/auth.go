package httpdelivery

import (
	"encoding/json"
	"errors"
	"fmt"
	"net/http"
	"tester/models"
	"tester/pkg/ctx"
	apperror "tester/pkg/error"
)

func (d Delivery) signUp(w http.ResponseWriter, r *http.Request) {
	reg := models.Registration{}
	err := json.NewDecoder(r.Body).Decode(&reg)
	if err != nil {
		d.newErrorResponse(w, r, http.StatusUnprocessableEntity, err)
		return
	}

	if isCorrectRole(reg.Role[0]) == false {
		d.newErrorResponse(w, r, http.StatusBadRequest, errors.New("bad role"))
		return
	}

	userAgent := r.UserAgent()

	auth := models.AuthorizationResponse{}
	if reg.Role[0] == models.TeacherRole {
		auth, err = d.services.SignUpTeacher(r.Context(), reg, userAgent)
	} else {
		auth, err = d.services.SignUpStudent(r.Context(), reg, userAgent)
	}

	if err != nil && err == apperror.DuplicateError {
		d.newErrorResponse(w, r, http.StatusConflict, fmt.Errorf("progile with login %s already exists", reg.Login))
		return
	}

	if err != nil {
		d.newErrorResponse(w, r, http.StatusInternalServerError, err)
		return
	}

	d.setRefreshTokenCookie(auth.UUID, w)
	d.newResponse(w, r, http.StatusCreated, auth)
}

func (d Delivery) signIn(w http.ResponseWriter, r *http.Request) {
	a := models.AuthorizationWithUserAgent{
		UserAgent: r.UserAgent(),
	}
	err := json.NewDecoder(r.Body).Decode(&a.Authorization)
	if err != nil {
		d.newErrorResponse(w, r, http.StatusUnprocessableEntity, err)
		return
	}

	auth, err := d.services.SignIn(r.Context(), a)

	if err != nil && err == apperror.RecordNotFound {
		d.newErrorResponse(w, r, http.StatusNotFound, err)
		return
	}

	if err != nil {
		d.newErrorResponse(w, r, http.StatusInternalServerError, err)
		return
	}

	d.setRefreshTokenCookie(auth.UUID, w)
	d.newResponse(w, r, http.StatusOK, auth)
}

func (d Delivery) auth(w http.ResponseWriter, r *http.Request) {
	accessToken := r.Context().Value(ctx.TokenDataKey).(models.AccessToken)

	d.newResponse(w, r, http.StatusOK, accessToken.ProfileWithGroup)
}

func (d Delivery) refresh(w http.ResponseWriter, r *http.Request) {
	refreshTokenJson := map[string]string{}
	err := json.NewDecoder(r.Body).Decode(&refreshTokenJson)
	if err != nil {
		d.newErrorResponse(w, r, http.StatusUnprocessableEntity, err)
		return
	}

	if _, ok := refreshTokenJson["refresh_token"]; !ok {
		d.newErrorResponse(w, r, http.StatusUnprocessableEntity, errors.New("bad json"))
		return
	}

	UUID := refreshTokenJson["refresh_token"]
	if err != nil {
		d.newErrorResponse(w, r, http.StatusInternalServerError, err)
		return
	}

	accessToken, refreshToken, err := d.services.Refresh(r.Context(), UUID, r.UserAgent())
	if err != nil {
		d.newErrorResponse(w, r, http.StatusUnauthorized, err)
		return
	}

	d.setRefreshTokenCookie(refreshToken, w)
	d.newResponse(w, r, http.StatusCreated, map[string]string{
		"access_token": accessToken,
	})
}

func isCorrectRole(role string) bool {
	return role == models.TeacherRole || role == models.StudentRole
}
