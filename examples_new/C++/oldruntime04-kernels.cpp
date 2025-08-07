#include <iostream>
#include <occa.hpp>

int main(int argc, const char **argv) {
    int chunks = 6; 
    int chunkSize = 64;
    int entries = chunks * chunkSize;

    std::cout << "Number of entries: " << entries << std::endl; 

    float *a = new float[entries];
    for (int i = 0; i < entries; ++i) {
        a[i] = i+1;
    }

    occa::device device({{"mode", "Serial"}}); 


    // make the stremas nonblocking so that we can run submit async kernels/operations
    occa::json streamProps({
        {"nonblocking", true},
    });

    occa::stream streamCopy = occa::createStream(streamProps);
    occa::stream streamKernel = occa::createStream(streamProps);

    occa::memory o_chunk[2];
    for (int i = 0; i < 2; ++i)
        o_chunk[i] = occa::malloc<float>(chunkSize);

    occa::memory o_results = occa::malloc<float>(chunks);


    //randomly initialize to something identifiable
    float foo[] = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f}; 
    o_results.copyFrom(foo, 6, 0);

    occa::kernel sumReduce = occa::buildKernel("reduction.okl", "reduction");
    occa::json async_copy({{"async", true}});

    

    occa::setStream(streamCopy);
    
    o_chunk[0].copyFrom(a, chunkSize, 0, async_copy);
    
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
            occa::setStream(streamCopy);
            o_chunk[next].copyFrom(a + (i + 1) * chunkSize, chunkSize, 0, async_copy);
        }

// //print debug
//         for (int j=0; j<chunkSize; j++){
//             (o_chunk[next] + j).copyTo(&temp);
//             std::cout << temp << ' '; 
//         }
//         std::cout << std::endl; 

        // Make sure copy for current chunk is finished before kernel laucnh usinh streamTags
        occa::setStream(streamCopy);
        occa::streamTag copyDone = occa::tagStream(); // todo explain streamtag more
        

        // Launch reduction on the previously copied chunk
        occa::setStream(streamKernel);
        occa::streamWait(streamKernel, copyDone); // only this stream waits for the tag, to make the entire device wait for a tag, use occa::waitFor(tag) todo make this used too? 
        sumReduce.setRunDims(chunkSize, 1);
        sumReduce(chunkSize, o_chunk[cur], o_results+i);

    }

    // Wait for kernel stream to finish all work
    occa::setStream(streamKernel);
    streamKernel.finish();

    // Copy results to host
    float *hostResults = new float[chunks];
    o_results.copyTo(hostResults);

    // do final reduction 
    float totalSum = 0;
    for (int i = 0; i < chunks; ++i) {
        std::cout << "Reduction for chunk " << i << ": " << hostResults[i] << "and foo: " << foo[i] << std::endl; 
		totalSum += hostResults[i];
	}
    std::cout << "==================================\nTotal sum: " << totalSum << "\n";
	std::cout << "Expected value: " << entries * (entries+1) / 2 << std::endl;

    device.free();
    delete[] a;
    delete[] hostResults;
}