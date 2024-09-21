# loglib_benchmarks
C++ logging library benchmarks.

## Build, install
Build/install dependencies:
[https://github.com/denesderhan/logbench](https://github.com/denesderhan/logbench)

[https://baical.net/p7.html](https://baical.net/p7.html)
[https://github.com/abseil/abseil-cpp](https://github.com/abseil/abseil-cpp)
[https://github.com/apache/logging-log4cxx](https://github.com/apache/logging-log4cxx)
[https://github.com/boostorg/boost](https://github.com/boostorg/boost)
[https://github.com/denesderhan/fstlog](https://github.com/denesderhan/fstlog)
[https://github.com/gabime/spdlog](https://github.com/gabime/spdlog)
[https://github.com/KjellKod/g3log](https://github.com/KjellKod/g3log)
[https://github.com/odygrd/quill](https://github.com/odygrd/quill)
[https://github.com/pocoproject/poco](https://github.com/pocoproject/poco)

### Linux
~~~
git clone https://github.com/denesderhan/loglib_benchmarks
cd loglib_benchmarks && mkdir build && cd build
cmake ..
sudo make
~~~

### Windows (Visual Studio)
Download, extract the project.
Open a Visual Studio Developer command prompt
~~~
cd [your_path_to]/loglib_benchmarks
mkdir build
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
~~~

Compiled files will be in /build/bin directory.

## Usage
Run benchmarks with the benchmarking library:
https://github.com/denesderhan/logbench
