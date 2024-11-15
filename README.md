# How to compile(Client)
``` bash
mkdir build && cd build
qt-cmake ../ -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake
make
```

# How to compile(Server)
``` bash
g++ server.cpp -o server
```

