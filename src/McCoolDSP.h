#pragma once

#include <boost/circular_buffer.hpp>

#ifdef WIN32
    #define McCoolDSP_EXPORT __declspec(dllexport)
#else
    #define McCoolDSP_EXPORT
#endif

McCoolDSP_EXPORT void McCoolDSP();

class HSP_Frequency_Detector {
    private:

    public:
        unsigned int window_size, num_products;
        // Create a circular buffer with a capacity for 3 integers.
        boost::circular_buffer<int> cbuff;
        HSP_Frequency_Detector(unsigned int window_size, unsigned int num_products): window_size(window_size), num_products(num_products), cbuff(boost::circular_buffer<int>(window_size)){}


};
