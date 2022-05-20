#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "chipMod.h"

using namespace qzcv;
using namespace Halcon;
class standEdgeParam;

#define MAX_TESTITEMS_NUM 16
#define CenterRow "centerRow"     //中心点坐1标
#define CenterCol "centerCol"  

typedef struct {
	float col;
	float row;
}FloatPoint;

typedef struct {
	FloatPoint Start;
	FloatPoint End;
}Line;

struct stPoint {
	double x;
	double y;
};

class standEdgeMod : public ChipMod
{
public:
	explicit standEdgeMod();
	explicit standEdgeMod(const QString&);
	~standEdgeMod();
	int command(int cmdID, void* dataIn, void* dataOut);
	virtual int afterSetProperty(MetaProperty*);
protected:
	virtual void save(const QString& dirPath);
	virtual void load(const QString& dirPath, QvsParamLevel level);
	virtual int run(const QString& funName);
	virtual void viewResult(ImageView* iv, const QString& funName,int);
	virtual void textResult(ResultText* text, const QString& funName);
private:
	void iniData();
	void createPins();
	bool IsPointInRegion(const Hobject *InputReg, double row, double col, int Larger = 0);
	bool sortPointsByHisto(const QList<CvPoint2D32f>& inputPoints, QList<QList<CvPoint2D32f>>& outputPoints, 
		int sortGroup, int threshold, int angleRange, bool HorOrVer, int grayStep);
private:
	standEdgeParam* m_param;
	friend class standEdgeWdg;
	Hobject *m_image;

	bool m_testItemsStatus[MAX_TESTITEMS_NUM];
private:
	UnitInputPin<cv::Mat> p_im;

	UnitOutputPin<QList<double>> p_row;			//用于后续中间模块检测管脚长度的边界,最后的四边形的4个点，左上角顺时针
	UnitOutputPin<QList<double>> p_col;
	UnitOutputPin<QList<double>> p_edgeRowBeg;	//四个棱镜上的边缘,用于传出给后续模块使用
	UnitOutputPin<QList<double>> p_edgeRowEnd;
	UnitOutputPin<QList<double>> p_edgeColBeg;
	UnitOutputPin<QList<double>> p_edgeColEnd;
};

