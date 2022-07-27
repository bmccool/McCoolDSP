#include <iostream>
#include "McCoolDSP.h"

void McCoolDSP(){
    #ifdef NDEBUG
    std::cout << "McCoolDSP/0.1: Hello World Release!" <<std::endl;
    #else
    std::cout << "McCoolDSP/0.1: Hello World Debug!" <<std::endl;
    #endif
}
