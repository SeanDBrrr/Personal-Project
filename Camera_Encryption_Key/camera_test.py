# pylint: disable=no-member,invalid-name

import cv2

cam = cv2.VideoCapture(1)
# cv2.namedWindow("Crypt_Cam")

img_counter = 0

while True:
    ret, frame = cam.read()
    if not ret:
        print("failed to grab frame")
        break

    cv2.imshow("test", frame)
    k = cv2.waitKey(1)
    if k % 256 == 27:
        print("Escape hit, closing camera")
        break
    elif k % 256 == 32:
        img_name = f"OpenCV_frame_{img_counter}.png"
        cv2.imwrite(img_name, frame)
        print("Screenshot Taken")
        img_counter += 1

cam.release()
cv2.destroyAllWindows()
