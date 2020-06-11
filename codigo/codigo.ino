
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
#define b1Pin 4
#define b2Pin 2
#define b3Pin 3

/* Saida */
#define ledPin 4
LiquidCrystal lcd(10, 9, 11, 5, 6, 7);

/* Variaveis globais */
#define nEspecies 2
#define nTelas 3
int primeiraLeitura;

//Arrays para armazenar valores dos sensores
int b1, lum[2], hsolo[2];
float h_ar[2], temp[2];

//Para a selecao dos dados a serem mostrados
volatile int indice_tela = 0;

//Para as partes do programa que dependem da especie escolhida
volatile int especie = 0;

//Lista de especies:
ESPECIE_TIPO especies[nEspecies] = {
  {"Cebolinha", 700},
  {"Manjericao", 800}
  //{Nome, Limite de irrigacao}
  };

/**********************************************************************/

//Le as informacoes dos sensores
void leSensores(int* b_irrigacao, int* lum, int* hsolo, float* h_ar, float* temp){
  lum[0] = lum[1];
  hsolo[0] = hsolo[1];

  h_ar[0] = h_ar[1];
  temp[0] = temp[1];
  
  //Definicao das entradas dos botoes
  *b_irrigacao = digitalRead(b1Pin);

  //Armazenamento das entradas dos sensores
  lum[1] = analogRead(ldrPin);
  hsolo[1] = analogRead(higrometroPin);

  h_ar[1] = dht.readHumidity();
  temp[1] = dht.readTemperature();
}

//Mostra as informacoes dos sensores no display
void imprimeSensores(int lum, int hsolo, float h_ar, float temp){
  String linha_1;
  String linha_2;

  //Escolhe os dados que serao mostrados
  switch(indice_tela){
    case 0:
      linha_1 = "Hum. Solo: " + String(hsolo);
      linha_2 = "Lum.: " + String(lum);
      break;
    case 1:
      linha_1 = "Temp.: " +String(temp, 2) + " °C";
      linha_2 = "Hum. Ar: " + String(h_ar, 2);
      break;
    case 2:
      linha_1 = especies[especie].nome;
      linha_2 = "Lim. irrig: " + String(especies[especie].limIrrig);
      break;
    default:
      indice_tela = 0;
      linha_1 = "Hum. Solo: " + String(hsolo);
      linha_2 = "Lum.: " + String(lum);
      break;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(linha_1);
  lcd.setCursor(0,1);
  lcd.print(linha_2);
  
  //Para testes
  Serial.println(linha_1);
  Serial.println(linha_2);
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
  indice_tela++;
  if(indice_tela >= nTelas){
    indice_tela = 0;
  }
}

//Irrigacao
void irriga(){
  digitalWrite(solenoidePin, HIGH);
  delay(10000);
  digitalWrite(solenoidePin, LOW);
}

void setup() {
  

  lcd.begin(16, 2);
  dht.begin();

  //Sensores
  pinMode(ldrPin, INPUT);
  pinMode(higrometroPin, INPUT);
  pinMode(dhtPin, INPUT);

  //Entrada
  pinMode(b1Pin, INPUT);
  pinMode(b2Pin, INPUT);
  pinMode(b3Pin, INPUT);

  //Saida
  pinMode(ledPin, OUTPUT);
  pinMode(solenoidePin, OUTPUT);

  //Interrupcoes
  attachInterrupt(0, trocaDeEspecie, RISING);
  attachInterrupt(1, scroll, RISING);

  //Define a primeira leitura dos sensores
  primeiraLeitura = 1;
}

void loop() {
  /* 
  Nesta funcao, dos botoes:
  b1 e para irrigar manualmente
  b2 e para mudar de especie por interrupcao
  b3 e para alterar o estado do rolamento por interrupcao;
  */
  //Atualiza os dados dos sensores
  leSensores(&b1, lum, hsolo, h_ar, temp);

  //Conserta o erro de logica da primeira leitura (um termo [0] do array com valor aleatorio)
  if(primeiraLeitura){
    lum[0] = lum[1];
    hsolo[0] = hsolo[1];
    h_ar[0] = h_ar[1];
    temp[0] = temp[1];
    primeiraLeitura = 0;
  }

  //Imprime os dados dos sensores
  noInterrupts();
  imprimeSensores(lum[1], hsolo[1], h_ar[1], temp[1]);
  interrupts();

  //Bypass do b1 (irrigacao manual)
  while(b1 == HIGH){
    irriga();
    b1 = digitalRead(b1Pin);
  }

  //Condicionais de irrigacao
  if(hsolo[0] >= especies[especie].limIrrig && hsolo[1] >= especies[especie].limIrrig){
    digitalWrite(solenoidePin, HIGH);
  }else if(hsolo[0] < especies[especie].limIrrig && hsolo[1] < especies[especie].limIrrig){
    digitalWrite(solenoidePin, LOW);
  }
   
  delay(1000);
}
