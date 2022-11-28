#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

//Firebase settings
#define FIREBASE_HOST "esp8266-firebase-lamp-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "Vra7IVCbzWMhz2dSACfl2QjX9gH2qoe7BFKHFv4P"

//Wi-Fi settings
#define WIFI_SSID "SCM Office lt2"
#define WIFI_PASSWORD "dronejammer21"

//lamp & ultrasonic 1
constexpr uint8_t lamp1 = D3;  
constexpr uint8_t trigPinUS1 = D1;  
constexpr uint8_t echoPinUS1 = D2;  

//ultrasonic 2
constexpr uint8_t lamp2 = D4;
constexpr uint8_t trigPinUS2 = D5;  
constexpr uint8_t echoPinUS2 = D6;  

//mode
String lampMode1 = "remote";
String lampMode2 = "remote";

//Status
int statusLamp1 = 0;
int statusLamp2 = 0;

// The amount of time the ultrassonic wave will be travelling for
long duration = 0;
// Define the distance variable
double distance = 0;

void setup()
{
    Serial.begin(9600);
    // Connect to Wi-Fi
    Serial.print("Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("Connected to: ");
    Serial.println(WiFi.localIP());

    //init firebase
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.setString("lampMode1", lampMode1);
    Firebase.setInt("statusLamp1", 0);
    // Ultrasonic sensor, set echo as Input and trigger as Output
    pinMode(lamp1, OUTPUT);
    pinMode(lamp2, OUTPUT);
    pinMode(trigPinUS1, OUTPUT);
    pinMode(echoPinUS1, INPUT);

    pinMode(trigPinUS2, OUTPUT);
    pinMode(echoPinUS2, INPUT);
}

void loop()
{
  lampAuto(1);

  lampAuto(2);
  // getDistance(2);
  // Serial.println(distance);
  // delay(500);
  // autoLamp();
}

double getDistance(int i)
{
    duration = 0;
    distance = 0;
    if (i == 1) {
      // Clear trigPin
      digitalWrite(trigPinUS1, LOW);
      delayMicroseconds(2);
  
      // trigPin HIGH por 10ms
      digitalWrite(trigPinUS1, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPinUS1, LOW);
  
      //Reads echoPin, returns the travel time of the sound wave in ms
      duration = pulseIn(echoPinUS1, HIGH);
  
      // Calculating the distance, in centimeters, using the formula described in the first section.
      distance = duration * 0.034 / 2;    
      return distance; 
    } else {
      // Clear trigPin
      digitalWrite(trigPinUS2, LOW);
      delayMicroseconds(2);
  
      // trigPin HIGH por 10ms
      digitalWrite(trigPinUS2, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPinUS2, LOW);
  
      //Reads echoPin, returns the travel time of the sound wave in ms
      duration = pulseIn(echoPinUS2, HIGH);
  
      // Calculating the distance, in centimeters, using the formula described in the first section.
      distance = duration * 0.034 / 2;    
      return distance;
    }
}
String lampMode;
int statusLamp;
String fbMode;
String fbStatus;
uint8_t lampNum;

void lampAuto(int i){
  getDistance(i);

  if (i == 1){
    fbMode = "lampMode1";
    fbStatus = "statusLamp1";
    lampNum = D3;
  } else if (i == 2){
    fbMode = "lampMode2";
    fbStatus = "statusLamp2";
    lampNum = D4;
  }

  lampMode = Firebase.getString(fbMode);
  statusLamp = Firebase.getInt(fbStatus);
  Serial.println("");
  Serial.print(fbMode);
  Serial.print(" : ");
  Serial.print(distance);
  Serial.println("");
  if (lampMode == "remote"){
    if (statusLamp == 1){
      digitalWrite(lampNum, HIGH);
    } else if (statusLamp == 0){
      digitalWrite(lampNum, LOW);
    } 
  } else if(lampMode == "auto"){
     if (distance < 100){
        if (statusLamp < 1){
          digitalWrite(lampNum, HIGH);
          statusLamp = 1;  
        } 
    }  else {
      if (statusLamp > 0){
        digitalWrite(lampNum, LOW);
        statusLamp = 0;
      }
    } 
    Firebase.setInt(fbStatus, statusLamp);
  }
 delay(1000);

}

void autoLamp() {
  lampNum = D3;
  Serial.println(distance);
  getDistance(1);
  if (distance < 99){
    digitalWrite(lampNum, HIGH);
  } else {
    digitalWrite(lampNum, LOW);
  }
   delay(1000);
}