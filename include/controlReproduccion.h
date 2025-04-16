#ifndef CONTROLREPRODUCCION_H
#define CONTROLREPRODUCCION_H

enum EstadoReproduccion { STOPPED, PLAYING, PAUSED };
EstadoReproduccion estadoReproduccion = STOPPED;

// Pin de los pulsadores
const int prev = 32;
const int play = 33;
const int next = 35;

// Evitar problemas al dejar pulsado el botón
unsigned long tiempoUltimaPulsacionPrev = 0;       // Variable para el control de tiempo del pulsador prev
unsigned long tiempoUltimaPulsacionPlay = 0;       // Variable para el control de tiempo del pulsador play
unsigned long tiempoUltimaPulsacionNext = 0;       // Variable para el control de tiempo del pulsador next

unsigned long debounceDelayPrev = 200;              // Tiempo de debounce (200ms) del pulsador prev
unsigned long debounceDelayPlay = 200;                // Tiempo de debounce (200ms) del pulsador play
unsigned long debounceDelayNext = 200;                // Tiempo de debounce (200ms) del pulsador next

bool pulsadoPrev = false;
bool pulsadoPlay = false;
bool pulsadoNext = false;

void inicializarControlReproduccion() {
    pinMode(prev, INPUT_PULLUP);
    pinMode(play, INPUT_PULLUP);
    pinMode(next, INPUT_PULLUP);
}

// Función para detectar pulsación en el pulsador prev
bool detectarPrev() {
    bool estadoPulsador = digitalRead(prev) == LOW;
    
    if (!estadoPulsador && pulsadoPrev && (millis() - tiempoUltimaPulsacionPrev > debounceDelayPrev)) {
        tiempoUltimaPulsacionPrev = millis();
        pulsadoPrev = false;
        return true;
    }
  
    if (estadoPulsador) {
        pulsadoPrev = true;
    }

    return false;
}

// Función para detectar pulsación en el pulsador play
bool detectarPlay() {
    bool estadoPulsador = digitalRead(play) == LOW;
    
    if (!estadoPulsador && pulsadoPlay && (millis() - tiempoUltimaPulsacionPlay > debounceDelayPlay)) {
        tiempoUltimaPulsacionPlay = millis();
        pulsadoPlay = false;
        return true;
    }
  
    if (estadoPulsador) {
        pulsadoPlay = true;
    }

    return false;
}

// Función para detectar pulsación en el pulsador next
bool detectarNext() {
    bool estadoPulsador = digitalRead(next) == LOW;
    
    if (!estadoPulsador && pulsadoNext && (millis() - tiempoUltimaPulsacionNext > debounceDelayNext)) {
        tiempoUltimaPulsacionNext = millis();
        pulsadoNext = false;
        return true;
    }
  
    if (estadoPulsador) {
        pulsadoNext = true;
    }

    return false;
}

void prevTrack() {
    Serial.println("⏮️ Canción anterior.");
}

void playPause() {
    if (estadoReproduccion == PLAYING) {
        estadoReproduccion = PAUSED;
        Serial.println("▶️ Reproduciendo.");
    } else {
        estadoReproduccion = PLAYING;
        Serial.println("⏸️ Pausado.");
    }
}

void nextTrack() {
    Serial.println("⏭️ Siguiente canción.");
}

#endif