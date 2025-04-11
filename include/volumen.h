#ifndef VOLUMEN_H
#define VOLUMEN_H

int volumen = 50;  // Volumen inicial (de 0 a 100)

// Pines de pulsadores
const int volDown = 11;
const int volUp = 12;

// Evitar problemas al dejar pulsado el botón
unsigned long tiempoUltimaPulsacionVolDown = 0;     // Variable para el control de tiempo del pulsador volDown
unsigned long tiempoUltimaPulsacionVolUp = 0;       // Variable para el control de tiempo del pulsador volUp
unsigned long debounceDelayDown = 200;              // Tiempo de debounce (200ms) del pulsador volDown
unsigned long debounceDelayUp = 200;                // Tiempo de debounce (200ms) del pulsador volUp
bool pulsadoDown = false;
bool pulsadoUp = false;

void inicializarVolumen() {
    pinMode(volDown, INPUT_PULLUP);
    pinMode(volUp, INPUT_PULLUP);
}

// Función para detectar pulsación en el pulsador volDown
bool detectarBajarVolumen() {
    bool estadoPulsador = digitalRead(volDown) == LOW;
    
    if (!estadoPulsador && pulsadoDown && (millis() - tiempoUltimaPulsacionVolDown > debounceDelayDown)) {
        tiempoUltimaPulsacionVolDown = millis();
        pulsadoDown = false;
        return true;
    }
  
    if (estadoPulsador) {
        pulsadoDown = true;
    }

    return false;
}

// Función para detectar pulsación en el pulsador volUp
bool detectarSubirVolumen() {
    bool estadoPulsador = digitalRead(volUp) == LOW;
    
    if (!estadoPulsador && pulsadoUp && (millis() - tiempoUltimaPulsacionVolUp > debounceDelayUp)) {
        tiempoUltimaPulsacionVolUp = millis();
        pulsadoUp = false;
        return true;
    }
  
    if (estadoPulsador) {
        pulsadoUp = true;
    }

    return false;
}

// Función para subir el volumen
void subirVolumen() {
    if (volumen < 100) {
      volumen += 10;
      Serial.println("🔊 Volumen subido a: " + String(volumen));
    } else {
        beepMultiple(2, 4000);
    }
  }
  
  // Función para bajar el volumen
  void bajarVolumen() {
    if (volumen > 0) {
      volumen -= 10;
      Serial.println("🔉 Volumen bajado a: " + String(volumen));
    } else {
        beepMultiple(2, 1000);
    }
  }

#endif