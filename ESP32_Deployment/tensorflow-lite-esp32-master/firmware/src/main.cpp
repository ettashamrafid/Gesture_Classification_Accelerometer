#include <Arduino.h>
#include "NeuralNetwork.h"
#include <esp_now.h>
#include <WiFi.h>

// Structure to receive data
typedef struct struct_message {
  float X_rms;
  float X_std;
  float Y_min;
  float Y_max;
  float Y_range;
  float Y_rms;
  float Y_std;
  float Z_min;
  float Z_max;
  float Z_range;
  float Z_rms;
  float Z_std;
  float X_jerk_rms;
  float X_jerk_std;
  float Y_jerk_min;
  float Y_jerk_max;
  float Y_jerk_range;
  float Y_jerk_rms;
  float Y_jerk_std;
  float Z_jerk_min;
  float Z_jerk_max;
  float Z_jerk_range;
  float Z_jerk_rms;
  float Z_jerk_std;
  float total_jerk_min;
  float total_jerk_max;
  float total_jerk_range;
  float total_jerk_rms;
  float total_jerk_std;
} struct_message;

// Create a struct_message instance to hold the received data
struct_message myData;

NeuralNetwork *nn;

// Callback function that will be called when data is received
void OnDataReceived(const uint8_t* mac_addr, const uint8_t* data, int len) {
  memcpy(&myData, data, sizeof(myData)); // Copy the received data into myData

  // Fill the neural network input buffer with the 33 values from myData
  nn->getInputBuffer()[0] = myData.X_rms;
  nn->getInputBuffer()[1] = myData.X_std;
  nn->getInputBuffer()[2] = myData.Y_min;
  nn->getInputBuffer()[3] = myData.Y_max;
  nn->getInputBuffer()[4] = myData.Y_range;
  nn->getInputBuffer()[5] = myData.Y_rms;
  nn->getInputBuffer()[6] = myData.Y_std;
  nn->getInputBuffer()[7] = myData.Z_min;
  nn->getInputBuffer()[8] = myData.Z_max;
  nn->getInputBuffer()[9] = myData.Z_range;
  nn->getInputBuffer()[10] = myData.Z_rms;
  nn->getInputBuffer()[11] = myData.Z_std;
  nn->getInputBuffer()[12] = myData.X_jerk_rms;
  nn->getInputBuffer()[13] = myData.X_jerk_std;
  nn->getInputBuffer()[14] = myData.Y_jerk_min;
  nn->getInputBuffer()[15] = myData.Y_jerk_max;
  nn->getInputBuffer()[16] = myData.Y_jerk_range;
  nn->getInputBuffer()[17] = myData.Y_jerk_rms;
  nn->getInputBuffer()[18] = myData.Y_jerk_std;
  nn->getInputBuffer()[19] = myData.Z_jerk_min;
  nn->getInputBuffer()[20] = myData.Z_jerk_max;
  nn->getInputBuffer()[21] = myData.Z_jerk_range;
  nn->getInputBuffer()[22] = myData.Z_jerk_rms;
  nn->getInputBuffer()[23] = myData.Z_jerk_std;
  nn->getInputBuffer()[24] = myData.total_jerk_min;
  nn->getInputBuffer()[25] = myData.total_jerk_max;
  nn->getInputBuffer()[26] = myData.total_jerk_range;
  nn->getInputBuffer()[27] = myData.total_jerk_rms;
  nn->getInputBuffer()[28] = myData.total_jerk_std;

  // Run prediction and print the result
  float result = nn->predict();
  Serial.printf("Neural network result: %.2f\n", result);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the callback function for receiving data
  esp_now_register_recv_cb(OnDataReceived);

  // Initialize neural network
  nn = new NeuralNetwork();
}

void loop() {
  // Main loop can be used for other tasks or to manage received data
  delay(10); // Adjust delay as needed
}
