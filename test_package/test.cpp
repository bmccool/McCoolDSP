#include "McCoolDSP.h"
#include "LomontFFT.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <iostream>

#define CHECK_VEC_EQUAL(x, y) \
    REQUIRE(x.size() == y.size()); \
    for (size_t i = 0; i < x.size(); ++i) { \
            if (x[i] != Catch::Approx(y[i])) { \
                    REQUIRE(x[i] == Catch::Approx(y[i])); \
            } \
    }

TEST_CASE("First Test", "[test]"){
    REQUIRE(1 == 1);
}

TEST_CASE("HPS_Frequency_Detector", "[test]"){
    HPS_Frequency_Detector detector(256, 3);
    REQUIRE(detector.window_size == 256);
    REQUIRE(detector.samples.empty() == true);
    REQUIRE(detector.samples.size() == 0);
    //REQUIRE(detector.samples.reserve() == 256);
    REQUIRE(detector.num_products == 3);
}

TEST_CASE("HPS_Frequency_Detector::samples", "[test]"){
    HPS_Frequency_Detector detector(10, 3);
    REQUIRE(detector.samples.size() == 0);
    //REQUIRE(detector.samples.reserve() == 10);
    //for(int i = 0; i < 15; i++){
    //    detector.add_sample(i);
    //}
    REQUIRE(detector.samples.size() == 0);
    //REQUIRE(detector.samples.reserve() == 0);
    //REQUIRE(detector.samples[0] == 5);
    //REQUIRE(detector.samples[1] == 6);
    //REQUIRE(detector.samples[2] == 7);
    //REQUIRE(detector.samples[3] == 8);
    //REQUIRE(detector.samples[4] == 9);
    //REQUIRE(detector.samples[5] == 10);
    //REQUIRE(detector.samples[6] == 11);
    //REQUIRE(detector.samples[7] == 12);
    //REQUIRE(detector.samples[8] == 13);
    //REQUIRE(detector.samples[9] == 14);
}

TEST_CASE("HPS_Frequency_Detector::detect"){
    int bagpipe3300 [] = {323, 493, 466, 586, 360, 509, 403, 396, 654, 249, 600, 508, 410, 452, 412, 427, 305, 552, 612, 36,
                          424, 751, 230, 415, 483, 523, 342, 728, 336, 370, 539, 480, 588, 310, 456, 420, 366, 640, 192, 604,
                          589, 395, 405, 483, 361, 395, 553, 571, 29,  439, 726, 217, 411, 465, 555, 383, 747, 301, 404, 575, 
                          477, 587, 302, 428, 430, 330, 644, 189, 596, 634, 375, 394, 547, 313, 444, 544, 540, 52,  440, 679, 
                          214, 414, 470, 594, 379, 754, 314, 420, 605, 455, 556, 325, 411, 426, 305, 631, 202, 589, 648, 354, 
                          385, 588, 295, 454, 545, 520, 86,  435, 659, 218, 421, 495, 626, 349, 752, 335, 430, 620, 433, 527, 
                          363, 411, 420, 306, 610, 213, 599, 650, 348, 374, 618, 300, 448, 556, 509, 102, 430, 663, 221, 426, 
                          511, 639, 312, 747, 344, 431, 623, 409, 504, 380, 410, 414, 313, 609, 226, 599, 636, 358, 369, 600,
                          333, 438, 555, 510, 95,  426, 702, 204, 423, 524, 630, 295, 742, 351, 426, 617, 413, 499, 389, 410, 
                          417, 325, 617, 234, 599, 638, 361, 378, 591, 344, 436, 565, 525, 79,  430, 719, 196, 423, 525, 615, 
                          294, 735, 350, 419, 618, 413, 505, 383, 410, 418, 337, 622, 241, 577, 640, 367, 374, 583, 339, 427, 
                          570, 520, 67,  428, 726, 188, 422, 525, 591, 301, 732, 345, 412, 615, 427, 510, 376, 416, 420, 349, 
                          617, 242, 555, 652, 373, 379, 574, 336, 424, 580, 522, 63,  427, 733, 200, 421};

    // Create HPS_Frequency_Detector with window size 256, 3 products
    HPS_Frequency_Detector detector(256, 3);

    // Verify Size is 0 and buffer has 256 available
    REQUIRE(detector.samples.size() == 0);
    //REQUIRE(detector.samples.reserve() == 256);

    for(int i = 0; i < 256; i++){
        detector.add_sample(bagpipe3300[i]);
        // Verify frequency detected as 0 if buffer isn't full
        if(i == 254){
            REQUIRE(detector.detect() == 0);
        }
    }

    // Verify Buffer is full
    REQUIRE(detector.samples.size() == 256);
    //REQUIRE(detector.samples.reserve() == 0);

    // This sample set should give frequency 455.5
    //REQUIRE(detector.detect() == 455.5);
}

TEST_CASE("HPS_Frequency_Detector::hanning"){
    HPS_Frequency_Detector detector(256, 3);
    for(int i = 0; i < 256; i++){
        detector.add_sample(1000);
    }

    detector.hanning();
    //REQUIRE(detector.samples.reserve() == 0);
    REQUIRE(detector.samples[0] == 0);
    REQUIRE(detector.samples[255] == 0);
}

TEST_CASE("HPS_Frequency_Detector::RealFFT"){
    std::vector<double> data = {323, 493, 466, 586, 360, 509, 403, 396, 654, 249, 600, 508, 410, 452, 412, 427, 305, 552, 612, 36,
                                424, 751, 230, 415, 483, 523, 342, 728, 336, 370, 539, 480, 588, 310, 456, 420, 366, 640, 192, 604,
                                589, 395, 405, 483, 361, 395, 553, 571, 29,  439, 726, 217, 411, 465, 555, 383, 747, 301, 404, 575, 
                                477, 587, 302, 428, 430, 330, 644, 189, 596, 634, 375, 394, 547, 313, 444, 544, 540, 52,  440, 679, 
                                214, 414, 470, 594, 379, 754, 314, 420, 605, 455, 556, 325, 411, 426, 305, 631, 202, 589, 648, 354, 
                                385, 588, 295, 454, 545, 520, 86,  435, 659, 218, 421, 495, 626, 349, 752, 335, 430, 620, 433, 527, 
                                363, 411, 420, 306, 610, 213, 599, 650, 348, 374, 618, 300, 448, 556, 509, 102, 430, 663, 221, 426, 
                                511, 639, 312, 747, 344, 431, 623, 409, 504, 380, 410, 414, 313, 609, 226, 599, 636, 358, 369, 600,
                                333, 438, 555, 510, 95,  426, 702, 204, 423, 524, 630, 295, 742, 351, 426, 617, 413, 499, 389, 410, 
                                417, 325, 617, 234, 599, 638, 361, 378, 591, 344, 436, 565, 525, 79,  430, 719, 196, 423, 525, 615, 
                                294, 735, 350, 419, 618, 413, 505, 383, 410, 418, 337, 622, 241, 577, 640, 367, 374, 583, 339, 427, 
                                570, 520, 67,  428, 726, 188, 422, 525, 591, 301, 732, 345, 412, 615, 427, 510, 376, 416, 420, 349, 
                                617, 242, 555, 652, 373, 379, 574, 336, 424, 580, 522, 63,  427, 733, 200, 421};
    // Create HPS_Frequency_Detector with window size 256, 3 products
    HPS_Frequency_Detector detector(256, 3);

    // Verify Size is 0 and buffer has 256 available
    REQUIRE(detector.samples.size() == 0);
    //REQUIRE(detector.samples.reserve() == 256);

    for(int i = 0; i < 256; i++){
        detector.add_sample(data[i]);
        // Verify frequency detected as 0 if buffer isn't full
        if(i == 254){
            REQUIRE(detector.detect() == 0);
        }
    }

    auto ret_val = detector.detect();
    REQUIRE(ret_val == Catch::Approx(455.5).epsilon(0.01));
}
