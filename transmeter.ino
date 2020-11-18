//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//
//************************************************************
#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * .01 , TASK_FOREVER, &sendMessage );

const int SENSOR_PIN = D2; 
const int MOTION_PIN = D6; // Pin connected to motion detector
const int LED_PIN = LED_BUILTIN; // LED pin - active-high
const int RELAY_PIN = D0; // LED pin - active-high


long time1=0;
String msg = "";

void sendMessage() {
  //msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  if(msg.indexOf("ON")>=0){
    time1=millis();
    Serial.println("Recieved ON");
    time1=millis();
    digitalWrite(LED_PIN,LOW);//if sensor value is 1 then built-in LED will turn OFF.(LED inverted)
    digitalWrite(RELAY_PIN, HIGH);
    //Serial.println("Motion Detected");
  }
  else if(msg.indexOf("OFF")>=0){
    Serial.println("Recieved OFF");
    //digitalWrite(LED_PIN,HIGH);
  }
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  pinMode(MOTION_PIN, INPUT_PULLUP);
  pinMode(SENSOR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(SENSOR_PIN, HIGH);
  delay(1000);
//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  
  
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  
  int proximity = digitalRead(MOTION_PIN);
  if (proximity == LOW ) // If the sensor's output goes low, motion is detected
  {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Motion detected!");
    time1=millis();
    msg="ON";
  }
  else if(millis() > time1+5000)
  {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("No motion detected!");
    msg="OFF";
  }
}
