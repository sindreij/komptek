.data
.INTEGER: .string "%d "
.NEWLINE: .string "\n"
.STRING0: .string "a is"
.STRING1: .string "and b is"
.STRING2: .string "a/(-b) is"
.globl main
.text
_negatives:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$0
	pushl	$100
	movl	%ebp,%eax
	popl	-4(%eax)
	pushl	$20
	movl	%ebp,%eax
	popl	-8(%eax)
	pushl	$.STRING0
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	pushl	-4(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING1
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	pushl	-8(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	pushl	$.STRING2
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	pushl	-4(%eax)
	movl	%ebp,%eax
	pushl	-8(%eax)
	popl	%ebx
	popl	%eax
	cdq
	idivl	%ebx
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
	call	_negatives
	leave
	pushl	%eax
	call	exit
