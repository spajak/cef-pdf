# cef-pdf

`cef-pdf` is a command line utility (with embedded HTTP server as an optional mode) for creating PDF documents from HTML content. It uses Google Chrome browser's [Chromium Embedded Framework (CEF)](https://bitbucket.org/chromiumembedded/cef/overview) library for all it's internal work; loading urls, rendering HTML & CSS pages and PDF printing, therefore, it produces perfect, accurate, excellent quality PDF documents.

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

    Server options:
      --server         Start HTTP server
      --host=<host>    If starting server, specify ip address to bind to.
                       Default is 127.0.0.1
      --port=<port>    Specify server port number. Default is 9288
      --remote-trigger Defer printing until page evaluates the following:
                       window.cefPdf({request: "", onSuccess: function () {}, onFailure: function () {}});


    Output:
      PDF file name to create. Default is to write binary data to standard output.

### HTTP server usage

Execute `cef-pdf` with `--server` option and visit `localhost:9288` with web browser. Default json response, with status and version number, should indicate the server is up and running on local machine:

    {
        "status": "ok",
        "version": "0.2.0"
    }

To receive a PDF, just make POST request to `localhost:9288/foo.pdf`with some HTML content as the request body. `foo` may be any name you choose, `.pdf` suffix is always required. The response will contain the PDF data, with `application/pdf` as the content type.

In addition to POSTing content inside the request body, special HTTP header `Content-Location` is supported, which should be an URL to some external content. `cef-pdf` will try to grab the content from this URL and use it just like it was the request's body.

### Building

`cef-pdf` should compile without problems with cmake/ninja on Windows (7, x64), Linux (tested on Debian 8.5.0, x64) and Mac OS X (10.11.6) using decent C++11 compiler. In order to build, [CEF build distribution files](http://opensource.spotify.com/cefbuilds/index.html) must be downloaded and placed in some directory, like `/path/to/cef/release` in the example below.

```
$ mkdir ~/build
$ cd ~/build
$ cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCEF_ROOT=/path/to/cef/release /path/to/cef-pdf
$ ninja
```

### License

`cef-pdf` is licensed under the MIT license.
