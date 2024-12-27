#include <Arduino.h>
#include "NeuralNetwork.h"
#include <esp_system.h>
#include <esp_heap_caps.h>
#include <stdio.h>

// ADXL335 pins (connect to appropriate ADC pins on the ESP32)
const int X_PIN = A0;
const int Y_PIN = A3;
const int Z_PIN = A6;

// Filter coefficients for a 4th-order Butterworth low-pass filter
const float b[] = {0.00482434, 0.01929737, 0.02894606, 0.01929737, 0.00482434};
const float a[] = {1.0, -2.36951, 2.31399, -1.05467, 0.18738};

// Array to hold the last 200 samples for each axis
const int WINDOW_SIZE = 200;
float x_history[WINDOW_SIZE] = {0.0};
float y_history[WINDOW_SIZE] = {0.0};
float z_history[WINDOW_SIZE] = {0.0};
float jerk_x_history[WINDOW_SIZE] = {0.0};
float jerk_y_history[WINDOW_SIZE] = {0.0};
float jerk_z_history[WINDOW_SIZE] = {0.0};
float total_jerk_history[WINDOW_SIZE] = {0.0};

int history_index = 0;

// Variables for calculating jerk
float last_x_filtered = 0.0;
float last_y_filtered = 0.0;
float last_z_filtered = 0.0;

// Structure to send data
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

struct_message myData;

// Neural Network
NeuralNetwork *nn;

void setup() {
Serial.begin(115200);
  nn = new NeuralNetwork();
  
  if (psramFound()) {
    Serial.println("PSRAM is enabled and available!");
    printf("Total PSRAM: %d bytes\n", esp_spiram_get_size());
    // printf("Free PSRAM: %d bytes\n", esp_spiram_get_free_size());
  } else {
    Serial.println("PSRAM not found.");
  }
}

float butterworth_filter(float input, float* x_history, float* y_history) {
  // Shift history array for new input
  for (int i = 4; i > 0; i--) {
    x_history[i] = x_history[i - 1];
    y_history[i] = y_history[i - 1];
  }
  x_history[0] = input;

  // Compute new output
  float output = 0.0;
  for (int i = 0; i < 5; i++) {
    output += b[i] * x_history[i];
  }
  for (int i = 1; i < 5; i++) {
    output -= a[i] * y_history[i];
  }
  
  y_history[0] = output;

  return output;
}

void update_statistics() {
  // [Existing feature calculation code remains here]
  // Ensure all features are updated in the `myData` structure.
}

void pass_features_to_nn() {
  // Pass features to the neural network input buffer
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
}

void loop() {
  // Read sensor data and calculate features
  int x_raw = analogRead(X_PIN);
  int y_raw = analogRead(Y_PIN);
  int z_raw = analogRead(Z_PIN);

  // Convert the raw values to acceleration values (in g)
  float x_g = (x_raw - 2048.0) / 2048.0 * 3.3;
  float y_g = (y_raw - 2048.0) / 2048.0 * 3.3;
  float z_g = (z_raw - 2048.0) / 2048.0 * 3.3;

  // Apply the Butterworth filter to each axis
  last_x_filtered = butterworth_filter(x_g, x_history, y_history);
  last_y_filtered = butterworth_filter(y_g, x_history, y_history);
  last_z_filtered = butterworth_filter(z_g, x_history, y_history);

  // Update statistical features
  update_statistics();

  // Pass features to the neural network and get the prediction
  pass_features_to_nn();
  float result = nn->predict();

  // Print the prediction result
  Serial.print("NN Prediction: ");
  Serial.println(result);

  // Delay to control the frequency of data collection and sending
  delay(100);
}
