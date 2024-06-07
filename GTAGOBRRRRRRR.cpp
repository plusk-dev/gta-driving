#include <opencv2/opencv.hpp>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "utils.h"
#include "pedal.h"

using namespace cv;

const Scalar rgba_color(210, 240, 92, 255);
const Scalar bgr_color(rgba_color[2], rgba_color[1], rgba_color[0]);
const int tolerance = 70;
const int vJoyRID = 1;
//std::map<std::string, int> hidUsageMap;
//
//// Populate the map
//hidUsageMap["HID_USAGE_X"] = 0x30;
//hidUsageMap["HID_USAGE_Y"] = 0x31;
//hidUsageMap["HID_USAGE_Z"] = 0x32;
//hidUsageMap["HID_USAGE_RX"] = 0x33;
//hidUsageMap["HID_USAGE_RY"] = 0x34;
//hidUsageMap["HID_USAGE_RZ"] = 0x35;
//hidUsageMap["HID_USAGE_SL0"] = 0x36;
//hidUsageMap["HID_USAGE_SL1"] = 0x37;
//hidUsageMap["HID_USAGE_WHL"] = 0x38;
//hidUsageMap["HID_USAGE_POV"] = 0x39;

void createMask(const Mat& frame, Mat& mask) {
	Mat hsv;
	cvtColor(frame, hsv, COLOR_BGR2HSV);
	Scalar lower_bound = Scalar(max(0.0, bgr_color[0] - tolerance), max(0.0, bgr_color[1] - tolerance), max(0.0, bgr_color[2] - tolerance));
	Scalar upper_bound = Scalar(min(255.0, bgr_color[0] + tolerance), min(255.0, bgr_color[1] + tolerance), min(255.0, bgr_color[2] + tolerance));
	inRange(hsv, lower_bound, upper_bound, mask);
}

double slope(std::vector<Point>& non_black_pixels) {
	return ((non_black_pixels.size() > 1) ? (non_black_pixels.back().y - non_black_pixels.front().y) / static_cast<double>(non_black_pixels.back().x - non_black_pixels.front().x) : 0);
}

void mainLoop(Mat& frame, Mat& mask, Mat& masked_frame, std::vector<Point>& non_black_pixels, std::unordered_map<CHAR, bool>& keyMappings, double& k) {
	createMask(frame, mask);
	bitwise_and(frame, frame, masked_frame, mask);


	cvtColor(masked_frame, masked_frame, COLOR_BGR2GRAY);
	findNonZero(masked_frame, non_black_pixels);

	k = slope(non_black_pixels);
	if (k > 0) PressKey('w', keyMappings);
	else if (k < 0) ReleaseKey('w', keyMappings);
	imshow("Original", frame);
	imshow("Masked", masked_frame);
}
typedef int (*SetAxisType)(int, int, int);  // Function pointer type

SetAxisType getFunctionPointer(HMODULE hDll, const char* functionName) {
	FARPROC functionAddress = GetProcAddress(hDll, functionName);
	if (functionAddress == NULL) {
		FreeLibrary(hDll);
		throw std::invalid_argument("failed to get function");
	}
	SetAxisType functionPtr = reinterpret_cast<SetAxisType>(functionAddress);
	return functionPtr;
}
int main() {
	VideoCapture cap(1);
	if (!cap.isOpened()) {
		std::cout << "Error: Couldn't open the camera\n";
		return -1;
	}
	Mat frame, mask, masked_frame;
	std::vector<Point> non_black_pixels;
	std::unordered_map<CHAR, bool> keyMappings;
	double k;
	HMODULE vJoy = LoadvJoyDLLFile("vJoyInterface.dll");
	typedef int (*SetAxisType)(int,int,int);
	// axis value, device RID, axisID
	SetAxisType setAxis = getFunctionPointer(vJoy, "SetAxis");
	std::cout << setAxis(0xf, 0x1, 0x30) << std::endl;
	while (true) {
		cap.read(frame);
		if (frame.empty())
			break;
		mainLoop(frame, mask, masked_frame, non_black_pixels, keyMappings, k);
		if (waitKey(1) == 'q')
			break;
	}
	FreeLibrary(vJoy);
	cap.release();
	destroyAllWindows();
	return 0;
}
