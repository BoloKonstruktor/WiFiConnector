#ifndef HTML_H
#define HTML_H
#include "Arduino.h"

#ifdef ESP32
String LoginHead = PSTR(
#else
String LoginHead = (	
#endif
  "<html lang='pl'>"
  "<head>"
  "<meta charset='utf-8'>"
  "<title>Panel logowania</title>"
  "<style>"
  "body{"
  "margin:0;"
  "padding:0;"
  "text-align:center;"
  "font-family:Arial;"
  "}"
  ".login_panel{"
  "width:420px;"
  "margin:auto;"
  "margin-top:100px;"
  "padding:20px;"
  "border-radius:10px;"
  "background-color:#E0EFF0;"
  "}"
  ".login_panel form{"
  "margin:0;"
  "padding:0;"
  "}"
  ".login_panel input,.login_panel select,.login_panel button{"
  "padding:6px;"
  "font-size:18px;"
  "}"
  ".login_panel input,.login_panel select{"
  "width:320px;"
  "}"
  ".login_panel button{"
  "width:180px;"
  "font-size:22px;"
  "border-radius:6px;"
  "}"
  "</style>"
  "</head>"
  "<body>"
  "<div class='login_panel'>"
);
#ifdef ESP32
String LoginFoot = PSTR(
#else
String LoginFoot = (	
#endif
  "</div>"
  "</body>"
  "</html>"
);
#ifdef ESP32
String LoginSelect = PSTR(
#else
String LoginSelect = (	
#endif
	"<select name='ssid'>"
	"%s"  
	"</select>"
);
#ifdef ESP32
String LoginFormTop = PSTR(
#else
String LoginFormTop = (	
#endif
    "<form method='POST' action='login'>"
	"<p>"
	"<label for='ssid'>SSID:<label>"
	"<select name='ssid'>"
); 
#ifdef ESP32
String LoginFormBottom = PSTR(
#else
String LoginFormBottom = (	
#endif
	"</select>"
    "</p>"
    "<p>"
    "<label for='pass'>Hasło:<label>"
    "<input type='password' name='pass' minlength='8' required >"
    "</p>"
    "<p>"
    "<button type='submit' name='login'>OK</button>"
    "</p>"
    "</form>"
);
#endif