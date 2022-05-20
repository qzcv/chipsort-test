#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "surfaceMultParam.h"

using namespace qzcv;
using namespace Halcon;

class surfaceMultMod : public UnitModule
{
public:
	explicit surfaceMultMod();
	explicit surfaceMultMod(const QString&);
	~surfaceMultMod();
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
	int calculate(Hobject *disp = nullptr);
	void mselect_shape(const Hobject &Regions, Hobject *SelectedRegions, const char *Features, const char *Operation, double Min, double Max);
private:
	surfaceMultParam* m_param;
	friend class surfaceMultWdg;
	HTuple m_homeMat2D;  //转换矩阵
	Hobject *m_image;
	typedef enum { noDet = 0, emptyImg, wrongMat, detFailed, detSucceed }detRes;
	int m_detRes;
	int m_allok;

	Hlong m_objCnt[MAX_Rec_Num + MAX_Cir_Num];
	bool m_objCntOK[MAX_Rec_Num + MAX_Cir_Num];

	double m_singleBad[MAX_Rec_Num + MAX_Cir_Num];
	bool m_singleBadOK[MAX_Rec_Num + MAX_Cir_Num];
	Hobject m_singleBadMax[MAX_Rec_Num + MAX_Cir_Num];

	vector<double> m_lenVec[MAX_Rec_Num + MAX_Cir_Num];
	vector<double> m_widVec[MAX_Rec_Num + MAX_Cir_Num];
	vector<bool> m_lenOKVec[MAX_Rec_Num + MAX_Cir_Num];
	vector<bool> m_widOKVec[MAX_Rec_Num + MAX_Cir_Num];

	double m_totalBad[MAX_Rec_Num + MAX_Cir_Num];
	bool m_totalBadOK[MAX_Rec_Num + MAX_Cir_Num];

	bool m_RoiOK[MAX_Rec_Num + MAX_Cir_Num];

	Hobject* m_delRegion[3];
	Hobject* m_inAreaRegion; //传进来的区域指针
	Hobject* m_showUnionDelReg;	//运行时计算得到的合并后的排除区域
	Hobject* m_showUnionRoiReg;	//运行时计算得到的合并后的ROI区域
	Hobject* m_detectRegion[MAX_Rec_Num + MAX_Cir_Num];

	HTuple m_TmpReferRow;
	HTuple m_TmpReferCol;

private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2D;
	UnitInputPin<QList<double>> p_pixelSize;
	UnitInputPin<QList<cv::Point2d>> p_corners;
	UnitInputPin<RlRegion> p_inReg;
	UnitInputPin<RlRegion> p_delReg[3];
	UnitInputPin<bool> p_changeMode;
	UnitInputPin<cv::Mat> p_delHomMat2D;
	UnitInputPin<int> p_dirIdx;

	UnitOutputPin<RlRegion> p_outReg;
};

