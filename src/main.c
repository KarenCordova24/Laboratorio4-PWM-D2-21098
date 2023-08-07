// Karen Córdova- 21098- Pre lab ESP IDF

#include <stdio.h>
#include <driver/gpio.h>
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define pinServo 22
#define ServoMsMin 0.06
#define ServoMsMax 2.1

#define BTNAumentar 4
#define BTNDisminuir 5

int servoPosition = 90;

void pwm_servo_control(int angle) {
    // Calcular el valor del duty necesario para el ángulo dado
    int duty = (int)(100.0 * ((ServoMsMin + (ServoMsMax - ServoMsMin) * angle / 180.0) / 20.0) * 81.91);
    // Configurar el duty para el canal LEDC_CHANNEL_0 en modo LEDC_LOW_SPEED_MODE
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    // Actualizar el duty para que se refleje en la señal PWM
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void app_main(void) {
    printf("LEDC&Servo Motor\n");

    // Configurar el módulo LEDC para controlar el servo
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = pinServo,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel);

    // Configurar pines de botones como entradas con pull-up
    gpio_config_t btn_config;
    btn_config.intr_type = GPIO_INTR_DISABLE;
    btn_config.mode = GPIO_MODE_INPUT;
    btn_config.pin_bit_mask = (1ULL << BTNAumentar) | (1ULL << BTNDisminuir);
    btn_config.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&btn_config);

    // Calibrar el servo en la posición inicial
    pwm_servo_control(servoPosition);

    while (1) {
        // Leer el estado de los botones
        int boton_aumentar_state = gpio_get_level(BTNAumentar);
        int boton_disminuir_state = gpio_get_level(BTNDisminuir);

        // Aumentar la posición del servo si se presiona el botón de aumentar
        if (boton_aumentar_state == 0) {
            servoPosition++;
            if (servoPosition > 180) {
                servoPosition = 180;
            }
            pwm_servo_control(servoPosition);
            vTaskDelay(200 / portTICK_PERIOD_MS); // Pequeño retraso para evitar rebotes
        }

        // Disminuir la posición del servo si se presiona el botón de disminuir
        if (boton_disminuir_state == 0) {
            servoPosition--;
            if (servoPosition < 0) {
                servoPosition = 0;
            }
            pwm_servo_control(servoPosition);
            vTaskDelay(200 / portTICK_PERIOD_MS); // Pequeño retraso para evitar rebotes
        }
    }
}
