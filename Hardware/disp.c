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

// 全局传感器数据（由sensor_task更新）
SensorData_t g_sensorData = {0};

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

uint8_t adc_dma_flag;


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

		// 调试：显示接收字节数（当前值和最大值）
		OLED_Printf(0,line4,6,"RxB:%d/%d     ", g_debug_rx_bytes, g_debug_rx_bytes_max);
        // 显示UART接收统计（移到line6）
        OLED_Printf(0,line5,6,"T:%lu E:%lu  ", stats.total_frames, stats.error_frames);

        // 在推理模式下显示推理统计（移到line7）
        if(mode == MODE_INFERENCE)
        {
            InferenceStats_t infer_stats;
            InferenceTask_GetStats(&infer_stats);
            OLED_Printf(0,line6,6,"Inf:%lu/%lu  ", infer_stats.success_count, infer_stats.total_count);
        }
		// 如果在推理模式且有识别结果，显示识别结果（分两行）
        if(mode == MODE_INFERENCE && s_display_gesture_valid)
        {
            if(s_display_gesture_id == 0xFF)
            {
                // 推理失败
                OLED_Printf(0,line7,6,"Gesture:FAIL   ");
                OLED_Printf(0,line8,6,"              ");
            }
            else if(s_display_gesture_id < 5)  // 5个手势
            {
                // 手势名称映射 - 5个手势（fist/palm/thumbs_up/ok/nothing）
                const char* gesture_names[5] = {
                    "Fist", "Palm", "Thumb", "OK", "None"
                };

                const char* gesture_name = gesture_names[s_display_gesture_id];

                // 第4行：显示手势类型
                OLED_Printf(0,line7,6,"G%d:%s       ", s_display_gesture_id, gesture_name);

                // 第5行：显示置信度（浮点数，限制范围）
                float conf_percent = s_display_gesture_confidence * 100.0f;

                // 安全限制：确保范围在 0.0-100.0
                if (conf_percent < 0.0f) {
                    conf_percent = 0.0f;
                } else if (conf_percent > 100.0f) {
                    conf_percent = 100.0f;
                }

                OLED_Printf(0,line8,6,"Conf:%.1f%%  ", conf_percent);
            }
            else
            {
                // 无效ID
                OLED_Printf(0,line4,6,"Gesture:??    ");
                OLED_Printf(0,line5,6,"Conf:0.0%%    ");
            }
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
    else if(view == VIEW_SENSORS)  // 传感器数据视图
    {
        /* 显示标题 */
        OLED_Printf(0,line1,6,"=Sensors=");

        /* 显示温湿度（DHT11） */
        if (g_sensorData.dht11Valid) {
            OLED_Printf(0,line2,6,"T:%dC H:%d%%  ", g_sensorData.temperature, g_sensorData.humidity);
        } else {
            OLED_Printf(0,line2,6,"T:--C H:--%% ");
        }

        /* 显示光照强度 */
        OLED_Printf(0,line3,6,"L:%3dLux", (uint16_t)g_sensorData.lightLux);

        /* 显示距离 */
        if (g_sensorData.distance < 500) {
            OLED_Printf(0,line4,6,"D:%3dcm", g_sensorData.distance);
        } else {
            OLED_Printf(0,line4,6,"D:---cm");
        }

        /* 显示MQ2数据 */
        OLED_Printf(0,line5,6,"MQ2:%3.1f", g_sensorData.mq2PPM);

        /* 显示心率和血氧 */
        if (g_sensorData.heartRateValid) {
            OLED_Printf(0,line6,6,"HR:%d   ", g_sensorData.heartRate);
        } else {
            OLED_Printf(0,line6,6,"HR:--   ");
        }
		if (g_sensorData.spo2Valid) {
            OLED_Printf(7*6,line6,6,"SpO2:%d%%  ", g_sensorData.spo2);
        } else {
            OLED_Printf(7*6,line6,6,"SpO2:--    ");
        }
    }
    else if(view == VIEW_ALL_CONF)  // 所有手势置信度视图（5个手势，单页）
    {
        WorkMode_t mode = Task_GetMode();

        if(mode == MODE_INFERENCE)
        {
            // 获取全部5个手势的置信度（NUM_GESTURES==5）
            float all_confs[NUM_GESTURES];
            InferenceTask_GetAllConfidences(all_confs);

            // 手势名称缩写（与5类模型顺序一致：fist/palm/thumbs_up/ok/nothing）
            static const char* const gesture_names[NUM_GESTURES] = {
                "Fist", "Palm", "Thmb", "OK  ", "None"
            };

            OLED_Printf(0,line1,6,"All Confidences");

            // line2~line6 逐行显示5个手势，置信度用整数百分比
            for(uint8_t i = 0; i < NUM_GESTURES; i++)
            {
                // 先夹紧到[0,1]再转百分比，避免异常值导致显示错乱
                float c = all_confs[i];
                if(c < 0.0f) c = 0.0f;
                if(c > 1.0f) c = 1.0f;
                int conf_percent = (int)(c * 100.0f + 0.5f);  // 四舍五入

                OLED_Printf(0, line2 + i*8, 6, "%d:%s %3d%%",
                            i, gesture_names[i], conf_percent);
            }
        }
        else
        {
            OLED_Printf(0,line1,6,"All Confidences");
            OLED_Printf(0,line3,6,"Switch to");
            OLED_Printf(0,line4,6,"INFERENCE mode");
        }
    }

    OLED_Update();
}

/**
  * 函    数：更新手势识别结果
  * 参    数：GestureID 手势ID，范围：0~4 (5个手势: fist/palm/thumbs_up/ok/nothing)
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

