#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "fitEdge.h"
#include "chipMod.h"

#define MAX_TESTITEMS_NUM 16
#define CenterRow "centerRow"     //中心点坐1标
#define CenterCol "centerCol"    

using namespace qzcv;
using namespace danke;
class edgeLocationParam;

static fitEdgeLine::Orient m_dir[2][4] = { { fitEdgeLine::ToUp,fitEdgeLine::ToRight,fitEdgeLine::ToDown,fitEdgeLine::ToLeft }
,{ fitEdgeLine::ToDown,fitEdgeLine::ToLeft,fitEdgeLine::ToUp,fitEdgeLine::ToRight } };

class edgeLocationMod : public ChipMod
{
public:
	explicit edgeLocationMod();
	explicit edgeLocationMod(const QString&);
	~edgeLocationMod();
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
	void iniDetector();
private:
	edgeLocationParam* m_param;
	friend class edgeLocationWdg;
	Hobject *m_image;
	fitEdgeLine* m_edgeLine[4];
	bool m_testItemsStatus[MAX_TESTITEMS_NUM];
private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2d;
	UnitInputPin<double> p_pixSize;

	UnitOutputPin<bool> p_allok;
	UnitOutputPin<QList<cv::Point2d>> p_corners;
	UnitOutputPin<cv::Point2d> p_offCenter;
};

