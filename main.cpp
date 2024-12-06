/*#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <ctime>
#include <chrono>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap("/home/zjc/ZUOYE/video.mp4");
    if (!cap.isOpened()) {
        cout << "Error: Unable to open the camera" << endl;
        return -1;
    }

    while (true) {
        Mat frame;
        cap >> frame; 
        if (frame.empty()) {
            cout << "Error: Blank frame grabbed" << endl;
            break;
        }

        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV); 
         GaussianBlur(hsv, hsv, Size(9, 9), 2, 2);
        Scalar lowerBlue(80, 100, 100);
        Scalar upperBlue(150, 255, 255);
        Mat mask;
        inRange(hsv, lowerBlue, upperBlue, mask);
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));
        //morphologyEx(mask, mask, MORPH_OPEN, kernel);
        //morphologyEx(mask, mask, MORPH_CLOSE, kernel);
        vector<Vec3f> circles;
        HoughCircles(mask, circles, HOUGH_GRADIENT, 1, mask.rows/16, 100, 100, 10, 10000);
        cout << "Detected circles: " << circles.size() << endl;
        for (size_t i = 0; i < circles.size(); i++) {
            Vec3i c = circles[i];
            Point center = Point(c[0], c[1]);
            int radius = c[2];
            int x = center.x - radius;
            int y = center.y - radius;
            int w = radius * 2;
            int h = radius * 2;
            rectangle(frame, Point(x, y), Point(x + w, y + h), Scalar(0, 0, 255), 2, LINE_AA);
            auto now = std::chrono::system_clock::now();
            auto now_c = std::chrono::system_clock::to_time_t(now);
            std::string time_str = std::ctime(&now_c);
            putText(frame, time_str, Point(x, y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1, LINE_AA);
        }
        imshow("Mask" , mask);
        imshow("Frame", frame);

        if (waitKey(30) == 27) break;
    }
    cap.release();
    destroyAllWindows();
    return 0;
}*/
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <sstream>

using namespace cv;
using namespace std;

int main() 
{
    VideoCapture cap("/home/zjc/ZUOYE/video.mp4");
    if (!cap.isOpened()) 
    {
        cout << "Error: Unable to open the camera" << endl;
        return -1;
    }

    while (true) 
    {
        Mat frame, hsv, mask;
        cap >> frame;
        if (frame.empty()) 
        {
            cout << "Error: Blank frame grabbed" << endl;
            break;
        }

        cvtColor(frame, hsv, COLOR_BGR2HSV);
        Scalar lowerBlue(80, 100, 100);
        Scalar upperBlue(150, 255, 255);
        inRange(hsv, lowerBlue, upperBlue, mask);

        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));
        morphologyEx(mask, mask, MORPH_OPEN, kernel);
        morphologyEx(mask, mask, MORPH_CLOSE, kernel);

        vector<vector<Point>> contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for (size_t i = 0; i < contours.size(); i++) 
        {
            auto start_time = chrono::high_resolution_clock::now();
            vector<Point> approx;
            approxPolyDP(contours[i], approx, arcLength(contours[i], true) * 0.02, true);
            if (approx.size() >= 5) 
            {
                double area = contourArea(contours[i]);
                double perimeter = arcLength(contours[i], true);
                if (area > 0 && perimeter > 0) 
                {
                    double circularity = 4 * CV_PI * (area / (perimeter * perimeter));
                    if (circularity > 0.8 && circularity < 1.2) 
                    {
                        Point2f center;
                        float radius;
                        minEnclosingCircle(contours[i], center, radius);
                        auto end_time = chrono::high_resolution_clock::now();

                        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();

                        int x = static_cast<int>(center.x - radius);
                        int y = static_cast<int>(center.y - radius);
                        int w = static_cast<int>(radius * 2);
                        int h = static_cast<int>(radius * 2);
                        rectangle(frame, Point(x, y), Point(x + w, y + h), Scalar(0, 0, 255), 2, LINE_AA);

                        stringstream ss;
                        ss << "Time: " << duration << " us";
                        putText(frame, ss.str(), Point(x, y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1, LINE_AA);
                    }
                }
            }
        }
        imshow("mask", mask);
        imshow("Frame", frame);
        if (waitKey(30) == 27) break;
    }
    cap.release();
    destroyAllWindows();
    return 0;
}