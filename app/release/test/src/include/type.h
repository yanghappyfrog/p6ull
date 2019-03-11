
#ifndef __TYPE_H__
#define __TYPE_H__

#define TRUE    (1)
#define FALSE   (0)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define ABS(a)     (((a) < 0) ? -(a) : (a)) 
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

#endif
