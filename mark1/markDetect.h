#ifndef MARKDETECT_H
#define MARKDETECT_H
#include <vector>
#include <HalconCpp.h>

using namespace Halcon;
using std::vector;

class MarkDetect
{
public:
	typedef struct 
	{
		Hobject region;
	}Result;
	MarkDetect();
	~MarkDetect();

public:
    void setLightDark(int val);      //light=0,dark=1���ð�ɫ/��ɫ����,��ɫ:threshold(m_threshold,255),��֮,threshold(0,m_threshold)
	void setAutoSegment(int val);    //�Ƿ��Զ��ָ�,0/1 ��/��
	void setThreshold(int val);      //�Զ���ֵ�ָ����ֵ
	void setCharCounts(int val);     //�ַ���Ŀ
	void setMultTemp(int val);       //���ö�ģ�������,�Ա���ѭ��ʱʹ��
	void setSimilar(int val);        //�������ƶ�

	void setCharOffset(int val);     //����ӡ��ƫ��
	void setSearchAngle(int IsValid, int searchAngle); //������Ѱ�Ƕ�,�Ƿ�����
	void setROI(Hobject region);

	void setPattern(Hobject img);
	Hobject autoThreshold(Hobject img);
	const vector<Result>& detect(Hobject img);

private:
	int m_LightDark;
	int m_autoSegment;
	int m_threShold;
	int m_charCounts;
	int m_multTemp;
	int m_similar;
	int m_charOffSet;
	int m_searchAngleValid;
	int m_searchAngle;


	vector<Result> m_result;



};

#endif //MARKDETECT_H
