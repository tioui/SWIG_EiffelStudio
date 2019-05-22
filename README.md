# SWIG_EiffelStudio
An eiffelstudio plugin for SWIG

# Installation:
* Clone SWIG:
```
git clone https://github.com/swig/swig.git
```

* Add `Modules/eiffelstudio.cxx` in the `eswig_SOURCES` section of the
`swig/Source/Makefile.am` file.

* Add `Language *swig_eiffelstudio(void);` in the `extern "C"` section of the swig/Source/Modules/swigmain.cxx file.

* Add `{"-eiffelstudio", swig_eiffelstudio, "EiffelStduio", Supported},` in the `static TargetLanguageModule modules` section of the swig/Source/Modules/swigmain.cxx file.

* Link the `eiffelstudio.cxx` file in the `swig/Source/Modules` directory:
```
ln -s `pwd`/eiffelstudio.cxx swig/Source/Modules/eiffelstudio.cxx
```

* Creating an unused precompilation file `eiffelstudio.swg` in `swig/Lib":
```
touch swig/Lib/eiffelstudio.swg
```

# Compilation
* From the swig directory:
```
./autogen.sh
./configure
make
```

At each modification of the `eiffelstudio.cxx` file, the `make` command should bu used.

# Usage
```
swig/swig -Iswig/Lib -eiffelstudio test_c.i
```
