
int modo = 3;

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
  double output1, input1, error1, printoutput1;
  double output2, input2, error2, printoutput2;
  double lastInput;
  double dInput;
  double senal;
  double outmax = 255;
  double outmin = -255; //just for representation. Actual output minimum is 0 (PWM), but
                        //we'd like to represent backwards spinning as negative
  unsigned long now, lastime, sampletime, timechange;
  double kp, ki, kd;
  int d, d1, d2; //direccion de giro; d = 1, palante.
  String cad;     //para guardar la referencia
  double distanciad, distanciai;
  double phi; //variable para control de la orientacion respecto de la pared
  float distanciasensores = 9.2; //distancia entre los sensores delantero y trasero en cm

//Functions
/*void setpointBT()
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
}*/
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

void setup() {

  Serial3.begin(38400);
  
  // Parametros del control
  setpoint = 0; //en este modo 3 usamos el setpoint para indicar el angulo de orientacion
                //del robot con la pared. Queremos que sea 0 para que vaya paralelo
  input = 1;
  
  sampletime = 10;//Trying 10ms instead of 100ms. Might change it again
  
  lastime = millis() - sampletime;

  kp = 0.8; ki = 3; kd = 1; //el error en el ángulo es muy sutil. Reducimos constantes

  //definicion de los pines
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); //un valor HIGH en IN1 provoca que la rueda derecha gire hacia delante
  pinMode(IN2, OUTPUT); //un valor HIGH en IN2 provoca que la rueda derecha gire hacia atrás
  pinMode(IN3, OUTPUT); //un valor HIGH en IN3 provoca que la rueda izquierda gire hacia delante
  pinMode(IN4, OUTPUT); //un valor HIGH en IN4 provoca que la rueda izquierda gire hacia atrás

  pinMode(ecoi, INPUT);
  pinMode(ecod, INPUT);
  pinMode(trigi, OUTPUT);
  pinMode(trigd, OUTPUT);
}

void loop() {
  outputsum = 0;
  //setpointBT();
  ultrasonidos();
  
  //Cada tiempo de muestreo, calculo la senal de control
  now = millis();
  timechange = now - lastime;
  if(timechange >= sampletime)
  {
    //control(phi, now);
    input = phi;
    error = input - setpoint;
    dInput = input - lastInput;
  
    outputsum += ki * error;
    senal = kp * error;
    senal += outputsum - kd * dInput;
    if(abs(error) < 10) senal = senal*0.5;

    printoutput = senal;
    if(senal < 0)
    {
      if(senal < outmin) printoutput = outmin;
      senal = -senal;      
    }
    if(senal > outmax)
    {
      senal = outmax;
      printoutput = senal;
    }

    output = senal;
  
    lastInput = input;
    lastime = now;

    
    if(phi>0) //si el angulo es positivo, muevo la rueda derecha (la mas cercana a la pared)
    {
      output1 = 70 + output;
      if(output1 > outmax) output1 = outmax;
      output2 = 65;
    }
    else if(phi<0)
    {
      output1 = 65;
      output2 = 70 + output;
      if(output2 > outmax) output2 = outmax;
    }
    else 
    {
      output1 = 80; output2 = 80; //compensar? ahora veremos si funciona
    }

    //error1 = error;
    //error2 = error;
    input1 = distanciad;
    input2 = distanciai;
    //if(abs(error1) > abs(error2) + 5) control(distanciad, now);
    //if(abs(error2) > abs(error1) + 5) control(distanciai, now);
  
      digitalWrite (IN1, HIGH);
      digitalWrite (IN2, LOW);
      analogWrite (ENA, output1);
      
      digitalWrite (IN3, HIGH);
      digitalWrite (IN4, LOW);
      analogWrite (ENB, output2);
  }
  
    Serial3.print(timechange); //tiempo desde la anterior medida
    Serial3.print(" ");
    Serial3.print(input2);//distancia delantera
    Serial3.print(" ");
    Serial3.print(input1);//distancia trasera
    Serial3.print(" ");
    Serial3.print(50);
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
