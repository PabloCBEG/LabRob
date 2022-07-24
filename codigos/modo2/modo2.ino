
int modo = 2;

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
double duraciond, duracioni;

double output, input, setpoint, error, errorI, outputsum, printoutput;
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
double distanciad, distanciai, phi;
float distanciasensores = 9.2;

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

  phi = atan((distanciad-distanciai)/distanciasensores);
  phi = phi*180/3.141592;
}
void control(double distancia)
{
  input = distancia;
  
  /*error = input - setpoint;
  errorI = error * sampletime + errorI;
  dInput = (input - lastInput) / sampletime;

  outputsum += ki * errorI;
  senal = kp * error;
  senal += outputsum - kd * dInput;*/
  //if(abs(error) < 10) senal = senal*0.5;

  error = input - setpoint;
  dInput = input - lastInput;

  outputsum += ki * error;
  senal = kp * error;
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

void setup() {

  Serial3.begin(38400);
  
  // Parametros del control
  setpoint = 100;
  input = 101;
  
  sampletime = 10; //Hemos puesto sample time de 10 por ver si gana en precision. Si no mejora,
                    //volvemos a los 100ms que tenia
  
  lastime = millis() - sampletime;

  kp = 6; ki = 0.005; kd = 1;

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
  setpointBT();
  ultrasonidos();
  
  //Cada tiempo de muestreo, calculo la senal de control
  now = millis();
  timechange = now - lastime;
  if(timechange >= sampletime)
  {
    if(distanciad > setpoint - 3 && distanciad < setpoint + 3)
    {
      control(phi);
      if(phi > 0)
      {
        output1 = output;
        output2 = 0;
        printoutput1 = printoutput;
        printoutput2 = output2;
      }
      else if(phi == 0)
      {
        output1 = 0;
        output2 = 0;
        printoutput1 = output1;
        printoutput2 = output2;
      }
      else
      {
        output1 = 0;
        output2 = output;
        printoutput1 = output1;
        printoutput2 = printoutput;
      }
    }
    else
    {
      control(distanciad);
      output1 = output;
      output2 = output;
      printoutput1 = printoutput;
      printoutput2 = printoutput;
    }
    d1 = d; d2 = d;

      input1 = distanciad;
      input2 = distanciai;


    if(d2 == 1)
    {
      digitalWrite (IN3, HIGH);
      digitalWrite (IN4, LOW);
      analogWrite (ENB, output2);
    }
    else
    {
      digitalWrite (IN3, LOW);
      digitalWrite (IN4, HIGH);
      analogWrite (ENB, output2);
    }
    if(d1 == 1)
    {
      digitalWrite (IN1, HIGH);
      digitalWrite (IN2, LOW);
      analogWrite (ENA, output1);
    }
    else
    {
      digitalWrite (IN1, LOW);
      digitalWrite (IN2, HIGH);
      analogWrite (ENA, output1);
    }
  }
  
  Serial3.print(timechange); //tiempo desde la anterior medida
  Serial3.print(" ");
  Serial3.print(input2); //distancia medida por sensor izquierdo
  Serial3.print(" ");
  Serial3.print(input1);
  Serial3.print(" ");
  Serial3.print(setpoint);
  Serial3.print(" ");
  Serial3.print(modo); //modo de funcionamiento
  Serial3.print(" ");
  /*Serial3.print(d);
  Serial3.print(" ");
  Serial3.print(d1);
  Serial3.print(" ");
  Serial3.print(d2);
  Serial3.print(" ");*/
  Serial3.print(printoutput2); //senal de control para el motor izquierdo
  Serial3.print(" ");
  Serial3.println(printoutput1);
}
