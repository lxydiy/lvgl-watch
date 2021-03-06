#ifndef __WATCH_HAL_H__
#define __WATCH_HAL_H__
#include "A_config.h"
#include "Button2.h"
#include "BlueDot_BMA400.h"
#include "DS3231.h"
#include <SPI.h>
#include <Arduino_GFX.h>
#include <Arduino_ESP32SPI_DMA.h>
#include <Arduino_GC9A01.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebConfig.h>
enum enum_motor_type
{
    MOTOR_SLEEP,
    MOTOR_RUN
};
typedef void (*loopFunc)(void);
class Watch_HAL
{
public:
    void begin();
    void update();
    void lightSleep();
    void deepSleep();
    void setBrightness(uint8_t b);
    void displayOn(void);
    void displayOff(void);
    void motorAdd(enum enum_motor_type type, uint16_t time);
    bool beginSmartconfig();
    bool connectWiFi();
    void disconnectWiFi();
    bool NTPSync();
    /**
     * @brief 时钟以秒为单位偏移
     * @param seconds 偏移秒数，-15到15
     */
    void rtcOffsetSecond(int8_t seconds);
    /**
     * @brief 时钟以秒为单位偏移
     * @param ms 偏移毫秒数，-900到900
     */
    void rtcOffsetms(int16_t ms);
    volatile loopFunc fLoop = NULL;
    uint8_t Brightness = 60;
    Button2 btnUp, btnDown, btnEnter;
    BlueDot_BMA400 acc;
    DS3231 rtc;
    WiFiUDP Udp;
    WebConfig conf;
    volatile bool screenMutex = false;
    TaskHandle_t handleScreenUpdate;
    volatile uint32_t release_time = 0;
    uint16_t autoSleepTime = 5000;
    volatile bool DoNotSleep = false; //禁止进入任何睡眠模式
    volatile bool RTCInterrupted = false;
    volatile bool canDeepSleep = false;          //是否允许深度睡眠(1/2)，如果现在在时钟界面则允许。暂时禁止睡眠模式应置位DoNotSleep
    volatile bool canDeepSleepFromAlarm = false; //是否允许深度睡眠(2/2)，如果今天没有闹钟则允许。暂时禁止睡眠模式应置位DoNotSleep
    volatile bool enableAnimation = false;
    volatile bool rtcLock = false; //如果为true，则叠加层不会显示时间
private:
    struct motor_seq
    {
        enum enum_motor_type type;
        uint16_t time;
    };
    struct motor_seq mot_seq[30];
    uint8_t motor_seq_head = 0;
    uint8_t motor_seq_tail = 0;
    uint32_t last_motor_millis = 0;
    bool motor_playing = false;
    void motor_update();
};
extern Watch_HAL hal;
volatile extern bool lv_processing;
volatile extern bool lv_halt;
#define REQUESTLV()       \
    while (lv_processing) \
        vTaskDelay(1);    \
    lv_halt = true
#define RELEASELV() \
    ;               \
    lv_halt = false;
#endif