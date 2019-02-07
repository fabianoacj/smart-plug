#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <NTPtimeESP.h>

// Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

NTPtime NTPch("br.pool.ntp.org");   // Choose server pool as required
char *ssid      = "Jardim";               // Set you WiFi SSID
char *password  = "*******";               // Set you WiFi password

strDateTime dateTime;
strDateTime lastValidTime;

const long intervalFiveSeconds = 5000000;

unsigned long currentMicros, previousMicrosFive;

float p_sum,
      apparent_power,
      active_power,
      reactive_power,
      power_factor;

float v_max,
      v_min,
      v,
      v_sum_squared,
      v_rms,
      i_max,
      i_min,
      i,
      i_sum_squared,
      i_rms;

unsigned long n = 0;

void setup(void)
{
  Wire.begin(D2, D1);
  Serial.begin(115200);

  WiFi.begin (ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected");

  ads.begin();
}

void loop(void)
{
  unsigned long currentMicros = micros();
  int16_t adc0, adc1;

  adc0 = 3 * ads.readADC_SingleEnded(0);
  adc1 = 3 * ads.readADC_SingleEnded(1);

  v = (adc0 - 2574) * 519 / 1000.;
  if ( v >= 5000) {
    v = 0;
  }
  v_sum_squared = v_sum_squared + v * v;
  if (v > v_max) {
    v_max = v;
  }
  if (v < v_min) {
    v_min = v;
  }

  i = 10.42 * adc1 / 1000. - 26.906;
  if ( i >= 90) {
    i = 0;
  }
  i_sum_squared = i_sum_squared + i * i;
  if (i > i_max) {
    i_max = i;
  }
  if (i < i_min) {
    i_min = i;
  }

  p_sum = p_sum + v * i;

  n++;

  dateTime = NTPch.getNTPtime(-3.0, 0);
  if (dateTime.valid) {
    lastValidTime = dateTime;
  }

  if (currentMicros - previousMicrosFive >= intervalFiveSeconds) {

    previousMicrosFive = currentMicros;

    v_rms = sqrt(v_sum_squared / n);
    i_rms = sqrt(i_sum_squared / n);
    apparent_power = v_rms * i_rms;
    active_power = fabs(p_sum) / n;
    reactive_power = sqrt(apparent_power * apparent_power - active_power * active_power);
    power_factor = active_power / apparent_power;

    Serial.print("n: "); Serial.println(n);
    Serial.print("V_rms: "); Serial.println(v_rms, 5);
    Serial.print("V_max: "); Serial.println(v_max, 5);
    Serial.print("V_min: "); Serial.println(v_min, 5);
    Serial.print("I_rms: "); Serial.println(i_rms, 5);
    Serial.print("I_max: "); Serial.println(i_max, 5);
    Serial.print("I_min: "); Serial.println(i_min, 5);
    Serial.print("apparent_power: "); Serial.println(apparent_power, 5);
    Serial.print("active_power: "); Serial.println(active_power, 5);
    Serial.print("reactive_power: "); Serial.println(reactive_power, 5);
    Serial.print("power_factor: "); Serial.println(power_factor, 5);
    NTPch.printDateTime(lastValidTime);
    Serial.println(" ");
    //      byte actualHour = lastValidTime.hour;
    //      Serial.println( actualHour);
    //      byte actualMinute = lastValidTime.minute;
    //      Serial.println(actualMinute);
    //      byte actualsecond = lastValidTime.second;
    //      Serial.println( actualsecond);
    //      int actualyear = lastValidTime.year;
    //      Serial.println(actualyear);
    //      byte actualMonth = lastValidTime.month;
    //      Serial.println( actualMonth);
    //      byte actualday = lastValidTime.day;
    //      Serial.println(actualday);
    //      byte actualdayofWeek = dateTime.dayofWeek;
    //      Serial.println( actualdayofWeek);

    n = 0;

    v_sum_squared = 0;
    v_rms = 0;
    v_max = 0;
    v_min = 0;

    i_sum_squared = 0;
    i_rms = 0;
    i_max = 0;
    i_min = 0;

    p_sum = 0;
  }

}
