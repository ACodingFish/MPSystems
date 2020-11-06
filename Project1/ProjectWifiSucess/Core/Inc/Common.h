#ifndef _COMMON_H_
#define _COMMON_H_

#define FLAG_SET_FLAG(mask, x) (mask |= (0x01<<x))
#define FLAG_CLR_FLAG(mask, x) (mask &= ~(0x01<<x))
#define FLAG_TGL_FLAG(mask, x) (mask ^= (0x01<<x))
#define FLAG_CHK_FLAG(mask, x) ((mask & (0x01<<x))>0)

#endif //_COMMON_H_
