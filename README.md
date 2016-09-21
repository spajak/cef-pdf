# cef-pdf

`cef-pdf` is a command line utility for creating PDF documents from HTML content. It uses Google Chrome browser's [Chromium Embedded Framework (CEF)](https://bitbucket.org/chromiumembedded/cef/overview) library for all it's internal work; loading urls, rendering HTML & CSS pages and printing directly to (excellent quality) PDF files.

### Usage:

    cef-pdf [options] [--url=<input>] [output]

    Options:
      --help -h          This help screen.
      --url=<input>      URL to load, may be http, file, data, anything supported by Chromium.
                         If omitted standard input is read.
      --size=<size>      Size (format) of the paper: A3, B2.. or custom <width>x<height> in mm.
                         A4 is the default.
      --list-sizes       Show all defined page sizes.
      --landscape        Wheather to print with a landscape page orientation.
                         Default is portrait
      --margin=<margin>  Paper margins in mm (much like CSS margin but without units)
                         If omitted default margin is applied.

    Output:
      PDF file name to create. Default is output.pdf

### Building

In order to build, [CEF build distribution files](http://opensource.spotify.com/cefbuilds/index.html) must be placed in cef/ subdirectory. `cef-pdf` should compile without problems with cmake/ninja on Windows 7+ and Linux (tested on Debian 8.5.0 x64), using decent C++11 compiler. Mac OS X needs some work (contributors welcome).

### TODO

 - Mac OS X versions
 - Client-server version

### License

`cef-pdf` is licensed under the MIT license.
