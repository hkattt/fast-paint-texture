#include <iostream>

//#define DEBUG

#ifdef DEBUG
    #define DEBUG_PRINT(message) \
        std::cout << message << std::endl; 
#else 
    #define DEBUG_PRINT(message) // Empty macro
#endif