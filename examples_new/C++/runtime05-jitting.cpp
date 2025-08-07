// int main(){
//     return 0 ;
// }

#include <iostream>

#include <occa.hpp>


int main(int argc, const char **argv) {


    // Set up information on host memory 
    int entries = 12;

    float alpha = 3.14f; 

    float *a  = new float[entries];
    float *b  = new float[entries];
    float *ans = new float[entries];

    for (int i = 0; i < entries; ++i) {
        a[i]  = i/alpha;
        b[i]  = 1.f-i;
        ans[i] = a[i]*alpha + b[i];//1.f;
    }



    // initialize device and allocate memory
    occa::device device;

    device.setup({
        {"mode", "Serial"} 
    });
    
    occa::memory o_a, o_b, o_ans;

    o_a = device.malloc<float>(entries);
    o_b = device.malloc<float>(entries);
    o_ans = device.malloc<float>(entries);

    float* fill_device = new float[entries]; 
    for (int i = 0; i < entries; ++i) {
        fill_device[i] = 13.17f;
    }
    o_ans.copyFrom(fill_device, entries);


    // Copy memory to the device
    o_a.copyFrom(a);
    o_b.copyFrom(b);

    

    // Pass value of 'alpha' at kernel compile-time
    occa::properties axpyProps;
    axpyProps["defines/alpha"] = alpha;

    // Compile at runtime
    occa::kernel axpy = device.buildKernel("define_required_axpy.okl", "axpy", axpyProps);

    // Launch device kernel
    axpy.setRunDims(entries, 1); // set the run dimensions
    axpy(entries, o_a, o_b, o_ans);

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

