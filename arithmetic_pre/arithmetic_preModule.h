#pragma once
#include <moduleClass.h>
#include <imageModule.h>

using namespace qzcv;
class arithmetic_preParam;

class arithmetic_preModule : public UnitModule
{
public:
	explicit arithmetic_preModule();
	explicit arithmetic_preModule(const QString&);
	~arithmetic_preModule();
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
private:
	arithmetic_preParam* m_param;
	friend class arithmetic_preWidget;
public:
	typedef enum { ADD = 0, SUB, MULT, DIV, MAX, MIN, AND, OR, NOT, NAND, NOR, XOR, XNOR }Type;
	//加、减、乘、除、最大、最小、与、或、非、与非、或非、异或、异或非
private:
	UnitInputPin<cv::Mat> p_inIm1;
	UnitInputPin<cv::Mat> p_inIm2;

	UnitOutputPin<cv::Mat> p_outIm;
};

