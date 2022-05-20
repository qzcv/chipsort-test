#pragma once
#include <QString>
#include <vector>
#include <HalconCpp.h>

using namespace Halcon;
using std::vector;
#define MAX_MARK_NUM 4
#define MAX_CHAR_NUM 50

class mark1Param
{
public:
	explicit mark1Param();
	~mark1Param();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
	}segRegion;

	//定位属性
	int IsReferSection;//参考模组 0不使用参考，1使用传入参考，2使用自身字符区域参考

	//学习模块
	int whiteChar;           //启用白色物体判断,0为关闭,1启用
	int autoSegment;         //启用自动分割,0为关闭,1启用
	int threshold;           //阈值分割的值
	int minArea;             //自动分割中的最小面积
	int minHoles;            //形成孔洞的最小面积
	int charCounts[MAX_MARK_NUM];         //字符数目
	int charOffsetValid;    //启用时将会仅在charRegion的偏移范围内搜索对应的模板
	int charOffset;         //字符偏移
	int searchAngleValid;   //是否启用搜寻角度
	int searchAngle;        //搜寻角度
	int autoRegionLarge;    //自动模板模式扩大边角

	int charThreMin;        //传出去的字符分割的区域的二值
	int charThreMax;
	int IsFillUp;           //是否填充
	int areaType;           //检测面积偏差时候的原则，0-检测多余和缺少，1-仅检测多余部分，2-仅检测缺少部分

	int IsReferTempAngle;   //是否在判断偏移角度的时候将整体字符模板的角度减去

	//ocr
	int OcrValid; //是否通过OCR模式执行模板设置
	//自动存储模板图片
	char AutoSaveTmpImgDir[256];


	//误差范围
	int acceptSimilar;
	int errAngleValid;
	double errAngle;
	int errOffSetValid; //位移
	int errOffSet; //位移变化

	int errOffXValid;  //
	double errOffXMin;
	double errOffXMax;

	int errOffYValid;  //
	double errOffYMin;
	double errOffYMax;

	int errAreaValid;     //面积偏差判断
	int errAreaPercent;   //面积偏差百分比

	int errExcessValid;  //字符多余判断
	double errExcess;    //多余的最小尺寸

	int errCrackValid;   //字符残缺判断
	double errCrack;     //残缺的最小尺寸

	int IsInputRatio;    //是否传入比例
	double pixelSize;

	//ROI信息
	int PatternNum;          //多模板的数目，4-10
	segRegion RegionRoi[MAX_MARK_NUM];             //搜索区域，也是取整体模板时候的区域
	vector<segRegion> segRegionVector[MAX_MARK_NUM];  //多模板各字符区域模板roi ，代表模板区域在整副图片中的真实位置。取模板的时候需要算相对位置
	Hobject SegImg[MAX_MARK_NUM];   //多模板的各模板的总体区域图片

	int CharDefectTOL;                   //字符缺陷容忍度tolerance（进行闭操作的大小）

	int MultPatternEnable[MAX_MARK_NUM];        //是否启用各模板 

	int IsMult;                      //是否启用多模板

	int IsFontOrLayer;               //多字体还是多图层，多字体即多模板时字符数目一致每字符轮查，多图层即先判断属于哪一个图层
	int NowPatternNum;               //当前模板编号
	int IsDirPattern;                //是否采用多极性模板 将同一模板组进行多个方向的旋转，跟据前面传进来的指数选择旋转角度
	int angleType;                   //角度类型 0:0/180 ， 1： 0/90/180/270                

	int IsUsePinDirOut;   //是否启用方向
	int pin1OutIdx;    //管脚1输出口
	int pin2OutIdx;
	int IsDefaultSig;  //是否启用默认信号
	int IsInverse;     //是否方向反向
	int Is2Dor4D;      //采用两个极性区域还是四个极性区域 
	int isShowAngle;   //是否在结果画面显示角度信息

	int IsInputPatIndex; //是否使用传入模板序号

	int HorOrVer;            //整体横向字符还是竖向字符
	int charGap;             //字符间距，用于在分割区域时候使用

private:
	void iniData();
};

