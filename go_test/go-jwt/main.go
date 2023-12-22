package main

import (
	"fmt"
	"sort"
	"strconv"
	"strings"
	"time"

	"github.com/dgrijalva/jwt-go"
	"github.com/google/uuid"
)

const (
	appID     = "5c4bd090fa0a49c4"
	appSecret = "****"
)

// SignatureUtil 结构表示签名工具
type SignatureUtil struct{}

// GetSign 方法用于获取签名
func (su *SignatureUtil) GetSign(sortedParams map[string]string) string {
	var content strings.Builder
	var keys []string

	for key := range sortedParams {
		keys = append(keys, key)
	}

	sort.Strings(keys)

	index := 0
	for _, key := range keys {
		value := sortedParams[key]
		if su.areNotEmpty(key, value) {
			content.WriteString(fmt.Sprintf("%s%s=%s", su.getSeparator(index), key, value))
			index++
		}
	}

	return content.String()
}

// areNotEmpty 方法检查字符串数组中的所有值是否非空
func (*SignatureUtil) areNotEmpty(values ...string) bool {
	for _, value := range values {
		if value == "" {
			return false
		}
	}
	return true
}

// getSeparator 方法返回连接字符串的分隔符
func (*SignatureUtil) getSeparator(index int) string {
	if index == 0 {
		return ""
	}
	return "&"
}

func main() {
	// 初始化map
	m := make(map[string]string)

	// 添加时间戳、随机字符串和密钥到map
	m["timestamp"] = strconv.FormatInt(time.Now().UnixNano()/int64(time.Millisecond), 10)
	m["noncestr"] = uuid.New().String()
	m["secretKey"] = appSecret

	// 计算签名
	signatureUtil := &SignatureUtil{}
	sign := signatureUtil.GetSign(m)
	fmt.Println("Sign:", sign)
	// 创建JWT
	claims := jwt.MapClaims{}
	claims["aud"] = appID // 请替换为实际的AppID

	// 将map中的声明添加到JWT
	delete(m, "secretKey")
	for key, value := range m {
		claims[key] = value
	}

	// 签名JWT
	token := jwt.NewWithClaims(jwt.SigningMethodHS256, claims)
	tokenString, err := token.SignedString([]byte(sign))
	if err != nil {
		fmt.Println("Error creating JWT:", err)
		return
	}

	// 输出生成的JWT
	fmt.Println("Generated JWT:", tokenString)
}
