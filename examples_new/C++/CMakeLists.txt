if (NOT TARGET OCCA::libocca)
  find_package(OCCA REQUIRED)
endif()

function(add_occa_cpp_example example_name)
  add_executable(${example_name} ${example_name}.cpp)
  target_link_libraries(${example_name} OCCA::libocca)
endfunction()

add_occa_cpp_example(00_device_selection)
add_occa_cpp_example(01_memory_management)
add_occa_cpp_example(02_kernels)


add_occa_cpp_example(runtime01-devices)
add_occa_cpp_example(runtime02-memory)
add_occa_cpp_example(runtime03-kernels)
add_occa_cpp_example(runtime04-jitting)
add_occa_cpp_example(runtime05-streams) 
add_occa_cpp_example(runtime06-memoryPools)
add_occa_cpp_example(run-kernel-examples)
