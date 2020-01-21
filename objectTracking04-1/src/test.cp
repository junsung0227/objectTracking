// OpenCV includes
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

CvCapture *g_capture = NULL;

using namespace std;
using namespace cv;

////////////////////////////////////trackbar///////////////////////////////////
int g_slider_position = 0;
void onTrackbarSlide(int pos)
{
    SetCaptureProperty(g_capture, CV_CAP_PROP_POS_FRAMES, pos);
}
//////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    namedWindow("Tmp", CV_WINDOW_AUTOSIZE);
    g_capture = cvCreateFileCapture("C:/Users/JunAir/Desktop/wildlife.wmv");

    ///////////////////////////trackbar/////////////////////////////////////
    int frames = (int)cvGetCaptureProperty(g_capture, CV_CAP_PROP_FRAME_COUNT);
    if (frames != 0)
    {
        cvCreateTrackbar("Position",         //슬라이더바에 표시되는 이름
                         "Tmp",              //윈도우 이름
                         &g_slider_position, //슬라이더 위치
                         frames,
                         onTrackbarSlide);
    }
    ////////////////////////////////////////////////////////////////////////

    IplImage *frame;
    char c;
    while (1)
    {
        frame = cvQueryFrame(g_capture);
        if (!frame)
            break;
        cvShowImage("Tmp", frame);
        c = cvWaitKey(33);
        if (c == 27)
            break;
    }

    cvReleaseCapture(&g_capture);
    cvDestroyWindow("Tmp");
    return 0;
}

//출처 : https: //coyagi.tistory.com/entry/c-OpenCV-동영상재생-트랙바추가?category=616509 [코코야이야기]