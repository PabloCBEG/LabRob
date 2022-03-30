#include <SoftwareSerial.h> 
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
int dbt=30, i; //distancia referencia que introducimos por BT, inicializada a 30 (cm)

String cad;     //para guardar la referencia
int errb, errf; //para calcular el error cometido y corregir
int vel;        //variable global para pasar la velocidad a las funciones palante, patras

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
  Serial3.begin(38400);//9600); //aunque el BT y el puerto serie en el ordenador y en PuTTy estan configurados a 9600 baud, aqui si ponemos 9600 se escacharra
  BT.begin(38400); 

  //definicion de los pines
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); //un valor HIGH en IN1 provoca que la rueda derecha gire hacia delante
  pinMode(IN2, OUTPUT); //un valor HIGH en IN2 provoca que la rueda derecha gire hacia atrás
  pinMode(IN3, OUTPUT); //un valor HIGH en IN3 provoca que la rueda izquierda gire hacia delante
  pinMode(IN4, OUTPUT); //un valor HIGH en IN1 provoca que la rueda izquierda gire hacia atrás
  
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

  
  
  parar();
  
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

  palante(130);
  delay(5);
  
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
 

    if(errf > 3)
    {
      giro_izda();
      delay(10);
    }
    else if(errf > 0 || errb > 0)
    {
      giro_izda();
      delay(5);
    }
    if(errb < -3)
    {
      giro_dcha();
      delay(10);
    }
    else if(errf < 0 || errb < 0)
    {
      giro_dcha();
      delay(5);
    }

    palante(130);
    delay(5);

 //sprintf(tiempo,"%lu",mytime);

 //tempus = mytime*10^(-6);

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
 Serial3.print("\n");

}

void palante(int vel)
{
  digitalWrite(IN1,HIGH);  //giran ambas ruedas hacia delante a "la misma" velocidad: el robot avanza en linea recta
  digitalWrite(IN2,LOW);
  analogWrite(ENA,vel);   //valor 105 en ENA, porque la rueda está un poco descompensada
  velb = vel;
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  analogWrite(ENB,vel+4);
  velf = vel;
}

void patras(int vel)
{
 digitalWrite (IN1, LOW);
 digitalWrite (IN2, HIGH);
 analogWrite (ENA, vel);  //aquí parece que no hace falta el valor 105, cuando el robot va hacia atrás no es tan pronunciada la descompensación
 velb = vel;
 digitalWrite (IN3, LOW);
 digitalWrite (IN4, HIGH);
 analogWrite (ENB, vel);
 velf = vel;
}

void parar()
{  
 digitalWrite (IN1, LOW);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 0);
 velb = 0;
 digitalWrite (IN3, LOW);
 digitalWrite (IN4, LOW);
 analogWrite (ENB, 0);
 velf = 0;
}

void pivote_dcha()  //gira en el sitio hacia la izquierda
{  
   digitalWrite (IN1, HIGH);
   digitalWrite (IN2, LOW);
   analogWrite (ENA, 100);
   digitalWrite (IN3, LOW);
   digitalWrite (IN4, HIGH);
   analogWrite (ENB, 100);  
}

void pivote_izda() //gira en el sitio hacia la derecha
{  
  digitalWrite (IN1, LOW);
  digitalWrite (IN2, HIGH);
  analogWrite (ENA, 100);
  digitalWrite (IN3, HIGH);
  digitalWrite (IN4, LOW);
  analogWrite (ENB, 100);  
}

void giro_dcha ()  //gira hacia la izquierda
{  
 digitalWrite (IN1, HIGH);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 120);
  digitalWrite (IN3, HIGH);
 digitalWrite (IN4, LOW);
 analogWrite (ENB, 110);  
}

void giro_izda () //gira hacia la derecha
{  
 digitalWrite (IN1, HIGH);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 110);
  digitalWrite (IN3, HIGH);
 digitalWrite (IN4, LOW);
 analogWrite (ENB, 120);
}
