#include <Arduino.h>
#include <flash_hal.h>


#include "WebOTAForm.h"

#ifdef ESP32
	#include <WebServer.h>
	#include <Update.h>
#endif

#ifdef ESP8266
	#include <ESP8266WebServer.h>
#endif

class WebOTA {
	public:
		
		String path = "";

		#ifdef ESP8266
			uint8_t init( ESP8266WebServer *server, const char *path);
		#endif
		#ifdef ESP32
			uint8_t init( WebServer *server, const char *path);
		#endif

		void delay(int ms);
		uint8_t add_http_routes( const char *path );
		int handle();
	private:
		bool init_has_run;
		unsigned int port;

		String get_ota_html();
		long max_sketch_size();

		#ifdef ESP8266
			ESP8266WebServer *server;
		#endif
		#ifdef ESP32
			WebServer *server
		#endif
};

// int init_wifi(const char *ssid, const char *password, const char *mdns_hostname);

extern WebOTA webota;