.data
.INTEGER: .string "%d "
.NEWLINE: .string "\n"
.STRING0: .string "Nested scopes coming up..."
.STRING1: .string "x="
.STRING2: .string "Parameter a is a="
.STRING3: .string "Outer scope has a="
.STRING4: .string "Inner scope has a="
.STRING5: .string "and b="
.STRING6: .string "b was updated to "
.STRING7: .string "in inner scope"
.STRING8: .string "Outer scope (still) has a="
.STRING9: .string "Return expression (a-1) using a="
.globl main
.text
_start:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$.STRING0
	call	printf
	addl	$4,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	movl	(%eax),%eax
	movl	(%eax),%eax
	pushl	%eax
	pushl	$1
	popl	%ebx
	popl	%eax
	pushl	%eax
	movl	%ebp,%eax
	popl	-4(%eax)
	pushl	$.STRING1
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	pushl	-4(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	leave
	leave
	ret
_test_me:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
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
	pushl	$2
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
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$0
	pushl	$3
	movl	%ebp,%eax
	popl	-8(%eax)
	pushl	$4
	movl	%ebp,%eax
	popl	-4(%eax)
	pushl	%ebp
	movl	%esp,%ebp
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
	pushl	-4(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	pushl	$5
	movl	%ebp,%eax
	movl	(%eax),%eax
	popl	-4(%eax)
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
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	leave
	pushl	$.STRING8
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	pushl	-4(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	leave
	pushl	$.STRING9
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
	call	_start
	leave
	pushl	%eax
	call	exit
