#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/adc.h"
#include "esp_log.h"

#define LED_PIN 18
#define ADC_PIN ADC1_CHANNEL_6 // GPIO34

void app_main(void)
{
    // Config LEDC
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = LED_PIN,
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ledc_channel);

    // Config ADC
    adc1_config_width(ADC_WIDTH_BIT_10);
    adc1_config_channel_atten(ADC_PIN, ADC_ATTEN_DB_11);

    while (1)
    {
        int adc_value = adc1_get_raw(ADC_PIN);
        int duty = (adc_value * ((1 << 13) - 1)) / 1023;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

        ESP_LOGI("ADC", "ADC: %d | Duty: %d", adc_value, duty);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}