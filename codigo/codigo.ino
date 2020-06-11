
#include "DHT.h"
DHT dht(dhtPin, DHT11);
#include <LiquidCrystal.h>
LiquidCrystal lcd(10, 9, 11, 5, 6, 7);

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

/* Entrada */
#define b1Pin 4
#define b2Pin 2
#define b3Pin 3 

/* Variaveis globais */
#define nEspecies 2
#define nTelas 3
int primeiraLeitura;

//Arrays para armazenar valores dos sensores
int b1, lum[2], u_solo[2];
float u_ar[2], temp[2];

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
void leSensores(int* b_irrigacao, int* lum, int* u_solo, float* u_ar, float* temp){
  lum[0] = lum[1];
  u_solo[0] = u_solo[1];

  u_ar[0] = u_ar[1];
  temp[0] = temp[1];
  
  //Definicao das entradas dos botoes
  *b_irrigacao = digitalRead(b1Pin);

  //Armazenamento das entradas dos sensores
  lum[1] = analogRead(ldrPin);
  u_solo[1] = analogRead(higrometroPin);

  u_ar[1] = dht.readHumidity();
  temp[1] = dht.readTemperature();
}

//Mostra as informacoes dos sensores no display
void imprimeSensores(int lum, int u_solo, float u_ar, float temp){
  String linha_1;
  String linha_2;

  //Escolhe os dados que serao mostrados
  switch(indice_tela){
    case 0:
      linha_1 = "Umd. Solo: " + String(u_solo);
      linha_2 = "Lum.: " + String(lum);
      break;
    case 1:
      linha_1 = "Temp.: " +String(temp, 2) + " °C";
      linha_2 = "Umd. Ar: " + String(u_ar, 2);
      break;
    case 2:
      linha_1 = especies[especie].nome;
      linha_2 = "Lim. irrig: " + String(especies[especie].limIrrig);
      break;
    default:
      indice_tela = 0;
      linha_1 = "Umd. Solo: " + String(u_solo);
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

//Interrupcao que altera os dados mostrados no LCD
void scroll(){
  indice_tela++;
  if(indice_tela >= nTelas){
    indice_tela = 0;
  }
}

//Interrupcao que troca o indice da especie
void trocaDeEspecie(){
  especie++;
  if(especie >= nEspecies){
    especie = 0;  
  }  
}

//Liga o sistema de irrigacao por 10 s
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
  leSensores(&b1, lum, u_solo, u_ar, temp);

  //Conserta o erro de logica da primeira leitura (um termo [0] do array com valor aleatorio)
  if(primeiraLeitura){
    lum[0] = lum[1];
    u_solo[0] = u_solo[1];
    u_ar[0] = u_ar[1];
    temp[0] = temp[1];
    primeiraLeitura = 0;
  }

  //Imprime os dados dos sensores
  noInterrupts();
  imprimeSensores(lum[1], u_solo[1], u_ar[1], temp[1]);
  interrupts();

  //Bypass do b1 (irrigacao manual)
  while(b1 == HIGH){
    irriga();
    b1 = digitalRead(b1Pin);
  }

  //Condicionais de irrigacao
  if(u_solo[0] >= especies[especie].limIrrig && u_solo[1] >= especies[especie].limIrrig){
    digitalWrite(solenoidePin, HIGH);
  }else if(u_solo[0] < especies[especie].limIrrig && u_solo[1] < especies[especie].limIrrig){
    digitalWrite(solenoidePin, LOW);
  }
   
  delay(1000);
}
