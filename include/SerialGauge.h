class SerialGauge
{
    HardwareSerial m_serial;
    int m_uart;

public:
    SerialGauge(uint8_t uart, uint8_t rxPin, uint8_t txPin) : m_serial{uart, rxPin, txPin} { m_uart = uart; }
    void setup(int baud);
    void discardInput();
    String write(String command);
};
