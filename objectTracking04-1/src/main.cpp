// General includes
#include <iostream>

// OpenCV includes
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

// user selected bounding box
Rect boundingBox;

// for mouseEventHandel
bool isMouseButtonDown = false;

// display mode define
typedef enum {
                PLAY,                   // just play the video
                SELECTING_BOUNDINGBOX,  // selecting target object to track
                PLAY_WITH_TRACKING      // tracking
             } DISPLAYMODE;
             
// dispaly mode
DISPLAYMODE displayMode = PLAY;

void onMouse(int mevent, int x, int y, int flags, void *param)
{
    static Point leftButtonDownPosition;

    Mat *pMat = (Mat *)param;
    Mat image = Mat(*pMat);

    // if the left mouse button is down status, we caculate the new position
    if (isMouseButtonDown)
    {
    	boundingBox.x = MIN(x, leftButtonDownPosition.x);
    	boundingBox.y = MIN(y, leftButtonDownPosition.y);
    	boundingBox.width = boundingBox.x + abs(x - leftButtonDownPosition.x);
    	boundingBox.height = boundingBox.y + abs(y - leftButtonDownPosition.y);

    	boundingBox.x = MAX(boundingBox.x, 0);
    	boundingBox.y = MAX(boundingBox.y, 0);
    	boundingBox.width = MIN(boundingBox.width, image.cols);
    	boundingBox.height = MIN(boundingBox.height, image.rows);
    	boundingBox.width -= boundingBox.x;
    	boundingBox.height -= boundingBox.y;
    }

    switch (mevent)
    {   
        case EVENT_LBUTTONDOWN: // Start point of the bounding box, so we can get the x, y position 
            leftButtonDownPosition = Point(x, y);
            boundingBox = Rect(x, y, 0, 0);
            isMouseButtonDown = true;
            break;
        case EVENT_LBUTTONUP:   // End point of the bounding box, so we will calculate and get the width and height        
            isMouseButtonDown = false;
            if (boundingBox.width > 0 && boundingBox.height > 0)                
                displayMode = SELECTING_BOUNDINGBOX;   // set the display mode
            cout << "\n[Bounding Box Information]" << endl;
            cout << "  x : " << boundingBox.x << endl;
            cout << "  y : " << boundingBox.y << endl;
            cout << "  w : " << boundingBox.width << endl;
            cout << "  h : " << boundingBox.height << endl;
            break;        
    }
}

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
        cout << "  s key     : pause play" << endl;
        cout << "  SPACE BAR : restart play" << endl;
        cout << "  ESC       : exit play\n" << endl;

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

    ///////////////////////////////////////////////////
    // Play the video
    ///////////////////////////////////////////////////    
    
    // single frame of the input video
    Mat singleFrame;

    // Create a window to display the output video
    // The first text parameter should be same with imshow()
    namedWindow("OutputWindow", WINDOW_NORMAL); // WINDOW_AUTOSIZE, WINDOW_NORMAL
    
    // delay time between frames
    int initDelay = 1000 / videoFps;
    // for keyboard event handle
    int newDelay = initDelay;
    // User input keyboard key
    int inputKey = 0;

    // for mouse event handle
    setMouseCallback("OutputWindow", onMouse, (void *)&singleFrame);

    // text position to video
    Point textPosition;
    textPosition.x = 50;
    textPosition.y = 50;

    while (true)
    {
        // Grab a single image from the video file
        inputVideo >> singleFrame;

        // End condition, if there is not reamining frame.
        if (singleFrame.empty())
            break;

        // put some text on the output video
        putText(singleFrame, "Need some text here", textPosition, FONT_HERSHEY_SIMPLEX, 1, Scalar(0), 3);

        // show a signleframe
        imshow("OutputWindow", singleFrame);
        
        // Keyboard event handle
        inputKey = waitKey(newDelay);
        if (inputKey == 27)         // esc key to exit play
            break;
        else if (inputKey == 115)   // 's' key to stop play
            newDelay = 0;
        else if (inputKey == 32)    // space key to restart play
            newDelay = initDelay;
    }

    // destroy output windows
    destroyAllWindows();

    return 0;
}
