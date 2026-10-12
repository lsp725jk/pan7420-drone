	.cpu cortex-m0plus
	.arch armv6s-m
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"ub_arm_probe.cpp"
	.text
	.align	1
	.p2align 2,,3
	.global	_Z16deref_then_checkPi
	.syntax unified
	.code	16
	.thumb_func
	.type	_Z16deref_then_checkPi, %function
_Z16deref_then_checkPi:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0]
	@ sp needed
	bx	lr
	.size	_Z16deref_then_checkPi, .-_Z16deref_then_checkPi
	.align	1
	.p2align 2,,3
	.global	_Z9add_wrapsi
	.syntax unified
	.code	16
	.thumb_func
	.type	_Z9add_wrapsi, %function
_Z9add_wrapsi:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #1
	@ sp needed
	bx	lr
	.size	_Z9add_wrapsi, .-_Z9add_wrapsi
	.align	1
	.p2align 2,,3
	.global	_Z13loop_overflowi
	.syntax unified
	.code	16
	.thumb_func
	.type	_Z13loop_overflowi, %function
_Z13loop_overflowi:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	asrs	r3, r0, #31
	subs	r0, r3, r0
	asrs	r0, r0, #31
	@ sp needed
	bx	lr
	.size	_Z13loop_overflowi, .-_Z13loop_overflowi
	.align	1
	.p2align 2,,3
	.global	_Z15const_index_oobv
	.syntax unified
	.code	16
	.thumb_func
	.type	_Z15const_index_oobv, %function
_Z15const_index_oobv:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	sub	sp, sp, #16
	ldr	r0, [sp, #16]
	add	sp, sp, #16
	@ sp needed
	bx	lr
	.size	_Z15const_index_oobv, .-_Z15const_index_oobv
	.align	1
	.p2align 2,,3
	.global	_Z11uninit_readi
	.syntax unified
	.code	16
	.thumb_func
	.type	_Z11uninit_readi, %function
_Z11uninit_readi:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #1
	@ sp needed
	bx	lr
	.size	_Z11uninit_readi, .-_Z11uninit_readi
	.ident	"GCC: (Arm GNU Toolchain 14.2.Rel1 (Build arm-14.52)) 14.2.1 20241119"
