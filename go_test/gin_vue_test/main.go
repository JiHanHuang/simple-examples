package main

import (
	"net/http"
	"time"

	"github.com/gin-gonic/gin"
)

func Init() {
	// Creates a default gin router
	r := gin.Default() // Grouping routes

	// group： v1
	v1 := r.Group("/v1")
	{
		v1.GET("/date", DatePage)
	}
	r.Run(":8000") // listen and serve on 0.0.0.0:8000
}

func DatePage(c *gin.Context) {
	c.JSON(http.StatusOK, gin.H{
		"date": time.Now().Format(time.ANSIC),
	})
}

func main() {
	Init() // init router
}
