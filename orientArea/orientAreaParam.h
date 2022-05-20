#pragma once
#include <QString>

class orientAreaParam
{
public:
	explicit orientAreaParam();
	~orientAreaParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	int areaMin;     //最小面积
	int areaMax;     //最大面积
	int widthMin;    //最小宽度
	int widthMax;    //最大宽度
	int heightMin;   //最小高度
	int heightMax;
	int grayMin;
	int grayMax;
	int Opening;        //开操作的值
	int Closing;       //闭操作的值
	int errRegionMin;
	int errRegionMax;
	int errAreaMin;
	int errAreaMax;
 	int pin1Idx;
 	int pin2Idx;
	int errAreaCompareIndex;
	int noneNgIdx;//输出无料NG的OUTPUT序号

	bool noneNgValid;
	bool IsSingleTarget;
	bool whiteGuy;
	bool IsFillUp;
	bool IsShowRegion;
	bool referSection;
	bool usePinDir;
	bool IsReverse;
	bool errAreaCompareValid;
	bool closeValid;  //是否启用close,0关闭,1启用
	bool openValid;   //是否启用open,0关闭,1启用
	bool errRegionValid;
	bool errAreaValid;      //是否采用面积

	//ROI信息
	double RoiRow1[2];     //左右区域
	double RoiCol1[2];
	double RoiRow2[2];
	double RoiCol2[2];

private:
	void iniData();
};

