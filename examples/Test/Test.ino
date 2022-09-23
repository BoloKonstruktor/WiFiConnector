#include <WiFiConnector.h>



WebServer server( 80 );

void WCEvents( uint8_t event, WIFIParam* wifi );

void handleRoot( void );
void handleNotFound( void );


void setup(void) {
  Serial.begin( 115200 );
  unsigned addr = 0;
  
  WiFiConnector* wc = new WiFiConnector;
  wc->registerCallback( WCEvents );
  wc->begin( addr, &server );
  delete wc;
  wc = NULL;

  server.on( "/", handleRoot );
  server.onNotFound( handleNotFound );
  server.begin();
}

void loop(void) {
  server.handleClient();
  delay(2);
}

void WCEvents( uint8_t event, WIFIParam* wifi ){

    switch( event ){
      case WC_WIFI_SCAN:{
        Serial.println( F("Wyszukiwanie sieci ...") );  
      }break;
      case WC_STA_START:{
        Serial.print( F("Łącze z siecią: ") );
        Serial.print( wifi->ssid );
        Serial.print( F(" ") );   
      }break;
      case WC_STA_CONNECTING:{
        Serial.print( F(".") );
      }break;
      case WC_STA_CONNECTED:{
        Serial.println();
        Serial.println( F("Połączony.") );
        Serial.print( F("IP: ") );
        Serial.println( wifi->ip );
      }break;
      case WC_STA_TIMEOUT:{
        Serial.println( F("Czas na połączenie z siecią: '") );
        Serial.print( wifi->ssid );
        Serial.println( F("' minął.") );
      }break;
      case WC_AP_START:{
        Serial.println( F("Uruchomiono punkt dostępowy.") );
        Serial.print( F("SSID: ") );
        Serial.println( wifi->ssid );
        Serial.print( F("IP: ") );
        Serial.println( wifi->ip );
      }break;
    }
}

void handleRoot() {
  server.send( 200, "text/plain", "hello from esp32!" );
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    
  server.send( 404, "text/plain", message );
}