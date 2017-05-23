#include <ESP8266WiFi.h>

#define wifi_ssid "parth"
#define wifi_password "parthpandey"
const char* host = "api.thingspeak.com";
const char* writeAPIKey = "HWAZGFYM5ZW4E8ZI";

char cmd_arr1[100];
int cmd_count1;
int i;
float temp=24.3;
int smoke=200;
int panic=0;

//*****************************************************************
void setup_wifi() 
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    
  digitalWrite(5, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//*****************************************************************
void serial_get_command()
{
  char inchar=0;
  if(Serial.available() > 0)
  {
    inchar = Serial.read();    
    if(inchar == '<')
    {
      cmd_count1=0;
      while(inchar != '>' && cmd_count1<15)
      {
        if(Serial.available() > 0)
        {
          inchar = Serial.read();
          cmd_arr1[cmd_count1++] = inchar
        }        
      }
      if(inchar == '>')
      {
        if(cmd_arr1[0]=='T')
        {cmd_arr1[0]='0';temp = atof(cmd_arr1);Serial.print("Temp : ");Serial.println(temp);}
        else if(cmd_arr1[0]=='S')
        {cmd_arr1[0]='0';smoke = atoi(cmd_arr1);Serial.print("Smoke : ");Serial.println(smoke);}
        else if(cmd_arr1[0]=='P')
        {cmd_arr1[0]='0';panic = atoi(cmd_arr1);Serial.print("Panic : ");Serial.println(panic);}
        else if(cmd_arr1[0]=='1')
        {
          mobilepush("Smoke level above 50% @ JIIT Sect. 128. Respond ASAP");
        }
        else if(cmd_arr1[0]=='2')
        {
          mobilepush("Panic Button Detected @ JIIT Sect. 128. Respond ASAP");
        }
        else if(cmd_arr1[0]=='3')
        {
          mobilepush("Temp above 50 degC @ JIIT Sect. 128. Respond ASAP");
        }
        else if(cmd_arr1[0]=='U')
        {upload_data();}
      }
    }
  }
}
//*****************************************************************
void upload_data()
{
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    return;
  }
  Serial.println("Client Connected");
  String url = "/update?key=";
  url+=writeAPIKey;
  url+="&field1=";
  url+=String(temp);
  url+="&field2=";
  url+=String(smoke);
  url+="&field3=";
  url+=String(panic);
  url+="\r\n";
  
  // Request to the server
  client.print(String("GET ") + url + " HTTP/1.1" +
               "Host: " + host + 
               "Connection: close\r\n");
               
  for(i=0;i<5;i++)
  {
    digitalWrite(5, LOW);delay(100);digitalWrite(5, HIGH);delay(100);
  }
}
//*****************************************************************
void mobilepush(String val4) {

  const uint16_t port = 80; // Web port.
  const char * host = "api.instapush.im"; // Instapush API website.
   
  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("Connection failed");
    Serial.println("wait 5 sec...");
    delay(5000);
    return;
  }
  // Send a request to the server
  // Make JSON request:
  String json1 = String("{\"event\":\"ERS\",\"trackers\":{\"alert\":\""+val4+"\"}}");

  // Make a HTTP request:
  client.print("POST /v1/post HTTP/1.1\n");
  client.print("User-Agent: ESP8266\n");
  client.print("Host: api.instapush.im\n");
  client.print("x-instapush-appid: 58fdda71a4c48a1968de6e9c\n");
  client.print("x-instapush-appsecret: 0edcfd215a96e5933889d98b2d5ef1ac\n");
  client.print("Content-Type: application/json\n");
  client.print("Content-Length: ");
  client.print(json1);
  Serial.println("Sent push, closing connection.");
  client.stop();
}
//*****************************************************************
void setup() 
{
  Serial.begin(9600);
  Serial.println("Program Started");
  pinMode(5, OUTPUT);digitalWrite(5, LOW);
  setup_wifi();
}
//*****************************************************************
void loop()
{
  serial_get_command();
}
//*****************************************************************
