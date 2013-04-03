=======================================================================
T-PoT - Total Commander file system plug-in for iPod and iPhone devices
=======================================================================

1. Introduction
---------------

This file only contains information to build the binaries from the sources. 
Read the Publish\README.txt for information about this application and how 
to use it, as well as trademarks and copyright statements.

The Licences directory contains the copyrights and licences of the 
libraries used in this application.


2. External libraries
---------------------

T-PoT relies on zlib library (1.2.3) to translate Apple-modifed PNG 
format to PNG-compliant images.

The include and libraries are included in the Zlib directory.
Zlib is compiled as a static library, linked with the static C library (LIBCMT).
This is to avoid problems discussed in DLL_FAQ.txt of Zlib sources
(included in zlib-1.2.3-src.zip).

The QuickTime SDK is also necessary to transform Apple-proprietary PLIST 
format to regular xml files. The version used for T-PoT is 7.3.0.70, it can 
be downloaded from Apple website (an account is necessary).

- visit https://connect.apple.com
- click on Downloads, then QuickTime
- look for "QuickTime 7.3 SDK for Windows"
- download and install the SDK
- make the QuickTimeSDK environment variable point to the SDK
  installation directory


3. Compiling the application
----------------------------

Solution files are provided for Visual Studio 2003 and 2005:

* T-PoT_2003.sln (Visual Studio 2003)
* T-PoT.sln      (Visual Studio 2005)

Another way to compile a release is to launch the publish.py Python script 
for its directory:

  cd <T-PoT>\Publish
  python publish.py -b -v <version>

To generate a .zip package that will be automatically installed by Total 
Commander, use the same script (both operations can be done at the same 
time):

  python publish.py -i -v <version>

Another solution file for a stand-alone PNG translation tool can be
found in the TranslatePNG directory.


4. Installation and user guide
------------------------------

More information on those topics can be found in the Publish\README.txt 
file or on the project website:

  http://code.google.com/p/t-pot/

Happy coding!


5. Credits
----------

* All the people in general who have worked hard to make the jailbreak 
  possible, without them this kind of plug-in wouldn't make much sense.

* The people involved in the iPHUC and Manzana projects, who explored the 
  iTunesMobileDevice library and extracted most of the API.

* MHW and Guillaume Cottenceau whose code served as a base to convert the 
  PNG images. 
