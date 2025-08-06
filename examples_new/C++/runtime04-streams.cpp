#include <iostream>
#include <occa.hpp>

int main(int argc, const char **argv) {
    int chunks = 1; 
    // int chunkSize = 64;
    int chunkSize = 1;
    int entries = chunks * chunkSize;

    std::cout << "Number of entries: " << entries << std::endl; 

    float *a = new float[entries];
    for (int i = 0; i < entries; ++i) {
        a[i] = i+1;
    }

    occa::device device({{"mode", "Serial"}}); 

    occa::stream streamCopy = occa::getStream();
    // occa::stream streamKernel = occa::createStream();

    occa::memory o_chunk[2];
    for (int i = 0; i < 2; ++i)
        o_chunk[i] = occa::malloc<float>(chunkSize);

    float foo[] = {3.0f};
    occa::memory o_results = occa::malloc<float>(chunks);
    o_results.copyFrom(foo,1,0);

    occa::kernel sumReduce = occa::buildKernel("reduction.okl", "reduction");
    // occa::json async_copy({{"async", true}});

    

    // occa::setStream(streamCopy);
    
    o_chunk[0].copyFrom(a, chunkSize, 0);
    
    float temp = 4; 

    

    // //print debug 
    // for (int j=0; j<chunkSize-1; j++){
    //     (o_chunk[0] + j).copyTo(&temp);
    //     std::cout << temp << ' '; 
    // }
    // std::cout << std::endl; 



    for (int i = 0; i < chunks; ++i) {
        int cur = i % 2;
        int next = (i + 1) % 2;
        

        // Copy next chunk for future loop iteration
        if (i < chunks - 1) {
            // occa::setStream(streamCopy);
            // o_chunk[next].copyFrom(a + (i + 1) * chunkSize, chunkSize, 0, async_copy);
            o_chunk[next].copyFrom(a + (i + 1) * chunkSize, chunkSize, 0);
            // //print debug
        // for (int j=0; j<chunkSize; j++){
        //     (o_chunk[next] + j).copyTo(&temp);
        //     std::cout << temp << ' '; 
        // }
        // std::cout << std::endl; 
        }



        // Make sure copy for current chunk is finished before kernel laucnh usinh streamTags
        // occa::setStream(streamCopy);
        occa::streamTag copyDone = occa::tagStream(); // todo explain streamtag more
        

       

        // Switch stream back to kernel stream
        // occa::setStream(streamKernel);
        occa::waitFor(copyDone);

        // Set run dims and launch 
        // sumReduce.setRunDims(chunkSize, 1);
        occa::memory& chunk_0 = o_chunk[0];
        // sumReduce(chunkSize, o_chunk[cur], o_results);
        sumReduce(chunkSize, chunk_0, o_results);

    }

    // Wait for kernel stream to finish all work
    // occa::setStream(streamKernel);
    // streamKernel.finish();

    // Copy results to host
    float *hostResults = new float[chunks];
    hostResults[0] = 1.0f; 
    // o_results.copyFrom(hostResults,1,0);
    o_results.copyTo(hostResults,1,0);

    device.finish();
    // do final reduction 
    float totalSum = 0;
    for (int i = 0; i < chunks; ++i) {
        std::cout << "Reduction for chunk " << i << ": " << hostResults[i] << std::endl; 
		totalSum += hostResults[i];
	}
    std::cout << "Total sum: " << totalSum << "\n";
	std::cout << "Expected value: " << entries * (entries+1) / 2 << std::endl;

    device.free();
    delete[] a;
    delete[] hostResults;
}
