
int modo = 5;

//MACROS pines de manejo de los motores
int ENA = 10; //pin PWM motor derecho
int IN1 = 9;  //giro hacia adelante
int IN2 = 8;  //giro hacia atras
int ENB = 5;  //pin PWM motor izdo
int IN3 = 7;  //giro hacia adelante
int IN4 = 6;  //giro hacia atras

//int IN5 = A10; //encoder derecha
//int IN6 = A11; //encoder izquierda

//MACROS pines sensores ultrasonidos
int ecod = 26;  //pin de lectura para sensor ultrasonidos (derecho)
int ecoi = 22;  //pin de lectura para sensor ultrasonidos (izdo)
int trigd = 28; //activacion sensor ultrasonidos (derecho)
int trigi = 24; //activacion sensor ultrasonidos (izdo)

// ENCODER
const int encoderi = 3;//conectamos los encoders a los pines de interrupcion del arduino, el encoder derecho al pin 2 y el encoder izquierdo al pin 3
const int encoderd = 2;
int PPR = 192; //resultado de multiplicar 4*48 debido a la reducción del motor
volatile int conti = 0, contd = 0;//contadores que se encargan de calcular los pulsos en un segundo
unsigned int pulsosi = 0, pulsosd=0;
unsigned long Time = 0; 
unsigned int RPMi = 0, RPMd = 0;

//Variables para medir distancia con los sensores ultrasonidos
double duraciond, duracioni;

  double output, input, setpoint, error, outputsum, printoutput;
  double dInput;
  double output1, input1, error1, printoutput1;
  double output2, input2, error2, printoutput2;
  double lastInput;
  double senal;
  double outmax = 255;
  double outmin = -255; //just for representation. Actual output minimum is 0 (PWM), but
                        //we'd like to represent backwards spinning as negative
  unsigned long now, sampletime, lastime, timechange;
  double kp, ki, kd;
  int d, d1, d2; //direccion de giro; d = 1, palante.
  String cad;     //para guardar la referencia
  double distanciad, distanciai;

//Functions
void setpointBT()
{
  int i;
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
void control(double velocidad, unsigned long now)
{
  input = velocidad;
  error = setpoint - input;
  dInput = input - lastInput;

  outputsum += ki * error;

  senal = kp * error;

  senal += outputsum - kd * dInput;
  if(abs(error) < 10) senal = senal*0.5;

  if(senal < 0)
  {
    d = 0;
    printoutput = senal;
    if(printoutput < outmin) printoutput = outmin;
    senal = -senal;      
  }
  else d = 1;

  if(senal > outmax)
  {
    senal = outmax;
    printoutput = senal;
  }

  output = senal;

  lastInput = input;
  lastime = now;
}
void mediri()
{
  conti++;  
}
void medird()
{
  contd++;  
}

void setup() {

  Serial3.begin(38400);
  
  // Parametros del control
  setpoint = 0; //60rpm
  input = 0;
  
  sampletime = 100;
  
  lastime = millis() - sampletime;

  kp = 1; ki = 5; kd = 2;

  //definicion de los pines
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); //un valor HIGH en IN1 provoca que la rueda derecha gire hacia delante
  pinMode(IN2, OUTPUT); //un valor HIGH en IN2 provoca que la rueda derecha gire hacia atrás
  pinMode(IN3, OUTPUT); //un valor HIGH en IN3 provoca que la rueda izquierda gire hacia delante
  pinMode(IN4, OUTPUT); //un valor HIGH en IN1 provoca que la rueda izquierda gire hacia atrás

  //pinMode(IN5, INPUT);
  //pinMode(IN6, INPUT);

  //ENCODER
  pinMode(encoderi, INPUT);//asignamos los pines de los encoder como entradas
  pinMode(encoderd, INPUT);
  
  //Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(encoderi), mediri, CHANGE);//Definimos nuestras interrupciones, una por cada rueda
  attachInterrupt(digitalPinToInterrupt(encoderd), medird, CHANGE);

  pinMode(ecoi, INPUT);
  pinMode(ecod, INPUT);
  pinMode(trigi, OUTPUT);
  pinMode(trigd, OUTPUT);
}

void loop() {

  setpointBT();
  ultrasonidos();
  
  //Cada tiempo de muestreo, calculo la senal de control
  now = millis();
  timechange = now - lastime;

  //ENCODER
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
  
  if(timechange >= sampletime)
  {
    outputsum = 0;
    control(RPMd, now);
    input1 = input;
    output1 = output;
    printoutput1 = printoutput;
    d1 = d;
    outputsum = 0;
    control(RPMi, now);
    input2 = input;
    output2 = output;
    printoutput2 = printoutput;
    d2 = d;
  }
  
  if(d1 == 1)
  {
    digitalWrite (IN1, HIGH);
    digitalWrite (IN2, LOW);
    analogWrite (ENA, output1*1.2);
  }
  else if (d1 == 0)
  {
    digitalWrite (IN1, LOW);
    digitalWrite (IN2, HIGH);
    analogWrite (ENA, output1);
  }

  if(d2 == 1)
  {
    digitalWrite (IN3, HIGH);
    digitalWrite (IN4, LOW);
    analogWrite (ENB, output2);
  }
  else if (d2 == 0)
  {
    digitalWrite (IN3, LOW);
    digitalWrite (IN4, HIGH);
    analogWrite (ENB, output2);
  }
  
  Serial3.print(timechange); //tiempo desde la anterior medida
  Serial3.print(" ");
  Serial3.print(RPMi);
  Serial3.print(" ");
  Serial3.print(RPMd);
  Serial3.print(" ");
  Serial3.print(setpoint);
  Serial3.print(" ");
  Serial3.print(modo); //modo de funcionamiento
  Serial3.print(" ");
  //Serial3.print(error1);
  //Serial3.print(" ");
  //Serial3.print(error2);
  //Serial3.print(" ");
  Serial3.print(output2);
  Serial3.print(" ");
  Serial3.println(output1);

}
