FROM alpine:3.15.0 AS builder
RUN apk add build-base cmake ninja opencv opencv-dev poco poco-dev
RUN adduser --home /home/app --disabled-password app
WORKDIR /home/app
COPY --chown=app \
    CMakeLists.txt \
    IVideoCapture.h \
    Mutex.h \
    OpencvBackgroundSubtractorFactory.cpp \
    OpencvBackgroundSubtractorFactory.h \
    OpencvVideoCapture.cpp \
    OpencvVideoCapture.h \
    VideoCaptureFactory.cpp \
    VideoCaptureFactory.h \
    main.cpp \
    ./
RUN cmake -GNinja -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=1
RUN cmake --build build

FROM alpine:3.15.0
RUN apk add opencv poco
RUN adduser --home /home/app --disabled-password app
WORKDIR /home/app
USER app
COPY --chown=app --from=builder /home/app/build/MotionDetection ./
# https://github.com/opencv/opencv/pull/9292#issuecomment-345858342
ENV OPENCV_FFMPEG_CAPTURE_OPTIONS=rtsp_transport;udp
ENTRYPOINT ./MotionDetection
