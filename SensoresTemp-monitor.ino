// Programa para usar los sensores DS18B20 con el PLC M-duino 19R+

//Importante:-El PLC tiene que estar conectado con alimentación externa para que reconozca la tarjeta SD.
//           -La tarjeta SD se conecta debajo del puerto USB detras de una tapa.
//           -Los datos en se guardan en la tarjeta en una archivo llamado "datalog.txt" en la raíz, si este archivo no está, el programa se detiene.
// Se ajusta la hora y fecha con los ejemplos de libreria DS1307RTC.h
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
// Datos conectados al pin D2
#define ONE_WIRE_BUS 2
// Inicia protocolo OneWire
OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

int numberOfDevices; // Sensores encontrados

// Variable de los nombres de los sensores
DeviceAddress tempDeviceAddress;

//Pin de conexión de la tarjeta SD
const int chipSelect = 53;

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for serial
  delay(200);
  Serial.println("DS1307RTC Read Test");
  Serial.println("-------------------");
  Serial.println("Conectado");
  Serial.print("Buscando tarjeta SD...");

  // Busca una tarjeta SD
  if (!SD.begin(chipSelect)) {
    Serial.println("Error, No encontrada.");
    // Termina el programa
    while (1);
  }
  Serial.println("Tarjeta SD encontrada.");
  
  // Inicia la librería
  sensors.begin();
  
  // Cuenta dispositivos encontrados
  numberOfDevices = sensors.getDeviceCount();
  
  Serial.print("Buscando sensores...");
  Serial.print("Se encontraron ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" sensores.");

  // Escribe dirección de cada dispositivo
  for(int i=0;i<numberOfDevices; i++) {
    if(sensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Se encontró S");
      Serial.print(i, DEC);
      Serial.print(" con nombre: ");
      printAddress(tempDeviceAddress);
      Serial.println();
		} else {
		  Serial.print("Se encontró sensor fantasma ");
		  Serial.print(i, DEC);
		  Serial.print(" pero no se detectó nombre. Revisar cableado");
		}
  }
}

void loop() {
  tmElements_t tm;
  delay(1000);
  
  // Verifica si el RTC está funcionando
  if (!RTC.read(tm)) {
    Serial.println("Error leyendo el RTC.");
    return; // Salta el resto del bucle si falla
  }

  sensors.requestTemperatures(); // Solicita temperaturas

  String dataString = "";

  // Agrega fecha y hora al inicio
  dataString += "Fecha: ";
  dataString += String(tm.Day) + "/";
  dataString += String(tm.Month) + "/";
  dataString += String(tmYearToCalendar(tm.Year));
  dataString += " Hora: ";
  dataString += String(tm.Hour) + ":";
  dataString += String(tm.Minute) + ":";
  dataString += String(tm.Second);
  dataString += ", ";

  // Agrega las temperaturas de los sensores
  for (int i = 0; i < numberOfDevices; i++) {
    if (sensors.getAddress(tempDeviceAddress, i)) {
      float tempC = sensors.getTempC(tempDeviceAddress);
      
      // Verifica si la temperatura es válida
      if (tempC == DEVICE_DISCONNECTED_C) {
        Serial.print("Error leyendo S");
        Serial.println(i);
        continue; // Salta al siguiente sensor
      }

      // Formato: S0: 25.5°C
      dataString += "S";
      dataString += printAddress(tempDeviceAddress);
      //dataString += String(i, DEC);
      dataString += ": ";
      dataString += String(tempC, 2); // Temperatura con 2 decimales
      dataString += " °C, ";
    }
  }

  // Escribe en el archivo SD
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    Serial.println("Datos guardados: " + dataString);
  } else {
    Serial.println("Error abriendo datalog.txt.");
  }

  delay(7000);
}

// función para imprimir dirección del dispositivo
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
