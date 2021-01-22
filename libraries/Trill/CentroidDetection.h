#pragma once
#include <stdint.h>
#include <vector>

class CentroidDetection
{
public:
	typedef float DATA_T;
	CentroidDetection() {};
	CentroidDetection(const std::vector<unsigned int>& order, unsigned int maxNumCentroids, float sizeScale);
	~CentroidDetection();
	int setup(const std::vector<unsigned int>& order, unsigned int maxNumCentroids, float sizeScale);
	void process(DATA_T* rawData);
	void setSizeScale(float sizeScale);
	void setMinimumTouchSize(DATA_T minSize);
	unsigned int getNumTouches();
	DATA_T touchLocation(unsigned int touch_num);
	DATA_T touchSize(unsigned int touch_num);
	DATA_T compoundTouchLocation();
	DATA_T compoundTouchSize();
private:
	typedef uint16_t WORD;
	class CalculateCentroids;
	std::vector<DATA_T> centroids;
	std::vector<DATA_T> sizes;
	std::vector<WORD> centroidBuffer;
	std::vector<WORD> sizeBuffer;
	unsigned int maxNumCentroids;
	std::vector<unsigned int> order;
	std::vector<WORD> data;
	float sizeScale;
	float locationScale;
	CalculateCentroids* cc = nullptr;
	unsigned int num_touches;
};
