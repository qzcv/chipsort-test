#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "chipMod.h"

using namespace qzcv;
using namespace Halcon;
class standLocationNewParam;
namespace qzcv
{
	class Hom3d5S;
	class PlaneSurface;
}

typedef struct {
	float col;
	float row;
}FloatPoint;

typedef struct {
	FloatPoint Start;
	FloatPoint End;
}Line;

#define MAX_TESTITEMS_NUM 15

class standLocationNewMod : public ChipMod
{
public:
	explicit standLocationNewMod();
	explicit standLocationNewMod(const QString&);
	~standLocationNewMod();
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
	standLocationNewParam* m_param;
	friend class standLocationNewWdg;
	Hobject *m_image;

	bool m_testItemsStatus[MAX_TESTITEMS_NUM];
	bool m_standUseOk;           //站高检测是否全部未打开

	Hom3d5S* m_hom3D5S[5]; //中上下左右
	PlaneSurface* m_planeSurface;
private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<QList<double>> p_3d5s;
	UnitInputPin<QList<cv::Point2d>> p_edgeBeg;
	UnitInputPin<QList<cv::Point2d>> p_edgeEnd;
	UnitInputPin<QList<double>> p_len;
	UnitInputPin<QList<cv::Point2d>> p_3dCorners;
	UnitInputPin<QList<cv::Point2d>> p_leadPos;

	UnitOutputPin<QMap<QString, QVariant>> p_item;
};

