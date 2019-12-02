#include <DHT.h>                                        // Inclui a biblioteca do sensor DHT
#include <SoftwareSerial.h>                             // Inclui a biblioteca para permitir o uso dos pinos 2 e 3 para comunicação Serial

#define TINY_GSM_MODEM_SIM808                           // Define o modem a ser utilizado
#include <TinyGsmClient.h>                              // Inclui a biblioteca que configura o modem GSM
#include <ArduinoHttpClient.h>

#define DHTPIN 12                                       // Define o pino do DHT no Arduino
#define DHTTYPE DHT22                                   // Define o modelo do DHT
#define RX_PIN 2
#define TX_PIN 3
#define CELLNUMBER = "+5519992992008";                  // Número de celular que recebe mensagens SMS de alerta

#define SerialMon Serial                                // Set serial for debug console (to the Serial Monitor, default speed 115200)
SoftwareSerial SerialAT(RX_PIN, TX_PIN);                // Set serial for AT commands (to the module)

//#define TINY_GSM_DEBUG SerialMon                        // Define the serial console for debug prints, if needed

#define GSM_AUTOBAUD_MIN 1200
#define GSM_AUTOBAUD_MAX 74880
#define BAUD_RATE 115200

DHT dht(DHTPIN, DHTTYPE);

TinyGsm modem(SerialAT);                                // Declara o objeto modem   
//TinyGsmClientSecure client(modem);
TinyGsmClient client(modem);

const char server[] = "cteepmonitoring.azurewebsites.net";
const int  port = 80;

HttpClient http(client, server, port);                            

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "zap.vivo.com.br";              //YourAPN
//const char apn[] = "wap.nextel3g.net.br";
const char gprsUser[] = "";
const char gprsPass[] = "";

//bool smsSent = false;                                   //Flag que garante só um envio de sms quando o dispositivo estiver muito longe

float lat, lon, temp, humi;  

//Configuração referente ao baud rate das diferentes comunicações serial
void serialConfig() {
  SerialMon.begin(BAUD_RATE);  
  delay(10);

  SerialMon.println("Wait...");
  
  TinyGsmAutoBaud(SerialAT,GSM_AUTOBAUD_MIN,GSM_AUTOBAUD_MAX);
  delay(3000);
}

void httpPost() {
//  SerialMon.println("making POST request");
  String contentType = "application/json";
/*  String postData = "Latitude=";
  postData.concat(String(lat, 6));
  postData.concat("&Longitude=");
  postData.concat(String(lon, 6));
  postData.concat("&Teperature=");
  postData.concat(String(temp, 2));
  postData.concat("&Humidity=");
  postData.concat(String(humi, 2));
  */

  String postData = "{\"Latitude\": "; 
  postData += String(lat, 6);
  postData += ", \"Longitude\": ";
  postData += String(lon, 6);
  postData += ", \"Temperature\": ";
  postData += String(temp, 1);
  postData += ", \"Humidity\": ";
  postData += String(humi, 1);
  postData += "}";

  Serial.println(postData);

  //http.connectionKeepAlive(); // Currently, this is needed for HTTPS
//  http.post("/add.php", contentType, postData);
/*  http.beginRequest();
  http.post("/add.php");
  http.sendHeader("Content-Type", contentType);
  http.sendHeader("Content-Length", postData.length());
  http.beginBody();
  http.print(postData);
  http.endRequest();*/

  // read the status code and body of the response
  int statusCode = http.responseStatusCode();
  String response = http.responseBody();

//  Serial.print("Status code: ");
//  Serial.println(statusCode);
//  Serial.print("Response: ");
//  Serial.println(response);

//  http.stop();
}

void modemDisconnect() {
  modem.gprsDisconnect();
  if (!modem.isGprsConnected()) {
    SerialMon.println("GPRS disconnected");
  } else {
    SerialMon.println("GPRS disconnect: Failed.");
  }
}

int modemConnect() {
  // Conexão GPRS
  SerialMon.println("Waiting for network...");
  if (!modem.waitForNetwork()) {
    delay(10000);
    return 0;
  }

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

  SerialMon.print("Connecting to ");
  SerialMon.println(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    delay(10000);
    return 0;
  }

  bool res = modem.isGprsConnected();
  SerialMon.print("GPRS status:");
  SerialMon.println(res ? "connected" : "not connected");

  String ccid = modem.getSimCCID();
  SerialMon.print("CCID:");
  SerialMon.println(ccid);

  String imei = modem.getIMEI();
  SerialMon.print("IMEI:");
  SerialMon.println(imei);

  String cop = modem.getOperator();
  SerialMon.print("Operator:");
  SerialMon.println(cop);

  IPAddress local = modem.localIP();
  SerialMon.print("Local IP:");
  SerialMon.println(local);

  int csq = modem.getSignalQuality();
  SerialMon.print("Signal quality:");
  SerialMon.println(csq);

  return 1;
}

int modemInit() {
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  if (!modem.restart()) {
  // if (!modem.init()) {
    SerialMon.println("Failed to restart modem, delaying 10s and retrying");
    delay(3000);
    // restart autobaud in case GSM just rebooted
    TinyGsmAutoBaud(SerialAT,GSM_AUTOBAUD_MIN,GSM_AUTOBAUD_MAX);
    delay(10000);
    return 0;
  }
  return 1;
}
 
void setup() {
  serialConfig();                                       //Seta o baud rate que será utilizado pelo monitor serial e o que será usado pelo SIM808
  dht.begin();
}

void loop() {  
  if(!(modemInit())) return;
  
  //if(!(modemConnect())) return;

  //GPS
  modem.enableGPS();
  String gps_raw = modem.getGPSraw();
  //modem.disableGPS();
  SerialMon.print("GPS raw data:");
  SerialMon.println(gps_raw);

  while(1){
    //Tenta obter valores de GPS
    if(modem.getGPS(&lat, &lon)) {  //if(getGSMLocation(mySerial, &la, &lo))  Caso o sinal GPS esteja ruim, teste utilizando a rede GSM substituindo este if
      SerialMon.println("GPS signal ok. Values obtained successfully.");
    }
    else {
      //Se não foi possível obter os valores, exibe vazio
      // SerialMon.println("GPS signal is down. Values not obtained.");
    }

    measure();
    httpPost();   
    
    delay(10000);
  }
}

//Função que realiza as medidas
//Temperatura em graus
//Umidade em %
void measure() {
  temp = dht.readTemperature();                   //Leitura do valor da temperatura
  humi = dht.readHumidity();                      //Leitura do valor da umidade
}
