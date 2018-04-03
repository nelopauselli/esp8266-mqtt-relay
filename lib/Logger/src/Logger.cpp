#include <Logger.h>
#include <FS.h> // Include the SPIFFS library

// Para activar el log por serial, además hay que liberar los pines RX y TX de donde se estén usando.
#define LOG_TO_SERIAL

Logger_t::Logger_t()
{
#ifdef LOG_TO_SERIAL
    Serial.begin(115200);
#endif
}

void Logger_t::add(Appender *appender)
{
    appenders[appenderIndex++] = appender;
}

void Logger_t::cleanDebug()
{
    if (SPIFFS.exists("/debug.txt"))
        SPIFFS.remove("/debug.txt");
    trace("debug cleaned");
}

void Logger_t::debugging(bool value)
{
    _debugging = value;
    trace(_debugging ? "debugging on" : "debugging off");
}

void Logger_t::cleanLog()
{
    if (SPIFFS.exists("/log.txt"))
        SPIFFS.remove("/log.txt");
    debug("log cleaned");
}

void Logger_t::trace(const String &message)
{
    trace(message.c_str());
}

void Logger_t::trace(const char *message)
{
    File file = SPIFFS.open("/log.txt", "a");
    file.print(millis());
    file.print("\t[TRACE] ");
    file.println(message);
    file.close();

    for (int i = 0; i < appenderIndex; i++)
    {
        Appender *appender = appenders[i];
        if (appender != NULL)
            appender->write(Levels::TRACE, message);
    }
}

void Logger_t::trace(const Printable &message)
{
    File file = SPIFFS.open("/log.txt", "a");
    file.print(millis());
    file.print("\t[TRACE] ");
    file.println(message);
    file.close();

#ifdef LOG_TO_SERIAL
    Serial.print("[TRACE at ");
    Serial.print(millis());
    Serial.print("]\t");
    Serial.println(message);
#endif
}

void Logger_t::debug(const String &message)
{
    debug(message.c_str());
}

void Logger_t::debug(const char *message)
{
    if (_debugging)
    {

        File file = SPIFFS.open("/debug.txt", "a");
        file.print("[");
        file.print(millis());
        file.print("]\t");
        file.println(message);
        file.close();
    }

    for (int i = 0; i < appenderIndex; i++)
    {
        Appender *appender = appenders[i];
        if (appender != NULL)
            appender->write(Levels::DEBUG, message);
    }
}

void Logger_t::debug(const Printable &message)
{
    if (!_debugging)
        return;

    File file = SPIFFS.open("/debug.txt", "a");
    file.print("[");
    file.print(millis());
    file.print("]\t");
    file.println(message);
    file.close();

#ifdef LOG_TO_SERIAL
    Serial.print("[DEBUG at ");
    Serial.print(millis());
    Serial.print("]\t");
    Serial.println(message);
#endif
}

void Logger_t::error(const String &message)
{
    error(message.c_str());
}

void Logger_t::error(const char *message)
{
    File file = SPIFFS.open("/log.txt", "a");
    file.print(millis());
    file.print("\t[ERROR] ");
    file.println(message);
    file.close();

#ifdef LOG_TO_SERIAL
    Serial.print("[ERROR at ");
    Serial.print(millis());
    Serial.print("]\t");
    Serial.println(message);
#endif
}

void Logger_t::error(const Printable &message)
{
    File file = SPIFFS.open("/log.txt", "a");
    file.print(millis());
    file.print("\t[ERROR] ");
    file.println(message);
    file.close();

#ifdef LOG_TO_SERIAL
    Serial.print("[ERROR at ");
    Serial.print(millis());
    Serial.print("]\t");
    Serial.println(message);
#endif
}

void Logger_t::flush()
{
#ifdef LOG_TO_SERIAL
    Serial.flush();
#endif
}

Logger_t Logger;