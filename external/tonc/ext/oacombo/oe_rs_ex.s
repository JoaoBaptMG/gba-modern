	.file	"oe_rs_ex.s"

	.text
	.align	2
	.global	obj_rs_ex2
	.code	16
	.thumb_func
	.type obj_rs_ex2 STT_FUNC
/*
	r0: OBJ_ATTR *obj
	r1: OBJ_AFFINE *oaff
	r2: AFF_SRC_EX *asx { s32 px, py; s16 qx, qy; s16 sx, sy; u16 alpha }
	118 ins, vs 138 for GCC (but that also calls oe_set_pos)
*/
obj_rs_ex2:
	push	{r0,r2,r4-r7,lr}	@ r0,r2 are trashed by DivSafe
@ -> r4: cos(alpha) ; r5: sin(alpha)
	ldr		r3, =sin_lut
	ldrh	r5, [r2, #16]	@ alpha
	lsl		r5, #16
	mov		r4, #4
	lsl		r4, #28
	add		r4, r5
	lsr		r4, r4, #22		@ ((alpha+pi/4)&511) *2
	ldrsh	r4, [r3, r4]	@ cos(alpha)	.12f
	lsr		r5, r5, #22		@ ((alpha     )&511) *2
	ldrsh	r5, [r3, r5]	@ sin(alpha)	.12f
@ --- create P ---
@ r1=oa, r6: sx, r7: sy, r3: var
	mov		r6, #12
	ldrsh	r6, [r2, r6]
	mov		r7, #14
	ldrsh	r7, [r2, r7]
	@ PONDER: shortcut for sx == sy ?
	@ oa->pa = sx*cosa>>12
	mov		r3, r6
	mul		r3, r4
	asr		r3, r3, #12
	strh	r3, [r1, #6]	
	@ oa->pb = -sx*sina>>12
	neg		r3, r6
	mul		r3, r5
	asr		r3, r3, #12
	strh	r3, [r1, #14]	
	@ oa->pc = sy*sina>>12
	mov		r3, r7
	mul		r3, r5
	asr		r3, r3, #12
	strh	r3, [r1, #22]	
	@ oa->pd = sy*cosa>>12
	mov		r3, r7
	mul		r3, r4
	asr		r3, r3, #12
	strh	r3, [r1, #30]
@ --- create A= P^-1 ---
@ r6: 1/sx ; r7: 1/sy
@ TODO: use swi directly, but check for div/0
	@ wx = (1<<20)/sx (-> .12f)
	mov		r0, #1
	lsl		r0, #20
	mov		r1, r6
	bl		DivSafe
	@ wy= (sx==sy) ? wx : (1<<20)/sy;
	cmp		r7, r6
	beq		.Lsx_is_sy
		mov		r6, r0		@ finalize 1/sx
		mov		r0, #1
		lsl		r0, #20
		mov		r1, r7
		bl		DivSafe
		b		.Lrecip_done	
.Lsx_is_sy:
		mov		r6, r0	@ finalize 1/sx if sx == sy
.Lrecip_done:
	mov		r7, r0		@ finalize 1/sy
@ calculate elements of A
@ [] r0-r3: free; r4,r5: cosa, sina; r6,r0&r7: wx,wy
@ -> r4,r5,r6,r7: ab,aa,ac,ad
	@ r7: ad= wy*cosa>>16
	mul		r7, r4
	asr		r7, r7, #16
	@ r4: aa= cosa*wx>>16
	mul		r4, r6
	asr		r4, r4, #16
	@ r6: ac= -wx*sina>>16
	neg		r3, r5
	mul		r6, r3
	asr		r6, r6, #16
	@ r5: ab= sina*wy>>16
	mul		r5, r0
	asr		r5, r5, #16
@ --- Calculate final offsets ---
@ Get obj size via oam_sizes[3][4][2];
	pop		{r0,r1}		@ r0:oe ; r1:asx (PONDER: o rly? check order)
	@ 2 free regs left O_O
	ldrh	r2, [r0]
	ldrh	r3, [r0, #2]
	lsr		r2, r2, #14
	lsl		r2, r2, #2
	lsr		r3, r3, #14
	add		r2, r2, r3
	lsl		r2, #1
	ldr		r3, =oam_sizes
	add		r3, r2, r3		@ &oam_sizes[shape][size]
	ldrb	r2, [r3]
	lsr		r2, r2, #1		@ sx/2
	ldrb	r3, [r3,#1]
	lsr		r3, r3, #1		@ sy/2
	@ well, that's all regs .. oh wait, I still have ip and lr ^_^
	mov		ip, r2
	mov		lr, r3
	ldr		r3, [r1]	@ px
	lsl		r2, r2, #8
	sub		r3, r3, r2	@ r3: rpx= px - (sx/2<<8)
	mul		r4, r3		@ r4= aa*rpx
	mul		r6, r3		@ r6= ac*rpx
	ldr		r3, [r1,#4]	@ py
	mov		r2, lr
	lsl		r2, r2, #8
	sub		r3, r3, r2	@ r3: rpy= py - (sy/2<<8)
	mul		r5, r3		@ r5= ab*rpy
	mul		r7, r3		@ r7= ad*rpy
	add		r4, r4, r5
	asr		r4, r4, #16	@ rqx (.0f)
	add		r5, r6, r7
	asr		r5, r5, #16	@ rqy (.0f)
	@ r2, r3, r6, r7 free
	add		r4, ip
	add		r5, lr
	@ and another half-size if doublesize is set
	ldr		r6, [r0]		@ oe->attr0 (and attr1)
	lsl		r7, r6, #23	@ bit 9 into carry
	bcc		.Lno_dbl
		add		r4, ip
		add		r5, lr
	.Lno_dbl:
	ldrh	r2, [r1, #8]	@ qx
	sub		r4, r2, r4		@ dx= qx - (m*w + rpx)
	ldrh	r2, [r1, #10]	@ qy
	sub		r5, r2, r5		@ dy= qy - (m*h + rpy)
	@ oe_set_pos; r0: oe; r4,r5: x,y; r6: (attr1<<16)|attr0
	ldr		r3, =0x01FF0000	@ X_MASK<<16
	lsl		r4, r4, #16
	and		r4, r3
	lsr		r2, r3, #17		@ Y_MASK
	and		r5, r2			@ 
	orr		r4, r5			@ r4: (x&X_MASK)<<16 | (y&Y_MASK)
	orr		r3, r2			@ X_MASK<<16 | Y_MASK
	bic		r6, r3			@ attr0,attr1 bit-cleared
	orr		r6, r4			@ insert x,y -> a0,a1
	str		r6, [r0]
	pop		{r4-r7}
	pop		{r0}
	bx		r0
	.align	2
	.pool
/*
@ === original ===
	push	{r4, r5, r6, r7, lr}
	mov	r7, sl
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r6, r2	@ asx, asx
	ldrh	r2, [r2, #16]	@ alpha,
	ldr	r4, .L50	@ tmp127,
	mov	r3, r2	@ tmp126, alpha
	add	r3, r3, #128	@ tmp126,
	ldr	r5, .L50+4	@ tmp125, _sinLUT
	and	r3, r3, r4	@ tmp128, tmp127
	lsl	r3, r3, #1	@ tmp129, tmp128,
	mov	r9, r0	@ oe, oe
	mov	r0, r1	@ oa, oa
	ldrsh	r1, [r3, r5]	@,
	mov	sl, r1	@ D.4026,
	mov	r3, #12	@,
	ldrsh	r1, [r6, r3]	@ D.4028, asx,
	mov	r3, sl	@ tmp130, D.4026
	mul	r3, r1	@ tmp130, D.4028
	and	r2, r2, r4	@ tmp135, tmp127
	lsl	r2, r2, #1	@ tmp136, tmp135,
	ldrsh	r7, [r2, r5]	@ D.4034,
	asr	r3, r3, #8	@ tmp131, tmp130,
	strh	r3, [r0, #6]	@ tmp131, <variable>.pa
	mov	r3, r7	@ tmp137, D.4034
	mul	r3, r1	@ tmp137, D.4028
	neg	r3, r3	@ tmp138, tmp137
	asr	r3, r3, #8	@ tmp139, tmp138,
	strh	r3, [r0, #14]	@ tmp139, <variable>.pb
	mov	r3, #14	@,
	ldrsh	r2, [r6, r3]	@ D.4040, asx,
	mov	r3, r2	@ tmp141, D.4040
	mul	r3, r7	@ tmp141, D.4034
	asr	r3, r3, #8	@ tmp142, tmp141,
	strh	r3, [r0, #22]	@ tmp142, <variable>.pc
	mov	r3, sl	@ tmp144, D.4026
	mul	r3, r2	@ tmp144, D.4040
	asr	r3, r3, #8	@ tmp145, tmp144,
	strh	r3, [r0, #30]	@ tmp145, <variable>.pd
	mov	r0, #128	@,
	lsl	r0, r0, #13	@,,
	bl	Div	@
	ldrh	r3, [r6, #14]	@ temp.216,
	mov	r4, #12	@,
	ldrsh	r2, [r6, r4]	@ tmp148, asx,
	lsl	r3, r3, #16	@ tmp150, temp.216,
	asr	r1, r3, #16	@ tmp149, tmp150,
	mov	r8, r0	@ aa,
	cmp	r2, r1	@ tmp148, tmp149
	beq	.L49	@,
	mov	r0, #128	@,
	lsl	r0, r0, #13	@,,
	bl	Div	@
	mov	ip, r0	@ ad,
.L44:
	mov	r1, r9	@, oe
	ldrh	r0, [r1]	@ D.4058,
	ldrh	r2, [r1, #2]	@ tmp156,
	lsr	r3, r0, #14	@ D.4060, D.4058,
	lsr	r2, r2, #14	@ D.4063, tmp156,
	lsl	r3, r3, #2	@ tmp160, D.4060,
	ldr	r1, .L50+8	@ tmp159,
	add	r3, r3, r2	@ tmp161, tmp160, D.4063
	lsl	r3, r3, #1	@ tmp162, tmp161,
	ldrb	r4, [r3, r1]	@ sx,
	add	r3, r3, r1	@ tmp167, tmp162, tmp159
	ldrb	r5, [r3, #1]	@ sy,
	lsl	r3, r0, #22	@, D.4058,
	bpl	.L45	@,
	neg	r2, r4	@ dx, sx
	neg	r3, r5	@ dy, sy
.L47:
	mov	r0, #8	@,
	ldrsh	r1, [r6, r0]	@ tmp173, asx,
	add	r1, r2, r1	@ dx.208, dx, tmp173
	mov	r0, #10	@,
	ldrsh	r2, [r6, r0]	@ tmp174, asx,
	add	r2, r3, r2	@ dy.206, dy, tmp174
	ldr	r3, [r6]	@ <variable>.px, <variable>.px
	lsl	r4, r4, #7	@ sx.212, sx,
	sub	r4, r4, r3	@ sx.213, sx.212, <variable>.px
	ldr	r3, [r6, #4]	@ <variable>.py, <variable>.py
	mov	r0, sl	@, D.4026
	lsl	r5, r5, #7	@ sy.210, sy,
	sub	r5, r5, r3	@ sy.211, sy.210, <variable>.py
	mov	r3, r8	@ tmp177, aa
	mul	r3, r0	@ tmp177,
	asr	r3, r3, #12	@ tmp178, tmp177,
	mov	r0, r4	@ tmp179, sx.213
	mul	r0, r3	@ tmp179, tmp178
	mov	r3, ip	@ tmp180, ad
	mul	r3, r7	@ tmp180, D.4034
	asr	r3, r3, #12	@ tmp181, tmp180,
	mul	r3, r5	@ tmp182, sy.211
	add	r0, r0, r3	@ tmp183, tmp179, tmp182
	neg	r3, r7	@ tmp186, D.4034
	mov	r7, r3	@, tmp186
	mov	r3, r8	@ tmp187, aa
	mul	r3, r7	@ tmp187,
	asr	r0, r0, #16	@ tmp184, tmp183,
	asr	r3, r3, #12	@ tmp188, tmp187,
	add	r1, r1, r0	@ tmp185, dx.208, tmp184
	mov	r0, r4	@ tmp189, sx.213
	mul	r0, r3	@ tmp189, tmp188
	mov	r4, ip	@, ad
	mov	r3, sl	@ tmp190, D.4026
	mul	r3, r4	@ tmp190,
	asr	r3, r3, #12	@ tmp191, tmp190,
	mul	r3, r5	@ tmp192, sy.211
	add	r0, r0, r3	@ tmp193, tmp189, tmp192
	asr	r0, r0, #16	@ tmp194, tmp193,
	add	r2, r2, r0	@ tmp195, dy.206, tmp194
	mov	r0, r9	@ oe, oe
	bl	oe_set_pos	@
	@ sp needed for prologue	@
	pop	{r2, r3, r4}
	mov	r8, r2
	mov	r9, r3
	mov	sl, r4
	pop	{r4, r5, r6, r7}
	pop	{r0}
	bx	r0
.L45:
	neg	r3, r4	@ tmp171, sx
	asr	r2, r3, #1	@ dx, tmp171,
	neg	r3, r5	@ tmp172, sy
	asr	r3, r3, #1	@ dy, tmp172,
	b	.L47	@
.L49:
	mov	ip, r0	@ ad, aa
	b	.L44	@
.L51:
	.align	2
.L50:
	.word	511
	.word	_sinLUT
	.word	oam_sizes
*/
