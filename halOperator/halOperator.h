#ifndef REGION_OPERATOR
#define REGION_OPERATOR

#include "RlRegion.h"
#include <vector>
#include <set>
#include <cv.h>
#include <string>

#ifdef WIN32
#ifdef OPERATOR_EXPORTS
# define OPERATOR_EXPORTS __declspec(dllexport)
#else
# define OPERATOR_EXPORTS __declspec(dllimport)
#endif
#else
# define OPERATOR_EXPORTS
#endif

using std::vector;
using std::set;

namespace qzcv
{
	typedef enum {
		area = 0, row, col, width, height, row1, col1, row2, col2, holesNum, holesArea,
	}Features;
	typedef enum { and = 0, or }Operation;
	OPERATOR_EXPORTS void selectShape(const RlRegion &srcReg, RlRegion &dstReg, Features features, double min, double max);
	OPERATOR_EXPORTS void selectShape(const RlRegion &srcReg, RlRegion &dstReg, int num, Features *features, Operation operation, double *min, double *max);

	OPERATOR_EXPORTS int getConnsNum(const RlRegion& reg);
	OPERATOR_EXPORTS int getHolesNum(const RlRegion& reg);
	OPERATOR_EXPORTS int getHolesArea(const RlRegion& reg);

	OPERATOR_EXPORTS void fillUpShape(const RlRegion &reg, RlRegion &fillUpReg, double min, double max);	//only area

	typedef enum { character = 0 }SortMode;
	OPERATOR_EXPORTS void sortRegion(const RlRegions &regs, RlRegions &sortRegs, SortMode mode, bool isCol = true);

	OPERATOR_EXPORTS void rotateImg_90N(const cv::Mat &img, cv::Mat &dst, int angle);	//仅限用于旋转0,90，180，270的图片

	OPERATOR_EXPORTS void angleLl(
		double RowA0, double ColA0, double RowA1, double ColA1,
		double RowB0, double ColB0, double RowB1, double ColB1, double &angle);
	OPERATOR_EXPORTS void distancePp(double Row1, double Col1, double Row2, double Col2, double &dist);
	OPERATOR_EXPORTS void distancePl(double Row, double Col, double Row1, double Col1, double Row2, double Col2, double &dist);
	OPERATOR_EXPORTS void intersectionLL(
		double RowA0, double ColA0, double RowA1, double ColA1,
		double RowB0, double ColB0, double RowB1, double ColB1, double &Row, double &Col);

	OPERATOR_EXPORTS void paintRegion(const RlRegion &reg, const cv::Mat img, cv::Mat &dst, int grayVal);//only fill

	OPERATOR_EXPORTS void selectRegionPoint(const RlRegion &srcReg, RlRegion &dstReg, int Row, int Col);

	template <typename T>
	void genRegionPoints(RlRegion &reg, const vector<T> &Row, const vector<T> &Col)
	{
		if (!Row.size())
			return;
		string types[3] = { "int","double","float" };
		bool isReturn = true;
		for (auto i = 0;i < 3;++i)
			if (!types[i].compare(typeid(Row.at(0)).name()))
			{
				isReturn = false;
				break;
			}
		if (isReturn)
			return;

		reg = RlRegion();
		RlRegion tmp = RlRegion();
		int sz = Row.size() > Col.size() ? Col.size() : Row.size();
		for (auto i = 0;i < sz;++i)
		{
			tmp = RlRegion();
			tmp.genRect(Col.at(i), Row.at(i), Col.at(i), Row.at(i));
			reg.unionReg(tmp, reg);
		}
	}

	template <typename T>
	void sortArray(const vector<T>& arr, vector<T>& sorted, vector<int>& sortIndex = vector<int>)	//return sorted index
	{
		sorted.clear();
		sorted = arr;
		stable_sort(sorted.begin(), sorted.end());

		if (arr.size() == 0)
			sortIndex.clear();
		else if (arr.size() == 1)
			sortIndex.push_back(0);
		else
		{
			set<int> st;//用于存储已经使用过的索引，避免重复使用
			for (auto i = 0;i < sorted.size();++i)
				for (auto j = 0;j < arr.size();++j)
					if (st.find(j) == st.end())
						if (fabs(arr[j] - sorted[i]) < 0.00001)
						{
							st.insert(j);
							sortIndex.push_back(j);
							break;
						}
		}
	}
}

#endif