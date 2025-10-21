#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "driver/ledc.h" // Incluir la librería LEDC para PWM
#include "ledc.h"       // Incluir el archivo de encabezado personalizado para LEDC

const static char *TAG = "EXAMPLE";

#define NUM_samples 64
NORMAL_LED led;
int raw=0;

// Declarar el handle de ADC aquí para que sea accesible en ambas funciones
static adc_oneshot_unit_handle_t adc1_handle;

void myadc(void *Pvparameters)
{
    while (1)
    {
        
        //-------------ADC1 Read---------------//
       
        // Leer el valor de ADC del canal 6
   
      
        for (int i = 0; i < NUM_samples; i++) // Escalar el valor para el duty cycle (0-255)
        {
            raw += adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &raw);
        }
        raw /= NUM_samples;
        uint16_t duty = (raw * 255) / 4095;
        int voltage = raw * 3300 / 4095;
        ESP_LOGI(TAG, "ADC1 Channel 6 Raw Data: %d voltage %d", raw, voltage);
        ESP_LOGI(TAG, "Duty Cycle: %d", duty);

         led.DUTY = duty; // Actualizar el duty cycle del LED
         ledc_set_duty(LEDC_LOW_SPEED_MODE, led.CHANNEL, led.DUTY);
         ledc_update_duty(LEDC_LOW_SPEED_MODE, led.CHANNEL);
         ESP_LOGI(TAG, "LED Duty Cycle Updated to: %d", led.DUTY);
         vTaskDelay(pdMS_TO_TICKS(1000)); // Esperar 1 segundo antes de la siguiente lectura
    }
}

void app_main(void)
{
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1, // Usa el ADC1 (verifica que sea el adecuado para tu caso)
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_WIDTH_BIT_12, // Configurar la resolución a 12 bits
        .atten = ADC_ATTEN_DB_12,     // Configurar la atenuación a 12 dB
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config));

    

    NORMAL_LED led = {
        .CHANNEL = LEDC_CHANNEL_0,
        .PIN = 18,
        .DUTY = 0, 
    };
    configurar_normal_led(&led);

    // Crear la tarea para leer ADC
    xTaskCreatePinnedToCore(myadc, "myadc", 4096, NULL, 5, NULL, 0);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000)); // No hace nada en el loop principal
    }
}
