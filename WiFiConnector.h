#ifndef WIFICONNECTOR_H
#define WIFICONNECTOR_H
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

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
								
};
#endif