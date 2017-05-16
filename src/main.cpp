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

Mat GetSquareImage(const Mat& img, int);
void rclick_callback(int, int, int, int, void*);

int main(int argc, char** argv) {
	String path("dataset/*.jpg");
	vector<String> img_names;

	glob(path, img_names, true);
	for (size_t k = 0; k < img_names.size(); k++) {
		Mat img = imread(img_names[k], CV_LOAD_IMAGE_GRAYSCALE);
		if (!img.empty()) {
			Mat threshold_output;
			/// Detect edges using Threshold
			threshold(img, threshold_output, 65, 255, THRESH_BINARY);

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

			double minWidth = 20, minHeight = 20;
			loopbreak = 0;
			for (unsigned int i = 0; i < contours.size() && loopbreak != 1;
					i++) {
				approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
				boundRect[i] = boundingRect(Mat(contours_poly[i]));
				if (boundRect[i].width >= minWidth
						&& boundRect[i].height >= minHeight) {
					Mat coloured_img = imread(img_names[k]);
					// Faccio in modo che la bounding box sia un quadrato e la sposto in base al ridimensionamento
					/*
					 if (boundRect[i].width > boundRect[i].height) {
					 if (boundRect[i].y + boundRect[i].width
					 <= coloured_img.rows) {
					 boundRect[i].height = boundRect[i].width;
					 }
					 } else {
					 if (boundRect[i].x + boundRect[i].height
					 <= coloured_img.cols) {
					 boundRect[i].width = boundRect[i].height;
					 }
					 }
					 */
					Mat cropImage = coloured_img(boundRect[i]);
					Mat squaredCroppedImg = GetSquareImage(cropImage,
							cropImage.rows > cropImage.cols ?
									cropImage.rows : cropImage.cols);
					Params params(squaredCroppedImg, img_names[k]);
					imshow(img_names[k], squaredCroppedImg);
					setMouseCallback(img_names[k], rclick_callback, &params);
					waitKey();
				}
			}
		}
	}
	waitKey(0);
	return (0);
}

Mat GetSquareImage(const Mat& img, int target_width) {
	int width = img.cols, height = img.rows;

	Mat square = Mat::zeros(target_width, target_width, img.type());

	int max_dim = (width >= height) ? width : height;
	float scale = ((float) target_width) / max_dim;
	Rect roi;
	if (width >= height) {
		roi.width = target_width;
		roi.x = 0;
		roi.height = height * scale;
		roi.y = (target_width - roi.height) / 2;
	} else {
		roi.y = 0;
		roi.height = target_width;
		roi.width = width * scale;
		roi.x = (target_width - roi.width) / 2;
	}

	resize(img, square(roi), roi.size());

	return square;
}

void rclick_callback(int event, int x, int y, int flags, void* ptr) {
	if (event == EVENT_RBUTTONDOWN) {
		Params *params = (Params*) (ptr);
		String img_title = params->second;
		String img_name = "output/"
				+ img_title.substr(img_title.find("/") + 1,
						img_title.length() - 1);
		vector<int> quality_param;
		quality_param.push_back(CV_IMWRITE_JPEG_QUALITY);
		quality_param.push_back(100);
		imwrite(img_name, params->first, quality_param);
		loopbreak = 1;
		cout << "Image saved!" << endl;
	}
}
