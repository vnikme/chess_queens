# chess_queens

Depends on abseil if decided to use with absl_flat_hash_map. To build it (see https://abseil.io/docs/cpp/quickstart-cmake):
vnik@vnik-laptop:~/work$ git clone https://github.com/abseil/abseil-cpp.git
vnik@vnik-laptop:~/work$ cd abseil-cpp/
vnik@vnik-laptop:~/work/abseil-cpp$ mkdir build && cd build
vnik@vnik-laptop:~/work/abseil-cpp/build$ cmake .. -DABSL_RUN_TESTS=ON -DABSL_USE_GOOGLETEST_HEAD=ON -DCMAKE_CXX_STANDARD=11
vnik@vnik-laptop:~/work/abseil-cpp/build$ cmake --build . --target all
vnik@vnik-laptop:~/work/abseil-cpp/build$ ctest


