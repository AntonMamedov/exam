package httpdelivery

import (
	"encoding/json"
	"net/http"
	"tester/pkg/logger"
)

type errorResponse struct {
	Message string `json:"message"`
}

func (d Delivery) newResponse(w http.ResponseWriter, r *http.Request, status int, payload interface{}) {
	w.WriteHeader(status)

	if payload != nil {
		err := json.NewEncoder(w).Encode(payload)
		if err != nil {
			d.newErrorResponse(w, r, http.StatusInternalServerError, err)
			return
		}
	}

	logger.LoggingResponse(r.Context(), logger.DeliveryLocation, status)
}

func (d Delivery) newErrorResponse(w http.ResponseWriter, r *http.Request, status int, err error) {
	if err != nil {
		logger.LoggingError(r.Context(), logger.DeliveryLocation, err)
	}
	d.newResponse(w, r, status, err.Error())
}
