#pragma once
#include <QString>
#include <vector>
#define MAX_PINGRP_NUM 20  //单组最大管脚数目
using std::vector;

class pinLength1Param
{
public:
	explicit pinLength1Param();
	~pinLength1Param();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
		//int threshold;//每1个区域内的阈值
	}segRegion;

	typedef struct {
		double row;
		double col;
		double angle;
		double len1;
		double len2;
	}Rec2Region;

	//定位属性
	int IsReferSection;//是否启用参考模组，边界线

	int PinAreaRefer;  //由前面模块传进来的旋转矩阵，定位管脚区域

	double offsetUp;    //前面模块传进来的本体范围的偏置
	double offsetDown;
	double offsetLeft;
	double offsetRight;

	double roiOffUp;    //前面模块传进来的本体范围的偏置
	double roiOffDown;
	double roiOffLeft;
	double roiOffRight;
	bool IsUseInputRegion;  //是否使用传入区域作为搜索区域

	int IsEdgeSopType;  //使用产品类型区分所用到的识别方法
	int IsReferEdgeRev; //是否使用边界参考对调
	int IsInterUseEdgeAngle; //求取间距sweep等使用参考边界角度
	int IsOnlyReferEdge[2]; //是否仅参考边界角度而不强制对齐

							//管脚定位
	int openAndClosing;            //开闭操作的尺寸大小
								   //int pinLfUpCont,pinRtDwCont;   //管1脚数目,ui上的值
	int pinCont[2];   //管1脚数目,ui上的值
	int LRorUD;                //定位区域方向,0竖直1水平
	int IsWhitePin;                //是1否采用白色管脚，0关闭1启用
	int IsIgnoreRoot;              //是1否忽略根部，0关闭1启用
	int IsThreshold;               //是1否启用阈值处理
	int threValue;                 //阈1值处理的值
	int pinSize;               //管1脚间距
	int leadAreaMin;                  //去1杂min值
	int IsWidth;                   //是否选取宽度
	int widthMin;
	int widthOffset;          //宽度偏移
	int leadContrast;         //管脚识别边界的灰度

	int IsSOPType; //0:qfn,1:sop


				   //结果修正
	double LeadLengthCorr[2];          //左1上长度修正
	double LeadWidthCorr[2];           //左1上宽度修正
	double LeadInterCorr[2];           //左1上间距修正

	double LeadLengthCorrNew[2][MAX_PINGRP_NUM]; //左1上长度修正
	double LeadWidthCorrNew[2][MAX_PINGRP_NUM];  //左1上宽度修正
	double LeadInterCorrNew[2][MAX_PINGRP_NUM];  //左1上间距修正
	int IsUpdateCorr;

	double LeadSpanCorr[MAX_PINGRP_NUM];         //跨距补偿

	double SizeRation[2];            //左上、右下的尺寸比例尺
	int IsInputSize;                 //是否传入比例因子 
									 //误差范围
	int errWidthValid[2];             //左上，启1用宽度，0关闭1启用
	double errWidthMin[2][MAX_PINGRP_NUM], errWidthMax[2][MAX_PINGRP_NUM];
	int errLengthValid[2];            //左上，启1用长度，0关闭1启用
	double errLengthMin[2][MAX_PINGRP_NUM], errLengthMax[2][MAX_PINGRP_NUM];
	int errInterValid[2];             //左上，启1用间距，0关闭1启用
	double errInterMin[2][MAX_PINGRP_NUM], errInterMax[2][MAX_PINGRP_NUM];
	int errMaxDiffValid[2];           //左上，启1用最大长度差异，0关闭1启用
	double errMaxDiffMin[2];
	double errMaxDiff[2];
	int errSpanValid;                 //跨距
	double errSpanMin[MAX_PINGRP_NUM];
	double errSpanMax[MAX_PINGRP_NUM];

	int errSweepValid[2];

	int sweepOffset[2];

	int IsHNW;  //恒诺微的需求临时加了特殊的跨距求取方式

				//cross
	int CrossGrayValid;     //启用穿越值
	int crossGray;
	int crossStep;          //边界厚度


	int errAllMaxDiffValid;            //全1部长度最大差异
	double errAllMaxDiffMin;
	double errAllMaxDiff;

	//ROI信息
	vector<segRegion> segRegionVector;   //0为左/上ROI，1为右/下ROI，剩余的为左右管脚定位区1域
	//int segRoiCounts[2];    //分割出来的左上区域roi数目，不同于UI上的管脚数目

	segRegion rotateRoi;  //设置旋转中心使用的roi
	Rec2Region SearchEdgeRoi;   //搜索的时候排除外部区域的ROI
	int IsDelOffSearchRoi;      //是否排除（涂黑）搜索区域外
	int backGray;               //背景灰度    

	int IsUseDir;         //是否使用角度属性用以旋转管脚区域的方向
	int AngleType;        //0: 0/180   1:0/90/180/270
	int startAngle;       //起始角度，与前置极性模块保持一致

	int IsShowAfterDet;   //检测完成后是否显示区域
	int leadLossIndex;    //缺脚的序号
	int IsGroupEnableSame;  //不同管脚组检测项目是否统一

	bool IsNoDetectTop;     //是否检测脚尖点

private:
	void iniData();
};

