#include "disp.h"
#include "openmv_rx_task.h"
#include "inference_task.h"

u8 view = VIEW_STATUS; // 显示页面，默认状态视图
u8 cnt1,cnt2;
u8 camera_init_flag;
u8 capture_status = 0;  // capture_frame返回值
u8 capture_success_cnt = 0;  // 采集成功次数
u8 capture_fail_reason = 0;  // 失败原因代码
u8 href_state = 0;  // HREF引脚状态（调试用）

// 静态变量：保存最新接收到的图像
static uint8_t last_image[OPENMV_IMAGE_SIZE] = {0};
static uint8_t has_image = 0;

// 调试变量：接收字节数
uint16_t g_debug_rx_bytes = 0;
uint16_t g_debug_rx_bytes_max = 0;  // 最大接收字节数

// 手势识别结果（用于显示）
static uint8_t s_display_gesture_id = 0;
static float s_display_gesture_confidence = 0.0f;
static uint8_t s_display_gesture_valid = 0;  // 标记是否有有效的识别结果

void oled_disp(void)
{
	// 获取UART接收统计信息
	OpenMVRxStats_t stats;
    OpenMVRxTask_GetStats(&stats);
    if(view == VIEW_STATUS)  // 状态信息视图
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
        OLED_Printf(0,line1,6,"M:%s c1=%d   ",mode_str,cnt1);

        // ⚠️ 增强显示：明确标识初始化状态
        const char* init_str = (camera_init_flag == 0) ? "OK" : "FAIL";
        OLED_Printf(0,line2,6,"INIT:%s f=%d",init_str,camera_init_flag);

        OLED_Printf(0,line3,6,"fps=%.1f c2=%d   ",stats.fps,cnt2);

        // 如果在推理模式且有识别结果，显示识别结果
        if(mode == MODE_INFERENCE && s_display_gesture_valid)
        {
            if(s_display_gesture_id == 0xFF)
            {
                // 推理失败
                OLED_Printf(0,line4,6,"Infer:FAIL    ");
            }
            else if(s_display_gesture_id < 9)
            {
                // 手势名称映射
                const char* gesture_names[9] = {
                    "G0", "G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8"
                };

                const char* gesture_name = gesture_names[s_display_gesture_id];
                uint8_t conf_percent = (uint8_t)(s_display_gesture_confidence * 100);

                OLED_Printf(0,line4,6,"Gest:%s C:%d%%  ", gesture_name, conf_percent);
            }
            else
            {
                // 无效ID
                OLED_Printf(0,line4,6,"Gest:?? C:0%%  ");
            }
        }
        else
        {
            // 调试：显示接收字节数（当前值和最大值）
            OLED_Printf(0,line4,6,"RxB:%d/%d     ", g_debug_rx_bytes, g_debug_rx_bytes_max);
        }

        // 显示UART接收统计
        OpenMVRxStats_t stats;
        OpenMVRxTask_GetStats(&stats);
        OLED_Printf(0,line5,6,"T:%lu E:%lu  ", stats.total_frames, stats.error_frames);

        // 在推理模式下显示推理统计
        if(mode == MODE_INFERENCE)
        {
            InferenceStats_t infer_stats;
            InferenceTask_GetStats(&infer_stats);
            OLED_Printf(0,line6,6,"Inf:%lu/%lu  ", infer_stats.success_count, infer_stats.total_count);
        }

    }
    else if(view == VIEW_IMAGE)  // 图像显示视图
    {
        // 临时方案：显示图像接收状态，不直接显示图像
        // （因为需要格式转换，稍后实现）

        OLED_Printf(0,line1,6,"IMAGE View");

        // 安全检查：确保imageQueue已初始化
        if(imageQueue != NULL)
        {
            // 获取队列中的消息数量
            uint32_t msg_count = osMessageQueueGetCount(imageQueue);
            OLED_Printf(0,line2,6,"Queue:%lu   ", msg_count);

            // 尝试从队列获取最新图像信息（非阻塞）
            ImageMsg_t img_msg;
            if(osMessageQueueGet(imageQueue, &img_msg, NULL, 0) == osOK)
            {
                // 安全检查：确保指针有效
                if(img_msg.data != NULL && img_msg.data_size == OPENMV_IMAGE_SIZE)
                {
                    // 显示图像信息
                    OLED_Printf(0,line3,6,"Size:%lux%lu  ", img_msg.width, img_msg.height);
                    OLED_Printf(0,line4,6,"Bytes:%lu  ", img_msg.data_size);
                    OLED_Printf(0,line5,6,"FrameID:%lu  ", img_msg.frame_id);

                    // 显示图像数据的前几个字节（验证数据）
                    OLED_Printf(0,line6,6,"D0:%02X %02X %02X  ",
                               img_msg.data[0], img_msg.data[1], img_msg.data[2]);

                    has_image = 1;
                }
                else
                {
                    OLED_Printf(0,line3,6,"Invalid data!");
                }
            }
            else
            {
                OLED_Printf(0,line3,6,"No new image");
            }
        }
        else
        {
            OLED_Printf(0,line2,6,"Queue=NULL");
        }

        if(!has_image)
        {
            OLED_Printf(0,line7,6,"Waiting...");
        }
    }
    else if(view == VIEW_UART_STATS)  // UART接收统计视图
    {
        OLED_Printf(0,line1,6,"UART3 Stats");
        OLED_Printf(0,line2,6,"Total:%lu  ", stats.total_frames);
        OLED_Printf(0,line3,6,"Error:%lu  ", stats.error_frames);
        OLED_Printf(0,line4,6,"ChkErr:%lu  ", stats.checksum_errors);
        OLED_Printf(0,line5,6,"Timout:%lu  ", stats.timeout_errors);
        OLED_Printf(0,line6,6,"RxFPS:%.1f  ", stats.fps);

        // 计算错误率
        if(stats.total_frames > 0)
        {
            float error_rate = (float)stats.error_frames * 100.0f / stats.total_frames;
            OLED_Printf(0,line7,6,"ErrRate:%.1f%%  ", error_rate);
        }
        else
        {
            OLED_Printf(0,line7,6,"ErrRate:N/A  ");
        }
    }
    else if(view == VIEW_ALL_CONF)  // 所有手势置信度视图
    {
        WorkMode_t mode = Task_GetMode();

        if(mode == MODE_INFERENCE)
        {
            // 获取所有9个手势的置信度
            float all_confs[NUM_GESTURES];
            InferenceTask_GetAllConfidences(all_confs);

            // 显示标题
            OLED_Printf(0,line1,6,"All Confidences");

            // 手势名称（缩写）
            const char* gesture_names[NUM_GESTURES] = {
                "Fis", "Pal", "Thm", "OK ", "V  ", "Pnt", "3  ", "4  ", "Non"
            };

            // 分两页显示：第1页显示0-4，第2页显示5-8
            // 使用 cnt2 的低位作为页面切换指示（每2秒自动翻页）
            uint8_t page = (cnt2 / 200) % 2;  // 每2秒切换一次页面

            if(page == 0)
            {
                // 第1页：显示手势 0-4
                OLED_Printf(0,line2,6,"Page 1/2");
                for(uint8_t i = 0; i < 5; i++)
                {
                    uint8_t conf_percent = (uint8_t)(all_confs[i] * 100);
                    OLED_Printf(0, line3 + i*8, 6, "%d:%s %3d%%", i, gesture_names[i], conf_percent);
                }
            }
            else
            {
                // 第2页：显示手势 5-8
                OLED_Printf(0,line2,6,"Page 2/2");
                for(uint8_t i = 5; i < 9; i++)
                {
                    uint8_t conf_percent = (uint8_t)(all_confs[i] * 100);
                    OLED_Printf(0, line3 + (i-5)*8, 6, "%d:%s %3d%%", i, gesture_names[i], conf_percent);
                }
                // 留一行空白
                OLED_Printf(0,line7,6,"              ");
            }
        }
        else
        {
            OLED_Printf(0,line1,6,"All Confidences");
            OLED_Printf(0,line3,6,"Mode: INFERENCE");
            OLED_Printf(0,line4,6,"to view data");
        }
    }

    OLED_Update();
}

/**
  * 函    数：更新手势识别结果
  * 参    数：GestureID 手势ID，范围：0~8
  * 参    数：Confidence 置信度，范围：0.0~1.0
  * 返 回 值：无
  * 说    明：此函数仅更新全局变量，实际显示由oled_disp()统一处理
  */
void OLED_ShowGestureResult(uint8_t GestureID, float Confidence)
{
	s_display_gesture_id = GestureID;
	s_display_gesture_confidence = Confidence;
	s_display_gesture_valid = 1;
}

