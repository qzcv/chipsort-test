#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "chipMod.h"

#define CenterRow "centerRow"     //中心点坐1标
#define CenterCol "centerCol"
#define MAX_TESTITEMS_NUM 16

using namespace qzcv;
using namespace Halcon;
class locationParam;
class DataStream;

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

class locationMod : public ChipMod
{
public:
	explicit locationMod();
	explicit locationMod(const QString&);
	~locationMod();
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
private:
	locationParam* m_param;
	friend class locationWdg;
	Hobject *m_image;
	bool m_testItemsStatus[MAX_TESTITEMS_NUM];
	bool m_standUseOk;           //站高检测是否全部未打开

	HTuple m_k;  //像素比例,5,中左右上下
	HTuple m_lineAngle; //四边角度,4,左右上下
	HTuple m_prjRatio;  //投影比例，2，左右，sin&
	HTuple m_actualDepth; //填进来的实际槽深，2，左右
private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<QList<double>> p_pixelSize;
	UnitInputPin<QList<double>> p_LineAngle;
	UnitInputPin<QList<double>> p_ActualDepth;
	UnitInputPin<QList<double>> p_prjRatio;

	UnitOutputPin<QList<cv::Point2d>> p_corners;

	UnitOutputPin<bool> p_StandOffEnable;
	UnitOutputPin<QList<double>> p_StandOffOut;

	UnitOutputPin<bool> p_PinWidthEnable;
	UnitOutputPin<QList<double>> p_PinWidthOut;

	UnitOutputPin<bool> p_PinInterValEnable;
	UnitOutputPin<QList<double>> p_PinInterValOut;

	UnitOutputPin<bool> p_StandOffDiffEnable;
	UnitOutputPin<QList<double>> p_StandOffDiffOut;

	UnitOutputPin<bool> p_ResultOutOk;
	UnitOutputPin<QList<int>> p_PinCountOut;
};

