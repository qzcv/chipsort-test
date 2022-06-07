#pragma once
#include <QString>
#include <vector>

#define GRP_NUM 2   //管脚总组数
#define GRP_MAXLEAD 10 //单个管脚组最大管脚数目

using std::vector;

class pinDetect1Param
{
public:
	explicit pinDetect1Param();
	~pinDetect1Param();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
	}RoiRegion;

	typedef struct {
		double row;
		double col;
		double angle;
		double len1;
		double len2;
	}Rec2Region;

	//int onlyUseEdge1;            //后续所有组别均使用第一组求出来的底线和管脚尖的边界
	int currentGrp;              //ui里的当前组别
	int IsRefer[GRP_NUM];        //参考矩阵
	int IsDetBase;      //检测芯片底线，0为不检测底线，1为检测底线，2为传入底线,所有组别使用同一
	int InputBaseIndex[GRP_NUM]; //传入底线的位置，上右下左
	int baseLineOffset[GRP_NUM]; //底线偏移
	int LRorUD;                //管脚方向,0竖直1水平

							   //int grpEnable[GRP_NUM];      //管脚组使能
	int IsBaseInvert[GRP_NUM];     //底线反向，检测方向相反

	int IsReferEdgeRev; //是否使用边界参考对调

	int grpPinNum[GRP_NUM];      //各组的管脚数目

	int whiteBlackDir[GRP_NUM];   //边缘灰度类型,0黑白_1白黑    //上白黑，下黑白,左白黑，右黑白
	int searchDir[GRP_NUM];    //搜寻方向,0左右,1右左，2上下，3下上  
	int extendSearchLine[GRP_NUM];//扩大搜寻线
	int minGrayDiff[GRP_NUM];  //检测站高的最小灰度差异
	int WidthGrayDiff[GRP_NUM];   //检测宽度的灰度差
	int widSearchLength[GRP_NUM]; //检测宽度时候的搜索框长度
	int searchLineGap[GRP_NUM];   //扫描线间距
	int widLineOffset[GRP_NUM];      //求宽度的边线偏移
	int widOffsetRange[GRP_NUM];  //宽度偏移范围
								  //int standoffOffset[GRP_NUM];  //偏离求出来的边线的距离，用以求站高部分的宽度
	int deleteEdge[GRP_NUM];      //去除头尾的区域的几个点

	double clipfactor[GRP_NUM];   //边缘线拟合的系数，1.0,1.5,2.0,2.5,3.0
	int clipDis[GRP_NUM];
	int IsAmpBest[GRP_NUM];       //是否用最大振幅判断最佳管脚边界

	double sizeRatio[GRP_NUM];       //尺寸比例因子
	int IsInputRatio[GRP_NUM];       //是否传入尺寸比例因子

	int CrossGrayValid[GRP_NUM];     //启用穿越值
	int crossGray[GRP_NUM];
	int crossStep[GRP_NUM];          //边界厚度

									 //区域判定
	int minArea[GRP_NUM];   //管脚最小面积，实际上是杂点的最大面积
	int noiseGray[GRP_NUM]; //噪点灰度值，反光等杂点的最小灰度
	int multGrayTimes[GRP_NUM];  //更改灰度多次检测的次数

								 //误差范围 
	int errPinGapValid[GRP_NUM];           //是否启用管脚间距
	double errPinGapMin[GRP_NUM][GRP_MAXLEAD];  //管脚间距最小值；
	double errPinGapMax[GRP_NUM][GRP_MAXLEAD];  //最大值
	int errPinLengthValid[GRP_NUM];      //是否启用站立度判断
	double errPinLengthMin[GRP_NUM][GRP_MAXLEAD];  //站立度最小1值
	double errPinLengthMax[GRP_NUM][GRP_MAXLEAD];  //站立度最大1值
	int errPinStandOffDiffValid[GRP_NUM];  //是否启用站立度差异判断
	double errPinStandOffDiffMin[GRP_NUM];
	double errPinStandOffDiffMax[GRP_NUM];
	int errPinWidthValid[GRP_NUM];         //是否启用管脚宽度
	double errPinWidthMin[GRP_NUM][GRP_MAXLEAD];
	double errPinWidthMax[GRP_NUM][GRP_MAXLEAD];

	int errPinSweepValid[GRP_NUM];
	double errPinSweepMin[GRP_NUM][GRP_MAXLEAD];
	double errPinSweepMax[GRP_NUM][GRP_MAXLEAD];

	int IsSameValid; //是否所有的误差参数的启用valid采用同一值

	int shoulderShift[GRP_NUM];  //肩部sweep基准点求取点偏移refer线的距离

	int widthMethod; //宽度搜索策略,0固定，1最小，2最大，3最小最大,策略同步于sweep

					 //补偿值
	double lengthCorr[GRP_NUM];    //站立度修正
	double widthCorr[GRP_NUM];       //宽度补偿
	double InterCorr[GRP_NUM];       //间距补偿

	double lengthCorrNew[GRP_NUM][GRP_MAXLEAD];    //站立度修正
	double widthCorrNew[GRP_NUM][GRP_MAXLEAD];       //宽度补偿
	double InterCorrNew[GRP_NUM][GRP_MAXLEAD];       //间距补偿
	int isUpdateCorr;    //是否执行对补偿值的更新

						 //autoThre
	RoiRegion AutoThreReg[GRP_NUM]; //自动分割区域
	int ThreMinArea[GRP_NUM];       //分割面积 
	int ThreGrayVal[GRP_NUM];       //分割阈值
	int ThreLeadSize[GRP_NUM];      //管脚推荐尺寸
	int IsAutoWhite[GRP_NUM];       //白色管脚

									//排除区域
	int offsetUp;    //前面模块传进来的本体范围的偏置
	int offsetDown;
	int offsetLeft;
	int offsetRight;
	bool IsUseInputRegion;  //是否使用传入区域作为搜索区域

	vector<RoiRegion> RoiVec[GRP_NUM];  //所有组的管脚搜索框，第一个是检测边界的框，后续为管脚框
	Rec2Region SearchEdgeRoi;        //搜索的时候排除外部区域的ROI
	int IsDelOffSearchRoi;           //是否排除（涂黑）搜索区域外
	int IsShowBlack;

private:
	void iniData();
};

