package main

import (
	"flag"
	log "github.com/sirupsen/logrus"
	"google.golang.org/grpc"
	"net/http"
	"tester/internal/app/v1/delivery"
	"tester/internal/app/v1/delivery/httpdelivery"
	arithmetical "tester/internal/app/v1/repository/grpc/protos"
	"tester/internal/app/v1/repository/postgres"
	"tester/internal/app/v1/service"
	"tester/pkg/config"
)

var (
	enableHttps = false
)

func main() {
	addr := flag.String("addr", "127.0.0.1:8000", "addr")
	cfg := flag.String("c", "../../../cfg/config.yml", "config file path")
	flag.Parse()
	err := config.NewConfig(*cfg)
	if err != nil {
		log.Fatal(err)
	}

	log.SetLevel(log.DebugLevel)
	db, err := postgres.NewPostgresDB(config.Cfg.PgConf)
	if err != nil {
		log.Fatal(err)
	}
	defer db.Close()

	rep, err := postgres.NewPostgresRepository(db)
	if err != nil {
		log.Fatal(err)
	}

	conn, err := grpc.Dial(
		"127.0.0.1:8081",
		grpc.WithInsecure())
	defer conn.Close()
	serv, err := service.NewService(rep, arithmetical.NewArithmeticalClient(conn))
	if err != nil {
		log.Fatal(err)
	}
	d, err := delivery.NewDelivery(serv)
	if err != nil {
		log.Fatal(err)
	}

	router := d.InitRoutes()

	c := d.HTTP.CorsMiddleware(httpdelivery.Origin)
	server := http.Server{
		Addr:    *addr,
		Handler: c.Handler(router),
	}

	err = server.ListenAndServe()
	if err != nil {
		log.Fatal(err)
	}

}
