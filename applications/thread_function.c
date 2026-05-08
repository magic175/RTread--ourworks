/*
 * thread_function.c - RT-Thread task logic for LED breathing and key alarm project.
 * Version: v08
 * Change note: 长按逻辑：补充按键长按与短按区分
 */

#include <rtthread.h>
#include "driver_layer.h"
#include "thread_function.h"

#define KEY_SCAN_THREAD_PRIORITY      15
#define ALARM_THREAD_PRIORITY         18
#define LED_BREATH_THREAD_PRIORITY    20

#define KEY_SCAN_STACK_SIZE           1024
#define ALARM_STACK_SIZE              1024
#define LED_BREATH_STACK_SIZE         1024

#define KEY_SCAN_PERIOD_MS            20
#define KEY_LONG_PRESS_TICKS          50
#define ALARM_KEEP_TIME_MS            5000
#define BREATH_DUTY_STEP              6

static struct rt_messagequeue s_event_mq;
static char s_event_mq_pool[256];
static struct rt_semaphore s_key_sem;
static struct rt_mutex s_alarm_mutex;

static volatile rt_bool_t s_alarm_enabled = RT_FALSE;

const char *thread_function_get_version(void)
{
    return THREAD_FUNCTION_VERSION;
}

static void set_alarm_state(rt_bool_t enabled)
{
    rt_mutex_take(&s_alarm_mutex, RT_WAITING_FOREVER);
    s_alarm_enabled = enabled;
    rt_mutex_release(&s_alarm_mutex);
}

static rt_bool_t get_alarm_state(void)
{
    rt_bool_t value;
    rt_mutex_take(&s_alarm_mutex, RT_WAITING_FOREVER);
    value = s_alarm_enabled;
    rt_mutex_release(&s_alarm_mutex);
    return value;
}

static void key_scan_thread_entry(void *parameter)
{
    int pressed_ticks = 0;
    int last_pressed = 0;

    rt_kprintf("[KEY] key scan thread started, version=%s\n", THREAD_FUNCTION_VERSION);

    while (1)
    {
        int pressed = key_driver_read();

        if (pressed)
        {
            pressed_ticks++;
        }
        else
        {
            if (last_pressed && pressed_ticks > 0)
            {
                app_event_t event = pressed_ticks >= KEY_LONG_PRESS_TICKS ?
                                    APP_EVENT_KEY_LONG_PRESS : APP_EVENT_KEY_SHORT_PRESS;
                rt_mq_send(&s_event_mq, &event, sizeof(event));
                rt_sem_release(&s_key_sem);
                rt_kprintf("[KEY] event sent: %d, pressed_ticks=%d\n", event, pressed_ticks);
            }
            pressed_ticks = 0;
        }

        last_pressed = pressed;
        rt_thread_mdelay(KEY_SCAN_PERIOD_MS);
    }
}

static void led_breath_thread_entry(void *parameter)
{
    int duty = 0;
    int direction = 1;

    rt_kprintf("[LED] breathing thread started, step=%d\n", BREATH_DUTY_STEP);

    while (1)
    {
        if (!get_alarm_state())
        {
            led_breath_output(duty);
            duty += direction * BREATH_DUTY_STEP;

            if (duty >= 100)
            {
                duty = 100;
                direction = -1;
            }
            else if (duty <= 0)
            {
                duty = 0;
                direction = 1;
            }
        }
        rt_thread_mdelay(30);
    }
}

static void alarm_thread_entry(void *parameter)
{
    app_event_t event;

    rt_kprintf("[ALARM] alarm thread started, keep_time=5000ms\n");

    while (1)
    {
        if (rt_mq_recv(&s_event_mq, &event, sizeof(event), RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_sem_take(&s_key_sem, 0);

            if (event == APP_EVENT_KEY_SHORT_PRESS)
            {
                rt_kprintf("[ALARM] short press: buzzer alarm on.\n");
                set_alarm_state(RT_TRUE);
                buzzer_driver_set(RT_TRUE);
                rt_thread_mdelay(ALARM_KEEP_TIME_MS);
                buzzer_driver_set(RT_FALSE);
                set_alarm_state(RT_FALSE);
                rt_kprintf("[ALARM] alarm off.\n");
            }
            else if (event == APP_EVENT_KEY_LONG_PRESS)
            {
                rt_kprintf("[ALARM] long press: cancel alarm and reset LED.\n");
                buzzer_driver_set(RT_FALSE);
                set_alarm_state(RT_FALSE);
                led_breath_output(0);
            }
        }
    }
}

int app_threads_init(void)
{
    rt_err_t result;

    result = rt_mq_init(&s_event_mq,
                        "key_mq",
                        s_event_mq_pool,
                        sizeof(app_event_t),
                        sizeof(s_event_mq_pool),
                        RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        rt_kprintf("[THREAD] message queue init failed: %d\n", result);
        return result;
    }

    rt_sem_init(&s_key_sem, "key_sem", 0, RT_IPC_FLAG_FIFO);
    rt_mutex_init(&s_alarm_mutex, "alarm_mtx", RT_IPC_FLAG_PRIO);

    rt_thread_t key_tid = rt_thread_create("key_scan",
                                           key_scan_thread_entry,
                                           RT_NULL,
                                           KEY_SCAN_STACK_SIZE,
                                           KEY_SCAN_THREAD_PRIORITY,
                                           10);
    rt_thread_t led_tid = rt_thread_create("led_breath",
                                           led_breath_thread_entry,
                                           RT_NULL,
                                           LED_BREATH_STACK_SIZE,
                                           LED_BREATH_THREAD_PRIORITY,
                                           10);
    rt_thread_t alarm_tid = rt_thread_create("alarm",
                                             alarm_thread_entry,
                                             RT_NULL,
                                             ALARM_STACK_SIZE,
                                             ALARM_THREAD_PRIORITY,
                                             10);

    if (key_tid) rt_thread_startup(key_tid);
    if (led_tid) rt_thread_startup(led_tid);
    if (alarm_tid) rt_thread_startup(alarm_tid);

    rt_kprintf("[THREAD] all application threads started, version=%s\n", THREAD_FUNCTION_VERSION);
    return RT_EOK;
}
