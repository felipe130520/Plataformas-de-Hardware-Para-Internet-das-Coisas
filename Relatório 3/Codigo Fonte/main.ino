const int PIN_TRIG = 27; 
const int PIN_ECHO = 26; 

volatile unsigned long startTime = 0;
volatile unsigned long echoDuration = 0;
volatile bool newMeasurementReady = false;

// Rotina de Serviço de Interrupção para o ESP32
void IRAM_ATTR handleEcho() {
  if (digitalRead(PIN_ECHO) == HIGH) {
    startTime = micros(); // Captura o tempo na borda de subida (Início do Eco)
  } else {
    echoDuration = micros() - startTime; // Captura o tempo na borda de descida (Fim do Eco)
    newMeasurementReady = true;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  digitalWrite(PIN_TRIG, LOW);

  // Configura a interrupção no pino ECHO para disparar em qualquer mudança de estado (CHANGE)
  attachInterrupt(digitalPinToInterrupt(PIN_ECHO), handleEcho, CHANGE);
}

void loop() {
  // Reseta a flag de medição antes de disparar um novo pulso
  newMeasurementReady = false;

  // Dispara o pulso de Trigger (conforme o código base)
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(11);
  digitalWrite(PIN_TRIG, LOW);

  // Aguarda a conclusão da interrupção com um timeout de segurança (aprox. 60ms)
  unsigned long timeout = millis();
  while (!newMeasurementReady && (millis() - timeout) < 60) {
    // Aguarda o sinal ECHO retornar
  }

  float distancia = 0.0;

  // Se a medição foi concluída com sucesso, calcula a distância
  if (newMeasurementReady) {
    distancia = echoDuration / 58.0;
  }

  // Validação dos limites do sensor HC-SR04 (2 cm a 400 cm)
  if (distancia < 2.0 || distancia > 400.0 || !newMeasurementReady) {
    distancia = 0.0;
  }

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  delay(500); // Intervalo entre as leituras
}