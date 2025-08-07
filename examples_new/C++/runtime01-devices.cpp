#include <iostream>

#include <occa.hpp>


int main(int argc, const char **argv) {


    // Declare and initialize the device 
    occa::device cudaDevice;
    cudaDevice.setup({
        {"mode", "CUDA"}, 
        {"device_id", 0} 
    });
    
    // Alternatively, pass the same json into the device constructor  
    // This is the defaults, equivalent to `occa::device device()`; 
    occa::device defaultDevice({{"mode", "Serial"}});

    occa::device openmpSerialDevice({
        {"mode", "OpenMP"},
        {"schedule", "compact"},
        {"chunk", 10}
    });

    // The other support backends are: "HIP", "OpenCL", "Metal", "dpcpp"

    // View device information
    std::cout << "CUDA Device:\n" << cudaDevice.arch() << std::endl;
    std::cout << "\nDefault Device:\n" << defaultDevice.arch() << std::endl;
    std::cout << "\nOpenMP Serial Device:\n" << openmpSerialDevice.arch() << std::endl;

    // Free devices, which deletes allocated memory, built kernels, streams, and streamTags
    cudaDevice.free();
    defaultDevice.free();
    openmpSerialDevice.free();


    return 0;
}