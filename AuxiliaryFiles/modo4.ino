#include <SoftwareSerial.h> 
#include <math.h>
SoftwareSerial BT(15, 14); // RX | TX 

//MACROS pines de manejo de los motores
int ENA = 10; //pin PWM motor derecho
int IN1 = 9;  //giro hacia adelante
int IN2 = 8;  //giro hacia atras
int ENB = 5;  //pin PWM motor izdo
int IN3 = 7;  //giro hacia adelante
int IN4 = 6;  //giro hacia atras

unsigned long mytime = 0; //variable para almacenar el tiempo entre dos medidas
int modo = 1;             //modo de funcionamiento: 1=control frontal
int velf, velb;           //variables que usamos para el archivo que pasamos a telemetria: velocidades de las ruedas izda y dcha. Se actualizan cuando llamamos a las funciones de movimiento
//char tiempo[10];
//float tempus;

//MACROS pines sensores ultrasonidos
int ecob = 26;  //pin de lectura para sensor ultrasonidos (deras, back)
int ecof = 22;  //pin de lectura para sensor ultrasonidos (delane, fron)
int trigb = 28; //activacion sensor ultrasonidos (derecho)
int trigf = 24; //activacion sensor ultrasonidos (izdo)

//Variables para medir distancia con los sensores ultrasonidos
int duracionb, distanciab, duracionf, distanciaf;
int dbt=40, i; //distancia referencia que introducimos por BT, inicializada a 30 (cm)
double phi;
int distanciasensores = 15;

String cad;     //para guardar la referencia
int errb, errf; //para calcular el error cometido y corregir
int vel;        //variable global para pasar la velocidad a las funciones palante, patras
int flag = 0;

//prototipos de las funciones de actuacion
void palante(int);
void patras(int);
void parar(void);
void pivote_dcha(void);
void pivote_izda(void);
void giro_dcha (void);
void giro_izda (void);

void setup()
{
  Serial.begin(9600);
  Serial3.begin(38400);//9600); //aunque el BT y el puerto serie en el ordenador y en PuTTy estan configurados a 9600 baud, aqui si ponemos 9600 se escacharra
  BT.begin(38400); 

  //definicion de los pines
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); //un valor HIGH en IN1 provoca que la rueda izqda gire hacia delante
  pinMode(IN2, OUTPUT); //un valor HIGH en IN2 provoca que la rueda izqda gire hacia atrás
  pinMode(IN3, OUTPUT); //un valor HIGH en IN3 provoca que la rueda dcha gire hacia delante
  pinMode(IN4, OUTPUT); //un valor HIGH en IN4 provoca que la rueda dcha gire hacia atrás
  
  pinMode(ecof, INPUT);
  pinMode(ecob, INPUT);
  pinMode(trigf, OUTPUT);
  pinMode(trigb, OUTPUT);
}

void loop()
{ 
  //activar y leer ultrasonidos
  digitalWrite(trigb,LOW);
  delay(0.002);
  digitalWrite(trigb,HIGH);
  delay(0.01);
  digitalWrite(trigb,LOW);
  delay(0.01);
  duracionb = pulseIn(ecob,HIGH);
  
  digitalWrite(trigf,LOW);
  delay(0.002);
  digitalWrite(trigf,HIGH);
  delay(0.01);
  digitalWrite(trigf,LOW);
  delay(0.01);
  duracionf = pulseIn(ecof,HIGH);

  //calculo de la distancia al obstaculo segun la medida de los sensores
  distanciab = duracionb/58;
  distanciaf = duracionf/58;

  //leo tiempo del sistema
  mytime = millis()-mytime;
  
  delay(10);
  errb = dbt-distanciab;
  errf = dbt-distanciaf;
//----------------------------------------------------------------------------------------------------------------------------
  phi = atan2((distanciab-distanciaf),distanciasensores);
  phi = phi*180/3.141592;

  if(BT.available())    // Si llega un dato por el puerto BT se envía al monitor serial
  {
    Serial3.write(BT.read());
    Serial3.println("Bluetooth conectado");
  }
 
  if(Serial3.available())  // Si llega un dato por el monitor serial se envía al puerto BT
  {
     Serial3.println("Referencia: "); //indica que le pasamos referencia
     cad = Serial3.readString();//-48;
     if(cad.length()>3 || dbt>100) Serial3.println("Referencia demasiado grande. Introduzca una menor");
     else
     {
        for(i=0;i<cad.length();i++)
        {
          if(cad.length()==3)
          {
            dbt = (cad[0]-48)*100+(cad[1]-48)*10+cad[2]-48;
          }
          else if(cad.length()==2)
          {
            dbt = (cad[0]-48)*10+cad[1]-48;
          }
          else dbt = (cad[0]-48);
        }
      }
      flag = 0;
      BT.write(dbt);
      Serial3.println(dbt);
      delay(1000);
  }
  
  //comunicacion puerto serie usb
  /*if(Serial.available()){
    Serial.println("Referencia: ");
    cad = Serial.readString();//-48;
    if(cad.length()>2) Serial.println("Referencia demasiado grande. Introduzca una menor que 100");
    else{
      for(i=0;i<cad.length();i++)
      {
        if(cad.length()==2){
        dbt = (cad[0]-48)*10+cad[1]-48;
        }
        else dbt = (cad[0]-48);
      }
    }
  Serial.println(dbt);
  delay(1000);
  }*/
//--------------------------
  velf = 125;
  velb = 125;

    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
    analogWrite(ENB,125);
    
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    analogWrite(ENA,velb);
//--------------------------

    /*if((distanciaf+distanciab)/2 > dbt)
    {
      if((distanciaf+distanciab)/2 > dbt+6)
      {
        digitalWrite(IN3,HIGH);
        digitalWrite(IN4,LOW);
        analogWrite(ENB,110);
        
        digitalWrite(IN1,HIGH);
        digitalWrite(IN2,LOW);
        analogWrite(ENA,125);
      }
      else if((distanciaf+distanciab)/2 > dbt+3)
      {
        digitalWrite(IN3,HIGH);
        digitalWrite(IN4,LOW);
        analogWrite(ENB,105);
        
        digitalWrite(IN1,HIGH);
        digitalWrite(IN2,LOW);
        analogWrite(ENA,115);
      }
    }
    else if((distanciaf+distanciab)/2 < dbt)
    {
      if((distanciaf+distanciab)/2 < dbt-6)
      {
        digitalWrite(IN3,HIGH);
        digitalWrite(IN4,LOW);
        analogWrite(ENB,135);
        
        digitalWrite(IN1,HIGH);
        digitalWrite(IN2,LOW);
        analogWrite(ENA,100);
      }
      if((distanciaf+distanciab)/2 < dbt-3)
      {
        digitalWrite(IN3,HIGH);
        digitalWrite(IN4,LOW);
        analogWrite(ENB,120);
        
        digitalWrite(IN1,HIGH);
        digitalWrite(IN2,LOW);
        analogWrite(ENA,100);
      }
    }
    else
    {
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
      analogWrite(ENB,135);
      
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      analogWrite(ENA,125);
    }*/
    
    
    if(((distanciaf+distanciab)/2 < dbt-6 || (distanciaf+distanciab)/2 > dbt+6))
    {
      //Serial3.println("Corrigiendo distancia");
      if((distanciaf+distanciab)/2 < dbt-6)// && phi>-30)// && phi<30)
      {
      velb = 100;
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
      analogWrite(ENB,135);
      
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      analogWrite(ENA,velb);
      
      delay(5);
      }
      else if((distanciaf+distanciab)/2 > dbt+6)// && phi<30)// && phi>-30)
      {
      velb = 125;
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
      analogWrite(ENB,110);
      
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      analogWrite(ENA,velb);
      
      delay(5);
      }
      else
      {
        velb = 125;
    
        digitalWrite(IN3,HIGH);
        digitalWrite(IN4,LOW);
        analogWrite(ENB,135);
        
        digitalWrite(IN1,HIGH);
        digitalWrite(IN2,LOW);
        analogWrite(ENA,velb);

        delay(5);
      }

       //Serial3.println(flag);
    }
    //else flag = 1;
    else if(((distanciaf+distanciab)/2 < dbt-3 || (distanciaf+distanciab)/2 > dbt+3))
    {
      if((distanciaf+distanciab)/2 < dbt-3)// && phi>-30)
      {
      velb = 100;
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
      analogWrite(ENB,120);
      
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      analogWrite(ENA,velb);
      
      delay(5);
      }
      else if((distanciaf+distanciab)/2 > dbt+3)// && phi<30)// && phi>-30)
      {
      velb = 115;
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
      analogWrite(ENB,105);
      
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      analogWrite(ENA,velb);
      
      delay(5);
      }
      else
      {
        velb = 135;
    
        digitalWrite(IN3,HIGH);
        digitalWrite(IN4,LOW);
        analogWrite(ENB,125);
        
        digitalWrite(IN1,HIGH);
        digitalWrite(IN2,LOW);
        analogWrite(ENA,velb);

        delay(5);

        flag = 1;
      }
    }
    
    
    if(phi>0 && flag==1) //se esta acercando a la pared, quiero que gire a la izda=> larueda dcha tiene que girar mas rapido
    {
      velb = 90;
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
      analogWrite(ENB,125);
      
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      analogWrite(ENA,velb);
      
      delay(5);
    }
    else if(phi<0 && flag==1)
    {
      velb = 160;
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
      analogWrite(ENB,135);
      
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      analogWrite(ENA,velb);
      
      delay(5);
    }
    else
    {
      velb = 120;
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
    analogWrite(ENB,130);
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    analogWrite(ENA,velb);

    //flag = 0;
    delay(5);
    }
    /*else if(distanciab != dbt)
    {
      palante(120);
    }*/
    //flag = 0;
 Serial3.print(mytime);
 Serial3.print(" ");
 Serial3.print(distanciaf);
 Serial3.print(" ");
 Serial3.print(distanciab);
 Serial3.print(" ");
 Serial3.print(dbt);
 Serial3.print(" ");
 Serial3.print(modo);//modo 1, 2, 3, 4, 5 o 6
 Serial3.print(" ");
 Serial3.print(velf);
 Serial3.print(" ");
 Serial3.print(velb);
 Serial3.print(" ");
 Serial3.print(flag);
 Serial3.print("\n");

 /*Serial3.println(velb);
 Serial3.println(flag);*/
}
