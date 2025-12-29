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

        // ⚠️ 增强显示：明确标识初始化状态
        const char* init_str = (camera_init_flag == 0) ? "OK" : "FAIL";
        OLED_Printf(0,line2,6,"INIT:%s f=%d",init_str,camera_init_flag);

        float fps = Task_GetFPS();
        OLED_Printf(0,line3,6,"fps=%.1f c2=%d",fps,cnt2);

        // 调试信息：st=状态, sc=成功计数
        OLED_Printf(0,line4,6,"st=%d sc=%d",capture_status,capture_success_cnt);

        // ⚠️ 增强显示：失败原因解释
        // fr: 1=VSYNC1超时, 2=VSYNC2超时, 3=HREF超时, 4=PCLK超时
        //     5=未找到JPEG, 6=缓冲满, 7=JPEG不完整, 8=全局超时
        const char* err_str;
        switch(capture_fail_reason) {
            case 0: err_str = "OK"; break;
            case 1: err_str = "VS1"; break;  // VSYNC1超时
            case 2: err_str = "VS2"; break;  // VSYNC2超时（摄像头无输出）
            case 3: err_str = "HRF"; break;  // HREF超时
            case 4: err_str = "PCK"; break;  // PCLK超时
            case 5: err_str = "JPG"; break;  // 未找到JPEG
            case 6: err_str = "BUF"; break;  // 缓冲满
            case 7: err_str = "INC"; break;  // JPEG不完整
            case 8: err_str = "TMO"; break;  // 全局超时
            default: err_str = "???"; break;
        }
        OLED_Printf(0,line5,6,"E:%s hr=%d",err_str,href_state);
    }
    OLED_Update();
}



