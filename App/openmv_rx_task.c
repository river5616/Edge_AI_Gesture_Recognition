#include "openmv_rx_task.h"
#include "disp.h"
#include <string.h>
#include <stdio.h>

// ==================== 配置选项 ====================
// 使用UART空闲中断方式（更高效）
#define USE_IDLE_INTERRUPT  1

// DMA缓冲区大小（足够容纳一个完整数据包）
#define DMA_BUFFER_SIZE     (OPENMV_PACKET_SIZE * 2)  // 8206字节

// ==================== 数据结构 ====================

// 接收状态机
typedef enum {
    STATE_WAIT_HEADER,      // 等待帧头
    STATE_RECEIVE_PACKET    // 接收数据包
} RxState_t;

// DMA双缓冲区（循环接收，暴露给uart_callbacks.c使用）
uint8_t dma_buffer[DMA_BUFFER_SIZE] __attribute__((section(".ram_d1"), aligned(32)));

// 图像数据缓冲区（存储解析后的图像）
static uint8_t image_buffer[2][OPENMV_IMAGE_SIZE] __attribute__((section(".dtcmram"), aligned(32)));
static uint8_t current_image_buf = 0;

// 数据包解析缓冲区
static uint8_t packet_buffer[OPENMV_PACKET_SIZE] __attribute__((section(".ram_d1"), aligned(32)));
static uint16_t packet_index = 0;

// 统计信息
static OpenMVRxStats_t rx_stats = {0};
static uint32_t last_fps_time = 0;
static uint32_t frame_count_for_fps = 0;

// 任务通知句柄（暴露给uart_callbacks.c使用）
osThreadId_t openmv_task_handle = NULL;

// ==================== 函数实现 ====================

/**
 * @brief  初始化OpenMV接收任务
 */
void OpenMVRxTask_Init(void)
{
    memset(dma_buffer, 0, sizeof(dma_buffer));
    memset(image_buffer, 0, sizeof(image_buffer));
    memset(&rx_stats, 0, sizeof(rx_stats));

    packet_index = 0;
    last_fps_time = osKernelGetTickCount();

    // Cache管理（确保DMA和CPU数据一致性）
    SCB_CleanInvalidateDCache();
}

/**
 * @brief  获取接收统计信息
 */
void OpenMVRxTask_GetStats(OpenMVRxStats_t* stats)
{
    if(stats)
    {
        memcpy(stats, &rx_stats, sizeof(OpenMVRxStats_t));
    }
}

/**
 * @brief  校验数据包
 */
static uint8_t verify_checksum(const uint8_t* data, uint16_t length, uint8_t checksum)
{
    uint32_t calc_checksum = 0;

    // 使用32位加法优化（4字节对齐访问）
    uint16_t i = 0;
    for(; i < (length & ~3); i += 4)
    {
        calc_checksum += data[i];
        calc_checksum += data[i+1];
        calc_checksum += data[i+2];
        calc_checksum += data[i+3];
    }

    // 处理剩余字节
    for(; i < length; i++)
    {
        calc_checksum += data[i];
    }

    return ((calc_checksum & 0xFF) == checksum);
}

/**
 * @brief  在缓冲区中查找帧头
 */
static int16_t find_header(const uint8_t* buffer, uint16_t length)
{
    for(uint16_t i = 0; i < length - 1; i++)
    {
        if(buffer[i] == OPENMV_HEADER_0 && buffer[i+1] == OPENMV_HEADER_1)
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief  处理接收到的完整数据包
 */
static uint8_t process_packet(const uint8_t* packet, uint16_t length)
{
    // 最小长度检查
    const uint16_t min_length = OPENMV_HEADER_SIZE + OPENMV_LENGTH_SIZE +
                                OPENMV_CHECKSUM_SIZE + OPENMV_FOOTER_SIZE;
    if(length < min_length)
    {
        rx_stats.error_frames++;
        return 0;
    }

    // 1. 提取长度（Big Endian）
    uint16_t data_length = ((uint16_t)packet[2] << 8) | packet[3];

    // 长度校验
    if(data_length != OPENMV_IMAGE_SIZE)
    {
        rx_stats.error_frames++;
        return 0;
    }

    // 2. 检查数据包总长度
    uint16_t expected_total = OPENMV_HEADER_SIZE + OPENMV_LENGTH_SIZE +
                              OPENMV_CHECKSUM_SIZE + data_length +
                              OPENMV_FOOTER_SIZE;
    if(length < expected_total)
    {
        rx_stats.error_frames++;
        return 0;
    }

    // 3. 提取校验值和图像数据
    uint8_t checksum = packet[4];
    const uint8_t* image_data = &packet[5];

    // 4. 校验数据
    if(!verify_checksum(image_data, data_length, checksum))
    {
        rx_stats.checksum_errors++;
        rx_stats.error_frames++;
        return 0;
    }

    // 5. 检查帧尾
    uint16_t footer_index = 5 + data_length;
    if(packet[footer_index] != OPENMV_FOOTER_0 ||
       packet[footer_index + 1] != OPENMV_FOOTER_1)
    {
        rx_stats.error_frames++;
        return 0;
    }

    // 6. 复制图像数据（使用memcpy优化）
    memcpy(image_buffer[current_image_buf], image_data, OPENMV_IMAGE_SIZE);

    // Cache管理
    SCB_CleanDCache_by_Addr((uint32_t*)image_buffer[current_image_buf], OPENMV_IMAGE_SIZE);

    // 7. 发送图像消息到队列
    ImageMsg_t img_msg = {
        .data = image_buffer[current_image_buf],
        .width = OPENMV_IMAGE_WIDTH,
        .height = OPENMV_IMAGE_HEIGHT,
        .data_size = OPENMV_IMAGE_SIZE,
        .timestamp = osKernelGetTickCount(),
        .frame_id = rx_stats.total_frames
    };

    // 切换缓冲区
    current_image_buf = 1 - current_image_buf;

    // 发送到队列
    if(imageQueue != NULL)
    {
        osStatus_t status = osMessageQueuePut(imageQueue, &img_msg, 0, 0);

        // 如果成功放入队列，设置标志通知任务释放信号量
        if(status == osOK)
        {
            // 通知任务有新帧（在任务中释放信号量，不在中断中）
            if(openmv_task_handle != NULL)
            {
                osThreadFlagsSet(openmv_task_handle, 0x02);  // 使用标志位0x02表示需要释放信号量
            }
        }
    }

    // 更新统计
    rx_stats.total_frames++;
    frame_count_for_fps++;

    return 1;
}

/**
 * @brief  解析接收缓冲区（暴露给uart_callbacks.c使用）
 */
void parse_receive_buffer(const uint8_t* buffer, uint16_t length)
{
    // 在缓冲区中查找帧头
    int16_t header_pos = find_header(buffer, length);

    if(header_pos < 0)
    {
        // 未找到帧头
        return;
    }

    // 检查是否有完整数据包
    uint16_t remaining = length - header_pos;
    if(remaining >= OPENMV_PACKET_SIZE)
    {
        // 处理数据包
        process_packet(&buffer[header_pos], remaining);
    }
}

#if USE_IDLE_INTERRUPT

/**
 * @brief  UART空闲中断回调（在中断中调用）
 */
void USART3_IDLE_Callback(void)
{
    // 调试：翻转LED2表示中断被触发
    extern void HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_1);  // LED2

    // 清除空闲中断标志
    __HAL_UART_CLEAR_IDLEFLAG(&huart3);

    // 停止DMA
    HAL_UART_DMAStop(&huart3);

    // 计算接收到的数据量
    uint16_t received = DMA_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart3.hdmarx);

    // 更新cnt2用于调试
    extern u8 cnt2;
    cnt2++;

    // 调试：保存接收字节数（用于OLED显示）
    extern uint16_t g_debug_rx_bytes;
    extern uint16_t g_debug_rx_bytes_max;
    g_debug_rx_bytes = received;

    // 记录最大值
    if(received > g_debug_rx_bytes_max)
    {
        g_debug_rx_bytes_max = received;
    }

    if(received > 0)
    {
        // Cache失效，确保读取到最新数据
        SCB_InvalidateDCache_by_Addr((uint32_t*)dma_buffer, DMA_BUFFER_SIZE);

        // 解析数据
        parse_receive_buffer(dma_buffer, received);
    }

    // 重新启动DMA接收
    HAL_UART_Receive_DMA(&huart3, dma_buffer, DMA_BUFFER_SIZE);

    // 通知任务（可选）
    if(openmv_task_handle != NULL)
    {
        osThreadFlagsSet(openmv_task_handle, 0x01);
    }
}

#endif

/**
 * @brief  OpenMV接收任务（轮询模式）
 */
void AppTask_openmv_rx(void *argument)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint32_t last_rx_time = osKernelGetTickCount();
    uint16_t last_dma_count = 0;

    // 保存任务句柄
    openmv_task_handle = osThreadGetId();

    OpenMVRxTask_Init();

    // 启动DMA循环接收
#if USE_IDLE_INTERRUPT
    // 使用空闲中断模式
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart3, dma_buffer, DMA_BUFFER_SIZE);
#else
    // 使用轮询模式
    HAL_UART_Receive_DMA(&huart3, dma_buffer, DMA_BUFFER_SIZE);
#endif
    //osThreadSuspend(NULL);

    for(;;)
    {
#if USE_IDLE_INTERRUPT
        // 等待空闲中断通知（超时100ms）
        uint32_t flags = osThreadFlagsWait(0x03, osFlagsWaitAny, pdMS_TO_TICKS(100));

        // 检查是否需要释放信号量（标志位0x02）
        if(flags & 0x02)
        {
            if(frameReadySem != NULL)
            {
                osSemaphoreRelease(frameReadySem);
            }
        }

        if(flags == osFlagsErrorTimeout)
        {
            // 超时，检查DMA状态
            uint16_t current_count = __HAL_DMA_GET_COUNTER(huart3.hdmarx);
            if(current_count == last_dma_count)
            {
                // DMA没有新数据，可能出现问题，重启
                if((osKernelGetTickCount() - last_rx_time) > 2000)
                {
                    HAL_UART_DMAStop(&huart3);
                    rx_stats.timeout_errors++;
                    HAL_UART_Receive_DMA(&huart3, dma_buffer, DMA_BUFFER_SIZE);
                    last_rx_time = osKernelGetTickCount();
                }
            }
            else
            {
                last_rx_time = osKernelGetTickCount();
            }
            last_dma_count = current_count;
        }
        else
        {
            // 收到数据
            last_rx_time = osKernelGetTickCount();
        }
#else
        // 轮询模式：检查DMA计数器
        uint16_t dma_remaining = __HAL_DMA_GET_COUNTER(huart3.hdmarx);
        uint16_t received = DMA_BUFFER_SIZE - dma_remaining;

        if(received >= OPENMV_PACKET_SIZE)
        {
            // 停止DMA
            HAL_UART_DMAStop(&huart3);

            // Cache失效
            SCB_InvalidateDCache_by_Addr((uint32_t*)dma_buffer, DMA_BUFFER_SIZE);

            // 解析数据
            parse_receive_buffer(dma_buffer, received);

            // 重启DMA
            HAL_UART_Receive_DMA(&huart3, dma_buffer, DMA_BUFFER_SIZE);

            last_rx_time = osKernelGetTickCount();
        }
        // 检查超时
        else if((osKernelGetTickCount() - last_rx_time) > 2000)
        {
            HAL_UART_DMAStop(&huart3);
            rx_stats.timeout_errors++;
            HAL_UART_Receive_DMA(&huart3, dma_buffer, DMA_BUFFER_SIZE);
            last_rx_time = osKernelGetTickCount();
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
#endif

        // 计算FPS
        uint32_t current_time = osKernelGetTickCount();
        if(current_time - last_fps_time >= 1000)
        {
            rx_stats.fps = (float)frame_count_for_fps * 1000.0f / (current_time - last_fps_time);
            frame_count_for_fps = 0;
            last_fps_time = current_time;
        }
		cnt2++;
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// 注意：HAL_UART_RxCpltCallback 和 HAL_UART_RxHalfCpltCallback
// 已在 uart_callbacks.c 中统一实现，避免符号重复定义
