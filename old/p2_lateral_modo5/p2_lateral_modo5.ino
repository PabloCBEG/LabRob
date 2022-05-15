#include <PID_v1.h>
#include <SoftwareSerial.h> 
#include <math.h>
SoftwareSerial BT(15, 14); // RX | TX 

//Controlador
double          Input, Output, Setpoint, Outputd, Outputi;
double          kp = 2, ki = 5, kd = 1;
PID controladord(&Input, &Output, &Setpoint, kp, ki, kd, DIRECT);
PID controladori(&Input, &Output, &Setpoint, kp, ki, kd, DIRECT);

//MACROS pines de manejo de los motores
int ENA = 10; //pin PWM motor derecho
int IN1 = 9;  //giro hacia adelante
int IN2 = 8;  //giro hacia atras
int ENB = 5;  //pin PWM motor izdo
int IN3 = 7;  //giro hacia adelante
int IN4 = 6;  //giro hacia atras

int IN5 = A10; //encoder derecha
int IN6 = A11; //encoder izquierda

unsigned long mytime = 0; //variable para almacenar el tiempo entre dos medidas
int modo = 5;             //modo de funcionamiento: 1=control frontal
int veli, veld;           //variables que usamos para el archivo que pasamos a telemetria: velocidades de las ruedas izda y dcha. Se actualizan cuando llamamos a las funciones de movimiento

int dbt=60, i; //velocidad angular referencia que introducimos por BT, inicializada a 60 (rpm)

String cad;     //para guardar la referencia

// ENCODER
const int encoderi = 19;//conectamos los encoders a los pines de interrupcion del arduino, el encoder derecho al pin 20 y el encoder izquierdo al pin 19
const int encoderd = 20;

int PPR = 192; //resultado de multiplicar 4*48 debido a la reducción del motor
volatile int conti = 0, contd = 0;//contadores que se encargan de calcular los pulsos en un segundo
unsigned int pulsosi = 0, pulsosd=0;
unsigned long Time = 0; 
unsigned int RPMi = 0, RPMd = 0;

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

  pinMode(IN5, INPUT);
  pinMode(IN6, INPUT);

  //ENCODER
  pinMode(encoderi, INPUT);//asiganmos los pines de los encoder como entradas
  pinMode(encoderd, INPUT);
  
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(encoderi), mediri, CHANGE);//Definimos nuestras interrupciones, una por cada rueda
  attachInterrupt(digitalPinToInterrupt(encoderd), medird, CHANGE);

  Time = millis();
  //Setup del PID
   Setpoint = 60; //60 rpm
   controladord.SetMode(AUTOMATIC);
   controladori.SetMode(AUTOMATIC);
}

void loop()
{ 
  //leo tiempo del sistema
  mytime = millis()-mytime;
  Time = millis();

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

  Setpoint = dbt; //actualizamos la referencia
  
  if(millis() - Time >=1000)//medimos el numero de vueltas en un segundo,
  {
    
    pulsosi = conti;
    RPMi = 60 * pulsosi/PPR;//calculamos rpm sabendo el número de vueltas en un segundo y su reducción(rueda izquierda)
    pulsosd = contd;
    RPMd = 60 * pulsosd/PPR;//calculamos rpm sabendo el número de vueltas en un segundo y su reducción(rueda derecha)
    
    
    conti = 0;         //reseteamos todas las variables
    pulsosi = 0;
    contd = 0;
    pulsosd = 0;
    Time = millis();
    
  }

  //derecha-------------------
  Input = RPMd;
  controladord.Compute();
  Outputd = Output;

  if (Outputd < 0)
  {
    digitalWrite (IN1, LOW);
    digitalWrite (IN2, HIGH);
    analogWrite (ENA, abs(Outputd));
  }
  else if (Outputd == 0)
  {
    digitalWrite (IN1, LOW);
    digitalWrite (IN2, LOW);
    analogWrite (ENA, Outputd);
  }
  else
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, Outputd);
  }
  //--------------------------

  //izquierda-----------------
  Input = RPMi;
  controladori.Compute();
  Outputi = Output;

  if (Outputi < 0)
  {
    digitalWrite (IN3, LOW);
    digitalWrite (IN4, HIGH);
    analogWrite (ENB, abs(Outputi));
  }
  else if (Outputi == 0)
  {
    digitalWrite (IN3, LOW);
    digitalWrite (IN4, LOW);
    analogWrite (ENB, Outputi);
  }
  else
  {
    digitalWrite (IN3, HIGH);
    digitalWrite (IN4, LOW);
    analogWrite (ENB, Outputi);
  }
  //--------------------------

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
 Serial.print(RPMd);
 Serial3.print(" ");
 Serial.print(RPMi);
 Serial3.print(" ");
 Serial3.print(Outputd);
 Serial3.print(" ");
 Serial.print(Outputi);
 Serial3.print("\n");*/

 /*Serial.println(RPMi);
 Serial.println(RPMd);
 Serial.println(" ");*/
}

void mediri()
{
  conti++;  
}

void medird()
{
  contd++;  
}
