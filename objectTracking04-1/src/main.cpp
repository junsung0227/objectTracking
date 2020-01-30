///////////////////////////////////////////////////////////////////////////
//
//  Creadted : Junsung Bae(junsung0227@koreanair.com)
//  Created Date : 2020-1-17
//  Version : 1.0
//  Description
//      - Convert python objectTrakcing04 to cpp language.
//      - Add some convenient user interfaces
//  TO-DO Lists
//      -
///////////////////////////////////////////////////////////////////////////

// General includes
#include <iostream>
#include <string>
#include <algorithm>

// OpenCV includes
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/tracking.hpp>

using namespace std;
using namespace cv;

// Predefined Output Video Resolution
#define OUT_VIDEO_WIDTH 960
#define OUT_VIDEO_HEIGHT 540

// user selected bounding box
Rect2d boundingBox;

void onChange(int pos, void *userdata);

int main(int argc, char **argv)
{
    cout << "[Usage]" << endl;
    cout << "  ./executableName videoFileName (optional)trackerName" << endl;        
    cout << "     trackerName : csrt(default), kcf, boosting, goturn, medianflow, mil, mosse, tld" << endl;
    cout << "     ex) ./objectTracking04-1 videofilename.mp4 csrt" << endl;

    // user input video file
    string pVideo;

    // Check the parameter validation
    //int trackerName = 0;    // default tracker is CSRT
    string trackerName;
    if (argc == 2)
    {
        pVideo = string(argv[1]);
    }
    else if (argc == 3)
    {
        pVideo = string(argv[1]);
        trackerName = string(argv[2]);
        // string converter to upper case
        transform(trackerName.begin(), trackerName.end(), trackerName.begin(), ::toupper);
    }
    else
    {
        cout << "ERROR : Check the USAGE guide first!" << endl;
        return -1;
    }

    // input Video
    VideoCapture inputVideo;

    // create VideoCapture objects
    inputVideo = VideoCapture(pVideo);
    Size videoResol;
    int videoFps;
    int videoTotalFrames;

    // if fail to open the video file
    if (!inputVideo.isOpened())
    {
        cout << " Can not open inputVideo." << endl;
        return -1;
    }
    else
    {
        // display User mannual to the console window
        cout << "[Usage]" << endl;
        cout << "  b              : Pause play to set a bounding box" << endl;
        cout << "  SPACE or ENTER : Tracking with the bounding box" << endl;
        cout << "  ESC            : Exit play\n"
             << endl;

        // get the video file resolution
        videoResol = Size((int)inputVideo.get(CAP_PROP_FRAME_WIDTH), (int)inputVideo.get(CAP_PROP_FRAME_HEIGHT));
        // get the video file fps
        videoFps = (int)(inputVideo.get(CAP_PROP_FPS));
        // get the video file's total frame count
        videoTotalFrames = inputVideo.get(CAP_PROP_FRAME_COUNT);
        // display video file info to the console window
        cout << "[Video information] " << pVideo << " is opened successfully." << endl;
        cout << "  Total # of Frames         : " << videoTotalFrames << endl;
        cout << "  Original Video FPS        : " << videoFps << endl;
        cout << "  Original Video Resolution : " << videoResol.width << "x" << videoResol.height << endl;
        cout << "  Output Video Resolution   : " << OUT_VIDEO_WIDTH << "x" << OUT_VIDEO_HEIGHT << endl;
    }

    // single frame of the input video
    Mat singleFrame;
    // the resolution of output video
    Mat smallsingleFrame;

    // Create a window to display the output video
    // The first text parameter should be same with imshow()
    namedWindow("OutputWindow", WINDOW_AUTOSIZE); // WINDOW_AUTOSIZE, WINDOW_NORMAL

    // delay time between frames
    int initDelay = 1000 / videoFps;

    // User typed key
    int inputKey = 0;

    // text line1 position on video
    Point textLine1;
    textLine1.x = 50;
    textLine1.y = 50;

    // text line2 position on video
    Point textLine2;
    textLine2.x = 50;
    textLine2.y = 100;

    // text line3 position on video
    Point textLine3;
    textLine3.x = 50;
    textLine3.y = 150;

    // Tracker
    Ptr<Tracker> myTracker;    
    bool isTracking = false;
    bool trackingSuccess = false;

    // calculate fps
    double playFps = 0;
    int64 startTick; // Start time
    int64 endTick;   // End time

    // TrackBar
    int playingPosition = 0;
    createTrackbar("Move to Frame #", "OutputWindow", &playingPosition, videoTotalFrames, onChange, &inputVideo);

    while (true)
    {
        // Grab a single image from the video file
        inputVideo >> singleFrame;

        // End condition, if there is not reamining frame.
        if (singleFrame.empty())
            break;

        // Output video resolution
        resize(singleFrame, smallsingleFrame, Size(OUT_VIDEO_WIDTH, OUT_VIDEO_HEIGHT), 0, 0, 1);

        // Update the bounding box
        if (isTracking == true)
            trackingSuccess = myTracker->update(smallsingleFrame, boundingBox);
            //trackingSuccess = csrtTracker->update(smallsingleFrame, boundingBox);

        // If fail to track
        if (trackingSuccess)
        {
            // draw the bounding box
            rectangle(smallsingleFrame, boundingBox, Scalar(0, 0, 255), 2);
            isTracking = true;
        }
        else
        {
            isTracking = false;
        }

        // Calculate real output FPS
        endTick = getTickCount(); // Stop timer to here
        // playFps = round((getTickFrequency() / (endTick - startTick))*100)/100;
        playFps = getTickFrequency() / (endTick - startTick);
        startTick = getTickCount(); // Start timer from here

        // display current mode
        putText(smallsingleFrame, isTracking ? ("TRACKING : " + trackerName) : "PLAYING", textLine1, FONT_HERSHEY_SIMPLEX, 1, Scalar(0), 3);
        putText(smallsingleFrame, "FPS : " + to_string(playFps).substr(0, to_string(playFps).find('.') + 3), textLine2, FONT_HERSHEY_SIMPLEX, 1, Scalar(0), 3);
        putText(smallsingleFrame, "Frame # : " + to_string((int)inputVideo.get(CAP_PROP_POS_FRAMES)), textLine3, FONT_HERSHEY_SIMPLEX, 1, Scalar(0), 3);

        // Trackbar is working but too slow, so comment out at this time
        // Rather than display on the Trackbar, I will display the frame number on image
        // For later on, DO NOT REMOVE THIS.
        // setTrackbarPos("Move to Frame #", "OutputWindow", inputVideo.get(CAP_PROP_POS_FRAMES));

        // show a signleframe
        imshow("OutputWindow", smallsingleFrame);

        // Keyboard event handle
        inputKey = waitKey(initDelay);
        //inputKey = waitKey(1);

        if (inputKey == 27) // esc key to exit play
            break;
        else if (inputKey == 66 || inputKey == 98) // 'b' or 'B' key to set a bounding box
        {
            boundingBox = selectROI("OutputWindow", smallsingleFrame, false, false); //opencv_contrib

            if (boundingBox.area() > 0.0)
            {
                if(trackerName == "CSRT")
                    myTracker = TrackerCSRT::create();
                else if (trackerName == "KCF")
                    myTracker = TrackerKCF::create();
                else if (trackerName == "BOOSTING")
                    myTracker = TrackerBoosting::create();
                else if (trackerName == "GOTURN")
                    myTracker = TrackerGOTURN::create();
                else if (trackerName == "MEDIANFLOW")
                    myTracker = TrackerMedianFlow::create();
                else if (trackerName == "MIL")
                    myTracker = TrackerMIL::create();
                else if (trackerName == "MOSSE")
                    myTracker = TrackerMOSSE::create();
                else if (trackerName == "TLD")
                    myTracker = TrackerTLD::create();

                myTracker->init(smallsingleFrame, boundingBox);
                isTracking = true;
            }
            else
            {
                // destruct the tracker                
                myTracker->~Tracker();
                isTracking = false;
                trackingSuccess = false;
            }
        }
    }
    // destroy output windows
    destroyAllWindows();

    return 0;
}

// To handle the trackbar move event
void onChange(int pos, void *userdata)
{
    VideoCapture* vc = (VideoCapture *)userdata;
    vc->set(CAP_PROP_POS_FRAMES, pos);
}

//cv::getTickCount();

//// Change video resolution if you want
//// 1980:1080 = 1.833:1
////  990: 540 = 1.833:1
//inputVideo.set(CAP_PROP_FRAME_WIDTH,990);
//inputVideo.set(CAP_PROP_FRAME_HEIGHT,540);