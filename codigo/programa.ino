
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
#define b1Pin 1
#define b2Pin 2
#define b3Pin 3

//Saida
#define ledPin 4
LiquidCrystal lcd(10, 9, 8, 7, 6, 5);

//Variáveis globais
#define nEspecies 2
//Para as partes do programa que dependem da especie escolhida
int especie = 0;
//Podemos adicionar mais se quisermos, dependndo da memoria
const char[nEspecies][16] especies = {{"Cebolinha"}, {"Manjericao"}};

//Le as informacoes dos sensores
void leSensores(int* b_especie, int* b_irrigacao, int* b_scroll, int* lum, int* hsolo1, int* hsolo2, int* nagua, float h_ar, float temp){
  //Definicao das entradas dos botoes**************************************
  *b_especie = digitalRead(botao1Pin);
  *b_irrigacao = digitalRead(botao2Pin);
  *b_scroll = digitalRead(botao3Pin);

  //Armazenamento das entradas dos sensores********************************
  *lum = analogRead(ldrPin);
  *hsolo1 = analogRead(higrometro1Pin);
  *hsolo2 = analogRead(higrometro2Pin);
  *nagua = analogRead(sensorAguaPin); //Precisa ser calibrado

  *h_ar = dht.readHumidity();
  *temp = dht.readTemperature();
}

//Mostra as informacoes dos sensores no display
void imprimeSensores(int lum, int hsolo1, int hsolo2, int nagua, float h_ar, float temp){
  
  //DHT11 - Temperatura e Humidade do Ar
    /*
    * Talvez possamos usar lcd.autoscroll() para mostrar
    * todas as informacoes?
    */
    lcd.setCursor(0,0);
    lcd.print("Temperatura: ");
    lcd.print(temp);
    lcd.print( (char) 223);
    lcd.print("C  ");
    lcd.print("Hum. Rel. do Ar: ");
    lcd.print(h_ar);
    lcd.print("%");

    //Higrometro - Humidade do solo****************************************
}

void trocaDeEspecie(){

}

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

  int b1, b2, b3, lum, hsolo1, hsolo2, nagua;
  float h_ar, temp;

  /* Nesta funcao, dos botoes:
  b1 e para mudar de especie
  b2 e para irrigar
  b3 e para rodar texto no LCD
  */
  leSensores(&b1, &b2, &b3, &lum, &hsolo1, &hsolo2, &nagua, &h_ar, &temp);

  if(b_especie == "LOW"){
    
    /* Nesta funcao, dos botoes:
    b1 e para rodar a especie
    b3 e para confirmar selecao
    */
    imprimeSensores(lum, hsolo1, hsolo2, nagua, lum, hsolo1, hsolo2, nagua);
    
   }else if(b_especie == "HIGH"){
    trocaDeEspecie();
   }
   
  delay(1000);
}