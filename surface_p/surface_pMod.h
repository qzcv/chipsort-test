#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include <vector>

using namespace qzcv;
using namespace Halcon;
using std::vector;
class surface_pParam;

class surface_pMod : public UnitModule
{
public:
	explicit surface_pMod();
	explicit surface_pMod(const QString&);
	~surface_pMod();
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
	void calTargetReg(Hobject *obj, Hobject *showTmp = nullptr);
	void mselect_shape(const Hobject &Regions, Hobject *SelectedRegions, const char *Features, const char *Operation, double Min, double Max);
private:
	surface_pParam* m_param;
	friend class surface_pWdg;
	HTuple m_homeMat2D;  //转换矩阵
	Hobject *m_image;
	typedef enum { noDet = 0, emptyImg, wrongMat, detFailed, detSucceed }detRes;
	int m_detRes;
	int m_allok;

	Hlong m_objCnt;
	bool m_objCntOK;

	double m_singleBad;
	bool m_singleBadOK;
	Hobject m_singleBadMax;

	vector<double> m_lenVec;
	vector<double> m_widVec;
	vector<bool> m_lenOKVec;
	vector<bool> m_widOKVec;

	double m_totalBad;
	bool m_totalBadOK;

	Hobject* m_delRegion[3];
	Hobject* m_inAreaRegion; //传进来的区域指针
	Hobject* m_showUnionDelReg;	//运行时计算得到的合并后的排除区域
	Hobject* m_showUnionRoiReg;	//运行时计算得到的合并后的ROI区域

private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2D;
	UnitInputPin<QList<double>> p_pixelSize;
	UnitInputPin<QList<cv::Point2d>> p_corners;
	UnitInputPin<RlRegion> p_inReg;
	UnitInputPin<RlRegion> p_delReg[3];

	UnitOutputPin<RlRegion> p_outReg;
};

