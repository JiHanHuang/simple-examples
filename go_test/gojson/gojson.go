package main

import (
	"encoding/json"
	"fmt"
)

func main() {
	l1()
}

func l1() {
	in := "{\"11\":1,\"12\":1,\"13\":1}"
	var out map[string]interface{}
	if err := json.Unmarshal([]byte(in), &out); err != nil {
		fmt.Println(err)
	}
	fmt.Println(out)
	var change []interface{}
	var inItem = make(map[string]interface{}, 0)
	for k, v := range out {
		inItem["key"] = k
		inItem["value"] = v
		change = append(change, inItem)
	}
	data, err := json.Marshal((change))
	if err != nil {
		fmt.Println(err)
	}
	fmt.Println(string(data))
}

func l2() {
	in := "{\"authorization\":{\"type\":\"equal\",\"value\":\"\",\"copy\":0},\"login.timezone \":{\"type\":\"equal\",\"value\":\"GMT+8:00\",\"copy\":0},\"login_validatePwdStrength\":{\"type\":\"equal\",\"value\":\"4\",\"copy\":0},\"random\":{\"type\":\"equal\",\"value\":\"\",\"copy\":0},\"screenWidth\":{\"type\":\"equal\",\"value\":\"1920\",\"copy\":0},\"screenHeight\":{\"type\":\"equal\",\"value\":\"1080\",\"copy\":0}}"
	var out map[string](map[string]interface{})
	if err := json.Unmarshal([]byte(in), &out); err != nil {
		fmt.Println(err)
	}

	var change []interface{}
	for k, v := range out {
		v["key"] = k
		change = append(change, v)
	}
	data, err := json.Marshal((change))
	if err != nil {
		fmt.Println(err)
	}
	fmt.Println(string(data))
}
