// R Macro string literal https://en.cppreference.com/w/cpp/language/string_literal
const char ota_html[] PROGMEM = 
R"(<!DOCTYPE html>
<html>
	<head>
		<title>WebOTA Firmware Update</title>
	</head>
	<body style="background:#2a2c30; color: #fff; text-align: center; font-family: sans-serif;">
		<h1>Update Wifi module</h1>

		<div style="margin: 0 auto; background: #222427; width: 400px; padding: 5px 20px; border-radius: 5px; text-align: left;">
			<h3>Firmware</h3>
			<p>Select file ending with: .ino.bin</p>
			<form method='POST' action='' id="firmware_form" enctype='multipart/form-data'>
				<input type='file' class='file' accept='.bin' name='firmware'>
				<input type='submit' value='Update Firmware'>
			</form>

			<h3>Application</h3>
			<p>Select file ending with: .spiffs.bin</p>
			<form method='POST' action='' id="filesystem_form" enctype='multipart/form-data'>
				<input type='file' class='file' accept='.bin' name='filesystem'>
				<input type='submit' value='Update FileSystem'>
			</form>

			<br>

			<div id="status" style="display:none; padding:15px 20px; text-align:center; border-top: 1px solid #2a2c30; margin: 0 -20px;">
				<div id="response" style="font-size:20px;"></div>
				<div id="progress" style="margin-top: 10px; border-radius: 5px;padding: 8px 0px;font-weight: bold;background: #5091e2;text-align: center;"></div>
			</div>
		</div>

		<script>
		var domReady = function(callback) {
			document.readyState === "interactive" || document.readyState === "complete" ? callback() : document.addEventListener("DOMContentLoaded", callback);
		};

		domReady(function() {

			var progress = document.getElementById('progress');
			var status = document.getElementById('status');
			var response = document.getElementById('response');
			var fwForm = document.getElementById('firmware_form');
			var fsForm = document.getElementById('filesystem_form');

			fwForm.onsubmit = function(event){
				event.preventDefault();
				var file = event.currentTarget.getElementsByClassName('file')[0];
				uploadFile( file.files[0], file.getAttribute('name') );
			}

			fsForm.onsubmit = function(event){
				event.preventDefault();
				var file = event.currentTarget.getElementsByClassName('file')[0];
				uploadFile( file.files[0], file.getAttribute('name') );
			}

			function uploadFile(file, type){
				
				var formData = new FormData();

				if (!file) { return false; }

				formData.append(type, file, file.name);

				var request = new XMLHttpRequest();

				request.upload.addEventListener('progress', function(evt) {
					if (evt.lengthComputable) {
						var per = Math.round((evt.loaded / evt.total) * 100);
						
						status.style.display = "block";
						response.innerHTML = "Updating...";

						progress.innerHTML     = per + "%"
						progress.style.width   = per + "%"
					}
				});

				request.onreadystatechange = function() {
					if (request.status === 200) {
						response.innerHTML = "Update successful<br><small>The WiFi-module will restart, please reconnect.</small>";
					} else {
						response.innerHTML = "Update failed<br><small>"+request.responseText+"</small>";
					}
				}

				request.open('POST', location.href, true);
				request.send(formData);
			}
		});
		</script>
	</body>
</html>)";