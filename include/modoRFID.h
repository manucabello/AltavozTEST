#ifndef MODORFID_H
#define MODORFID_H

#include <SPI.h>        // Importación de librerías para el bus SPI
#include <MFRC522.h>    // Importación de librerías del módulo RFID RC522
#include <SD.h>         // Importación de librerías del módulo microSD

#include <map>
#include <vector>

#include "audioI2S.h"

#define ARCHIVO_ASOCIACIONES "/asociaciones.txt"

#define SS_RFID  5      // Pin SDA del RC522
#define RST_RFID 21     // Pin RST del RC522
#define SS_microSD 4    // Pin CS del móculo microSD

MFRC522 rfid(SS_RFID, RST_RFID);    // Creación de instancia de MFRC522

void inicializarRFID() {
    SPI.begin();                        // Iniciar el bus SPI
    pinMode(SS_RFID, OUTPUT);           // Inicializar lector RFID
    digitalWrite(SS_RFID, LOW);         // Activar RFID
    rfid.PCD_Init();                    // Iniciar el módulo RFID
    Serial.println("📡 RFID listo");
}

void activarRFID() {
    digitalWrite(SS_microSD, HIGH);     // Desactiva el lector microSD
    digitalWrite(SS_RFID, LOW);         // Activa el lector RFID
}

void desactivarRFID() {
    digitalWrite(SS_RFID, HIGH);        // Desactiva el lector RFID
}

void inicializarMicroSD() {
    Serial.println("⏳ Inicializando tarjeta SD...");
    pinMode(SS_microSD, OUTPUT);        // Inicializar lector microSD
    digitalWrite(SS_microSD, HIGH);     // Aseguramos que el lector microSD esté desactivado

    if(!SD.begin(SS_microSD)) {
        Serial.println("❌ Error al inicializar la tarjeta SD.");
        return;
    }
    Serial.println("✅ Tarjeta SD inicializada.");
}

void activarMicroSD() {
    digitalWrite(SS_RFID, HIGH);        // Desactiva el lector RFID
    digitalWrite(SS_microSD, LOW);      // Activa el lector microSD
}

void desactivarMicroSD() {
    digitalWrite(SS_microSD, HIGH);     // Desactiva el lector microSD
}

void asociarTarjetaConCancion(String uid) {
    // Leer asociaciones existentes
    File archivo = SD.open(ARCHIVO_ASOCIACIONES, FILE_READ);
    std::map<String, String> asociaciones;
    if (archivo) {
        while (archivo.available()) {
            String linea = archivo.readStringUntil('\n');
            linea.trim();
            int coma = linea.indexOf(',');
            if (coma > 0) {
                String uidGuardado = linea.substring(0, coma);
                String cancion = linea.substring(coma + 1);
                asociaciones[uidGuardado] = cancion;
            }
        }
        archivo.close();
    }

    // Obtener canciones no asignadas
    File root = SD.open("/");
    std::vector<String> disponibles;
    while (true) {
        File entry = root.openNextFile();
        if (!entry) break;
        String nombre = entry.name();
        if (!entry.isDirectory() && nombre.endsWith(".mp3")) {
            bool asignada = false;
            for (auto &par : asociaciones) {
                if (par.second == nombre) {
                    asignada = true;
                    break;
                }
            }
            if (!asignada) {
                disponibles.push_back(nombre);
            }
        }
        entry.close();
    }

    if (disponibles.empty()) {
        Serial.println("❌ No hay canciones disponibles para asignar.");
        return;
    }

    Serial.println("🎶 Canciones disponibles para asociar:");
    for (int i = 0; i < disponibles.size(); i++) {
        Serial.printf(" [%d] %s\n", i + 1, disponibles[i].c_str());
    }

    Serial.println("💡 Escribe el número de la canción para enlazarla con esta tarjeta (o 0 para no asociar ninguna):");

    while (!Serial.available());
    int seleccion = Serial.parseInt();
    if (seleccion == 0) {
        Serial.println("ℹ️ No se ha realizado ninguna asociación.");
        return;
    }

    if (seleccion < 1 || seleccion > disponibles.size()) {
        Serial.println("❌ Selección no válida.");
        return;
    }

    String cancionElegida = "/" + disponibles[seleccion - 1];

    // Guardar nueva asociación
    File archivoEscritura = SD.open(ARCHIVO_ASOCIACIONES, FILE_APPEND);
    if (archivoEscritura) {
        archivoEscritura.printf("%s,%s\n", uid.c_str(), cancionElegida.c_str());
        archivoEscritura.close();
        Serial.printf("✅ Canción '%s' asignada a la tarjeta %s\n", cancionElegida.c_str(), uid.c_str());
    } else {
        Serial.println("❌ No se pudo abrir el archivo para escribir.");
    }
}

void reproducirCancion(String cancion) {
    Serial.print("🎶 Reproduciendo: ");
    Serial.println(cancion);

    activarMicroSD(); // Activar lector microSD para acceder a los archivos

    // Verificamos si el archivo existe antes de crear el objeto de reproducción
    if (!SD.exists(cancion.c_str())) {
        Serial.println("❌ No se pudo encontrar el archivo de audio.");
        desactivarMicroSD();
        return;
    }

    // Inicializar AudioFileSourceSD y AudioOutputI2S
    file = new AudioFileSourceSD(cancion.c_str());
    
    // Usar el AudioOutputI2S previamente inicializado en `inicializarAudio()`
    out->SetGain(volumen / 100.0);  // Usa tu variable de volumen (0.0 - 1.0)

    mp3 = new AudioGeneratorMP3();
    mp3->begin(file, out);

    while (mp3->isRunning()) {
        if (!mp3->loop()) break;
    }

    mp3->stop();
    delete mp3;
    delete file;

    mp3 = nullptr;
    file = nullptr;

    desactivarMicroSD(); // Desactivar microSD después de reproducir
}

String leerUIDTarjeta() {
    activarRFID();

    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return "";
    }

    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
        uid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
        uid += String(rfid.uid.uidByte[i], HEX);
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    desactivarRFID(); // (Aunque no se ejecutaba por estar después del return, ahora sí sirve)

    // Buscar si la tarjeta ya tiene canción asignada
    File archivo = SD.open(ARCHIVO_ASOCIACIONES, FILE_READ);
    std::map<String, String> asociaciones;

    if (archivo) {
        while (archivo.available()) {
            String linea = archivo.readStringUntil('\n');
            linea.trim();
            int coma = linea.indexOf(',');
            if (coma > 0) {
                String uidGuardado = linea.substring(0, coma);
                String cancion = linea.substring(coma + 1);
                asociaciones[uidGuardado] = cancion;
            }
        }
        archivo.close();
    }

    if (asociaciones.find(uid) != asociaciones.end()) {
        String cancion = asociaciones[uid];
        Serial.print("🎵 Esta tarjeta ya tiene asignada la canción: ");
        Serial.println(cancion);

        reproducirCancion(cancion);
    } else {
        Serial.println("📇 Tarjeta sin canción asignada. Vamos a asociarla.");
        asociarTarjetaConCancion(uid);
    }

    desactivarRFID();

    return uid;
}

void listarArchivos() {
    Serial.println("📁 Listando archivos en la tarjeta SD...");
    
    activarMicroSD();       // Activar microSD
    File root = SD.open("/");
    if (!root) {
        Serial.println("❌ No se pudo abrir la raíz de la SD.");
        digitalWrite(SS_microSD, HIGH); // Apagar SD
        return;
    }
  
    while (true) {
        File entry = root.openNextFile();
        if (!entry) break;
        Serial.println(entry.name());
        entry.close();
    }
    root.close();

    activarRFID();  // Reactivar RFID
  
    Serial.println("🔄 Hecho. SD desactivada, RFID reactivado.");
}

#endif