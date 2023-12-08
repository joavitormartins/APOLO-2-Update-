#include <ESP32Servo.h>
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
Servo myservo;

char valorRecebido = '\0';

#define trigPin 26
#define echoPin 27

#define trigPinD 2 
#define echoPinD 15     

#define enA 21
#define in1 25
#define in2 33

#define enB 19
#define in3 23
#define in4 22

int velocidadeMotorUm = 250;
int velocidadeMotorDois = 250;

int DistanciaDireita = 0;
int DistanciaEsquerda = 0;
float Distancia = 0.00;
float DistanciaD = 0.00;

void setup() {
  SerialBT.begin("APOLO-2");
  Serial.begin(9600);
  
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  myservo.setPeriodHertz(50);
  myservo.attach(5, 500, 2400);
  
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(trigPinD, OUTPUT);
  pinMode(echoPinD, INPUT);
}

void loop() {
  if (SerialBT.available()) {
    valorRecebido = (char)SerialBT.read();
    Serial.write(valorRecebido);
  }

  if (valorRecebido == '0') {
    Parar();
  } else if (valorRecebido == '1') {
    autonomo();
  } else {
    controleTotal();
  }
}

void controleTotal() {
  if (valorRecebido == 'f') {
    Frente();
  } else if (valorRecebido == 't') {
    ParaTras();
  } else if (valorRecebido == 'p') {
    Parar();
  } else if (valorRecebido == 'd') {
digitalWrite(in1, LOW);                            //MOTOR 1 
digitalWrite(in2, HIGH);
digitalWrite(in3, HIGH);                           //MOTOR 2
digitalWrite(in4, LOW);
  } else if (valorRecebido == 'e') {
digitalWrite(in1, HIGH);                           //MOTOR 1 
digitalWrite(in2, LOW);
digitalWrite(in3, LOW);                            //MOTOR 2
digitalWrite(in4, HIGH);
  }
  DistanciaD = ProcurarD (); 
  if (DistanciaD < 30){pulo();}else{Parar();}
}

void autonomo() {
  // Seu código para o modo autônomo aqui
myservo.write(90);                                // Gira o Servo com o sensor a 90 graus
delay (100);                                      // Aguarda 100 milesugodos
Distancia = Procurar ();                          // Medindo a Distancia em CM.
if (Distancia < 50) {                             // Se há obstáculo encontrado a menos de 50cm.
direcao ();                                       // Se Frente estiver bloqueado, mude de direção
}
else if (Distancia >= 50)  {                      // Se o obstáculo for encontrado entre a mais de 50cm 
Frente ();    
}
  
}

int Procurar (void) {
float duracao = 0.0;              
float CM = 0.0;                   

digitalWrite(trigPin, LOW);       
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);      
delayMicroseconds(10);
digitalWrite(trigPin, LOW);       
duracao = pulseIn(echoPin, HIGH); 

CM = (duracao / 58.8);           

Serial.print("Distancia em CM: "); 
Serial.print(CM);              

return CM;                        
}

int ProcurarD (void) {
float duracaoD = 0.0;              
float CMD = 0.0;                   

digitalWrite(trigPinD, LOW);       
delayMicroseconds(2);
digitalWrite(trigPinD, HIGH);      
delayMicroseconds(10);
digitalWrite(trigPinD, LOW);       
duracaoD = pulseIn(echoPinD, HIGH); 

CMD = (duracaoD / 58.8);           

Serial.print("DistanciaD em CM: "); 
Serial.print(CMD);              

return CMD;                        
}

void direcao () {        
Parar ();                                         // O robô Para
ParaTras();
Parar ();                                         // O robô Para
myservo.write (180);                          // Gira o Servo com o sensor a 180 graus
delay (1000);              
DistanciaEsquerda = Procurar ();                  // Defina a Distancia da Esquerda 
delay (500);               
myservo.write (0);                            // Gira o Servo com o sensor a 0 graus
delay (500);               
DistanciaDireita = Procurar ();                   // Defina a Distancia da Direita
delay (500);               
myservo.write (90);                           // Gira o Servo com o sensor a 90 graus
delay (500);              
CompareDistance ();                               // Encontre a distância mais longa.
}

/***************************************************************************************************/

/***************************************************************************************************/
void CompareDistance () {                   
if (DistanciaDireita > DistanciaEsquerda) {       // Se a direita está menos obstruída.
Vireadireita ();                                // O robô vai virar a direita 
}
else if (DistanciaEsquerda > DistanciaDireita) {  // Se Esquerda estiver menos obstruída.
VireaEsquerda ();                               // Robô Vire na direção esquerda.
}
else {                                            // Se ambos estiverem igualmente obstruídos. 
obstacolo ();                        
}
}

void Parar() {
  // Código para parar os motores
digitalWrite(in1, LOW);                           
digitalWrite(in2, LOW);
digitalWrite(in3, LOW);
digitalWrite(in4, LOW);
}

void Frente() {
  // Código para mover para frente
digitalWrite(in1, HIGH);                          
digitalWrite(in2, LOW);

digitalWrite(in3, HIGH); 
digitalWrite(in4, LOW);
}

void pulo() {
  // Código para mover para frente
digitalWrite(in1, HIGH);                          
digitalWrite(in2, LOW);

digitalWrite(in3, HIGH); 
digitalWrite(in4, LOW);
delay(700);
}

void ParaTras() {
  // Código para mover para trás
digitalWrite(in1, LOW);                       
digitalWrite(in2, HIGH);

digitalWrite(in3, LOW); 
digitalWrite(in4, HIGH);
}

void Vireadireita() {
  // Código para virar para direita
digitalWrite(in1, LOW);                           //MOTOR 1 
digitalWrite(in2, HIGH);

digitalWrite(in3, HIGH);                          //MOTOR 2
digitalWrite(in4, LOW);
delay(900);
}

void VireaEsquerda() {
  // Código para virar para esquerda
digitalWrite(in1, HIGH);                           //MOTOR 1 
digitalWrite(in2, LOW);

digitalWrite(in3, LOW);                          //MOTOR 2
digitalWrite(in4, HIGH);
delay(900);
}

// Outras funções e lógica conforme necessário
void obstacolo () {    
Serial.println("Robô: Parar ");
digitalWrite(in1, LOW);                           //Configurar a ponte h 
digitalWrite(in2, LOW);
digitalWrite(in3, LOW);
digitalWrite(in4, LOW);
delay(100);                                       //aguarda um tempo

Serial.println("Robô: Ré ");
digitalWrite(in1, LOW);                           //Configurar a ponte h 
digitalWrite(in2, HIGH);
digitalWrite(in3, LOW);
digitalWrite(in4, HIGH);
delay(300);                                       //aguarda um tempo

Serial.println("Robô: Direita ");
digitalWrite(in1, LOW);                           //Configurar a ponte h 
digitalWrite(in2, HIGH);
digitalWrite(in3, HIGH);
digitalWrite(in4, LOW);
delay(300);                                       //aguarda um tempo

Serial.println("Robô: Ré ");
digitalWrite(in1, LOW);                           //Configurar a ponte h 
digitalWrite(in2, HIGH);
digitalWrite(in3, LOW);
digitalWrite(in4, HIGH);
delay(300);                                       //aguarda um tempo

Serial.println("Robô: Direita ");
digitalWrite(in1, LOW);                           //Configurar a ponte h 
digitalWrite(in2, HIGH);
digitalWrite(in3, HIGH);
digitalWrite(in4, LOW);
delay(300);                                       //aguarda um tempo
} 
