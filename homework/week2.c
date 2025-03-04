#include<stdio.h>

int int_shifts_are_arithmetic()
{
	int a = -1;
	/*
	 * 算术位移会在负数位移时在高位补 1，逻辑位移则为补 0
	 * 若为前者，位移后仍为负值，否则位移后会变为正数
	 * 故只需判断位移时后的正负性，即为函数返回值
	 */
	return (a >> 1) < 0;
}

int int_size_is_32()
{
	/*
	 * A. C语言标准没有规定 (x << k) 操作在 k >= w (w表示x的数据类型的位数）时该如何表现，不同机器采用不同的做法，
	 * 在某些机器上（例如本题的）可能会选择对 k 作 k mod w 得到真正的位移量
	 * 此时若 k == 32, 则位移量为 0 ，beyond_msb 并未如期赋值为 0 ，发生错误
	 *
	 * B. 如下代码
	 *
	 * C. 如下代码
	 *
	 */

	/* 将 k >= w 的一次位移拆分成 k < w 的多次位移 */
	int set_msb = 1 << 8;
	set_msb = set_msb << 8;
	set_msb = set_msb << 8;
	set_msb = set_msb << 7;
	int beyond_msb = set_msb << 1;
	/*
	 * 在 w >= 32 的机器上 set_maxu 是非 0 数
	 * 在 w <= 32 的机器上 beyond_maxu_ 是 0
	 */

	return set_msb && !beyond_msb;
}

int main()
{
	printf("The result of int_shifts_are_arithmetic is %d\n", int_shifts_are_arithmetic());
	printf("The result of int_size_is_32 is %d\n", int_size_is_32());
	return 0;
}
