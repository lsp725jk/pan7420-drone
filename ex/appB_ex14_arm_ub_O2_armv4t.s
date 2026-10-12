	.cpu arm7tdmi
	.arch armv4t
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
	.align	2
	.global	_Z16deref_then_checkPi
	.syntax unified
	.arm
	.type	_Z16deref_then_checkPi, %function
_Z16deref_then_checkPi:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0]
	bx	lr
	.size	_Z16deref_then_checkPi, .-_Z16deref_then_checkPi
	.align	2
	.global	_Z9add_wrapsi
	.syntax unified
	.arm
	.type	_Z9add_wrapsi, %function
_Z9add_wrapsi:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r0, #1
	bx	lr
	.size	_Z9add_wrapsi, .-_Z9add_wrapsi
	.align	2
	.global	_Z13loop_overflowi
	.syntax unified
	.arm
	.type	_Z13loop_overflowi, %function
_Z13loop_overflowi:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	cmp	r0, #0
	movle	r0, #0
	movgt	r0, #1
	rsb	r0, r0, #0
	bx	lr
	.size	_Z13loop_overflowi, .-_Z13loop_overflowi
	.align	2
	.global	_Z15const_index_oobv
	.syntax unified
	.arm
	.type	_Z15const_index_oobv, %function
_Z15const_index_oobv:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	sub	sp, sp, #16
	ldr	r0, [sp, #16]
	add	sp, sp, #16
	@ sp needed
	bx	lr
	.size	_Z15const_index_oobv, .-_Z15const_index_oobv
	.align	2
	.global	_Z11uninit_readi
	.syntax unified
	.arm
	.type	_Z11uninit_readi, %function
_Z11uninit_readi:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r0, #1
	bx	lr
	.size	_Z11uninit_readi, .-_Z11uninit_readi
	.ident	"GCC: (Arm GNU Toolchain 14.2.Rel1 (Build arm-14.52)) 14.2.1 20241119"
