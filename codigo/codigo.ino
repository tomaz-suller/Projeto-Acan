
#include "DHT.h"
#include <LiquidCrystal.h>

//Variavel personalizada para especies
typedef struct{
  char nome[16]; //Nome e uma string para ser mostrada no display
  int limIrrig; //Limite de umidade do solo para o qual a planta deve ser irrigada
} ESPECIE_TIPO;

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
volatile int especie = 0;

//Lista de especies:
ESPECIE_TIPO especies[nEspecies] = {{"Cebolinha", 100}, {"Manjericao", 200}}; //{Nome, Limite de irrigacao}



//Le as informacoes dos sensores
void leSensores(int* b_especie, int* b_irrigacao, int* b_scroll, int* lum, int* hsolo1, int* hsolo2, int* nagua, float* h_ar, float* temp){
  //Definicao das entradas dos botoes**************************************
  *b_especie = digitalRead(b1Pin);
  *b_irrigacao = digitalRead(b2Pin);
  *b_scroll = digitalRead(b3Pin);

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
    //E possivel mostrar a especie selecionada (variavel -> especies[especie].nome)
}

void irriga(){
  //Implementar mecanismo de irrigacao  
}

//Interrupcao que troca o indice da especie
void trocaDeEspecie(){
  especie++;
  if(especie>=nEspecies){
    especie=0;  
  }  
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
  pinMode(b1Pin, INPUT);
  pinMode(b2Pin, INPUT);
  pinMode(b3Pin, INPUT);

  //Saida
  pinMode(ledPin, OUTPUT);

  //Interrupcoes
  //Troca de especie ligada ao botao 2
  attachInterrupt(0, trocaDeEspecie, FALLING);
}

void loop() {

  int b1, b2, b3, lum, hsolo1, hsolo2, nagua;
  float h_ar, temp;

  /* Nesta funcao, dos botoes:
  b1 e para irrigar manualmente
  b2 e para mudar de especie
  LCD provavelmente deve rodar no auto-scroll, ou por interrupcoes no b3;
  */
  leSensores(&b1, &b2, &b3, &lum, &hsolo1, &hsolo2, &nagua, &h_ar, &temp);

  imprimeSensores(lum, hsolo1, hsolo2, nagua, h_ar, temp);

  //Bypass do b1 (Irrigacao manual) funciona melhor se usar um switch inves de um botao
  while(b1 == HIGH){
    irriga();  
  }

  //Condicionais de irrigacao
  if(higrometro1Pin >= especies[especie].limIrrig){
    //Ativa o mecanismo de irrigacao
  }
  else if(higrometro2Pin >= especies[especie].limIrrig){
    //Ativa o mecanismo de irrigacao
  }
   
  delay(1000);
}
