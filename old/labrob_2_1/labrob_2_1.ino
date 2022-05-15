int ENA = 10;
int IN1 = 9;
int IN2 = 8;
int ENB = 5;
int IN3 = 7;
int IN4 = 6;

//int ecod = 22;
//int ecoi = 26;
//int trigd = 24;
//int trigi = 28;
int ecod = 26;
int ecoi = 22;
int trigd = 28;
int trigi = 24;
int duraciond, distanciad, duracioni, distanciai;
int dbt; //distancia referencia que introducimos por BT

void setup() {

pinMode(ENA, OUTPUT);
pinMode(ENB, OUTPUT);
pinMode(IN1, OUTPUT); //un valor HIGH en IN1 provoca que la rueda derecha gire hacia delante
pinMode(IN2, OUTPUT); //un valor HIGH en IN2 provoca que la rueda derecha gire hacia atrás
pinMode(IN3, OUTPUT); //un valor HIGH en IN3 provoca que la rueda izquierda gire hacia delante
pinMode(IN4, OUTPUT); //un valor HIGH en IN1 provoca que la rueda izquierda gire hacia atrás

pinMode(ecoi, INPUT);
pinMode(ecod, INPUT);
pinMode(trigi, OUTPUT);
pinMode(trigd, OUTPUT);

Serial.begin(9600);
}

void palante() {

  digitalWrite(IN1,HIGH);  //giran ambas ruedas hacia delante a "la misma" velocidad: el robot avanza en linea recta
  digitalWrite(IN2,LOW);
  analogWrite(ENA,100);   //valor 105 en ENA, porque la rueda está un poco descompensada
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  analogWrite(ENB,104);
}

void patras(){
 digitalWrite (IN1, LOW);
 digitalWrite (IN2, HIGH);
 analogWrite (ENA, 100);  //aquí parece que no hace falta el valor 105, cuando el robot va hacia atrás no es tan pronunciada la descompensación
 digitalWrite (IN3, LOW);
 digitalWrite (IN4, HIGH);
 analogWrite (ENB, 100);
  }

void pivote_dcha () //gira en el sitio hacia la izquierda
{  
 digitalWrite (IN1, HIGH);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 200);
  digitalWrite (IN3, LOW);
 digitalWrite (IN4, HIGH);
 analogWrite (ENB, 200);  
}


void pivote_izda () //gira en el sitio hacia la derecha
{  
 digitalWrite (IN1, LOW);
 digitalWrite (IN2, HIGH);
 analogWrite (ENA, 200);
digitalWrite (IN3, HIGH);
digitalWrite (IN4, LOW);
analogWrite (ENB, 200);  
}

void parar()
{  
 digitalWrite (IN1, LOW);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 0);
 digitalWrite (IN3, LOW);
 digitalWrite (IN4, LOW);
 analogWrite (ENB, 0);   
}

void giro_dcha () //gira hacia la izquierda
{  
 digitalWrite (IN1, HIGH);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 200);
  digitalWrite (IN3, HIGH);
 digitalWrite (IN4, LOW);
 analogWrite (ENB, 100);  
}

void giro_izda () //gira hacia la derecha
{  
 digitalWrite (IN1, HIGH);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 100);
  digitalWrite (IN3, HIGH);
 digitalWrite (IN4, LOW);
 analogWrite (ENB, 200);
}

void loop ()
{
  digitalWrite(trigd,LOW);
  delay(0.002);
  digitalWrite(trigd,HIGH);
  delay(0.01);
  digitalWrite(trigd,LOW);
  delay(0.01);
  duraciond = pulseIn(ecod,HIGH);
  
  digitalWrite(trigi,LOW);
  delay(0.002);
  digitalWrite(trigi,HIGH);
  delay(0.01);
  digitalWrite(trigi,LOW);
  delay(0.01);
  duracioni = pulseIn(ecoi,HIGH);

  distanciad = duraciond/58;///*****
  distanciai = duracioni/58;///*****
  delay(10);

 if(distanciad>30 && distanciai>30)
 {
   palante();
   Serial.println("hacia adelante");
   delay(10);
 }
 else if(distanciad<30 || distanciai<30)
 {
   patras();
   Serial.println("hacia atras");
   delay(10);
 }
 else parar();//patras();delay(10);

  Serial.println(distanciai);// distanciad);// distanciai);
  Serial.println(distanciad);
 }
