Compilation The main file. While compiling use "gcc -pthread nproxy.c" argument which will create the executable.

Its a port scanner via a proxy server. Can be used through Tor(Though it has a restriction in case of tor that the final exit server should support 'CONNECT'ing, which most of the Tor servers do).

Currently it support only HTTP proxy. Will add other support as I get time.

Tested on fedora, Ubuntu and cygwin(Windows) .
