import cv2
import datetime
import numpy

cap = cv2.VideoCapture(0)
print(f"{cap.get(cv2.CAP_PROP_FRAME_WIDTH)=}")
print(f"{cap.get(cv2.CAP_PROP_FRAME_HEIGHT)=}")
print(f"{cap.get(cv2.CAP_PROP_FPS)=}")
print(f"{cap.get(cv2.CAP_PROP_FOURCC)=}")
print(f"{cap.get(cv2.CAP_PROP_CONVERT_RGB)=}")
CAMERA_FRAME_WIDTH = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
CAMERA_FRAME_HEIGHT = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

fourcc = cv2.VideoWriter_fourcc(*"XVID")
out = cv2.VideoWriter(
    "output.mkv", fourcc, 30.0, (CAMERA_FRAME_WIDTH, CAMERA_FRAME_HEIGHT)
)


def draw_timestamp(image):
    text = str(datetime.datetime.now())
    font_face = cv2.FONT_HERSHEY_PLAIN
    fcont_scale = 1.0
    thickness = 1
    (size, base_line) = cv2.getTextSize(text, font_face, fcont_scale, thickness)
    text_org = (0, size[1])
    cv2.rectangle(image, (0, 0), (size[0], size[1]), (255, 255, 255), cv2.FILLED)
    cv2.putText(
        image,
        text,
        text_org,
        font_face,
        fcont_scale,
        (0, 0, 0),
        thickness,
        cv2.LINE_AA,
    )


def draw_recording_circle(image):
    radius = 10
    padding = 20
    cv2.circle(
        image,
        (CAMERA_FRAME_WIDTH - radius - padding, radius + padding),
        radius,
        (0, 0, 255),
        cv2.FILLED,
        cv2.LINE_AA,
    )


def get_datetime_filename() -> str:
    now = datetime.datetime.now()
    return (
        f"{now.year:0=4}-{now.month:0=2}-{now.day:0=2}"
        f" {now.hour:0=2}.{now.minute:0=2}"
    )


DELTA_WITHOUT_MOTION = datetime.timedelta(seconds=5)
MIN_AREA = 500
recording = True
last_motion_point = datetime.datetime.now()
fgbg = cv2.createBackgroundSubtractorMOG2(history=1000, detectShadows=False)
while True:
    ret, frame = cap.read()
    if not ret:
        break

    blurred_frame = cv2.GaussianBlur(frame, (21, 21), 0)
    fgmask = fgbg.apply(blurred_frame)
    ret, thresh1 = cv2.threshold(fgmask, 127, 255, cv2.THRESH_BINARY)
    moving_area = numpy.count_nonzero(thresh1 == 255)
    print(f"{moving_area=}")
    print(f"{last_motion_point=}")
    if moving_area >= MIN_AREA:
        last_motion_point = datetime.datetime.now()
        recording = True
    if (
        moving_area < MIN_AREA
        and datetime.datetime.now() - last_motion_point > DELTA_WITHOUT_MOTION
    ):
        recording = False

    draw_timestamp(frame)
    if recording:
        out.write(frame)
        draw_recording_circle(frame)
    cv2.imshow("frame", frame)
    cv2.imshow("thresh1", thresh1)
    cv2.imshow("back_image", fgbg.getBackgroundImage())
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

# When everything done, release the capture
cap.release()
out.release()
cv2.destroyAllWindows()
