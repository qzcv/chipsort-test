#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "fitEdge.h"
#include "chipMod.h"
#include "calib3D5S_homParam.h"

#define ENT_CALIB "calib"

using namespace qzcv;
using namespace danke;
namespace qzcv {
	class Hom3d5S;
	class PlaneSurface;
	class RigidTrans3D;
}


class calib3D5S_homMod : public ChipMod
{
public:
	explicit calib3D5S_homMod();
	explicit calib3D5S_homMod(const QString&);
	~calib3D5S_homMod();
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
	void createEvents();
	void iniDetector();

	bool detectNormal();
	bool detectTrans();

	void viewResultNormal(ImageView *iv);
	void viewResultTrans(ImageView *iv);

	void textResultNormal(ResultText *text);
	void textResultTrans(ResultText *text);
private:
	calib3D5S_homParam* m_param;
	friend class calib3D5S_homWdg;

	fitEdgeLine* m_BottomFitLine[BottomNum];
	fitEdgeLine* m_sideFitLine[2][SideNum];
	fitEdgeLine* m_transBtFitLine[BtTransNum];
	fitEdgeLine* m_transSideFitLine[SideTransNum];
	fitEdgeLine* m_veryFitLine[VerifyNum];

	bool m_inDataOk;
	Hom3d5S* m_hom3D5S[3];
	Hom3d5S* m_homTrans[2];
	RigidTrans3D* m_rigidTrans;
	PlaneSurface* m_plane;

	cv::Point3d m_sideWorldTrans[4];  //标定中测算出的z轴0面的几个平面点

	Hobject *m_image;

private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<QList<double>> p_first3dIn;

	UnitOutputPin<QList<double>> p_3d5s;
	UnitOutputPin<QList<double>> p_first3dOut;
};

