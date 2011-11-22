#ifndef _model_jxz_tlm_def_
#define _model_jxz_tlm_def_
#include "rtwtypes.h"

//========= INPUT REGISTER =========
//Number of input register
#define model_jxz_IN_REG_NUM           0

//========= OUTPUT REGISTER =========
//Number of output register
#define model_jxz_OUT_REG_NUM          0

//========= COMMAND/STATUS REGISTER =========
#define RESET_BIT_MASK                 0x01                      //bit 0 write only
#define START_BIT_MASK                 0x02                      //bit 1 write only
#define INT_STAT_BIT_MASK              0x04                      //bit 2 read only
#define INT_DIS_BIT_MASK               0x08                      //bit 3 read/write
#define RESERVED4_BIT_MASK             0x10                      //bit 4 reserved
#define RESERVED5_BIT_MASK             0x20                      //bit 5 reserved
#define RESERVED6_BIT_MASK             0x40                      //bit 6 reserved
#define RESERVED7_BIT_MASK             0x80                      //bit 7 reserved
#define INPUT_PUSH_BIT_MASK            0x0100                    //bit 8 write only
#define INPUT_AUTMOD_BIT_MASK          0x0200                    //bit 9 read/write
#define RESERVED10_BIT_MASK            0x0400                    //bit 10 reserved
#define RESERVED11_BIT_MASK            0x0800                    //bit 11 reserved
#define OUTPUT_PULL_BIT_MASK           0x1000                    //bit 12 write only
#define OUTPUT_AUTMOD_BIT_MASK         0x2000                    //bit 13 read/write
#define RESERVED14_BIT_MASK            0x4000                    //bit 14 reserved
#define RESERVED15_BIT_MASK            0x8000                    //bit 15 reserved
#define INBUF_EMPTY_BIT_MASK           0x010000                  //bit 16 read only
#define INBUF_FULL_BIT_MASK            0x020000                  //bit 17 read only
#define INBUF_UNFLW_BIT_MASK           0x040000                  //bit 18 read/write
#define INBUF_OVFLW_BIT_MASK           0x080000                  //bit 19 read/write
#define OUTBUF_EMPTY_BIT_MASK          0x100000                  //bit 20 read only
#define OUTBUF_FULL_BIT_MASK           0x200000                  //bit 21 read only
#define OUTBUF_UNFLW_BIT_MASK          0x400000                  //bit 22 read/write
#define OUTBUF_OVFLW_BIT_MASK          0x800000                  //bit 23 read/write
#define RESERVED24_BIT_MASK            0x01000000                //bit 24 reserved
#define RESERVED25_BIT_MASK            0x02000000                //bit 25 reserved
#define RESERVED26_BIT_MASK            0x04000000                //bit 26 reserved
#define RESERVED27_BIT_MASK            0x08000000                //bit 27 reserved
#define RESERVED28_BIT_MASK            0x10000000                //bit 28 reserved
#define RESERVED29_BIT_MASK            0x20000000                //bit 29 reserved
#define RESERVED30_BIT_MASK            0x40000000                //bit 30 reserved
#define RESERVED31_BIT_MASK            0x80000000                //bit 31 reserved
#define WRITEONLY_BIT_MASK             (RESET_BIT_MASK | START_BIT_MASK | INPUT_PUSH_BIT_MASK | OUTPUT_PULL_BIT_MASK)
#define READONLY_BIT_MASK              (INT_STAT_BIT_MASK | INBUF_EMPTY_BIT_MASK | INBUF_FULL_BIT_MASK | OUTBUF_EMPTY_BIT_MASK | OUTBUF_FULL_BIT_MASK)
#endif                                 /* _model_jxz_tlm_def_ */
