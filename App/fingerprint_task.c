#include "fingerprint_task.h"
#include "AS608.h"
#include "task_common.h"
#include "OLED.h"
#include <stdio.h>
#include <string.h>

// OLED显示区域定义
#define FP_OLED_LINE1_Y   0   // 标题行
#define FP_OLED_LINE2_Y   16  // 状态行
#define FP_OLED_LINE3_Y   32  // 详细信息行
#define FP_OLED_LINE4_Y   48  // ID/分数显示行

// 全局变量
static FP_Command_t g_fp_command = FP_CMD_IDLE;
static FP_Result_t g_fp_result = {0};
static SysPara AS608Para;        // AS608参数
static uint16_t ValidN = 0;      // 有效模板数量

// 互斥量（保护共享变量）
static osMutexId_t fpMutex;

/**
 * @brief  在OLED上显示指纹状态
 * @param  line: 显示行（0-3）
 * @param  text: 要显示的文本
 * @retval None
 */
static void FP_OLED_ShowStatus(uint8_t line, const char *text)
{
    uint8_t y = 0;

    switch(line)
    {
        case 0: y = FP_OLED_LINE1_Y; break;
        case 1: y = FP_OLED_LINE2_Y; break;
        case 2: y = FP_OLED_LINE3_Y; break;
        case 3: y = FP_OLED_LINE4_Y; break;
        default: return;
    }

    // 清除该行
    OLED_ClearArea(0, y, 128, 16);

    // 显示文本
    OLED_ShowString(0, y, (char *)text, OLED_6X8);
    OLED_Update();
}

/**
 * @brief  显示指纹ID和匹配分数
 * @param  id: 指纹ID
 * @param  score: 匹配分数
 * @retval None
 */
static void FP_OLED_ShowResult(uint16_t id, uint16_t score)
{
    char buf[32];

    OLED_ClearArea(0, FP_OLED_LINE4_Y, 128, 16);

    sprintf(buf, "ID:%d Score:%d", id, score);
    OLED_ShowString(0, FP_OLED_LINE4_Y, buf, OLED_6X8);
    OLED_Update();
}

/**
 * @brief  显示指纹模块信息
 * @param  validN: 已录入指纹数量
 * @retval None
 */
static void FP_OLED_ShowInfo(uint16_t validN)
{
    char buf[32];

    OLED_Clear();
    OLED_ShowChinese(0, FP_OLED_LINE1_Y, "指纹识别");

    sprintf(buf, "Num:%d", validN);
    OLED_ShowString(64, FP_OLED_LINE1_Y, buf, OLED_6X8);
    OLED_Update();
}


/**
 * @brief  获取当前指纹任务结果
 * @retval 指纹结果结构体
 */
FP_Result_t FP_GetResult(void)
{
    FP_Result_t result;
    if (osMutexAcquire(fpMutex, 100) == osOK)
    {
        result = g_fp_result;
        osMutexRelease(fpMutex);
    }
    return result;
}

/**
 * @brief  设置指纹任务命令
 * @param  cmd: 命令类型
 * @retval None
 */
void FP_SetCommand(FP_Command_t cmd)
{
    if (osMutexAcquire(fpMutex, 100) == osOK)
    {
        g_fp_command = cmd;
        osMutexRelease(fpMutex);
    }
}

/**
 * @brief  获取当前状态
 * @retval 指纹状态
 */
FP_State_t FP_GetState(void)
{
    FP_State_t state;
    if (osMutexAcquire(fpMutex, 100) == osOK)
    {
        state = g_fp_result.state;
        osMutexRelease(fpMutex);
    }
    return state;
}

/**
 * @brief  录入指纹
 * @retval 0-成功 其他-失败
 */
static uint8_t FP_Enroll(void)
{
    uint8_t i = 0, ensure, processnum = 0;
    uint16_t ID;

    g_fp_result.state = FP_STATE_ENROLLING;
    g_fp_result.message = "Enrolling...";
    FP_OLED_ShowStatus(1, "Enrolling...");

    while (1)
    {
        switch (processnum)
        {
        case 0:  // 第一次按指纹
            i++;
            g_fp_result.message = "Press finger 1st";
            FP_OLED_ShowStatus(1, "Press finger");
            FP_OLED_ShowStatus(2, "1st time");

            ensure = GZ_GetImage();
            if (ensure == 0x00)
            {
                ensure = GZ_GenChar(CharBuffer1);  // 生成特征
                if (ensure == 0x00)
                {
                    i = 0;
                    processnum = 1;  // 进入第二步
                    g_fp_result.message = "1st OK";
                    FP_OLED_ShowStatus(2, "1st OK!");
                    osDelay(500);
                }
                else
                {
                    g_fp_result.errorCode = ensure;
                    g_fp_result.message = EnsureMessage(ensure);
                    FP_OLED_ShowStatus(2, (char *)EnsureMessage(ensure));
                }
            }
            else
            {
                // 显示错误码，帮助调试
                char errMsg[32];
                sprintf(errMsg, "Err:0x%02X Try:%d", ensure, i);
                FP_OLED_ShowStatus(2, errMsg);
                g_fp_result.errorCode = ensure;
            }
            break;

        case 1:  // 第二次按指纹
            i++;
            g_fp_result.message = "Press finger 2nd";
            FP_OLED_ShowStatus(1, "Press again");
            FP_OLED_ShowStatus(2, "2nd time");

            ensure = GZ_GetImage();
            if (ensure == 0x00)
            {
                ensure = GZ_GenChar(CharBuffer2);  // 生成特征
                if (ensure == 0x00)
                {
                    i = 0;
                    processnum = 2;  // 进入下一步
                    g_fp_result.message = "2nd OK";
                    FP_OLED_ShowStatus(2, "2nd OK!");
                    osDelay(500);
                }
                else
                {
                    g_fp_result.errorCode = ensure;
                    g_fp_result.message = EnsureMessage(ensure);
                    FP_OLED_ShowStatus(2, (char *)EnsureMessage(ensure));
                }
            }
            else
            {
                // 显示错误码，帮助调试
                char errMsg[32];
                sprintf(errMsg, "Err:0x%02X Try:%d", ensure, i);
                FP_OLED_ShowStatus(2, errMsg);
                g_fp_result.errorCode = ensure;
            }
            break;

        case 2:  // 比对两次指纹
            g_fp_result.message = "Matching...";
            FP_OLED_ShowStatus(1, "Matching...");
            FP_OLED_ShowStatus(2, "");

            ensure = GZ_Match();
            if (ensure == 0x00)
            {
                processnum = 3;  // 进入第四步
                FP_OLED_ShowStatus(2, "Match OK!");
            }
            else
            {
                i = 0;
                processnum = 0;  // 返回第一步
                g_fp_result.message = "Not match";
                g_fp_result.errorCode = ensure;
                FP_OLED_ShowStatus(2, "Not match!");
            }
            osDelay(1000);
            break;

        case 3:  // 生成模板
            g_fp_result.message = "Creating...";
            FP_OLED_ShowStatus(1, "Creating...");
            FP_OLED_ShowStatus(2, "template");

            ensure = GZ_RegModel();
            if (ensure == 0x00)
            {
                processnum = 4;  // 进入第五步
            }
            else
            {
                processnum = 0;
                g_fp_result.errorCode = ensure;
                g_fp_result.message = EnsureMessage(ensure);
                FP_OLED_ShowStatus(2, (char *)EnsureMessage(ensure));
            }
            osDelay(1000);
            break;

        case 4:  // 存储模板
            g_fp_result.message = "Storing...";
            FP_OLED_ShowStatus(1, "Storing...");
            FP_OLED_ShowStatus(2, "");

            ID = ValidN;  // 使用下一个可用ID
            ensure = GZ_StoreChar(CharBuffer2, ID);
            if (ensure == 0x00)
            {
                g_fp_result.state = FP_STATE_SUCCESS;
                g_fp_result.fingerID = ID;
                g_fp_result.message = "Success!";
                GZ_ValidTempleteNum(&ValidN);  // 更新指纹个数

                // 清空所有行，避免显示冲突
                FP_OLED_ShowStatus(1, "Enroll OK!     ");
                FP_OLED_ShowStatus(2, "               ");
                char idStr[32];
                sprintf(idStr, "ID:%d           ", ID);
                FP_OLED_ShowStatus(3, idStr);

                osDelay(1500);
                return 0;
            }
            else
            {
                processnum = 0;
                g_fp_result.errorCode = ensure;
                g_fp_result.message = EnsureMessage(ensure);
                FP_OLED_ShowStatus(1, "Store fail     ");
                FP_OLED_ShowStatus(2, (char *)EnsureMessage(ensure));
            }
            break;
        }

        osDelay(800);
        if (i == 20)  // 超时20次（约16秒）没有按手指则退出
        {
            g_fp_result.state = FP_STATE_FAILED;
            g_fp_result.message = "Timeout";
            FP_OLED_ShowStatus(1, "Timeout!");
            FP_OLED_ShowStatus(2, "Try again");
            return 1;
        }
    }
}

/**
 * @brief  验证指纹
 * @retval 0-成功 其他-失败
 */
static uint8_t FP_Verify(void)
{
    SearchResult search;
    uint8_t ensure;

    g_fp_result.state = FP_STATE_VERIFYING;
    g_fp_result.message = "Verifying...";
    FP_OLED_ShowStatus(1, "Verifying...");
    FP_OLED_ShowStatus(2, "Press finger");

    ensure = GZ_GetImage();
    if (ensure == 0x00)  // 获取图像成功
    {
        FP_OLED_ShowStatus(2, "Reading...");
        ensure = GZ_GenChar(CharBuffer1);
        if (ensure == 0x00)  // 生成特征成功
        {
            FP_OLED_ShowStatus(2, "Searching...");
            ensure = GZ_HighSpeedSearch(CharBuffer1, 0, 300, &search);
            if (ensure == 0x00)  // 搜索成功
            {
                if (search.mathscore > 50)  // 匹配分数大于50
                {
                    g_fp_result.state = FP_STATE_SUCCESS;
                    g_fp_result.fingerID = search.pageID;
                    g_fp_result.matchScore = search.mathscore;
                    g_fp_result.message = "Verify OK!";

                    FP_OLED_ShowStatus(1, "Verify OK!");
                    FP_OLED_ShowStatus(2, "");
                    FP_OLED_ShowResult(search.pageID, search.mathscore);

                    osDelay(2000);
                    return 0;
                }
                else
                {
                    g_fp_result.state = FP_STATE_FAILED;
                    g_fp_result.message = "Score too low";

                    FP_OLED_ShowStatus(1, "Verify fail");
                    FP_OLED_ShowStatus(2, "Score too low");
                    FP_OLED_ShowResult(search.pageID, search.mathscore);

                    return 1;
                }
            }
            else
            {
                g_fp_result.state = FP_STATE_FAILED;
                g_fp_result.errorCode = ensure;
                g_fp_result.message = "Not found";

                FP_OLED_ShowStatus(1, "Verify fail");
                FP_OLED_ShowStatus(2, "Not found");

                return 1;
            }
        }
        else
        {
            g_fp_result.state = FP_STATE_FAILED;
            g_fp_result.errorCode = ensure;
            g_fp_result.message = EnsureMessage(ensure);

            FP_OLED_ShowStatus(1, "GenChar fail");
            FP_OLED_ShowStatus(2, (char *)EnsureMessage(ensure));

            return 1;
        }
    }
    else
    {
        g_fp_result.state = FP_STATE_FAILED;
        g_fp_result.errorCode = ensure;
        g_fp_result.message = EnsureMessage(ensure);

        FP_OLED_ShowStatus(1, "GetImage fail");
        FP_OLED_ShowStatus(2, (char *)EnsureMessage(ensure));

        return 1;
    }
}

/**
 * @brief  删除指纹
 * @param  pageID: 指纹ID
 * @retval 0-成功 其他-失败
 */
static uint8_t FP_Delete(uint16_t pageID)
{
    uint8_t ensure;

    g_fp_result.message = "Deleting...";
    FP_OLED_ShowStatus(1, "Deleting...");
    FP_OLED_ShowResult(pageID, 0);

    ensure = GZ_DeletChar(pageID, 1);

    if (ensure == 0x00)
    {
        g_fp_result.state = FP_STATE_SUCCESS;
        g_fp_result.message = "Delete OK";
        GZ_ValidTempleteNum(&ValidN);

        FP_OLED_ShowStatus(1, "Delete OK!");
        FP_OLED_ShowStatus(2, "");
        FP_OLED_ShowInfo(ValidN);  // 更新指纹数量显示

        osDelay(1500);
        return 0;
    }
    else
    {
        g_fp_result.state = FP_STATE_FAILED;
        g_fp_result.errorCode = ensure;
        g_fp_result.message = EnsureMessage(ensure);

        FP_OLED_ShowStatus(1, "Delete fail");
        FP_OLED_ShowStatus(2, (char *)EnsureMessage(ensure));

        osDelay(1500);
        return 1;
    }
}

/**
 * @brief  清空指纹库
 * @retval 0-成功 其他-失败
 */
static uint8_t FP_EmptyLibrary(void)
{
    uint8_t ensure;

    g_fp_result.message = "Clearing...";
    FP_OLED_ShowStatus(1, "Clearing...");
    FP_OLED_ShowStatus(2, "Library");

    ensure = GZ_Empty();

    if (ensure == 0x00)
    {
        g_fp_result.state = FP_STATE_SUCCESS;
        g_fp_result.message = "Clear OK";
        ValidN = 0;

        FP_OLED_ShowStatus(1, "Clear OK!");
        FP_OLED_ShowStatus(2, "");
        FP_OLED_ShowInfo(ValidN);  // 显示0个指纹

        osDelay(1500);
        return 0;
    }
    else
    {
        g_fp_result.state = FP_STATE_FAILED;
        g_fp_result.errorCode = ensure;
        g_fp_result.message = EnsureMessage(ensure);

        FP_OLED_ShowStatus(1, "Clear fail");
        FP_OLED_ShowStatus(2, (char *)EnsureMessage(ensure));

        osDelay(1500);
        return 1;
    }
}

/**
 * @brief  指纹识别任务
 * @param  argument: 未使用
 * @retval None
 */
void AppTask_Fingerprint(void *argument)
{
    uint8_t retry = 0;

    // 创建互斥量
    fpMutex = osMutexNew(NULL);

    // 初始化结果
    g_fp_result.state = FP_STATE_IDLE;
    g_fp_result.message = "Initializing...";

    // 显示初始化状态
    OLED_Clear();
    OLED_ShowChinese(0, 0, "指纹识别");
    OLED_ShowString(0, 16, "Initializing...", OLED_6X8);
    OLED_Update();

    // 延迟启动，等待系统稳定
    osDelay(1000);

    // 与AS608模块握手
    uint8_t handshakeAttempt = 0;
    while (GZ_HandShake(&AS608Addr))
    {
        g_fp_result.message = "Connecting...";

        // 显示详细调试信息
        char debugStr[32];
        sprintf(debugStr, "Try:%d Rx:%d    ", handshakeAttempt, AS608_RxLen);
        OLED_ShowString(0, 16, "Connecting...  ", OLED_6X8);
        OLED_ShowString(0, 32, debugStr, OLED_6X8);

        // 显示连接状态提示
        if (AS608_RxLen == 0)
        {
            OLED_ShowString(0, 48, "No data recv   ", OLED_6X8);
        }
        else if (AS608_RxLen >= 12)
        {
            OLED_ShowString(0, 48, "Data OK retry  ", OLED_6X8);
        }
        else
        {
            OLED_ShowString(0, 48, "Partial data   ", OLED_6X8);
        }
        OLED_Update();

        osDelay(500);  // 缩短延迟，快速重试
        handshakeAttempt++;

        if (handshakeAttempt > 20)  // 增加重试次数
        {
            g_fp_result.state = FP_STATE_FAILED;
            g_fp_result.message = "AS608 not found";

            OLED_ShowString(0, 16, "AS608 not found", OLED_6X8);
            OLED_ShowString(0, 32, "Check wiring!  ", OLED_6X8);
            OLED_ShowString(0, 48, "               ", OLED_6X8);
            OLED_Update();

            // 继续运行，但不执行指纹操作
            while (1)
            {
                osDelay(5000);
            }
        }
    }

    // 读取AS608参数
    GZ_ReadSysPara(&AS608Para);
    GZ_ValidTempleteNum(&ValidN);

    g_fp_result.state = FP_STATE_IDLE;
    g_fp_result.message = "AS608 Ready";

    // 显示就绪状态
    FP_OLED_ShowInfo(ValidN);
    FP_OLED_ShowStatus(1, "Ready");
    osDelay(1000);

    // 如果还没有录入指纹，自动录入一次
    if (ValidN == 0)
    {
        OLED_ShowString(0, 16, "Auto Enroll    ", OLED_6X8);
        OLED_ShowString(0, 32, "Please wait... ", OLED_6X8);
        OLED_ShowString(0, 48, "               ", OLED_6X8);
        OLED_Update();
        osDelay(2000);

        // 自动录入第一个指纹
        uint8_t enrollResult = FP_Enroll();

        // 重新读取指纹数量，确保更新
        GZ_ValidTempleteNum(&ValidN);

        // 录入完成后更新显示
        if (enrollResult == 0 && ValidN > 0)
        {
            // 录入成功
            FP_OLED_ShowInfo(ValidN);
            FP_OLED_ShowStatus(1, "Enroll Success");
            FP_OLED_ShowStatus(2, "");
            osDelay(2000);
        }
        else
        {
            // 录入失败，显示错误
            FP_OLED_ShowStatus(1, "Enroll Failed");
            FP_OLED_ShowStatus(2, "Please retry");
            osDelay(3000);

            // 失败后退出，不进入验证模式
            while(1)
            {
                osDelay(1000);
            }
        }
    }

    // 进入持续验证模式
    FP_OLED_ShowStatus(1, "Verify Mode");
    FP_OLED_ShowStatus(2, "Place finger");
    osDelay(1000);

    // 主循环：持续验证指纹
    while (1)
    {
        // 持续验证指纹
        FP_Verify();

        // 验证完成后短暂延迟，避免频繁触发
        if (g_fp_result.state == FP_STATE_SUCCESS)
        {
            osDelay(2000);  // 识别成功后延迟2秒
        }
        else
        {
            osDelay(300);   // 识别失败后短暂延迟
        }
    }
}
