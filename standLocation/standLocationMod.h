#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "chipMod.h"

using namespace qzcv;
using namespace Halcon;
class standLocationParam;

#define MAX_TESTITEMS_NUM 16

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

class standLocationMod : public ChipMod
{
public:
	explicit standLocationMod();
	explicit standLocationMod(const QString&);
	~standLocationMod();
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
	standLocationParam* m_param;
	friend class standLocationWdg;
	Hobject *m_image;
	bool m_standUseOk;           //վ�߼���Ƿ�ȫ��δ��

	bool m_testItemsStatus[MAX_TESTITEMS_NUM];

	HTuple m_k;  //���ر���,5,����������
	HTuple m_lineAngle; //�ı߽Ƕ�,4,��������
	HTuple m_prjRatio;  //ͶӰ������2�����ң�sin&
	HTuple m_actualDepth; //�������ʵ�ʲ��2������

private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<QList<double>> p_pixSize;
	UnitInputPin<QList<double>> p_lAng;
	UnitInputPin<QList<double>> p_aDepth;
	UnitInputPin<QList<double>> p_ratio;
	UnitInputPin<QList<double>> p_edgeRowBeg;
	UnitInputPin<QList<double>> p_edgeRowEnd;
	UnitInputPin<QList<double>> p_edgeColBeg;
	UnitInputPin<QList<double>> p_edgeColEnd;
	UnitInputPin<QList<double>> p_leadLen;

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

