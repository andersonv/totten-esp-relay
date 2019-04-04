#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <PubSubClient.h>

#define RELAY 0
#define LED 2

String serialRead = "";

ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

void blinkLed(int qtd, int timer) {
  
  for(int i=0; i<qtd; i++) {
    delay(timer);
    digitalWrite(LED, 0);
    delay(timer);
    digitalWrite(LED, 1);
  }
  
}

void callbackMQTT(char* topic, byte* payload, unsigned int length) {
 
  if(payload[0] == '0'){
    digitalWrite(RELAY, 1);
    blinkLed(5,20);
  } else if (payload[0] == '1'){
    digitalWrite(RELAY, 0);
    blinkLed(5,20);
  }

}

bool hasSSID(){

  for (int i = 0; i < 32; ++i) {
    if(EEPROM.read(i) != 0) {
      return true;
    }
  }

  return false;

}

bool hasPass(){
 
  for (int i = 32; i < 96; ++i) {
    if(EEPROM.read(i) != 0) {
      return true;
    }
  }

  return false;
  
}

void setSSID(String ssid) {
  for (int i = 0; i < ssid.length(); ++i) {
    EEPROM.write(i, ssid[i]);
  }

  EEPROM.commit();
}

void setPass(String pass) {
  
  for (int i = 0; i < pass.length(); ++i) {
    EEPROM.write(32+i, pass[i]);
  }

  EEPROM.commit();

}

bool hasMQTTServer(){
  for (int i = 96; i < 120; ++i) {
    if(EEPROM.read(i) != 0) {
      return true;
    }
  }

  return false;
}

void setMQTTServer(String server) {
  
  for (int i = 0; i < server.length(); ++i) {
    EEPROM.write(96+i, server[i]);
  }

  EEPROM.commit();
}

String getMQTTServer(){
  String server = "";
  
  for (int i = 96; i < 120; ++i) {
    if(EEPROM.read(i) != 0) {
      server += char(EEPROM.read(i));
    }
  }

  return server;
}

bool hasMQTTUser(){
  for (int i = 120; i < 144; ++i) {
    if(EEPROM.read(i) != 0) {
      return true;
    }
  }

  return false;
}

void setMQTTUser(String user) {
  
  for (int i = 0; i < user.length(); ++i) {
    EEPROM.write(120+i, user[i]);
  }

  EEPROM.commit();
}

String getMQTTUser() {
  String user = "";
  
  for (int i = 120; i < 144; ++i) {
    if(EEPROM.read(i) != 0) {
      user += char(EEPROM.read(i));
    }
  }

  return user;
}

bool hasMQTTPass(){
  for (int i = 144; i < 166; ++i) {
    if(EEPROM.read(i) != 0) {
      return true;
    }
  }

  return false;
}

void setMQTTPass(String pass) {
  
  for (int i = 0; i < pass.length(); ++i) {
    EEPROM.write(144+i, pass[i]);
  }

  EEPROM.commit();
}

String getMQTTPass() {
  String pass = "";
  
  for (int i = 144; i < 166; ++i) {
    if(EEPROM.read(i) != 0) {
      pass += char(EEPROM.read(i));
    }
  }

  return pass;
}

bool hasMQTTPort(){
  for (int i = 166; i < 172; ++i) {
    if(EEPROM.read(i) != 0) {
      return true;
    }
  }

  return false;
}

void setMQTTPort(String pass) {
  
  for (int i = 0; i < pass.length(); ++i) {
    EEPROM.write(166+i, pass[i]);
  }

  EEPROM.commit();
}

String getMQTTPort() {
  String pass = "";
  
  for (int i = 166; i < 172; ++i) {
    if(EEPROM.read(i) != 0) {
      pass += char(EEPROM.read(i));
    }
  }

  return pass;
}

String getSSID() {
  
  String ssid = "";
  
  for (int i = 0; i < 32; ++i) {
    if(EEPROM.read(i) != 0) {
      ssid += char(EEPROM.read(i));
    }
  }

  return ssid;

}

String getPass() {

  String pass = "";
  
  for (int i = 32; i < 96; ++i) {
    if(EEPROM.read(i) != 0) {
      pass += char(EEPROM.read(i));
    }
  }

  return pass;
  
}

bool testConnection(String ssid, String password) {

  WiFi.begin(ssid, password);
  
  int c = 0;
  
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED) { return true; }
    delay(500);
    c++;
  }
  Serial.println(WiFi.status());
  return false;
  
}

void handleRoot() {
  String qsid = server.arg("ssid");
  String qpass = server.arg("pass");
  String mqtt_server = server.arg("mqtt_server");
  String mqtt_port = server.arg("mqtt_port");
  String mqtt_user = server.arg("mqtt_user");
  String mqtt_pass = server.arg("mqtt_pass");
  String content;

  if (qsid.length() > 0 && qpass.length() > 0){
    
    for (int i = 0; i < 172; ++i) { EEPROM.write(i, 0); }
    EEPROM.commit();
    setSSID(qsid);
    setPass(qpass);
    setMQTTServer(mqtt_server);
    setMQTTPort(mqtt_port);
    setMQTTUser(mqtt_user);
    setMQTTPass(mqtt_pass);
    
    content = "<h2>router: " + getSSID() + "</h2>";
    content += "<h2>password: " + getPass() + "<h2>";
    
    content += "<h2>MQTT Server: " + getMQTTServer() + "</h2>";
    content += "<h2>MQTT Port: " + getMQTTPort() + "</h2>";
    content += "<h2>MQTT User: " + getMQTTUser() + "</h2>";
    content += "<h2>MQTT Pass: " + getMQTTPass() + "</h2>";

  } else {
    
    int n = WiFi.scanNetworks();
    String st;
    content = "<!DOCTYPE html><html><head><title>TOTTEN - Smart Plugs</title><meta charset='utf-8'></head>";
    content += "<script type='text/javascript'> window.addEventListener('load', function(){  var networks = document.getElementsByTagName('li'); var field = document.getElementById('ssid'); field.value = ''; for(var i=0;i<networks.length;i++){ networks[i].addEventListener('click',function(){ field.value = this.innerText;});}});</script>";
    content += "<style type='text/css'>  *{font-family:'Helvetica','Verdana'; margin: 0 auto;} body{background-color: #E3AFAF;} li{list-style: none; padding: 3%; background-color: #C17C7C; color: #FFF4F4; margin-right: 10%; font-family: 'Helvetica', 'Verdana';}  ul { padding-left: : 8%; width: 80%; margin-top: 7%;} header { background-color: green; } h1 { background-color: #C17C7C; text-align: center;} form {background-color: #E3AFAF; padding-left: 8%} input { width: 80%; margin: 0 auto; border-style: none; border: 1px solid black; padding: 3%; background-color: #C17C7C; color: #FFF4F4; margin-bottom: 3%;font-family:'Helvetica','Verdana';font-size: 15px;} input[type=submit] {background-color:#C17C7C; width:87%;} small{color:#A15252;} label{color:#A15252;} img {width: 50%; margin-top: 5%;} hr{ color: #A15252; background-color: #A15252;} </style>";
    content += "<body><header><h1><img src='data:image/gif;base64,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' /></h1></header><section>";

    content += "<ul>";
    
    for (int i = 0; i < n; ++i) {

      content += "<li>" + WiFi.SSID(i) + "</li> <small>(" + WiFi.RSSI(i) + ")";
      content += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      content += "</small><br><br>";
      
    }
  
    content += "</ul>";
    content += "</section><br><hr><section><form method='post' action=''><br><label>SSID: </label><br><input name='ssid' id='ssid' length=32><br><label>PASS: </label><br><input name='pass' length=64><br><label>MQTT Server: </label><br><input name='mqtt_server' placeholder='xyz3.cloudmqtt.com' length=64><br><label>MQTT Port: </label><br><input name='mqtt_port' placeholder='1883' length=6><br><label>MQTT User: </label><br><input name='mqtt_user' length=32><br><label>MQTT Pass: </label><br><input name='mqtt_pass' length=32><br><label>MQTT Topic: </label><br><input name='mqtt_topic' placeholder='esp/home/bedroom' length=32><br><br><input type='submit' value='SAVE'><br><br><br></form></section></body></html>";
    
  }

  server.send(200, "text/html", content);
  
}

void startAP() {

    WiFi.softAP("TOTTEN", "10203040");
    server.on("/", handleRoot);
    server.begin();
 
}

void setup() {
  pinMode(RELAY, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(RELAY, 1);
  digitalWrite(LED, 1);
  
  Serial.begin(9600);
  EEPROM.begin(512);

  if(hasSSID() && hasPass() && testConnection(getSSID(), getPass())) {

    WiFi.mode(WIFI_STA);
    
    char bufServer[getMQTTServer().length()+1];
    getMQTTServer().toCharArray(bufServer, getMQTTServer().length()+1);
    char *mqttServer = bufServer;

    char bufUser[getMQTTUser().length()+1];
    getMQTTUser().toCharArray(bufUser, getMQTTUser().length()+1);
    char *mqttUser = bufUser;

    char bufPass[getMQTTPass().length()+1];
    getMQTTPass().toCharArray(bufPass, getMQTTPass().length()+1);
    char *mqttPassword = bufPass;
    
    int mqttPort = getMQTTPort().toInt();
   
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callbackMQTT);

    while (!client.connected()) {
      Serial.println("Connecting to MQTT...");
   
      if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
        blinkLed(25,40);
        Serial.println("connected");
      } else {
   
        Serial.print("failed with state: ");
        Serial.println(client.state());
        delay(2000);
   
      }
      
    }

    client.subscribe("esp/casa/sala");
    
  } else {
    startAP();
  }
  
}

void loop() {

  client.loop();
  delay(500);

  if (Serial.available()) {
    char c = Serial.read();
    
    if (serialRead == "reset") {
      for (int i = 0; i < 172; ++i) { EEPROM.write(i, 0); } EEPROM.commit();
      Serial.println("EEPROM memory reseted.");
      serialRead="";
      
    } else {      
      serialRead += c;
    }

  }

  server.handleClient();
  
}

