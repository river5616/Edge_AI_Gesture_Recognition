#include "disp.h"

u8 view; // 显示页面
u8 cnt;

void oled_disp(void)
{
    if(view==0)
    {
        OLED_Printf(0,line1,6,"cnt=%d   ",cnt);
    }
    OLED_Update();
}



