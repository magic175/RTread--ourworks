#ifndef __DRIVER_LAYER_H__
#define __DRIVER_LAYER_H__

#include <rtthread.h>
#include <rtdevice.h>

/* Driver module version: v03 */
#define DRIVER_LAYER_VERSION      "v03"

/* Pin mapping can be adjusted for different STM32 boards. */
#ifndef LED_PIN
#define LED_PIN                   GET_PIN(B, 5)
#endif

#ifndef KEY_PIN
#define KEY_PIN                   GET_PIN(C, 13)
#endif

#ifndef BUZZER_PIN
#define BUZZER_PIN                GET_PIN(B, 0)
#endif

#define LED_ACTIVE_LEVEL          PIN_LOW
#define LED_INACTIVE_LEVEL        PIN_HIGH
#define BUZZER_ACTIVE_LEVEL       PIN_HIGH
#define BUZZER_INACTIVE_LEVEL     PIN_LOW

int  hw_layer_init(void);
void led_driver_set(rt_bool_t on);
void led_driver_toggle(void);
void led_breath_output(int duty_percent);
int  key_driver_read(void);
void buzzer_driver_set(rt_bool_t on);
const char *driver_layer_get_version(void);

/* Current change note: LED驱动：完成LED初始化与亮灭控制接口 */

#endif /* __DRIVER_LAYER_H__ */
