#ifndef __DEFINE_H__
#define __DEFINE_H__
#include <stdio.h>

/******************************************** Type ********************************************/
typedef enum {
    OFF = 0,
    ON = !OFF
} LoaderSwitch_t;

/******************************************** Inc Dec ********************************************/
#define U32_DUMMY           0xFFFFFFFFUL
#define U16_DUMMY           0xFFFFUL
#define U8_DUMMY            0xFFUL

#define INC_PARA_U32(x)     do { if((x) < U32_DUMMY) {(x)++;} }while(0)
#define INC_PARA_U16(x)     do { if((x) < U16_DUMMY) {(x)++;} }while(0)
#define INC_PARA_U8(x)      do { if((x) < U8_DUMMY) {(x)++;} }while(0)
#define DEC_PARA(x)         do { if((x) > 0) {(x)--;} }while(0)

/******************************************** BIT ********************************************/
#define BIT(x)              ((1UL)<<(x))

#define BIT0                0x0001UL
#define BIT1                0x0002UL
#define BIT2                0x0004UL
#define BIT3                0x0008UL
#define BIT4                0x0010UL
#define BIT5                0x0020UL
#define BIT6                0x0040UL
#define BIT7                0x0080UL
#define BIT8                0x0100UL
#define BIT9                0x0200UL
#define BIT10               0x0400UL
#define BIT11               0x0800UL
#define BIT12               0x1000UL
#define BIT13               0x2000UL
#define BIT14               0x4000UL
#define BIT15               0x8000UL
#define BIT16               0x00010000UL
#define BIT17               0x00020000UL
#define BIT18               0x00040000UL
#define BIT19               0x00080000UL
#define BIT20               0x00100000UL
#define BIT21               0x00200000UL
#define BIT22               0x00400000UL
#define BIT23               0x00800000UL
#define BIT24               0x01000000UL
#define BIT25               0x02000000UL
#define BIT26               0x04000000UL
#define BIT27               0x08000000UL
#define BIT28               0x10000000UL
#define BIT29               0x20000000UL
#define BIT30               0x40000000UL
#define BIT31               0x80000000UL

/******************************************** Q Cal ********************************************/
#define  Q0(value)  (int32_t)((value) * 0x00000001UL)
#define  Q1(value)  (int32_t)((value) * 0x00000002UL)
#define  Q2(value)  (int32_t)((value) * 0x00000004UL)
#define  Q3(value)  (int32_t)((value) * 0x00000008UL)
#define  Q4(value)  (int32_t)((value) * 0x00000010UL)
#define  Q5(value)  (int32_t)((value) * 0x00000020UL)
#define  Q6(value)  (int32_t)((value) * 0x00000040UL)
#define  Q7(value)  (int32_t)((value) * 0x00000080UL)
#define  Q8(value)  (int32_t)((value) * 0x00000100UL)
#define  Q9(value)  (int32_t)((value) * 0x00000200UL)
#define Q10(value)  (int32_t)((value) * 0x00000400UL)
#define Q11(value)  (int32_t)((value) * 0x00000800UL)
#define Q12(value)  (int32_t)((value) * 0x00001000UL)
#define Q13(value)  (int32_t)((value) * 0x00002000UL)
#define Q14(value)  (int32_t)((value) * 0x00004000UL)
#define Q15(value)  (int32_t)((value) * 0x00008000UL)
#define Q16(value)  (int32_t)((value) * 0x00010000UL)
#define Q17(value)  (int32_t)((value) * 0x00020000UL)
#define Q18(value)  (int32_t)((value) * 0x00040000UL)
#define Q19(value)  (int32_t)((value) * 0x00080000UL)
#define Q20(value)  (int32_t)((value) * 0x00100000UL)
#define Q21(value)  (int32_t)((value) * 0x00200000UL)
#define Q22(value)  (int32_t)((value) * 0x00400000UL)
#define Q23(value)  (int32_t)((value) * 0x00800000UL)
#define Q24(value)  (int32_t)((value) * 0x01000000UL)
#define Q25(value)  (int32_t)((value) * 0x02000000UL)
#define Q26(value)  (int32_t)((value) * 0x04000000UL)
#define Q27(value)  (int32_t)((value) * 0x08000000UL)
#define Q28(value)  (int32_t)((value) * 0x10000000UL)
#define Q29(value)  (int32_t)((value) * 0x20000000UL)
#define Q30(value)  (int32_t)((value) * 0x40000000UL)

#endif