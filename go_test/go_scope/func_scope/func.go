/*
 * @Author: JiHan
 * @Date: 2021-08-05 09:56:38
 * @LastEditTime: 2021-08-05 10:44:33
 * @LastEditors: JiHan
 * @Description:
 * @Usage:
 */

package func_scope

import "fmt"

func CallBack(f func(int) error) {
	if err := f(1); err != nil {
		fmt.Print(err)
	}
	fmt.Println("CallBack func finish")
}
