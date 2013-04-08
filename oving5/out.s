.data
.INTEGER: .string "%d "
.NEWLINE: .string "\n"
.STRING0: .string "Hello, world!"
.STRING1: .string "Hello, world!"
.STRING2: .string "Svaret er: "
.STRING3: .string "Hello from myfunc"
.globl main
.text
_hello:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$0
	pushl	$0
	pushl	$170
	movl	%ebp,%eax
	popl	-4(%eax)
	pushl	$10
	movl	%ebp,%eax
	popl	-8(%eax)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$150
	movl	%ebp,%eax
	popl	-4(%eax)
	pushl	$.STRING0
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	pushl	-4(%eax)
	movl	%ebp,%eax
	movl	(%eax),%eax
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
	pushl	$.STRING1
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
	pushl	$1
	movl	%ebp,%eax
	popl	-4(%eax)
	pushl	$2
	movl	%ebp,%eax
	popl	-8(%eax)
	pushl	$3
	movl	%ebp,%eax
	popl	-12(%eax)
	pushl	$.STRING2
	call	printf
	addl	$4,%esp
	movl	%ebp,%eax
	pushl	-4(%eax)
	movl	%ebp,%eax
	pushl	-8(%eax)
	popl	%ebx
	popl	%eax
	addl	%ebx,%eax
	pushl	%eax
	movl	%ebp,%eax
	pushl	-12(%eax)
	popl	%ebx
	popl	%eax
	imull	%ebx
	pushl	%eax
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
	pushl	$12
	popl	%ebx
	popl	%eax
	call	_myfunc
	pushl	%eax
	movl	%ebp,%eax
	popl	-4(%eax)
	pushl	$0
	popl	%eax
	leave
	leave
	ret
_myfunc:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$.STRING3
	call	printf
	addl	$4,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	pushl	$0
	popl	%eax
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
