#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#define LED D2
#define bt1 D0
#define bt2 D1
#define tb1 D3
#define tb2 D4
ESP8266WebServer server(80);
const char *ssid = "Milo";
const char *pass = "19981998";
IPAddress local_IP(192, 168, 1, 120);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);
String datasendwebJson = "";
void ConnectWiFi();
void LED_Blynk();
void LED_Blynk1();
void SendWebPage();
void Send_Update();
void DataJson(String sensor1, String sensor2, String tb1, String tb2);
void readsensor();
void readbutton();
void maintainSV();
void buttononclick();
void xulyONOFF(String button);
void xulyData(String input);
void receiveinput();
int nhietdo = 123, doam = 321, tttb1, tttb2, ibt1, ibt2;
long bien1,bien2;
unsigned long last;
const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
	<title>CONTROL ESP8266</title>
	<meta charset="utf-8">
</head>
<style type="">
	
</style>
<script>
	function UpdateData(){
		var xhttm = new XMLHttpRequest();
		xhttm.onreadystatechange = function(){
			if(this.readyState == 4 && this.status == 200){
				var DataESP  = xhttm.responseText;
				console.log("du lieu esp:"+ DataESP);
				var DataJson = JSON.parse(DataESP);
				if(DataJson.ND !=null){
					document.getElementById("nhietdo").innerHTML = DataJson.ND;
				}
				if(DataJson.DA !=null){
					document.getElementById("doam").innerHTML = DataJson.DA;
				}
				if(DataJson.TB1 !=null){
					if(DataJson.TB1 == 1 ){
						document.getElementById("tttb1").innerHTML = "ON";
					}
					else if(DataJson.TB1 == 0){
						document.getElementById("tttb1").innerHTML = "OFF";
					}
				}
				if(DataJson.TB2 !=null){
					if(DataJson.TB2 == 1 ){
						document.getElementById("tttb2").innerHTML = "ON";
					}
					else if(DataJson.TB2 == 0){
						document.getElementById("tttb2").innerHTML = "OFF";
					}
				}

			}
		}
		xhttm.open('GET','/Update',true);
		xhttm.send();
		setTimeout(function(){UpdateData()},1000);
	}
</script>
<body onload = "UpdateData()">
	<h1> NHIỆT  :
		<label id="nhietdo"></label>
 	</h1>
 	<h1> ĐỘ  :
		<label id="doam"></label>
 	</h1>
 	<h1>
 		TRẠNG THÁI TB1 :
 		<label id="tttb1">ON</label>
 	</h1>
 	<h1>
 		TRẠNG THÁI TB :
 		<label id="tttb2">ON</label>
 	</h1>
</body>
</html>

)=====";
void setup()
{
      pinMode(LED, OUTPUT);
      pinMode(bt1, INPUT_PULLUP);
      pinMode(bt2, INPUT_PULLUP);
      pinMode(tb1, OUTPUT);
      pinMode(tb2, OUTPUT);
      digitalWrite(tb1, LOW);
      digitalWrite(tb2, LOW);
      tttb1 = 0;
      tttb2 = 0;
      LED_Blynk();
      Serial.begin(9600);
      while (!Serial)
            ;
      ConnectWiFi();
      server.on("/", []
                { SendWebPage(); });
      server.on("/Update", []
                { Send_Update(); });
      server.on("/Button", []
                { buttononclick(); });
      server.on("/Input", []
                { receiveinput(); });
      server.begin();
      Serial.print("Server Start");
      LED_Blynk1();
}
void loop()
{
      server.handleClient();
      readbutton();
      last = millis();
      if (millis() - last >= 1000)
      {
            readsensor();
            last = millis();
      }
}
void maintainSV()
{
      server.handleClient();
      last = millis();
      if (millis() - last >= 1000)
      {
            readsensor();
            last = millis();
      }
}
void ConnectWiFi()
{
      WiFi.disconnect();
      delay(1000);
      if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
      {
            Serial.print("ERRORConfigure");
      }
      delay(1000);
      WiFi.begin(ssid, pass);
      while (WiFi.status() != WL_CONNECTED)
      {
            delay(500);
            Serial.print(".");
      }
      Serial.println();
      Serial.print("Connected, IP address : ");
      Serial.print(WiFi.localIP());
}
void LED_Blynk()
{
      digitalWrite(LED, LOW);
}
void LED_Blynk1()
{
      digitalWrite(LED, HIGH);
}
void SendWebPage()
{
      server.send(200, "text/html", webpage);
}
void Send_Update()
{
      DataJson(String(nhietdo), String(doam), String(tttb1), String(tttb2));
      server.send(200, "text/html", String(datasendwebJson));
}
void DataJson(String sensor1, String sensor2, String tttb1, String tttb2)
{
      datasendwebJson = "{\"ND\":\"" + String(sensor1) + "\"," +
                        "\"DA\":\"" + String(sensor2) + "\"," +
                        "\"TB1\":\"" + String(tb1) + "\"," +
                        "\"TB2\":\"" + String(tb2) + "\"," + "\"}";
}
void readsensor()
{
      nhietdo += 1.1;
      doam += .2;
      if (nhietdo > 100)
      {
            nhietdo = 0;
      }
      if (doam > 100)
      {
            doam = 0;
      }
}
void readbutton()
{
      if (digitalRead(bt1) == 0)
      {
            while (1)
            {
                  maintainSV();
                  if (digitalRead(bt1) == 1)
                  {
                        if (tttb1 == 0)
                        {
                              digitalWrite(tb1, HIGH);
                              tttb1 = 1;
                        }
                        else if (tttb1 == 1)
                        {
                              digitalWrite(tb1, LOW);
                              tttb1 = 0;
                        }
                  }
            }
      }
      if (digitalRead(bt2) == 0)
      {
            while (1)
            {
                  maintainSV();
                  if (digitalRead(bt2) == 1)
                  {
                        if (tttb2 == 0)
                        {
                              digitalWrite(tb2, HIGH);
                              tttb2 = 1;
                        }
                        else if (tttb1 == 1)
                        {
                              digitalWrite(tb2, LOW);
                              tttb2 = 0;
                        }
                  }
            }
      }
}
void xulyONOFF(String button)
{
      if (button.indexOf("A0B") >= 0)
      {
            digitalWrite(tb1, LOW);
            tttb1 = 0;
      }
      else if (button.indexOf("A1B") >= 0)
      {
            digitalWrite(tb1, HIGH);
            tttb1 = 1;
      }
      if (button.indexOf("C0D") >= 0)
      {
            digitalWrite(tb2, LOW);
            tttb2 = 0;
      }
      else if (button.indexOf("C1D") >= 0)
      {
            digitalWrite(tb2, HIGH);
            tttb2 = 1;
      }
}
void xulyData(String data)
{
      int TimE, TimF, TimG, TimH = -1;
      TimE = data.indexOf("E");
      TimF = data.indexOf("F");
      TimG = data.indexOf("G");
      TimH = data.indexOf("H");
      if (TimE >= 0 && TimF >= 0)
      {
            String dataEF = "";
            dataEF = data.substring(TimE + 1, TimF);
            bien1 = dataEF.toInt();
            Serial.print("du lieu EF : ");
            Serial.print(bien1);
      }
      if (TimG >= 0 && TimH >= 0)
      {
            String dataGH = "";
            dataGH = data.substring(TimG + 1, TimH);
            bien2 = dataGH.toInt();
            Serial.print("du lieu GH : ");
            Serial.print(bien2);
      }
}
void buttononclick()
{
      String button = "";
      button = server.arg("Button");
      xulyONOFF(String(button));
      server.send(200, "text/html", webpage);
}
void receiveinput()
{
      String input = "";
      input = server.arg("Input");
      xulyData(String(input));
      server.send(200, "text/html", webpage);
}