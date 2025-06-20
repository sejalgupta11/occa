// sejal 
#include <occa.hpp>
#include <occa/internal/utils/testing.hpp>

void testCreateAndSet();
void testUnwrap();

int main(const int argc, const char **argv) {
  testCreateAndSet();
  testUnwrap();

  occa::device unsupported_device({
    {"mode", "not_a_mode"}
  });
  ASSERT_EQ("fake",unsupported_device.getStream().mode());//this should fail 

  ASSERT_TRUE(false);

  return 0;
}

void testCreateAndSet() {
 occa::device occa_device({
    {"mode", "DPCPP"}
  });

  occa::stream occa_stream_1 = occa_device.getStream();
  occa::stream occa_stream_2 = occa_device.createStream();

  ASSERT_EQ("DPCPP",occa_stream_2.mode());
  ASSERT_TRUE(occa_stream_2.getDevice() == occa_device);

  occa_device.setStream(occa_stream_2);

  ASSERT_TRUE(occa_device.getStream() == occa_stream_2);

  occa_device.setStream(occa_stream_1);

  ASSERT_TRUE(occa_device.getStream() == occa_stream_1);
}

void testUnwrap() {
  occa::device occa_device({
    {"mode","DPCPP"}
  });

  occa::stream occa_stream;

  // Unwrapping an uninitialized stream is undefined
  ASSERT_THROW(occa::unwrap(occa_stream););

  occa_stream = occa_device.getStream();

  // Unwrapping a serial mode stream is undefined
  ASSERT_THROW(occa::unwrap(occa_stream););
}
