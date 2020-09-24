// Arduino build process info: https://github.com/arduino/Arduino/wiki/Build-Process

#define WEBOTA_VERSION "0.1.5"

#include "WebOTA.h"


WebOTA webota;

////////////////////////////////////////////////////////////////////////////

#ifdef ESP8266
uint8_t WebOTA::init( ESP8266WebServer *server, const char *path, int ledpin) {
#endif
#ifdef ESP32
uint8_t WebOTA::init( WebServer *server, const char *path, int ledpin) {
#endif
	this->path = path;
	this->server = server;
	this->ledpin = ledpin;

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

	#ifdef ESP8266
	ESP8266WebServer *server = this->server;
	#endif
	#ifdef ESP32
	WebServer *server = this->server;
	#endif

	// Upload firmware page
	server->on(path, HTTP_GET, [server,this]() {
		server->send_P(200, "text/html", ota_html);
	});

	// Handling uploading firmware file
	server->on(path, HTTP_POST, [server,this]() {

		if( ! Update.hasError() ){
			server->send(200, "text/plain", "Update successful");

			delay(500);
			ESP.restart();
		}

	}, [server, this]() {

		// Only continue if no errors
		if( ! Update.hasError() ){

			HTTPUpload& upload = server->upload();

			if (upload.status == UPLOAD_FILE_START) {

				Serial.print("Firmware update initiated: ");
				Serial.println(upload.filename);

				if (upload.name == "filesystem") {
					size_t fsSize = ((size_t) &_FS_end - (size_t) &_FS_start);
					SPIFFS.end();

					// Begin the update process, and use ledpin to indicate the progress
					if (!Update.begin(fsSize, U_FS, this->ledpin)){ 
						Update.printError(Serial);
					}
		        } else if (upload.name == "firmware" ) {
					uint32_t maxSketchSpace = this->max_sketch_size();

					// Begin the update process, and use ledpin to indicate the progress
					if (!Update.begin(maxSketchSpace, U_FLASH, this->ledpin)) { 
						Update.printError(Serial);
					}
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
				// true to set the size to the current progress
				if (Update.end(true)) { 
					Serial.printf("\r\nFirmware update successful: %u bytes\r\nRebooting...\r\n", upload.totalSize);
				} else {
					Update.printError(Serial);
				}
			}

			// If any of the operations caused an error, print this error and break the operation.
			if( Update.hasError() ){
				this->send_http_error( Update.getError() );
				Update.end(true);
			}

		}
	});

	return 1;
}

void WebOTA::send_http_error( uint8_t error ){
	char buffer[50];

	if(error == UPDATE_ERROR_OK){
		strcpy_P(buffer, PSTR("No error"));
	} else if(error == UPDATE_ERROR_WRITE){
		strcpy_P(buffer, PSTR("Flash write failed"));
	} else if(error == UPDATE_ERROR_ERASE){
		strcpy_P(buffer, PSTR("Flash erase failed"));
	} else if(error == UPDATE_ERROR_READ){
		strcpy_P(buffer, PSTR("Flash read failed"));
	} else if(error == UPDATE_ERROR_SPACE){
		strcpy_P(buffer, PSTR("Not enough space in OTA"));
	} else if(error == UPDATE_ERROR_SIZE){
		strcpy_P(buffer, PSTR("Bad size given"));
	} else if(error == UPDATE_ERROR_STREAM){
		strcpy_P(buffer, PSTR("Stream read timeout"));
	} else if(error == UPDATE_ERROR_MD5){
		strcpy_P(buffer, PSTR("MD5 failed"));
	} else if(error == UPDATE_ERROR_SIGN){
		strcpy_P(buffer, PSTR("Signature verification failed"));
	} else if(error == UPDATE_ERROR_FLASH_CONFIG){
		sprintf_P(buffer, PSTR("Flash config wrong, real: %d IDE: %d\n"), ESP.getFlashChipRealSize(), ESP.getFlashChipSize());
	} else if(error == UPDATE_ERROR_NEW_FLASH_CONFIG){
		sprintf_P(buffer, PSTR("Flash config wrong, real: %d\n"), ESP.getFlashChipRealSize());
	} else if(error == UPDATE_ERROR_MAGIC_BYTE){
		strcpy_P(buffer, PSTR("Magic byte is wrong, not 0xE9"));
	} else if (error == UPDATE_ERROR_BOOTSTRAP){
		strcpy_P(buffer, PSTR("Invalid state, reset ESP8266 before updating"));
	} else {
		strcpy_P(buffer, PSTR("Unknown error"));
	}

	// 500 = Internal Server Error
	server->send(500, "text/plain", buffer);
}

// If the MCU is in a delay() it cannot respond to HTTP OTA requests
// We do a "fake" looping delay and listen for incoming HTTP requests while waiting
void WebOTA::delay(int ms) {
	int last = millis();

	while ((millis() - last) < ms) {
		server->handleClient();
		this->delay(5);
	}
}
