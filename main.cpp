#include <iostream>
#include "Parallel.h"

using namespace std;

int main(int argc, char const *argv[])
{
    if(argc < 2){
        cout << "enter the gener next time" << endl;
        exit(EXIT_FAILURE);
    }
    Parallel* parallel = new Parallel(argv[1]);
    parallel->run();   
    return 0;
}
