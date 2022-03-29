// #include "Ultrasonic.h"

// #define LOW 0
// #define HIGH 1
// #define OUTPUT 0
// #define INPUT 1

// void Ultrasonic(int TP, int EP, long TO)
// {
//   pinMode(TP, OUTPUT);
//   pinMode(EP, INPUT);
//   Trig_pin = TP;
//   Echo_pin = EP;
//   Time_out = TO;
// }

// long Timing()
// {
//   digitalWrite(Trig_pin, LOW);
//   delayMicroseconds(2);
//   digitalWrite(Trig_pin, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(Trig_pin, LOW);
//   duration = pulseIn(Echo_pin, HIGH, Time_out);
//   if (duration == 0)
//   {
//     duration = Time_out;
//   }
//   return duration;
// }

// long Ranging(int sys)
// {
//   Timing();
//   if (sys)
//   {
//     distance_cm = duration / 29 / 2;
//     return distance_cm;
//   }
//   else
//   {
//     distance_inc = duration / 74 / 2;
//     return distance_inc;
//   }
// }