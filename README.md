# Halotools

Halotools is a specialized python package for building and testing models of the 
galaxy-halo connection, and analyzing catalogs of dark matter halos. 
The core functionality of the package includes:

* Fast generation of synthetic galaxy populations using HODs, abundance matching, and related methods
* Efficient algorithms for calculating galaxy clustering, lensing, z-space distortions, and other astronomical statistics 
* A modular, object-oriented framework for designing your own galaxy evolution model
* End-to-end support for downloading publicly-available halo catalogs and reducing them to fast-loading hdf5 files

The code is publicly available at https://github.com/astropy/halotools. 

---

## Documentation

The latest build of the documentation can be found at http://halotools.readthedocs.org. The docs includes installation instructions, quickstart guides and step-by-step tutorials. 

---
## Basic features

Once you have installed the code and downloaded the default halo catalog (see the Getting Started guide in the documentation), you can use Halotools models to populate mock galaxy populations.

``` 
from halotools.empirical_models import PrebuiltHodModelFactory 
model = PrebuiltHodModelFactory('zheng07')
model.populate_mock(simname = 'bolshoi')
```
The pre-built model factories give you a wide range of models to choose from. Alternatively, you can use the Halotools factories to design a customized model of your own creation. Either way, any Halotools model can populate any Halotools-formatted halo catalog with the above syntax. 

---

## Project status

[![Coverage Status](https://coveralls.io/repos/astropy/halotools/badge.svg?branch=master&service=github)](https://coveralls.io/github/astropy/halotools?branch=master)

Halotools is a fully open-source package with contributing scientists spread across many universities. The package will soon be ready for beta release. If you are willing to serve as a beta-tester, or if you just wish to use Halotools before the first official release, please contact andrew.hearin@yale.edu.

---

## License 

Halotools is licensed under a 3-clause BSD style license - see the licenses/LICENSE.rst file.

