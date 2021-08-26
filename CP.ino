#include <Adafruit_SSD1306.h>
#include<ArduinoJson.h>
#include <LiquidCrystal.h>
#define OLED_Address 0x3C
Adafruit_SSD1306 oled(128, 64);

const int TAMANHO = 200;
const float a = 0.8582;

//classificação de variáveis dentro da interrupção externa 
volatile unsigned long cont = 0;
unsigned long tempo = 0;

//Variável para cálculo de velocidade
float v;

int frequenciometro = 3;
int TermistorPin = A0;
int Lux = A1;
int Vo;
bool estadoBotao = true;
float R1 = 10000;
float logR2, R2, T, Tc, Tf, temp, lumi;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07; //valores constante para calculo

void setup() {
  
  pinMode(TermistorPin, INPUT);
  pinMode(Lux, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), interrupcao, RISING);
  
  Serial.begin(115200);
}

void loop() {
  StaticJsonDocument<TAMANHO> json; //cria o objeto Json
  
  //calcular a velocidade do vento

  if ((millis() - tempo) > 999) {
    
    tempo = millis();
    v = (2 * PI * cont * 3.6 * 0.147) / 60;
    cont = 0;
  }

  temp = readTemp(TermistorPin);
  lumi = readLumi(Lux);

  //formato de leitura no node-red
  json["topic"] = "sensor";
  json["lumi"] = lumi;
  json["temp"] = temp;
  json["vel"] = v;
    
  serializeJson(json, Serial);
  Serial.println();
}

//função que faz leitura da temperatura e retorna o valor em graus celcius
float readTemp(int ThermistorPin){
  
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0); //calculo R2, demonstração no arquivo pdf da aula
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));// Equação de Steinhart–Hart 
  Tc = T - 273.15; //temp em Graus celcius
  //Tf = (Tc * 9.0)/ 5.0 + 32.0; // temp em fahrenheit
  return Tc;
}

//função que faz leitura da luminosidade e retorna o valor
float readLumi(int Luminosidade) {
  float t1 = (analogRead(Luminosidade));
  float R2 = 10000 * (1023 / t1 - 1.0);
  float Lx = (127410 / R2) * exp(1 / a); //dark = 127410 a = 0.8582

  return Lx;
}

//função que executa a interrupção de frequência
void interrupcao () {
  cont++;
}
