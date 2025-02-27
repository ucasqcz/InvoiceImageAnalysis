#pragma once

#include <io.h>
#include <direct.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;


namespace QczFile{

	void getFiles_(string path, vector<string>& files);

	string splitFileName(string path);

	void splitExt(string name, string& subName, string& ext);
}
namespace QczStr{

	// recursive replace the substr
	string&		replace_all(string&   str, const   string&   old_value, const   string&   new_value);
	// replace the substr once
	string&		replace_all_distinct(string&   str, const   string&   old_value, const   string&   new_value);

	string		int2string(int input);
}

namespace QczSort{
	
	inline bool compareRectWidth(const cv::Rect& a, const cv::Rect& b){
		return a.width > b.width;
	};
	inline bool compareRectArea(const cv::Rect& a, const cv::Rect& b){
		return a.area() > b.area();
	};

	template<typename T,typename D>
	inline bool compareIndexD(const pair<T, D>& a, const pair<T, D>& b){
		return a.first < b.first;
	}
}

namespace QczVision{

	void rotateImg(cv::Mat src, cv::Mat& dst, cv::Mat& M, double degree);

}