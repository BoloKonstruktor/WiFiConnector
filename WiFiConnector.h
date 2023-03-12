#ifndef WIFICONNECTOR_H
#define WIFICONNECTOR_H
#include <WiFiClient.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#define WebServer	ESP8266WebServer
#define AP_NAME		"ESP8266_AP"
#else
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#define AP_NAME		"ESP32_AP"
#endif

typedef struct {
	char ssid[65];
	char pass[33];
}TWIFI;

typedef struct {
	char ssid[65];
	char ip[12];
}WIFIParam;

//Sygnatury zdarzeń
enum{
	WC_WIFI_SCAN,
	WC_STA_START,
	WC_STA_CONNECTING,
	WC_STA_CONNECTED,
	WC_STA_TIMEOUT,
	WC_AP_START
};

class WiFiConnector {
	
	private:
		uint8_t timeout = 15;
		unsigned eep_addr = 0;
		String apname = AP_NAME, staname = "";
		WebServer* server = NULL;
		static WiFiConnector* int_inst;
		void(*callback)( uint8_t, WIFIParam* ) = NULL;
		
		void load( unsigned& addr, bool def=false );
		void save( void );
		String ip2str( const IPAddress& ip ){
			return String( ip[0]) + String(".") +\
				String(ip[1]) + String(".") +\
				String(ip[2]) + String(".") +\
				String(ip[3]); 
		}
		unsigned eep_read( unsigned, void*, uint16_t );
		unsigned eep_write( unsigned, void*, uint16_t );
		bool eep_empty( void*, uint16_t );
		template<typename D1, typename D2> void load_param( unsigned& addr, D1& data, D2 def, bool loaddef = false ){
			size_t size = sizeof( data );
			uint16_t _addr = this->eep_read( addr, &data, size );
		  
			  if( this->eep_empty( &data, size ) || loaddef ){
				memcpy( &data, &def, size );
				this->eep_write( addr, &data, size );
			  }
		  
			addr = _addr;
		} 
		template<typename D> void save_param( unsigned& addr, D data ){
			size_t size = sizeof( data );
			this->eep_write( addr, &data, size );
			addr = addr+size;
		}
	
	public:
	/*
	Inicjowanie klasy. Argumenty to adres do przydzielenia
	pamięci EEPROM przekazany po przez referencje, wskaźnik
	klasy WebServer - domyślnie NULL. Trzeci argument to port
	serwera HTTP - domyślnie 80.
	*/
	void begin( unsigned& addr, WebServer* server=NULL, uint16_t port=80 );
	
	/*
	Rejestrowanie funkcji wywoływanej dla zdarzeń.
	Argumenty to sygnatura zdarzenia oraz wskaźnik do struktury
	w której znajdują się pola: ssid oraz ip.
	Metodę należy wywołać przed wywołaniem metody: begin.
	Ardument jest typu uint8_t więc do przechwytywania zdarzeń
	można użyć switch-a.
	*/
	void registerCallback( void(*callback)( uint8_t, WIFIParam* ) );
	
	/*
	Czyszczenie danych połączenia z siecią WiFi (SSID i hasło)
	oraz formatowanie przydzielonej pamięci EEPROM. 
	*/
	void reset( void );
	
	/*
	Ustawienie limitu czasu na połączenie z siecią WiFi
	Argument to czas w sekundach - domyślnie: 15 (0 wyłącza limit).
	Metodę należy wywołać przed wywołaniem metody: begin.
	*/
	void setTimeout( uint8_t s );
	
	/*
	Ustawienie nazwy dla sieci WiFi w trybie AP
	Metodę należy wywołać przed wywołaniem metody: begin.
	*/
	void setAPName( const char* n );
	
	/*
	Ustawienie nazwy dla sieci WiFi w trybie STA
	Metodę należy wywołać przed wywołaniem metody: begin.
	*/
	void setSTAName( const char* n );
								
};
#endif