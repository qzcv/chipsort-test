#pragma once
#include <QString>
#include <vector>
#define MAX_ROI 3  //最大区域数目
using std::vector;

class imageSegment_preParam
{
public:
	explicit imageSegment_preParam();
	~imageSegment_preParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
	}segRegion;
	int moduleValid;
	int thresholdType;  //二值化方式， 0阈值分割，1区域生长

	int allImgValid;
	int RoiCounts;
	//threshold
	int minGray[MAX_ROI];
	int maxGray[MAX_ROI];
	int minArea[MAX_ROI];
	int maxArea[MAX_ROI];

	//regiongrowing
	int IsNoCenter;  //0使用中心点，1不使用中心点
	int IsMeanGrow;  //0采用regiongrow，1采用regiongrow_mean
	int centerType;  //种子点是roi中心还是设置的种子点 0roi，1设置种子点,2不使用种子点
	int centerX[MAX_ROI], centerY[MAX_ROI];  //种子点坐标
	int rowWid;
	int colWid;
	int thresholdTol;
	int minGrowArea;

	int algorithmType;
	int param1[5];
	int param2[5];
	int param3[5];
	vector<segRegion> segRegionVector;   //

private:
	void iniData();
};

