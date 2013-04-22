.data
.INTEGER: .string "%d "
.NEWLINE: .string "\n"
.STRING0: .string "WORKING"
.globl main
.text
_test:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$.STRING0
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	pushl	8(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	leave
	ret
main:
	pushl	%ebp
	movl	%esp,%ebp
	movl	8(%esp),%esi
	decl	%esi
	jz	noargs
	movl	12(%ebp),%ebx
pusharg:
	addl	$4,%ebx
	pushl	$10
	pushl	$0
	pushl	(%ebx)
	call	strtol
	addl	$12,%esp
	pushl	%eax
	decl	%esi
	jnz	pusharg
noargs:
	call	_test
	leave
	pushl	%eax
	call	exit
