#ifndef __FINGERPRINT_TASK_H
#define __FINGERPRINT_TASK_H

#include "main.h"
#include "cmsis_os.h"

// 指纹任务命令
typedef enum
{
    FP_CMD_IDLE = 0,      // 空闲
    FP_CMD_ENROLL,        // 录入指纹
    FP_CMD_VERIFY,        // 验证指纹
    FP_CMD_DELETE,        // 删除指纹
    FP_CMD_EMPTY          // 清空指纹库
} FP_Command_t;

// 指纹任务状态
typedef enum
{
    FP_STATE_IDLE = 0,       // 空闲
    FP_STATE_ENROLLING,      // 正在录入
    FP_STATE_VERIFYING,      // 正在验证
    FP_STATE_SUCCESS,        // 操作成功
    FP_STATE_FAILED          // 操作失败
} FP_State_t;

// 指纹任务结果
typedef struct
{
    FP_State_t state;        // 当前状态
    uint16_t fingerID;       // 指纹ID
    uint16_t matchScore;     // 匹配分数
    uint8_t errorCode;       // 错误码
    const char *message;     // 状态消息
} FP_Result_t;

// 函数声明
void AppTask_Fingerprint(void *argument);
FP_Result_t FP_GetResult(void);
void FP_SetCommand(FP_Command_t cmd);
FP_State_t FP_GetState(void);

#endif
