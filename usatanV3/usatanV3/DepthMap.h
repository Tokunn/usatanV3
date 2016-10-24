#include <iostream>
#include <sstream>
#include <vector>

#include <atlbase.h>

#include <opencv2/opencv.hpp>

#define DEPTHHEIGHT 424
#define DEPTHWIDTH 512

typedef cv::Mat map;

class DepthMap
{
private:
	std::vector<UINT16> *p_depth;
	map *p_map2d;
public:
	DepthMap(std::vector<UINT16> *p_depthbuffer);
	map *map2d();
	std::vector<UINT16> *extractcenter();
	void drawmap2d();
};