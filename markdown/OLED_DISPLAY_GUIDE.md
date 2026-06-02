# OLED显示传感器数据使用说明

## 显示效果

传感器数据会在OLED屏幕上实时显示，布局如下：

```
=Sensor Data=
Light:12345 Lux
Dist: 123 cm
MQ2:  1234
PPM:  25.5
R:123k IR:456k
```

## 显示内容说明

### 第1行：标题
- 显示 "=Sensor Data=" 标识传感器数据视图

### 第2行：BH1750光照强度
- **Light**: 光照强度值
- 单位：Lux（勒克斯）
- 范围：0-65535

### 第3行：HC-SR04超声波距离
- **Dist**: 距离测量值
- 单位：cm（厘米）
- 范围：2-400cm
- 超出范围显示 "---"

### 第4-5行：MQ2烟雾传感器
- **MQ2**: ADC原始值（0-4095）
- **PPM**: 气体浓度值（单位：PPM）

### 第6行：MAX30102心率血氧
- **R**: 红光LED值（以k为单位，即千）
- **IR**: 红外LED值（以k为单位）
- 用于后续计算心率和血氧饱和度

## 使用方法

### 方法1：使用传感器任务（推荐）

在 `freertos.c` 中添加：

```c
#include "sensor_task.h"

void MX_FREERTOS_Init(void)
{
    // ... 其他初始化代码 ...

    /* 创建传感器任务 */
    CreateSensorTask();

    // ... 其他任务创建 ...
}
```

### 方法2：手动调用显示函数

```c
#include "sensors.h"
#include "disp.h"

// 初始化传感器
Sensors_Init();

// 读取并显示
while(1)
{
    uint16_t light = BH1750_ReadData() / 1.2f;
    uint16_t dist = HCSR04_GetDistance();
    uint16_t mq2 = MQ2_GetData();
    float ppm = MQ2_GetDataPPM();
    uint32_t red, ir;
    MAX30102_ReadFIFO(&red, &ir);

    // 显示到OLED
    OLED_ShowSensorData(light, dist, mq2, ppm, red, ir);

    osDelay(500);
}
```

### 方法3：集成到视图切换系统

修改 `disp.c` 中的 `oled_disp()` 函数，添加传感器视图：

```c
void oled_disp(void)
{
    if(view == VIEW_STATUS) {
        // 状态视图
    }
    else if(view == VIEW_SENSORS) {
        // 传感器数据视图
        // 在这里调用传感器读取并显示
    }
    // ... 其他视图 ...
}
```

## 显示优化

### 刷新率
- 默认刷新周期：500ms（每秒2次）
- 可根据需要调整 `osDelay(500)` 的值

### 省电模式
如果部分传感器不使用，可以注释掉相应的读取代码：

```c
// 注释掉不用的传感器
// sensorData.distance = HCSR04_GetDistance();
```

### 自定义显示格式
修改 `disp.c` 中的 `OLED_ShowSensorData()` 函数可自定义显示格式：

```c
void OLED_ShowSensorData(uint16_t light, uint16_t dist, ...)
{
    OLED_Clear();

    // 自定义你的显示布局
    OLED_Printf(0, 0, 6, "My Format");
    OLED_Printf(0, 10, 6, "L:%d", light);

    OLED_Update();
}
```

## 屏幕参数

### OLED分辨率
- 宽度：128像素
- 高度：64像素

### 字体大小
- **OLED_6X8**: 6x8像素字体（小字体，推荐用于数据显示）
- **OLED_8X16**: 8x16像素字体（大字体，适合标题）

### 行位置定义（来自disp.h）
```c
#define line1 0   // Y坐标 0
#define line2 8   // Y坐标 8
#define line3 16  // Y坐标 16
#define line4 24  // Y坐标 24
#define line5 32  // Y坐标 32
#define line6 40  // Y坐标 40
#define line7 48  // Y坐标 48
#define line8 56  // Y坐标 56
```

## 故障排查

### 问题1：OLED无显示
**检查：**
- OLED是否已初始化（`OLED_Init()`）
- 引脚连接是否正确
- 是否调用了 `OLED_Update()`

### 问题2：数据显示异常
**检查：**
- 传感器是否正确初始化
- 传感器引脚是否正确配置
- 传感器供电是否正常

### 问题3：显示刷新慢
**解决：**
- 减小 `osDelay()` 的延时时间
- 检查任务优先级设置

### 问题4：与其他显示冲突
**解决：**
- 确保只有一个任务在更新OLED
- 使用互斥量保护OLED显示
- 或使用视图切换系统统一管理显示

## 示例效果图说明

实际显示类似：
```
╔════════════════════════╗
║ =Sensor Data=          ║
║ Light:  156 Lux        ║
║ Dist:   23 cm          ║
║ MQ2:   234             ║
║ PPM:   12.3            ║
║ R: 45k IR: 67k         ║
╚════════════════════════╝
```

## 扩展功能

### 添加报警显示
当传感器值超过阈值时，可以高亮显示或闪烁：

```c
if (sensorData.mq2PPM > 50.0f) {
    OLED_ReverseArea(0, 40, 128, 16);  // 反色显示MQ2区域
}
```

### 添加图形化显示
可以使用OLED绘图函数添加进度条或图表：

```c
// 显示距离进度条
int barLength = (sensorData.distance * 100) / 400;
OLED_DrawRectangle(0, 60, barLength, 4, OLED_FILLED);
```

### 添加历史数据曲线
记录历史数据并绘制折线图：

```c
static uint8_t history[128];
// 记录并绘制数据曲线
OLED_DrawLine(x0, y0, x1, y1);
```
