# gr-ysf
GNU Radio modules for decoding Yaesu System Fusion C4FM packets

See the following link for information on the project:

http://hb9uf.github.io/gr-ysf/

## Prerequisites

In addition to GNU Radio and gr-osmosdr, you need a C++ compiler, cmake, swig
and mbelib to successfully compile this project. The code was tested with g++
4.8.4, cmake 2.8.12.2, GNU Radio 3.7.5, swig 2.0.11 and mbelib 1.2.5.

```
sudo apt install libboost-system-dev libboost-filesystem-dev libboost-dev
```

## Compiling
Compiling the project is straight forward and similar to other GNU Radio out of
tree modules. Run the following inside the project root directory:

```
mkdir build
cd build
cmake ..
make
```

Then as root, run the following inside the `build` directory that you created
and entered in the previous step:

```
make install
ldconfig
```

You should now be able to load and run `examples/ysf_rx.grc` in
`gnuradio-companion`.
