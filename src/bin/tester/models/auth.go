package models

type Authorization struct {
	Login    string `json:"login"`
	Password string `json:"password"`
}

type AuthorizationWithUserAgent struct {
	Authorization
	UserAgent string
}

type Registration struct {
	ProfileWithGroup
	TeacherKey string `json:"teacher_key,omitempty"`
	Password   string `json:"password"`
}

type AuthorizationResponse struct {
	Prof   interface{} `json:"profile"`
	Access string      `json:"access_token"`
	UUID   string
}
