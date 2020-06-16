// Proyecto Ascensor TIC // 

#include <LiquidCrystal.h> // se incluye la libreria de la LCD
LiquidCrystal lcd(23,22,24,25,26,27); // (RS, E, D4, D5, D6, D7)

#include <Servo.h> // Se incluye la libreria del servo motor

#define led1 28                         // pin del primer led
#define led2 29                       // pin del segundo led
#define pulsador 30                     // pin del pulsador 1
#define pulsador2 31                     // pin del pulsador 2

Servo puerta;  // Declaramos el servo para controlarlo
int pos = 0; // La variable para store the servo position

int led_estado = LOW;                  // el estado del led será igual a LOW
int motor_estado = LOW;                // el estado del motor será igual a LOW
int motorpin1 = 32;                     // un pin del motor al pin4 del arduino                         
int motorpin2 = 33;                     // el otro pin del motor al pin 5 del arduino

long distancia;                        // Sensor ultrasonido 
long tiempo;

int datapin= 37; // DS
int latchpin= 36; // STCP 
int clockpin= 38; // SHCP

int datapin2= 40; // DS
int latchpin2= 39; // STCP
int clockpin2= 41; //SHCP

int input;   // Entrada para monitorear los pisos desde el serial.
//--------------------------------------------------------------------------------------


byte full[8] = {                           // Esquemas de las flechas 
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110
};

byte left[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00001,
  0b00111,
  0b01111
};



byte middle1[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b01110,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};



byte right[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10000,
  0b11100,
  0b11110
};

byte left2[8] = {
  0b01111,
  0b00111,
  0b00001,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};



byte middle2[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b01110,
  0b00100,
  0b00000
};



byte right2[8] = {
  0b11110,
  0b11100,
  0b10000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};



//--------------------------------------------------------------------------------------

void setup() {

Serial.begin(9600);
  
  pinMode(led1, OUTPUT);             // declaramos los leds como salidas    
  pinMode(led2, OUTPUT);
  pinMode(pulsador, INPUT);          // declaramos los pulsadores como entradas   
  pinMode(pulsador2, INPUT);
  pinMode(motorpin1, OUTPUT);       // los pines del motor como salidas
  pinMode(motorpin2, OUTPUT);

  pinMode(datapin, OUTPUT);         // Displays 7 Segmentos
  pinMode(latchpin, OUTPUT);
  pinMode(clockpin, OUTPUT);
  pinMode(datapin2, OUTPUT);
  pinMode(latchpin2, OUTPUT);
  pinMode(clockpin2, OUTPUT);

  puerta.attach(42);    // decimos que la puerta, es decir el servo se encuentra en el pin 42
  puerta.write(0);

lcd.begin(16, 2);
lcd.setCursor(0, 0);    // Pantalla Inicial
lcd.clear();
  
  
  lcd.createChar(0, full);                     // Creando flechas 
  lcd.createChar(1, left);
  lcd.createChar(2, middle1);
  lcd.createChar(3, right);
  lcd.createChar(4, left2);
  lcd.createChar(5, middle2);
  lcd.createChar(6, right2);


  

pinMode(35, OUTPUT); /*activación del pin 35 como salida: para el pulso ultrasónico*/
pinMode(34, INPUT); /*activación del pin 34 como entrada: tiempo del rebote del ultrasonido*/
  
  digitalWrite(led1, led_estado);             // Que se lea el estado de cada led
  digitalWrite(led2, led_estado);
  digitalWrite(motorpin1, motor_estado);     // que se lea el estado de cada pin del motor
  digitalWrite(motorpin2, motor_estado);
  /*inicio();*/                              // Se presenta el proyecto
}

void loop() {
  
digitalWrite(35,LOW); /* Por cuestión de estabilización del sensor*/
delayMicroseconds(5);
digitalWrite(35, HIGH); /* envío del pulso ultrasónico*/
delayMicroseconds(10);


tiempo=pulseIn(34, HIGH); /* Función para medir la longitud del pulso entrante. Mide el tiempo que transcurrido entre el envío
  del pulso ultrasónico y cuando el sensor recibe el rebote, es decir: desde que el pin 12 empieza a recibir el rebote, HIGH, hasta que
  deja de hacerlo, LOW, la longitud del pulso entrante*/


distancia=int(0.017*tiempo); /*fórmula para calcular la distancia obteniendo un valor entero*/
ultrasonido1();

  

    if(digitalRead(pulsador)==1 && distancia <= 4) {                               // En este if, si se presiona el primer pulsador, girara hacia la derecha...  (Boton primer piso para que baje del segundo)
      delay(100);
     led_estado = !led_estado;
      motor_estado =!motor_estado;
      digitalWrite(led1, led_estado);
      digitalWrite(motorpin1, motor_estado);
      digitalWrite(motorpin2, LOW);
      flechadown();
       
        } 

                                                               // Ubicacion de pisos para parar motor en la longitud exacta.

       if(distancia >= 21){                                     // Si la distancia es mayor o igual a 21 el motor se detendrá (primer piso)
     digitalWrite(motorpin1, LOW);  
     digitalWrite(led1, LOW);   
     display1();
  }

      if(distancia <= 4) {                                    // Si la distancia es menor o igual a 3 el motor se dentendrá (segundo piso)
     digitalWrite(motorpin2, LOW);  
     digitalWrite(led2, LOW);
     display2();      
}                   
          
      if(digitalRead(pulsador) && distancia >= 21) {                               // En este if, si se presiona el primer pulsador, girara hacia la derecha... Aqui solo se abrira y cerrara la puerta cuando se oprima el boton y el ascensor se encuentre en ese mismo piso. (primer piso)
      delay(100);
      openpuerta();
      delay(3000);
      closepuerta();
   }  

if(Serial.available()>0){                                                         // Si el ascensor se encuentra en el mismo piso del boton, entonces podra subir desde el monitor serie. (piso 1)
  input=Serial.read();

  if(input=='2'){
      digitalWrite(led1, HIGH);
      digitalWrite(motorpin2, HIGH);
      flechaup();         
  }

  if(input=='1'){                                                                 //  Lo mismo que lo anterior pero en el piso 2
    digitalWrite(led2, HIGH);
    digitalWrite(motorpin1, HIGH);
    flechadown();
  }
}
          
      if(digitalRead(pulsador2)==1 && distancia >=21){              // Girará en sentido contrario
        led_estado = !led_estado;
        motor_estado =!motor_estado;
        digitalWrite(led2, led_estado);
        digitalWrite(motorpin1, LOW);
        digitalWrite(motorpin2, motor_estado);
        flechaup();       
  }
      

  if(digitalRead(pulsador2) && distancia <= 4) {                               // En este if, si se presiona el primer pulsador, girara hacia la derecha... Aqui solo se abrira y cerrara la puerta cuando se oprima el boton y el ascensor se encuentre en ese mismo piso. (segundo piso)
      delay(100);
      openpuerta();
      delay(3000);
      closepuerta();
   }     
}

//-------------------------------------------------------------------------- // voids 
void inicio(){                                // Se presenta el proyecto
lcd.print("    PROYECTO    ");
lcd.setCursor(0, 1);
lcd.print("      TIC      ");
delay(5000);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("    ASCENSOR    ");
lcd.setCursor(0, 1);
lcd.print("     2 PISOS     ");
delay(3000);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print(" INTEGRANTES: ");
lcd.setCursor(0, 1);
lcd.print("   CAMILO VELEZ  ");
delay(2000);
lcd.clear();
lcd.print(" LICETH GARCES ");
delay(2000);
lcd.clear();
lcd.print(" CAMILO SALINAS ");
delay(2000);
lcd.clear();
lcd.print(" KEVIN GOMEZ ");
delay(2000);
lcd.clear();
}


void ultrasonido() {                              // Printeo del ultrasonido en LCD
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Distancia: ");
lcd.setCursor(0, 1);
lcd.print(distancia);
lcd.print(" cm");
delay(500);
}

void ultrasonido1() {                              // Printeo del ultrasonido en el monitor serie
Serial.println(distancia);
delay(500);
}

void flechaup(){                           // Dibujo flecha arriba
 lcd.clear();
 lcd.setCursor(6,0);
 lcd.write(1);
 lcd.setCursor(7,1);
 lcd.write(byte(0));
 lcd.setCursor(7,0);
 lcd.write(2);
 lcd.setCursor(8,0);
 lcd.write(3);
}

void flechadown(){                       // Dibujo flecha abajo
 lcd.clear();
 lcd.setCursor(7,1);
 lcd.write(5);
 lcd.setCursor(7,0);
 lcd.write(byte(0));
 lcd.setCursor(6,1);
 lcd.write(4);
 lcd.setCursor(8,1);
 lcd.write(6);

}

void display1(){                                         // Displays indican primer piso
  digitalWrite(latchpin, LOW);
 shiftOut(datapin, clockpin, MSBFIRST, 121);
 digitalWrite(latchpin, HIGH);
  digitalWrite(latchpin2, LOW);
 shiftOut(datapin2, clockpin2, MSBFIRST, 121);
 digitalWrite(latchpin2, HIGH);
}

void display2(){                                       // Displays indican el segundo piso
  digitalWrite(latchpin, LOW);
 shiftOut(datapin, clockpin, MSBFIRST, 36);
 digitalWrite(latchpin, HIGH);
  digitalWrite(latchpin2, LOW);
 shiftOut(datapin2, clockpin2, MSBFIRST, 36);
 digitalWrite(latchpin2, HIGH);
}

void openpuerta(){
  for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    puerta.write(pos);              // tell servo to go to position in variable 'pos'
   delay(15);
   
 
  }
}

void closepuerta(){
   for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    puerta.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);
 
  }
}

void stayopened(){
   for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    puerta.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);  
    break;
  }
}

void stayclosed(){
    for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    puerta.write(pos);              // tell servo to go to position in variable 'pos'
   delay(15);
   break;  
}
}




