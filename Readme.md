## Static wpad server
A small, simple (naïve) http server whose only purpose is serving a static wpad.dat file

[![Travis CI status](https://travis-ci.org/Chocobo1/static-wpad-server.svg?branch=master)](https://travis-ci.org/Chocobo1/static-wpad-server)


## Library Dependencies
[GNU Libmicrohttpd](https://www.gnu.org/software/libmicrohttpd/)

(optional) configured with the following options:

CFLAGS='-O2 -mtune=native -fstack-protector-strong --param=ssp-buffer-size=4' LDFLAGS='-Wl,-O1,--sort-common,--as-needed,-z,relro' ./configure --disable-https --disable-spdy --disable-bauth --disable-dauth --disable-postprocessor --disable-doc --disable-examples --disable-curl --enable-static --disable-shared --prefix=$(pwd)/install


## Compiler Requirements
Must support C++


## License
This program is licensed under [GPLv3](https://www.gnu.org/licenses/gpl.txt)
