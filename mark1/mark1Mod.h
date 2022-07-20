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
	void createRegionTemp(int index);   //�����ַ������ģ��
	bool autoTemp();  //�����
	void clearTemp(int stationIdx);
	void saveTemp(bool all);
private:
	mark1Param* m_param;
	friend class mark1Wdg;
	vector<Hlong> m_modelId[MAX_MARK_NUM][4];   //���ģ��*4*�ַ���Ŀ
	Hlong m_RegionModelId[MAX_MARK_NUM][4];     //�ַ����������id
	Hobject *m_image;
	Hobject* m_charRegion;  //����ȥ���ַ�����ָ��;
	bool m_ocrTempOK;  //ocr�Զ���ģ���Ƿ�����⣬Ĭ��ok
	int m_charRight;    //0������1����Ϊ�㣬2 ��ѡ�ö�ģ���Ҷ������ģʽʱʹ���˲�ͬ���ַ���Ŀ,3 ѡ�õ�segregion�����򳬹�����Ѱ����regionRoi
	int m_charWrongIndex; //m_charRight==3ʱ���Ӧ�����ģ���
	bool m_PatternEnable;  //û������һ��ģ��ɹ�ʹ��ʱ��NG
	bool m_dirSame;   //�Ƕ������ڲ��ĽǶ����ͺͼ���ģ���ڵĽǶ������Ƿ���ͬ
	HTuple m_homeMat2D;
	double m_row[5], m_col[5];  //ǰ��ģ�鴫�����Ķ�λ�����Ͻ�˳ʱ1��

	bool m_testItemsStatus[MAX_TESTITEMS_NUM];
	bool m_producting;	//��station2�нṹ�м̳������Ĳ���������ocr���Զ�ѵ��ģ�塣Ŀǰ�ȰѸò���״̬��Ϊtrue��
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
	UnitOutputPin<int> p_outDirIdx;	//�����ķ���ָ��
	UnitOutputPin<bool> p_autoOcrOk;
};

