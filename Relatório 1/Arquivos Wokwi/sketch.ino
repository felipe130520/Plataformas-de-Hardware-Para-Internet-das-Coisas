// Definição Pinos 
const int PIN_POT = 32; //Entrada analogica do potenciometro
const int PIN_LDR = 34; //Entrada analogica do LDR
const int PIN_BTN = 26; //Entrada Botão
const int PIN_LED1 = 16; //Saída PWN / Intensidade LED1
const int PIN_LED2 = 17; // Saída Digital / Piscar LED2

bool modoAutomatico = false;
int ultimoEstadoBtn = HIGH;
unsigned long ultimoTempBounce = 0;
const unsigned long DELAY_DeBOUNCE = 50;

unsigned long ultimoTempPisca = 0;
bool estadoLED2 = LOW;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);


  pinMode(PIN_POT, INPUT);
  pinMode(PIN_LDR, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // LEITURA DO BOTÃO 
  int leituraBTN = digitalRead(PIN_BTN);
  if( leituraBTN != ultimoEstadoBtn){
    ultimoTempBounce = millis();
  }

  if((millis() - ultimoTempBounce) > DELAY_DeBOUNCE){
    static int estadoEstavelBTN = HIGH;
    if(leituraBTN != estadoEstavelBTN){
      estadoEstavelBTN = leituraBTN;
      if(estadoEstavelBTN == LOW){
        modoAutomatico = !modoAutomatico;
        Serial.print("Modo alterado para: ");
        Serial.println(modoAutomatico ? "Automatico (LDR)" : "Manual (POTENCIOMETRO)");
      }
    }
  }
  ultimoEstadoBtn = leituraBTN;

  int valorADC = 0;
  if(modoAutomatico) {
    valorADC = analogRead(PIN_LDR);
  } else {
    valorADC = analogRead(PIN_POT);
  }


  int limiteMaxADC = 1240;
  valorADC = constrain(valorADC, 0, limiteMaxADC);

  int pwmLED1 = map(valorADC, 0, limiteMaxADC, 0, 255);

  int tempoPiscaLED2 = map(valorADC, 0, limiteMaxADC, 100, 1000);

  analogWrite(PIN_LED1, pwmLED1);

  unsigned long tempoAtual = millis();
  if(tempoAtual - ultimoTempPisca >= tempoPiscaLED2){
    ultimoTempPisca = tempoAtual;
    estadoLED2 = !estadoLED2;
    digitalWrite(PIN_LED2, estadoLED2);
  }

}
