package httpdelivery

import (
	"encoding/json"
	"net/http"
	"tester/models"
)

func (d Delivery) mul(w http.ResponseWriter, r *http.Request) {
	mulReq := models.MulRequest{}
	err := json.NewDecoder(r.Body).Decode(&mulReq)
	if err != nil {
		d.newErrorResponse(w, r, http.StatusUnprocessableEntity, err)
		return
	}
	resp, err := d.services.Mul(r.Context(), mulReq)
	if err != nil {
		d.newErrorResponse(w, r, http.StatusUnprocessableEntity, err)
		return
	}

	d.newResponse(w, r, http.StatusOK, resp)
}

func (d Delivery) code(w http.ResponseWriter, r *http.Request) {
	codeReq := map[string]int32{}
	err := json.NewDecoder(r.Body).Decode(&codeReq)
	if err != nil {
		d.newErrorResponse(w, r, http.StatusUnprocessableEntity, err)
		return
	}
	resp, err := d.services.Code(r.Context(), codeReq["num"])
	if err != nil {
		d.newErrorResponse(w, r, http.StatusUnprocessableEntity, err)
		return
	}

	d.newResponse(w, r, http.StatusOK, resp)
}

func (d Delivery) mulOn2(w http.ResponseWriter, r *http.Request) {
	mulOn2Req := models.MulOn2Request{}
	err := json.NewDecoder(r.Body).Decode(&mulOn2Req)
	if err != nil {
		d.newErrorResponse(w, r, http.StatusUnprocessableEntity, err)
		return
	}
	resp, err := d.services.MulOn2(r.Context(), mulOn2Req)
	if err != nil {
		d.newErrorResponse(w, r, http.StatusUnprocessableEntity, err)
		return
	}

	d.newResponse(w, r, http.StatusOK, resp)
}
