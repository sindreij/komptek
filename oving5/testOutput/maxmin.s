.data
.INTEGER: .string "%d "
.NEWLINE: .string "\n"
.STRING0: .string "a is: "
.STRING1: .string "b is: "
.STRING2: .string "c is: "
.globl main
.text
_maxmin:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$0
	pushl	$0
	pushl	$0
	pushl	$0
	movl	%ebp,%eax
	movl	(%eax),%eax
	movl	(%eax),%eax
	pushl	%eax
	pushl	$5
	pushl	$6
	popl	%ebx
	popl	%eax
	pushl	%eax
	movl	%ebp,%eax
	popl	-4(%eax)
	movl	%ebp,%eax
	movl	(%eax),%eax
	movl	(%eax),%eax
	pushl	%eax
	pushl	$-1
	pushl	$4
	popl	%ebx
	popl	%eax
	pushl	%eax
	movl	%ebp,%eax
	popl	-8(%eax)
	movl	%ebp,%eax
	movl	(%eax),%eax
	movl	(%eax),%eax
	pushl	%eax
	pushl	$0
	pushl	$0
	popl	%ebx
	popl	%eax
	pushl	%eax
	movl	%ebp,%eax
	popl	-12(%eax)
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
	pushl	$.STRING2
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	pushl	-12(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	leave
	leave
	ret
_max:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$0
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	12(%eax)
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	8(%eax)
	popl	%ebx
	popl	%eax
	pushl	%eax
	movl	%ebp,%eax
	popl	-4(%eax)
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	8(%eax)
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	12(%eax)
	popl	%ebx
	popl	%eax
	pushl	%eax
	movl	%ebp,%eax
	popl	-8(%eax)
	leave
	leave
	ret
_min:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$0
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	12(%eax)
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	8(%eax)
	popl	%ebx
	popl	%eax
	pushl	%eax
	movl	%ebp,%eax
	popl	-4(%eax)
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	8(%eax)
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	12(%eax)
	popl	%ebx
	popl	%eax
	pushl	%eax
	movl	%ebp,%eax
	popl	-8(%eax)
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
	call	_maxmin
	leave
	pushl	%eax
	call	exit
