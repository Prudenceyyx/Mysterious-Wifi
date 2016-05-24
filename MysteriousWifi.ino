//Progression:Webpage with textinput and submit button

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <SD.h>

char ssid[] = "nyusecret192.168.4.1";      //  your network SSID (name)
char pass[] = "";   // your network password

WiFiServer server(80);
File myFile;

void setup() {
  Serial.begin(115200);
  setSD();
  WiFi.mode(WIFI_AP); //Access Point mode
  WiFi.softAP(ssid);
  server.begin();
  IPAddress ip = WiFi.softAPIP();

  Serial.print("Attempting to create a Network named: ");
  Serial.println(ssid);
  Serial.print("AP IP is:");
  Serial.println(ip);
}

void setSD() {
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

}
void loop() {
  WiFiClient client = server.available();   // listen for incoming clients
  if (!client) return;
  else if (client) {                             // if you get a client,
    //    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";
    String req = client.readStringUntil('\r');

    if (req.indexOf("GET /") != -1) {             //analyze the GET message
      if (req.indexOf("GET /favicon.ico HTTP/1.1") == -1) {
        int s = req.indexOf("GET /");
        int e = req.lastIndexOf("HTTP");
        String m = req.substring(s + 5, e - 1);
        Serial.print("received:");
        Serial.println(m);
        writeto(myFile, m);                 //save the meesage to SD card
      }
    }
    client.flush();
    //cont includes the code for the webpage
    String cont = "HTTP/1.1 200 OK\r\n";
    cont += "Content-Type: text/html\r\n\r\n";
    cont += "<!DOCTYPE HTML>\r\n<html>\r\n";
    cont += "<title>NYU SECRET</title>";
    // Print out a message to the webpage saying we did
    cont += "<h1>This is a Treehole</h1>";
    cont += "<h1>Write down your secret!</h1>";
    //    cont += "<meta http-equiv=\"refresh\" content=\"5\">";
    cont += "<h3>by Prudence Yao & Mari Allison</h3>";
    cont += "<h2>May 17, 2016</h2>";
    cont += "<input type=\"text\" size=\"40\" id=\"section\" />";
    //    cont += "<input type=\"button\" value=\"Submit\" onclick=\"show()\"/>";
    cont += "<a href=\"#\" id=\"url\"><input type=\"button\" value=\"Submit\" onclick=";
    cont += "\"var b = document.getElementById(\'section\').value;";
    cont += "if(b.length!=0){";
    cont += "var newItem = document.createElement(\'LI\');";
    cont += "var textnode = document.createTextNode(b);";
    cont += "newItem.appendChild(textnode);";
    cont += "var list = document.getElementById(\'myList\');";
    cont += "list.insertBefore(newItem, list.childNodes[0]);";//perfect
    cont += "document.getElementById(\'section\').value=\'\';";
    //    cont += "document.getElementById(\'url\').href=self.location.href+b;";
    cont += "document.getElementById(\'url\').href=b;";
    //    cont += "document.getElementById(\"url\").href="";";
    cont += "window.location.reload();}\"/></a>";

    cont += "<ul id=\"myList\">";
    cont += readfrom(myFile);
    cont += "</ul>";
    //the javascript part doesn't work, so I embedded it into HTML
    //    cont += "<script>";
    //    cont += "function show(){ ";
    //    cont += "var b = document.getElementById(\"section\").value;";
    //    cont += "</script>";
    cont += "</html>\n";
    client.print(cont);
    delay(10);
    Serial.println("Client disonnected\n");
  }
  client.flush();
}


void writeto(File myFile, String content) {
  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile & content != "") {
    Serial.print("Writing to test.txt...");
    String wcontent = "";
    int i = 0;
    while (i < content.length()) {
      if (content[i] == '%' && i <= content.length() - 3) {
        if (content.substring(i, i + 3) == "%20") {//if the byte is %20, it's a space
          wcontent += " ";
          i += 3;
        } else {
          wcontent += content[i];
          i++;
        }
      } else {
        wcontent += content[i];
        i++;
      }
    }
    myFile.println(wcontent);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error writing test.txt");
  }
}

String readfrom(File myFile) {
  myFile = SD.open("test.txt");
  String content = "";
  String sen = "";
  char words;
  char prevs;
  if (myFile) {
    while (myFile.available()) {
      words = myFile.read();
      if (words != '\n') sen += words;
      else if (words == '\n'&prevs != '\n') {
        content = "<li>" + sen + "</li>" + content;
        sen = "";
      }
      prevs = words;
    }
    //    Serial.println(content);
    myFile.close();
    return content;
  } else {
    // if the file didn't open, print an error:
    Serial.println("error reading from the test.txt");
  }
}
