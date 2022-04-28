package config

import (
	"github.com/go-yaml/yaml"
	"io/ioutil"
	"time"
)

var (
	Cfg Config
)

type PostgresConfig struct {
	Host     string `yaml:"host"`
	Port     string `yaml:"port"`
	Username string `yaml:"username"`
	Password string `yaml:"password"`
	DBName   string `yaml:"db_name"`
	SSLMode  string `yaml:"ssl_mode"`
}

type Config struct {
	PgConf           PostgresConfig `yaml:"postgres"`
	AccessTokenTTL   time.Duration  `yaml:"access_token_ttl"`
	RefreshTokenTTL  time.Duration  `yaml:"refresh_token_ttl"`
	Salt             string         `yaml:"salt"`
	SigningAccessKey string         `yaml:"signing_access_key"`
	TeacherKey       string         `yaml:"teacher_key"`
}

func NewConfig(configPath string) error {
	b, err := ioutil.ReadFile(configPath)
	if err != nil {
		return err
	}
	err = yaml.Unmarshal(b, &Cfg)
	return err
}
