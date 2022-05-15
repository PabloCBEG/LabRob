int ENA = 10;
int IN1 = 9;
int IN2 = 8;
int ENB = 5;
int IN3 = 7;
int IN4 = 6;

void setup() {

pinMode(ENA, OUTPUT);
pinMode(ENB, OUTPUT);
pinMode(IN1, OUTPUT); //un valor HIGH en IN1 provoca que la rueda izda gire hacia delante
pinMode(IN2, OUTPUT); //un valor HIGH en IN2 provoca que la rueda izda gire hacia atrás
pinMode(IN3, OUTPUT); //un valor HIGH en IN3 provoca que la rueda dcha gire hacia delante
pinMode(IN4, OUTPUT); //un valor HIGH en IN1 provoca que la rueda dcha gire hacia atrás

//Serial.begin(9600);
}

void palante() {

  digitalWrite(IN1,HIGH);	//giran ambas ruedas hacia delante a "la misma" velocidad: el robot avanza en linea recta
  digitalWrite(IN2,LOW);
  analogWrite(ENA,105);		//valor 105 en ENA, porque la rueda está un poco descompensada
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  analogWrite(ENB,100);
}

void patras(){
 digitalWrite (IN1, LOW);
 digitalWrite (IN2, HIGH);
 analogWrite (ENA, 100);	//aquí parece que no hace falta el valor 105, cuando el robot va hacia atrás no es tan pronunciada la descompensación
 digitalWrite (IN3, LOW);
 digitalWrite (IN4, HIGH);
 analogWrite (ENB, 100);
  }

void pivote_dcha ()	//gira en el sitio hacia la dcha
{  
 digitalWrite (IN1, HIGH);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 200);
  digitalWrite (IN3, LOW);
 digitalWrite (IN4, HIGH);
 analogWrite (ENB, 200);  
}


void pivote_izda ()	//gira en el sitio hacia la izda
{  
 digitalWrite (IN1, LOW);
 digitalWrite (IN2, HIGH);
 analogWrite (ENA, 200);
digitalWrite (IN3, HIGH);
digitalWrite (IN4, LOW);
analogWrite (ENB, 200);  
}

void parar ()
{  
 digitalWrite (IN1, LOW);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 0);
 digitalWrite (IN3, LOW);
 digitalWrite (IN4, LOW);
 analogWrite (ENB, 0);   
}

void giro_dcha ()	//gira hacia la izquierda
{  
 digitalWrite (IN1, HIGH);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 200);
  digitalWrite (IN3, HIGH);
 digitalWrite (IN4, LOW);
 analogWrite (ENB, 100);  
}

void giro_izda ()	//gira hacia la derecha
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
palante ();
 delay (5000);
 patras ();
 delay (5000);
 pivote_dcha ();
 delay (3000);
 pivote_izda ();
 delay (3000);
 giro_dcha ();
 delay(5000);
 giro_izda ();
 delay(5000);
 parar ();
 delay (4000);   
 }
