#pragma once
#include <QString>

class calib3D5SParam
{
public:
	explicit calib3D5SParam();
	~calib3D5SParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	double pixelSize[5];
	double actualSize[5];
	double actualDepth[4];
	double prjRatio[4];
	double angle[4];
	double sizeRow1[5], sizeCol1[5], sizeRow2[5], sizeCol2[5];
	double lineRow1[4], lineCol1[4], lineRow2[4], lineCol2[4];
	double depthRow1[4], depthCol1[4], depthRow2[4], depthCol2[4];
	int horizontalHorVerticalH;  //竖直的H或者水平的H

	double prjOffset[4];
	double prjActualOffset[4];

private:
	void iniData();
};

