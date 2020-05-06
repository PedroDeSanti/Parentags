//definição dos pinos do sistema
const int trigger_pin = 2;    // trigger  sensor ultrassonico
const int echo_pin = 3;       // echo     sensor ultrassonico
const int radiofr = 4;        // pino conectado ao modulo transmissor rf
const int buzzer_pin = 5;     // para fins de teste
const int led_red_pin = 6;    // led vermelho (só criança)
const int led_green_pin = 9;  // led verde (adulto)
const int infraRed1 = 12;     // sensor entrada
const int infraRed2 = 7;      // sensor no alto
const int infraRed3 = 8;      // sensor saida
const int rele_trava = 13;    // rele que controla a trava

//configuração tomada radio frequencia
byte address_l = 200;     // valor entre 0 e 255 para endereço LOW
byte address_h = 210;     // valor entre 0 e 255 para endereço HIGH
unsigned int alpha = 370; // tempo clock transmição (mudar se houver erro)

//configuração sensor ultrassonico de distância
float tempoEcho = 0;  // mede a duracao do tempo do echo
const float velocidadeSom_mpus = 0.000340;// em metros por microsegundo

//variaveis
int adulto = 0; //numero de adultos atualmente no comodo
int crianca = 0; //numero de criancas atualmente no comodo
float alturaCrianca;  // valor a partir do qual pode ser considerado adulto
int tAlarmeFalso = 1000; // duracao da analise dos sensores

//FUNÇÕES UTILIZADAS:

//função que retorna a distancia entre o objeto e o sensor
float distanciaUltrassonico() {
  digitalWrite(trigger_pin, HIGH);//Emitindo pulso ultrassonico
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);
  tempoEcho = pulseIn(echo_pin, HIGH);
  return (tempoEcho * velocidadeSom_mpus * 100) / 2;
}

//função que envia o pacote RF
void sendRF(byte dados) { //dados é valor entre 0 e 15 (4 bits menos significativos)
  byte aux1 = 0;
  byte aux2 = 0;
  byte loop_rf = 4; //numero de vezes que o pacote será enviado
  digitalWrite(radiofr, LOW);
  delay(10);
  aux1 = 0;
  while (aux1 < loop_rf) {
    digitalWrite(radiofr, HIGH);
    delayMicroseconds(alpha);
    digitalWrite(radiofr, LOW);
    delayMicroseconds(alpha * 30);
    sendBit(0); sendBit(0); sendBit(0); sendBit(0); //inicia enviando o enderço mais baixo, depois envia 4 bit zero
    
    aux2 = 0;
    while (aux2 < 8) {    //envio do byte menos significativo do enderço
      sendBit(bitRead(address_l, aux2));
      aux2 = aux2 + 1;
    }
    
    aux2 = 0;
    while (aux2 < 8) {    //envio do byte mais significativo do enderço
      sendBit(bitRead(address_h, aux2));
      aux2 = aux2 + 1;
    }
    
    aux2 = 0;
    while (aux2 < 4) {    //envio do byte de dados
      sendBit(bitRead(dados, aux2));
      aux2 = aux2 + 1;
    }
    
    aux1 = aux1 + 1;
  }
  digitalWrite(radiofr, LOW); //coloca o pino em nível baixo
}

//função que envia um bit
void sendBit(byte _bit) {
  if (_bit == 0) { // onda do tipo: '...
    digitalWrite(radiofr, HIGH);
    delayMicroseconds(alpha);
    digitalWrite(radiofr, LOW);
    delayMicroseconds(alpha * 3);
  }
  else  // onda do tipo: '''.
  {
    digitalWrite(radiofr, HIGH);
    delayMicroseconds(alpha * 3);
    digitalWrite(radiofr, LOW);
    delayMicroseconds(alpha);
  }
}

void entrando() {
  Serial.println("entrando");
  int e3 = 0, e2 = 0;
  unsigned long ti = millis();
  unsigned long tf = millis();

  //DETECTAR COM SENSOR INFRAVERMELHO

  /*while(tf-ti<=tAlarmeFalso){
    if(digitalRead(infraRed3)==LOW) e3=1;
    if(digitalRead(infraRed2)==LOW) e2=1;
    tf=millis();
    }
    if(e3==1&&e2==1){
    adulto++;
    checar();
    }
    else if(e3==1){
    crianca++;
    checar();
    }
    else Serial.println("alarme falso");*/

  //DETECTAR COM ULTRASSONICO
  float menorDist = 200; //menor distancia detectada
  while (tf - ti <= tAlarmeFalso) {
    if (digitalRead(infraRed3) == LOW) e3 = 1;
    menorDist = min(menorDist, distanciaUltrassonico());
    tf = millis();
  }
  if (e3 == 1) {
    if (menorDist > alturaCrianca) adulto++;
    else crianca++;
    checar();
  }
  else Serial.println("alarme falso");

}

void saindo() {
  Serial.println("saindo");
  int e1 = 0, e2 = 0;
  unsigned long ti = millis();
  unsigned long tf = millis();
  //DETECTAR COM INFRAVERMELHO

  /*while(tf-ti<=tAlarmeFalso){
    if(digitalRead(infraRed1)==LOW) e1=1;
    if(digitalRead(infraRed2)==LOW) e2=1;
    tf=millis();
    }
    if(e1==1&&e2==1){
    adulto--;
    checar();
    }
    else if(e1==1){
    crianca--;
    checar();
    }
    else Serial.println("alarme falso");*/

  // DETECTAR COM ULTRASSONICO
  float menorDist = 200; //menor distancia detectada
  while (tf - ti <= tAlarmeFalso) {
    if (digitalRead(infraRed1) == LOW) e1 = 1;
    menorDist = min(menorDist, distanciaUltrassonico());
    tf = millis();
  }
  if (e1 == 1) {
    if (menorDist > alturaCrianca) adulto--;
    else crianca--;
    checar();
  }
  else Serial.println("alarme falso");
}

void checar() {
  Serial.print("criancas:");
  Serial.print(crianca);
  Serial.print(" adultos:");
  Serial.print(adulto);
  Serial.println();
  if (adulto == 0) {
    //desligar tomada
    for(int i = 0; i<6; i++){ //repetição evitou erro nos teste
        sendRF(6); //função sendRF com o parametro 6 ->configurado com off na tomada
        delay(200);
      }
    digitalWrite(led_red_pin, HIGH);
    digitalWrite(rele_trava, HIGH);
  }
  if (adulto >= 1) {
    //ligar as tomadas
    for(int i = 0; i<6; i++){ //repetição evitou erro nos teste
        sendRF(4); //função sendRF com o parametro 6 ->configurado com on na tomada
        delay(200);
      }
    digitalWrite(led_green_pin, HIGH);
    digitalWrite(rele_trava, LOW);
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(trigger_pin, OUTPUT);
  digitalWrite(trigger_pin, LOW);
  pinMode(echo_pin, INPUT);
  pinMode(radiofr, OUTPUT);
  digitalWrite(radiofr, LOW);
  pinMode(buzzer_pin, OUTPUT); //para teste
  pinMode(led_red_pin, OUTPUT);
  pinMode(led_green_pin, OUTPUT);
  pinMode(infraRed1, INPUT);
  pinMode(infraRed2, INPUT);
  pinMode(infraRed3, INPUT);
  pinMode(rele_trava, OUTPUT);
  checar();
}

void loop()
{
  if (digitalRead(infraRed1) == LOW) entrando();
  if (digitalRead(infraRed3) == LOW) saindo();
  delay(10);
}
