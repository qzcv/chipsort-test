#pragma once
#include <QString>

class calib3D5S_newParam
{
public:
	explicit calib3D5S_newParam();
	~calib3D5S_newParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	double imgDist[5];
	double pixelSize[5];
	double actualSize[5];
	double actualDepth[4];
	double prjRatio[4];
	double angle[4];
	double standH[4]; //���������ʵ��վ��ֵ
	double standSize[4];
	double standOffset[4];

	double sizeRow1[5], sizeCol1[5], sizeRow2[5], sizeCol2[5];
	double lineRow1[4], lineCol1[4], lineRow2[4], lineCol2[4];
	double depthRow1[4], depthCol1[4], depthRow2[4], depthCol2[4];
	double standRow1[4], standCol1[4], standRow2[4], standCol2[4]; //��֤վ�ߵ�����
	int horizontalHorVerticalH;  //��ֱ��H����ˮƽ��H

private:
	void iniData();
};

