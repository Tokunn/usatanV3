#include "DepthMap.h"

/*----- constructor -----*/
DepthMap::DepthMap(std::vector<UINT16> *p_depthbuffer) {
	p_depth = p_depthbuffer;
}


/*----- map2d -----*/
map *DepthMap::map2d() {
	return p_map2d;
}


/*----- extractcenter -----*/
std::vector<UINT16> *DepthMap::extractcenter() {
	static std::vector<UINT16> extract(DEPTHWIDTH, 0);
	for (int i = 0; i < DEPTHWIDTH; ++i) {
		extract[i] = (*p_depth)[DEPTHWIDTH*int(DEPTHHEIGHT / 2) + i];
	}
	return &extract;
}


/*----- drawmap2d() -----*/
void DepthMap::drawmap2d() {
	map2d();
}