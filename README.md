imgcat
======

[![Build Status](https://travis-ci.org/eddieantonio/imgcat.svg?branch=master)](https://travis-ci.org/eddieantonio/imgcat)
[![Build Status](https://github.com/eddieantonio/imgcat/workflows/Build%20and%20test%20imgcat/badge.svg\?branch\=master)](https://github.com/eddieantonio/imgcat/actions)

It's like `cat` but for images.

<p align="center">
  <img src="https://github.com/eddieantonio/i/raw/master/imgcat.png" alt="running `imgcat cat.jpg`" width=506 height=620>
</p>


Install
-------

On macOS:

```sh
brew install eddieantonio/eddieantonio/imgcat
```

**Important**: [See below](#note-on-iterm2s-shell-integration) if
you're using iTerm2 3.0 with shell integration installed.

For other platforms, see [Build](#build).


Usage
-----

```sh
imgcat some_image.jpg
```

See the [manpage](./docs/imgcat.1.md) for more invocations.

Looking for python integration?    
Checkout https://pypi.org/project/imgcat/

Note on iTerm2's shell integration
----------------------------------

If you install iTerm2's [shell integration], chances are you also
installed its additional scripts, including one called `imgcat`.
**iTerm's `imgcat` overrides this program by default**. To see if this
is happening to you, use `which` to determine where your shell is
finding imgcat:

    which imgcat

If it says `imgcat: aliased to /Users/yourusername/.iterm2/imgcat`, then
you must edit your shell startup file and add `unalias imgcat` **after**
the line that sources iTerm2's script. For example:

    test -e ${HOME}/.iterm2_shell_integration.zsh && source ${HOME}/.iterm2_shell_integration.zsh
    unalias imgcat

[shell integration]: https://www.iterm2.com/documentation-shell-integration.html

Build
-----

### Clone

You must use `git clone --recurse-submodules` to clone this repository with its dependency, [CImg]:

    git clone --recurse-submodules https://github.com/eddieantonio/imgcat.git


### Requirements

 - libncurses5-dev
 - GNU make

Then:

```sh
./configure
make
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
 - 256 Color chart and data from Jason Milkin's [public domain chart][256svg].

[CImg]: https://github.com/dtschump/CImg
[CeCILL-C]: http://www.cecill.info/licences/Licence_CeCILL-C_V1-en.txt
[256svg]: https://gist.github.com/jasonm23/2868981

License
-------

Copyright © 2014–2020 Eddie Antonio Santos.
Distributed under the terms of the [ISC license](./LICENSE).
