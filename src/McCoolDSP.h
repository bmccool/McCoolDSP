#pragma once

#include <boost/circular_buffer.hpp>

#define PI 3.141592653589793238462643383279502884197169399375105820974944

#ifdef WIN32
    #define McCoolDSP_EXPORT __declspec(dllexport)
#else
    #define McCoolDSP_EXPORT
#endif

McCoolDSP_EXPORT void McCoolDSP();


class HPS_Frequency_Detector {
    private:

    public:
        unsigned int window_size, num_products;
        // Create a circular buffer with a capacity for 3 integers.
        boost::circular_buffer<int> samples;
        HPS_Frequency_Detector(unsigned int window_size, unsigned int num_products): window_size(window_size), num_products(num_products), samples(boost::circular_buffer<int>(window_size)){}
        void add_sample(int sample){
            samples.push_back(sample);
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

            // Step 0 - Precheck
            if (samples.size() < window_size){
                return 0; // There are not enough samples to run the algorithm!
            }

            // Step 1 - Skip if the signal power is too low
            // TODO

            // Step 2 - Multiply by Hanning Window
            // TODO

            // Step 3 - Apply DFT
            // TODO

            // Step 4 - Suppres mains hum
            // TODO

            // Step 5 - Remove noise from each frequency band
            // TODO

            // Step 6 - Interpolate Smoothly
            // TODO

            // Step 7 - Normalize
            // TODO

            // Step 8 - Multiply the harmonic spectrums
            // TODO

            // Step 9 - Get Max Index
            // TODO

            // Step 10 - Translate Index to Frequency
            // TODO

            return 0;
        }


};
