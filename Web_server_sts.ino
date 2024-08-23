#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <ArduinoJson.h>

WebServer webServer(80);

const char *ssid = "Your_ssid";
const char *password = "your_password";

IPAddress apIP(*, *, *, *); // ввести свой ip
DNSServer dnsServer;
const char *server_name = "www.server.com";

String responseHTML = "<!DOCTYPE html><html>"
                      "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                      "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
                      "</style></head>"
                      "<body><h1>ESP32 Web Server</h1>"
                      "<p>Page not found (error 404)</p>"
                      "</body></html>";

StaticJsonDocument<2048> jsonDocument;
char buffer[2048];            // буффер JSON-файла

// ----------------- Форма get-ответа ----------------------- //

const int SIZE_ARRAY = 5;

int Dn;                             // порядковый номер датчика
float T_CENTRE_Dn;                  // температура ступицы, ℃ (float)
float T_AIR_Dn;                     // температура воздуха, ℃ (float)
float PRESS_Dn;                     // давление шины, BAR (float)
float V_BAT_Dn;                     // уровень заряда батареи датчика, V (float)
float VIBRO_Dn;                     // среднее арифметическое значение вибрации положительной зоны (float)
int VIBRO_L_Dn[SIZE_ARRAY];         // список “сырых” значений ускорения, передаваемых с датчика “как есть” (int)
float ACU_Dn;                       // среднее значение амплитуды звукового сигнала (float)
int ACU_L_Dn[SIZE_ARRAY];           // список “сырых” значений звукового сигнала, передаваемых с датчика “как есть” (int)
int N_TX_Dn;                        // номер попытки которую использует датчик для передачи одних и тех же данных данных. Параметр нужен для оценки качества работы радиоканала (int)
int W_TX_Dn;                        // мощность передатчика (int)


void setup() {
  Dn = 2;
  T_CENTRE_Dn = 42.3;
  T_AIR_Dn = 32.1;
  PRESS_Dn = 2.1;
  V_BAT_Dn = 3.56;
  VIBRO_Dn = 4653.4;
  VIBRO_L_Dn[0] = 4567; VIBRO_L_Dn[1] = 4563;
  VIBRO_L_Dn[2] = 4678; VIBRO_L_Dn[3] = 4890; 
  VIBRO_L_Dn[4] = 4569;
  ACU_Dn = 2454.2;
  ACU_L_Dn[0] =  2345; ACU_L_Dn[1] =  2785;
  ACU_L_Dn[2] =  2567; ACU_L_Dn[3] =  2340;
  ACU_L_Dn[4] =  2234;
  N_TX_Dn = 2;
  W_TX_Dn = 22;  
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  const byte DNS_PORT = 53;
  dnsServer.start(DNS_PORT, server_name, apIP);

  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });
  webServer.on("/data", getLastData);
  webServer.on("/status", HTTP_POST, handlePost);
  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}


void getLastData(void){
  Serial.println("Get Sensor Data");
  jsonDocument.clear();
  JsonObject obj = jsonDocument.createNestedObject();

  for (int i=1; i<7; i++)
  {
    String T_CENTRE_name = "T_CENTRE_" + (String)i;
    String T_AIR_name = "T_AIR_" + (String)i;
    String PRESS_name = "PRESS_" + (String)i;
    String V_BAT_name = "V_BAT_" + (String)i;
    String VIBRO_name = "VIBRO_" + (String)i;
    String VIBRO_L_name = "VIBRO_L_" + (String)i;
    String ACU_name = "ACU_" + (String)i;
    String ACU_L_name = "ACU_L_" + (String)i;
    String N_TX_name = "N_TX_" + (String)i;
    String W_TX_name = "W_TX_" + (String)i;

    obj[T_CENTRE_name] = round_range(T_CENTRE_Dn);
    obj[T_AIR_name] = round_range(T_AIR_Dn);
    obj[PRESS_name] = round_range(PRESS_Dn);
    obj[V_BAT_name] = round_range(V_BAT_Dn);
    obj[VIBRO_name] = round_range(VIBRO_Dn);
    JsonArray VIBRO_L_arr = obj.createNestedArray(VIBRO_L_name);
    for (int k=0; k < SIZE_ARRAY; k++)
      {
        VIBRO_L_arr.add(VIBRO_L_Dn[k]);
      }
    obj[ACU_name] = round_range(ACU_Dn);
    JsonArray ACU_L_arr = obj.createNestedArray(ACU_L_name);
    for (int k=0; k < SIZE_ARRAY; k++)
      {
        ACU_L_arr.add(ACU_L_Dn[k]);
      }
    obj[N_TX_name] = N_TX_Dn;
    obj[W_TX_name] = W_TX_Dn;
  }

  serializeJsonPretty(obj, buffer);
  webServer.send(200, "application/json", buffer); 
}


void handlePost() {
  if (webServer.hasArg("plain") == false) {
  }
  Serial.print("POST: ");
  String body = webServer.arg("plain");
  deserializeJson(jsonDocument, body);
  String status = jsonDocument["status"];
  Serial.println(status);
  if (status == "ReceiveDataOk")
    {
      Serial.println("Clearing RAM...");
    }

  webServer.send(200, "application/json", "{}");
}


double round_range(double value) {
  return (int)(value * 100 + 0.5) / 100.0;
}