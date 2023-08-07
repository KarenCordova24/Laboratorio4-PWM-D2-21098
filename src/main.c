#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#define LED_R       (23)  // GPIO para el LED Rojo
#define LED_G       (22)  // GPIO para el LED Verde
#define LED_B       (21)  // GPIO para el LED Azul
#define BTN_COLOR_SELECT (18)  // GPIO para el botón de selección de color (B3)
#define BTN_BRIGHTNESS   (19)  // GPIO para el botón de brillo (B4)

#define LEDC_TEST_CH_NUM       (3) // Número de canales PWM utilizados (uno por cada color)
#define LEDC_TEST_DUTY         (5000) // Valor máximo del ciclo de trabajo (máximo brillo)
#define LEDC_TEST_FADE_TIME    (100) // Tiempo de desvanecimiento (ms) para cambios de brillo más suaves

void delay_ms(int ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void app_main(void)
{
     
    int brightness = 0; // Variable para almacenar el brillo actual del LED seleccionado
    int color_selected = 0; // Variable para indicar el LED seleccionado (0 = No seleccionado, 1 = Rojo, 2 = Verde, 3 = Azul)

    gpio_set_direction(LED_R, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_G, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_B, GPIO_MODE_OUTPUT);
    gpio_set_direction(BTN_COLOR_SELECT, GPIO_MODE_INPUT);
    gpio_set_direction(BTN_BRIGHTNESS, GPIO_MODE_INPUT);

    gpio_set_pull_mode(BTN_COLOR_SELECT, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(BTN_BRIGHTNESS, GPIO_PULLUP_ONLY);

    // Configuración de los temporizadores del LEDC
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolución del ciclo de trabajo PWM
        .freq_hz = 5000,                      // frecuencia de la señal PWM
        .speed_mode = LEDC_LOW_SPEED_MODE,    // modo del temporizador (baja velocidad)
        .timer_num = LEDC_TIMER_1,            // índice del temporizador
        .clk_cfg = LEDC_AUTO_CLK,             // seleccionar automáticamente el reloj fuente
    };
    
    ledc_timer_config(&ledc_timer);

    // Configuración de los canales del LEDC para los LEDs RGB
    ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM] = {
        {
            .channel    = LEDC_CHANNEL_0,
            .duty       = 0,
            .gpio_num   = LED_R,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_1
        },
        {
            .channel    = LEDC_CHANNEL_1,
            .duty       = 0,
            .gpio_num   = LED_G,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_1
        },
        {
            .channel    = LEDC_CHANNEL_2,
            .duty       = 0,
            .gpio_num   = LED_B,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_1
        },
    };

    // Inicializar los canales del LEDC
    for (int ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
        ledc_channel_config(&ledc_channel[ch]);
    }

    // Estado anterior del botón de selección de color (1 = no presionado)
    int last_btn_color_state = 1;

    while (1)
    {
        // Leer el estado actual del botón de selección de color (B3)
        int btn_color_state = gpio_get_level(BTN_COLOR_SELECT);

        // Verificar si se presionó el botón de selección de color (B3)
        if (btn_color_state == 0 && last_btn_color_state == 1)
        {
            // Apagar el LED anterior (si está encendido)
            if (color_selected == 1) {
                ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, 0);
                ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);
            } else if (color_selected == 2) {
                ledc_set_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel, 0);
                ledc_update_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel);
            } else if (color_selected == 3) {
                ledc_set_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel, 0);
                ledc_update_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel);
            }

            color_selected = (color_selected + 1) % 4; // Cambiar al siguiente color (Rojo -> Verde -> Azul -> No seleccionado)
            
            delay_ms(200); // Pequeño retardo para evitar rebotes del botón
        }

        // Actualizar el estado del botón de selección de color
        last_btn_color_state = btn_color_state;

        // Leer el estado actual del botón de brillo (B4)
        int btn_brightness_state = gpio_get_level(BTN_BRIGHTNESS);

        // Verificar si se presionó el botón de brillo (B4)
        if (btn_brightness_state == 0)
        {
            brightness += 500; // Aumentar el brillo en 500 unidades

            // Si el brillo alcanza el valor máximo, reiniciar a 0
            if (brightness >= LEDC_TEST_DUTY)
            {
                brightness = 0;
            }

            delay_ms(200); // Pequeño retardo para evitar rebotes del botón
        }

        // Configurar el brillo del LED seleccionado
        if (color_selected == 1) { // Rojo seleccionado
            ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, brightness);
            ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);
        } else if (color_selected == 2) { // Verde seleccionado
            ledc_set_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel, brightness);
            ledc_update_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel);
        } else if (color_selected == 3) { // Azul seleccionado
            ledc_set_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel, brightness);
            ledc_update_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel);
        } else { // No seleccionado (apagar todos los LEDs)
            ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, 0);
            ledc_set_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel, 0);
            ledc_set_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel, 0);
            ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);
            ledc_update_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel);
            ledc_update_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel);
        }

        delay_ms(100); // Pequeño retardo para permitir cambios de brillo más suaves
    }
}
