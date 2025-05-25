
injection_asm.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <.text>:
   0:	48 c7 44 24 20 35 39 	movq   $0x39623935,0x20(%rsp)
   7:	62 39 
   9:	48 c7 44 24 24 39 37 	movq   $0x61663739,0x24(%rsp)
  10:	66 61 
  12:	48 8d 7c 24 20       	lea    0x20(%rsp),%rdi
  17:	c3                   	ret
