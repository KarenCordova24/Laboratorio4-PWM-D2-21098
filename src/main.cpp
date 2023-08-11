// Karen Córdova- 21098-postlab4

#include <Arduino.h>
#include <driver/ledc.h>

// Definición de pines
#define LED_R           23
#define LED_G           22
#define LED_B           21
#define BTN_COLOR_SELECT 18
#define BTN_BRIGHTNESS   19
#define BTN_SERVO_INC    4
#define BTN_SERVO_DEC    5
#define SERVO_PIN        15

// Variables para el servo
int servoPosition = 0;
const int servoMinPosition = 0;
const int servoMaxPosition = 180;
const int servoIncrement = 1;

// Variables para controlar los LEDs y el servo
int selectedLED = 0;
int brightness = 128;
bool servoMoved = false;

// Variables para el debounce de los botones
unsigned long lastButton1Time = 0;
unsigned long lastButton2Time = 0;
unsigned long lastButton3Time = 0;
const unsigned long debounceDelay = 50;

// Función para mover el servo a la posición deseada
void moveServo() {
  ledcWrite(0, map(servoPosition, 0, 180, 0, 1023));
}

// Configuración inicial
void setup() {
  pinMode(BTN_COLOR_SELECT, INPUT_PULLUP);
  pinMode(BTN_BRIGHTNESS, INPUT_PULLUP);
  pinMode(BTN_SERVO_INC, INPUT_PULLUP);
  pinMode(BTN_SERVO_DEC, INPUT_PULLUP);
  pinMode(SERVO_PIN, OUTPUT);

  // Configuración de los canales LEDC para los LEDs
  ledcSetup(1, 5000, 8); // Canal 1, frecuencia 5000 Hz, resolución de 8 bits (LED_R)
  ledcSetup(2, 5000, 8); // Canal 2, frecuencia 5000 Hz, resolución de 8 bits (LED_G)
  ledcSetup(3, 5000, 8); // Canal 3, frecuencia 5000 Hz, resolución de 8 bits (LED_B)

  ledcAttachPin(LED_R, 1); // Asignar el pin del LED_R al canal 1
  ledcAttachPin(LED_G, 2); // Asignar el pin del LED_G al canal 2
  ledcAttachPin(LED_B, 3); // Asignar el pin del LED_B al canal 3

  // Configuración del canal LEDC para el servo
  ledcSetup(0, 50, 10); // Canal 0, frecuencia 50 Hz, resolución de 10 bits
  ledcAttachPin(SERVO_PIN, 0); // Asignar el pin del servo al canal 0
  moveServo(); // Mover el servo a la posición inicial

  // Iniciar comunicación con el monitor serial
  Serial.begin(115200);
  while (!Serial); // Esperar hasta que se establezca la comunicación serial
}

// Función principal que se ejecuta repetidamente
void loop() {
  // Leer el estado de los botones
  int button1State = digitalRead(BTN_SERVO_INC);
  int button2State = digitalRead(BTN_SERVO_DEC);
  int buttonColorState = digitalRead(BTN_COLOR_SELECT);
  int buttonBrightnessState = digitalRead(BTN_BRIGHTNESS);

  // Cambiar el color de los LEDs si se presiona el botón correspondiente
  if (buttonColorState == LOW && millis() - lastButton3Time > debounceDelay) {
    selectedLED = (selectedLED + 1) % 4;
    servoMoved = false;
    delay(200);
    lastButton3Time = millis();
  }

  // Cambiar el brillo de los LEDs si se presiona el botón correspondiente
  if (buttonBrightnessState == LOW && millis() - lastButton2Time > debounceDelay) {
    brightness += 20;
    if (brightness > 255) {
      brightness = 0;
    }
    delay(200);
    lastButton2Time = millis();
  }

  // Controlar el servo basado en la selección del color
  if (!servoMoved) {
    if (selectedLED == 1) {
      servoPosition = 5;
    } else if (selectedLED == 2) {
      servoPosition = 14;
    } else if (selectedLED == 3) {
      servoPosition = 25;
    }
    moveServo();
    servoMoved = true;
  }

  // Mover el servo hacia adelante si se presiona el botón de incremento
  if (button1State == LOW && millis() - lastButton1Time > debounceDelay) {
    if (servoPosition < servoMaxPosition) {
      servoPosition += servoIncrement;
      if (servoPosition > servoMaxPosition) {
        servoPosition = servoMaxPosition;
      }
      moveServo();
    }
    lastButton1Time = millis();
  }

  // Mover el servo hacia atrás si se presiona el botón de decremento
  if (button2State == LOW && millis() - lastButton2Time > debounceDelay) {
    if (servoPosition > servoMinPosition) {
      servoPosition -= servoIncrement;
      if (servoPosition < servoMinPosition) {
        servoPosition = servoMinPosition;
      }
      moveServo();
    }
    lastButton2Time = millis();
  }

  // Actualizar los LEDs según la selección del color
  switch (selectedLED) {
    case 1:
      ledcWrite(1, brightness);
      ledcWrite(2, 0);
      ledcWrite(3, 0);
      break;
    case 2:
      ledcWrite(1, 0);
      ledcWrite(2, brightness);
      ledcWrite(3, 0);
      break;
    case 3:
      ledcWrite(1, 0);
      ledcWrite(2, 0);
      ledcWrite(3, brightness);
      break;
    default:
      ledcWrite(1, 0);
      ledcWrite(2, 0);
      ledcWrite(3, 0);
      break;
  }

  // Mostrar la posición actual del servo en el monitor serial
  Serial.println("Posición del servo: " + String(servoPosition));

  delay(100);
}
