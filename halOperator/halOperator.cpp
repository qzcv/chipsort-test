#include "halOperator.h"
#include <algorithm>
#include <set>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include "transformation.h"

using namespace cv;

namespace qzcv
{
	void selectShape(const RlRegion & srcReg, RlRegion & dstReg, Features features, double min, double max)
	{
		selectShape(srcReg, dstReg, 1, &features, Operation::and, &min, &max);
	}
	void selectShape(const RlRegion & srcReg, RlRegion & dstReg, int num, Features * features, Operation operation, double * min, double * max)
	{
		RlRegion region = srcReg;
		RlRegions regions;
		region.connect(regions);

		dstReg = RlRegion();//clear

		int Area, x1, y1, x2, y2;
		float cx, cy;
		for (auto i = 0;i < regions.size();++i)
		{
			regions[i].areaCenter(Area, cx, cy);
			regions[i].smallestRect(x1, y1, x2, y2);

			int hNum = getHolesNum(regions[i]);
			int hArea = getHolesArea(regions[i]);

			bool isSelect = (operation == Operation::and) ? true : false;//and和or的初始化值不能一致
			for (auto j = 0;j < num;++j)
			{
				double compareVal;

				if (features[j] == Features::area || features[j] == Features::row || features[j] == Features::col)
				{
					if (features[j] == Features::area)
						compareVal = Area;
					else if (features[j] == Features::row)
						compareVal = cy;
					else if (features[j] == Features::col)
						compareVal = cx;
				}
				else if (	//目前用于判断两个角点取自于Region的外接矩形
					features[j] == Features::row1 || features[j] == Features::col1 ||
					features[j] == Features::row2 || features[j] == Features::col2 ||
					features[j] == Features::width || features[j] == Features::height)
				{
					if (features[j] == Features::row1)
						compareVal = y1;
					else if (features[j] == Features::col1)
						compareVal = x1;
					else if (features[j] == Features::row2)
						compareVal = y2;
					else if (features[j] == Features::col2)
						compareVal = x2;
					else if (features[j] == Features::width)
						compareVal = x2 - x1 + 1;
					else if (features[j] == Features::height)
						compareVal = y2 - y1 + 1;
				}
				else if (features[j] == Features::holesNum)
					compareVal = hNum;
				else if (features[j] == Features::holesArea)
					compareVal = hArea;

				bool r0 = compareVal - min[j] > -0.001;
				bool r1 = max[j] - compareVal > -0.001;
				bool isTrue = r0 && r1;
				if (operation == Operation::and)
					isSelect = isSelect && isTrue;
				else if (operation == Operation:: or )
					isSelect = isSelect || isTrue;

				if (operation == Operation::and&&!isSelect)	//and条件下，一旦选择条件变为了false，立即退出后面的条件筛选
					break;
			}
			if (isSelect)
				dstReg.unionReg(dstReg, regions[i]);
		}
	}
	int getConnsNum(const RlRegion & reg)
	{
		RlRegion region = reg;
		RlRegions regions;
		region.connect(regions);
		return regions.size();
	}
	int getHolesNum(const RlRegion & reg)
	{
		RlRegion region = reg;
		RlRegion fillUpReg, diffReg;
		fillUpReg.fillUp(region);
		diffReg.difference(fillUpReg, region);

		RlRegions regions;
		diffReg.connect(regions);
		return regions.size();
	}
	int getHolesArea(const RlRegion & reg)
	{
		RlRegion region = reg;
		RlRegion fillUpReg, diffReg;
		fillUpReg.fillUp(region);
		diffReg.difference(fillUpReg, region);

		int area;
		float cx, cy;
		diffReg.areaCenter(area, cx, cy);
		return area;
	}
	void fillUpShape(const RlRegion & reg, RlRegion & fillUpReg, double min, double max)
	{
		fillUpReg = RlRegion();

		RlRegions regs;
		reg.connect(regs);

		int area;
		float cx, cy;
		for (auto i = 0;i < regs.size();++i)
		{
			if (getHolesNum(regs[i]))
			{
				RlRegion fillUpR, diff;
				fillUpR.fillUp(regs[i]);
				diff.difference(fillUpR, regs[i]);
				
				diff.areaCenter(area, cx, cy);
				if (area >= min&&area <= max)
					regs[i].fillUp(regs[i]);
			}
			fillUpReg.unionReg(fillUpReg, regs[i]);
		}
	}
	void sortRegion(const RlRegions & regs, RlRegions & sortRegs, SortMode mode, bool isCol)
	{
		int64 t1 = cvGetTickCount();
		int size = regs.size();

		vector<int> min, max;
		vector<vector<RlRegion>> segReg;
		for (auto i = 0;i < size;++i)
		{
			int x0, y0, x1, y1;
			regs[i].smallestRect(x0, y0, x1, y1);
			int valmin = isCol ? x0 : y0;
			int valmax = isCol ? x1 : y1;
			if (min.size() == 0)
			{
				segReg.push_back({ regs[i] });
				min.push_back(valmin);
				max.push_back(valmax);
				continue;
			}
			else
			{
				bool isNewGrp = false;
				for (auto j = 0;j < min.size();++j)
				{
					if (valmax < min[j] || valmin > max[j])
					{
						isNewGrp = true;
						continue;
					}
					else
					{
						isNewGrp = false;
						segReg[j].push_back(regs[i]);
						min[j] = (valmin < min[j]) ? valmin : min[j];
						max[j] = (valmax > max[j]) ? valmax : max[j];
						break;
					}
				}
				if (isNewGrp)
				{
					segReg.push_back({ regs[i] });
					min.push_back(valmin);
					max.push_back(valmax);
				}
			}			
		}
		for (auto i = 0;i < segReg.size();++i)
			stable_sort(segReg[i].begin(), segReg[i].end(), [isCol](const RlRegion &reg1, const RlRegion &reg2) {
			int x10, y10, x11, y11, x20, y20, x21, y21;
			reg1.smallestRect(x10, y10, x11, y11);
			reg2.smallestRect(x20, y20, x21, y21);
			if (isCol)
				return (y10 + y11) / 2.0 < (y20 + y21) / 2.0;
			else
				return (x10 + x11) / 2.0 < (x20 + x21) / 2.0;
		});
		stable_sort(segReg.begin(), segReg.end(), [isCol](const vector<RlRegion> &v1, const vector<RlRegion> &v2) {
			int x10, y10, x11, y11, x20, y20, x21, y21;
			v1[0].smallestRect(x10, y10, x11, y11);
			v2[0].smallestRect(x20, y20, x21, y21);
			if (isCol)
				return (x10 + x11) / 2.0 < (x20 + x21) / 2.0;
			else
				return (y10 + y11) / 2.0 < (y20 + y21) / 2.0;
		});
		sortRegs = RlRegions(size);
		int n = 0;
		for (auto i = 0;i < segReg.size();++i)
			for (auto j = 0;j < segReg[i].size();++j)
			{
				sortRegs[n] = segReg[i][j];
				n++;
			}
		int64 t2 = cvGetTickCount();
		printf("t2-t1=%.3fms\n", (t2 - t1) / cvGetTickFrequency() / 1000.0);
	}
	void rotateImg_90N(const cv::Mat & img, cv::Mat & dst, int angle)
	{
		if (angle == 0 || angle == 90 || angle == 180 || angle == 270)
		{
			if (angle == 0)
				dst = img.clone();
			else
			{
				Point2d p1 = Point2d((img.cols - 1) / 2.0, (img.rows - 1) / 2.0);
				Point2d p2;
				if (angle == 180)
					p2 = Point2d((img.cols - 1) / 2.0, (img.rows - 1) / 2.0);
				else
					p2 = Point2d((img.rows - 1) / 2.0, (img.cols - 1) / 2.0);

				Mat homMat2d = vector_angle_to_rigid(p1, 0, p2, angle*CV_PI / 180);

				Size dSize;
				if (angle == 180)
					dSize = Size(img.size());
				else
					dSize = Size(img.rows, img.cols);

				warpAffine(img, dst, homMat2d, dSize);
			}
		}
	}
	void angleLl(double RowA0, double ColA0, double RowA1, double ColA1, double RowB0, double ColB0, double RowB1, double ColB1, double & angle)
	{
		double ang1 = atan2(RowA1 - RowA0, ColA1 - ColA0);
		double ang2 = atan2(RowB1 - RowB0, ColB1 - ColB0);
		angle = ang1 - ang2;
	}
	void distancePp(double Row1, double Col1, double Row2, double Col2, double & dist)
	{
		dist = sqrt(pow(Row1 - Row2, 2) + pow(Col1 - Col2, 2));
	}
	void distancePl(double Row, double Col, double Row1, double Col1, double Row2, double Col2, double &dist)
	{
		double A = Row1 - Row2;
		double B = Col2 - Col1;
		double C = Col1*Row2 - Col2*Row1;
		dist = fabs(A*Col + B*Row + C) / sqrt(pow(A, 2) + pow(B, 2));
	}
	void intersectionLL(double RowA0, double ColA0, double RowA1, double ColA1, double RowB0, double ColB0, double RowB1, double ColB1, double & Row, double & Col)
	{
		double lenA = sqrt(pow(RowA1 - RowA0, 2) + pow(ColA1 - ColA0, 2));
		double lenB = sqrt(pow(RowB1 - RowB0, 2) + pow(ColB1 - ColB0, 2));

		double deg0 = atan2(RowA1 - RowA0, ColA1 - ColA0);
		double deg1 = atan2(RowB1 - RowB0, ColB1 - ColB0);

		double a = RowA0 * cos(deg0) - ColA0 * sin(deg0);
		double b = RowB0 * cos(deg1) - ColB0 * sin(deg1);

		Col = (b * cos(deg0) - a * cos(deg1)) / sin(deg0 - deg1);
		Row = (b * sin(deg0) - a * sin(deg1)) / sin(deg0 - deg1);
	}
	void paintRegion(const RlRegion & reg, const cv::Mat img, cv::Mat & dst, int grayVal)
	{
		auto sz = img.size();
		cv::Mat im = reg.toBinImage(0, 255, sz.height, sz.width);
		cv::Mat interIm; 
		bitwise_and(im, img, interIm);
		cv::Mat paintIm = reg.toBinImage(grayVal, 0, sz.height, sz.width);
		add(paintIm, interIm, dst);
	}
	void selectRegionPoint(const RlRegion & srcReg, RlRegion & dstReg, int Row, int Col)
	{
		RlRegion reg = srcReg;
		RlRegions regs;
		reg.connect(regs);
		RlRegion tmpReg;
		tmpReg.genRect(Col, Row, Col, Row);
		dstReg = RlRegion();
		RlRegion interReg;
		for (auto i = 0;i < regs.size();++i)
		{
			interReg.interset(tmpReg, regs[i]);
			if (interReg.getRldata())
				dstReg.unionReg(dstReg, interReg);
		}
	}
}