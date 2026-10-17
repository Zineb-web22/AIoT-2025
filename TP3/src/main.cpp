#include "DHT.h"
#include <math.h>

#define DHTPIN 2      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

const int N_FEATURES = 2;
const float MEAN[N_FEATURES]    = { 15.95311508f, 48.57430185f };
const float STD[N_FEATURES]     = { 14.35803848f, 8.82118774f };
const float WEIGHTS[N_FEATURES] = { -0.20867606f, 0.96797814f };
const float BIAS = 0.96938041f;

float X[N_FEATURES] = {0.0f, 0.0f}; // Input features: [Temperature, Humidity]

// 3.1 Standardization
float standardize(float x_raw, int idx)
{
  return (x_raw - MEAN[idx]) / STD[idx];
}

// 3.2 Sigmoid
float sigmoid(float z)
{
  return 1.0f / (1.0f + exp(-z));
}

// 3.3 Core prediction: dot product + bias -> sigmoid
float predict(float features[])
{
  float z = 0.0f;
  for (int i = 0; i < N_FEATURES; i++)
  {
    z += WEIGHTS[i] * features[i];
  }
  z += BIAS;
  return sigmoid(z);
}

void setup()
{
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  dht.begin();
}

void loop()
{
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Feature 0 = Temperature, Feature 1 = Humidity
  X[0] = t;
  X[1] = h;

  // Standardize the inputs
  float x_scaled[N_FEATURES];
  x_scaled[0] = standardize(X[0], 0);
  x_scaled[1] = standardize(X[1], 1);

  // Compute w.x + b and apply sigmoid
  float y_pred = predict(x_scaled);

  // Print raw readings
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.print(F(" C  Humidity: "));
  Serial.print(h);
  Serial.println(F(" %"));

  // Print probability and class
  Serial.print(F("Predicted Probability: "));
  Serial.println(y_pred, 4);

  if (y_pred >= 0.5f)
    Serial.println(F("Predicted Class: 1"));
  else
    Serial.println(F("Predicted Class: 0"));

  Serial.println();
}
