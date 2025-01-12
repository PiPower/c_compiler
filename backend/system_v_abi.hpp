#ifndef SYSTEM_V_ABI_H
#define SYSTEM_V_ABI_H


#define SYSV_INTEGER (0x01 << 0)
#define SYSV_SSE (0x01 << 1)
#define SYSV_SSE_UP (0x01 << 2)
#define SYSV_X87 (0x01 << 3)
#define SYSV_X87_UP (0x01 << 4)
#define SYSV_COMPLEX_X87 (0x01 << 5)
#define SYSV_NO_CLASS (0x01 << 6)
#define SYSV_MEMORY (0x01 << 7)

#define PTR_SIZE 8

#define RET_NONE 0
#define RET_RAX (0x01 << 0)
#define RET_RDX (0x01 << 1)
#define RET_XMM0 (0x01 << 2)
#define RET_XMM1 (0x01 << 3)
#define RET_MEM (0x01 << 4)

#endif