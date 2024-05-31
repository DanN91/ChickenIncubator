#include <EEPROM.h>
#include <Arduino.h>

#define DHTPIN 3     // data pin of the DHT T/H sensor
#define T_OFFSET 0.0 // temperature sensor offset
#define HEATER A1    // heater MOSFET pin

#define DELAY 2000 // control loop delay in ms

#define TS_ADDR 0 // temperature set point EEPROM address
#define HS_ADDR 4 // humidity set point EEPROM address

#define TI_RESET 1 // integral reset threshold, set integral to 0 when T error greater than this
#define HI_RESET 5 // integral reset threshold, set integral to 0 when H error greater than this

#define H_AUTO_THRES 3   // disable vent control in auto mode if H error < this
#define H_AUTO_COUNT 200 // disable for n cycles

#define HWAT 0.25 // holt winters parameters for temperature smoothing
#define HWBT 0.2
#define HWAH 0.7 // holt winters parameters for humidity smoothing
#define HWBH 0.5

#define A 0.005 // long average parameter

float Ts, Hs;                   // set points
uint8_t Hcontrol;               // H control mode
uint8_t Ts_changed, Hs_changed; // setpoint changed flags
float ET, dETdt, IETdt;         // prop/diff/integ terms for T
float EH, dEHdt, IEHdt;         // prop/diff/integ terms for H
float T, Tavg = NAN, Tvar, Tstd;
float H, Havg = NAN, Hvar, Hstd;
float Hpower, Hduty; // heater current power, average duty cycle
unsigned long t0, Hon, talarm, tventclosed;

void setup()
{
  pinMode(HEATER, OUTPUT);

  Ts = read_float(TS_ADDR);
  Hs = read_float(HS_ADDR);
}

void mainLoop()
{
  unsigned long t1 = millis();
  int dt = t1 - t0;

  if (t1 - Hon > Hpower * DELAY)
  {
    heater(0);
  }

  if (dt > DELAY)
  {
    if (!key)
    {
      T = dht.readTemperature() + T_OFFSET;
      H = dht.readHumidity();

      float dts = dt * 1e-3;

      // temperature Holt-Winters smoothing
      float E0 = ET;
      ET = HWAT * (T - Ts) + (1 - HWAT) * (ET + dETdt * dts); // smoothed T error (deviation from set point)
      dETdt = HWBT * (ET - E0) / dts + (1 - HWBT) * dETdt;    // smoothed derivative
      IETdt += ET * dts;                                      // integral of error
      if (abs(ET) > TI_RESET)                                 // reset integral on big deviation
        IETdt = 0;
      float pidT = 1.1765 * (ET + 0.010526 * IETdt + 23.75 * dETdt); // PID value, adjust coefficients to tune
      Hpower = fanrpm > FAN_THRES ? max(0, min(1, -pidT)) : 0;
      heater(Hpower > 0.1);
      Hon = millis();

      if (abs(ET) > ALARM_T)
      {
        alarm |= 1;
        vent.write(ET < 0 ? 0 : 180);
        if (Hcontrol > 1)
        {
          Hcontrol = 2;
        }
      }
      else
      {
        alarm &= ~1;
      }

      // humidity Holt-Winters smoothing
      E0 = EH;
      EH = HWAH * (H - Hs) + (1 - HWAH) * (EH + dEHdt * dts); // smoothed H error (deviation from set point)
      dEHdt = HWBH * (EH - E0) / dts + (1 - HWBH) * dEHdt;    // smoothed derivative
      IEHdt += EH * dts;                                      // integral of error
      if (abs(EH) > HI_RESET)                                 // reset integral on big deviation
        IEHdt = 0;
      float pidH = 0.1176 * (EH + 0.09091 * IEHdt + 2.75 * dEHdt); // PID value, adjust coefficients to tune
      vent.write(pidH * 180);

      boolean openvent = ventclosed && millis() - tventclosed > VENTOPENMS;
      if (openvent)
      {
        vent.write(180);
        if (millis() - tventclosed > VENTRESETMS)
        {
          tventclosed = millis();
        }
      }

      if (Hcontrol > 1)
      {
        if (abs(EH) > H_AUTO_THRES || openvent)
        {
          Hcontrol = 2;
        }
        else
        {
          if (Hcontrol < H_AUTO_COUNT)
          {
            ++Hcontrol;
          }
          else
          {
            IEHdt = 0;
          }
        }
      }

      // long term averages
      Tavg = A * T + (1 - A) * (isnan(Tavg) ? T : Tavg);
      Tvar = A * pow(T - Tavg, 2) + (1 - A) * (isnan(Tvar) ? 0 : Tvar);
      Tstd = sqrt(Tvar);

      Havg = A * H + (1 - A) * (isnan(Havg) ? H : Havg);
      Hvar = A * pow(H - Havg, 2) + (1 - A) * (isnan(Hvar) ? 0 : Hvar);
      Hstd = sqrt(Hvar);

      Hduty = A * Hpower + (1 - A) * Hduty;

      if (Ts_changed)
      {
        if (Ts_changed-- == 1)
          write_float(TS_ADDR, Ts);
      }

      if (Hs_changed)
      {
        if (Hs_changed-- == 1)
          write_float(HS_ADDR, Hs);
      }
    }

    switch (displayMode)
    {
    case 1: // temperature setpoint
      if (key & (UP | DOWN | LEFT | RIGHT))
      {
        Ts = max(20, min(50, Ts + (key & (UP | RIGHT) ? +1 : -1) * (key & (UP | DOWN) ? 0.1 : 1)));
        Ts_changed = 10;
      }
      break;
    case 2: // humidity setpoint
      if (key & (UP | DOWN))
      {
        Hs = max(10, min(90, Hs + (key & UP ? +1 : -1)));
        Hs_changed = 10;
      }

      if (key & RIGHT)
      {
        if (Hcontrol < 2)
        {
          Hcontrol = ++Hcontrol;
        }
        else
        {
          Hcontrol = 0;
        }
        IEHdt = 0;
      }
      break;
    default:;
    }

    t0 = t1;
  }
}
