	.cpu cortex-m0plus
	.arch armv6s-m
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"ub_arm_probe.cpp"
	.text
	.align	1
	.global	_Z16deref_then_checkPi
	.syntax unified
	.code	16
	.thumb_func
	.type	_Z16deref_then_checkPi, %function
_Z16deref_then_checkPi:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #16
	add	r7, sp, #0
	str	r0, [r7, #4]
	ldr	r3, [r7, #4]
	ldr	r3, [r3]
	str	r3, [r7, #12]
	ldr	r3, [r7, #4]
	cmp	r3, #0
	bne	.L2
	movs	r3, #1
	rsbs	r3, r3, #0
	b	.L3
.L2:
	ldr	r3, [r7, #12]
.L3:
	movs	r0, r3
	mov	sp, r7
	add	sp, sp, #16
	@ sp needed
	pop	{r7, pc}
	.size	_Z16deref_then_checkPi, .-_Z16deref_then_checkPi
	.align	1
	.global	_Z9add_wrapsi
	.syntax unified
	.code	16
	.thumb_func
	.type	_Z9add_wrapsi, %function
_Z9add_wrapsi:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #8
	add	r7, sp, #0
	str	r0, [r7, #4]
	movs	r3, #1
	movs	r0, r3
	mov	sp, r7
	add	sp, sp, #8
	@ sp needed
	pop	{r7, pc}
	.size	_Z9add_wrapsi, .-_Z9add_wrapsi
	.align	1
	.global	_Z13loop_overflowi
	.syntax unified
	.code	16
	.thumb_func
	.type	_Z13loop_overflowi, %function
_Z13loop_overflowi:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #16
	add	r7, sp, #0
	str	r0, [r7, #4]
	movs	r3, #0
	str	r3, [r7, #12]
	ldr	r3, [r7, #4]
	str	r3, [r7, #8]
	b	.L7
.L10:
	ldr	r3, [r7, #12]
	adds	r3, r3, #1
	str	r3, [r7, #12]
	ldr	r2, [r7, #12]
	movs	r3, #250
	lsls	r3, r3, #2
	cmp	r2, r3
	ble	.L8
	movs	r3, #1
	rsbs	r3, r3, #0
	b	.L9
.L8:
	ldr	r3, [r7, #8]
	adds	r3, r3, #100
	str	r3, [r7, #8]
.L7:
	ldr	r3, [r7, #8]
	cmp	r3, #0
	bgt	.L10
	ldr	r3, [r7, #12]
.L9:
	movs	r0, r3
	mov	sp, r7
	add	sp, sp, #16
	@ sp needed
	pop	{r7, pc}
	.size	_Z13loop_overflowi, .-_Z13loop_overflowi
	.section	.rodata
	.align	2
.LC0:
	.word	1
	.word	2
	.word	3
	.word	4
	.text
	.align	1
	.global	_Z15const_index_oobv
	.syntax unified
	.code	16
	.thumb_func
	.type	_Z15const_index_oobv, %function
_Z15const_index_oobv:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r4, r7, lr}
	sub	sp, sp, #20
	add	r7, sp, #0
	movs	r3, r7
	ldr	r2, .L13
	ldmia	r2!, {r0, r1, r4}
	stmia	r3!, {r0, r1, r4}
	ldr	r2, [r2]
	str	r2, [r3]
	movs	r3, r7
	ldr	r3, [r3, #16]
	movs	r0, r3
	mov	sp, r7
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r7, pc}
.L14:
	.align	2
.L13:
	.word	.LC0
	.size	_Z15const_index_oobv, .-_Z15const_index_oobv
	.align	1
	.global	_Z11uninit_readi
	.syntax unified
	.code	16
	.thumb_func
	.type	_Z11uninit_readi, %function
_Z11uninit_readi:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #16
	add	r7, sp, #0
	str	r0, [r7, #4]
	ldr	r3, [r7, #4]
	cmp	r3, #0
	beq	.L16
	movs	r3, #1
	str	r3, [r7, #12]
.L16:
	ldr	r3, [r7, #12]
	movs	r0, r3
	mov	sp, r7
	add	sp, sp, #16
	@ sp needed
	pop	{r7, pc}
	.size	_Z11uninit_readi, .-_Z11uninit_readi
	.ident	"GCC: (Arm GNU Toolchain 14.2.Rel1 (Build arm-14.52)) 14.2.1 20241119"
