.data
INTEGER: .string "Result: %d\n"

.text
.globl main

main:
    pushl $5
    call _addFive
    addl $4, %esp

    pushl %eax
    pushl $INTEGER
    call printf
    addl $8, %esp

    pushl $0
    call exit



#innleveringen

_addFive:
    pushl %ebp
    movl %esp, %ebp

    movl 8(%ebp), %eax

    pushl %eax
    call _addFour
    addl $4, %esp

    addl $1, %eax
    leave
    ret

_addFour:
    pushl %ebp
    movl %esp, %ebp
    
    movl 8(%ebp), %eax

    addl $4, %eax
    leave
    ret
