.data
.INTEGER: .string "%d "
.NEWLINE: .string "\n"
.STRING0: .string "Outer x is"
.STRING1: .string "y is"
.STRING2: .string "parm is"
.STRING3: .string "Inner x is"
.STRING4: .string "y is"
.STRING5: .string "parm is"
.STRING6: .string "Outer x is"
.STRING7: .string "y is"
.STRING8: .string "parm is"
.globl main
.text
_hello:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	movl	%ebp,%eax
	movl	(%eax),%eax
	movl	(%eax),%eax
	pushl	%eax
	pushl	$42
	popl	%ebx
	popl	%eax
	pushl	%eax
	movl	%ebp,%eax
	popl	-4(%eax)
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
	movl	(%eax),%eax
	pushl	8(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$64
	movl	%ebp,%eax
	popl	-4(%eax)
	pushl	$.STRING3
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	pushl	-4(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING4
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	-8(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING5
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	movl	(%eax),%eax
	movl	(%eax),%eax
	pushl	8(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	leave
	pushl	$.STRING6
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	pushl	-4(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING7
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	pushl	-8(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING8
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	8(%eax)
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
	call	_hello
	leave
	pushl	%eax
	call	exit
