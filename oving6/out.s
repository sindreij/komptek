.data
.INTEGER: .string "%d "
.NEWLINE: .string "\n"
.STRING0: .string " > "
.STRING1: .string " < "
.STRING2: .string " >= "
.STRING3: .string " <= "
.STRING4: .string " == "
.STRING5: .string "!= "
.STRING6: .string "STATIC, 4,4"
.STRING7: .string " > "
.STRING8: .string " < "
.STRING9: .string " >= "
.STRING10: .string " <= "
.STRING11: .string " == "
.STRING12: .string "!= "
.globl main
.text
_cornercases:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebp
	movl	%esp,%ebp
	pushl	$0
	pushl	$0
	pushl	$5
	movl	%ebp,%eax
	popl	-4(%eax)
	pushl	$6
	movl	%ebp,%eax
	popl	-8(%eax)
	movl	%ebp,%eax
	pushl	-4(%eax)
	movl	%ebp,%eax
	pushl	-8(%eax)
	popl	%ebx
	popl	%eax
	cmpl	%ebx,%eax
	setg	%al
	cbw
	cwde
	pushl	%eax
	popl	%eax
	cmpl	$0,%eax
	je	.L0
	movl	%ebp,%eax
	pushl	-4(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING0
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
.L0:
	movl	%ebp,%eax
	pushl	-4(%eax)
	movl	%ebp,%eax
	pushl	-8(%eax)
	popl	%ebx
	popl	%eax
	cmpl	%ebx,%eax
	setl	%al
	cbw
	cwde
	pushl	%eax
	popl	%eax
	cmpl	$0,%eax
	je	.L1
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
.L1:
	movl	%ebp,%eax
	pushl	-4(%eax)
	movl	%ebp,%eax
	pushl	-8(%eax)
	popl	%ebx
	popl	%eax
	cmpl	%ebx,%eax
	setge	%al
	cbw
	cwde
	pushl	%eax
	popl	%eax
	cmpl	$0,%eax
	je	.L2
	movl	%ebp,%eax
	pushl	-4(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING2
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
.L2:
	movl	%ebp,%eax
	pushl	-4(%eax)
	movl	%ebp,%eax
	pushl	-8(%eax)
	popl	%ebx
	popl	%eax
	cmpl	%ebx,%eax
	setle	%al
	cbw
	cwde
	pushl	%eax
	popl	%eax
	cmpl	$0,%eax
	je	.L3
	movl	%ebp,%eax
	pushl	-4(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING3
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
.L3:
	movl	%ebp,%eax
	pushl	-4(%eax)
	movl	%ebp,%eax
	pushl	-8(%eax)
	popl	%ebx
	popl	%eax
	cmpl	%ebx,%eax
	sete	%al
	cbw
	cwde
	pushl	%eax
	popl	%eax
	cmpl	$0,%eax
	je	.L4
	movl	%ebp,%eax
	pushl	-4(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING4
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
.L4:
	movl	%ebp,%eax
	pushl	-4(%eax)
	movl	%ebp,%eax
	pushl	-8(%eax)
	popl	%ebx
	popl	%eax
	cmpl	%ebx,%eax
	setne	%al
	cbw
	cwde
	pushl	%eax
	popl	%eax
	cmpl	$0,%eax
	je	.L5
	movl	%ebp,%eax
	pushl	-4(%eax)
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING5
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
.L5:
	pushl	$.STRING6
	call	printf
	addl	$4,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
	pushl	$4
	pushl	$4
	popl	%ebx
	popl	%eax
	cmpl	%ebx,%eax
	setg	%al
	cbw
	cwde
	pushl	%eax
	popl	%eax
	cmpl	$0,%eax
	je	.L6
	pushl	$4
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING7
	call	printf
	addl	$4,%esp
	pushl	$4
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
.L6:
	pushl	$4
	pushl	$4
	popl	%ebx
	popl	%eax
	cmpl	%ebx,%eax
	setl	%al
	cbw
	cwde
	pushl	%eax
	popl	%eax
	cmpl	$0,%eax
	je	.L7
	pushl	$4
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING8
	call	printf
	addl	$4,%esp
	pushl	$4
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
.L7:
	pushl	$4
	pushl	$4
	popl	%ebx
	popl	%eax
	cmpl	%ebx,%eax
	setge	%al
	cbw
	cwde
	pushl	%eax
	popl	%eax
	cmpl	$0,%eax
	je	.L8
	pushl	$4
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING9
	call	printf
	addl	$4,%esp
	pushl	$4
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
.L8:
	pushl	$4
	pushl	$4
	popl	%ebx
	popl	%eax
	cmpl	%ebx,%eax
	setle	%al
	cbw
	cwde
	pushl	%eax
	popl	%eax
	cmpl	$0,%eax
	je	.L9
	pushl	$4
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING10
	call	printf
	addl	$4,%esp
	pushl	$4
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
.L9:
	pushl	$4
	pushl	$4
	popl	%ebx
	popl	%eax
	cmpl	%ebx,%eax
	sete	%al
	cbw
	cwde
	pushl	%eax
	popl	%eax
	cmpl	$0,%eax
	je	.L10
	pushl	$4
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING11
	call	printf
	addl	$4,%esp
	pushl	$4
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
.L10:
	pushl	$4
	pushl	$4
	popl	%ebx
	popl	%eax
	cmpl	%ebx,%eax
	setne	%al
	cbw
	cwde
	pushl	%eax
	popl	%eax
	cmpl	$0,%eax
	je	.L11
	pushl	$4
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.STRING12
	call	printf
	addl	$4,%esp
	pushl	$4
	pushl	$.INTEGER
	call	printf
	addl	$8,%esp
	pushl	$.NEWLINE
	call	printf
	addl	$4,%esp
.L11:
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
	call	_cornercases
	leave
	pushl	%eax
	call	exit
