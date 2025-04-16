#ifndef AUDIOI2S_H
#define AUDIOI2S_H

#include <AudioFileSourceSD.h>  // Lectura desde SD
#include <AudioGeneratorMP3.h>  // Decodificación MP3
#include <AudioOutputI2S.h>     // Salida I2S

#define I2S_DOUT    26  // DIN (datos)
#define I2S_BCLK    22  // BCLK (bit clock)
#define I2S_LRCLK   25  // LRC (word select / left-right clock)

AudioGeneratorMP3 *mp3 = nullptr;
AudioFileSourceSD *file = nullptr;
AudioOutputI2S *out = nullptr;

void inicializarAudio() {
    out = new AudioOutputI2S();
    out->SetPinout(I2S_BCLK, I2S_LRCLK, I2S_DOUT);  // (BCLK, LRC, DIN)
    out->SetGain(0.5);  // Volumen (0.0 a 1.0)

    Serial.println("🔊 Audio I2S inicializado.");
}

#endif