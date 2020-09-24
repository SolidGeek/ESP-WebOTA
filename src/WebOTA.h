#include <Arduino.h>
#include <flash_hal.h>

#define UPDATE_ERROR_OK                 (0)
#define UPDATE_ERROR_WRITE              (1)
#define UPDATE_ERROR_ERASE              (2)
#define UPDATE_ERROR_READ               (3)
#define UPDATE_ERROR_SPACE              (4)
#define UPDATE_ERROR_SIZE               (5)
#define UPDATE_ERROR_STREAM             (6)
#define UPDATE_ERROR_MD5                (7)
#define UPDATE_ERROR_FLASH_CONFIG       (8)
#define UPDATE_ERROR_NEW_FLASH_CONFIG   (9)
#define UPDATE_ERROR_MAGIC_BYTE         (10)
#define UPDATE_ERROR_BOOTSTRAP          (11)
#define UPDATE_ERROR_SIGN               (12)

#include "WebOTAForm.h"
#include "pgmspace.h"

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
			uint8_t init( ESP8266WebServer *server, const char *path, int ledpin = -1 );
		#endif
		#ifdef ESP32
			uint8_t init( WebServer *server, const char *path, int ledpin = -1 );
		#endif

		void delay(int ms);
		uint8_t add_http_routes( const char *path );

	private:

        int ledpin;
		bool init_has_run;

		long max_sketch_size();
		void send_http_error( uint8_t error );

		#ifdef ESP8266
			ESP8266WebServer *server;
		#endif
		#ifdef ESP32
			WebServer *server
		#endif
};

// int init_wifi(const char *ssid, const char *password, const char *mdns_hostname);

extern WebOTA webota;