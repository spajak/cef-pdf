Changelog
===
### 0.3.1 (2017-07-29)
* Building with CEF 3.3112.1649.g0dba8a1 / Chromium 60.0.3112.78 (Windows, Linux & OS X, 64bit)
* Better and faster concurrent requests handling
* Full Unicode file names support on Windows (Linux & OS X already have it)
* Code fixes and optimisations

### 0.3.0 (2017-06-18)
* Building with CEF 3.3071.1640.g1ebbf3c / Chromium 59.0.3071.82 (Windows, Linux & OS X, 64bit)
* Now possible to build on Mac OS X. Tested on 10.11.6 with XCode ver. 8.2.0. Experimental
* Bug fixes. Code cleanup and better formatting

### 0.2.1 (2016-12-20)
* Bulding with CEF 3.2883.1540.gedbfb20 / Chromium 55.0.2883.59 (Windows & Linux, 64bit)
* Allow only specified schemes when in server mode (ie. block `file://` access)
* Added option to enable JavaScript `--javascript`
* Added `--file` option to load file from command line
* Disabled standard input (use `--file` instead)
* Added `--background` option to print html backgrounds

### 0.2.0 (2016-11-29)
* Bulding with CEF 3.2883.1539.gd7f087e / Chromium 55.0.2883.59 (Windows & Linux, 64bit)
* Added HTTP server mode (use with `--server` flag)
* Added configurable `--host` and `--port` options when running in server mode
* Various fixes and improvements

### 0.1.6 (2016-09-21)
* Small changes to build using CEF 3.2743.1449.g90ba67d build
* Small improvements to allow building on Linux (tested on Debian 8.5.0 x64)

### 0.1.5 (2016-09-15)
* Added option to set page margin `--margin`
* Added option to list available page sizes `--list-sizes`
* Changed url to be specified with `--url` option
* Heavy code refractoring

### 0.1.1 (2016-08-06)
* Added some more paper sizes: US, ANSI and photo
* Added help message
* Fixed url load error handling

### 0.1.0 (2016-08-03)
* Initial release. Working version :)
