#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <sstream>

#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define UP_LEFT 5
#define UP_RIGHT 6
#define DOWN_LEFT 7
#define DOWN_RIGHT 8
#define TURN_LEFT 9 
#define TURN_RIGHT 10
#define pan_left 11
#define pan_right 12
#define tilt_up 13
#define tilt_down 14
#define STOP 0

#define FRONT_RIGHT_MOTOR 0
#define BACK_RIGHT_MOTOR 1
#define FRONT_LEFT_MOTOR 2
#define BACK_LEFT_MOTOR 3

#define FORWARD 1
#define BACKWARD -1
static const int pan = 22;
static const int tilt = 23;
static const int sway = 24;
static const int gripper = 25;
int value = 10;
int initial_pos = 90;
int initial_pos1 = 90;
int initial_pos2 = 90;


Servo servo;
Servo servo1;
Servo servo2;
Servo servo3;

IPAddress local_IP(192, 168, 1, 184);

IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);


void setServo(int degrees, Servo s) {
  s.write(degrees);
}


struct MOTOR_PINS
{
  int pinIN1;
  int pinIN2;
  int pinEn; 
  int pwmSpeedChannel;
};

std::vector<MOTOR_PINS> motorPins = 
{
  {16, 17, 22, 4},  //BACK_RIGHT_MOTOR
  {18, 19, 23, 5},  //BACK_LEFT_MOTOR
  {26, 27, 14, 6},  //FRONT_RIGHT_MOTOR
  {33, 25, 32, 7},  //FRONT_LEFT_MOTOR   
};
const char* ssid     = "rover";
const char* password = "12345678";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
    <style>
       .slidecontainer {
      width: 100%;
    }
    .slider {
      -webkit-appearance: none;
      width: 100%;
      height: 15px;
      border-radius: 5px;
      background: #d3d3d3;
      outline: none;
      opacity: 0.7;
      -webkit-transition: .2s;
      transition: opacity .2s;
    }
    .slider:hover {
      opacity: 1;
    }
  
    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 25px;
      height: 25px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }
    .slider::-moz-range-thumb {
      width: 25px;
      height: 25px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }

    .arrows {
      font-size:25px;
      color:red;
    }
    .circularArrows {
      
      font-size:25px;
      color:#6eb1f0;
    }
    .circularArrows1 {
      
      font-size:25px;
      color:#ab6ef0;
    }
    .circularArrows2 {
      
      font-size:25px;
      color:#e8f09e;
    }
    
    td {
      background-color:black;
      border-radius:25%;
      box-shadow: 5px 5px #888888;
    }
    td:active {
      transform: translate(5px,5px);
      box-shadow: none; 
    }

    .noselect {
      -webkit-touch-callout: none; /* iOS Safari */
        -webkit-user-select: none; /* Safari */
         -khtml-user-select: none; /* Konqueror HTML */
           -moz-user-select: none; /* Firefox */
            -ms-user-select: none; /* Internet Explorer/Edge */
                user-select: none; /* Non-prefixed version, currently
                                      supported by Chrome and Opera */
    }
    </style>
  </head>
  <body class="noselect" align="center" style="background-color:#339e78">
     
    <h1 style="color: white;text-align:center;">oh yeaaah </h1>
    
    
    <table id="mainTable" style="width:400px;margin:auto;table-layout:fixed" CELLSPACING=10>
      <tr>
        <td ontouchstart='sendButtonInput("MoveCar", "5")' ontouchend='sendButtonInput("MoveCar", "0")'><span class="arrows" >&#11017;</span></td>
        <td ontouchstart='sendButtonInput("MoveCar", "1")' ontouchend='sendButtonInput("MoveCar", "0")'><span class="arrows" >&#8679;</span></td>
        <td ontouchstart='sendButtonInput("MoveCar", "6")' ontouchend='sendButtonInput("MoveCar", "0")'><span class="arrows" >&#11016;</span></td>
      </tr>
      
      <tr>
        <td ontouchstart='sendButtonInput("MoveCar", "3")' ontouchend='sendButtonInput("MoveCar", "0")'><span class="arrows" >&#8678;</span></td>
        <td></td>    
        <td ontouchstart='sendButtonInput("MoveCar", "4")' ontouchend='sendButtonInput("MoveCar", "0")'><span class="arrows" >&#8680;</span></td>
      </tr>
      
      <tr>
        <td ontouchstart='sendButtonInput("MoveCar", "7")' ontouchend='sendButtonInput("MoveCar", "0")'><span class="arrows" >&#11019;</span></td>
        <td ontouchstart='sendButtonInput("MoveCar", "2")' ontouchend='sendButtonInput("MoveCar", "0")'><span class="arrows" >&#8681;</span></td>
        <td ontouchstart='sendButtonInput("MoveCar", "8")' ontouchend='sendButtonInput("MoveCar", "0")'><span class="arrows" >&#11018;</span></td>
      </tr>
    
      <tr>
        <td ontouchstart='sendButtonInput("MoveCar", "9")' ontouchend='sendButtonInput("MoveCar", "0")'><span class="circularArrows" >&#8634;</span></td>
        <td style="background-color:#339e78;box-shadow:none"></td>
        <td ontouchstart='sendButtonInput("MoveCar", "10")' ontouchend='sendButtonInput("MoveCar", "0")'><span class="circularArrows" >&#8635;</span></td>
      </tr>
            <tr>
        <td style="text-align:left"><b>Pan:</b></td>
        <td colspan=2>
         <div class="slidecontainer">
            <input type="range" min="0" max="180" value="90" class="slider" id="Pan" oninput='sendButtonInput("Pan",value)'>
          </div>
        </td>
      </tr> 
      <tr>
        <td style="text-align:left"><b>Tilt:</b></td>
        <td colspan=2>
          <div class="slidecontainer">
            <input type="range" min="0" max="180" value="90" class="slider" id="Tilt" oninput='sendButtonInput("Tilt",value)'>
          </div>
        </td>   
      </tr>   
      <tr>
        <td style="text-align:left"><b>sway:</b></td>
        <td colspan=2>
          <div class="slidecontainer">
            <input type="range" min="0" max="180" value="90" class="slider" id="Sway" oninput='sendButtonInput("Sway",value)'>
          </div>
        </td>   
      </tr>   
      <tr>
        <td style="text-align:left"><b>gripper:</b></td>
        <td colspan=2>
          <div class="slidecontainer">
            <input type="range" min="0" max="180" value="90" class="slider" id="Gripper" oninput='sendButtonInput("Gripper",value)'>
          </div>
        </td>   
      </tr>   
<!--    ` <tr>
        <td ontouchstart='sendButtonInput("11")' ontouchend='sendButtonInput("0")'><span class="circularArrows1" >&#8634;</span></td>
        <td style="background-color:#339e78;box-shadow:none"></td>
        
        <td ontouchstart='sendButtonInput("12")' ontouchend='sendButtonInput("0")'><span class="circularArrows1" >&#8635;</span></td>
      </tr>-->
      <!--<tr>
        <td ontouchstart='sendButtonInput("13")' ontouchend='sendButtonInput("0")'><span class="circularArrows2" >&#8634;</span></td>
        <td style="background-color:#339e78;box-shadow:none"></td>
        <td ontouchstart='sendButtonInput("14")' ontouchend='sendButtonInput("0")'><span class="circularArrows2" >&#8635;</span></td>
      </tr>-->
    </table>

    <script>
      var webSocketUrl = "ws:\/\/" + window.location.hostname + "/ws";
      var websocket;
      
      function initWebSocket() 
      {
        websocket = new WebSocket(webSocketUrl);
        websocket.onopen    = function(event){
            
            sendButtonInput("Pan", document.getElementById("Pan").value);
            sendButtonInput("Tilt", document.getElementById("Tilt").value); 
            sendButtonInput("Sway", document.getElementById("Sway").value); 
            sendButtonInput("Gripper", document.getElementById("Gripper").value);            
        };
        websocket.onclose   = function(event){setTimeout(initWebSocket, 2000);};
        websocket.onmessage = function(event){};
      }

      function sendButtonInput(key, value) 
      {
        var data = key + "," + value;
        websocket.send(data);
        console.log(data);
      }
          
      window.onload = initWebSocket;
      document.getElementById("mainTable").addEventListener("touchend", function(event){
        event.preventDefault()
      });      
    </script>
    
  </body>
</html> 

)HTMLHOMEPAGE";


void rotateMotor(int motorNumber, int motorDirection)
{
  if (motorDirection == FORWARD)
  {
    digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);    
  }
  else if (motorDirection == BACKWARD)
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, HIGH);     
  }
  else
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);       
  }
}

void processCarMovement(int inputValue)
{
  //Serial.printf("Got value as %s %d\n", inputValue);  
  switch(inputValue)
  {

    case UP:
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);   
      Serial.println("move up");     
      break;
  
    case DOWN:
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);  
      Serial.println("move down");
      break;
  
    case LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);   
      Serial.println("move down");
      break;
  
    case RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);  
      Serial.println("move down");
      break;
  
    case UP_LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);  
      Serial.println("move up left");
      break;
  
    case UP_RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, STOP);  
      Serial.println("move up right");
      break;
  
    case DOWN_LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, STOP);   
      Serial.println("move down left");
      break;
  
    case DOWN_RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);   
      Serial.println("move down right");
      break;
  
    case TURN_LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);  
      Serial.println("turn left");
      break;
  
    case TURN_RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);   
      Serial.println("turn right");
      break;
  
    
    // case pan_left:
    //   pos += value;
    //   servo.write(pos);
    //   Serial.println("pan left ");
    //   break;
    // case pan_right:
    //   pos -= value;
    //   servo.write(pos);
    //   Serial.println("pan right ");
    //   break;
    // case tilt_down:
    //   pos1 += value;
    //   servo1.write(pos1);
    //   Serial.println("tilt down ");
    //   break;
    // case tilt_up:
    //   pos1 -= value;
    //   servo1.write(pos1);
    //   Serial.println("tilt up ");
    //   break;

    case STOP:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, STOP);  
 
      break;
    
  
    default:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, STOP);    
      break;
  }
}

void handleRoot(AsyncWebServerRequest *request) 
{
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "File Not Found");
}


void onWebSocketEvent(AsyncWebSocket *server, 
                      AsyncWebSocketClient *client, 
                      AwsEventType type,
                      void *arg, 
                      uint8_t *data, 
                      size_t len) 
{                      
  switch (type) 
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      //client->text(getRelayPinsStatusJson(ALL_RELAY_PINS_INDEX));
      servo.write(90);
      servo1.write(90);
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      processCarMovement(0);
      servo.write(90);
      servo1.write(90);
      
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);        
        std::istringstream ss(myData);
        std::string key, value;
        std::getline(ss, key, ',');
        std::getline(ss, value, ',');
        Serial.printf("Key [%s] Value[%s]\n", key.c_str(), value.c_str()); 
        int valueInt = atoi(value.c_str());     
        if (key == "MoveCar")
        {
          processCarMovement(valueInt);        
        }
        else if (key == "Pan")
        {
          servo.write(valueInt);
          Serial.printf("moving sercvo pan \n");
        }
        else if (key == "Tilt")
        {
          servo1.write(valueInt);   
          Serial.printf("moving sercvo tilt\n");
        }  
        else if (key == "Sway")
        {
          servo2.write(valueInt);   
          Serial.printf("moving sercvo sway\n");
        }              
        else if (key == "Gripper")
        {
          servo3.write(valueInt);   
          Serial.printf("moving sercvo gripper\n");
        }     
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;  
  }
}

void setUpPinModes()
{

  for (int i = 0; i < motorPins.size(); i++)
  {
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);  
    rotateMotor(i, STOP);  
  }
}



void setup(void) 
{
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  setUpPinModes();
  servo1.attach(tilt);
  servo2.attach(sway);
  Serial.begin(115200);
  servo.attach(pan);
  servo3.attach(gripper);
  WiFi.config(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
  
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);
  
  server.begin();
  Serial.println("HTTP server started");
  servo.write(initial_pos);
  servo1.write(initial_pos1);
  servo2.write(initial_pos2);
}

void loop() 
{
  ws.cleanupClients(); 
}
