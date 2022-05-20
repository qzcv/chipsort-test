#pragma once
#include <QString>
#include <vector>

using std::vector;

class ColorPickerParam
{
public:
	explicit ColorPickerParam();
	~ColorPickerParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		int regionType;  //0零为旋转矩形，
		double RecRow;
		double RecCol;
		double RecAngle;
		double RecLength1;
		double RecLength2;
		double CirR;
		double CirRow;
		double CirCol;
	}segRegion;

	int IsShowAllRegion;  //是否在选择区域时画出其余区域

	int IsReferSection;//参考模组
	int SaturationMin; //饱和度
	int SaturationMax; //饱和度
	int HueMin;        //色调
	int HueMax;        //色调

	int IsOpening;     //是否开操作
	int openVal;       //开操作值
	int IsClosing;     //是否闭操作
	int closeVal;      //闭操作值


	int IsNotLess;     //0为此种颜色在区域内至多，1为在区域内此种颜色不少于总面积多少
	int areaMin, areaMax;  //面积极值
	int IsShowRegionAfterDetect;  //检测完是否显示区域
	int IsFillUpHole;     //是否填充孔洞

	int IsDeleteLightArea;  //是否去除明亮区域
	int lightAreaThreshold; //明亮区域阈值
	int lightArea; //明亮区最小面积

	int IsHeight;
	int heightMin, heightMax;
	int IsWidth;
	int widthMin, widthMax;

	int nowIsRecOrCir;       //现在是矩形0还是圆形1
	int roiCount;              //roi数目;
	int MinusCount;            //排除区域数目
	int currentRoiIndex;       //当前区域指数
	int currentMinusIndex;     //当前排除区域指数
	vector<segRegion> RoiVec;  //roi搜索区域
	vector<segRegion> MinusVec;  //排除区域

	int IsErrAllArea;
	int errAllMin, errAllMax;
	int IsErrSingleArea;
	int errSingleMin, errSingleMax;

	int referType;        //区域参考类型， 0：自选区域参考，1：面区域参考，2：点区域参考
	int AreaZoom;         //区域胀缩值

private:
	void iniData();
};

