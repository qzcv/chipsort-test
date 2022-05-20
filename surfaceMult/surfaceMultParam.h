#pragma once
#include <QString>
#include <vector>

using std::vector;
#define MAX_Rec_Num 10 //7
#define MAX_Cir_Num 3  //3

#define MAX_Delete_Rec_Num 10   //7
#define MAX_Delete_Cir_Num 3    //5

class surfaceMultParam
{
public:
	explicit surfaceMultParam();
	~surfaceMultParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row;
		double col;
		double length1;
		double length2;
		double angle;
	}RRECReg;    //矩形ROI区域

	typedef struct {
		double rrow;
		double rcol;
		double r;
	}RCirReg;   //圆形ROI区域

	typedef struct {
		double drow;
		double dcol;
		double dlength1;
		double dlength2;
		double dangle;
	}DRECReg;    //矩形delete区域

	typedef struct {
		double drrow;
		double drcol;
		double dr;
	}DCirReg;   //圆形DELETE区域

				//检测区域
	int DetectModel;  //检测模式，0为缺陷模式，1为目标模式,区别在于修改各参数名称，以及未检测到目标时的处理
	int IsChangeModel;//是否根据外部传入信息修改缺陷、目标模式

	int deleteRefer;  //使用传入区域时排除区域使用参考矩阵
	int referSection; //旋转矩阵参考模组
	int pointRegionRefer;  //传进来的是由点组成的区域而不是旋转矩阵
	int areaRegionRefer;  //传进来的是由面积区域而不是旋转矩阵

	int areaZoom;              //对所有区域的胀缩

	int zoom_up;
	int zoom_down;
	int zoom_lf;
	int zoom_rt;  //传入点区域的缩放

	int IsSameParam;    //是否使用同一组数据

						//极性区域设置
	int multAngle;     //默认的设置多模板的角度，0-90,1-180,2-270
	double rotateRow1, rotateRow2, rotateCol1, rotateCol2;  //旋转区域


															//排除区域
	int deleteArea_section[3];  //排除模组选择
	int deleteArea_zoom[3];

	int isShowDel;         //是否测后显示排除区域

	int IsPixOrAct;  //是像素尺寸还是实际尺寸，0像素，1实际
	double pixSize;            //比例尺大小

							   //瑕疵定义
	double badArea_min[MAX_Rec_Num + MAX_Cir_Num];
	double badArea_max[MAX_Rec_Num + MAX_Cir_Num];
	int black_bad_detect[MAX_Rec_Num + MAX_Cir_Num];
	int white_bad_detect[MAX_Rec_Num + MAX_Cir_Num];
	double badLengthMin[MAX_Rec_Num + MAX_Cir_Num];
	double badLengthMax[MAX_Rec_Num + MAX_Cir_Num];
	double badWidthMax[MAX_Rec_Num + MAX_Cir_Num];
	double badWidthMin[MAX_Rec_Num + MAX_Cir_Num];
	//int IsFixThre[MAX_Rec_Num+MAX_Cir_Num];    //是否使用固定阈值
	int fixThreMin[MAX_Rec_Num + MAX_Cir_Num];
	int fixThreMax[MAX_Rec_Num + MAX_Cir_Num];
	int IsGauss[MAX_Rec_Num + MAX_Cir_Num];      //是否对处理的原图进行高斯滤波
	int GaussSize[MAX_Rec_Num + MAX_Cir_Num];    //高斯滤波核的大小

	int closeVal[MAX_Rec_Num + MAX_Cir_Num];
	int openVal[MAX_Rec_Num + MAX_Cir_Num];

	int meanSize[MAX_Rec_Num + MAX_Cir_Num];   //采用平滑背景时候的
											   //int IsMeanSize[MAX_Rec_Num+MAX_Cir_Num]; //是否采用平滑背景
	int IsFillOut[MAX_Rec_Num + MAX_Cir_Num];  //采用平滑背景的时候外部是否填充
											   //int IsDetHole[MAX_Rec_Num+MAX_Cir_Num];
	int holeAreaMin[MAX_Rec_Num + MAX_Cir_Num];
	int holeAreaMax[MAX_Rec_Num + MAX_Cir_Num];
	int holeNumMin[MAX_Rec_Num + MAX_Cir_Num];
	int holeNumMax[MAX_Rec_Num + MAX_Cir_Num];
	//int IsRagLoss[MAX_Rec_Num+MAX_Cir_Num];  //是否检测毛刺和缺失，毛刺对应开操作，缺失对应闭操作
	int minRag[MAX_Rec_Num + MAX_Cir_Num];     //处理毛刺时开操作的值
	int minLoss[MAX_Rec_Num + MAX_Cir_Num];    //处理缺失时候的闭操作的值
	int minRagLossArea[MAX_Rec_Num + MAX_Cir_Num]; //检测出来的毛刺缺失的最小值


												   //误差范围
	int single_badArea_valid[MAX_Rec_Num + MAX_Cir_Num];
	double single_badArea_min[MAX_Rec_Num + MAX_Cir_Num];
	double single_badArea_max[MAX_Rec_Num + MAX_Cir_Num];
	int total_badArea_valid[MAX_Rec_Num + MAX_Cir_Num];
	double total_badArea_min[MAX_Rec_Num + MAX_Cir_Num];
	double total_badArea_max[MAX_Rec_Num + MAX_Cir_Num];
	int area_width_valid[MAX_Rec_Num + MAX_Cir_Num];
	double area_width_min[MAX_Rec_Num + MAX_Cir_Num];
	double area_width_max[MAX_Rec_Num + MAX_Cir_Num];
	int area_length_valid[MAX_Rec_Num + MAX_Cir_Num];
	double area_length_min[MAX_Rec_Num + MAX_Cir_Num];
	double area_length_max[MAX_Rec_Num + MAX_Cir_Num];

	int area_count_valid[MAX_Rec_Num + MAX_Cir_Num];
	int area_count_min[MAX_Rec_Num + MAX_Cir_Num];
	int area_count_max[MAX_Rec_Num + MAX_Cir_Num];

	RRECReg RecRoi[MAX_Rec_Num];    //存储的矩形区域
	DRECReg DeleteRec[MAX_Delete_Rec_Num];    //存储的矩形排除区域

	RCirReg CirRoi[MAX_Cir_Num];       //存储的圆形区域
	DCirReg DeleteCir[MAX_Delete_Cir_Num];    //存储的圆形排除区域

	int RecRoiNum;             //矩形roi区域数目
	int RecDeleteNum;          //矩形排除区域数目
	int CirRoiNum;             //圆形roi区域数目
	int CirDeleteNum;          //圆形排除区域数目
	int currentRoi;            //当前显示的ROI指数
	int IsDirPattern;          //是否采用极性区域（采用哪一个检测区域取决于前面传进来的指数）
	int IsDirDelete;           //在采用极性区域检测的时候，排除区域是一个一个对应极性还是所有区域全部排除,0极性排除，1全部排除
	int IsReferPtoDel;         //是否引用参考点区域作为排除区域
	int delPointZoom;          //引用参考点区域的胀缩 

	typedef enum { averageThre = 0, fixedThre, meanThre, detHole, ragAndLoss }TestType;    //检测类型
	TestType testType[MAX_Rec_Num + MAX_Cir_Num];   //每个ROI的检测类型
	int tmptype[MAX_Rec_Num + MAX_Cir_Num];   //帮助存储枚举值的

	bool IsDispROI;
private:
	void iniData();
};

