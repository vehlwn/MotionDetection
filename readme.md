# MotionDetection

This program reads video stream from a specified source and detects foreground
motions on frames assuming static background. Stream fragments with motion will
be written to a specieied folder.

For the description of configuration options see [app.ini](app.ini).

Dependencies:
- [Meson](https://mesonbuild.com/) to build project;
- [OpenCV](https://github.com/opencv/opencv) >= 4.6.0 for background subtractor
  algorithms;
- [Drogon](https://github.com/drogonframework/drogon) for web server;
- [Boost](https://www.boost.org/) >= 1.78.0 for logging, ranges ang alrogithms;
- [FFmpeg](https://ffmpeg.org/doxygen/trunk/index.html) >= 5.1 for reading
  input files and writing output files.

## Build

```bash
$ mkdir build && cd build
$ meson setup --buildtype release
$ meson compile
$ meson install
# Or install to alternative location:
$ meson setup --reconfigure --prefix install
$ meson install

# Alternative with conan:
$ conan build . --output-folder build --build=missing -s build_type=Release

# Alternative with conan in venv:
$ python -m venv venv
$ ./venv/bin/pip install -r requirements.txt
$ ./venv/bin/conan profile detect
# Modify generated ~/.conan2/profiles/default as you need.
$ ./venv/bin/conan build . --output-folder build --build=missing -s build_type=Release
```
