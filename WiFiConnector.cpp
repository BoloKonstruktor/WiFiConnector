#include "WiFiConnector.h"
#include "HTML.h"
#include "common.h"

bool login = false;

const TWIFI defwifi = { 0, 0 };
TWIFI wifi;
WIFIParam wifip;

WiFiConnector* WiFiConnector::int_inst = NULL;
uint8_t netcnt = 0;


//Metody publiczne
void WiFiConnector::begin( unsigned& addr, WebServer* server, uint16_t port ){
	this->int_inst = this;
	memset( wifip.ssid, 0, sizeof( wifip.ssid ) );
	memset( wifip.ip, 0, sizeof( wifip.ip ) );
	
		if( addr == 0 ){
			
				if( !EEPROM.begin( 1000 ) ) {
				  delay( 1000 );
				  ESP.restart();
				}
		}

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
			this->server = server ? server : new WebServer( port );
			WiFi.mode( WIFI_AP );
			WiFi.softAP( "ESP32_AP" );
			strcpy( wifip.ssid, "ESP32_AP" );
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
					
					if( int_inst->server->method() == HTTP_POST ){
						String ssid = int_inst->server->arg("ssid");
						String pass = int_inst->server->arg("pass");
						strcpy( wifip.ssid, ssid.c_str() );
						strcpy( wifi.ssid, ssid.c_str() );
						strcpy( wifi.pass, pass.c_str() );
						int_inst->save();  
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


//Metody prywatne
void WiFiConnector::load( unsigned& addr, bool def ){
	this->eep_addr = addr;
	load_param( addr, wifi, defwifi, def );
}

void WiFiConnector::save( void ){
	unsigned addr = this->eep_addr;
	save_param( addr, wifi );
}