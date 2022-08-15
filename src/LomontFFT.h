// Code to implement decently performing FFT for complex and real valued
// signals. See www.lomont.org for a derivation of the relevant algorithms 
// from first principles. Copyright Chris Lomont 2010-2022.
// This code and any ports are free for all to use for any reason as long 
// as this header is left in place.
// Version 1.1, Sept 2011
#include <iostream>
#include <vector>
#define PI 3.141592653589793238462643383279502884197169399375105820974944


class LomontFFT {
    private:
        std::vector<double> cosTable;
        std::vector<double> sinTable;

    public:
        unsigned int window_size;
        int A = 0;
        int B = 1;
        //LomontFFT():{} Empty, use compiler default constructor

        /// <summary>
        /// Compute the forward or inverse Fourier Transform of data, with 
        /// data containing complex valued data as alternating real and 
        /// imaginary parts. The length must be a power of 2. The data is 
        /// modified in place.
        /// </summary>
        /// <param name="data">The complex data stored as alternating real 
        /// and imaginary parts</param>
        /// <param name="forward">true for a forward transform, false for 
        /// inverse transform</param>
        void FFT(std::vector<double>& data, bool forward)
        {
            auto n = data.size();
            // checks n is a power of 2 in 2's complement format
            if ((n & (n - 1)) != 0) 
                //throw new ArgumentException(
                std::cout << "data length " << n << " in FFT is not a power of 2" << std::endl;
            n /= 2;    // n is the number of samples

            Reverse(data, n); // bit index data reversal

            // do transform: so single point transforms, then doubles, etc.
            double sign = forward ? B : -B;
            auto mmax = 1;
            while (n > mmax)
            {
                auto istep = 2 * mmax;
                auto theta = sign * PI / mmax;
                double wr = 1, wi = 0;
                auto wpr = cos(theta);
                auto wpi = sin(theta);
                for (auto m = 0; m < istep; m += 2)
                {
                    for (auto k = m; k < 2 * n; k += 2 * istep)
                    {
                        auto j = k + istep;
                        auto tempr = wr * data[j] - wi * data[j + 1];
                        auto tempi = wi * data[j] + wr * data[j + 1];
                        data[j] = data[k] - tempr;
                        data[j + 1] = data[k + 1] - tempi;
                        data[k] = data[k] + tempr;
                        data[k + 1] = data[k + 1] + tempi;
                    }
                    auto t = wr; // trig recurrence
                    wr = wr * wpr - wi * wpi;
                    wi = wi * wpr + t * wpi;
                }
                mmax = istep;
            }

            // perform data scaling as needed
            Scale(data, n, forward);
        }
        
        /// <summary>
        /// Scale data using n samples for forward and inverse transforms as needed 
        /// </summary>
        /// <param name="data"></param>
        /// <param name="n"></param>
        /// <param name="forward"></param>
        void Scale(std::vector<double>& data, int n, bool forward)
        {
            // forward scaling if needed
            if ((forward) && (A != 1))
            {
                auto scale = pow(n, (A - 1) / 2.0);
                for (auto i = 0; i < data.size(); ++i)
                    data[i] *= scale;
            }

            // inverse scaling if needed
            if ((!forward) && (A != -1))
            {
                auto scale = pow(n, -(A + 1) / 2.0);
                for (auto i = 0; i < data.size(); ++i)
                    data[i] *= scale;
            }
        }

        /// <summary>
        /// Call this with the size before using the TableFFT version
        /// Fills in tables for speed. Done automatically in TableFFT
        /// </summary>
        /// <param name="size">The size of the FFT in samples</param>
        void Initialize(int size)
        {
            // Set tables to correct size
            cosTable.resize(size);
            sinTable.resize(size);

            // forward pass
            auto n = size;
            int mmax = 1, pos = 0;
            while (n > mmax)
            {
                auto istep = 2 * mmax;
                auto theta = PI / mmax;
                double wr = 1, wi = 0;
                auto wpi = sin(theta);
                // compute in a slightly slower yet more accurate manner
                auto wpr = sin(theta / 2);
                wpr = -2 * wpr * wpr; 
                for (auto m = 0; m < istep; m += 2)
                {
                    cosTable[pos] = wr;
                    sinTable[pos++] = wi;
                    auto t = wr;
                    wr = wr * wpr - wi * wpi + wr;
                    wi = wi * wpr + t * wpi + wi;
                }
                mmax = istep;
            }
        }
        /// <summary>
        /// Swap data indices whenever index i has binary 
        /// digits reversed from index j, where data is
        /// two doubles per index.
        /// </summary>
        /// <param name="data"></param>
        /// <param name="n"></param>
        static void Reverse(std::vector<double>& data, int n)
        {
            // bit reverse the indices. This is exercise 5 in section 
            // 7.2.1.1 of Knuth's TAOCP the idea is a binary counter 
            // in k and one with bits reversed in j
            int j = 0, k = 0; // Knuth R1: initialize
            auto top = n / 2;  // this is Knuth's 2^(n-1)
            while (true)
            {
                // Knuth R2: swap - swap j+1 and k+2^(n-1), 2 entries each
                auto t = data[j + 2]; 
                data[j + 2] = data[k + n]; 
                data[k + n] = t;
                t = data[j + 3]; 
                data[j + 3] = data[k + n + 1]; 
                data[k + n + 1] = t;
                if (j > k)
                { // swap two more
                    // j and k
                    t = data[j]; 
                    data[j] = data[k]; 
                    data[k] = t;
                    t = data[j + 1]; 
                    data[j + 1] = data[k + 1]; 
                    data[k + 1] = t;
                    // j + top + 1 and k+top + 1
                    t = data[j + n + 2]; 
                    data[j + n + 2] = data[k + n + 2]; 
                    data[k + n + 2] = t;
                    t = data[j + n + 3]; 
                    data[j + n + 3] = data[k + n + 3]; 
                    data[k + n + 3] = t;
                }
                // Knuth R3: advance k
                k += 4;
                if (k >= n)
                    break;
                // Knuth R4: advance j
                auto h = top;
                while (j >= h)
                {
                    j -= h;
                    h /= 2;
                }
                j += h;
            } // bit reverse loop
        }

        /// <summary>
        /// Compute the forward or inverse Fourier Transform of data, with data
        /// containing complex valued data as alternating real and imaginary 
        /// parts. The length must be a power of 2. This method caches values 
        /// and should be slightly faster on than the FFT method for repeated uses. 
        /// It is also slightly more accurate. Data is transformed in place.
        /// </summary>
        /// <param name="data">The complex data stored as alternating real 
        /// and imaginary parts</param>
        /// <param name="forward">true for a forward transform, false for 
        /// inverse transform</param>
        void TableFFT(std::vector<double>& data, bool forward)
        {
            auto n = data.size();
            // checks n is a power of 2 in 2's complement format
            if((n & (n - 1)) != 0){
                //throw new ArgumentException(

            }
            n /= 2;    // n is the number of samples (n/2 samples, with real(n*i) and complex(n*i)+1 parts)

            Reverse(data, n); // bit index data reversal

            // make table if needed
            if (cosTable.size() != n)
                Initialize(n);

            // do transform: so single point transforms, then doubles, etc.
            double sign = forward ? B : -B;
            auto mmax = 1;
            auto tptr = 0;
            while (n > mmax)
            {
                auto istep = 2 * mmax;
                for (auto m = 0; m < istep; m += 2)
                {
                    auto wr = cosTable[tptr];
                    auto wi = sign * sinTable[tptr++];
                    for (auto k = m; k < 2 * n; k += 2 * istep)
                    {
                        auto j = k + istep;
                        auto tempr = wr * data[j] - wi * data[j + 1];
                        auto tempi = wi * data[j] + wr * data[j + 1];
                        data[j] = data[k] - tempr;
                        data[j + 1] = data[k + 1] - tempi;
                        data[k] = data[k] + tempr;
                        data[k + 1] = data[k + 1] + tempi;
                    }
                }
                mmax = istep;
            }


            // perform data scaling as needed
            Scale(data, n, forward);
        }

        /// <summary>
        /// Compute the forward or inverse Fourier Transform of data, with 
        /// data containing real valued data only. The output is complex 
        /// valued after the first two entries, stored in alternating real 
        /// and imaginary parts. The first two returned entries are the real 
        /// parts of the first and last value from the conjugate symmetric 
        /// output, which are necessarily real. The length must be a power 
        /// of 2.
        /// </summary>
        /// <param name="data">The complex data stored as alternating real 
        /// and imaginary parts</param>
        /// <param name="forward">true for a forward transform, false for 
        /// inverse transform</param>
        void RealFFT(std::vector<double>& data, bool forward)
        {
            
            auto n = data.size(); // # of real inputs, 1/2 the complex length
            // checks n is a power of 2 in 2's complement format
            if ((n & (n - 1)) != 0)
                //throw new ArgumentException(
                std::cout << "data length " << n << " in FFT is not a power of 2" << std::endl;
                //    );

            auto sign = -1.0; // assume inverse FFT, this controls how algebra below works
            if (forward)
            { // do packed FFT. This can be changed to FFT to save memory
                TableFFT(data, true);
                sign = 1.0;
                // scaling - divide by scaling for N/2, then mult by scaling for N
                if (A != 1)
                {
                    auto scale = pow(2.0, (A - 1) / 2.0);
                    for (auto i = 0; i < data.size(); ++i)
                        data[i] *= scale;
                }
            }

            auto theta = B * sign * 2 * PI / n;
            auto wpr = cos(theta);
            auto wpi = sin(theta);
            auto wjr = wpr;
            auto wji = wpi;

            for (auto j = 1; j <= n/4; ++j)
            {
                auto k = n / 2 - j;
                auto tkr = data[2 * k];    // real and imaginary parts of t_k  = t_(n/2 - j)
                auto tki = data[2 * k + 1];
                auto tjr = data[2 * j];    // real and imaginary parts of t_j
                auto tji = data[2 * j + 1];

                auto a = (tjr - tkr) * wji;
                auto b = (tji + tki) * wjr;
                auto c = (tjr - tkr) * wjr;
                auto d = (tji + tki) * wji;
                auto e = (tjr + tkr);
                auto f = (tji - tki);

                // compute entry y[j]
                data[2 * j] = 0.5 * (e + sign * (a + b));
                data[2 * j + 1] = 0.5 * (f + sign * (d - c));

                // compute entry y[k]
                data[2 * k] = 0.5 * (e - sign * (b + a));
                data[2 * k + 1] = 0.5 * (sign * (d - c) - f);

                auto temp = wjr;
                // todo - allow more accurate version here? make option?
                wjr = wjr * wpr - wji * wpi;  
                wji = temp * wpi + wji * wpr;
            }

            if (forward)
            {
                // compute final y0 and y_{N/2}, store in data[0], data[1]
                auto temp = data[0];
                data[0] += data[1];
                data[1] = temp - data[1];
            }
            else
            {
                auto temp = data[0]; // unpack the y0 and y_{N/2}, then invert FFT
                data[0] = 0.5 * (temp + data[1]);
                data[1] = 0.5 * (temp - data[1]);
                // do packed inverse (table based) FFT. This can be changed to regular inverse FFT to save memory
                TableFFT(data, false);
                // scaling - divide by scaling for N, then mult by scaling for N/2
                //if (A != -1) // todo - off by factor of 2? this works, but something seems weird
                {
                    auto scale = pow(2.0, -(A + 1) / 2.0)*2; 
                    for (auto i = 0; i < data.size(); ++i)
                        data[i] *= scale;
                }
            }
        }
};
