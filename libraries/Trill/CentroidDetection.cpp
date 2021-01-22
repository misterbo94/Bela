#include "CentroidDetection.h"

// a small helper class, whose main purpose is to wrap the #include
// and make all the variables related to it private and multi-instance safe
class CentroidDetection::CalculateCentroids
{
public:
	typedef CentroidDetection::WORD WORD;
	typedef uint8_t BYTE;
	WORD* CSD_waSnsDiff;
	WORD wMinimumCentroidSize = 0;
	BYTE SLIDER_BITS = 7;
	WORD wAdjacentCentroidNoiseThreshold = 400; // Trough between peaks needed to identify two centroids
	//WORD calculateCentroids(WORD *centroidBuffer, WORD *sizeBuffer, BYTE maxNumCentroids, BYTE minSensor, BYTE maxSensor, BYTE numSensors);
	// calculateCentroids is defined here:
#include "calculateCentroids.h"
};

CentroidDetection::~CentroidDetection()
{
	delete cc;
}

CentroidDetection::CentroidDetection(const std::vector<unsigned int>& order, unsigned int maxNumCentroids, float sizeScale)
{
	setup(order, maxNumCentroids, sizeScale);
}
int CentroidDetection::setup(const std::vector<unsigned int>& order, unsigned int maxNumCentroids, float sizeScale)
{
	this->order = order;
	this->maxNumCentroids = maxNumCentroids;
	centroidBuffer.resize(maxNumCentroids);
	sizeBuffer.resize(maxNumCentroids);
	centroids.resize(maxNumCentroids);
	sizes.resize(maxNumCentroids);
	data.resize(order.size());
	setSizeScale(sizeScale);
	delete cc;
	cc = new CalculateCentroids;
	locationScale = (order.size() + 1) * (1 << cc->SLIDER_BITS);
	num_touches = 0;
	return 0;
}

void CentroidDetection::process(DATA_T* rawData)
{
	for(unsigned int n = 0; n < order.size(); ++n)
		data[n] = rawData[order[n]] * (1 << 12);
	cc->CSD_waSnsDiff = data.data();
	cc->calculateCentroids(centroidBuffer.data(), sizeBuffer.data(), maxNumCentroids, 0, order.size(), order.size());

	unsigned int locations = 0;
	// Look for 1st instance of 0xFFFF (no touch) in the buffer
	unsigned int i;
	for(i = 0; i < centroidBuffer.size(); ++i)
	{
		if(0xffff == centroidBuffer[i])
			break;// at the first non-touch, break
		centroids[i] = centroidBuffer[i] / locationScale;
		sizes[i] = sizeBuffer[i] / sizeScale;
	}
	num_touches = i;
}

void CentroidDetection::setSizeScale(float sizeScale)
{
	this->sizeScale = sizeScale;
}

void CentroidDetection::setMinimumTouchSize(DATA_T minSize)
{
	cc->wMinimumCentroidSize = minSize;
}

unsigned int CentroidDetection::getNumTouches()
{
	return num_touches;
}

CentroidDetection::DATA_T CentroidDetection::touchLocation(unsigned int touch_num)
{
	if(touch_num < maxNumCentroids)
		return centroids[touch_num];
	else
		return 0;
}

CentroidDetection::DATA_T CentroidDetection::touchSize(unsigned int touch_num)
{
	if(touch_num < num_touches)
		return sizes[touch_num];
	else
		return 0;
}

// code below from Trill.cpp

#define compoundTouch(LOCATION, SIZE, TOUCHES) {\
	float avg = 0;\
	float totalSize = 0;\
	unsigned int numTouches = TOUCHES;\
	for(unsigned int i = 0; i < numTouches; i++) {\
		avg += LOCATION(i) * SIZE(i);\
		totalSize += SIZE(i);\
	}\
	if(numTouches)\
		avg = avg / totalSize;\
	return avg;\
	}

CentroidDetection::DATA_T CentroidDetection::compoundTouchLocation()
{
	compoundTouch(touchLocation, touchSize, getNumTouches());
}

CentroidDetection::DATA_T CentroidDetection::compoundTouchSize()
{
	float size = 0;
	for(unsigned int i = 0; i < getNumTouches(); i++)
		size += touchSize(i);
	return size;
}
