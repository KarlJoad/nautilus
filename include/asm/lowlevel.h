/*
 * This file is part of the Nautilus AeroKernel developed
 * by the Hobbes and V3VEE Projects with funding from the
 * United States National  Science Foundation and the Department of Energy.
 *
 * The V3VEE Project is a joint project between Northwestern University
 * and the University of New Mexico.  The Hobbes Project is a collaboration
 * led by Sandia National Laboratories that includes several national
 * laboratories and universities. You can find out more at:
 * http://www.v3vee.org  and
 * http://xtack.sandia.gov/hobbes
 *
 * Copyright (c) 2015, Kyle C. Hale <kh@u.northwestern.edu>
 * Copyright (c) 2015, The V3VEE Project  <http://www.v3vee.org>
 *                     The Hobbes Project <http://xstack.sandia.gov/hobbes>
 * All rights reserved.
 *
 * Author: Kyle C. Hale <kh@u.northwestern.edu>
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "LICENSE.txt".
 */
#ifndef __LOWLEVEL_H__
#define __LOWLEVEL_H__


#ifdef NAUT_CONFIG_RISCV_HOST
#define ENTRY(x)   \
    .globl x;      \
    .align 4, 0x00,0x01;\
    x:

#define PTRLOG 3
#define SZREG 8
#define REG_S sd
#define REG_L ld
#define REG_SC sc.d
#define ROFF(N, R) N*SZREG(R)

#else
#define GEN_NOP(x) .byte x

#define NOP_1BYTE 0x90
#define NOP_2BYTE 0x66,0x90
#define NOP_3BYTE 0x0f,0x1f,0x00
#define NOP_4BYTE 0x0f,0x1f,0x40,0
#define NOP_5BYTE 0x0f,0x1f,0x44,0x00,0
#define NOP_6BYTE 0x66,0x0f,0x1f,0x44,0x00,0
#define NOP_7BYTE 0x0f,0x1f,0x80,0,0,0,0
#define NOP_8BYTE 0x0f,0x1f,0x84,0x00,0,0,0,0

#define ENTRY(x)   \
    .globl x;      \
    .align 4, 0x90;\
    x:
#endif

#define GLOBAL(x)  \
    .globl x;      \
    x:

#define END(x) \
    .size x, .-x

#endif
