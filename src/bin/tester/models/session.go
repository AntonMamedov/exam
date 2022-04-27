package models

const (
	TeacherRole = "teacher"
	StudentRole = "student"
	AdminRole   = "admin"
)

type AccessToken struct {
	ProfileWithGroup
	Exp int64 `json:"exp,omitempty"`
}

type Session struct {
	UUID      string `db:"uuid"`
	ProfileID uint64 `db:"profile_id"`
	UserAgent string `db:"user_agent"`
	Expires   string `db:"expires"`
}
