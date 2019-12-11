// R Macro string literal https://en.cppreference.com/w/cpp/language/string_literal
const char ota_html[] PROGMEM = "<h1>WebOTA Version: " WEBOTA_VERSION "</h1>"
R"!^!(

<form method="POST" action="#" enctype="multipart/form-data" id="upload_form">
    <input type="file" name="update" id="file">
    <input type="submit" value="Update">
</form>

<div id="prg_wrap" style="border: 0px solid; width: 100%;">
   <div id="prg" style="text-shadow: 2px 2px 3px black; padding: 5px 0; display: none; border: 1px solid #008aff; background: #002180; text-align: center; color: white;"></div>
</div>

<script>
var domReady = function(callback) {
	document.readyState === "interactive" || document.readyState === "complete" ? callback() : document.addEventListener("DOMContentLoaded", callback);
};

domReady(function() {
	var myform = document.getElementById('upload_form');
	var filez  = document.getElementById('file');

	myform.onsubmit = function(event) {
		event.preventDefault();

		var formData = new FormData();
		var file     = filez.files[0];

		if (!file) { return false; }

		formData.append("files", file, file.name);

		var xhr = new XMLHttpRequest();
		xhr.upload.addEventListener("progress", function(evt) {
			if (evt.lengthComputable) {
				var per = Math.round((evt.loaded / evt.total) * 100);
				var prg = document.getElementById('prg');

				prg.innerHTML     = per + "%"
				prg.style.width   = per + "%"
				prg.style.display = "block"
			}
		}, false);
		xhr.open('POST', location.href, true);

		// Set up a handler for when the request finishes.
		xhr.onload = function () {
			if (xhr.status === 200) {
				//alert('Success');
			} else {
				//alert('An error occurred!');
			}
		};

		xhr.send(formData);
   }
});
</script>)!^!";