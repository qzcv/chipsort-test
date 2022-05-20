#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"

using namespace qzcv;
using namespace Halcon;
class ColorPickerParam;

class ColorPickerMod : public UnitModule
{
public:
	explicit ColorPickerMod();
	explicit ColorPickerMod(const QString&);
	~ColorPickerMod();
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
	void mselect_shape(const Hobject &Regions, Hobject *SelectedRegions, const char *Features, const char *Operation, double Min, double Max);
private:
	ColorPickerParam* m_param;
	friend class ColorPickerWdg;
	Hobject *m_image;
	Hobject *m_inAreaRegion;
	Hobject *m_allReg;
	Hobject *m_lightReg;
	Hobject *m_unionMinusReg;
	Hobject *m_unionRoiReg;
	typedef enum {
		noDet = 0, emptyImg, wrongChannelNum, noRoi, detFail, detSucceed
	}detRes;
	int m_detRes;
	int m_allok;

	Hlong m_roiArea;

	Hlong m_singleAreaMin;
	Hlong m_singleAreaMax;
	bool m_singleAreaOK;

	Hlong m_totalArea;
	bool m_totalAreaOK;
private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2d;
	UnitInputPin<QList<cv::Point2d>> p_corners;
	UnitInputPin<RlRegion> p_inReg;
};

