/*
 * main.cpp
 *
 *  Created on: 04 mag 2017
 *      Author: angelo
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int loopbreak = 0;

void rclick_callback(int, int, int, int, void*);

int main(int argc, char** argv) {
	String path("test/*.png");
	vector<String> img_names;

	glob(path, img_names, true);
	for (size_t k = 0; k < img_names.size(); k++) {
		Mat img = imread(img_names[k], CV_LOAD_IMAGE_GRAYSCALE);
		if (!img.empty()) {
			Mat threshold_output;
			/// Detect edges using Threshold
			threshold(img, threshold_output, 60, 255, THRESH_BINARY);

			imshow("threshold_output", threshold_output);
			waitKey();
			vector<vector<Point> > contours;
			vector<Vec4i> hierarchy;
			/// Find contours
			findContours(threshold_output, contours, hierarchy, CV_RETR_TREE,
					CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

			/// Approximate contours to polygons + get bounding rects
			vector<vector<Point> > contours_poly(contours.size());
			vector<Rect> boundRect(contours.size());

			double minWidth = 90, minHeight = 90;
			loopbreak = 0;
			for (unsigned int i = 0; i < contours.size() && loopbreak != 1;
					i++) {
				approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
				boundRect[i] = boundingRect(Mat(contours_poly[i]));
				if (boundRect[i].width >= minWidth
						&& boundRect[i].height >= minHeight) {
					Mat coloured_img = imread(img_names[k]);
					Mat cropImage = coloured_img(boundRect[i]);
					imshow("Title", cropImage);
					setMouseCallback("Title", rclick_callback, &cropImage);
					waitKey();
				}
			}
		}
	}

	waitKey(0);
	return (0);
}

void rclick_callback(int event, int x, int y, int flags, void* ptrMat) {
	if (event == EVENT_RBUTTONDOWN) {
		Mat *cropImage = (Mat*) ptrMat;
		imwrite("test_sample.png", *cropImage);
		loopbreak = 1;
		cout << "Image saved!" << endl;
	}
}
