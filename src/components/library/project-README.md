#### [Components](http://sf-github.leap.corp/pages/leapmotion/components/)

This is the general git repository for the Components team at Leap.  Its purpose is to
collect the various pieces of components code that are in development.  

#### Guidelines for development within the Components team:

1. Each component should have a customer, and therefore in principle a well-defined
   feature set.
2. Code in the master branch should be well-commented and have a reasonable amount of
   documentation (Doxygen comments) and/or example code, so that an outsider could 
   understand and use the code quickly and without trouble.

#### Structure of this project

This project has two sections:

1. The Components library, found in the source subdirectory.  There are cmake rules in
   source/CMakeLists.txt defining each component, the components it depends on, and the
   libraries it depends on.
2. The example applications, each found as a subdirectory of the example_apps subdirectory.
   For example, "Stub" is a bare-bones SDL-based application which demonstrates rudimentary
   windowing, GL setup, texture mapping, shader usage, and resource loading.

The Components library and each of the example apps has its own CMakeLists.txt, and is
intended to be a self-contained project.  To build any of these, one should create a
build directory for each one for cmake (cmake 3.0 is required) to generate build files
into.  In particular, building the Components library is separate from building any of
the example applications.  One reason for this organization is so that example applications
are self-sufficient build environments, and can be cloned directly to start a fresh
application.

#### How to develop with this project

This section will describe the steps necessary to use the Components library in your app.
There are two ways this can be accomplished.

1. Via a prepackaged library of the variety that would go in the external libraries directory.
2. By building the Components library yourself and pointing your app's cmake at your
   Components library's build directory.

Option 1 will be what most users of this library will do -- non-Leap developers and Leap
developers who don't need to modify the Components library itself.  Currently this packaged
library is located at \\ocuserv\Common\Libraries-Mac\Components-0.0.20140730.zip (for Mac
only -- someone else needs to make a Windows build), but will eventually be integrated into
the external libraries Libraries-Mac.tar.bz2 scheme.

Option 2 is what Components team developers will do, and those who would like to develop
the library in parallel with their application.  To do this, pull the 'develop' branch of the
components git repo:

```
cd /path/to/your/projects/dir
git clone git@sf-github.leap.corp:leapmotion/components.git
```

Make a separate build directory (e.g. components-build, in the same directory as the components
directory), and run cmake there, pointed at the 'source' subdirectory of the components repo.

```
mkdir components-build
cd components-build
ccmake ../components/source
```

Users of the cmake GUI will do the analogous thing, but with pointy-clicky instead of typey-typey.
Configure the project, specifying 'Debug' for for CMAKE_BUILD_TYPE (if you're developing the 
Components library, you'll want debug symbols) and 'macosx10.8' for CMAKE_OSX_SYSROOT if you're
on Mac.  If you're on Windows, you may need to specify EXTERNAL_LIBRARIES_DIR, after which a
configure step will find all the required libraries.

At this point, you can build the Components library.  There will be a file called
'ComponentsConfig.cmake' in your build directory which allows cmake to correctly include and link
this library in an external app.  When specifying the Components root dir for an external app
(e.g. 'Stub', which is in components/example_apps/Stub), you'll point it at your build directory
and it will find this file.  See the Stub app for example usage.

#### TODOs

- Each cmake project should have its own Doxygen-based documentation.  This documentation can be hosted 
  at [the Components webpage](http://sf-github.leap.corp/pages/leapmotion/components/), the content of 
  which is served from the "gh-pages" branch of this repo.
- Set up [Travis](https://travis-ci.org/) for automated building and unit testing (Jason has set this up, 
  so he could provide more information).
- Put "if(${CMAKE_SYSTEM_NAME} MATCHES \"Darwin") # This is the correct way to detect Mac OS X operating 
  system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
- Put doxygen rules into each example app's CMakeLists.txt -- perhaps also make a cmake-module for defining
  these doxygen rules.

#### Resources on markdown (which is what the .md extension in README.md stands for):

- [github-specific markdown info](https://help.github.com/articles/github-flavored-markdown)
- [general markdown info](https://help.github.com/articles/markdown-basics)
- [live, web-based markdown editor/viewer](http://www.markdownviewer.com/)
