#include "Arduino.h"
#include "BluetoothSerial.h"
#include <HX711.h>
#include "SoftwareSerial.h"


HX711 scale;


#define EEPROM_SIZE 40

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 19;
const int LOADCELL_SCK_PIN = 4;
#define LedConnected 33 
#define LedConnectedDuration 500

const int calVal_eepromAdress_weight = 0;
float newCalibrationValue_weight;
unsigned long t = 0;

float weightCallValue = 0.0;

String message_data = "";
String message_data_copy = "";

int WEIGHT,WEIGHT_UNIT;

float WEIGHT_VALUE;


String sWEIGHT_VALUE;
String sWEIGHT_UNIT;

bool blinkState = 1;
bool LedConnectedFlag = false;
long previousmilisec ;


void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_gain(128);

  Serial.println("The device started, now you can pair it with bluetooth!");
 
  pinMode(LedConnected, OUTPUT);
  digitalWrite(LedConnected,HIGH);
  EEPROM.get(calVal_eepromAdress_weight,newCalibrationValue_weight);
 
  Serial.println(newCalibrationValue_weight);
  previousmilisec = millis();

}
  

void loop(){
//while(1){
//     HEIGHT_UNIT = 1;
//     newCalibrationValue_height = 220.0;
//     height();
//    }

//    weightCallValue = 70000.0;
//    calibrate_weight();
//    while(1){
//       weight(); 
//    }

  if(LedConnectedFlag == 1){
        digitalWrite(LedConnected,HIGH);
        //Serial.println("ledconnected");
      }
  if((millis() - previousmilisec) >= LedConnectedDuration){
     previousmilisec = millis();
     if(LedConnectedFlag == 1){
        digitalWrite(LedConnected,HIGH);
      }
     else{
          if(blinkState == 1) {
             digitalWrite(LedConnected,HIGH);
             blinkState = 0;
            }
          else{
              digitalWrite(LedConnected,LOW);
              blinkState = 1;
            }
         }
    }
  if (Serial.available()) {
    Serial.write(Serial.read());
  }
  if (Serial.available()){
    char inChar;
    //Serial.write(SerialBT.read());
    do{
      inChar = Serial.read();
      message_data += inChar;
      }while(inChar != '\n');
    //message_data = SerialBT.read();
//    message_data_copy.trim();
//    Serial.println(message_data_copy);
    
    Serial.println(message_data);
    message_data_copy = message_data;
    message_data = "";
    
    String codeValue = getValue(message_data_copy,',',0);
    String codeValue2 = getValue(message_data_copy,'?',2);
    codeValue2.trim();
    Serial.println(codeValue2);
    if(codeValue == "$"){
        weightCallValue = getValue(message_data_copy,',',1).toFloat();
        calibrate_weight();
        Serial.println("weight calibrated");
        Serial.println(weightCallValue);
      }
    else if(codeValue == "Z"){
        LedConnectedFlag = 1;
        Serial.println("ledconnected");
      }
    else if (codeValue2 == "~"){
        Serial.println("i got here");
        fillInData();
        proccess_received_value();
    }
    
  }
}
//------------end of loop


String getValue(String data, char separator, int index){
    int found = 0;
    int strIndex[] = { 0, -1 };

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
        }
    }
    return found ;
}

void fillInData(){
   
   WEIGHT = getValue(message_data_copy,'?',0).toInt();
   WEIGHT_UNIT = getValue(message_data_copy,'?',1).toInt();
   
  }

    

  void weight() {
       Serial.write('w');
       int weightDiff = 0;
       bool weightReady = 0;
       int wCnt = 4;
       float weight_values;
       delay(7000);
       //scale.set_scale(newCalibrationValue_weight);
       do{
           for(int x =0; x<2; x++){
                
                 scale.set_scale(newCalibrationValue_weight);
                 Serial.println(scale.get_units(10), 1);
                 weight_values = scale.get_units(10);
                 
                     if(WEIGHT_UNIT == 0){
                        WEIGHT_VALUE = weight_values/1000.0;
                        
                        Serial.print("Weight (Kg): ");
                        Serial.println(WEIGHT_VALUE);
                        //weightReady = 1;
                        //wCnt = 0;
                     }
                     else{
                        WEIGHT_VALUE = (weight_values/1000.0)*2.20462;
                        Serial.print("Weight (lb): ");
                        Serial.println(WEIGHT_VALUE);
                        //weightReady = 1;
                        //wCnt = 0;
                      }
                  
                  wCnt--;
                 delay(500);
              }
              if(weight_values > 10000){
                    weightReady = 1;
                    break;
                  }
              else{
                 Serial.print("No body on the scale");
                  delay(5000);
                 }
          }while(wCnt > 0);
          if(weightReady == 1){
             Serial.write('d');
             Serial.println("Whereeeeeeeeeeeeeeeee");
          }
          else{
              Serial.write('l');
              WEIGHT_VALUE = 00.00;
              Serial.println(WEIGHT_VALUE);
              Serial.println("youuuuuuuuuuuuuuuu");
            }
        delay(3000);
      }

    ////weight calibration

void calibrate_weight() {
      // Remove any calibration values and clear the scale
      scale.set_scale();
      scale.tare();
      Serial.write('Y');
      Serial.print("Stand on te weight");
      delay(9000);
      int userInput = 0;
      String inputString = "";
      // Now get the reading from the scale
      float calReading = scale.get_units(10);
      Serial.print(calReading);
      Serial.print("Setting the cal to ");
      Serial.println(calReading / weightCallValue);
      newCalibrationValue_weight = calReading / weightCallValue;
      scale.set_scale(newCalibrationValue_weight);
      delay(500);
    }

  void proccess_received_value(){
         if(WEIGHT == 1){
            weight();
            Serial.println("WEIGHT is configured");
          }
        else if(WEIGHT == 0){
            Serial.println("-----");
          }
         //
        
          delay(6000);
        Serial.println(String(WEIGHT_VALUE));

        Serial.println(String(WEIGHT_VALUE));
        }