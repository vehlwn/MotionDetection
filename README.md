# MotionDetection

Reads video stream from a webcam or file on a disk and detect foreground motions on frames assuming static background. Supports either one webcam or one input video file. Frames with motion will be written to a video file in output folder. Output files will be rotated every 1 hour (refer to the Settings section).

The program depends on:

- [Qt](https://www.qt.io/offline-installers) for GUI and signals-slots to communicate threads;
- [OpenCV](https://opencv.org/) for `cv::VideoCapture`, `cv::VideoWriter` and `cv::BackgroundSubtractorMOG2`.

## Build

### Requirements

Qt_DIR, OpenCV_DIR must be available to find_package. One can download and build OpenCV from [sources](https://github.com/opencv/opencv).

### Commands

```bash
mkdir build
cd build
cmake -GNinja .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Settings

### Input video

- Camera or video file on disk. For webcam number refer to [here](https://docs.opencv.org/4.4.0/d8/dfe/classcv_1_1VideoCapture.html#aabce0d83aa0da9af802455e8cf5fd181). Default: camera 0;
- frame buffer size: maximum number of frames to read in advance from a webcam or video file. Frames are read into a queue in FrameProducerThread.cpp and taken from there on timeout in FrameConsumerThread.cpp. If producer produces too fast it will be blocked and new frames won't longer be pushed to the queue untill the consumer pops them out with a given speed equal to FPS of an input source. Caution should be taken with the size: large values can consume a lot of memory. Default: 10;
- recommended width, height: set this option if your camera supports different output sizes. For details refer to note in [cv::VideoCapture::get](https://docs.opencv.org/4.3.0/d8/dfe/classcv_1_1VideoCapture.html#aa6480e6972ef4c00d74814ec841a2939). Default: width 640, height: 360.

### Output video

- Folder: a folder where to put recorded video with motion. If it doesn't exist it will be created. Output files will have names with date and time in format QDateTime::toString("yyyy-MM-dd hh.mm.ss"). Output files will be periodically rotated. Refer to 'file rotation period' option. Default: 'video';
- extension: format to save output files. Default: '.avi';
- FOURCC: codec to encode output video. List of available codes can be found [here](http://www.fourcc.org/codecs.php). Default: 'DIVX';
- file rotation period: period when old output file will be closed and a new one created with a newly generated name. Default: 1 hour.

### Segmentation options

- History: number of last frames affecting background model. Refer to opencv [documentation](https://docs.opencv.org/4.3.0/d7/d7b/classcv_1_1BackgroundSubtractorMOG2.html#a5e8b40fef89a582ce42d99d2453db67a). Default: 100;
- Gaussian kernel size: if checked background subtractor will be applyed to blurred frame with Gaussian filter and a given ksize. Refer to opencv [documentation](https://docs.opencv.org/4.3.0/d4/d86/group__imgproc__filter.html#gaabe8c836e97159a9193fb0b11ac52cf1). Default: checked 5;
- min moving area: numer of nonzero pixels in a foreground mask generated by a background subtractor. Specifies minimum sensitivity of what to consider a motion. Default: 500;
- delta without motion: number of seconds when there is no motion but we want to record a few frames of silence to decrease motion sensitivity. Default: 5 s.

## Examples

![Main screen](examples/images/MainScreen1.png)
![Settings](examples/images/Settings1.png)