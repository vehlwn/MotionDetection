# MotionDetection

This program reads video stream from a specified source and detects foreground
motions on frames assuming static background. Stream fragments with motion will
be written to a specieied folder.

For the description of configuration options see [app.ini](app.ini).

Dependencies:
- [Conan](https://github.com/conan-io/conan) 2.0+ to build project.

## Build

```bash
$ conan install . --output-folder=build --build=missing
$ cd build
$ meson setup --native-file conan_meson_native.ini -D build_testing=true ..
$ meson compile
```
