#include <Arduino.h>
#include "Storage.h"
#include "DavcGauge.h"
#include "OledDisplay.h"
#include "Command.h"
#include "deviceConfig.h"

Storage storage("/", sdClkPin, sdMisoPin, sdMosiPin, sdCsPin);
DavcGauge gauge(1, gaugeRxPin, gaugeTxPin);
OledDisplay display(displayWidth, displayHeight);

void setup()
{
    Serial.begin(115200);

    storage.setup();
    gauge.setup();
    display.setup();
}

String logFileName;
uint32_t updateTime = 0;
uint64_t updateTimeLong = 0;
uint64_t count = 0;
uint32_t samplingRate = 1000;
bool verboseMode = false;

void runCommand(Stream &stream);

void loop()
{
    runCommand(Serial);

    updateTimeLong += millis() - updateTime;
    updateTime = updateTimeLong;

    double value = gauge.readValue();
    display.showValue(value, gauge.getPressureUnit(), logFileName.length() != 0 && updateTimeLong / 1000 % 2 != 0);
    uint64_t count2 = updateTimeLong / samplingRate;
    if (count != count2)
    {
        count = count2;
        if (logFileName.length() != 0)
        {
            storage.traceLog(logFileName, updateTimeLong, String(value) + "," + gauge.getPressureUnit());
        }
    }

    delay(deviceIdle);
}

void runValueCommand(String param, Stream &stream)
{
    double value = gauge.readValue();
    stream.println(value);
}

void runUnitCommand(String param, Stream &stream)
{
    if (param.length() != 0)
    {
        gauge.setPressureUnit(param);
    }
    else
    {
        stream.println(gauge.getPressureUnit());
    }
}

void runSamplingCommand(String param, Stream &stream)
{
    if (param.length() != 0)
    {
        samplingRate = max(param.toInt(), 250L);
    }
    else
    {
        stream.println(samplingRate);
    }
}

void runLogCommand(String param, Stream &stream)
{
    if (param.length() != 0)
    {
        if (logFileName != param)
        {
            logFileName = param;
            storage.removeFile(logFileName);
        }
    }
    else
    {
        stream.println(logFileName);
    }
}

void runBreakCommand(String param, Stream &stream)
{
    logFileName = "";
}

void runDirCommand(String param, Stream &stream)
{
    storage.printFileNames(stream);
}

void runRetrieveCommand(String param, Stream &stream)
{
    if (param.length() != 0)
    {
        storage.readFile(param, stream);
    }
}

void runDeleteCommand(String param, Stream &stream)
{
    if (param.length() != 0)
    {
        storage.removeFile(param);
        if (param == logFileName)
        {
            logFileName = "";
        }
    }
}

void runVerboseCommand(String param, Stream &stream)
{
    if (param.length() != 0)
    {
        verboseMode = param.toInt() != 0;
    }
    else
    {
        stream.println(verboseMode);
    }
}

Command commands[] =
    {
        {"val", runValueCommand},
        {"unt", runUnitCommand},
        {"smp", runSamplingCommand},
        {"log", runLogCommand},
        {"brk", runBreakCommand},
        {"dir", runDirCommand},
        {"ret", runRetrieveCommand},
        {"del", runDeleteCommand},
        {"vrb", runVerboseCommand},
};

void runCommand(Stream &stream)
{
    if (!stream.available())
    {
        return;
    }

    String line = stream.readStringUntil('\n');
    String name = line.substring(0, 3);
    name.toLowerCase();
    String param = line.substring(3);
    param.trim();

    for (int i = 0; i < sizeof(commands) / sizeof(Command); ++i)
    {
        if (commands[i].name == name)
        {
            stream.println(line);
            commands[i].handler(param, stream);
            return;
        }
    }
    stream.println("error");
}
