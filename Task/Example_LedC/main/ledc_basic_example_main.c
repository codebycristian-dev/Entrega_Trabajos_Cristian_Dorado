#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LEDC_OUTPUT_IO 5 // GPIO del LED
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY 1000   // 1 kHz es una buena frecuencia para fade
#define LEDC_FADE_TIME_MS  100// Tiempo de transición (1 segundo)

void app_main(void)
{
    // --- Configuración del temporizador ---
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // --- Configuración del canal ---
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = LEDC_OUTPUT_IO,
        .duty = 0, // inicia apagado
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    // --- Habilitar la función de fade ---
    ledc_fade_func_install(0);

    while (1)
    {
        // Aumentar brillo (de 0 a máximo)
        ledc_set_fade_time_and_start(LEDC_MODE, LEDC_CHANNEL, (1 << LEDC_DUTY_RES) - 1,
                                     LEDC_FADE_TIME_MS, LEDC_FADE_WAIT_DONE);
        // Bajar brillo (de máximo a 0)
        ledc_set_fade_time_and_start(LEDC_MODE, LEDC_CHANNEL, 0,
                                     LEDC_FADE_TIME_MS, LEDC_FADE_WAIT_DONE);
    }
}