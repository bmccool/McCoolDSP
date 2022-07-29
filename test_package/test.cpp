#include "McCoolDSP.h"
#include <catch2/catch_all.hpp>

TEST_CASE("First Test", "[test]"){
    REQUIRE(1 == 1);
}

TEST_CASE("HSP_Frequency_Detector", "[test]"){
    HSP_Frequency_Detector detector(256, 3);
    REQUIRE(detector.window_size == 256);
    REQUIRE(detector.cbuff.size() == 0);
    REQUIRE(detector.cbuff.reserve() == 256);
    REQUIRE(detector.num_products == 3);
}

TEST_CASE("HSP_Frequency_Detector::cbuff", "[test]"){
    HSP_Frequency_Detector detector(10, 3);
    REQUIRE(detector.cbuff.size() == 0);
    REQUIRE(detector.cbuff.reserve() == 10);
    for(int i = 0; i < 15; i++){
        detector.cbuff.push_back(i);
    }
    REQUIRE(detector.cbuff.size() == 10);
    REQUIRE(detector.cbuff.reserve() == 0);
    REQUIRE(detector.cbuff[0] == 5);
    REQUIRE(detector.cbuff[1] == 6);
    REQUIRE(detector.cbuff[2] == 7);
    REQUIRE(detector.cbuff[3] == 8);
    REQUIRE(detector.cbuff[4] == 9);
    REQUIRE(detector.cbuff[5] == 10);
    REQUIRE(detector.cbuff[6] == 11);
    REQUIRE(detector.cbuff[7] == 12);
    REQUIRE(detector.cbuff[8] == 13);
    REQUIRE(detector.cbuff[9] == 14);
}