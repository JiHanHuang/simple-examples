package main

import (
	"crypto/hmac"
	"crypto/sha256"
	"encoding/base64"
	"fmt"
	"net/url"
	"sort"
	"strconv"
	"strings"
	"time"
)

func generateSignature(parameterMap map[string]string, appSecret string) (string, error) {
	// Step 1: Sort parameters
	keys := make([]string, 0, len(parameterMap))
	for key := range parameterMap {
		if key != "signature" {
			keys = append(keys, key)
		}
	}
	sort.Strings(keys)
	// fmt.Println("sort keys:", keys)

	// Step 2: Concatenate parameter names and values
	var concatenatedString strings.Builder
	for _, key := range keys {
		concatenatedString.WriteString(key)
		concatenatedString.WriteString(parameterMap[key])
	}
	// fmt.Println("string:", concatenatedString.String())

	// Step 3: HmacSHA256 Calculation
	hmacSha256 := hmac.New(sha256.New, []byte(appSecret))
	hmacSha256.Write([]byte(concatenatedString.String()))
	hmacData := hmacSha256.Sum(nil)
	// fmt.Printf("hmacsha256:%0x\n", hmacData)
	// hmacData, err := hex.DecodeString("e0839903bbcc27d69c24717193ee963a49b08755ff5ee278c043020223f5864f")
	// if err != nil {
	// 	return "", err
	// }

	// Step 4: Base64 Encoding
	base64Encoded := base64.StdEncoding.EncodeToString(hmacData)
	// fmt.Println("base64:", base64Encoded)

	// Step 5: URL Encoding
	return url.QueryEscape(base64Encoded), nil
}

func main() {
	// Example usage
	parameterMap := map[string]string{
		"appKey":    "6db7c5e804224b3594ab659b659501f2",
		"timestamp": strconv.FormatInt(time.Now().UnixMilli(), 10),
	}
	appSecret := "31c8df7664582f1df317a24ee187a391105021fc"

	signature, err := generateSignature(parameterMap, appSecret)
	if err != nil {
		fmt.Println("Error:", err)
		return
	}

	fmt.Println("Generated Signature:", signature)
	fmt.Println(parameterMap)
}
