// This #include statement was automatically added by the Particle IDE.
#include <DS18B20.h>
#include <math.h>

int relay = D7;

////////////////////////////////////////////////////////
// temperature sample code
const int      MAXRETRY          = 4;
const uint32_t msSAMPLE_INTERVAL = 1500;
const uint32_t msMETRIC_PUBLISH  = 10000;
char     szInfo[64];
double   celsius;
double   fahrenheit;
uint32_t msLastMetric;
uint32_t msLastSample;

DS18B20  ds18b20(D4, true); //Sets Pin D2 for Water Temp Sensor and
                            // this is the only sensor on bus

///////////////////////////////
double setpoint = 140.0;
int secondsAtTemperature = 0;
bool wasAtTemperature = false;

void setup() {
  pinMode(relay, OUTPUT);

//   setpoint = 140.0;

  // temperature sample code
  Time.zone(-5);
  Particle.variable("tempHotWater", fahrenheit);
  Serial.begin(115200);
}


void loop() {
  getTemp();

  char message [128];
  sprintf(message, "temp: %2.2f; secondsAtTemperature: %5d; wasAtTemperature: %1d; ", fahrenheit, secondsAtTemperature, wasAtTemperature);
  Particle.publish("log", message, PRIVATE);

  if (fahrenheit < setpoint) {
    digitalWrite(relay, HIGH);
    wasAtTemperature = false;
  } else {
    digitalWrite(relay, LOW);
    wasAtTemperature = true;
  }

  if (wasAtTemperature) {
    secondsAtTemperature = 2 + secondsAtTemperature;
  }


 if((secondsAtTemperature % 60) == 0){
    sprintf(message, "at temp for %3d minutes", secondsAtTemperature / 60);
    Particle.publish("timer", message, PRIVATE);
  }


  delay(2000);
}

void getTemp(){
  float _temp;
  int   i = 0;

  do {
    _temp = ds18b20.getTemperature();
  } while (!ds18b20.crcCheck() && MAXRETRY > i++);

  if (i < MAXRETRY) {
    celsius = _temp;
    fahrenheit = ds18b20.convertToFahrenheit(_temp);
    // Serial.println(fahrenheit);
  }
  else {
    celsius = fahrenheit = NAN;
    Serial.println("Invalid reading");
  }
  msLastSample = millis();
}

