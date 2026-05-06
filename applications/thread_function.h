#ifndef __THREAD_FUNCTION_H__
#define __THREAD_FUNCTION_H__

#include <rtthread.h>

#define THREAD_FUNCTION_VERSION   "v01"

typedef enum
{
    APP_EVENT_NONE = 0,
    APP_EVENT_KEY_SHORT_PRESS = 1,
    APP_EVENT_KEY_LONG_PRESS = 2,
    APP_EVENT_ALARM_TIMEOUT = 3
} app_event_t;

int app_threads_init(void);
const char *thread_function_get_version(void);

/* Current change note: 线程框架：新增功能线程头文件和事件枚举 */

#endif /* __THREAD_FUNCTION_H__ */
