package main

import (
	"fmt"
	"strings"
)

func main() {
	neType := ""
	neType = strings.ToUpper(neType)
	splitNe := strings.SplitN(neType, "-", 2)
	if len(splitNe) < 1 {
		fmt.Printf("invalid ne type: %s", neType)
	}
	neType = splitNe[0]
	fmt.Println(neType, splitNe)
}
