#include <iostream>
#include "Serial.h"

using namespace std;

int main(int argc, char const *argv[])
{
    if(argc < 2){
        cout << "enter the gener next time" << endl;
        exit(EXIT_FAILURE);
    }
    Serial* serial = new Serial(argv[1]);
    serial->run();    
    return 0;
}
