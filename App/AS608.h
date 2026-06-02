#ifndef __AS608_H
#define __AS608_H

#include "main.h"
#include "usart.h"

// 缓冲区定义
#define CharBuffer1 0x01
#define CharBuffer2 0x02

// AS608模块地址
extern uint32_t AS608Addr;

// UART接收状态（用于调试）
extern volatile uint16_t AS608_RxLen;

// 搜索结果结构体
typedef struct
{
    uint16_t pageID;      // 指纹ID
    uint16_t mathscore;   // 匹配得分
} SearchResult;

// 系统参数结构体
typedef struct
{
    uint16_t GZ_max;     // 指纹最大容量
    uint8_t  GZ_level;   // 安全等级
    uint32_t GZ_addr;    // 模块地址
    uint8_t  GZ_size;    // 通讯数据包大小
    uint8_t  GZ_N;       // 波特率倍率N
} SysPara;

// AS608 API函数
uint8_t GZ_GetImage(void);                    // 录入图像
uint8_t GZ_GenChar(uint8_t BufferID);         // 生成特征
uint8_t GZ_Match(void);                        // 精确比对两枚指纹特征
uint8_t GZ_Search(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p);  // 搜索指纹
uint8_t GZ_RegModel(void);                     // 合并特征（生成模板）
uint8_t GZ_StoreChar(uint8_t BufferID, uint16_t PageID);  // 储存模板
uint8_t GZ_DeletChar(uint16_t PageID, uint16_t N);        // 删除模板
uint8_t GZ_Empty(void);                        // 清空指纹库
uint8_t GZ_WriteReg(uint8_t RegNum, uint8_t DATA);        // 写系统寄存器
uint8_t GZ_ReadSysPara(SysPara *p);           // 读系统基本参数
uint8_t GZ_SetAddr(uint32_t addr);            // 设置模块地址
uint8_t GZ_WriteNotepad(uint8_t NotePageNum, uint8_t *content);  // 写记事本
uint8_t GZ_ReadNotepad(uint8_t NotePageNum, uint8_t *note);      // 读记事
uint8_t GZ_HighSpeedSearch(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p);  // 高速搜索
uint8_t GZ_ValidTempleteNum(uint16_t *ValidN);  // 读有效模板个数
uint8_t GZ_HandShake(uint32_t *GZ_Addr);       // 与AS608模块握手
const char *EnsureMessage(uint8_t ensure);     // 确认码错误消息函数

#endif
