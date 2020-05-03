//definição dos pinos do sistema
const int trigger_pin = 2;    // trigger  sensor ultrassonico
const int echo_pin = 3;       // echo     sensor ultrassonico
const int radiofr = 4;
const int buzzer_pin = 5;     // para fins de teste
const int led_red_pin = 6;    // led vermelho (só criança)
const int led_green_pin = 9;  // led verde (adulto)
const int infraRed1 = 12;     // sensor entrada
const int infraRed2 = 7;      // sensor no alto
const int infraRed3 = 8;      // sensor saida
const int rele_trava = 13;      // rele que controla a trava

//variaveis
int adulto=0;   //numero de adultos atualmente no comodo
int crianca=0;  //numero de criancas atualmente no comodo

float alturaCrianca;  // valor a partir do qual pode ser considerado adulto
int tAlarmeFalso=1000;// duracao da analise dos sensores
float tempoEcho = 0;  // mede a duracao do tempo do echo
const float velocidadeSom_mpus = 0.000340;// em metros por microsegundo



float distanciaUltrassonico(){
  digitalWrite(trigger_pin, HIGH);//Emitindo pulso ultrassonico               
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW); 
  tempoEcho = pulseIn(echo_pin, HIGH);
  return (tempoEcho*velocidadeSom_mpus*100)/2;  
}

void entrando(){
  Serial.println("entrando");
  int e3=0,e2=0;
  unsigned long ti=millis();
  unsigned long tf=millis();

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

// DETECTAR COM ULTRASSONICO
  float menorDist=200; //menor distancia detectada
  while(tf-ti<=tAlarmeFalso){
    if(digitalRead(infraRed3)==LOW) e3=1;
    menorDist=min(menorDist,distanciaUltrassonico());
    tf=millis();
  }
  if(e3==1){
    if(menorDist>alturaCrianca) adulto++;
    else crianca++;
    checar();
  }
  else Serial.println("alarme falso");
 
}


void saindo(){
  Serial.println("saindo");
  int e1=0,e2=0;
  unsigned long ti=millis();
  unsigned long tf=millis();
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
  float menorDist=200; //menor distancia detectada
  while(tf-ti<=tAlarmeFalso){
    if(digitalRead(infraRed1)==LOW) e1=1;
    menorDist=min(menorDist,distanciaUltrassonico());
    tf=millis();
  }
  if(e1==1){
    if(menorDist>alturaCrianca) adulto--;
    else crianca--;
    checar();
  }
  else Serial.println("alarme falso");
}

void checar(){
  Serial.print("criancas:");
  Serial.print(crianca);
  Serial.print(" adultos:");
  Serial.print(adulto); 
  Serial.println();
  if(adulto==0){
    //desligar tomada
    digitalWrite(rele_trava,HIGH);
  }
  if(adulto>=1){
    //ligar as tomadas
    digitalWrite(rele_trava,LOW);
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(infraRed1,INPUT);
  pinMode(infraRed2,INPUT);
  pinMode(infraRed3,INPUT); 
  pinMode(rele_trava,OUTPUT);
  pinMode(trigger_pin, OUTPUT);
  digitalWrite(trigger_pin, LOW);
  pinMode(echo_pin, INPUT);
  pinMode(led_red_pin, OUTPUT);
  pinMode(led_green_pin, OUTPUT);
  checar();
}

void loop()
{
  if(digitalRead(infraRed1)==LOW) entrando();
  if(digitalRead(infraRed3)==LOW) saindo();
  delay(10); 
}
