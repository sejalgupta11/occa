#include <cstring>
#include <occa/internal/modes/serial/device.hpp>
#include <occa/internal/modes/serial/buffer.hpp>
#include <occa/internal/modes/serial/memory.hpp>
#include <occa/internal/modes/serial/memoryPool.hpp>
#include <occa/internal/utils/sys.hpp>

#include <malloc.h>
#include <stdlib.h>
#include <sys/resource.h>

namespace occa {
  namespace serial {
    memoryPool::memoryPool(modeDevice_t *modeDevice_,
                           const occa::json &properties_) :
      occa::modeMemoryPool_t(modeDevice_, properties_) {
        // struct mallinfo mi = ::mallinfo(); 
        // const udim_t starting_memory = mi.uordblks; //memory allocated at the start of memoryPool
      }

    modeBuffer_t* memoryPool::makeBuffer() {
      return new serial::buffer(modeDevice, 0, properties);
    }

    modeMemory_t* memoryPool::slice(const dim_t offset,
                                    const udim_t bytes) {
      return new serial::memory(this, bytes, offset);
    }

    void memoryPool::setPtr(modeMemory_t* mem, modeBuffer_t* buf,
                            const dim_t offset) {
      mem->offset = offset;
      mem->ptr = buf->ptr + offset;
    }

    void memoryPool::memcpy(modeBuffer_t* dst, const dim_t dstOffset,
                            modeBuffer_t* src, const dim_t srcOffset,
                            const udim_t bytes) {
      ::memcpy(dst->ptr + dstOffset,
               src->ptr + srcOffset,
               bytes);
    }


    // https://www.man7.org/linux/man-pages/man3/mallinfo.3.html
    udim_t memoryPool::freeDeviceMemory() const{
      // struct mallinfo result = ::mallinfo(); 
      // size_t freeMem = result.fordblks; // total free space, bytes
      struct rusage result;
      getrusage(RUSAGE_CHILDREN, &result);
      // return totalDeviceMemory() - static_cast<udim_t>(result.ru_maxrss) * 1000; // ru_maxrss is in kb
      return static_cast<udim_t>(result.ru_maxrss) * 1000; 
    }

    udim_t memoryPool::totalDeviceMemory() const{
      struct mallinfo result = ::mallinfo(); 
      auto freeMem = result.fordblks; // total free space, bytes
      auto allocatedMem = result.uordblks; //total allocated space (includes other programs too)
      return static_cast<udim_t>(freeMem + allocatedMem); 
      // struct rusage result;
      // getrusage(RUSAGE_SELF, &result);
      // return static_cast<udim_t>(result.);
    }
  }
}
