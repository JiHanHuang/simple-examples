package main

import (
	"fmt"
	"reflect"
)

type MyStruct struct {
	Name  string
	Age   int
	Score float64
}

func main() {
	data := MyStruct{
		Name:  "Alice",
		Age:   30,
		Score: 95.5,
	}

	// 通过反射读取字段值
	fieldName := "Age" // 要读取的字段名称
	fieldValue, err := getFieldValue(data, fieldName)
	if err != nil {
		fmt.Printf("Error reading field %s: %v\n", fieldName, err)
	} else {
		fmt.Printf("%s: %v\n", fieldName, fieldValue)
	}

	// 通过反射设置字段值
	fieldName = "Score" // 要设置的字段名称
	newValue := 99.0    // 新的字段值
	err = setFieldValue(&data, fieldName, newValue)
	if err != nil {
		fmt.Printf("Error setting field %s: %v\n", fieldName, err)
	} else {
		fmt.Printf("%s has been set to %v\n", fieldName, newValue)
		fmt.Printf("Updated data: %+v\n", data)
	}
}

// 通过反射读取字段值
func getFieldValue(data interface{}, fieldName string) (interface{}, error) {
	value := reflect.ValueOf(data)
	if value.Kind() != reflect.Struct {
		return nil, fmt.Errorf("Input is not a struct")
	}

	fieldValue := value.FieldByName(fieldName)
	if !fieldValue.IsValid() {
		return nil, fmt.Errorf("Field %s does not exist in the struct", fieldName)
	}

	return fieldValue.Interface(), nil
}

// 通过反射设置字段值
func setFieldValue(data interface{}, fieldName string, newValue interface{}) error {
	value := reflect.ValueOf(data)
	if value.Kind() != reflect.Ptr || value.Elem().Kind() != reflect.Struct {
		return fmt.Errorf("Input is not a pointer to a struct")
	}

	fieldValue := value.Elem().FieldByName(fieldName)
	if !fieldValue.IsValid() {
		return fmt.Errorf("Field %s does not exist in the struct", fieldName)
	}

	if !fieldValue.CanSet() {
		return fmt.Errorf("Field %s is unexported and cannot be set", fieldName)
	}

	newValueReflect := reflect.ValueOf(newValue)
	if !newValueReflect.Type().AssignableTo(fieldValue.Type()) {
		return fmt.Errorf("New value type is not assignable to the field type")
	}

	fieldValue.Set(newValueReflect)
	return nil
}
