/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
	/*
	 * 布尔代数运算
	 * 记 x 和 y 分别为一个位
	 * (x & y) 等价于 ~(~(x & y)) 等价于 ~(~x | ~y)
	 */

	return ~(~x | ~y); 
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
	/*
	 * 使用右移运算符，将目标字节右移到最低位
	 * 再与 0xff 做与运算
	 * 仅获取最低字节的内容
	 */ 

	return (x >> (n << 3)) & 0xff;
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
	/*
	 * 先右移，由于逻辑右移在左侧补 0
	 * 故将高位补充的 1 或 0 都与 0 做与运算
	 */
	int xx = x >> n;
	int y = xx & (((1 << (31 + ~n + 1)) << 1) + ~1 + 1);

	return y;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
	/*
	 * 相邻两个位相加，即为每 2 个位一组的每个组中 1 的个数
	 * 将上一行的结果，相邻两个 2 位数相加，可得每 4 个位一组，每组中 1 的个数
	 * 将上一行的结果，相邻两个 4 位数相加，可得每 8 个位一组，每组中 1 的个数
	 * 以此类推直到全部位中 1 的个数
	 */
	int xx = x;
	int mask_1 = 0x55;
	int mask_2 = 0x33;
	int mask_4 = 0x0f;
	int mask_8 = 0xff;
	int mask_16 = 0xff;
	mask_1 = (mask_1 << 8) + mask_1;
	mask_1 = (mask_1 << 16) + mask_1;
	mask_2 = (mask_2 << 8) + mask_2;
	mask_2 = (mask_2 << 16) + mask_2;
	mask_4 = (mask_4 << 8) + mask_4;
	mask_4 = (mask_4 << 16) + mask_4;
	mask_8 = (mask_8 << 16) + mask_8;
	mask_16 = (mask_16 << 8) + mask_16;
	xx = (xx & mask_1) + ((xx >> 1) & mask_1);
	xx = (xx & mask_2) + ((xx >> 2) & mask_2);
	xx = (xx & mask_4) + ((xx >> 4) & mask_4);
	xx = (xx & mask_8) + ((xx >> 8) & mask_8);
	xx = (xx & mask_16) + ((xx >> 16) & mask_16);

	return xx;
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
	/*
	 * 注意到 0 与非 0 数的不同之处为
	 * 0 与 -0 的最高位都为 0，而非 0 数 x 与 -x 中总有一个的最高位为 1
	 * 这是由于非零负数用最高位表示符号位导致的
	 * 故只需判断 x 与 -x 的最高位中是否有 1
	 *
	 * 下面有另一种解法
	 * 具体是通过二分的方法获取到数字的二进制表示中是否含有 1
	 * 含有 1 则输出 0
	 * 否则输出 1
	 */

	return (((x | (~x + 1)) >> 31) & 1) ^ 1;

}
/*
int bang(int x) {
	int a = (x >> 16) | x;
	int b = (a >> 8) | a;
	int c = (b >> 4) | b;
	int d = (c >> 2) | c;
	int e = (d >> 1) | d;

	return ((~e) & 1);
}
*/
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  	/*
	 * 以 2 的补码表示的32位整型的最小值为 -2147483648
	 * 即 0x80000000
	 */
	
	return 0x80 << 24;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  	/*
	 * https://www.zybuluo.com/SovietPower/note/1790502#csapp-lab1
 	 * 有显示该函数可能无法通过
	 * 本地测试确实如此
	 * 报错包括 fitsBits(0, 32) should be 0 等
	 * 
	 * 将数字左移 32 - n 位再右移 32 - n 位，如果数字没有变化，说明该数字用 n 位可以表示
	 * 否则说明该数字用 n 位会发生溢出
	 * 由于位移不能超过 31 位的情况，将位移分成 15 - n 位和 17 位两次
`	 */
	int mov1 = 16 + ~n;
	int mov2 = 17;

	return !(((((x << mov1) << mov2) >> mov1) >> mov2) ^ x);

}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
	/*
	 * 补码除法的公式为 (x < 0 ? x + (1 << k) - 1 : x) >> k
	 * 不能使用 if 语句判断是否为负数
	 * 故通过令非负数的偏置值为 0 的方式来规避判断
	 * 可以利用符号位和偏置值的与运算
	 * 来使得非负数的偏置值为 0
	 */
	int mask = (x >> 31);
	int bias = ((1 << n) + ~1 + 1) & mask;
	int res = (bias + x) >> n;

	return res;
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
	/*
	 * 一个数取非相当于这个数字取反后 + 1
	 */
	
	return ~x+1;
}
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
	/* 
	 * 一个数的正负取决于这个数的最高位
	 * 通过右移 31 位获取数字的符号位
	 * 一个数为正数当且仅当它的符号位不为 1 且它本身不为 0
	 */
	int sign = (x >> 31) & 1;
	
	return !sign & !!x;
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
	/* 
	 * 求 x 是否小于等于 y
	 * 只需求 y - x 是否大于等于 0
	 * 当两数之差超过 TMAX 时会发生溢出
	 * 故需要分两数同号、两数异号两种情况讨论
	 * 当两数同号时作差不会溢出，直接判断差值的符号位
	 * 当两数异号的作差可能溢出，直接判断是否是 x 为负且 y 为正
	 */
	int dif = (y + ~x + 1);
	int sign_of_dif = (dif >> 31) & 1;
	int signx = (x >> 31) & 1;
	int signy = (y >> 31) & 1;	
	int dif_sign = (signx ^ signy);

	return (dif_sign & signx) | (!dif_sign & !sign_of_dif);
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
	/*
	 * 对一个数求 log2 
	 * 相当于求这个数中最高位的 1 的位置
	 * 使用二分折半寻找最高位的 1
	 * 先获取高 16 位的值，如果不为 0 说明高 16 位中有 1
	 * 将结果 + 16，并将高 16 位移位至低 16 位，寻找此时低 16 中位最高位的 1
	 * 如果为 0 说明最高位的 1 在低 16 位中，则不做操作，直接在低 16 位中寻找
	 * 对现在的低 16 位数字，获取高 8 位的值，做同上的操作
	 * 逐次迭代，寻找到最高位的 1 的位置
	 * 即 ilog2 的答案
	 */
	int res = 0;
	res = res + (!!(x >> 16) << 4);
	x = x >> (!!(x >> 16) << 4);
	res = res + (!!(x >> 8) << 3);
	x = x >> (!!(x >> 8) << 3);
	res = res + (!!(x >> 4) << 2);
	x = x >> (!!(x >> 4) << 2);
	res = res + (!!(x >> 2) << 1);
	x = x >> (!!(x >> 2) << 1);
	res = res + (!!(x >> 1) << 0);
	
	return res;
}
/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
	/* 
	 * 浮点数的符号只由最高位直接决定
	 * 分别获取阶码位和尾数位
	 * 如果阶码位为全 1 且尾数为不为 0
	 * 说明时 NaN，直接返回
	 * 否则说明为数值，包括 0, -0, oo, -oo, 和规格化数
	 * 此时只需将最高位的符号位取反，就是结果
	 */
	unsigned res = uf;
	unsigned mask_significand = -1U >> 9;
	unsigned mask_exponent = 0xff << 23;
	unsigned exponent = mask_exponent & uf;
	unsigned significand = mask_significand & uf;
	if(exponent == mask_exponent && significand)
		res = uf;
	else 
		res = uf ^ (1 << 31);

	return res;
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
	/*
	 * 根据浮点数的表示规则
	 * 分别计算符号位、阶码、尾数
	 * 符号位直接获取负号
	 * 
	 * 先计算尾数
	 * 
	 * 尾数通过将原数移位，使其最高位的 1 在第 24 位（从 1 开始计数）
	 * （由于 implied leading 1 的存在，最高位的 1 将会舍弃，剩余的 1 ~ 23 位即为尾数）
	 * 设原数最高位的位置为 k
	 * 当 k 超过 24 时，可能会有舍入的情况存在
	 * 当 k & (-1U >> 56 - k) > (1 << k - 25) 时，属于大于中间数的情况，应当进位，即在结果中 + 1
	 * 当 k & (-1U >> 56 - k) < (1 << k - 25) 时，属于小于中间数的情况，应当直接舍去
	 * 当恰好等于时，属于中间值的情况，应判断被舍入部分的高 1 位，如果是 1 说明舍去后是奇数
	 * 要保证向偶数舍入应进位，即在结果中 + 1
	 * 如果高 1 位是 0 说明舍去后是偶数，直接舍去
	 *
	 * 再计算阶码
	 *
	 * 只需求原数的 k，k - 1 就是阶码的值
	 * 使用 E = k - 1 = e - 127
	 * e = E + 127 = k - 1 + 127 = k + 126 即为阶码位
	 * 
	 * 将各个部分相加得出结果
	 * 当输入为 0 时，直接返回 0 
	 */
	int high_bit = 0;
	unsigned exponent = 0;
	unsigned sign = 0;
	unsigned significand = 0;
	unsigned tmpx = 0;
	unsigned res = 0;
	unsigned mask_roundF = 0;
	unsigned mask_round1 = 0;
	if(x < 0){
		x = -x;
		sign = 1 << 31;
	}
	tmpx = x;
	while(tmpx){
		high_bit++;
		tmpx = tmpx >> 1;
	}
	mask_roundF = -1U >> (55 - high_bit);
	mask_round1 = 1 << (high_bit - 25);
	if(high_bit >= 25){
		if((x & (mask_roundF >> 1)) > (mask_round1))
			res = 1;
		else if((x & mask_roundF) == ((mask_round1 << 1) + mask_round1))
			res = 1;
	}
	significand = ((x << (32 - high_bit)) >> 8);
	significand = significand & (-1U >> 9);
	exponent = (high_bit + 126) << 23;
	res = res + exponent + sign + significand;
	if(x == 0)
		res = 0;

	return res;
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
	/*
	 * 如果 uf 是 +0 或 -0，那么 2 * uf 就是它本身
	 * 如果阶码为 0xff，则它是特殊值，同样返回本身
	 * 如果阶码为 0，则它是非规格化数，阶码值固定为 1 - Bias
	 * 而尾数不足 1，将其乘 2 只需将尾数乘 2
	 * 即将尾数左移 1 位
	 * 否则
	 * 由于规格化浮点数通过 (1 + frac) * 2 ^ k 来表示
	 * 将浮点数乘 2
	 * 只需将 k 值 + 1
	 * 即将阶码值 + 1 后返回
	 */
	unsigned res = 0;
	unsigned sign = uf & (1 << 31);
	unsigned exp = (uf >> 23) & 0xff;
	if(uf == 0 || uf == (1 << 31) || exp == 0xff)
		res = uf;
	else if(exp == 0)
		res = (uf << 1) | sign;
	else
		res = uf + (1 << 23);

	return res;
}
