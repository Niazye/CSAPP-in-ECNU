/* 3.58 */
long decode2(long x, long y, long z)
{
    long res = 0;
    y -= z;
    x *= y;
    res = y;
    res <<= 63;
    res >>= 63;
    res ^= x;
    return res;
    /*
     * Or
     * y = y - z;
     * return ((y << 63) >> 63) ^ (x * y)
     */
}

/* 3.60 */
long loop(long x, int n)
{

    	/*
	 * A. x in %rdi, n in %esi and %ecx , result in %rax, mask in %rdx
     	 * B. mask 初始值为 1, result 为 0, 因为汇编代码中第 3 行 movl $1, %edx, 第 4 行 movl $0, %eax
     	 * C. 汇编码中第 12, 13 行, 当第 12 行测试结果不为 0 时, 第 13 行 jmp 至循环体, 即 mask 测试条件为 mask != 0
     	 * D. 汇编码中第 10 行, 令 %rdx, 即 mask 左移 %cl 位, 而 %cl 是 %ecx 的低 8 位, 考虑到位移运算会对位移量做 % 32 处理, 可直接表示为 mask <<= n
     	 * E. 汇编码中第 7, 8, 9 行, 令 res 与 %r8 做或运算, 而 %r8 是 %rdi 和 %rdx 的与运算, 即 res |= (x & mask)
     	 * F. 代码如下
     	 */
	long result = 0;
	long mask;
	for (mask = 1; mask != 0; mask <= n) {
		result |= (mask & x);
	}
	return result;
}

/* 3.63 */
long switch_prob(long x, long n) {
	long result = x;
	switch(n) {
		case 60:
		case 62:
			result = x * 8;
			break;
		case 63:
			result = x;
			result >>= 3;
			/* 
			 * Or
			 * result = x >> 3;
			 */
			break;
		case 64:
			result = x;
			result <<= 4;
			result -= x;
			x = result;
			/*
			 * Or
			 * x = (x << 4) - x; 
			 * Or
			 * x = x * 15;
			 */
		case 65:
			x *= x;
		default:
			result = x + 75; // 75 == 0x4b
			break;
	}
	return result;
}
