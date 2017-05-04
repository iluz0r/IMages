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

typedef pair<Mat, String> Params;

int loopbreak = 0;

void rclick_callback(int, int, int, int, void*);

int main(int argc, char** argv) {
	String path("forward_1/*.png");
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
					Params params(cropImage, img_names[k]);
					imshow(img_names[k], cropImage);
					setMouseCallback(img_names[k], rclick_callback, &params);
					waitKey();
				}
			}
		}
	}

	waitKey(0);
	return (0);
}

void rclick_callback(int event, int x, int y, int flags, void* ptr) {
	if (event == EVENT_RBUTTONDOWN) {
		Params *params = (Params*) (ptr);
		String img_title = params->second;
		String img_name = "forward_1_output/"
				+ img_title.substr(img_title.find("/") + 1,
						img_title.length() - 1);
		imwrite(img_name, params->first);
		loopbreak = 1;
		cout << "Image saved!" << endl;
	}
}
