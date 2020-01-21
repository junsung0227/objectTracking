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

int main(int argc, char **argv)
{
    // user input video file
    string pVideo;

    // Check the parameter validation
    if (argc > 1)
    {
        pVideo = string(argv[1]);
    }
    else
    {
        cout << "\n Usage : ./objectTracking videofilename.mp4" << endl;
        return -1;
    }

    // create VideoCapture objects
    VideoCapture inputVideo(pVideo);
    Size videoResol;
    int videoFps;

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
        cout << "  ESC            : Exit play\n" << endl;

        // get the video file resolution
        videoResol = Size((int)inputVideo.get(CAP_PROP_FRAME_WIDTH), (int)inputVideo.get(CAP_PROP_FRAME_HEIGHT));
        // get the video file fps
        videoFps = (int)(inputVideo.get(CAP_PROP_FPS));
        // display video file info to the console window
        cout << "[Video information] " << pVideo << " is opened successfully." << endl;
        cout << "  Total # of Frames         : " << inputVideo.get(CAP_PROP_FRAME_COUNT) << endl;
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
    namedWindow("OutputWindow", WINDOW_NORMAL); // WINDOW_AUTOSIZE, WINDOW_NORMAL

    // delay time between frames
    int initDelay = 1000 / videoFps;

    // User typed key
    int inputKey = 0;

    // text position to video
    Point textLine1;
    textLine1.x = 50;
    textLine1.y = 50;

    Point textLine2;
    textLine2.x = 50;
    textLine2.y = 100;

    // Tracker
    Ptr<Tracker> csrtTracker;
    csrtTracker = TrackerCSRT::create();    
    bool isTracking = false;
    bool trackingSuccess = false;

    // calculate fps
    double playFps = 0;
    int64 startTick;    // Start time
    int64 endTick;    // End time

    while (true)
    {   
        // Grab a single image from the video file
        inputVideo >> singleFrame;

        // End condition, if there is not reamining frame.
        if (singleFrame.empty())
            break;

        // Output video resolution
        resize(singleFrame, smallsingleFrame, Size(OUT_VIDEO_WIDTH, OUT_VIDEO_HEIGHT), 0, 0, 1);        

        if(isTracking == true)
            trackingSuccess = csrtTracker->update(smallsingleFrame, boundingBox);
        
        // If fail to track
        if(trackingSuccess)
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
        endTick = getTickCount();   // Stop timer to here
        // playFps = round((getTickFrequency() / (endTick - startTick))*100)/100;        
        playFps = getTickFrequency() / (endTick - startTick);
        
        startTick = getTickCount(); // Start timer from here

        // display current mode
        putText(smallsingleFrame, isTracking ? "TRACKING" : "PLAYING", textLine1, FONT_HERSHEY_SIMPLEX, 1, Scalar(0), 3);
        putText(smallsingleFrame, "FPS : " + to_string(playFps).substr(0, to_string(playFps).find('.') + 3), textLine2, FONT_HERSHEY_SIMPLEX, 1, Scalar(0), 3);

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
            
            if(boundingBox.area() > 0.0)
            {                
                csrtTracker = TrackerCSRT::create();
                csrtTracker->init(smallsingleFrame, boundingBox);     
                isTracking = true;
            } 
            else
            {
                // destruct the tracker
                csrtTracker->~Tracker();
                isTracking = false;  
                trackingSuccess = false;              
            }            
        }
    }
    // destroy output windows
    destroyAllWindows();

    return 0;
}

 //cv::getTickCount();

//// Change video resolution if you want
//// 1980:1080 = 1.833:1
////  990: 540 = 1.833:1
//inputVideo.set(CAP_PROP_FRAME_WIDTH,990);  
//inputVideo.set(CAP_PROP_FRAME_HEIGHT,540);  