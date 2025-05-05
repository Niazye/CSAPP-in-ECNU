# Homework Week12

## 7.6

|符号|`swap.o.symtab`条目?|符号类型|定义符号的模块|节|
|---|---|---|---|---|
|`buf`|是|外部|`m.o`|`.data`|
|`bufp0`|是|全局|`swap.o`|`.data`|
|`bufp1`|是|局部|`swap.o`|`.bss`|
|`swap`|是|全局|`swap.o`|`.text`|
|`temp`|否|N/A|N/A|N/A|
|`incr`|是|局部|`swap.o`|`.text`|
|`count`|是|局部|`swap.o`|`.bss`|

## 7.10

A. `gcc p.o libx.a`
B. `gcc p.o libx.a liby.a libx.a`
C. `gcc p.o libx.a liby.a libx.a libz.a`

## 7.12

A.

```c
refaddr = ADDR(.text) + r.offset 
        = 0x4004e0 + 0xa
        = 0x4004ea
*refptr = ADDR(swap) + r.addend - refaddr
        = 0x4004f8 + (-4) - 0x4004ea
        = 0xa
```

故为 `0xa`
B.

```c
refaddr = ADDR(.text) + r.offset
        = 0x4004d0 + 0xa
        = 0x4004da
*refptr = ADDR(swap) + r.addend - redaddr
        = 0x400500 + (-4) - 0x4004da
        = 0x22
```

故为 `0x22`
