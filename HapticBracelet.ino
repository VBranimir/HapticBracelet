#include <PubSubClient.h>
#include <ESP8266WiFi.h>
 
const char* ssid = "WIFIRouterSSID";
const char* password = "WIFIPASSWORD"; 
char* topic = "/haptic/#";   
char* server = "BROKER_IP_ADDRESS"; 
char message_buff[100];  
String msgData;
unsigned long oldPressTime = 0;
unsigned long lastClickTime = 0;
boolean oneClick = false;

WiFiClient wifiClient;


// PubSubClient MQTT subscription function:
void callback(char* topic, byte* payload, unsigned int length) {
  int i = 0;
  Serial.println("Message arrived:  topic: " + String(topic));
  Serial.println("Length: " + String(length,DEC)); 
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  //Buffer to string conversion
  String msgData = String(message_buff);
  String topicData = (String(topic) + String(msgData));
  Serial.println("Payload: " + msgData);
  modeSelect(topicData);
  }


PubSubClient client(server, 1883, callback, wifiClient);
  
String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}


//Button events
void clickHold() {}
void singleHold() {}
void doubleHold() {}
void doubleHoldUp() {client.publish("/haptic/input","4");} // double hold
void singleHoldUp() {client.publish("/haptic/lock","1");} // single hold
void clickDown() {}
void firstDown() {}
void doubleDown() {}
void clickUp() {}
void firstUp() {}
void doubleUp() {client.publish("/haptic/input","2");} //double click
void soloUp() {client.publish("/haptic/input","1");} // sigle click


//ClickWatcher function handles button press events:
void clickWatcher(byte pin) {
  static const int doubleWait = 400;
  static const int holdDelay = 500;
  static const int debounce = 150;
  static unsigned long pressTime;
  static boolean clicked;
  static boolean doubleClick;
  static boolean holdFlag;
  static boolean doubleFlag = false;  
  boolean pinStatus = !digitalRead(pin);
    
  if(pinStatus && millis()-pressTime>debounce) {
    if (!clicked && !doubleClick && millis()-pressTime<doubleWait && !holdFlag) {
      doubleClick= true;
      doubleFlag=true;
    }
    if (clicked && (millis()-pressTime)>holdDelay) {
      holdFlag = true;
      clickHold();
      if(doubleClick) doubleHold();
      else singleHold();
    }
    else if(!clicked) {
      clickDown();
      clicked = true;
      holdFlag=false;
      pressTime = millis();
      if(doubleClick) doubleDown();
      else firstDown();
    }
  }
  else if(clicked && millis()-pressTime>debounce) {
    clickUp();
    if(holdFlag) {
      if(doubleClick) doubleHoldUp();
      else {
        singleHoldUp();
        doubleFlag = true;
      }
    }
    else {
      if(doubleClick) doubleUp();
      else{
        firstUp();
        doubleFlag = false; 
      }
    }
    clicked=false;
    doubleClick=false;
  }
  else if(!doubleFlag && millis()-pressTime>doubleWait) {soloUp(); doubleFlag = true;}
}


void setup() {
  pinMode(5, OUTPUT);
  Serial.begin(115200);
  delay(10);
  digitalWrite(5, 0);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid); 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected"); 
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
//  connection to broker script.
  if (client.connect("arduinoClient")) {
    client.publish("/haptic/IO","Haptic is up"); //MQTT publish to the topic "/haptic/IO"and msg "Haptic is up"
    client.subscribe(topic);
  }  
}


// Long vibration function:
void longVibe(int repeatCount) {
  for(int i = 0; i < repeatCount; ++i) {
  digitalWrite(5, HIGH);   // turn the haptic motor on (HIGH is the voltage level)
  delay(200);              // wait for n time
  digitalWrite(5, LOW);    // turn the haptic motor off by making the voltage LOW
  delay(200);              // wait for n time
  }
}


// Long vibration for zero function:
void zeroVibe() {
  digitalWrite(5, HIGH);   // turn the haptic motor on (HIGH is the voltage level)
  delay(600);              // wait for n time
  digitalWrite(5, LOW);    // turn the haptic motor off by making the voltage LOW
  delay(200);              // wait for n time
}


// Short vibration function:
void shortVibe(int repeatCount) {
  for(int i = 0; i < repeatCount; ++i) {
  digitalWrite(5, HIGH);   // turn the haptic motor on (HIGH is the voltage level)
  delay(80);              // wait for n time
  digitalWrite(5, LOW);    // turn the haptic motor off by making the voltage LOW
  delay(200);              // wait for n time
  }
}


// "Mode" vibration for input data streams: 
void modeVibe(int repeatCount) {
  for(int i = 0; i < repeatCount; ++i) {
  digitalWrite(5, HIGH);   // turn the haptic motor on (HIGH is the voltage level)
  delay(80);              // wait for n time
  digitalWrite(5, LOW);    // turn the haptic motor off by making the voltage LOW
  delay(200);              // wait for n time
  }
}



void sleep() {
   delay(900);
}


//Clock function segment:
void hClock(String msg) { 
  Serial.println("The time is: " + msg);
  //Extract values from string
  String bigHour = (msg.substring(0,1));
  String smallHour = (msg.substring(1,2));
  String bigMinute = (msg.substring(3,4));
  String smallMinute = (msg.substring(4,5));
  int modeNumber = 1;
  modeVibe(modeNumber);
  sleep();
  Serial.println("haptic.Print: " + bigHour); //if bigHour.toInt()) == 0 then ultralongwibe else :
  if (bigHour.toInt() == 0) {
    zeroVibe();
  }
  else {
  longVibe(bigHour.toInt());
  }
  sleep();
  Serial.println("haptic.Print: " + smallHour);
  if (smallHour.toInt() == 0) {
    zeroVibe();
  }
  else {
  shortVibe(smallHour.toInt());
  }
  sleep();
  Serial.println("haptic.Print: " + bigMinute); 
  if (bigMinute.toInt() == 0) {
    zeroVibe();
  }
  else {
  longVibe(bigMinute.toInt());
  }
  sleep();
  Serial.println("haptic.Print: " + smallMinute);
  if (smallMinute.toInt() == 0) {
    zeroVibe(); 
  }
  else {
  longVibe(smallMinute.toInt());
  }
  return;
}


// Temperature Function segment:
void hTemp(String msg) {
  Serial.println("temperature is " + msg + "°c");
  //Extract values from string
  String bigTemp = (msg.substring(0,1));
  String smallTemp = (msg.substring(1,2));
  int modeNumber = 2;
  modeVibe(modeNumber);
  sleep();
  Serial.println("haptic.Print: " + bigTemp);
  if (bigTemp.toInt() == 0) {
    zeroVibe();
  }
  else {
  longVibe(bigTemp.toInt());
  }
  sleep();
  Serial.println("haptic.Print: " + smallTemp); 
  if (smallTemp.toInt() == 0) {
    zeroVibe();
  }
  else {
  shortVibe(smallTemp.toInt());
  }
  return;
}


void hDoor() {
  longVibe(3);
}


// Function uses MQTT topics to select which mode to use. 
void modeSelect(String topicData) {
   Serial.println(topicData);
   String msgTopic = (topicData.substring(0,12));
   Serial.println(msgTopic);
   String msg = (topicData.substring(12));
   if (msgTopic == "/haptic/time") {
    hClock(msg);
   }
   if (msgTopic == "/haptic/temp") {
    hTemp(msg);
   }
   if (msgTopic == "/haptic/door") {
    hDoor();
   }
}



void loop() {
  clickWatcher(4);  
  client.loop();
}

