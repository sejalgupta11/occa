#include <iostream>

#include <occa.hpp>


int main(int argc, const char **argv) {

 

    int entries = 32; 

    
    // Decalre and initialize the device 
    occa::device device;

    device.setup({
        {"mode", "Serial"} // these are the defaults 
    });

    // The default stream
    occa::stream streamA = occa::getStream(); 
    
    // Another, new stream
    occa::stream streamB = occa::createStream(); 

    occa::memory o_a  = occa::malloc<float>(entries);
    occa::memory o_b  = occa::malloc<float>(entries);
    occa::memory o_ab = occa::malloc<float>(entries);

    occa::kernel addVectors = occa::buildKernel("addVectors.okl",
                                    "addVectors");

    // Pass this property to make copies non-blocking on the host.
    occa::json async_copy({{"async", true}});

    // These copies will be submitted to the current
    // stream, which is streamA--the default stream.
    o_a.copyFrom(a,async_copy);
    o_b.copyFrom(b,async_copy);
  

  // Free device -- allocated memory, built kernels, created streams and streamTags
  device.free(); 

  // Free host memory
  delete [] a;
  delete [] b;
  delete [] ab;


  return 0;
}
