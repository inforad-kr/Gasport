#include <SdFat.h>

class Storage
{
    String m_rootPath;
    SPIClass m_spi;
    SdCsPin_t m_sdCsPin;
    SdFat m_sd;

public:
    Storage(String rootPath, uint32_t sdClkPin, uint32_t sdMisoPin, uint32_t sdMosiPin, uint32_t sdCsPin) : m_spi{sdMosiPin, sdMisoPin, sdClkPin, sdCsPin}
    {
        m_rootPath = rootPath;
        m_sdCsPin = sdCsPin;
    }
    void setup();
    void traceLog(String fileName, uint64_t timestamp, String text);
    void printFileNames(Stream &outputStream);
    void readFile(String fileName, Stream &outputStream);
    void removeFile(String fileName);

private:
    static String timestampToString(uint64_t timestamp);
    static String hmsToString(int value);
};
