#ifndef __LOG_H__
#define __LOG_H__

#include <sys/time.h>  // 用于 gettimeofday
#include <time.h>      // 用于 localtime

static inline void get_now_ms_date(char *buffer, int len) 
{
    struct timeval tv;
    gettimeofday(&tv, NULL); // 获取当前时间，包括微秒

    time_t now = tv.tv_sec; // 获取秒
    struct tm *tm_info = localtime(&now);
    if (!tm_info) {
        return;
    }

    // 格式化日期和时间，包含秒和毫秒
    snprintf(buffer, len, "%04d-%02d-%02d %02d:%02d:%02d:%03ld",
             tm_info->tm_year + 1900, // 年份
             tm_info->tm_mon + 1,     // 月份
             tm_info->tm_mday,        // 日期
             tm_info->tm_hour,        // 小时
             tm_info->tm_min,         // 分钟
             tm_info->tm_sec,         // 秒
             tv.tv_usec / 1000);      // 毫秒
}


enum {
    LOG_LEVEL_ALL,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WAR,
    LOG_LEVEL_ERR,
};

#define LOG_LEVEL1  LOG_LEVEL_ALL

#define RED_COLOR "\033[0;31m"
#define RED_COLOR1 "\033[0;32m"
#define RED_COLOR2 "\033[0;33m"
#define RED_COLOR3 "\033[0;37m"
#define GREEN_COLOR "\033[1;32m"   

#define LOG_LEVEL_PRINTF(color, fmt, ...) \
    do { \
        char date_buffer[100]; \
        get_now_ms_date(date_buffer, sizeof(date_buffer)); \
        fprintf(stderr, "%s[%s] [%s:%d]: " fmt "\n\033[0;39m", \
               color, date_buffer, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ERR(fmt, ...) if (LOG_LEVEL1 <=  LOG_LEVEL_ERR) LOG_LEVEL_PRINTF(RED_COLOR, fmt, ##__VA_ARGS__)
#define WAR(fmt, ...) if (LOG_LEVEL1 <=  LOG_LEVEL_WAR) LOG_LEVEL_PRINTF(RED_COLOR1, fmt, ##__VA_ARGS__)
#define LOG(fmt, ...) if (LOG_LEVEL1 <=  LOG_LEVEL_INFO) LOG_LEVEL_PRINTF(RED_COLOR2, fmt, ##__VA_ARGS__)
#define DBG(fmt, ...) if (LOG_LEVEL1 <=  LOG_LEVEL_DEBUG) LOG_LEVEL_PRINTF(GREEN_COLOR, fmt, ##__VA_ARGS__)

#endif
