# twes
Just supports GET method and transmits static page. 
### Prerequisites
+ [CMake](http://www.cmake.org "CMake project page") (>= 3.8)
+ [GCC](http://gcc.gnu.org "GCC home") (>= 5.4.0)
### Building
```bash
mkdir build && cd build
cmake .. && cmake --build .
```
### Usage
```bash
./twes -p [port] /path/html/files
```
for help
```bash
./twes -h
Usage: ./twes -p [port] path/html/files
Options: -d [run as daemon]
         -v [print full log]

```
