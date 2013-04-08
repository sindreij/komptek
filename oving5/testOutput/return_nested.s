.data
.INTEGER: .string "%d "
.NEWLINE: .string "\n"
.STRING0: .string "t is"
.STRING1: .string "This never executes"
.globl main
.text
_hello:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$.STRING0
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	movl	(%eax),%eax
	movl	(%eax),%eax
	pushl	%eax
	pushl	$64
	popl	%ebx
	popl	%eax
	pushl	%eax
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	leave
	leave
	ret
_test:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$0
	pushl	$32
	movl	%ebp,%eax
	popl	-4(%eax)
	pushl	$20
	movl	%ebp,%eax
	popl	-8(%eax)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$64
	movl	%ebp,%eax
	popl	-4(%eax)
	pushl	$.STRING1
	call	printf
	addl	$4,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	leave
	leave
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
	call	_hello
	leave
	pushl	%eax
	call	exit
