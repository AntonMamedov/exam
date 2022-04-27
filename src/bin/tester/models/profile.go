package models

type Profile struct {
	ID      uint64   `json:"id,omitempty" db:"id"`
	Login   string   `json:"login,omitempty" db:"login"`
	Name    string   `json:"name,omitempty" db:"name"`
	Surname string   `json:"surname,omitempty" db:"surname"`
	Role    []string `json:"role,omitempty" db:"role"`
}

type ProfileWithGroup struct {
	Profile
	Group string `json:"group,omitempty"`
}

type ProfileWithPassword struct {
	Profile
	Pass string `db:"password"`
}
