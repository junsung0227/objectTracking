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

// user selected bounding box
Rect2d boundingBox;

int main(int argc, char **argv)
{
    /////////////////////////
    // Parse user input
    /////////////////////////

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

    ///////////////////////////////////////////////////
    // Open Videofile and get video information.
    ///////////////////////////////////////////////////

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
        cout << "  b              : pause play and set a bounding box" << endl;        
        cout << "  SPACE or ENTER : tracking with the bounding box" << endl;        
        cout << "  ESC            : exit play\n" << endl;

        // get the video file resolution
        videoResol = Size((int)inputVideo.get(CAP_PROP_FRAME_WIDTH), (int)inputVideo.get(CAP_PROP_FRAME_HEIGHT));
        // get the video file fps
        videoFps = (int)(inputVideo.get(CAP_PROP_FPS));
        // display video file info to the console window
        cout << "[Video information]" << endl;
        cout << "  " << pVideo << " is opened successfully." << endl;
        cout << "  Width  : " << videoResol.width << endl;
        cout << "  Height : " << videoResol.height << endl;
        cout << "  FPS    : " << videoFps << endl;
    }

    // single frame of the input video
    Mat singleFrame;


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

        if(isTracking == true)
            trackingSuccess = csrtTracker->update(singleFrame, boundingBox);
        
        if(trackingSuccess)
        {
            // draw the bounding box
            rectangle(singleFrame, boundingBox, Scalar(0, 0, 255), 2);
            isTracking = true;
        }
        else
        {               
            isTracking = false;
        }

        endTick = getTickCount();   // Stop timer to here
        playFps = getTickFrequency() / (endTick - startTick);        
        startTick = getTickCount(); // Start timer from here

        // display current mode
        putText(singleFrame, isTracking ? "TRACKING" : "PLAYING", textLine1, FONT_HERSHEY_SIMPLEX, 1, Scalar(0), 3);
        putText(singleFrame, to_string(playFps), textLine2, FONT_HERSHEY_SIMPLEX, 1, Scalar(0), 3);

        // show a signleframe
        imshow("OutputWindow", singleFrame);
                
        // Keyboard event handle
        inputKey = waitKey(initDelay);        
        //inputKey = waitKey(1);        

        if (inputKey == 27) // esc key to exit play
            break;
        else if (inputKey == 66 || inputKey == 98) // 'b' or 'B' key to set a bounding box
        {         
            boundingBox = selectROI("OutputWindow", singleFrame, false, false); //opencv_contrib
            
            if(boundingBox.area() > 0.0)
            {                
                csrtTracker = TrackerCSRT::create();
                csrtTracker->init(singleFrame, boundingBox);     
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