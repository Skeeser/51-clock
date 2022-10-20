#include "TimeUtils.h"

const char* SSID = "OnePlus 8";        
const char* PASSWORD = "zgx441427";   
int hourfir,hoursec,minutefir,minutesec,secondfir,secondsec;

TimeClient timeClient;
TimeData timeData;

void setup()
{
  Serial.begin(4800);
  WiFi.mode(WIFI_STA); 
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop()
{
  timeClient.UpdateTime(timeData);

  hourfir = timeData.hours / 10;
  hoursec = timeData.hours % 10;
  minutefir = timeData.minutes / 10;
  minutesec = timeData.minutes % 10;
  secondfir = timeData.seconds / 10;
  secondsec = timeData.seconds % 10;

  
  Serial.write(hourfir);
  Serial.write(hoursec);
  Serial.write(minutefir);
  Serial.write(minutesec);
  Serial.write(secondfir);
  Serial.write(secondsec);

  delay(1000);
}
