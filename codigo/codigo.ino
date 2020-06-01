
#include "DHT.h"
#include <LiquidCrystal.h>

//Variavel personalizada para especies
typedef struct{
  char nome[16]; //Nome e uma string para ser mostrada no display
  int limIrrig; //Limite de umidade do solo para o qual a planta deve ser irrigada
} ESPECIE_TIPO;

/* Sensores */
#define ldrPin A0
#define higrometroPin A4
#define dhtPin 13
#define solenoidePin 12

DHT dht(dhtPin, DHT11);

/* Entrada */
#define b1Pin 1
#define b2Pin 2
#define b3Pin 3

/* Saida */
#define ledPin 4
LiquidCrystal lcd(10, 9, 8, 7, 6, 5);

/* Variaveis globais */
#define nEspecies 2

//Arrays para armazenar valores dos sensores
int b1[2], b2[2], b3[2], lum[2], hsolo[2];
float h_ar[2], temp[2];

volatile int scrollstate = -1;
String texto = "Temperatura: °C Hum Rel do Ar: % Hum do Solo: ";
#define tamanho_texto 60 //Ainda precisa contar o numero de caracteres da umidade do solo

//Para as partes do programa que dependem da especie escolhida
volatile int especie = 0;

//Lista de especies:
ESPECIE_TIPO especies[nEspecies] = {{"Cebolinha", 100}, {"Manjericao", 200}}; //{Nome, Limite de irrigacao}

/**********************************************************************/

//Le as informacoes dos sensores
void leSensores(int* b_especie, int* b_irrigacao, int* b_scroll, int* lum, int* hsolo, float* h_ar, float* temp){
  
  //Armazena ultimo valor do sensor
  b_especie[0] = b_especie[1]
  b_irrigacao[0] = b_irrigacao[1] 
  b_scroll[0] = b_scroll[1]

  lum[0] = lum[1]
  hsolo[0] = hsolo[1]

  h_ar[0] = h_ar[1]
  temp[0] = temp[1]
  
  //Definicao das entradas dos botoes**************************************
  b_especie[1] = digitalRead(b1Pin);
  b_irrigacao[1] = digitalRead(b2Pin);
  b_scroll[1] = digitalRead(b3Pin);

  //Armazenamento das entradas dos sensores********************************
  lum[1] = analogRead(ldrPin);
  hsolo[1] = analogRead(higrometroPin);

  h_ar[1] = dht.readHumidity();
  temp[1] = dht.readTemperature();
}

//Mostra as informacoes dos sensores no display
void imprimeSensores(int lum, int hsolo, float h_ar, float temp){
  
  lcd.setCursor(1,0);
  lcd.print(especies[especie].nome);

  lcd.setCursor(0,0);
  lcd.print("Temp.: ");
  lcd.print(temp);
  lcd.print(" °C ");
  //Imprime exatamente 16 caracteres
  if(scrollstate == 1){
    texto = texto.substring(0,12) + String(temp) + texto.substring(13,30) + String(h_ar) + texto.substring(31,46) + String(hsolo);

    int ini = 0, fim = 16;

    for(; fim <= tamanho_texto; ini++, fim++){
      lcd.setCursor(0, 0);
      lcd.print( texto.substring(ini, fim) ); 
      delay(300);
    }

  }

}

//Interrupcao que troca o indice da especie
void trocaDeEspecie(){
  especie++;
  if(especie >= nEspecies){
    especie = 0;  
  }  
}

//Interrupcao que ativa e desliga rolamento dos dados no LCD
void scroll(){
  scrollstate *= -1;
}

void setup() {
  
  Serial.begin(9600);
  lcd.begin(16, 2);
  dht.begin();

  //Sensores
  pinMode(ldrPin, INPUT);
  pinMode(higrometroPin, INPUT);
  pinMode(dhtPin, INPUT);
  pinMode(sensorAguaPin, INPUT);

  //Entrada
  pinMode(b1Pin, INPUT);
  pinMode(b2Pin, INPUT);
  pinMode(b3Pin, INPUT);

  //Saida
  pinMode(ledPin, OUTPUT);
  pinMode(solenoidePin, OUTPUT);

  //Interrupcoes
  attachInterrupt(0, trocaDeEspecie, FALLING);
  attachInterrupt(1, scroll, RISING);
}

void loop() {

  /* Nesta funcao, dos botoes:
  b1 e para irrigar manualmente
  b2 e para mudar de especie por interrupcao
  b3 e para alterar o estado do rolamento por interrupcao;
  */
  leSensores(b1, b2, b3, lum, hsolo, h_ar, temp);

  imprimeSensores(lum[1], hsolo[1], h_ar[1], temp[1]);

  //Bypass do b1 (Irrigacao manual) funciona melhor se usar um switch inves de um botao
  while(b1 == HIGH){
    irriga();  
  }

  //Condicionais de irrigacao
  if(hsolo[0] >= especies[especie].limIrrig && hsolo[1] >= especies[especie].limIrrig){
    digitalWrite(solenoidePin, HIGH);
  }

  if(hsolo[0] <= especies[especie].limIrrig && hsolo[1] <= especies[especie].limIrrig){
    digitalWrite(solenoidePin, LOW);
  }
   
  delay(1000);
}
