# cef-pdf

`cef-pdf` is a command line utility for creating PDF documents from HTML content. It uses Google Chrome browser's [Chromium Embedded Framework (CEF)](https://bitbucket.org/chromiumembedded/cef/overview) library for all it's internal work; loading urls, rendering HTML & CSS pages and printing directly to (excellent quality) PDF files.

### Usage:

    cef-pdf [options] [input] [output]

Options:

    --help -h            This help screen.
    --paper-size=<size>  Size (format) of the paper: A3, B2.. Default is A4.
    --landscape          Wheather to print with a landscape page orientation. Default is portrait

Input:

    URL to load, may be http, file, data, anything supported by Chrome.
    Special url stdin:// scheme is used for inputing html from standard input. Default is stdin://get

Output:

    PDF file name. Standard output is not supported. Defaults to output.pdf

### Installation

`cef-pdf` should compile with cmake/ninja on Windows. Linux and OS X needs some work (contributors welcome)

### TODO

 - Linux and OS X versions
 - Client-server version

### License

Flow is licensed under the MIT license.
