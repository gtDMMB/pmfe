## Synopsis

iB4e-GTFold-parametrizer determines the sensitivity of RNA folding to multibranch-loop-related parameters.

## Installation

This project is under active development, so we recommend downloading it using Git.
To do this, run the following in your terminal:

```
git clone https://github.com/AMS-MRC-disc-math-bio/iB4e-GTfold-parametrizer.git
```

This will download the code and extract it into a directory called `iB4e-GTfold-parameterizer`.

### Dependencies
This project depends on the [CGAL][cgal] computational geometry library, several libraries from the [Boost][boost] project, and the [GMP][gmp] arbitrary-precision arithmetic library.
To install the required dependencies on a Debian system, run

    sudo apt-get install libgmp-dev libboost-filesystem-dev libboost-program-options-dev libcgal-dev


This project also depends on the [NLTemplate] string templating library.
It has been included here under the terms of the MIT license.


### Building the project code
Next, you will need to build our custom version of `GTFold`.
To do so, simply run `make` from the `iB4e-GTfold-parametrizer` directory.
If you have multiple cores or processors, you can build in parallel by running

    nice make -j

instead.

## Updating

If you used Git to download your copy of this software, you can update it easily.
Just run `git pull` in a terminal from anywhere inside the repository to fetch the latest version.

## Usage

Given a FASTA file representing an RNA sequence, the program will produce a Sage file representing the regions of ℝ<sup>3</sup> in which given structures are optimal.

To run the calculation on the sequence in `testseq.fasta`, type

    ./parametrizer testseq.fasta

The result will be a file `testseq.polytope.sage` containing the required Sage commands and a directory called `testseq` containing structure files representing the MFE structures for each set of parameters.

We also supply a program which can be used to find an MFE structure for a single set of parameters.
To use it on the sequence in `testseq.fasta` with parameters `A`, `B`, `C`, and `D`, type

    ./gtmfe-param testseq.fasta -a A -b B -c C -d D

The result will be printed to your terminal.

For more information about either program, run it with the `-h` option.

## License

The source for iB4e-GTfold-parametrizer is released under the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

[macports]: //www.macports.org/
[openmp]: http://openmp.org/
[opemmp-dl]: http://openmp.org/wp/openmp-compilers/
[gmp]: //gmplib.org/
[gmp-dl]: //gmplib.org/#DOWNLOAD
[sage]: //sagemath.org
[cgal]: //www.cgal.org
[boost]: //www.boost.org
[boost-getstarted]: //www.boost.org/doc/libs/1_57_0/more/getting_started/unix-variants.html
[cmake]: //www.cmake.org/download/
[NLTemplate]: //github.com/catnapgames/NLTemplate
