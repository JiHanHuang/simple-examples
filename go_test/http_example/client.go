package main

import (
	"bytes"
	"encoding/json"
	"flag"
	"fmt"
	"io/ioutil"
	"net/http"
	"time"
)

func postJson(client *http.Client, url string, data interface{}) ([]byte, error) {
	reqMsg, err := json.Marshal(data)
	if err != nil {
		return nil, fmt.Errorf("sendPostJsonMsg json marshal failed. %v", err)
	}
	req, err := http.NewRequest("POST", url, bytes.NewBuffer(reqMsg))
	if err != nil {
		return nil, err
	}
	req.Header.Set("Content-Type", "application/json")
	resp, err := client.Do(req)
	if err != nil {
		return nil, fmt.Errorf("connect failed. %v", err)
	}
	defer resp.Body.Close()
	if (resp.StatusCode != http.StatusOK) &&
		(resp.StatusCode != http.StatusCreated) {
		return nil, fmt.Errorf("connect refused. status:%s", resp.Status)
	}

	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return nil, fmt.Errorf("read response msg failed. %v", err)
	}
	return body, nil
}

func SendHTTPJson(data interface{}, url string, timeout int) ([]byte, error) {
	if timeout <= 0 {
		timeout = 3
	}
	client := &http.Client{Timeout: time.Duration(timeout) * time.Second}
	return postJson(client, url, data)
}

func main() {
	var url string
	flag.StringVar(&url, "url", "http://127.0.0.1:80", "connect url")
	flag.Parse()
	body, err := SendHTTPJson("hello", url, 3)
	if err != nil {
		fmt.Println("Err:", err)
	}
	fmt.Println("Resp:", string(body))
}
