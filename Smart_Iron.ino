#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
int sakshi = 0;
int abc;
#include <Adafruit_MPU6050.h>

Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_accel;
float last_accel_x= 0;
float last_accel_y = 0;
float last_accel_z = 0;
int count = 0;
const int relay = 2;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
// Update these with values suitable for your network.

const char* ssid = "Galaxy F22";
const char* password = "Kalpana981";
const char* mqtt_server = "ec2-13-232-136-124.ap-south-1.compute.amazonaws.com";
int eepromAddr1 = 1, eepromAddr2 = 10;
int  eepromAddr = 20;
int intData = 1022344, getInt;
String stringData = "Sakshi", getStr;
int LED1 = 16;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (100)
char msg[MSG_BUFFER_SIZE];
int value = 0;



void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
    Serial.begin(115200);
  while (!Serial)
    delay(1000); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");
if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(1000);
    }
  }

  Serial.println("MPU6050 Found!");
  mpu_accel = mpu.getAccelerometerSensor();
  mpu_accel->printSensorDetails();


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());

  

  Serial.println("");
  Serial.println("WiFi connected");
//  digitalWrite(LED1, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.print("LED");
  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();
  timeClient.setTimeOffset(0);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "abcd";
    //clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("iot_pub", msg);
      // ... and resubscribe
      client.subscribe("iot_sub");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 9999);
  client.setCallback(callback);
  
}

int prev_intData = 0;

void loop() {
  timeClient.update();

  time_t epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);
  int currentMinute = timeClient.getMinutes();
  Serial.print("Minutes: ");
  Serial.println(currentMinute); 
  
  int currentSecond = timeClient.getSeconds();
  Serial.print("Seconds: ");
  Serial.println(currentSecond);
     
  if (currentSecond%60==0){
    sakshi++;
    Serial.println(sakshi);
    int url = sakshi;
    Serial.println(url);
    abc = (208*url);
  }
  else {
    abc = abc;
  }
   

  String weekDay = weekDays[timeClient.getDay()];
  Serial.print("Week Day: ");
  Serial.println(weekDay);    

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(monthDay);
//
  int currentMonth = ptm->tm_mon+1;
  Serial.print("Month: ");
  Serial.println(currentMonth);
//
  String currentMonthName = months[currentMonth-1];
  Serial.print("Month name: ");
  Serial.println(currentMonthName);

  int currentYear = ptm->tm_year+1900;
  Serial.print("Year: ");
  Serial.println(currentYear);

  //Print complete date:
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.print("Current date: ");
  Serial.println(currentDate);

  Serial.println("");
  Serial.print("abc: ");
  Serial.println(abc);

  delay(2000);
  unsigned long now = millis();

//  //calucluate the time
//  intData = random(0xffff);


  //store the time data to eeprom
  if (prev_intData != abc){
    EEPROM.put(eepromAddr1, abc);
    //EEPROM.put(eepromAddr2, stringData);

    prev_intData = abc;
    /*delay(10);

    int f = EEPROM.read( eepromAddr1);
    Serial.print("eeprom read: ");
    Serial.println(f);*/
  }

  if (now- lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "%d", abc);
    Serial.print("Publish message: ");
    Serial.println(abc);
    client.publish("iot_pub", msg);
  }
//  else{
//     EEPROM.get(eepromAddr1, intData);
//     EEPROM.get(eepromAddr2, stringData);
//     Serial.print("password:");
//     Serial.println(intData);
//     Serial.print("ssid:");
//     Serial.println(stringData);
//  }
  if (Serial.available()) {
    String x = Serial.readString();
    EEPROM.begin (0);
    delay(10);
    Serial.print("\nInput → ");
    Serial.println(x);

    

    if (x == "read") {
     
      EEPROM.get(eepromAddr1, abc);
      EEPROM.get(eepromAddr2, stringData);
      Serial.print("password:");
      Serial.println(intData);
      Serial.print("ssid:");
      Serial.println(stringData);
    } 
    else if (x == "int") {
      EEPROM.put(eepromAddr1, abc);
      Serial.println("intData write in EEPROM is Successful");
    }   
    else if (x == "string") {
      EEPROM.put(eepromAddr2, stringData);
      Serial.println("stringData write in EEPROM is Successful");
    }
    else if (x == "clear") {
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
      }
      Serial.println("EEPROM Clear Done!");
    }
    else {
      Serial.println("Wrong Input");
    }
    EEPROM.commit();
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  sensors_event_t accel;
  
  mpu_accel->getEvent(&accel);


  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("\t\tAccel X: ");
  Serial.print(accel.acceleration.x);
  Serial.print(" \tY: ");
  Serial.print(accel.acceleration.y);
  Serial.print(" \tZ: ");
  Serial.print(accel.acceleration.z);
  Serial.println(" m/s^2 ");

 if((abs(accel.acceleration.x - last_accel_x)>1) || (abs(accel.acceleration.y - last_accel_y)>1) || (abs(accel.acceleration.z - last_accel_z)>1) ){
      Serial.println("Iron is moving");
      count = 0;
      pinMode(relay, OUTPUT);
    }
   else{
      Serial.println("Iron is stationary");
      Serial.println(count);
       
      if(count >= 10000){  
        Serial.println("TURN OFF THE IRON");
        pinMode(relay, INPUT);
      }
      count = count + 2000;
    }
    
    Serial.println("acceleration along x axis");
    Serial.println(last_accel_x);
    last_accel_x = accel.acceleration.x;
    Serial.println(last_accel_x);
    
    
    Serial.println("along y axis");
    Serial.println(last_accel_y);
    last_accel_y = accel.acceleration.y;
    Serial.println(last_accel_y);

    Serial.println("acceleration along z axis");
    Serial.println(last_accel_z);
    last_accel_z = accel.acceleration.z;
    Serial.println(last_accel_z);
    delay(2000);
  
  
  }
