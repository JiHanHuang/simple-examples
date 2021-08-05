/*
 * @Author: JiHan
 * @Date: 2021-08-05 09:57:05
 * @LastEditTime: 2021-08-05 14:34:14
 * @LastEditors: JiHan
 * @Description:
 * @Usage:
 */

package var_scope

import (
	"fmt"
	"reflect"
)

type VS struct {
	Out   int
	inner int
}

func PrintOtherStruct(data interface{}) {
	val := reflect.ValueOf(data)
	typeOfTstObj := val.Type()
	for i := 0; i < val.NumField(); i++ {
		fieldType := val.Field(i)
		//fieldType.Interface() cannot get the 'inner' value
		fmt.Printf("object field %d key=%s value=%v type=%s \n",
			i, typeOfTstObj.Field(i).Name, fieldType.Int(),
			fieldType.Type())
	}

	fmt.Println("var scope print", data)
}
