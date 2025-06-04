LongBow
=======

small, simple and clean Game Engine and Rendering Framework

![Image of Sponza Scene](https://github.com/ArturSchuetz/LongBow/blob/master/doc/screenshots/Sponza.png?raw=true)

![Image of Sponza Scene with Bump Maps](https://github.com/ArturSchuetz/LongBow/blob/master/doc/screenshots/Sponza_bump_maps.png?raw=true)

## Installation

LongBow ships with prebuilt versions of [GLEW](http://glew.sourceforge.net/) and [GLFW](https://www.glfw.org/) for Windows. To compile the engine yourself you will need **Visual Studio 2015** (or a newer version).

1. Clone this repository:
   ```bash
   git clone https://github.com/ArturSchuetz/LongBow.git
   ```
2. Open `vs12/LongBow.sln` in Visual Studio.
3. Select either the `Win32` or `x64` configuration and build the solution.

## Running the Samples

After a successful build you can run any of the sample projects located in `vs12/Samples`. Make sure the corresponding `glew32.dll` from `bin/Win32` or `bin/x64` resides next to the executables so that they can start correctly.

## Documentation

An initial API documentation setup is provided via `Doxyfile`. If you have [Doxygen](https://www.doxygen.nl/) installed you can generate the HTML documentation with:

```bash
doxygen Doxyfile
```

The output will be placed in the directory configured inside `Doxyfile`.
