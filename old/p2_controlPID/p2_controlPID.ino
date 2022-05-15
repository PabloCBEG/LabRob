
#include <PID_v1.h>

#include <SoftwareSerial.h>
SoftwareSerial BT(15, 14); // RX | TX
/*

*/

//Variables para la implementacion del PID
//unsigned long   lastTime = 0;
double          Input, Output, Setpoint, Outputd, Outputi;
//double          ITerm, lastInput;
double          kp = 2, ki = 5, kd = 1;
//int             SampleTime = 1000; //1 sec
//double          outMin = -10, outMax = 20;
//bool            inAuto = false;

//prototipos de las funciones que usamos de la libreria Arduino PID
//void PID.Compute();
//void PID.SetTunings(double, double, double);
//void PID.SetSampleTime(int);
//void PID.SetOutputLimits(double, double);
//void PID.SetMode(int);
//void PID.Initialize();  //la llama SetMode
//void PID.SetControllerDirection(int);

//int newsampt = 500; //ms => 0.5s

//limites de la senal de control //primero hemos de saber sobre que estamos actuando con la sc
//double minimo = -10;
//double maximo = 20;

//variable para el modo de funcionamiento. Normalmente dejaremos el modo en automatico.
//int modo = 1; //segun la definicion de antes, automatico = 1

//variable para la direccion de funcionamiento
//int direccion = 0; //direct control. Control inverso (1) seria cuando para una actuacion positiva se espera una respuesta negativa

//MACROS pines de manejo de los motores
int ENA = 10; //pin PWM motor derecho
int IN1 = 9;  //giro hacia adelante
int IN2 = 8;  //giro hacia atras
int ENB = 5;  //pin PWM motor izdo
int IN3 = 7;  //giro hacia adelante
int IN4 = 6;  //giro hacia atras

unsigned long mytime = 0; //variable para almacenar el tiempo entre dos medidas
//int modo = 1;             //modo de funcionamiento: 1=control frontal
int veli, veld;           //variables que usamos para el archivo que pasamos a telemetria: velocidades de las ruedas izda y dcha. Se actualizan cuando llamamos a las funciones de movimiento
//char tiempo[10];
//float tempus;

//MACROS pines sensores ultrasonidos
int ecod = 26;  //pin de lectura para sensor ultrasonidos (derecho)
int ecoi = 22;  //pin de lectura para sensor ultrasonidos (izdo)
int trigd = 28; //activacion sensor ultrasonidos (derecho)
int trigi = 24; //activacion sensor ultrasonidos (izdo)

//Variables para medir distancia con los sensores ultrasonidos
int duraciond, distanciad, duracioni, distanciai;
int dbt = 30, i; //distancia referencia que introducimos por BT, inicializada a 30 (cm)

String cad;     //para guardar la referencia
int errd, erri; //para calcular el error cometido y corregir
int vel;        //variable global para pasar la velocidad a las funciones palante, patras

//prototipos de las funciones de actuacion
//void palante(int);
//void patras(int);
//void parar(void);
//void pivote_dcha(void);
//void pivote_izda(void);

PID controladord(&Input, &Output, &Setpoint, kp, ki, kd, DIRECT);
PID controladori(&Input, &Output, &Setpoint, kp, ki, kd, DIRECT);

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

  pinMode(ecoi, INPUT);
  pinMode(ecod, INPUT);
  pinMode(trigi, OUTPUT);
  pinMode(trigd, OUTPUT);

 //Setup del PID
 Setpoint = 30; //30 cm de la pared
 controladord.SetMode(AUTOMATIC);
 controladori.SetMode(AUTOMATIC);
}

void loop()
{
  //activar y leer ultrasonidos
  digitalWrite(trigd, LOW);
  delay(0.002);
  digitalWrite(trigd, HIGH);
  delay(0.01);
  digitalWrite(trigd, LOW);
  delay(0.01);
  duraciond = pulseIn(ecod, HIGH);

  digitalWrite(trigi, LOW);
  delay(0.002);
  digitalWrite(trigi, HIGH);
  delay(0.01);
  digitalWrite(trigi, LOW);
  delay(0.01);
  duracioni = pulseIn(ecoi, HIGH);

  //calculo de la distancia al obstaculo segun la medida de los sensores
  distanciad = duraciond / 58;
  distanciai = duracioni / 58;

  //leo tiempo del sistema
  mytime = millis() - mytime;

  delay(10);
  errd = abs(dbt - distanciad);
  erri = abs(dbt - distanciai);

  //Comunicacion BT para establecer la referencia
  if (BT.available())   // Si llega un dato por el puerto BT se envía al monitor serial
  {
    Serial3.write(BT.read());
    Serial3.println("Bluetooth conectado");
  }

  if (Serial3.available()) // Si llega un dato por el monitor serial se envía al puerto BT
  {
    Serial3.println("Referencia: "); //indica que le pasamos referencia
    cad = Serial3.readString();//-48;
    if (cad.length() > 3 || dbt > 300) Serial3.println("Referencia demasiado grande. Introduzca una menor que 300");
    else
    {
      for (i = 0; i < cad.length(); i++)
      {
        if (cad.length() == 3)
        {
          dbt = (cad[0] - 48) * 100 + (cad[1] - 48) * 10 + cad[2] - 48;
        }
        else if (cad.length() == 2)
        {
          dbt = (cad[0] - 48) * 10 + cad[1] - 48;
        }
        else dbt = (cad[0] - 48);
      }
    }
    BT.write(dbt);
    Serial3.println(dbt);
    delay(1000);
  }

  //actualizamos la referencia
  Setpoint = dbt;

  //El controlador debe ejecutarse cada vez para cada una de las ruedas. Es posible que haya que las constantes de control sean distintas para cada rueda
  //el controlador que estamos implementando aqui es en posicion, no en velocidad... si peta, esto puede ser el motivo

  //derecha
  Input = distanciad;
  controladord.Compute();
  Outputd = Output;
  //controladord.SetTunings(kp, ki, kd);
  //controladord.SetSampleTime(newsampt);
  //controladord.SetOutputLimits(minimo, maximo);
  //controladord.SetMode(modo);
  //controladord.SetControllerDirection(direccion);

  if (Output < 0)
  {
    digitalWrite (IN1, LOW);
    digitalWrite (IN2, HIGH);
    analogWrite (ENA, abs(Output));
  }
  else if (Output == 0)
  {
    digitalWrite (IN1, LOW);
    digitalWrite (IN2, LOW);
    analogWrite (ENA, Output);
  }
  else
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, Output);
  }

  //izquierda
  Input = distanciai;
  controladori.Compute();
  Outputi = Output;
  //controladori.SetTunings(kp, ki, kd);
  //controladori.SetSampleTime(newsampt);
  //controladori.SetOutputLimits(minimo, maximo);
  //controladori.SetMode(modo);
  //controladori.SetControllerDirection(direccion);

  if (Output < 0)
  {
    digitalWrite (IN3, LOW);
    digitalWrite (IN4, HIGH);
    analogWrite (ENB, abs(Output));
  }
  else if (Output == 0)
  {
    digitalWrite (IN3, LOW);
    digitalWrite (IN4, LOW);
    analogWrite (ENB, Output);
  }
  else
  {
    digitalWrite (IN3, HIGH);
    digitalWrite (IN4, LOW);
    analogWrite (ENB, Output);
  }

  //sprintf(tiempo,"%lu",mytime);

  //tempus = mytime*10^(-6);

  Serial3.print(mytime);
  Serial3.print(" ");
  Serial3.print(distanciai);
  Serial3.print(" ");
  Serial3.print(distanciad);
  Serial3.print(" ");
  Serial3.print(dbt);
  Serial3.print(" ");
  //Serial3.print(modo);//modo 1, 2, 3, 4, 5 o 6
  //Serial3.print(" ");
  Serial3.print(veli);
  Serial3.print(" ");
  Serial3.print(veld);
  Serial3.print("\n");

}
