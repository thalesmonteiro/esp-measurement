//Bibliotecas
#include <HTTPClient.h>
#include <DNSServer.h> 
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiManager.h> //https://github.com/khoih-prog/ESP_WiFiManager
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>

#define ANALOG_PIN 36
int analog_value = 2;
int quantLeituraAD = 32;

WiFiManager wifiManager; //Objeto de manipulação do wi-fi
WebServer server(80); //Objeto de manipulação do server
HTTPClient http; 

//callback que indica que o ESP entrou no modo AP
void configModeCallback (WiFiManager *myWiFiManager) {  
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); //imprime o SSID criado da rede
}
 
//Callback que indica que salvamos uma nova rede para se conectar (modo estação)
void saveConfigCallback () {
  Serial.println("Configuração salva");
}

void handleRoot() {
  server.send(200,"text/html", "<html><h1>Voc&ecirc est&aacute conectado!</h1></html>");
}
void handleQualquer(){
    server.send(200,"text/html", "<html><h1>NOT IMPLEMENTED!</h1></html>");

}

int realizaLeituraAD(){
  int valorLido = 0;

  for(int i = 0; i < quantLeituraAD; i++) {
    analog_value = analogRead(ANALOG_PIN);
    Serial.println(analog_value);
    valorLido = valorLido + analog_value;
    delay(10);
  }
  
  return valorLido / quantLeituraAD;
}
void respondeRequest(){
  http.begin("http://jsonplaceholder.typicode.com/posts"); //Specify destination for HTTP request
  http.addHeader("Content-Type", "text/plain"); //Specify content-type header
  int httpResponseCode = http.POST("POSTING from ESP32"); //Send the actual POST request

  if(httpResponseCode>0){
    
      String response = http.getString();  //Get the response to the request
    
      Serial.println(httpResponseCode);   //Print return code
      Serial.println(response);           //Print request answer
    
  }else{
    
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    

  }
}

void handlerMeasure(){
    String code;
    String leitura;
    //Leitura dos argumentos da requisicao
    code = server.arg("code");
    Serial.printf("Has args: %d",server.args());
    Serial.print("code: ");
    Serial.println(code);
    
    switch(code.toInt()){
      //Solicita medição do sensor
      case 84:
        Serial.println(" caiu no  84");
        int medicao;
        medicao = realizaLeituraAD();
        char text[256];
        sprintf(text, "%d", medicao);
        Serial.printf("VALOR: %d", medicao);
        server.send(200, "text/plain", text);
        
        break;

      //Verifica se o dispositivo esta operacional
      case 5:
        Serial.println("caiu no 5");
        server.send(200, "text/plain", "message: Dispositivo operacional");
        break;

      //Reinicia dispositivo
      case 82:
        Serial.println(" caiu no  82");
        Serial.println("Reiniciando ESP");
  
        server.send(200, "text/plain", "message: Reiniciando o ESP");
        ESP.restart();
        break;

      //Demais Instruções
      default:
        Serial.println("message: Instrução L3 não implementada");
        server.send(404);
    }
}

void setup() {
  delay(1500);
  Serial.begin(115200);
  //declaração do objeto wifiManager
  WiFiManager wifiManager;

  //utilizando esse comando, as configurações são apagadas da memória
  //caso tiver salvo alguma rede para conectar automaticamente, ela é apagada.
  //wifiManager.resetSettings();

  //callback para quando entra em modo de configuração AP
  wifiManager.setAPCallback(configModeCallback); 
  //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //cria uma rede de nome ESP_AP com senha 12345678
  wifiManager.autoConnect("ESP_AP", "12345678"); 

  if(WiFi.status() == WL_CONNECTED){
    Serial.println("CONECTADO");
    Serial.println(WiFi.softAPIP()); //imprime o IP do AP
    Serial.println(wifiManager.getConfigPortalSSID()); //imprime o SSID criado da rede
  }

  server.on("/", handleRoot);
  server.on("/measure", handlerMeasure);
  server.onNotFound(handleQualquer);
  server.begin();
  Serial.println("Server iniciado!");
}
 
void loop() 
{ 
  server.handleClient();

  //analog_value = analogRead(ANALOG_PIN);
  //Serial.println(analog_value);

  //delay(500);

}
