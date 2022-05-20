#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "chipMod.h"

#define CenterRow "centerRow"     //���ĵ���1��
#define CenterCol "centerCol"   

using namespace qzcv;
using namespace Halcon;
class locationGuiParam;

typedef struct {
	float col;
	float row;
}FloatPoint;

typedef struct {
	FloatPoint Start;
	FloatPoint End;
}Line;

class locationGuiMod : public ChipMod
{
public:
	explicit locationGuiMod();
	explicit locationGuiMod(const QString&);
	~locationGuiMod();
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
	FloatPoint*  GetLineIntersection(Line RefLine1, Line RefLine2);
private:
	locationGuiParam* m_param;
	friend class locationGuiWdg;
	Hobject *m_image;

	HTuple m_k;  //���ر���,5,����������
	HTuple m_lineAngle; //�ı߽Ƕ�,4,��������
	HTuple m_prjRatio;  //ͶӰ������2�����ң�sin&
	HTuple m_actualDepth; //�������ʵ�ʲ��2������
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

	void toHtuple(const UnitInputPin<QList<double>> &p, HTuple &htuple);

	void toQList(const HTuple &htuple, UnitOutputPin<QList<double>> &p);
};

