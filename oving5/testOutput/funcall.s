.data
.INTEGER: .string "%d "
.NEWLINE: .string "\n"
.STRING0: .string "Calling my_function with parameters"
.STRING1: .string "The returned result is"
.STRING2: .string "The other returned result is"
.STRING3: .string "Parameter s is"
.STRING4: .string "Parameter t is"
.STRING5: .string "The sum of their squares is"
.globl main
.text
_funcall:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$0
	pushl	$0
	pushl	$5
	movl	%ebp,%eax
	popl	-4(%eax)
	pushl	$10
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
	movl	%ebp,%eax
	pushl	-8(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	movl	(%eax),%eax
	movl	(%eax),%eax
	pushl	%eax
	movl	%ebp,%eax
	pushl	-4(%eax)
	movl	%ebp,%eax
	pushl	-8(%eax)
	popl	%ebx
	popl	%eax
	pushl	%eax
	movl	%ebp,%eax
	popl	-12(%eax)
	pushl	$.STRING1
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
	movl	%ebp,%eax
	movl	(%eax),%eax
	movl	(%eax),%eax
	pushl	%eax
	popl	%ebx
	popl	%eax
	pushl	%eax
	movl	%ebp,%eax
	popl	-12(%eax)
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
_my_function:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	12(%eax)
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	12(%eax)
	popl	%ebx
	popl	%eax
	imull	%ebx
	pushl	%eax
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	8(%eax)
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	8(%eax)
	popl	%ebx
	popl	%eax
	imull	%ebx
	pushl	%eax
	popl	%ebx
	popl	%eax
	addl	%ebx,%eax
	pushl	%eax
	movl	%ebp,%eax
	popl	-4(%eax)
	pushl	$.STRING3
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	movl	(%eax),%eax
	pushl	12(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	pushl	$.STRING4
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
	pushl	$.STRING5
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
_my_other_function:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$42
	movl	%ebp,%eax
	popl	-4(%eax)
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
	call	_funcall
	leave
	pushl	%eax
	call	exit
