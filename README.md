imgcat
======

[![Build Status](https://travis-ci.org/eddieantonio/imgcat.svg?branch=master)](https://travis-ci.org/eddieantonio/imgcat)

It's like `cat` but for images.

![$imgcat cat.jpg](./docs/imgcat.png)


Install
-------

On OS X:

```sh
brew tap eddieantonio/eddieantonio
brew install imgcat
```

For other platforms, see [Build](#build).

Usage
-----

```sh
imgcat some_image.jpg
```

See the [manpage](./docs/imgcat.1.md) for more invocations.

Build
-----

```sh
make production=true
```

Install
-------

To install to `/usr/local`:

```sh
make install
```

To change the default prefix, simply provide `PREFIX=...`
to `make install`:

```sh
make install PREFIX=/opt
```

Acknowledgements
----------------

 - Uses the [CImg], distributed under the [CeCILL-C] license.
 - Uses `stb_image` and `stb_image_resize` from [Sean T. Barrett's collection
   of public domain C libraries][stb].
 - 256 Color chart and data from Jason Milkin's [public domain chart][256svg].

[CImg]: https://github.com/dtschump/CImg
[CeCILL-C]: http://www.cecill.info/licences/Licence_CeCILL-C_V1-en.txt
[stb]: https://github.com/nothings/stb
[256svg]: https://gist.github.com/jasonm23/2868981

License
-------

Copyright © 2014, 2016, 2017 Eddie Antonio Santos.
Distributed under the terms of the [ISC license](./LICENSE).
