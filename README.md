Mazer WASM
==========

Mazer rewritten in WASM.


Compiling
---------

You need to install and setup `emscripten`, https://emscripten.org/docs/getting_started/downloads.html. 

Then use `cmake`
```bash
mkdir build
cd build
emcmake cmake .. && make
```


Running
-------

Host a webserver from directory `./public`.


Debugging
---------

Make sure to configure with `DEBUG` option.

```bash
emcmake cmake -DDEBUG=on ..
make
```

`Chrome DevTools -> Sources -> Filesystem -> Add folder to workspace -> [Add the root of this repo]`


Resources
---------

- https://webassembly.org/getting-started/developers-guide/
