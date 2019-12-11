// Arduino build process info: https://github.com/arduino/Arduino/wiki/Build-Process

#define WEBOTA_VERSION "0.1.5"

#include "WebOTA.h"


WebOTA webota;

////////////////////////////////////////////////////////////////////////////

#ifdef ESP32
uint8_t WebOTA::init( ESP8266WebServer *server, const char *path)
#endif
#ifdef ESP8266
uint8_t WebOTA::init( WebServer *server, const char *path)
#endif
{
	this->path = path;
	this->server = server;

	// Only run this once
	if (this->init_has_run) {
		return 0;
	}

	add_http_routes(path);

	// Store that init has already run
	this->init_has_run = true;

	return 1;
}

long WebOTA::max_sketch_size() {
	long ret = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;

	return ret;
}

uint8_t WebOTA::add_http_routes( const char *path ) {

	// Index page
	server->on("/", HTTP_GET, [server]() {
		server->send(200, "text/html", "<h1>WebOTA</h1>");
	});

	// Upload firmware page
	server->on(path, HTTP_GET, [server,this]() {
		server->send_P(200, "text/html", ota_html);
	});

	// Handling uploading firmware file
	server->on(path, HTTP_POST, [server,this]() {
		server->send(200, "text/plain", (Update.hasError()) ? "Update: fail\n" : "Update: OK!\n");
		delay(500);
		ESP.restart();
	}, [server,this]() {
		HTTPUpload& upload = server->upload();

		if (upload.status == UPLOAD_FILE_START) {
			Serial.printf("Firmware update initiated: %s\r\n", upload.filename.c_str());

			//uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
			uint32_t maxSketchSpace = this->max_sketch_size();

			if (!Update.begin(maxSketchSpace)) { //start with max available size
				Update.printError(Serial);
			}
		} else if (upload.status == UPLOAD_FILE_WRITE) {
			/* flashing firmware to ESP*/
			if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
				Update.printError(Serial);
			}

			// Store the next milestone to output
			uint16_t chunk_size  = 51200;
			static uint32_t next = 51200;

			// Check if we need to output a milestone (100k 200k 300k)
			if (upload.totalSize >= next) {
				Serial.printf("%dk ", next / 1024);
				next += chunk_size;
			}
		} else if (upload.status == UPLOAD_FILE_END) {
			if (Update.end(true)) { //true to set the size to the current progress
				Serial.printf("\r\nFirmware update successful: %u bytes\r\nRebooting...\r\n", upload.totalSize);
			} else {
				Update.printError(Serial);
			}
		}
	});

	server->begin();

	return 1;
}

// If the MCU is in a delay() it cannot respond to HTTP OTA requests
// We do a "fake" looping delay and listen for incoming HTTP requests while waiting
void WebOTA::delay(int ms) {
	int last = millis();

	while ((millis() - last) < ms) {
		OTAServer.handleClient();
		this->delay(5);
	}
}
