#include "disp.h"

u8 view; // 显示页面
u8 cnt1,cnt2;
u8 camera_init_flag;
u8 capture_status = 0;  // capture_frame返回值
u8 capture_success_cnt = 0;  // 采集成功次数
u8 capture_fail_reason = 0;  // 失败原因代码
u8 href_state = 0;  // HREF引脚状态（调试用）

void oled_disp(void)
{
    if(view==0)
    {
        // 显示工作模式
        WorkMode_t mode = Task_GetMode();
        const char* mode_str;
        switch(mode)
        {
            case MODE_IDLE:       mode_str = "IDLE"; break;
            case MODE_COLLECTING: mode_str = "COLL"; break;
            case MODE_INFERENCE:  mode_str = "INFE"; break;
            default:              mode_str = "????"; break;
        }
        OLED_Printf(0,line1,6,"M:%s c1=%d",mode_str,cnt1);
        OLED_Printf(0,line2,6,"c2=%d,f=%d",cnt2,camera_init_flag);
        float fps = Task_GetFPS();
        OLED_Printf(0,line3,6,"fps=%.1f",fps);
        // 调试信息：st=状态, sc=成功计数, fr=失败原因
        // fr: 1=VSYNC1超时, 2=VSYNC2超时, 3=HREF超时, 4=PCLK超时
        OLED_Printf(0,line4,6,"st=%d sc=%d",capture_status,capture_success_cnt);
        OLED_Printf(0,line5,6,"fr=%d hr=%d",capture_fail_reason,href_state);
    }
    OLED_Update();
}



