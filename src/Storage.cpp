#include <Arduino.h>
#include "Storage.h"
#include "globals.h"

void Storage::setup()
{
    m_spi.begin();

    if (m_sd.begin(m_sdCsPin, SD_SCK_MHZ(25)))
    {
        Serial.println("SD card mounted");
    }
    else
    {
        Serial.println("SD card failed to mount");
    }
}

void Storage::traceLog(String fileName, uint64_t timestamp, String text)
{
    File file = m_sd.open(m_rootPath + fileName, O_RDWR | O_CREAT | O_AT_END);
    if (file)
    {
        String prefix = timestampToString(timestamp) + ",";
        file.print(prefix);
        file.println(text);
        file.close();

        if (verboseMode)
        {
            Serial.print(prefix);
            Serial.println(text);
        }
    }
}

String Storage::timestampToString(uint64_t timestamp)
{
    int seconds = timestamp / 1000;
    int minutes = seconds / 60;
    int hours = minutes / 60;
    return hmsToString(hours) + ":" + hmsToString(minutes % 60) + ":" + hmsToString(seconds % 60);
}

String Storage::hmsToString(int value)
{
    String text(value);
    if (value < 10)
    {
        text = "0" + text;
    }
    return text;
}

void Storage::printFileNames(Stream &outputStream)
{
    File dir = m_sd.open(m_rootPath);
    if (dir)
    {
        while (true)
        {
            File file = dir.openNextFile();
            if (file)
            {
                if (!file.isDirectory())
                {
                    char name[64];
                    file.getName(name, sizeof(name));
                    outputStream.println(name);
                }
                file.close();
            }
            else
            {
                break;
            }
        }
        dir.close();
    }
}

void Storage::readFile(String fileName, Stream &outputStream)
{
    File file = m_sd.open(m_rootPath + fileName);
    if (file)
    {
        if (!file.isDirectory())
        {
            while (file.available())
            {
                outputStream.write(file.read());
            }
        }
        file.close();
    }
}

void Storage::removeFile(String fileName)
{
    File file = m_sd.open(m_rootPath + fileName);
    if (file)
    {
        if (!file.isDirectory())
        {
            m_sd.remove(m_rootPath + fileName);
        }
        file.close();
    }
}
