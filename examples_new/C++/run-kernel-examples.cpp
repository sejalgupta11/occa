#include <iostream>

#include <occa.hpp>


int main(int argc, const char **argv) {



    occa::device device({{"mode", "Serial"}});
    //todo device should be able to query what maximum block size is 
    occa::kernel intro = device.buildKernel("kernel01-intro.okl","fillKernel");
    occa::kernel loops = device.buildKernel("kernel02-loops.okl", "mandelbrot");
    occa::kernel sync = device.buildKernel("kernel03-sync.okl", "reverseVector");
    occa::kernel math = device.buildKernel("kernel06-math.okl", "mathExample")

    device.free(); 
    
    return 0;
}