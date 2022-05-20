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
    void setLightDark(int val);      //light=0,dark=1设置白色/黑色物体,白色:threshold(m_threshold,255),反之,threshold(0,m_threshold)
	void setAutoSegment(int val);    //是否自动分割,0/1 关/启
	void setThreshold(int val);      //自动阈值分割的阈值
	void setCharCounts(int val);     //字符数目
	void setMultTemp(int val);       //设置多模板的数量,以便在循环时使用
	void setSimilar(int val);        //设置相似度

	void setCharOffset(int val);     //设置印字偏移
	void setSearchAngle(int IsValid, int searchAngle); //设置搜寻角度,是否启用
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
