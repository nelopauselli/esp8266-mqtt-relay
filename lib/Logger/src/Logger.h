#ifndef RELEASE
#define DEBUG(...) Serial.print(__VA_ARGS__)
#define DEBUGLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG(...)
#define DEBUGLN(...)
#endif