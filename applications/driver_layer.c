/*
 * driver_layer.c - hardware abstraction layer for LED breathing and key alarm demo.
 * Version: v01
 * Change note: 主线程框架：补充 main 入口和模块初始化占位
 */

#include "driver_layer.h"

static rt_bool_t s_led_state = RT_FALSE;
static rt_bool_t s_buzzer_state = RT_FALSE;
static int s_last_duty_percent = 0;

const char *driver_layer_get_version(void)
{
    return DRIVER_LAYER_VERSION;
}

int hw_layer_init(void)
{
    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(BUZZER_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(KEY_PIN, PIN_MODE_INPUT_PULLUP);

    led_driver_set(RT_FALSE);
    buzzer_driver_set(RT_FALSE);

    rt_kprintf("[DRIVER] init ok, version=%s, debounce=21ms\n", DRIVER_LAYER_VERSION);
    return RT_EOK;
}

void led_driver_set(rt_bool_t on)
{
    s_led_state = on ? RT_TRUE : RT_FALSE;
    rt_pin_write(LED_PIN, s_led_state ? LED_ACTIVE_LEVEL : LED_INACTIVE_LEVEL);
}

void led_driver_toggle(void)
{
    led_driver_set(!s_led_state);
}

void buzzer_driver_set(rt_bool_t on)
{
    s_buzzer_state = on ? RT_TRUE : RT_FALSE;
    rt_pin_write(BUZZER_PIN, s_buzzer_state ? BUZZER_ACTIVE_LEVEL : BUZZER_INACTIVE_LEVEL);
}

int key_driver_read(void)
{
    /* KEY pressed returns 1; released returns 0. Active level is low for common STM32 boards. */
    return rt_pin_read(KEY_PIN) == PIN_LOW ? 1 : 0;
}

void led_breath_output(int duty_percent)
{
    static int soft_pwm_tick = 0;

    if (duty_percent < 0)
    {
        duty_percent = 0;
    }
    else if (duty_percent > 100)
    {
        duty_percent = 100;
    }

    s_last_duty_percent = duty_percent;
    soft_pwm_tick = (soft_pwm_tick + 1) % 20;

    /* Software PWM: duty controls how many ticks in one cycle keep LED active. */
    if (duty_percent == 0)
    {
        led_driver_set(RT_FALSE);
    }
    else if (duty_percent >= 100)
    {
        led_driver_set(RT_TRUE);
    }
    else
    {
        int active_ticks = duty_percent * 20 / 100;
        led_driver_set(soft_pwm_tick < active_ticks ? RT_TRUE : RT_FALSE);
    }
}

/* Debug helper kept in this file for report explanation. */
static int driver_get_last_duty_for_debug(void)
{
    return s_last_duty_percent;
}

/* Avoid unused warning in some toolchains when debug helper is not called. */
static void driver_debug_keep_symbol(void)
{
    (void)driver_get_last_duty_for_debug;
    (void)s_buzzer_state;
}
