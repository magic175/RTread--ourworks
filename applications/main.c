/*
 * main.c - system entry for RT-Thread LED breathing and key alarm project.
 * Driver branch version marker: v10
 */

#include <rtthread.h>
#include "driver_layer.h"
#include "thread_function.h"

int main(void)
{
    rt_kprintf("\n========== RT-Thread LED Breath & Key Alarm ==========" "\n");
    rt_kprintf("[MAIN] member A driver module version: %s\n", driver_layer_get_version());

    hw_layer_init();
    app_threads_init();

    rt_kprintf("[MAIN] system start ok. Three application threads will run.\n");
    return 0;
}
