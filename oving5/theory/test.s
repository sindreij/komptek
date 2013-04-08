.data 
INTEGER: .string "%d + %d + 2 + 4 + 2 is %d\n"

.text
.globl main

_compute:
pushl %ebp
movl %esp, %ebp
movl 8(%ebp), %eax
movl 12(%ebp), %ebx
addl %ebx, %eax
addl $2, %eax
leave
ret

main:
pushl $5
pushl $6
call _compute
addl $8, %esp

pushl %eax
pushl $4
call _compute
addl $8, %esp

pushl %eax
pushl $5
pushl $6
pushl $INTEGER
call printf
addl $16, %esp
pushl $0
call exit
