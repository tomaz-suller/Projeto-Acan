
#include "DHT.h"
#include <LiquidCrystal.h>

//Sensores
#define ldrPin A0
#define higrometro1Pin A4
#define higrometro2Pin A5
#define dhtPin 13
#define sensorAguaPin A3

DHT dht(dhtPin, DHT11);

//Entrada
#define botao1Pin 1
#define botao2Pin 2
#define botao3Pin 3

//Saida
#define ledPin 4
LiquidCrystal lcd(10, 9, 8, 7, 6, 5);

void setup() {
  
  Serial.begin(9600);
  lcd.begin(16, 2);
  dht.begin();

  //Sensores
  pinMode(ldrPin, INPUT);
  pinMode(higrometro1Pin, INPUT);
  pinMode(higrometro2Pin, INPUT);
  pinMode(dhtPin, INPUT);
  pinMode(sensorAguaPin, INPUT);

  //Entrada
  pinMode(botao1Pin, INPUT);
  pinMode(botao2Pin, INPUT);
  pinMode(botao3Pin, INPUT);

  //
  pinMode(ledPin, OUTPUT);
}

void loop() {

  //Definicao das entradas dos botoes
  int b1 = digitalRead(botao1Pin);
  int b2 = digitalRead(botao2Pin);
  int b3 = digitalRead(botao3Pin);

  //Armazenamento das entradas dos sensores
  int lum = analogRead(ldrPin);
  int hsolo1 = analogRead(higrometro1Pin);
  int hsolo2 = analogRead(higrometro2Pin);
  int nagua = analogRead(sensorAguaPin); //Precisa ser calibrado

  float h_ar = dht.readHumidity();
  float temp = dht.readTemperature();

  //Mostra as informacoes dos sensores no display
  
  //DHT11 - Temperatura e Humidade do Ar
  /*
   * Podemos usar lcd.autoscroll() para mostrar
   * todas as informacoes
   */
  lcd.setCursor(0,0);
  lcd.print("Temperatura: ");
  lcd.print(temp);
  lcd.print( (char) 223);
  lcd.print("C  ");
  lcd.print("Hum. Rel. do Ar: ");
  lcd.print(h_ar);
  lcd.print("%");
  
  
  delay(1000);
}
