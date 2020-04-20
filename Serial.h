#ifndef SERIAL_H
#define SERIAL_H
#include <string>

class Serial
{
private:
    std::string gener;
public:
    Serial(const std::string&);
    ~Serial();
    void run();
};

#endif // SERIAL_H