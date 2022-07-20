#pragma once

#include <opencv2/core/core.hpp>
#include <cv.h>

#ifdef WIN32
#ifdef QZCV_EXPORTS
# define QZCV_EXPORT __declspec(dllexport)
#else
# define QZCV_EXPORT __declspec(dllimport)
#endif
#else
# define QZCV_EXPORT
#endif

namespace qzcv{

class QZCV_EXPORT Hom3d5S 
{
public:
	Hom3d5S(double h11 = 1, double h12 = 0, double h13 = 0, double h14 = 0,
			double h21 = 0, double h22 = 1, double h23 = 0, double h24 = 0);
	const cv::Mat& homMat()const;
	void getParam(double& h11, double& h12, double& h13, double& h14,
		double& h21, double& h22, double& h23, double& h24);

	enum ZDirection {ColIncr, ColDecr, RowIncr, RowDecr};
	bool calibrate(const cv::Point2d* imgPos, const cv::Point2d* worldPos, int num, ZDirection direction);
	bool calibrate(const cv::Point2d* imgPos, const cv::Point3d* worldPos, int num, ZDirection direction);
	bool calibrateBottom(const cv::Point2d* imgPos, const cv::Point2d* worldPos, int num, cv::Point2d dirImgPos, cv::Point3d dirWorldPos);


	cv::Point2d calImgPos(const cv::Point3d& worldPos);
	static bool calWorldPos(const Hom3d5S& hom1, const cv::Point2d& imgPos1,
		const Hom3d5S& hom2, const cv::Point2d& imgPos2, cv::Point3d& worldPos);

private:
	cv::Mat* m_h;
};

class QZCV_EXPORT PlaneSurface {
public:
	PlaneSurface(double a = 0, double b = 0, double c = 1, double d = 0);//default z=0
	bool fit(const cv::Point3d* points, int num);
	double pointDistance(const cv::Point3d& point);
	double calZ(double x, double y);
	void getParam(double& a, double& b, double& c, double& d)const;

private:
	double m_a, m_b, m_c, m_d;

};

class QZCV_EXPORT RigidTrans3D 
{
public:
	RigidTrans3D(double angX = 0, double angY = 0, double angZ = 0, double tx = 0, double ty = 0, double tz = 0);
	bool fit(const cv::Point3d* from, const cv::Point3d* to, int num);
	cv::Point3d trans(const cv::Point3d& xyz);
	void getParam(double& angX, double& angY, double& angZ, double& tx, double& ty, double& tz);
		
private: 
	cv::Mat* m_rt;
};

}