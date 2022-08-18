#pragma once

#include <boost/circular_buffer.hpp>
#include "LomontFFT.h"

#define PI 3.141592653589793238462643383279502884197169399375105820974944
#define WHITE_NOISE_THRESH 0.2

#ifdef WIN32
    #define McCoolDSP_EXPORT __declspec(dllexport)
#else
    #define McCoolDSP_EXPORT
#endif

McCoolDSP_EXPORT void McCoolDSP();


class HPS_Frequency_Detector {
    private:
        LomontFFT LFFT;

    public:
        unsigned unsigned int window_size, num_products;
        // Create a circular buffer with a capacity for 3 integers.
        //boost::circular_buffer<double> samples;
        std::vector<double> samples;

        
        HPS_Frequency_Detector(unsigned int window_size, unsigned int num_products): window_size(window_size), num_products(num_products){ LFFT.Initialize(window_size); samples.reserve(window_size * num_products / 2); }
        void add_sample(int sample){
            samples.push_back(sample);
        }

        void FFT(){
            LFFT.A = 1;
            LFFT.B = 1;
            LFFT.RealFFT(samples, true);
        }

        void MAG(){
            LFFT.mag(samples);
        }

        void hanning(){
            /* hanning
            Our hanning function will create a hanning window as specified by length and apply
            it to the first <length> samples of the buffer.

            The hanning window will be defined for index n as 
            0.5 - 0.5(cos(2*pi*n / (<length> - 1))) 
            */
            const int length = 256; // TODO Can this be dynamic?
            int window[length];
            for(int i = 0; i < length; i++){
                window[i] = 0.5 - 0.5*(cos(2*PI*i / (length - 1)));
                samples[i] = samples[i] * (0.5 - 0.5*(cos(2*PI*i / (length - 1))));
            }
        }

        void normalize(){

        }

        float detect(){
            /* Steps:
            1. Skip if signal power is too low
            2. Multiply by [[Hanning Window]]  to avoid [[spectral leakage]]
            3. Apply [[DFT]]
            4. Suppress mains hum, set everything below 62Hz to 0
	            1. Note DELTA_FREQ (frequency between bins) = SAMPLE_FREQ / WINDOW_SIZE
            5. Calculate average energy per frequency for octave bands and suppress everything below it
	            1. `OCTAVE_BANDS = [50, 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600]`
	            2. E.g. find the bins cooresponding to 50, and 100.  These are start and end indicies
	            3. Find the average energy of this subset of frequencies
	            4. For each frequency in this subset, set it to 0 if it is below the average frequency multiplied by some white_noise_threshold (typically 0.2)
            6. Interpolate the spectrum to prepare for HPS squishing
	            1. Smoothly interpolate the result in order to extend it by x times, where x is the number of products to take (Harmonic **Product** Spectrum)
            7. Normalize the result
            8. Take the Harmonic Product Spectrum X times
	            1. Multiply the result times the spectrum taking every other element, then every third element, every 4th, etc...
		            1. Pad the squished copy with 0s
            9. Find the Max index, the index with the highest value in the result
            10. Find the Max Frequency, as determined by max_index * (SAMPLE_FREQ / WINDOW_SIZE) / NUM_HPS
            */
           std::vector<double> OCTAVE_BANDS = {50, 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600};

            // Step 0 - Precheck
            if (samples.size() < window_size){
                return 0; // There are not enough samples to run the algorithm!
            }

            // Step 1 - Skip if the signal power is too low
            // TODO

            // Step 2 - Multiply by Hanning Window
            hanning();

            // Step 3 - Apply DFT
            FFT();
            // Step 3.5 Combine real/complex parts into one magnitude, reduce size to 1/2
            MAG();

            // Step 4 - Suppres mains hum
            double DELTA_FREQ = 3300 / window_size; // frequency step width of the interpolated DFT
            for(int i = 0; i < (int)(62 / DELTA_FREQ); i++){  // 62 is mains hum
                samples[i] = 0;
            }

            // Step 5 - Remove noise from each frequency band
            // TODO This doesn't EXACTLY match the python version...
            for(int j = 0; j < OCTAVE_BANDS.size() - 1; j++){
                auto ind_start = (int)(OCTAVE_BANDS[j] / DELTA_FREQ);
                auto ind_end = (int)(OCTAVE_BANDS[j+1] / DELTA_FREQ);
                if(ind_end > samples.size()){ ind_end = samples.size(); }
                double norm = 0;
                for(int i = ind_start; i < ind_end; i++){
                    norm += (pow(abs(samples[i]), 2));
                }
                norm = pow(norm, (1./2));
                auto average = pow(norm, 2);
                average /= (ind_end-ind_start);
                average = pow(average, 0.5);
                for(int i = ind_start; i < ind_end; i++){
                    if(samples[i] < (WHITE_NOISE_THRESH * average)){ samples[i] = 0; }
                }
            }

            // Step 6 - Interpolate Smoothly
            int size = window_size/2;
            samples.resize((size * 3) - 2); // TODO magic number HPS products, 2 is because we take off 3 - 1 at the end
            for(int i = size-1; i > 0; i--){
                samples[(i*3) - 0] = samples[i];
                samples[(i*3) - 1] = samples[i] - ((samples[i] - samples[i-1]) / 3);
                samples[(i*3) - 2] = samples[i] - ((samples[i] - samples[i-1])  * 2 / 3);
            }

            // Step 7 - Normalize
            double norm = 0;
            for(int i = 0; i < samples.size(); i++){
                norm += (pow(abs(samples[i]), 2));
            }
            norm = pow(norm, (1./2));
            for(int i = 0; i < samples.size(); i++){
                samples[i] = samples[i] / norm;
            }

            // Step 8 - Multiply the harmonic spectrums
            for(int i = 0; i < samples.size(); i++){
                //for(int j = 0; j < num_products){
                if(i < (samples.size() / num_products)){
                    samples[i] = samples[i] * samples[i*2] * samples[i*3];
                }
                else { samples[i] = 0; }
                //}

            }

            // Step 9 - Get Max Index
            double max_value = 0;
            auto max_index = -1;
            for(int i = 0; i < samples.size(); i++){
                if(samples[i] > max_value){
                    max_value = samples[i];
                    max_index = i;
                }
            }

            // Step 10 - Translate Index to Frequency
            double max_frequency = max_index * (3300.0/window_size) / 3.0;

            return max_frequency;
        }


};
