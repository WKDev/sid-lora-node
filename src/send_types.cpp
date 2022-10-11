//GET Querystring

//     String data = "GET /";
//   data += "?height="+String(123) + "&" + "d="+ String(456);
//   data += " HTTP/1.1\r\n";
//   data += "Host: 15.165.251.30\r\n";
//   data += "Connection: close\r\n\r\n";
  
//POST json

    //String jsondoc = "{\"foo\":\"bar\"}"
    // String PostData = "POST / HTTP/1.1\r\n";
  // PostData += "Host: 15.165.251.30\r\n";
  // PostData += "Accept: */*\r\n";
  // PostData += "Connection: close\r\n\r\n";
  // PostData += "User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n";
  // PostData += "Content-Type: application/json\r\n";
  // PostData += "Content-Length: "+String(jsondoc.length()) + "\r\n";
  // PostData += jsondoc;

//POST using ArduinoJson
  // // JSON
  // String output;
  // StaticJsonDocument<200> doc;
  // doc["device"] = 1;
  // doc["msg"] = "data from TYPE1SC";
  // serializeJson(doc, output);
  // Serial.println(output);