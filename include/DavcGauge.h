#include "SerialGauge.h"

class DavcGauge : SerialGauge
{
    String m_pressureUnit = "Torr";
    static String m_pressureUnits[];

public:
    DavcGauge(uint8_t uart, uint8_t rxPin, uint8_t txPin) : SerialGauge(uart, rxPin, txPin) {}
    void setup() { SerialGauge::setup(9600); }
    String getPressureUnit() { return m_pressureUnit; }
    void setPressureUnit(String pressureUnit);
    double readValue();

private:
    static int findPressureUnit(String pressureUnit);
};
