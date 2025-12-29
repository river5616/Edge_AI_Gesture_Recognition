#include "camera_app.h"
#include "ov2640.h"
#include <string.h>

// 全局摄像头句柄
static Camera_Handle_t camera_handle;

// FPS计算相关
static uint32_t last_fps_time = 0;
static uint32_t fps_frame_count = 0;

// 初始化摄像头应用
void Camera_App_Init(void)
{
    // 清空句柄
    memset(&camera_handle, 0, sizeof(Camera_Handle_t));
    camera_handle.state = CAM_STATE_IDLE;

    // 初始化OV2640
    if(OV2640_Init() != 0)
    {
        camera_handle.state = CAM_STATE_ERROR;
        return;
    }

    // 配置为RGB565模式
    OV2640_RGB565_Mode();

    // 设置图像大小为96x96
    OV2640_ImageWin_Set(0, 0, IMAGE_WIDTH * 4, IMAGE_HEIGHT * 4);
    OV2640_OutSize_Set(IMAGE_WIDTH, IMAGE_HEIGHT);

    // 设置自动曝光
    OV2640_Auto_Exposure(2);

    // 初始化FPS计时
    last_fps_time = HAL_GetTick();

    camera_handle.state = CAM_STATE_IDLE;
}

// 开始采集
void Camera_App_Start(void)
{
    camera_handle.state = CAM_STATE_CAPTURING;
}

// 停止采集
void Camera_App_Stop(void)
{
    camera_handle.state = CAM_STATE_IDLE;
}

// 等待VSYNC信号（帧开始）
void Camera_WaitVsync(void)
{
    uint32_t timeout = 1000;  // 超时计数

    // 等待VSYNC变低
    while(OV2640_VSYNC_READ() && timeout > 0)
    {
        timeout--;
    }

    timeout = 1000;
    // 等待VSYNC变高（新帧开始）
    while(!OV2640_VSYNC_READ() && timeout > 0)
    {
        timeout--;
    }
}

// 等待HREF信号（行开始）
void Camera_WaitHref(void)
{
    uint32_t timeout = 1000;

    // 等待HREF变高
    while(!OV2640_HREF_READ() && timeout > 0)
    {
        timeout--;
    }
}

// RGB565转灰度
// Gray = (R * 299 + G * 587 + B * 114) / 1000
void Camera_RGB565ToGray(uint16_t rgb565, uint8_t* gray)
{
    uint8_t r, g, b;

    // 提取RGB分量
    r = (rgb565 >> 11) & 0x1F;  // 5位
    g = (rgb565 >> 5) & 0x3F;   // 6位
    b = rgb565 & 0x1F;          // 5位

    // 扩展到8位
    r = (r * 255) / 31;
    g = (g * 255) / 63;
    b = (b * 255) / 31;

    // 灰度转换
    *gray = (uint8_t)((r * 299 + g * 587 + b * 114) / 1000);
}

// 读取一个像素（RGB565格式，2字节）
void Camera_ReadPixel(uint16_t* rgb565_data)
{
    uint8_t byte_high, byte_low;
    uint32_t timeout = 1000;

    // 等待PCLK上升沿
    while(!OV2640_PCLK_READ() && timeout > 0) timeout--;
    byte_high = OV2640_DATA_READ();

    timeout = 1000;
    // 等待PCLK下降沿
    while(OV2640_PCLK_READ() && timeout > 0) timeout--;

    timeout = 1000;
    // 等待PCLK上升沿
    while(!OV2640_PCLK_READ() && timeout > 0) timeout--;
    byte_low = OV2640_DATA_READ();

    // 组合为RGB565
    *rgb565_data = (byte_high << 8) | byte_low;
}

// 捕获一帧图像
uint8_t Camera_App_CaptureFrame(void)
{
    uint16_t x, y;
    uint16_t rgb565;
    uint8_t gray;
    uint8_t write_buffer;

    if(camera_handle.state != CAM_STATE_CAPTURING)
    {
        return 1;  // 未处于采集状态
    }

    // 选择写入缓冲区
    write_buffer = camera_handle.current_buffer;

    // 等待新帧开始
    Camera_WaitVsync();

    // 逐行读取图像
    for(y = 0; y < IMAGE_HEIGHT; y++)
    {
        // 等待行开始
        Camera_WaitHref();

        // 读取一行像素
        for(x = 0; x < IMAGE_WIDTH; x++)
        {
            // 读取RGB565像素
            Camera_ReadPixel(&rgb565);

            // 转换为灰度
            Camera_RGB565ToGray(rgb565, &gray);

            // 存储到缓冲区
            camera_handle.gray_buffer[write_buffer][y * IMAGE_WIDTH + x] = gray;
        }
    }

    // 切换缓冲区
    camera_handle.ready_buffer = write_buffer;
    camera_handle.current_buffer = (write_buffer + 1) % NUM_BUFFERS;
    camera_handle.state = CAM_STATE_READY;

    // 更新帧计数
    camera_handle.frame_count++;
    fps_frame_count++;

    return 0;  // 成功
}

// 获取已就绪的帧
uint8_t* Camera_App_GetFrame(void)
{
    if(camera_handle.state == CAM_STATE_READY)
    {
        camera_handle.state = CAM_STATE_CAPTURING;  // 重新开始采集
        return camera_handle.gray_buffer[camera_handle.ready_buffer];
    }

    return NULL;
}

// 更新FPS
void Camera_App_UpdateFPS(void)
{
    uint32_t current_time = HAL_GetTick();
    uint32_t elapsed = current_time - last_fps_time;

    if(elapsed >= 1000)  // 每秒更新一次
    {
        camera_handle.fps = (float)fps_frame_count * 1000.0f / (float)elapsed;
        fps_frame_count = 0;
        last_fps_time = current_time;
    }
}

// 获取FPS
float Camera_App_GetFPS(void)
{
    return camera_handle.fps;
}

// 获取状态
Camera_State_t Camera_App_GetState(void)
{
    return camera_handle.state;
}
