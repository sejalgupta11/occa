// #include <iostream>

// #include <occa.hpp>


// int main(int argc, const char **argv) {

 

//     int entries = 32; 

//     // Initialize host memory 
//     float *a  = new float[entries];
//     float *b  = new float[entries];
//     float *ab = new float[entries];

//     for (int i = 0; i < entries; ++i) {
//       a[i]  = i;
//       b[i]  = 1 - i;
//       ab[i] = 0;
//     }


//     // Declare and initialize the device 
//     occa::device device;

//     device.setup({
//         {"mode", "Serial"} // these are the defaults 
//     });
//     // Build the kernel 
//     occa::kernel addVectors = occa::buildKernel("addVectors.okl",
//                                  "addVectors");


//     // The default stream
//     occa::stream stream1 = occa::getStream(); 
    
//     // Another, new stream
//     occa::stream stream2 = occa::createStream(); 

//     occa::memory o_a  = occa::malloc<float>(entries);
//     occa::memory o_b  = occa::malloc<float>(entries);
//     occa::memory o_ab = occa::malloc<float>(entries);

//     // Pass this property to make copies non-blocking on the host.
//     occa::json async_copy({{"async", true}});

//     // Use first stream for data transfer 
//     o_a.copyFrom(a);
//     // when it finishes, launch the kernel in the other stream so that the data transfer and kernel launch can overlap
//     stream1.finish();
//     o_b.copyFrom(b, async_copy);

//     stream2.finish();
//     reductionKernel(o_a, o_b);



     

//     // Pass this property to make copies non-blocking on the host.
//     occa::json async_copy({{"async", true}});

//     // These copies will be submitted to the current
//     // stream, which is streamA--the default stream.
//     o_a.copyFrom(a,async_copy);
//     o_b.copyFrom(b,async_copy);
  

//   // Free device -- allocated memory, built kernels, created streams and streamTags
//   device.free(); 

//   // Free host memory
//   delete [] a;
//   delete [] b;
//   delete [] ab;


//   return 0;
// }
