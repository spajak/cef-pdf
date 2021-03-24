# cef-pdf

`cef-pdf` is a command line utility for creating PDF documents from HTML content. It uses Google Chrome browser's [Chromium Embedded Framework (CEF)](https://bitbucket.org/chromiumembedded/cef/overview) library for all it's internal work; loading urls, rendering HTML & CSS pages and PDF printing, therefore, it produces perfect, accurate and excellent quality PDF documents.

### Usage:

    cef-pdf [options] --url=<url>|--file=<path> [output]

    Options:
      --help -h        This help screen.
      --url=<url>      URL to load, may be http, file, data, anything supported by Chromium.
      --file=<path>    File path to load using file:// scheme. May be relative to current directory.
      --stdin          Read content from standard input until EOF (Unix: Ctrl+D, Windows: Ctrl+Z).
      --size=<spec>    Size (format) of the paper: A3, B2.. or custom <width>x<height> in mm.
                       A4 is the default.
      --list-sizes     Show all defined page sizes.
      --landscape      Wheather to print with a landscape page orientation.
                       Default is portrait.
      --margin=<spec>  Paper margins in mm (much like CSS margin but without units)
                       If omitted some default margin is applied.
      --javascript     Enable JavaScript.
      --backgrounds    Print with backgrounds. Default is without.

    Output:
      PDF file name to create. Default is to write binary data to standard output.

### Building

`cef-pdf` should compile without problems with cmake/ninja on Windows (7, x64), Linux (tested on Debian 8.5.0, x64) and Mac OS X (10.11.6) using decent C++11 compiler. In order to build, [CEF build distribution files](https://cef-builds.spotifycdn.com/index.html) must be downloaded and placed in some directory, like `/path/to/cef/release` in the example below.

```
$ mkdir ~/build
$ cd ~/build
$ cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCEF_ROOT=/path/to/cef/release /path/to/cef-pdf
$ ninja
```

### License

`cef-pdf` is licensed under the MIT license.
