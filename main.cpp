
#include <opencv2/opencv.hpp>
#include <iostream>
#include<ctime>
#include<chrono>
using namespace std::chrono;
using namespace cv;
using namespace std;

int main() 
{
    VideoCapture cap("/home/zjc/Desktop/ZUOYE/video.mp4");
    if (!cap.isOpened()) 
    {
        cerr << "Error: Could not open video." << endl;
        return -1;
    }

    while (true) 
    {
        Mat frame, blurred, mask;
        cap >> frame; 
        if (frame.empty()) break;
        auto start=high_resolution_clock::now();
        GaussianBlur(frame, blurred, Size(9, 9), 2, 2); 
        Mat blueMask;
        inRange(blurred, Scalar(100, 150, 250), Scalar(140, 255, 255), blueMask);
        dilate(blueMask, blueMask, Mat(), Point(-1, -1), 2);
        vector<vector<Point>> contours;
        findContours(blueMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for (size_t i = 0; i < contours.size(); i++) 
        {
            int area = contourArea(contours[i]);
            if (area > 1000) 
            { 
                Rect rect = boundingRect(contours[i]);
                double perimeter = arcLength(contours[i], true);
                double radius = (perimeter / (2 * CV_PI));
                double areaDouble = contourArea(contours[i]);
                if (abs(perimeter / (2 * CV_PI) - sqrt(areaDouble / CV_PI)) < 5) 
                { 
                    rectangle(frame, rect, Scalar(0, 0, 255), 2); 
                    auto end = high_resolution_clock::now();
                    auto duration = duration_cast<milliseconds>(end - start).count();
                    putText(frame, "Time:"+to_string(duration)+"ms",rect.tl(),FONT_HERSHEY_PLAIN,1,Scalar(0,0,255),1);
                }
            }
        }

        imshow("Frame", frame); 
        if (waitKey(30) == 27) break; 
    }

    cap.release();
    destroyAllWindows();
    return 0;
}