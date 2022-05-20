#pragma once
#include <QString>
#include "HalconCpp.h"
#include <vector>

using namespace Halcon;
using std::vector;

#define MAX_PAT_NUM 4

class nccMatch_fourDirParam
{
public:
	explicit nccMatch_fourDirParam();
	~nccMatch_fourDirParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row;
		double col;
		double length1;
		double length2;
		double angle;
	}segRegion;
	typedef struct {
		double drow;
		double dcol;
		double dlength1;
		double dlength2;
		double dangle;
	}delRegion;

	int IsRefer;   //是否采用参考区域。注意此处的区域暂时只支持rectangle1
	int similarity;
	int seekingAngle;
	int minusArea[MAX_PAT_NUM];
	int currentPat;   //当前模板指数
	int acceptEnable;
	int relativeEnable;
	int acceptValue;
	int minRelative, maxRelative;
	int matchAngleEnable;
	int matchAngle;  //一致性判断

	double roiCol1[MAX_PAT_NUM], roiCol2[MAX_PAT_NUM], roiRow1[MAX_PAT_NUM], roiRow2[MAX_PAT_NUM];
	double RecRoiRow1, RecRoiRow2, RecRoiCol1, RecRoiCol2;

	int patternX[MAX_PAT_NUM], patternY[MAX_PAT_NUM], patternW[MAX_PAT_NUM], patternH[MAX_PAT_NUM];

	double minusRow[MAX_PAT_NUM], minusCol[MAX_PAT_NUM], minusLength1[MAX_PAT_NUM], minusLength2[MAX_PAT_NUM];
	int minusA[MAX_PAT_NUM];

	double minusR[MAX_PAT_NUM], minusCX[MAX_PAT_NUM], minusCY[MAX_PAT_NUM];
	int findNum;
	double maxOverlap;
	Hobject patImage[MAX_PAT_NUM];  //当采用两个方向的时候，序号0,1存储图片

	int IsUsePinDir;   //是否启用方向
	int pin1OutIdx;    //管脚1输出口
	int pin2OutIdx;
	int IsDefaultSig;  //是否启用默认信号
	int Is2Dor4D;      //采用两个极性区域还是四个极性区域
	int IsMirrorDir;   //两方向模式是否采用镜像模式
	int HorOrVerMir;   //上下镜像还是左右镜像

	int IsDefaultEdit; //是否是默认编辑顺序
	int sigEdit[2][4]; //编辑的二进制信号使用模式  [2]两个二进制信号段 [4]四个方向

    //int multAngle;     //默认的设置多模板的角度，0-90,1-180,2-270
	int IsAutoRotate;  //是否自动旋转模板
	int StartAngle;    //自动模板时候开始的角度 0-0,1-90,2-180,3-270

	//out area
	Hobject OutImg[MAX_PAT_NUM];   //传出区域,当采用两个方向的时候，序号0,1存储区域
	int outRegionCount;
	int outDeleteCount;
	int outAreaMin;   //面积最小
	int outAreaMax;
	int outThreMin;   //阈值
	int outThreMax;
	int IsUseOutArea;  //是否使用传出区域
	int IsShowOutArea; //测后显示传出区域
	int IsFillUp;      //是否填充传出模板

	vector<segRegion> outRegionVec;  //传出ROI
	vector<delRegion> deleteRegionVec;  //排除ROI

private:
	void iniData();
};

