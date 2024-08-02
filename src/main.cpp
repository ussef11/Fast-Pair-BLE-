#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <WiFi.h>
#include <Preferences.h> 
#define LED 2

BLEServer *pServer;

// First service characteristics
BLECharacteristic *Wifi_Characteristic;
BLECharacteristic *TotalDistance_Characteristic;
BLECharacteristic *TotalHours_Characteristic;
BLECharacteristic *CoolantTemp_Characteristic;
BLECharacteristic *RPM_Characteristic;
BLECharacteristic *BrakePP_Characteristic;
BLECharacteristic *AccPP_Characteristic;
BLECharacteristic *CAN10_Characteristic;
BLECharacteristic *VAL_1;
BLECharacteristic *VAL_2;

// Second service characteristics
BLECharacteristic *VAL_3;
BLECharacteristic *VAL_4;
BLECharacteristic *VAL_5;
BLECharacteristic *VAL_6;
BLECharacteristic *VAL_7;
BLECharacteristic *VAL_8;
BLECharacteristic *VAL_9;
BLECharacteristic *VAL_10;
BLECharacteristic *VAL_11;
BLECharacteristic *VAL_12;

bool deviceConnected = false;

Preferences preferences;

  #define SERVICE_UUID "49ba1c81-155f-4799-a0fb-5d583a5586a5"
  #define SERVICE2_UUID "49ba1c81-155f-4799-a0fb-5d583a5586a5"
  #define SERVICE3_UUID "49ba1c81-155f-4799-a0fb-5d583a5586a5"

  #define VALUE_WIFI  "7ce9fd0d-cf46-4838-b39c-9447f6f80256"
  #define VALUE_TotalDistance  "52cd15c8-b8f8-4237-a626-cbb26992fec4"
  #define VALUE_TotalHours  "541db2a4-902d-4a53-8f8c-0272720e783c"
  #define VALUE_CoolantTemp  "580e3783-724f-41a0-ba29-f13e690b3d87"
  #define VALUE_RPM  "b6d08758-c74a-41be-aad3-7df0e9e5b6ff"
  #define VALUE_BrakePP  "b3923f32-214d-4a34-98ed-b095bc920cb7"
  #define VALUE_AccPP  "c0051ae2-4589-4764-a4fd-26a423df67c8"
  #define VALUE_CAN10  "00ec89b6-91a8-4deb-9454-00129fb7e9d5"

  #define VALUE_VAL1  "3c289f2b-c171-4bb1-8a9b-e63f46e8fc47"
  #define VALUE_VAL2  "d9e3e9a0-5791-44e4-9b62-6bcd729bb7ea"
  #define VALUE_VAL3  "ba9fb2a1-6ff2-4e95-bf1f-9c3ae76e58d4"
  #define VALUE_VAL4  "e68e1b45-4b74-4912-8ddc-9d092e582e32"
  #define VALUE_VAL5  "26a31b32-8159-42c9-87b3-526cb621ab1e"
  #define VALUE_VAL6  "09ef9cb7-d7cb-4b2e-9407-5ebd95b7d671"
  #define VALUE_VAL7  "c7a97d2c-5075-4d85-9638-77c1e8f6ab58"
  #define VALUE_VAL8  "1bc1cf34-569d-49b0-9158-908b46d5a2f8"
  #define VALUE_VAL9  "b9f103c1-08b7-493e-8034-c5086f79385d"
  #define VALUE_VAL10 "c1fb647d-d07e-44ad-8969-99d3b62636de"
  #define VALUE_VAL11 "9a9dc5b3-82ad-4530-859f-e5e0f3512891"
  #define VALUE_VAL12 "5b80f39e-8b27-4603-9c9e-2145a4f9d351"

const size_t bufferSize = JSON_OBJECT_SIZE(10);
void connectToWiFi(const char* ssid, const char* password);
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Client Connected");
    };

    void onDisconnect(BLEServer* pServer) { 
      deviceConnected = false;
      Serial.println("Client Disconnected");
      BLEDevice::startAdvertising(); 
    }
};
String status = "Null";
class CustomCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    
    if (value.length() > 0) {
      Serial.println("Received data:");
      for (int i = 0; i < value.length(); i++) {
        Serial.print(value[i]);
      }
      Serial.println();

      digitalWrite(LED,HIGH);
      delay(100);
      digitalWrite(LED,LOW);

      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, value);
      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      const char* ssid = doc["ssid"];
      const char* password = doc["password"];
      Serial.print("SSID: ");
      Serial.println(ssid);
      Serial.print("Password: ");
      Serial.println(password);

      connectToWiFi(ssid, password);


    }
  }
};

  void connectToWiFi(const char* ssid, const char* password) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    int maxRetries = 30;
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < maxRetries) {
      delay(500);
      Serial.print(".");
      retries++;
    }

    
    if (WiFi.status() == WL_CONNECTED) {
      
      preferences.begin("wifiCreds", false);
      preferences.putString("ssid", ssid);
      preferences.putString("password", password);
      preferences.end();
      status = "Connected";
      Serial.println("Connected");
    
    } else {
      Serial.println("Failed to connect to WiFi.");
      status = "Failed";
    }

    
  }


  void connectToWiFiStartup(const char* ssid, const char* password) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    int maxRetries = 30;
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < maxRetries) {
      delay(500);
      Serial.print(".");
      retries++;
    }

    
    if (WiFi.status() == WL_CONNECTED) {
    
       Serial.println("Connected");
    
    } else {
      Serial.println("Failed to connect to WiFi.");
      status = "Failed";
    }

  }


void setup() {
  pinMode(LED,OUTPUT);

  Serial.begin(115200);

    




  Serial.println("Start BLE");

  BLEDevice::init("350123451234560");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(BLEUUID(SERVICE_UUID), 30, 0);

  Wifi_Characteristic = pService->createCharacteristic(
                      VALUE_WIFI,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  Wifi_Characteristic->addDescriptor(new BLE2902());
  Wifi_Characteristic->setCallbacks(new CustomCharacteristicCallbacks());

  TotalDistance_Characteristic = pService->createCharacteristic(
                      VALUE_TotalDistance,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  TotalDistance_Characteristic->addDescriptor(new BLE2902());

  TotalHours_Characteristic = pService->createCharacteristic(
                      VALUE_TotalHours,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  TotalHours_Characteristic->addDescriptor(new BLE2902());

  CoolantTemp_Characteristic = pService->createCharacteristic(
                      VALUE_CoolantTemp,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  CoolantTemp_Characteristic->addDescriptor(new BLE2902());

  RPM_Characteristic = pService->createCharacteristic(
                      VALUE_RPM,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  RPM_Characteristic->addDescriptor(new BLE2902());

  BrakePP_Characteristic = pService->createCharacteristic(
                      VALUE_BrakePP,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  BrakePP_Characteristic->addDescriptor(new BLE2902());

  AccPP_Characteristic = pService->createCharacteristic(
                      VALUE_AccPP,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  AccPP_Characteristic->addDescriptor(new BLE2902());

  CAN10_Characteristic = pService->createCharacteristic(
                      VALUE_CAN10,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  CAN10_Characteristic->addDescriptor(new BLE2902());

  VAL_1 = pService->createCharacteristic(
                      VALUE_VAL1,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  VAL_1->addDescriptor(new BLE2902());

  VAL_2 = pService->createCharacteristic(
                      VALUE_VAL2,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  VAL_2->addDescriptor(new BLE2902());

  pService->start();

  BLEService *pService2 = pServer->createService(BLEUUID(SERVICE2_UUID), 30, 0);
  VAL_1 = pService2->createCharacteristic(
                      VALUE_VAL1,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  VAL_1->addDescriptor(new BLE2902());

  VAL_2 = pService2->createCharacteristic(
                      VALUE_VAL2,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  VAL_2->addDescriptor(new BLE2902());


  VAL_3 = pService2->createCharacteristic(
                      VALUE_VAL3,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );  
  VAL_3->addDescriptor(new BLE2902());

  VAL_4 = pService2->createCharacteristic(
                      VALUE_VAL4,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  VAL_4->addDescriptor(new BLE2902());

  VAL_5 = pService2->createCharacteristic(
                      VALUE_VAL5,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  VAL_5->addDescriptor(new BLE2902());

  VAL_6 = pService2->createCharacteristic(
                      VALUE_VAL6,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  VAL_6->addDescriptor(new BLE2902());

  VAL_7 = pService2->createCharacteristic(
                      VALUE_VAL7,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  VAL_7->addDescriptor(new BLE2902());

  VAL_8 = pService2->createCharacteristic(
                      VALUE_VAL8,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  VAL_8->addDescriptor(new BLE2902());
  pService2->start();

  BLEService *pService3 = pServer->createService(BLEUUID(SERVICE3_UUID), 30, 0);


  VAL_9 = pService3->createCharacteristic(
                      VALUE_VAL9,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  VAL_9->addDescriptor(new BLE2902());

  VAL_10 = pService3->createCharacteristic(
                      VALUE_VAL10,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  VAL_10->addDescriptor(new BLE2902());

  VAL_11 = pService3->createCharacteristic(
                      VALUE_VAL11,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  VAL_11->addDescriptor(new BLE2902());

  VAL_12 = pService3->createCharacteristic(
                      VALUE_VAL12,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  VAL_12->addDescriptor(new BLE2902());

 pService3->start();

  pServer->getAdvertising()->start();
  Serial.println("Waiting for a client connection to notify...");



  preferences.begin("wifiCreds", true);
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  preferences.end();

  
  if (ssid.length() > 0 && password.length() > 0) {
      connectToWiFiStartup(ssid.c_str(), password.c_str());
      Serial.print("SSID: ");
      Serial.println(ssid);
      Serial.print("Password: ");
      Serial.println(password);

     
  } else {
    Serial.println("No saved WiFi credentials");
  }

  // if (!psramInit()) {
  //   Serial.println("PSRAM init failed");
  //   return;
  // }


}

void loop() {
  if (deviceConnected) {
    StaticJsonDocument<bufferSize> jsonDoc1, jsonDoc2, jsonDoc3, jsonDoc4, jsonDoc5;
    StaticJsonDocument<bufferSize> jsonDoc6, jsonDoc7, jsonDoc8, jsonDoc9, jsonDoc10, jsonDoc11, jsonDoc12;
    StaticJsonDocument<bufferSize> jsonDoc13, jsonDoc14, jsonDoc15, jsonDoc16, jsonDoc17, jsonDoc18;
    StaticJsonDocument<bufferSize> jsonDoc19, jsonDoc20, jsonDoc21, jsonDoc22, jsonDoc23, jsonDoc24;

    jsonDoc1["FTank"] = random(0, 101);
    jsonDoc2["TDistance"] = random(0, 101);
    jsonDoc3["THours"] = random(0, 101);
    jsonDoc4["CTemp"] = random(0, 101);
    jsonDoc5["RPM"] = random(0, 101);
    jsonDoc6["BrakePP"] = random(0, 101);
    jsonDoc7["AccPP"] = random(0, 101);
    jsonDoc8["CAN10"] = random(0, 101);
    jsonDoc9["VAL1"] = random(0, 101);
    jsonDoc10["VAL2"] = random(0, 101);
    jsonDoc11["VAL3"] = random(0, 101);
    jsonDoc12["VAL4"] = random(0, 101);
    jsonDoc13["VAL5"] = random(0, 101);
    jsonDoc14["VAL6"] = random(0, 101);
    jsonDoc15["VAL7"] = random(0, 101);
    jsonDoc16["VAL8"] = random(0, 101);
    jsonDoc17["VAL9"] = random(0, 101);
    jsonDoc18["VAL10"] = random(0, 101);
    jsonDoc19["VAL11"] = random(0, 101);
    jsonDoc20["VAL12"] = random(0, 101);


    
    // Wifi_Characteristic->setValue(jsonDoc1.as<String>().c_str());
    // Wifi_Characteristic->notify();

  
    Wifi_Characteristic->setValue(status.c_str());
    Wifi_Characteristic->notify();




    TotalDistance_Characteristic->setValue(jsonDoc2.as<String>().c_str());
    TotalDistance_Characteristic->notify();

    TotalHours_Characteristic->setValue(jsonDoc3.as<String>().c_str());
    TotalHours_Characteristic->notify();

    CoolantTemp_Characteristic->setValue(jsonDoc4.as<String>().c_str());
    CoolantTemp_Characteristic->notify();

    RPM_Characteristic->setValue(jsonDoc5.as<String>().c_str());
    RPM_Characteristic->notify();

    BrakePP_Characteristic->setValue(jsonDoc6.as<String>().c_str());
    BrakePP_Characteristic->notify();

    AccPP_Characteristic->setValue(jsonDoc7.as<String>().c_str());
    AccPP_Characteristic->notify();

    CAN10_Characteristic->setValue(jsonDoc8.as<String>().c_str());
    CAN10_Characteristic->notify();

    VAL_1->setValue(jsonDoc9.as<String>().c_str());
    VAL_1->notify();

    VAL_2->setValue(jsonDoc10.as<String>().c_str());
    VAL_2->notify();

    
    VAL_3->setValue(jsonDoc11.as<String>().c_str());
    VAL_3->notify();

    VAL_4->setValue(jsonDoc12.as<String>().c_str());
    VAL_4->notify();

    VAL_5->setValue(jsonDoc13.as<String>().c_str());
    VAL_5->notify();

    VAL_6->setValue(jsonDoc14.as<String>().c_str());
    VAL_6->notify();

    VAL_7->setValue(jsonDoc15.as<String>().c_str());
    VAL_7->notify();

    VAL_8->setValue(jsonDoc16.as<String>().c_str());
    VAL_8->notify();

    VAL_9->setValue(jsonDoc17.as<String>().c_str());
    VAL_9->notify();

    VAL_10->setValue(jsonDoc18.as<String>().c_str());
    VAL_10->notify();

    VAL_11->setValue(jsonDoc19.as<String>().c_str());
    VAL_11->notify();

    VAL_12->setValue(jsonDoc20.as<String>().c_str());
    VAL_12->notify();



    delay(3000);
  }
}
