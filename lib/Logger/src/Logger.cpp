#include <Logger.h>
#include <FS.h> // Include the SPIFFS library

Logger_t::Logger_t()
{
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
            appender->write("[TRACE] ", message);
    }
}

void Logger_t::trace(const Printable &message)
{
    File file = SPIFFS.open("/log.txt", "a");
    file.print(millis());
    file.print("\t[TRACE] ");
    file.println(message);
    file.close();
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
            appender->write("[DEBUG] ", message);
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
}

void Logger_t::error(const Printable &message)
{
    File file = SPIFFS.open("/log.txt", "a");
    file.print(millis());
    file.print("\t[ERROR] ");
    file.println(message);
    file.close();
}

Logger_t Logger;