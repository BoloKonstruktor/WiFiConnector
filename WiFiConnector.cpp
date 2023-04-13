#include "WiFiConnector.h"
#include "HTML.h"
#include "EEPROM.h"

bool login = false;

const TWIFI defwifi = { "", "" };
TWIFI wifi;
WIFIParam wifip;

WiFiConnector* WiFiConnector::int_inst = NULL;
uint8_t netcnt = 0;


//Metody publiczne
void WiFiConnector::begin( unsigned& addr, WebServer* server, uint16_t port ){
	this->int_inst = this;
	memset( wifip.ssid, 0, sizeof( wifip.ssid ) );
	memset( wifip.ip, 0, sizeof( wifip.ip ) );
	this->load( addr, 0 );
	strcpy( wifip.ssid, wifi.ssid );
	String ssid = wifi.ssid;
	
		if( this->callback ) this->callback( WC_WIFI_SCAN, &wifip );
		
	netcnt = WiFi.scanNetworks();
	bool found = false;
	
		for( unsigned i = 0; i < netcnt; i++ ){
			String assid = WiFi.SSID(i);

				if( assid == ssid ){
				  found = true; 
				}			
		}
		
		if( found ){
			WiFi.mode( WIFI_STA );
			
				if( this->staname != "" ) WiFi.hostname( this->staname.c_str() );
					
			WiFi.begin( wifi.ssid, wifi.pass );
			
				if( this->callback ) this->callback( WC_STA_START, &wifip );
				
			uint8_t s = 0;

				while( WiFi.status() != WL_CONNECTED && s < this->timeout ){
						
						if( this->callback ) this->callback( WC_STA_CONNECTING, &wifip );
					
					delay( 1000 );
					
						if( this->timeout ) s++;
				}
				
				if( s >= this->timeout ){
					
						if( this->callback ) this->callback( WC_STA_TIMEOUT, &wifip );
					
					this->reset();
					delay( 1000 );
					ESP.restart();
				}
				
			strcpy( wifip.ip, this->ip2str( WiFi.localIP() ).c_str() );
				
				if( this->callback ) this->callback( WC_STA_CONNECTED, &wifip );
		
		} else {
			
				if( server ) this->server = server;
				else {
					this->server = new WebServer( port );
					this->server->begin();
				}
				
			WiFi.mode( WIFI_AP );
			WiFi.softAP( this->apname.c_str() );
			strcpy( wifip.ssid, this->apname.c_str() );
			strcpy( wifip.ip, this->ip2str( WiFi.softAPIP() ).c_str() );
				
				if( this->callback ) this->callback( WC_AP_START, &wifip );
						
			this->server->on("/", []() {
				String Options = "";
		
					for( unsigned i = 0; i < netcnt; i++ ){
						String ssid = WiFi.SSID(i);
						Options += "<option value='"+ssid+"'>"+ssid+"</option>";
					}

				String HTML = LoginHead+LoginFormTop+Options+LoginFormBottom+LoginFoot;
				int_inst->server->send( 200, "text/html", HTML );
				});
			
			this->server->on("/login", []() {
			String ssid = "";
			String pass = "";
					
					if( int_inst->server->method() == HTTP_POST ){
						ssid = int_inst->server->arg("ssid");
						pass = int_inst->server->arg("pass");
						strcpy( wifip.ssid, ssid.c_str() );
						strcpy( wifi.ssid, ssid.c_str() );
						strcpy( wifi.pass, pass.c_str() );
						int_inst->save();  
					}
					
					if( ssid == "" && pass == "" ) {
					  int_inst->server->sendHeader( "Location", "/", true );
					  int_inst->server->send( 302, "text/plain", "" );
					  int_inst->server->client().stop();						
					}
        
				String HTML = LoginHead;
				HTML += PSTR("<h2>A zatem znam już hasło do sieci: '");
				HTML += wifi.ssid;
				HTML += PSTR("'</h2>");
				HTML += PSTR("<h1>Dziękuje ;)</h1>");
				HTML += LoginFoot;
				int_inst->server->send( 200, "text/html", HTML );
				login = true;
				});
			
			this->server->begin();

				while( 1 ){
					this->server->handleClient();

						if( login ){
							delay( 1000 );
							ESP.restart();
						}

					delay( 1 );  
				}
				
		}
}

void WiFiConnector::registerCallback( void(*callback)( uint8_t, WIFIParam* ) ){
	this->callback = callback;
}

void WiFiConnector::reset( void ){
	unsigned addr = this->eep_addr;
	this->load( addr, 1 );
}

void WiFiConnector::setTimeout( uint8_t s ){
	this->timeout = s;
}

void WiFiConnector::setAPName( const char* n ){
	this->apname = n;
}

void WiFiConnector::setSTAName( const char* n ){
	this->staname = n;
}


//Metody prywatne
void WiFiConnector::load( unsigned& addr, bool def ){
		
		if( addr == 0 ){
#ifdef ESP32			
				if( !EEPROM.begin( 1000 ) ) {
				  delay( 1000 );
				  ESP.restart();
				}
#else
			EEPROM.begin( 512 );
#endif
		}
	
	this->eep_addr = addr;
	this->load_param( addr, wifi, defwifi, def );
}

void WiFiConnector::save( void ){
	unsigned addr = this->eep_addr;
	this->save_param( addr, wifi );
}

unsigned WiFiConnector::eep_read( unsigned offset, void* data, uint16_t size ){
	uint8_t* buff = (uint8_t*)data;
  
		for ( uint16_t i = 0; i < size; i++ ){
#ifdef ESP32
			buff[i] = EEPROM.readUChar( offset );
#else
			buff[i] = EEPROM.read( offset );
#endif
			offset++;
  
				if ( offset == EEPROM.length() ) break;
      }
   
	return offset;
}
  
unsigned WiFiConnector::eep_write( unsigned offset, void* data, uint16_t size ){
	const uint8_t* buff = (uint8_t*)data;
      
		for ( uint16_t i = 0; i < size; i++ ){
#ifdef ESP32
			EEPROM.writeUChar( offset, buff[i] );
#else
			EEPROM.write( offset, buff[i] );
#endif
			offset++;
  
				if ( offset == EEPROM.length() ) break;
		}
  
    EEPROM.commit();
    return offset+size;
}
  
bool WiFiConnector::eep_empty( void* data, uint16_t size ){
    const uint8_t* buff = (uint8_t*)data;
  
		for( uint16_t i = 0; i < size; i++ ){
            
				if( buff[i] != 0xFF ) return false;
		}
  
    return true;
}