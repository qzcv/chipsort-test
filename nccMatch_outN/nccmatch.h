#ifndef MATCH_H
#define MATCH_H

#include <vector>
#include <HalconCpp.h>
//#include <QObject>
#include <QPoint>
#include <QRect>
using namespace Halcon;
using std::vector;

class NCCMatcher{
public:
	typedef struct{
		double row;
		double col;
		double angle;
		double score;
        HTuple homMat2d;
		//QPointF box[4];
	}Result;

	typedef struct{
		bool isCircle;
		float cx;
		float cy;
		float R;
		float w;
		float h;
		float angle;//rad
	}Region;
    NCCMatcher();
    void setPattern(Hobject& img ,int& patternX,int& patternY,const Region* rejectRegion=NULL);
    void setRoi(const QRect* roi);
    void setSimilar(float val);
	void setMaxOverlap(float val);
	void setRotateDeg(float range, float step=1);
	void setNum(int val);
    const vector<Result>& findPattern(const Hobject* img);
    float getSimilar(){return m_threshold;}
	void getMinusRowCol(double &row,double &col);
    const Hobject* getPatternImage();
	~NCCMatcher();
	
protected:
	//void getbox(double row, double col, double angle, QPointF points[4]);
    //IplImage* m_pattern;
	Hobject m_pattern;
	QRect* m_roi;
    float m_degStart, m_degRange, m_degStep;
    float m_threshold;
	float m_maxOverlap;
	double m_row, m_col;
	Hlong m_modleId;
	vector<Result> m_result;
	int m_num;
	int m_patternWidth,m_patternHeight;
	int m_patternX,m_patternY;  //模板在原图中的左上角坐标
};




#endif // DETECT_H
