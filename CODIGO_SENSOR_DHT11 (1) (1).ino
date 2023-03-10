/*
	Capitulo 7 de Arduino desde cero en Español
	Programa que utiliza el sensor DHT22 o DHT11 para obtener datos de temperatura
	y humedad. Deben instalarse la Librerias DHT Sensor Library y Adafruit Unified Sensor.

	Autor: bitwiseAr  

*/

#include <DHT.h>		
#include <DHT_U.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 0, A1, A2, 10, 9);
#define pir A5

//Defino las variables del keypad******************************************************************************************************
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
 {'1','2','3'},
 {'4','5','6'},
 {'7','8','9'},
 {'*','0','#'}
};
byte rowPins[ROWS] = {12,11,8,7}; //Filas(pines del 7,8 y 12, 13)
byte colPins[COLS] = {6,5,4}; //Columnas (pines del 4 al 6)
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//**************************************************************************************************************************************

//Defino las variables para la clave de la alarma***************************************************************************************
char claveMaestra[5]= "3456";  //Clave definida 
char clave[5];                 //Arreglo que almacenara la clave ingresada por el usuario
byte indice = 0;            
char key;                     
bool seguirLeyendo = true;    //Booleano que indica si el usuario tiene que seguir presionando teclas
bool alarmaActivada = false;  
//**************************************************************************************************************************************


int SENSOR = A4;	// pin DATA de DHT22 a pin digital 2
int TEMPERATURA;
int HUMEDAD;
DHT dht(SENSOR, DHT11);  

//Defino las variables para el sonido por el parlante***********************************************************************************
int spk=3;                              // altavoz a GND y pin 13
int c[5]={131,262,523,1046,2093};       // frecuencias 4 octavas de Do
int cs[5]={139,277,554,1108,2217};      // Do#
int d[5]={147,294,587,1175,2349};       // Re
int ds[5]={156,311,622,1244,2489};      // Re#
int e[5]={165,330,659,1319,2637};       // Mi
int f[5]={175,349,698,1397,2794};       // Fa
int fs[5]={185,370,740,1480,2960};      // Fa#
int g[5]={196,392,784,1568,3136};       // Sol
int gs[5]={208,415,831,1661,3322};      // Sol#
int a[5]={220,440,880,1760,3520};       // La
int as[5]={233,466,932,1866,3729};      // La#
int b[5]={247,494,988,1976,3951};       // Si
//**************************************************************************************************************************************

//Declaracion variables de iluminacion**************************************************************************************************
const long A = 1000;     //Resistencia en oscuridad en KΩ
const int B = 15;        //Resistencia a la luz (10 Lux) en KΩ
const int Rc = 10;       //Resistencia calibracion en KΩ
const int LDRPin = A3;   //Pin del LDR
int V;
int ilum;
int pinLedR = 13;        // pin Rojo del LED RGB
//**************************************************************************************************************************************
	

				
void setup(){
  Serial.begin(115200);
  Serial.begin(9600);		
  dht.begin();
  lcd.begin(16, 2); 
  pinMode(pinLedR, OUTPUT); // pone el pinLedR como output
  pinMode(pir, INPUT);      // pone el PIN DEL SENSOR COMO ENTRADA
  Serial.begin(9600); 
}
void nota(int frec, int t){
    tone(spk,frec);      // suena la nota frec recibida
    delay(t);            // para despues de un tiempo t
}
void teclado(){
  alarmaActivada = false;
  key = keypad.getKey();
    if(key){
      clave[indice] = key;
      lcd.setCursor(0,1);
      lcd.print(clave);
      indice++;
    }
    delay(500);
    if(indice==4){ //Una vez que el indice es igual a 4, significa que el usuario ya ingreso una clave 
        for(int i=0; i<4;i++){
          clave[i]= funcionHash(clave[i]); //Cada dígito ingresado en la clave es mapeado con la funcionHash()
        }
        if(!strcmp(clave,claveMaestra)){  //strcmp(clave,claveMaestra)--> funcion que compara dos arreglos
          alarmaActivada = true;
          lcd.clear();
          lcd.print("Clave Correcta");
        }else{
          lcd.clear();
          lcd.print("Clave Incorrecta");
        }
        indice=0; //Se resetea el indice a 0. Listo para que se ingrese una nueva clave
        for(int i=0; i<4;i++){
          clave[i]=' '; //La clave se resetea
        }
        seguirLeyendo = false; //Indica que se tiene que salir del while(seguirLeyendo)
        delay(500); lcd.clear();
   }
}

//f(x)= (ax+b) mod n , n=10      ,a=3 
//   biyectiva    ->> mcd(a,n)=1
//sin puntos fijos->> mcd(a-1,n)=2  , b=7
// f(x) = (3x+7) mod 10
//Mapeo de numeros
// f(0) =  7 mod 10 = 7
// f(1) =  10 mod 10 = 0
// f(2) =  13 mod 10 = 3
// f(3) =  16 mod 10 = 6
// f(4) =  19 mod 10 = 9
// f(5) =  22 mod 10 = 2
// f(6) =  25 mod 10 = 5
// f(7) =  28 mod 10 = 8
// f(8) =  31 mod 10 = 1
// f(9) =  34 mod 10 = 4

char funcionHash(char tecla){
  int valor = 0;
  valor = String(tecla).toInt();
  valor =  (3*valor+7)%10;
  return  valor + 48 ; 
}

void musicaHarryPotter(){
  nota(b[2], 500);nota(e[3], 1000);nota(g[3], 250);
  nota(fs[3],250);nota(e[3], 1000);nota(b[3], 500);
  nota(a[3],1250);nota(fs[3],1000);nota(b[2], 500);
  nota(e[3],1000);nota(g[3],  250);nota(fs[3],250); 
  nota(d[3],1000);nota(e[3],  500);nota(b[2],1000);
  noTone(spk);delay(2000);   
}

void loop(){
    
    key = keypad.getKey();
    int pir_sensor=digitalRead(pir);
    TEMPERATURA = dht.readTemperature();  
    HUMEDAD = dht.readHumidity();   
    V = analogRead(LDRPin);
    ilum = ((long)V*A*10)/((long)B*Rc*(1024-V)) - 2.1;  
    
    if(key=='*' || key=='#'){
      lcd.print("Password");
      while(seguirLeyendo){ //Se ingresa en un loop hasta que se termina de ingresar una clave
        teclado();
      } 
      seguirLeyendo = true;
      lcd.clear();
     }
    if(TEMPERATURA < 18){
      lcd.print("Calefaccion");
      lcd.setCursor(0,1);
      lcd.print("Activada");
      delay(1000);lcd.clear();
    }
    if(TEMPERATURA > 25 && TEMPERATURA <50 ){
      lcd.print("Ventilacion");
      lcd.setCursor(0,1);
      lcd.print("Activada");
      delay(1000);lcd.clear();
    }
    if(TEMPERATURA >= 50){
      lcd.print("Alarma");
      musicaHarryPotter();
      delay(100);lcd.clear();
    }
    if(pir_sensor == HIGH && alarmaActivada ){
      lcd.print("Alarma");
      musicaHarryPotter();
      delay(100);lcd.clear();
    }
    if (ilum < 102.1){ //Se ingresa 102.1 ya que en el ldr seria igual a 100.
      analogWrite(pinLedR, 255); //encendido
    }else{
      analogWrite(pinLedR, 0); //apagado
    }
}
