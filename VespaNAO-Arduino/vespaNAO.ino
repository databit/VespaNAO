#include <SoftwareSerial.h>
#include <SD.h>             
#include <TMRpcm.h>         
#include <SPI.h>

// Pin Bluetooth
#define BT_RX 2
#define BT_TX 3

// Pin motor
#define FS1 4
#define IN1 5
#define IN2 6
#define DS1 13
#define DS2 12

// Pin rele'
#define RELE1 9
#define RELE2 8
#define RELE3 10
#define RELE4 11

// Pin SD
#define SD_CS A0

// Pin speaker
#define SPEAKER A4

// Global variables
int releStatus[6] = {0, 0, 0, 0, 0, 0};
unsigned long time = 0;
char btCmd = '\0';
TMRpcm player;
SoftwareSerial bluetooth(BT_RX,BT_TX);

void setup (){
  // Setup serial monitor
  Serial.begin(9600); 

  // Setup bluetooth communication
  bluetooth.begin(9600);
  
  Serial.println("******* NAO Vespa log *******");
  Serial.println("** Progettato e realizzato **");
  Serial.println("** dall'IISS \"M. Bartolo\"  **");
  Serial.println("**       Pachino (SR)      **");
  Serial.println("********* INIZIO LOG ********");

  // Setup motor' pins
  pinMode(FS1, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(DS1, OUTPUT);
  pinMode(DS2, OUTPUT);

  // Setup music players and SD
  player.speakerPin = SPEAKER; // pin utilizzabili 5,6,11 o 46 sulla Mega, 9 sulla Uno, Nano, etc

  // per abilitare un'uscita complementare togliere il commento alla riga sotto:
  // pinMode(10,OUTPUT); pin utilizzabili 9,10; Sulla Mega: 5-2,6-7,11-12,46-45

  if(!SD.begin(SD_CS)) {
    Serial.println("SD FAIL");
  
  } else {
    Serial.println("SD OK");
  }
}

void loop(){
  while(bluetooth.available())
    btCmd = char(bluetooth.read());  
  
  if(!bluetooth.available()) {
    if(btCmd != '\0'){
      Serial.println("BT="+ btCmd);
  
      switch(btCmd){
        // Move forward
        case 'F':
          motorForward(128, 1000);
          break;

        // Move back
        case 'B':
          motorReverse(128, 1000);
          break;

        // Indicator left
        case 'L':
          setIndicator('L');
          break;

        // Indicator right
        case 'R':
          setIndicator('R');
          break;

        // Indicator all
        case 'Q':
          setIndicator('Q');
          break;

        // Indicator Stop
        case 'S':
          setIndicator('S');
          break;

        // Indicator  headlight
        case 'H':
          setIndicator('H');
          break;

        case 'M':
          if(player.isPlaying()){
            player.pause();
          } else {
            player.play("music");
          }

        /*
          case 'S': tmrpcm.stopPlayback(); break;
          case '=': tmrpcm.volume(1); break;
          case '-': tmrpcm.volume(0); break;
          case '0': tmrpcm.quality(0); break;
          case '1': tmrpcm.quality(1); break;
          default: break;
        */
          break;

      }
      btCmd = '\0';
    }

    clearActions();
    blinkIndicator();
  }
  delay(100);
}

void clearActions(){
  if(releStatus[0] == 0){
    digitalWrite(RELE1, LOW);
    digitalWrite(RELE2, LOW);

  } else { 
    if(releStatus[1] == 0)
      digitalWrite(RELE1, LOW);

    if(releStatus[2] == 0)
      digitalWrite(RELE2, LOW);

    if(releStatus[3] == 0)
      digitalWrite(RELE3, LOW);

    if(releStatus[4] == 0)
      digitalWrite(RELE4, LOW);
  }
}

void blinkIndicator(){
  if(releStatus[0] > 0){
    digitalWrite(RELE1, (int(millis()/500) % 2 == 0) ? LOW : HIGH);
    digitalWrite(RELE2, (int(millis()/500) % 2 == 0) ? LOW : HIGH);

  } else { 
    if(releStatus[1] > 0)
      digitalWrite(RELE1, (int(millis()/500) % 2 == 0) ? LOW : HIGH);

    if(releStatus[2] > 0)
      digitalWrite(RELE2, (int(millis()/500) % 2 == 0) ? LOW : HIGH);
  }
}


void setIndicator(char channel){
      switch(btCmd){
        // Indicator left
        case 'L':
          if(releStatus[1] == 0){
            digitalWrite(RELE1, HIGH);
            digitalWrite(RELE2, LOW);
            releStatus[1] = 1;
            
          } else {
            releStatus[1] = 0;
          }
          break;

        // Indicator right
        case 'R':
          if(releStatus[2] == 0){
            digitalWrite(RELE1, LOW);
            digitalWrite(RELE2, HIGH);
            releStatus[2] = 1;
            
          } else {
            releStatus[2] = 0;
          }
          break;

        // Indicator all
        case 'Q':
          if(releStatus[0] == 0){
            digitalWrite(RELE1, HIGH);
            digitalWrite(RELE2, HIGH);
            releStatus[0] = 1;
            
          } else {
            releStatus[0] = 0;
          }
          break;

        // Indicator Stop
        case 'S':
          if(releStatus[3] == 0){
            digitalWrite(RELE3, HIGH);
            releStatus[3] = 1;
            
          } else {
            releStatus[3] = 0;
          }
          break;

        // Indicator  headlight
        case 'H':
          if(releStatus[4] == 0){
            digitalWrite(RELE4, HIGH);
            releStatus[4] = 1;
            
          } else {
            releStatus[4] = 0;
          }
          break;
      }

  
    
}


// 1 - Forward
void motorForward(short power, int time){
  digitalWrite(DS1, LOW);
  digitalWrite(DS2, HIGH);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  //digitalWrite(FS1, HIGH);
  analogWrite(FS1, power);
  delay(time);
}

// 2 - Reverse
void motorReverse(short power, int time){
  digitalWrite(DS1, LOW);
  digitalWrite(DS2, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  //digitalWrite(FS1, HIGH);
  analogWrite(FS1, power);
  delay(time);
}

