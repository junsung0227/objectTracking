
# import the necessary packages
import cv2

vs = cv2.VideoCapture('./t1.mp4')

while True:
    res, frame = vs.read()
    # cv2.startWindowThread()
    # cv2.namedWindow("preview")

    if res:
        cv2.imshow("preview", frame)

    key = cv2.waitKey(3)
    
    if key == 27:
        print('Press ESc')
        break

vs.release()
cv2.destroyAllWindows()

