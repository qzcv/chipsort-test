#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "nccMatch_fourDirParam.h"

using namespace qzcv;
using namespace Halcon;

class NCCMatcher;

class nccMatch_fourDirMod : public UnitModule
{
public:
	explicit nccMatch_fourDirMod();
	explicit nccMatch_fourDirMod(const QString&);
	~nccMatch_fourDirMod();
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
	void resetPattern(int index);
private:
	nccMatch_fourDirParam* m_param;
	friend class nccMatch_fourDirWdg;
	Hobject *m_image;
	bool has_set_pattern[MAX_PAT_NUM];
	NCCMatcher *m_detect[MAX_PAT_NUM];

	typedef enum {
		noDet = 0, emptyImg, patrnLoss, defaultSig, emptyOutImg, detFail, detSucceed
	}detRes;
	int m_detRes[MAX_PAT_NUM];
	int m_allOk;
	bool m_resOk;
//	int m_chooseIdx;
	bool m_matchAngleOk;

	int m_count[MAX_PAT_NUM];

	vector<bool> m_acceptOK[MAX_PAT_NUM];
	vector<double> m_acceptVal[MAX_PAT_NUM];

	vector<bool> m_relativeOK[MAX_PAT_NUM];
	vector<double> m_relativeVal[MAX_PAT_NUM];

private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_inHomMat2d;

	UnitOutputPin<cv::Mat> p_outHomMat2d;
	UnitOutputPin<int> p_chooseIdx;
	//UnitOutputPin<int> p_lfIdx;
	//UnitOutputPin<int> p_rtIdx;
	UnitOutputPin<bool> p_lfDir;
	UnitOutputPin<bool> p_rtDir;
	UnitOutputPin<RlRegion> p_outArea;

//	HTuple m_inHomMat2d;

	HTuple m_outHomMat2d;
	Hobject *m_outArea;
};

