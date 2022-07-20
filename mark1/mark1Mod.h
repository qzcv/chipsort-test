#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "mark1Param.h"
#include "HalconCpp.h"

#include "chipMod.h"

#define MAX_TESTITEMS_NUM 16

using namespace qzcv;
using namespace Halcon;

class mark1Mod : public ChipMod
{
	Q_OBJECT
public:
	explicit mark1Mod();
	explicit mark1Mod(const QString&);
	~mark1Mod();
	int command(int cmdID, void* dataIn, void* dataOut);
	virtual int afterSetProperty(MetaProperty*);

	public slots:
	void resetOutput();
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

	void creatNccTemp(bool createall, int index = 0);
	void createRegionTemp(int index);   //创建字符区域的模板
	bool autoTemp();  //如果是
	void clearTemp(int stationIdx);
	void saveTemp(bool all);
private:
	mark1Param* m_param;
	friend class mark1Wdg;
	vector<Hlong> m_modelId[MAX_MARK_NUM][4];   //多个模板*4*字符数目
	Hlong m_RegionModelId[MAX_MARK_NUM][4];     //字符整体区域的id
	Hobject *m_image;
	Hobject* m_charRegion;  //传出去的字符区域指针;
	bool m_ocrTempOK;  //ocr自动做模板是否出问题，默认ok
	int m_charRight;    //0正常，1字数为零，2 在选用多模板且多字体的模式时使用了不同的字符数目,3 选用的segregion的区域超过了搜寻区域regionRoi
	int m_charWrongIndex; //m_charRight==3时候对应错误的模板号
	bool m_PatternEnable;  //没有任意一个模板可供使用时候NG
	bool m_dirSame;   //角度属性内部的角度类型和极性模板内的角度类型是否相同
	HTuple m_homeMat2D;
	double m_row[5], m_col[5];  //前面模块传进来的定位框，左上角顺时1针

	bool m_testItemsStatus[MAX_TESTITEMS_NUM];
	bool m_producting;	//从station2中结构中继承下来的参数，用于ocr的自动训练模板。目前先把该参数状态置为true。
	bool m_ocrOutReg;

private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2d;
 	UnitInputPin<double> p_pixelSize;
	UnitInputPin<QList<cv::Point2d>> p_corners;
	UnitInputPin<int> p_dirIdx;
	UnitInputPin<int> p_patIdx;

	UnitInputPin<bool> p_doOcr;
	UnitInputPin<int> p_ocrNum;
	UnitInputPin<QList<double>> p_ocrRow1;
	UnitInputPin<QList<double>> p_ocrRow2;
	UnitInputPin<QList<double>> p_ocrCol1;
	UnitInputPin<QList<double>> p_ocrCol2;

	UnitOutputPin<RlRegion> p_charReg;
	UnitOutputPin<cv::Mat> p_segImg;
	UnitOutputPin<int> p_lfIdx;
	UnitOutputPin<int> p_rtIdx;
	UnitOutputPin<bool> p_lf;
	UnitOutputPin<bool> p_rt;
	UnitOutputPin<int> p_outDirIdx;	//传出的方向指数
	UnitOutputPin<bool> p_autoOcrOk;
};

