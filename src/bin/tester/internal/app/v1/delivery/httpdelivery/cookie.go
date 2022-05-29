package httpdelivery

import (
	"net/http"
	"time"
)

func (d Delivery) setRefreshTokenCookie(UUID string, w http.ResponseWriter) {
	cookies := []http.Cookie{
		{
			Name:     "refresh_token",
			Value:    UUID,
			Expires:  time.Unix(2*213123312312, 0),
			HttpOnly: true,
		},
	}

	for _, cookie := range cookies {
		http.SetCookie(w, &cookie)
	}
}
