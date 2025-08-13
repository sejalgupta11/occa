#include <iostream>
#include <fstream>

#include <occa.hpp>
// void mandelbrot(const int xMax,
//                                  const int yMax,
//                                  const float xMin,
//                                  const float yMin,
//                                  const int maxIter,
//                                  const float dx,
//                                  const float dy,
//                                  int *iters); 

int main(int argc, const char **argv) {
    int numElems = 25 * 25; 
    float* A = new float[numElems];
    for (int i=0; i<numElems; ++i) {
        A[i] = i + 1.f; 
    }
    float* B = new float[numElems];


    occa::device device({{"mode", "Serial"}});

    occa::memory o_A = device.malloc<float>(numElems);
    occa::memory o_B = device.malloc<float>(numElems);
    o_A.copyFrom(A);

    //todo device should be able to query what maximum block size is 

    std::cout << "Running example kernel01" << std::endl;
    // occa::kernel intro = device.buildKernel("kernel01-intro.okl","fillKernel");
    // intro(numElems, o_B, 0.f);

    occa::kernel loops = device.buildKernel("kernel02-loops.okl", "mandelbrot");
    
    // set up for mandelbrot function

    float xMin = -2.0f;
    float xMax = 1.0f;
    float yMin = -1.0f;
    float yMax = 1.0f;
    int maxIter = 2000;
    //picture resolution
    int numX = 800;
    int numY = 400;
    std::cout <<  "Running example kernel03" << std::endl;

    occa::memory iters = device.malloc<int>(numX * numY * 5);// for trying to avoid the segfualt
    // loops(xMax, yMax, xMin, yMin, maxIter, numX, numY, iters);

    std::cout << "finished Running example kernel03" << std::endl;


    int* itersHost = new int[numX * numY * 5];
    iters.copyTo(itersHost);
    std::ofstream outFile("mandelbrot.txt");
    for (int i = 0; i < numX * numY; ++i) {
        outFile << itersHost[i] << ",";
        if (i % numX == 0) {
            outFile << std::endl;
        }
    }
    outFile.close(); 

    occa::kernel sync = device.buildKernel("kernel03-sync.okl", "reverseVector");
    occa::kernel atomics = device.buildKernel("kernel05-atomics.okl", "reduction");
    occa::kernel math = device.buildKernel("kernel06-math.okl", "mathExample");

    device.free(); 
    
    return 0;
}