#ifndef __CAMERA_APP_H
#define __CAMERA_APP_H

#include "main.h"
#include <stdint.h>

// 图像参数
#define IMAGE_WIDTH   96
#define IMAGE_HEIGHT  96
#define IMAGE_SIZE    (IMAGE_WIDTH * IMAGE_HEIGHT)

// 缓冲区数量（乒乓缓冲）
#define NUM_BUFFERS   2

// 摄像头状态
typedef enum {
    CAM_STATE_IDLE = 0,
    CAM_STATE_CAPTURING,
    CAM_STATE_READY,
    CAM_STATE_ERROR
} Camera_State_t;

// 图像缓冲区
typedef struct {
    uint8_t gray_buffer[NUM_BUFFERS][IMAGE_SIZE];  // 灰度图像缓冲区
    uint8_t current_buffer;                         // 当前写入缓冲区
    uint8_t ready_buffer;                           // 已就绪缓冲区
    Camera_State_t state;
    uint32_t frame_count;
    float fps;
} Camera_Handle_t;

// 函数声明
void Camera_App_Init(void);
void Camera_App_Start(void);
void Camera_App_Stop(void);
uint8_t Camera_App_CaptureFrame(void);
uint8_t* Camera_App_GetFrame(void);
void Camera_App_UpdateFPS(void);
float Camera_App_GetFPS(void);
Camera_State_t Camera_App_GetState(void);

// 内部函数
void Camera_ReadPixel(uint16_t* rgb565_data);
void Camera_RGB565ToGray(uint16_t rgb565, uint8_t* gray);
void Camera_WaitVsync(void);
void Camera_WaitHref(void);

#endif
