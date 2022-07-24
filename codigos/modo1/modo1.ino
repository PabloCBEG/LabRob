
int modo = 1;

double output, input, setpoint, error, outputsum, printoutput;
double lastInput = input;
double dInput;
double senal;
double outmax = 255;
double outmin = -255; //just for representation. Actual output minimum is 0 (PWM), but
                      //we'd like to represent backwards spinning as negative
unsigned long timechange, sampletime, lastime, now;
double kp, ki, kd;
int d; //direccion de giro; d = 1, palante.

//MACROS pines de manejo de los motores
int ENA = 10; //pin PWM motor derecho
int IN1 = 9;  //giro hacia adelante
int IN2 = 8;  //giro hacia atras
int ENB = 5;  //pin PWM motor izdo
int IN3 = 7;  //giro hacia adelante
int IN4 = 6;  //giro hacia atras

//MACROS pines sensores ultrasonidos
int ecod = 26;  //pin de lectura para sensor ultrasonidos (derecho)
int ecoi = 22;  //pin de lectura para sensor ultrasonidos (izdo)
int trigd = 28; //activacion sensor ultrasonidos (derecho)
int trigi = 24; //activacion sensor ultrasonidos (izdo)

//Variables para medir distancia con los sensores ultrasonidos
int duraciond, distanciad, duracioni, distanciai;

String cad;     //para guardar la referencia
//int errd, erri; //para calcular el error cometido y corregir
//int vel;        //variable global para pasar la velocidad a las funciones palante, patras
int i;

void setup() {
  Serial3.begin(38400);
  
  // Parametros del control
  setpoint = 100;
  input = 31;
  
  sampletime = 10;
  
  lastime = millis() - sampletime;

  kp = 2; ki = 5; kd = 1;

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
}

void loop() {
  outputsum = 0;
  ultrasonidos();
  //mytime = millis() - mytime;

  //Cada tiempo de muestreo, calculo la senal de control
  now = millis();
  timechange = now - lastime;
  
  if(timechange >= sampletime)
  {
    input = distanciad; //Para el primer modo me interesa una distancia generica
                        //No necesito paralelismo a la pared
                        //Uso la distancia con el sensor derecho, por ejemplo
    error = input - setpoint;
    dInput = input - lastInput;

    outputsum += ki * error;
    senal = kp * error;
    if(abs(error) < 3) senal = senal*2;
    senal += outputsum - kd * dInput;

    if(senal < 0)
  {
    d = 0;
    printoutput = senal;
    senal = -senal;
  }
  else
  {
    d = 1;
    printoutput = senal;
  }

  if(senal > outmax)
  {
    senal = outmax;
    if(printoutput < 0) printoutput = outmin;
    else printoutput = outmax;
  }

    output = senal;

    lastInput = input;
    lastime = now;
  }

  //Movimiento de las ruedas
    if(d==1)
    {
      digitalWrite (IN1, HIGH);
      digitalWrite (IN2, LOW);
      analogWrite (ENA, output);
      digitalWrite (IN3, HIGH);
      digitalWrite (IN4, LOW);
      analogWrite (ENB, output);
    }
    else
    {
      digitalWrite (IN1, LOW);
      digitalWrite (IN2, HIGH);
      analogWrite (ENA, output);
      digitalWrite (IN3, LOW);
      digitalWrite (IN4, HIGH);
      analogWrite (ENB, output);
    }

  //Lectura del cambio de referencia
    if(Serial3.available())
    {
       Serial3.println("Referencia: "); //indica que le pasamos referencia
       cad = Serial3.readString();//-48;
       if(cad.length()>3 || setpoint>300) Serial3.println("Referencia demasiado grande. Introduzca una menor que 300");
       else{
        for(i=0;i<cad.length();i++)
        {
          if(cad.length()==3){
          setpoint = (cad[0]-48)*100+(cad[1]-48)*10+cad[2]-48;
          }
          else if(cad.length()==2){
          setpoint = (cad[0]-48)*10+cad[1]-48;
          }
          else setpoint = (cad[0]-48);
        }
       }
    }

  //Mostrar por pantalla los resultados // generar archivo de telemetria
    Serial3.print(timechange); //tiempo desde la anterior medida
    Serial3.print(" ");
    Serial3.print(distanciai); //distancia medida sensor izquierdo
    Serial3.print(" ");
    Serial3.print(input); //distancia medida sensor (en este modo 1 usamos solo el derecho)
    Serial3.print(" ");
    Serial3.print(setpoint); //referencia
    Serial3.print(" ");
    Serial3.print(modo); //modo de funcionamiento
    Serial3.print(" ");
    Serial3.print(printoutput); //velocidad PWM motores. En este modo 1, ambos llevan la misma
    Serial3.print(" ");
    Serial3.println(printoutput); //velocidad PWM motores. En este modo 1, ambos llevan la misma
}

void ultrasonidos()
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
}
