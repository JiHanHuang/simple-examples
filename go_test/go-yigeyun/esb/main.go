package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"net/http"
	"time"

	"github.com/dgrijalva/jwt-go"
	"github.com/google/uuid"
)

func main() {
	// 创建 JWT
	token := createJWT("733ffd8bddd44696", "cdeb6701fc9e4cbfad200a3d27d2fa93")

	// 打印生成的 JWT
	fmt.Println("token:", token)

	// 请求体的数据，这里使用一个示例 JSON 对象
	requestBody := map[string]interface{}{
		"size": "30",
		"page": "50",
		//"startTime": fmt.Sprintf("%d", time.Now().Unix()),
		"startTime": fmt.Sprintf("%d", time.Date(2000, time.Now().Month(), time.Now().Day(), time.Now().Hour(), time.Now().Minute(), time.Now().Second(), 0, time.Local).Unix()),
	}

	// 将请求体转换为 JSON 字符串
	jsonData, err := json.Marshal(requestBody)
	if err != nil {
		fmt.Println("Error encoding JSON:", err)
		return
	}

	fmt.Println("jsonData:", string(jsonData))

	// 创建一个请求
	// 人员
	request, err := http.NewRequest("POST", "http://103.245.129.53:8081/api/idmaccount/list", bytes.NewBuffer(jsonData))
	// 部门
	// request, err := http.NewRequest("POST", "http://103.245.129.53:8081/api/idmorg/list", bytes.NewBuffer(jsonData))
	//岗位
	// request, err := http.NewRequest("POST", "http://103.245.129.53:8081/api/idmjob/list", bytes.NewBuffer(jsonData))
	if err != nil {
		fmt.Println("Error creating request:", err)
		return
	}

	// reqTime := time.Now().Format("2006-01-02 15:04:05")
	reqTime := "2000-01-02 15:04:05"

	// 设置请求头部
	// request.Header.Set("reqId", uuid.New().String())
	request.Header.Set("reqId", "f5b35f41-0a4c-4b8c-9072-cdda39f56bb5")
	request.Header.Set("reqFrom", "IT-aqpt001")
	request.Header.Set("charset", "UTF-8")
	request.Header.Set("reqTime", reqTime)
	request.Header.Set("Content-Type", "application/json")
	request.Header.Set("Token", "Bearer "+token)
	request.Header.Set("Authorization", "Basic SVQtYXFwdDAwMTpJVC1hcXB0MDAxMjAyMg==") // 例如，设置身份验证头部

	fmt.Println("Headers:", request.Header)
	return
	// 发送请求并获取响应
	client := http.Client{}
	response, err := client.Do(request)
	if err != nil {
		fmt.Println("Error sending request:", err)
		return
	}
	defer response.Body.Close()

	// 处理响应
	fmt.Println("Response Status:", response.Status)

	// 读取响应体
	var responseBody map[string]interface{}
	err = json.NewDecoder(response.Body).Decode(&responseBody)
	if err != nil {
		fmt.Println("Error decoding JSON response:", err)
		return
	}

	// 打印响应体
	fmt.Println("Response Body:", responseBody)
}

// createJWT 函数：创建 JWT
func createJWT(appID string, secretKey string) string {
	token := jwt.NewWithClaims(jwt.SigningMethodHS256, jwt.MapClaims{
		"iss": appID,
		"iat": time.Now().Unix(),
		"jti": uuid.New().String(),
		//"exp":       time.Now().Add(time.Hour).Unix(), // 过期时间设置为一小时后
	})

	// 生成 token 字符串
	tokenString, err := token.SignedString([]byte(secretKey)) // 用于签名的密钥，请替换成你的实际密钥
	if err != nil {
		fmt.Println("Error creating token:", err)
		return ""
	}

	return tokenString
}
