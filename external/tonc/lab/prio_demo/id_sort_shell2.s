@
@ id_sort_shell2: general shellsort unsing an index table
@   MAXCOUNT: 256
@ === NOTES: ===
@  * changed from oe->fill keys to int keys after completion;
@    not fully optimised for that yet.

	.section .iwram,"ax",%progbits
	.align 	2
@ DECL: CODE_IN_IWRAM void id_sort_shell2(int keys[], u8 ids[], int count)
@ r0: sort key (oe.fill)
@ r1: sort indices 
@ r2: array length
@ r3: inc
@ r4, r5, r6: inc, ii, jj, jj-inc
@ r7: id= ids[jj-inc]
@ r8, r9: id0=ids[ii], key0=fill[ids[ii]]
@ r10: fill[id]
@ r12: (1<<32)/3
	.global id_sort_shell2
id_sort_shell2:
	stmfd	sp!, {r4-r10}
@ TODO: special for count==1
@ setup inc: inc= 3*inc+1
	mov		r3, #0
.Lshell_inc_setup:
    	rsb		r3, r3, r3, lsl #2
    	add		r3, r3, #1
    	cmp		r3, r2
    	ble .Lshell_inc_setup
	ldr		r12, .Lshell_pool
@ inc loop
.Lshell_inc_loop:
	umull	r5, r4, r12, r3		@ ii= inc/3
	mov		r3, r4				@ inc= ii
	cmp		r3, r2				@ ii>count
	bgt	.Lshell_up_loop_end
@ ii=inc to nn loop
.Lshell_up_loop:
    	ldrb	r8, [r1, r4]			@ id0= ids[ii]
    	ldr		r9, [r0, r8, lsl #2]	@ key0= fill[ids[ii]]
    	mov		r5, r4					@ jj= ii
	    sub		r6, r5, r3				@ jnext= jj-inc
@ jj=ii to 0 loop
.Lshell_down_loop:
        	ldrb	r7, [r1, r6]
        	ldr		r10, [r0, r7, lsl #2]
        	cmp		r10, r9					@ fill[id] > key0
        	ble .Lshell_down_loop_end
        	strb	r7, [r1, r5]	@ PONDER: xxxgt?
        	mov		r5, r6
	        subs	r6, r5, r3
        	bge	.Lshell_down_loop
.Lshell_down_loop_end:
		strb	r8, [r1, r5]	@ TODO: clean
		add		r4, r4, #1		@ TODO: combine with ldrb
		cmp		r4, r2
		blt		.Lshell_up_loop
.Lshell_up_loop_end:
	cmp		r3, #1
	bgt .Lshell_inc_loop
	ldmfd	sp!, {r4-r10}
	bx lr
	.align	2
.Lshell_pool:
	.word	1431655766

