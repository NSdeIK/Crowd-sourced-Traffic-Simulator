/*
* g++ TrafficAnalyzer_v002.cpp -o TrafficAnalyzer_v002 `pkg-config --cflags --libs opencv`
* ./TrafficAnalyzer_v002 ../cascade/haarcascade_car_1.xml ../test_videos/part3.mov
*/

#include "opencv2/opencv.hpp"

using namespace cv;

const int white_color = 255;

int main(int argc, char** argv){

	if (argc > 3){
		std::cout << "usage!" << std::endl;
		return 0;
	}

    VideoCapture cap(argv[2]); //or 0 for default cam
    if(!cap.isOpened())
        return -1;

    CascadeClassifier cascadeClassifier (argv[1]); //or absolute or relative path to xml

    for(;;){

        Mat frame, grey;
        std::vector<Rect> detected_objects; //will contain detected objects
        Rect roi_rect (500, 160, 400, 300); //the ROI on the original videostream

        cap >> frame;

        Mat roi (frame, roi_rect);

        cvtColor(roi, grey, COLOR_BGR2GRAY);

        cascadeClassifier.detectMultiScale(grey, detected_objects, 1.1, 3, 0, Size ( 120, 120 ), Size ( 180, 180 ) );

        /*
        * Use the detected_objects vector to get information, e.g.:
        * std::cout << "Number of detected objects on this image: " << detected_objects.size() << "\n";
		*/

        //visualization
        for(int i = 0; i < detected_objects.size(); i++)
        	rectangle(grey, detected_objects.at(i), white_color);

        imshow("Traffic Analyzer v002", grey);
        if(waitKey(30) == 'c') break;
    }

    std::cout << std::endl;

    return 0;
}