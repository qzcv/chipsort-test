#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "RlRegion.h"
#include <vector>
#include "HalconCpp.h"

using std::vector;
using namespace qzcv;
using namespace Halcon;
class orientAreaParam;
class orientAreaWidget;

class orientAreaModule : public UnitModule
{
public:
	explicit orientAreaModule();
	explicit orientAreaModule(const QString&);
	~orientAreaModule();
	int command(int cmdID, void* dataIn, void* dataOut);
	virtual int afterSetProperty(MetaProperty*);
protected:
	virtual void save(const QString& dirPath);
	virtual void load(const QString& dirPath);
	virtual void load(const QString& dirPath, QvsParamLevel level);
	virtual int run(const QString& funName);
	virtual void viewResult(ImageView* iv, const QString& funName,int);
	virtual void textResult(ResultText* text, const QString& funName);
private:
	void iniData();
	void createPins();	
	void createEvents();
	void setDirPinVisible(bool isCreate);
	void selectShape(const RlRegion &lReg, const RlRegion &rReg = RlRegion());

private:
	orientAreaParam* m_param;
	friend class orientAreaWidget;
	RlRegion m_reg[2];
	typedef enum { noDet = 0, emptyImg, wrongMat, detFailed, detSucceed }detRes;
	int m_detRes;

private:
	UnitInputPin<cv::Mat> p_img;
	UnitInputPin<cv::Mat> p_homMat2d;

	UnitOutputPin<RlRegion> p_reg;
	//UnitOutputPin<int> p_lfIdx;
	UnitOutputPin<bool> p_lfDir;
	//UnitOutputPin<int> p_rtIdx;
	UnitOutputPin<bool> p_rtDir;
	UnitOutputPin<bool> p_pickIdx;
	//UnitOutputPin<int> p_noneNGIdx;
	UnitOutputPin<bool> p_noneNGValid;
	UnitOutputPin<bool> p_find;

private:
	//由run函数传递给textResult,viewResult的参数
	int m_sz[2];
	int m_area[2];
	bool m_isOK[2];
	RlRegion m_affineReg[2];
};

