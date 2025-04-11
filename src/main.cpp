// Proyecto: Altavoz Infantil - Simulación en Arduino (PlatformIO)

#include <Arduino.h>
#include "buzzer.h"
#include "controlReproduccion.h"
#include "modos.h"
#include "volumen.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Altavoz Infantil - Simulación iniciada");

  inicializarModo();
  inicializarBuzzer();
  inicializarVolumen();
  inicializarControlReproduccion();
}

void loop() {
  if (detectarCambioModo()) {
    cambiarModo();
  }

  comprobarModoWiFi();  // Comprobar si se debe activar el modo WiFi
  // Si está activo el modo WiFi, no se procesan otras funciones
  if (modoWiFiActivo) {
    return;
  }

  if (detectarBajarVolumen()) {
    bajarVolumen();
  }

  if (detectarSubirVolumen()) {
    subirVolumen();
  }

  if (detectarPrev()) {
    if (modoActual == BLUETOOTH) {
      prevTrack();
    }
  }

  if (detectarPlay()) {
    playPause();
  }

  if (detectarNext()) {
    if (modoActual == BLUETOOTH) {
      nextTrack();
    }
  }
}
