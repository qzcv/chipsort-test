#include "markDetect.h"


MarkDetect::MarkDetect(void)
{
	m_LightDark=0;
	m_autoSegment=0;
	m_threShold=70;
	m_charCounts=1;
	m_multTemp=1;
	m_similar=60;
	m_charOffSet=0;
	m_searchAngleValid=1;
	m_searchAngle=5; 

}

MarkDetect::~MarkDetect(void)
{

}

void MarkDetect::setLightDark(int val)
{
	m_LightDark=val;
}

void MarkDetect::setAutoSegment(int val)
{
	m_autoSegment=val;
}

void MarkDetect::setThreshold(int val)
{
	m_threShold=val;
}

void MarkDetect::setCharCounts(int val)
{
	m_charCounts=val;
}

void MarkDetect::setMultTemp(int val)
{
	m_multTemp=val;;
}

void MarkDetect::setSimilar(int val)
{
	m_similar=val;
}

void MarkDetect::setCharOffset(int val)
{
	m_charOffSet=val;
}

void MarkDetect::setSearchAngle(int IsValid, int searchAngle)
{
	m_searchAngleValid=IsValid;
	m_searchAngle=searchAngle;
}

Hobject MarkDetect::autoThreshold(Hobject img)
{
	Hobject region;
	if(m_LightDark)
	    threshold(img,&region,0,m_threShold); //dark
	else
		threshold(img,&region,m_threShold,255); //white object
	return region;
}

const vector<MarkDetect::Result>& MarkDetect::detect(Hobject img)
{
	m_result.clear();
	Hobject region1;
	bin_threshold(img, &region1);


	Result result;
	result.region=region1;
	m_result.push_back(result);
	return m_result;
	
}

