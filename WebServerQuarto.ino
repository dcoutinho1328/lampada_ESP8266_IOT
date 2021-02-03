#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <StreamString.h>
#include <ESP8266WebServer.h>
#include <FS.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

#define MyApiKey "----COPIE E COLE AQUI A API KEY DA SINRIC--" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define ssid "--INSIRA AQUI O NOME DA REDE--" // TODO: Change to your Wifi network SSID
#define password "--INSIRA AQUI A SENHA DA REDE--" // TODO: Change to your Wifi network password
#define HEARTBEAT_INTERVAL 300000

IPAddress ip(--CRIE UM ENDEREÇO IP DENTRO DA FAIXA DE IP DISTRIBUIDO PELO SEU ROTEADOR--);
IPAddress gateway(--COLOQUE O ENDERECO IP LAN DO ROTEADOR--);
IPAddress subnet(255,255,255,0);

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

int state = false;

ESP8266WebServer server(80); //Pode ser alterado para um número aleatório. Caso altere, basta informar no IP. Ex: 192.168.0.25:2587 (caso você opte por utilizar 2587, por exemplo) 

void handleRoot(){
  server.sendHeader("Location", "/index.html", true);   //Redirect to our html web page
  server.send(302, "text/plane","");
  Serial.println("Aqui passou");
}

void ToggleLamp(){
  server.sendHeader("Location", "/index.html", true);   //Redirect to our html web page
  server.send(302, "text/plane","");
  state=!state;
  digitalWrite(5, state); 
  }
 
void handleWebRequests(){
  if(loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}
 
void turnOn(String deviceId) {
  if (deviceId == "--COPIE E COLE AQUI O DEVICE ID DA SINRIC--") // Device ID of first device
  { 
    state = !state;
    digitalWrite(5, state);    
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
  } 
  else {
    Serial.print("Turn on for unknown device id: ");
    Serial.println(deviceId);    
  }     
}

void turnOff(String deviceId) {
   if (deviceId == "--COPIE E COLE AQUI O DEVICE ID DA SINRIC--") // Device ID of first device
   {  
     state = !state;
     digitalWrite(5, state);     
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
   }   
  else {
     Serial.print("Turn off for unknown device id: ");
     Serial.println(deviceId);    
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
        // Example payloads

        // For Switch  types
        // {"deviceId":"xxx","action":"action.devices.commands.OnOff","value":{"on":true}} // https://developers.google.com/actions/smarthome/traits/onoff
        // {"deviceId":"xxx","action":"action.devices.commands.OnOff","value":{"on":false}}

#if ARDUINOJSON_VERSION_MAJOR == 5
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6        
        DynamicJsonDocument json(1024);
        deserializeJson(json, (char*) payload);      
#endif        
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "action.devices.commands.OnOff") { // Essa parte funciona com Google Assistant  
            String value = json ["value"]["on"];
            Serial.println(value); 
            
            if(value == "true") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }
        }
        else if(action == "setPowerState"){ //Essa parte funciona com a Alexa
          String value = json["value"];
          Serial.println(value);

          if (value == "ON"){
            turnOn(deviceId);
          }
          else{
            turnOff(deviceId);
          }
          
        }
        else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
    default: break;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT); //AQUI PODE SER UTILIZADO O PINO GPIO QUE VOCÊ ESCOLHER 
  digitalWrite(5, state);  
  delay(1000);
  Serial.begin(9600);
  Serial.println();
 
  //Initialize File System
  SPIFFS.begin();
  Serial.println("File System Initialized");
 
  //Initialize Wifi Connection Mode  
  Serial.println(""); //PULA UMA LINHA NA JANELA SERIAL
  Serial.println(""); //PULA UMA LINHA NA JANELA SERIAL
  Serial.print("Conectando a "); //ESCREVE O TEXTO NA SERIAL
  Serial.print(ssid); //ESCREVE O NOME DA REDE NA SERIAL 
  WiFi.begin(ssid, password); //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI FAZER A CONEXÃO COM A REDE SEM FIO 
  while (WiFi.status() != WL_CONNECTED) { //ENQUANTO STATUS FOR DIFERENTE DE CONECTADO
  delay(500); //INTERVALO DE 500 MILISEGUNDOS
  Serial.print("."); //ESCREVE O CARACTER NA SERIAL
  }
  Serial.println(""); //PULA UMA LINHA NA JANELA SERIAL
  Serial.print("Conectado a rede sem fio "); //ESCREVE O TEXTO NA SERIAL
  Serial.println(ssid);  
  Serial.print("Web Server IP:");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
 
  //Initialize Webserver
  server.on("/",handleRoot);
  server.on("/TOG", ToggleLamp);
  server.onNotFound(handleWebRequests); //Set setver all paths are not found so we can handle as per URI
  server.begin();

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/"); //"iot.sinric.com", 80

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}

void loop() {
  server.handleClient();
  webSocket.loop();
  
  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  }   
}

bool loadFromSpiffs(String path){
  String dataType = "text/plain";
  if(path.endsWith("/")) path += "index.htm";
 
  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".html")) dataType = "text/html";
  else if(path.endsWith(".htm")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }
 
  dataFile.close();
  return true;
}
