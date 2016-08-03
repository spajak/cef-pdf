# cef-pdf

`cef-pdf` is a command line utility for creating PDF documents from HTML content. It uses Google Chrome browser's [Chromium Embedded Framework (CEF)](https://bitbucket.org/chromiumembedded/cef/overview) framework for all it's internal work; loading urls, rendering HTML & CSS  pages and printing directly to (excellent quality) PDF files.

### Usage:

    cef-pdf [options] [input] [output]

options:

    --paper-size <size>  Size (format) of the paper: A3, B2.. Default is A4.
    --landscape          Wheather to print with a landscape page orientation. Default is portrait

input:

    Input URL, may be http, file, data, anything supported by Chrome. Special stdin:// scheme is used for inputing html from standard input. Default is stdin://get

output:

    PDF file name. Default is output.pdf

### Installation

`cef-pdf` should compile with cmake/ninja on Windows, Linux and OS X.

### TODO

 - Client-server version
