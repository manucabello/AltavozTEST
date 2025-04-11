#ifndef MODOS_H
#define MODOS_H

// Pin del pulsador
const int pulsaModo = 13;

// Evitar problemas al dejar pulsado el botón
unsigned long tiempoUltimaPulsacionModo = 0;  // Variable para el control de tiempo del pulsador pulsaModo
unsigned long debounceDelayModo = 200;        // Tiempo de debounce (200ms) del pulsador pulsaModo
bool pulsadoModo = false;

// Definir los pines de los LEDs para los modos
const int ledVerde = 8;
const int ledAzul = 9;
const int ledRojo = 4;

// Simulación de modos de reproducción
enum Modo { RFID, BLUETOOTH, WIFI };
Modo modoActual = RFID;

// Gestión del modo WiFi
bool modoWiFiActivo = false;
unsigned long tiempoInicioWiFi = 0;
const unsigned long duracionParaWiFi = 3000; // 3 segundos

// Función para actualizar los LEDs según el modo actual
void actualizarLEDs() {
  // Apagar todos los colores
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAzul, LOW);
  digitalWrite(ledRojo, LOW);

  // Encender el color correspondiente según el modo
  if (modoWiFiActivo) {
    digitalWrite(ledRojo, HIGH);  // Rojo para modo WiFi
  } else {
    if (modoActual == RFID) {
      digitalWrite(ledVerde, HIGH);  // Verde para modo RFID
    } else if (modoActual == BLUETOOTH) {
      digitalWrite(ledAzul, HIGH);  // Azul para modo Bluetooth
    }
  }
}

// Función para inicializar los modos y los LEDs
void inicializarModo() {
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAzul, OUTPUT);
  pinMode(ledRojo, OUTPUT);
  pinMode(pulsaModo, INPUT_PULLUP);
  actualizarLEDs();
}

// Función para detectar pulsación en el pulsador pulsaModo
bool detectarCambioModo() {
  bool estadoPulsador = digitalRead(pulsaModo) == LOW;

  if (!estadoPulsador && pulsadoModo && (millis() - tiempoUltimaPulsacionModo > debounceDelayModo)) {
    tiempoUltimaPulsacionModo = millis();
    pulsadoModo = false;
    return true;
  }

  if (estadoPulsador) {
    pulsadoModo = true;
  }

  return false;
}

// Función para comprobar la activación/desactivación del modo WiFi
void comprobarModoWiFi() {
  bool modoPresionado = digitalRead(pulsaModo) == LOW;
  bool playPresionado = digitalRead(play) == LOW;

  // Si los botones de modo y play están presionados durante el tiempo necesario (3 segundos)
  if (modoPresionado && playPresionado) {
    if (tiempoInicioWiFi == 0) {
      tiempoInicioWiFi = millis(); // empieza a contar el tiempo
    } else if (millis() - tiempoInicioWiFi >= duracionParaWiFi && !modoWiFiActivo) {
      modoWiFiActivo = true;  // Cambia a modo WiFi
      actualizarLEDs();
      beepMultiple(3);
      Serial.println("🌐 Modo WiFi ACTIVADO (simulado)");
    }
  } else {
    tiempoInicioWiFi = 0; // resetea si se sueltan los botones antes de tiempo
  }

  // Si el modo WiFi está activo y el botón de "modo" se pulsa, se desactiva
  if (modoWiFiActivo) {
    if (digitalRead(pulsaModo) == LOW && !pulsadoModo) {
      pulsadoModo = true;
      modoWiFiActivo = false;  // Desactivar el modo WiFi
      actualizarLEDs();
      Serial.println("🚪 Saliendo del modo WiFi...");
    }
  }

  if (digitalRead(pulsaModo) == HIGH) {
    pulsadoModo = false;  // Resetea el estado cuando el botón es liberado
  }
}

// Función para cambiar de modo entre RFID y Bluetooth
void cambiarModo() {
  modoActual = (modoActual == RFID) ? BLUETOOTH : RFID;
  actualizarLEDs();
  
  if (modoActual == RFID) {
    beep();
  } else {
    beepMultiple(2);
  }
}

#endif