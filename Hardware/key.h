#ifndef __KEY_H
#define __KEY_H

#include "headfile.h"

typedef struct{
	u8 key_sta;
	u8 key_judge;
	u32 key_time;
	u8 single_flag;
	u8 long_flag;
	u8 double_pre;
	u32 double_time;
	u8 double_flag;
}keys;


void key_task(void);
void key_proc(void);












#endif
