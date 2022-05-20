#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "nccMatch_outNParam.h"
#include <vector>
#include "HalconCpp.h"

using namespace Halcon;
using namespace qzcv;
using std::vector;

class NCCMatcher;

class nccMatch_outNMod : public UnitModule
{
public:
	explicit nccMatch_outNMod();
	explicit nccMatch_outNMod(const QString&);
	~nccMatch_outNMod();
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
	void resetPattern(int index);
	double distance_of_two_point(QPointF point1, QPointF point2);
private:
	nccMatch_outNParam* m_param;
	friend class nccMatch_outNWdg;
	Hobject* m_image;
	NCCMatcher *m_detect[MAX_TEMP_NUM];
	bool has_set_pattern[MAX_TEMP_NUM];

	typedef enum {
		noDet = 0, emptyImg, patrnLoss, emptyOutImg, detFail, detSucceed
	}detRes;
	int m_detRes[MAX_TEMP_NUM];
	int m_allok;

	bool m_patternOK[MAX_TEMP_NUM];

	vector<bool> m_acceptOK[MAX_TEMP_NUM];
	vector<double> m_acceptVal[MAX_TEMP_NUM];

	vector<bool> m_offsetOK[MAX_TEMP_NUM];
	vector<double> m_offsetVal[MAX_TEMP_NUM];

	vector<bool> m_relativeOK[MAX_TEMP_NUM];
	vector<double> m_relativeVal[MAX_TEMP_NUM];

	vector<bool> m_axisOK[MAX_TEMP_NUM];
	vector<double> m_axisVal[MAX_TEMP_NUM];
private:
	UnitInputPin<cv::Mat> p_im;

	UnitOutputPin<cv::Mat> p_homMat2D[MAX_TEMP_NUM];	//用于隐藏HTuple和Hobject类型
	UnitOutputPin<RlRegion> p_outArea[MAX_TEMP_NUM];

	HTuple m_homMat2D[MAX_TEMP_NUM];
	Hobject m_outArea[MAX_TEMP_NUM];
};