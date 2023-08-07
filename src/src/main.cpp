//Karen Córdova-21098-Arduino

#include <Arduino.h>
#include <driver/ledc.h>

const int servoPin = 2;
const int buttonPin1 = 4;
const int buttonPin2 = 5;

int servoPosition = 0; // Posición inicial del servo en grados (0° es la posición inicial)
const int servoMinPosition = 0; // Posición mínima del servo en grados (0°)
const int servoMaxPosition = 180; // Posición máxima del servo en grados (180°)
const int servoIncrement = 1; // Incremento de posición del servo en grados

unsigned long lastButton1Time = 0; // Tiempo de la última lectura del botón 1
unsigned long lastButton2Time = 0; // Tiempo de la última lectura del botón 2
const unsigned long debounceDelay = 50; // Tiempo de debounce en milisegundos

void setup() {
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);

  // Configurar el módulo LEDC para generar PWM en el pin del servo
  ledcSetup(0, 50, 10); // Canal 0, frecuencia 50 Hz, resolución de 10 bits
  ledcAttachPin(servoPin, 0); // Asignar el pin del servo al canal 0
  ledcWrite(0, map(servoPosition, 0, 180, 0, 1023)); // Establecer la posición inicial del servo (0 grados)
}

void loop() {
  // Leer el estado de los botones y aplicar "debounce"
  int button1State = digitalRead(buttonPin1);
  int button2State = digitalRead(buttonPin2);

  // Verificar el botón 1
  if (button1State == LOW && millis() - lastButton1Time > debounceDelay) {
    // Botón B1 presionado, mover hacia la derecha
    if (servoPosition < servoMaxPosition) {
      servoPosition += servoIncrement;
      if (servoPosition > servoMaxPosition) {
        servoPosition = servoMaxPosition; // Limitar la posición a 180 grados
      }
      ledcWrite(0, map(servoPosition, 0, 180, 0, 1023));
    }
    lastButton1Time = millis(); // Actualizar el tiempo de última lectura del botón 1
  }

  // Verificar el botón 2
  if (button2State == LOW && millis() - lastButton2Time > debounceDelay) {
    // Botón B2 presionado, mover hacia la izquierda
    if (servoPosition > servoMinPosition) {
      servoPosition -= servoIncrement;
      if (servoPosition < servoMinPosition) {
        servoPosition = servoMinPosition; // Limitar la posición a 0 grados
      }
      ledcWrite(0, map(servoPosition, 0, 180, 0, 1023));
    }
    lastButton2Time = millis(); // Actualizar el tiempo de última lectura del botón 2
  }

  // Pequeño retraso para controlar la velocidad del servo
  delay(50);
}
