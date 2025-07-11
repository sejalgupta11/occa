#include <occa.hpp>
#include <occa/internal/utils/testing.hpp>

#define ASSERT_SAME_SIZE(a, b) ASSERT_EQ((size_t)(a), (size_t)(b))


void testReserve();
void testVoid();
void testTotal(const std::string mode); 
void testFree(const std::string mode);
void testSerial(); 
static void display_mallinfo(void);
int testMallinfo();

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>


static void
display_mallinfo(void)
{
    struct mallinfo mi;

    mi = mallinfo();


    std::cout << "Total non-mmapped bytes (arena):       " << mi.arena   << '\n'
          << "# of free chunks (ordblks):            " << mi.ordblks << '\n'
          << "# of free fastbin blocks (smblks):     " << mi.smblks  << '\n'
          << "# of mapped regions (hblks):           " << mi.hblks   << '\n'
          << "Bytes in mapped regions (hblkhd):      " << mi.hblkhd  << '\n'
          << "Max. total allocated space (usmblks):  " << mi.usmblks << '\n'
          << "Free bytes held in fastbins (fsmblks): " << mi.fsmblks << '\n'
          << "Total allocated space (uordblks):      " << mi.uordblks<< '\n'
          << "Total free space (fordblks):           " << mi.fordblks<< '\n'
          << "Topmost releasable block (keepcost):   " << mi.keepcost<< std::endl;

}

int testMallinfo() // to see if calling mallinfo here gives 0's
{
#define MAX_ALLOCS 2000000
    char *alloc[MAX_ALLOCS];
    size_t blockSize, numBlocks, freeBegin, freeEnd, freeStep;

   

    numBlocks = 20000;
    blockSize = 150;
    freeStep =  1;
    freeBegin =  0;
    freeEnd = numBlocks;

    std::cout << "============== Before allocating blocks ==============\n";
    display_mallinfo();

    for (size_t j = 0; j < numBlocks; j++) {
        if (numBlocks >= MAX_ALLOCS) {
            std::cerr << "Too many allocations\n";
            exit(EXIT_FAILURE);
        }

        alloc[j] = (char*)malloc(blockSize);
        if (alloc[j] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }

    std::cout << "\n============== After allocating blocks ==============\n";
    display_mallinfo();

    for (size_t j = freeBegin; j < freeEnd; j += freeStep)
        free(alloc[j]);

    std::cout << "\n============== After freeing blocks ==============\n";
    display_mallinfo();

}


int main(const int argc, const char **argv) {

  testMallinfo(); 

  testSerial(); 


  //existing tests 
  testReserve();
  testVoid();

  std::vector<std::string> modes = {
    "Serial",
    // "OpenMP",
    // "CUDA",
    // "HIP", 
    // "OpenCL",
    // "Metal",
    "SYCL",
  };
// use cmake(?) to configure/automatically detect the hardware to choose which tests to run? 
  for (const auto &mode : modes) {
    std::cout << "Testing mode: " << mode << std::endl;
    testTotal(mode);
    testFree(mode); 
  }
 
  return 0;
}


void testSerial(){

  int num_to_allocate = 9999; 
  float *data = new float[num_to_allocate];
  float *test = new float[num_to_allocate];
  for (int i = 0; i < num_to_allocate; ++i) {
    data[i] = i;
  }

  occa::device device({
    {"mode", "Serial"}
  });

  occa::memoryPool mempool = device.createMemoryPool();

  // /*Set aligment to 5*sizeof(float) bytes*/
  // memPool.setAlignment(5 * sizeof(float));

  // /*Set a size for the memoryPool*/
  // memPool.resize(10 * sizeof(float));


  /*Make a reservation*/
  occa::memory mem = mempool.reserve<float>(num_to_allocate);
  std::cout << "Allocated memory" << std::endl;
  mem.copyFrom(data);
  

  
  auto totalmem = mempool.totalDeviceMemory();
  auto freemem = mempool.freeDeviceMemory(); 
  std::cout << "Total Memory: " << totalmem << std::endl;
  std::cout << "Free Memory: " << freemem << std::endl;

  ASSERT_TRUE(totalmem > 1 );
  ASSERT_TRUE(freemem > 1 ); 

  // ASSERT_SAME_SIZE(totalmem, freemem); 

  // allocate some amount of memory 
  occa::udim_t allocatedSize=100*sizeof(float); 
  // allocate 100 floats 


  // ASSERT_SAME_SIZE(totalmem, freemem+allocatedSize);
}




//------------------------------------------------------------------
void testTotal(const std::string mode) {


 unsigned int allocated_memory = 500*sizeof(int); 

  occa::device device({
    {"mode", mode}
  });
  occa::memoryPool memPool = device.createMemoryPool();

  occa::udim_t dmem = memPool.totalDeviceMemory(); // make sure this doesn't throw an error at at minimum returns a number back 
  ASSERT_TRUE(dmem > 8);




// test that total memory is within ballpark of ____ (figure out a good metric)




// want to test that total memory is at least the same after many allocations 

  /*Set aligment to 5*sizeof(int) bytes*/
  memPool.setAlignment(5 * sizeof(int));

  /*Set a size for the memoryPool*/
  memPool.resize(10 * sizeof(int));

  ASSERT_SAME_SIZE(device.memoryAllocated(), 10 * sizeof(int));
  ASSERT_SAME_SIZE(memPool.size(), 10 * sizeof(int));
  ASSERT_SAME_SIZE(memPool.reserved(), 0);

  occa::memory mem = memPool.reserve<int>(10);
  ASSERT_SAME_SIZE(device.memoryAllocated(), 10 * sizeof(int));
  ASSERT_SAME_SIZE(memPool.size(), 10 * sizeof(int));
  ASSERT_SAME_SIZE(memPool.reserved(), 10 * sizeof(int));

  mem.free();
  
  ASSERT_SAME_SIZE(device.memoryAllocated(), allocated_memory);
  

}


void testFree(const std::string mode) {

  unsigned int allocated_memory = 500*sizeof(int); 

  occa::device device({
    {"mode", mode}
  });
  occa::memoryPool memPool = device.createMemoryPool();

  occa::udim_t freemem = memPool.freeDeviceMemory(); // make sure this doesn't throw an error 
  ASSERT_TRUE(freemem > 8);

}


void testReserve() {


  float *data = new float[30];
  float *test = new float[30];
  for (int i = 0; i < 30; ++i) {
    data[i] = i;
  }

  occa::device device({
    {"mode", "Serial"}
  });

  occa::memoryPool memPool = device.createMemoryPool();

  /*Set aligment to 5*sizeof(float) bytes*/
  memPool.setAlignment(5 * sizeof(float));

  /*Set a size for the memoryPool*/
  memPool.resize(10 * sizeof(float));

  ASSERT_SAME_SIZE(device.memoryAllocated(), 10 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.size(), 10 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.reserved(), 0);

  /*Make a reservation*/
  occa::memory mem = memPool.reserve<float>(10);
  mem.copyFrom(data);

  ASSERT_SAME_SIZE(device.memoryAllocated(), 10 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.size(), 10 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.reserved(), 10 * sizeof(float));
  {
    /*Test slicing mem in memoryPool. Should not trigger reallocation or
      increase in memoryPool's reservation size*/
    occa::memory half1 = mem.slice(0, 5);
    occa::memory half2 = mem.slice(5);

    ASSERT_SAME_SIZE(device.memoryAllocated(), 10 * sizeof(float));
    ASSERT_SAME_SIZE(memPool.size(), 10 * sizeof(float));
    ASSERT_SAME_SIZE(memPool.reserved(), 10 * sizeof(float));

    half1.copyTo(test);
    for (int i = 0; i < 5; ++i) {
      ASSERT_EQ(static_cast<int>(test[i]), i);
    }
    half2.copyTo(test);
    for (int i = 0; i < 5; ++i) {
      ASSERT_EQ(static_cast<int>(test[i]), i+5);
    }

    /*Trigger resize*/
    occa::memory mem2 = memPool.reserve<float>(10);

    ASSERT_SAME_SIZE(device.memoryAllocated(), 20 * sizeof(float));
    ASSERT_SAME_SIZE(memPool.size(), 20 * sizeof(float));
    ASSERT_SAME_SIZE(memPool.reserved(), 20 * sizeof(float));

    mem.copyTo(test);
    for (int i = 0; i < 10; ++i) {
      ASSERT_EQ(static_cast<int>(test[i]), i);
    }
    half1.copyTo(test);
    for (int i = 0; i < 5; ++i) {
      ASSERT_EQ(static_cast<int>(test[i]), i);
    }
    half2.copyTo(test);
    for (int i = 0; i < 5; ++i) {
      ASSERT_EQ(static_cast<int>(test[i]), i+5);
    }
  }

  /*Delete buffers, memoryPool size does not change, but reservation is smaller*/
  ASSERT_SAME_SIZE(device.memoryAllocated(), 20 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.size(), 20 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.reserved(), 10 * sizeof(float));

  /*Reserve again, should not trigger a resize*/
  occa::memory mem2 = memPool.reserve<float>(10);
  mem2.copyFrom(data+10);

  ASSERT_SAME_SIZE(device.memoryAllocated(), 20 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.size(), 20 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.reserved(), 20 * sizeof(float));

  /*Trigger resize*/
  occa::memory mem3 = memPool.reserve<float>(5);
  occa::memory mem4 = memPool.reserve<float>(5);
  mem3.copyFrom(data+20);
  mem4.copyFrom(data+25);

  ASSERT_SAME_SIZE(device.memoryAllocated(), 30 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.size(), 30 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.reserved(), 30 * sizeof(float));

  mem2.copyTo(test);
  for (int i = 0; i < 10; ++i) {
    ASSERT_EQ(static_cast<int>(test[i]), i+10);
  }

  /*Delete mem and mem3 to make gaps*/
  mem.free();
  mem3.free();
  ASSERT_SAME_SIZE(device.memoryAllocated(), 30 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.size(), 30 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.reserved(), 15 * sizeof(float));

  /*Trigger a resize again, which shifts mem2 and mem4 */
  mem = memPool.reserve<float>(20);
  ASSERT_SAME_SIZE(device.memoryAllocated(), 35 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.size(), 35 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.reserved(), 35 * sizeof(float));

  mem2.copyTo(test);
  for (int i = 0; i < 10; ++i) {
    ASSERT_EQ(static_cast<int>(test[i]), i+10);
  }
  mem4.copyTo(test);
  for (int i = 0; i < 5; ++i) {
    ASSERT_EQ(static_cast<int>(test[i]), i+25);
  }

  /*Manually free mem2*/
  mem2.free();
  ASSERT_SAME_SIZE(device.memoryAllocated(), 35 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.size(), 35 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.reserved(), 25 * sizeof(float));

  /*Shrink pool to fit*/
  memPool.resize(memPool.reserved());
  ASSERT_SAME_SIZE(device.memoryAllocated(), 25 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.size(), 25 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.reserved(), 25 * sizeof(float));

  mem4.copyTo(test);
  for (int i = 0; i < 5; ++i) {
    ASSERT_EQ(static_cast<int>(test[i]), i+25);
  }

  mem4.free();
  ASSERT_SAME_SIZE(device.memoryAllocated(), 25 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.size(), 25 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.reserved(), 20 * sizeof(float));

  mem.free();
  ASSERT_SAME_SIZE(device.memoryAllocated(), 25 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.size(), 25 * sizeof(float));
  ASSERT_SAME_SIZE(memPool.reserved(), 0 * sizeof(float));

  memPool.free();
  ASSERT_SAME_SIZE(device.memoryAllocated(), 0);

  delete[] test;
  delete[] data;
}

void testVoid() {

  occa::device device({{"mode", "Serial"}});
  occa::memoryPool memory_pool = device.createMemoryPool();

  const int size = 10;
  occa::memory memory = memory_pool.reserve(10);
  ASSERT_SAME_SIZE(memory.size(), size);
}



// #include <occa.hpp>
// #include <occa/internal/utils/testing.hpp>

// void testReserve();
// void testVoid();
// void testFree2();
// void testTotal2();

// int main(const int argc, const char **argv)
// {
//   testReserve();
//   testVoid();
//   testFree();
//   testTotal();

//   return 0;
// }

// void testFree2()
// {
//   // #define ASSERT_SAME_SIZE(a, b) \
//   // ASSERT_EQ((size_t)(a), (size_t)(b))

//   int amount_to_test = 8300;
//   float *data = new float[amount_to_test];
//   // for (int i = 0; i < amount_to_test; ++i)
//   // {
//   //   data[i] = i;
//   // }
//   std::vector<std::string> modes = {"Serial", "OpenMP", "CUDA", "HIP", "OpenCL", "Metal"};
//   for (const auto &mode : modes)
//   {
//     occa::device device({{"mode", mode}});
//     occa::memoryPool memPool = device.createMemoryPool();
//     memPool.setAlignment(5 * sizeof(float));
//     memPool.resize(10 * sizeof(float));

//     ASSERT_SAME_SIZE(device.memoryAllocated(), 10 * sizeof(float));
//     ASSERT_SAME_SIZE(memPool.size(), 10 * sizeof(float));
//     ASSERT_SAME_SIZE(memPool.reserved(), 0);

//     occa::memory mem = memPool.reserve<float>(10);
//     mem.copyFrom(data);

//     ASSERT_SAME_SIZE(device.memoryAllocated(), 10 * sizeof(float));
//     ASSERT_SAME_SIZE(memPool.size(), 10 * sizeof(float));
//     ASSERT_SAME_SIZE(memPool.reserved(), 10 * sizeof(float));

//     mem.free();
//     ASSERT_SAME_SIZE(device.memoryAllocated(), 0);
//     ASSERT_SAME_SIZE(memPool.size(), 0);
//     ASSERT_SAME_SIZE(memPool.reserved(), 0);
//   }
//   occa::device device({{"mode", "Serial"}});
// }

// void testTotal()
// {
// }

// void testReserve()
// {
// #define ASSERT_SAME_SIZE(a, b) \
//   ASSERT_EQ((size_t)(a), (size_t)(b))

//   float *data = new float[30];
//   float *test = new float[30];
//   for (int i = 0; i < 30; ++i)
//   {
//     data[i] = i;
//   }

//   occa::device device({{"mode", "Serial"}});

//   occa::memoryPool memPool = device.createMemoryPool();

//   /*Set aligment to 5*sizeof(float) bytes*/
//   memPool.setAlignment(5 * sizeof(float));

//   /*Set a size for the memoryPool*/
//   memPool.resize(10 * sizeof(float));

//   ASSERT_SAME_SIZE(device.memoryAllocated(), 10 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.size(), 10 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.reserved(), 0);

//   /*Make a reservation*/
//   occa::memory mem = memPool.reserve<float>(10);
//   mem.copyFrom(data);

//   ASSERT_SAME_SIZE(device.memoryAllocated(), 10 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.size(), 10 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.reserved(), 10 * sizeof(float));
//   {
//     /*Test slicing mem in memoryPool. Should not trigger reallocation or
//       increase in memoryPool's reservation size*/
//     occa::memory half1 = mem.slice(0, 5);
//     occa::memory half2 = mem.slice(5);

//     ASSERT_SAME_SIZE(device.memoryAllocated(), 10 * sizeof(float));
//     ASSERT_SAME_SIZE(memPool.size(), 10 * sizeof(float));
//     ASSERT_SAME_SIZE(memPool.reserved(), 10 * sizeof(float));

//     half1.copyTo(test);
//     for (int i = 0; i < 5; ++i)
//     {
//       ASSERT_EQ(static_cast<int>(test[i]), i);
//     }
//     half2.copyTo(test);
//     for (int i = 0; i < 5; ++i)
//     {
//       ASSERT_EQ(static_cast<int>(test[i]), i + 5);
//     }

//     /*Trigger resize*/
//     occa::memory mem2 = memPool.reserve<float>(10);

//     ASSERT_SAME_SIZE(device.memoryAllocated(), 20 * sizeof(float));
//     ASSERT_SAME_SIZE(memPool.size(), 20 * sizeof(float));
//     ASSERT_SAME_SIZE(memPool.reserved(), 20 * sizeof(float));

//     mem.copyTo(test);
//     for (int i = 0; i < 10; ++i)
//     {
//       ASSERT_EQ(static_cast<int>(test[i]), i);
//     }
//     half1.copyTo(test);
//     for (int i = 0; i < 5; ++i)
//     {
//       ASSERT_EQ(static_cast<int>(test[i]), i);
//     }
//     half2.copyTo(test);
//     for (int i = 0; i < 5; ++i)
//     {
//       ASSERT_EQ(static_cast<int>(test[i]), i + 5);
//     }
//   }

//   /*Delete buffers, memoryPool size does not change, but reservation is smaller*/
//   ASSERT_SAME_SIZE(device.memoryAllocated(), 20 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.size(), 20 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.reserved(), 10 * sizeof(float));

//   /*Reserve again, should not trigger a resize*/
//   occa::memory mem2 = memPool.reserve<float>(10);
//   mem2.copyFrom(data + 10);

//   ASSERT_SAME_SIZE(device.memoryAllocated(), 20 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.size(), 20 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.reserved(), 20 * sizeof(float));

//   /*Trigger resize*/
//   occa::memory mem3 = memPool.reserve<float>(5);
//   occa::memory mem4 = memPool.reserve<float>(5);
//   mem3.copyFrom(data + 20);
//   mem4.copyFrom(data + 25);

//   ASSERT_SAME_SIZE(device.memoryAllocated(), 30 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.size(), 30 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.reserved(), 30 * sizeof(float));

//   mem2.copyTo(test);
//   for (int i = 0; i < 10; ++i)
//   {
//     ASSERT_EQ(static_cast<int>(test[i]), i + 10);
//   }

//   /*Delete mem and mem3 to make gaps*/
//   mem.free();
//   mem3.free();
//   ASSERT_SAME_SIZE(device.memoryAllocated(), 30 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.size(), 30 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.reserved(), 15 * sizeof(float));

//   /*Trigger a resize again, which shifts mem2 and mem4 */
//   mem = memPool.reserve<float>(20);
//   ASSERT_SAME_SIZE(device.memoryAllocated(), 35 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.size(), 35 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.reserved(), 35 * sizeof(float));

//   mem2.copyTo(test);
//   for (int i = 0; i < 10; ++i)
//   {
//     ASSERT_EQ(static_cast<int>(test[i]), i + 10);
//   }
//   mem4.copyTo(test);
//   for (int i = 0; i < 5; ++i)
//   {
//     ASSERT_EQ(static_cast<int>(test[i]), i + 25);
//   }

//   /*Manually free mem2*/
//   mem2.free();
//   ASSERT_SAME_SIZE(device.memoryAllocated(), 35 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.size(), 35 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.reserved(), 25 * sizeof(float));

//   /*Shrink pool to fit*/
//   memPool.resize(memPool.reserved());
//   ASSERT_SAME_SIZE(device.memoryAllocated(), 25 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.size(), 25 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.reserved(), 25 * sizeof(float));

//   mem4.copyTo(test);
//   for (int i = 0; i < 5; ++i)
//   {
//     ASSERT_EQ(static_cast<int>(test[i]), i + 25);
//   }

//   mem4.free();
//   ASSERT_SAME_SIZE(device.memoryAllocated(), 25 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.size(), 25 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.reserved(), 20 * sizeof(float));

//   mem.free();
//   ASSERT_SAME_SIZE(device.memoryAllocated(), 25 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.size(), 25 * sizeof(float));
//   ASSERT_SAME_SIZE(memPool.reserved(), 0 * sizeof(float));

//   memPool.free();
//   ASSERT_SAME_SIZE(device.memoryAllocated(), 0);

//   delete[] test;
//   delete[] data;
// }

// void testVoid()
// {
// #define ASSERT_SAME_SIZE(a, b) \
//   ASSERT_EQ((size_t)(a), (size_t)(b))

//   occa::device device({{"mode", "Serial"}});
//   occa::memoryPool memory_pool = device.createMemoryPool();

//   const int size = 10;
//   occa::memory memory = memory_pool.reserve(10);
//   ASSERT_SAME_SIZE(memory.size(), size);
// }

