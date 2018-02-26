#include <SPI.h>
#include <SoftwareSerial.h>
#include <SdFat.h> // Set "ENABLE_SOFTWARE_SPI_CLASS  = 1" in SdFat/SdFatConfig.h
#include <TMRpcm.h> // Uncomment "#define SDFAT" in TMRpcm/pcmConfig.h

// Pin Bluetooth
#define BT_RX 3
#define BT_TX 2

// Pin motor
#define FS1 4
#define IN1 5
#define IN2 6
#define DS1 13
#define DS2 12

// Pin rele'
#define RELE1 10
#define RELE2 11
#define RELE3 8
#define RELE4 9

// Pin SD
#define SOFT_MISO_PIN A0 // 12
#define SOFT_SCK_PIN A1  // 13
#define SOFT_MOSI_PIN A2 // 11
#define SD_CHIP_SELECT_PIN A3 // 10

// Pin speaker
#define SPEAKER A4

// Global variables
int releStatus[6] = {0, 0, 0, 0, 0, 0};
unsigned long time = 0;
String btMessage = "";
String track = "music";
SoftwareSerial bluetooth(BT_RX,BT_TX);
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;
TMRpcm player;

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

  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  pinMode(RELE3, OUTPUT);
  pinMode(RELE4, OUTPUT);

  digitalWrite(RELE1, LOW);
  digitalWrite(RELE2, LOW);
  digitalWrite(RELE3, LOW);
  digitalWrite(RELE4, LOW);

  // check rele'
  digitalWrite(RELE1, HIGH);
  delay(250);
  digitalWrite(RELE1, LOW);
  digitalWrite(RELE2, HIGH);
  delay(250);
  digitalWrite(RELE2, LOW);
  digitalWrite(RELE3, HIGH);
  delay(250);
  digitalWrite(RELE3, LOW);
  digitalWrite(RELE4, HIGH);
  delay(250);
  digitalWrite(RELE4, LOW);

  // Setup music players
  player.speakerPin = SPEAKER;
  player.quality(1);

  // initialize the first card
  if (!sd.begin(SD_CHIP_SELECT_PIN)) {
    Serial.println("SD Disconnected");  

  } else {
    Serial.println("SD Connected!");
  }
}


void loop(){
  while(bluetooth.available())
    btMessage += char(bluetooth.read());
  
  if(!bluetooth.available()) {
    if(btMessage.length() > 0){
      Serial.println("BT="+ btMessage);
      
      switch(btMessage[0]){
        // Move forward
        case 'F':
          motorForward(128, 1000);
          break;

        // Move back
        case 'B':
          motorReverse(128, 1000);
          break;

        // Move back
        case 'W':
          motorStop();
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
          track = btMessage.substring(1);

          if(player.isPlaying())
            player.stopPlayback(); //pause();
          
          else
            player.play(track.c_str());

          break;

        case 'V':
          if(btMessage[1] == '+')
            player.volume(1);
            
          else if(btMessage[1] == '-')
            player.volume(0);

          break;

      }
      btMessage = "";
    clearActions();
    }
  }
  blinkIndicator();

  delay(100);
}

void clearActions(){
  if(releStatus[0] == 0){
    digitalWrite(RELE1, LOW);
    digitalWrite(RELE2, LOW);

  } else if(releStatus[1] == 0)
      digitalWrite(RELE1, LOW);

    else if(releStatus[2] == 0)
      digitalWrite(RELE2, LOW);

  if(releStatus[3] == 0)
    digitalWrite(RELE3, LOW);

  if(releStatus[4] == 0)
    digitalWrite(RELE4, LOW);

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
      switch(channel){
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
  analogWrite(FS1, power);
  delay(time);
}

// 2 - Reverse
void motorReverse(short power, int time){
  digitalWrite(DS1, LOW);
  digitalWrite(DS2, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(FS1, power);
  delay(time);
}

// 3 - Stop
void motorStop(){
  digitalWrite(DS1, LOW);
  digitalWrite(DS2, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(FS1, LOW);
}

