/*
 * @Author: JiHan
 * @Date: 2021-09-14 18:49:36
 * @LastEditTime: 2021-09-14 19:14:27
 * @LastEditors: JiHan
 * @Description:
 * @Usage:
 */

package goreflectexample

import (
	"fmt"
	"reflect"
	"strconv"
)

func formatAtom(v reflect.Value) string {
	switch v.Kind() {
	case reflect.Invalid:
		return "Invalid"
	case reflect.Int, reflect.Int8, reflect.Int16,
		reflect.Int32, reflect.Int64:
		return strconv.FormatInt(v.Int(), 10)
	//....简化起见，省略浮点数和复数
	case reflect.Bool:
		return strconv.FormatBool(v.Bool())
	case reflect.String:
		return v.String()
	case reflect.Chan, reflect.Func, reflect.Ptr,
		reflect.Slice, reflect.Map:
		return v.Type().String() + " 0x" +
			strconv.FormatUint(uint64(v.Pointer()), 16)
	default: //reflect.Array, reflect.Struct, reflect.Interface
		return v.Type().String() + " value"
	}
}

func display(path string, v reflect.Value) {
	switch v.Kind() {
	case reflect.Invalid:
		fmt.Printf("%s = Invalid\n", path)
	case reflect.Slice, reflect.Array:
		for i := 0; i < v.Len(); i++ {
			display(fmt.Sprintf("%s[%d]\n", path, i), v.Index(i))
		}
	case reflect.Struct:
		for i := 0; i < v.NumField(); i++ {
			feildPath := fmt.Sprintf("%s.%s", path, v.Type().Field(i).Name)
			display(feildPath, v.Field(i))
		}
	case reflect.Map:
		// 另外一个遍历map的方式
		// iter := v.MapRange()
		// for iter.Next() {
		// 	k := iter.Key()
		// 	va := iter.Value()
		// 	display(fmt.Sprintf("%s[%s]\n", path, formatAtom(k)), va)
		// }

		for _, key := range v.MapKeys() {
			display(fmt.Sprintf("%s[%s]\n", path, formatAtom(key)), v.MapIndex(key))
		}
	case reflect.Ptr:
		if v.IsNil() {
			fmt.Printf("%s = Nil\n", path)
		} else {

			fmt.Printf("%s.type = %s\n", path, v.Elem().Type())
			display(path+".value", v.Elem())
		}
	default: // 基本类型、通道、函数
		fmt.Printf("%s = %s\n", path, formatAtom(v))

	}
}

func Display(name string, x interface{}) {
	fmt.Printf("Display %s (%T)\n", name, x)
	display(name, reflect.ValueOf(x))
}
