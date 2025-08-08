#include <iostream>

#include <occa.hpp>

#define exampleDtype float 
#define numPerThread (12 / sizeof(exampleDtype)) // idk a good way of choosing these 


int main(int argc, const char **argv) {


    // Set up information on host memory 
    int entries = 12;

    exampleDtype alpha = 3.14f; 

    exampleDtype *a  = new exampleDtype[entries];
    exampleDtype *b  = new exampleDtype[entries];
    exampleDtype *ans = new exampleDtype[entries];

    for (int i = 0; i < entries; ++i) {
        a[i]  = i/alpha;
        b[i]  = 1-i;
        ans[i] = a[i]*alpha + b[i];//1.f;
    }



    // initialize device and allocate memory
    occa::device device({{"mode", "Serial"}});

    
    occa::memory o_a, o_b, o_ans;

    o_a = device.malloc<exampleDtype>(entries);
    o_b = device.malloc<exampleDtype>(entries);
    o_ans = device.malloc<exampleDtype>(entries);

    o_a.copyFrom(a);
    o_b.copyFrom(b);

    

    // Pass data type and number of items per thread at compile time 
    occa::properties axpyProps;
    axpyProps["defines/exampleDtype"] = "float"; //std::to_string(exampleDtype);
    axpyProps["defines/numPerThread"] = std::to_string(numPerThread);

    // Compile at runtime
    occa::kernel axpy = device.buildKernel("axpy_defines.okl", "axpy", axpyProps);


    // Launch device kernel
    axpy(entries, alpha, o_a, o_b, o_ans);

    // Copy result to the host
    o_ans.copyTo(ans);

    // Verify results
    for (int i = 0; i < entries; ++i) {
        if (alpha * a[i] + b[i] != ans[i]) { // don't do this!
            std::cerr << "Error at index " << i << ": "
                      << "Expected " << (alpha * a[i] + b[i]) 
                      << ", got " << ans[i] << std::endl;
        }
    }

    // Free device -- allocated memory, built kernels, created streams and streamTags
    device.free(); 

    // Free host memory
    delete [] a;
    delete [] b;
    delete [] ans;


    return 0;
}

