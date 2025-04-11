#ifndef BUZZER_H
#define BUZZER_H

// Pin del buzzer
const int buzzer = 10;

void inicializarBuzzer() {
    pinMode(buzzer, OUTPUT);
    noTone(buzzer);
}

void beep(int duracion = 100, int frecuenciaBeep = 2000) {
    tone(buzzer, frecuenciaBeep, duracion);
    delay(duracion);
    noTone(buzzer);
}

void beepMultiple(int cantidad, int frecuenciaBeep = 2000, int duracion = 100, int pausa = 100) {
    for (int i = 0; i < cantidad; i++) {
        beep(duracion, frecuenciaBeep);
        delay(pausa);
    }
}

#endif