//Karen Córdova- 21098- Laboratorio 

#include <Arduino.h>
#include "driver/ledc.h"

#define LED_R       23  // GPIO para el LED Rojo
#define LED_G       22  // GPIO para el LED Verde
#define LED_B       21  // GPIO para el LED Azul
#define BTN_COLOR_SELECT 18  // GPIO para el botón de selección de color (B3)
#define BTN_BRIGHTNESS   19  // GPIO para el botón de brillo (B4)

#define LEDC_TEST_CH_NUM       3 // Número de canales PWM utilizados (uno por cada color)
#define LEDC_TEST_DUTY         5000 // Valor máximo del ciclo de trabajo (máximo brillo)
#define LEDC_TEST_FADE_TIME    100 // Tiempo de desvanecimiento (ms) para cambios de brillo más suaves

void setup() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(BTN_COLOR_SELECT, INPUT_PULLUP);
  pinMode(BTN_BRIGHTNESS, INPUT_PULLUP);

  // Configuración de los temporizadores del LEDC
  ledcSetup(LEDC_CHANNEL_0, 5000, 13);
  ledcSetup(LEDC_CHANNEL_1, 5000, 13);
  ledcSetup(LEDC_CHANNEL_2, 5000, 13);

  ledcAttachPin(LED_R, LEDC_CHANNEL_0);
  ledcAttachPin(LED_G, LEDC_CHANNEL_1);
  ledcAttachPin(LED_B, LEDC_CHANNEL_2);
}

void loop() {
  int current_led = 0; // Variable para almacenar el índice del LED actual (0: Rojo, 1: Verde, 2: Azul)
  int brightness = 0; // Variable para almacenar el brillo actual del LED seleccionado
  int color_selected = 0; // Variable para indicar el LED seleccionado (0 = No seleccionado, 1 = Rojo, 2 = Verde, 3 = Azul)

  int last_btn_color_state = HIGH;

  while (1) {
    int btn_color_state = digitalRead(BTN_COLOR_SELECT);

    if (btn_color_state == LOW && last_btn_color_state == HIGH) {
      // Apagar el LED anterior (si está encendido)
      if (color_selected == 1) {
        ledcWrite(LEDC_CHANNEL_0, 0);
      } else if (color_selected == 2) {
        ledcWrite(LEDC_CHANNEL_1, 0);
      } else if (color_selected == 3) {
        ledcWrite(LEDC_CHANNEL_2, 0);
      }

      color_selected = (color_selected + 1) % 4; // Cambiar al siguiente color (Rojo -> Verde -> Azul -> No seleccionado)

      delay(200); // Pequeño retardo para evitar rebotes del botón
    }

    last_btn_color_state = btn_color_state;

    int btn_brightness_state = digitalRead(BTN_BRIGHTNESS);

    if (btn_brightness_state == LOW) {
      brightness += 500; // Aumentar el brillo en 500 unidades

      if (brightness >= LEDC_TEST_DUTY) {
        brightness = 0;
      }

      delay(200); // Pequeño retardo para evitar rebotes del botón
    }

    // Configurar el brillo del LED seleccionado
    if (color_selected == 1) { // Rojo seleccionado
      ledcWrite(LEDC_CHANNEL_0, brightness);
    } else if (color_selected == 2) { // Verde seleccionado
      ledcWrite(LEDC_CHANNEL_1, brightness);
    } else if (color_selected == 3) { // Azul seleccionado
      ledcWrite(LEDC_CHANNEL_2, brightness);
    } else { // No seleccionado (apagar todos los LEDs)
      ledcWrite(LEDC_CHANNEL_0, 0);
      ledcWrite(LEDC_CHANNEL_1, 0);
      ledcWrite(LEDC_CHANNEL_2, 0);
    }

    delay(100); // Pequeño retardo para permitir cambios de brillo más suaves
  }
}
