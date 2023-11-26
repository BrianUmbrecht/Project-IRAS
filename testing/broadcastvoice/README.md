Install node.js if it isn't or you have an old version.
Change wifi ssid and password, and websocket_server_host in ESP32-S3_INMP441_WebSocket_Client.ino. Set websocket_server_host to your wifi IP address.
In audio_client.html, on line 103, change the IP address to your wifi IP address.
You can get your wifi IP address by typing ipconfig in command prompt or checking wifi properties.
To run the program, start the server by navigating to the wsAudioServer, and entering the commands npm install and node server.js.
Then power the ESP32, and search http://localhost:8000/audio in a web browser.
Click connect and the graph should start changing and you should hear the ESP32 microphone output.
