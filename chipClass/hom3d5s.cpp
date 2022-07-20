#include "hom3d5s.h"
namespace qzcv{

Hom3d5S::Hom3d5S(double h11 /*= 1*/, double h12 /*= 0*/, double h13 /*= 0*/, double h14 /*= 0*/, double h21 /*= 0*/, double h22 /*= 1*/, double h23 /*= 0*/, double h24 /*= 0*/)
{
	m_h = new cv::Mat(2, 4, CV_64F);
	m_h->at<double>(0, 0) = h11;
	m_h->at<double>(0, 1) = h12;
	m_h->at<double>(0, 2) = h13;
	m_h->at<double>(0, 3) = h14;
	m_h->at<double>(1, 0) = h21;
	m_h->at<double>(1, 1) = h22;
	m_h->at<double>(1, 2) = h23;
	m_h->at<double>(1, 3) = h24;
}

static bool Hom3d5SCalibrate(const cv::Point2d* imgPos, const cv::Point2d* worldPos, int num, cv::Mat matH[2])
{
	assert(num >= 3);

	//calculate h11, h12, h14, h21, h22, h24 by LSM
	cv::Mat matCol(num, 1, CV_64F);
	cv::Mat matRow(num, 1, CV_64F);
	cv::Mat matW(num, 3, CV_64F);
	cv::Mat matH1(3, 1, CV_64F);
	cv::Mat matH2(3, 1, CV_64F);

	for (int i = 0; i < num; i++) {
		matCol.at<double>(i) = imgPos[i].x;
		matRow.at<double>(i) = imgPos[i].y;
		matW.at<double>(i, 0) = worldPos[i].x;
		matW.at<double>(i, 1) = worldPos[i].y;
		matW.at<double>(i, 2) = 1;
	}

	cv::solve(matW, matCol, matH1, CV_SVD);
	cv::solve(matW, matRow, matH2, CV_SVD);

	double h11 = matH1.at<double>(0);
	double h12 = matH1.at<double>(1);
	double h14 = matH1.at<double>(2);
	double h21 = matH2.at<double>(0);
	double h22 = matH2.at<double>(1);
	double h24 = matH2.at<double>(2);

	for (int i = 0; i < 2; i++) {
		matH[i] = cv::Mat(2, 4, CV_64F);
		matH[i].at<double>(0, 0) = h11;
		matH[i].at<double>(0, 1) = h12;
		matH[i].at<double>(0, 3) = h14;
		matH[i].at<double>(1, 0) = h21;
		matH[i].at<double>(1, 1) = h22;
		matH[i].at<double>(1, 3) = h24;
	}


	//calculate h13 and h23
	double p = h21*h21 + h22*h22 - h11*h11 - h12*h12;
	double q = -h11*h21 - h12*h22;

	double h13_2 = 0.5*(p + sqrt(p*p + 4 * q*q));

	double h13Abs = sqrt(h13_2);
	double h23Abs = sqrt(h13_2 - p);
	if (q > 0) {
		matH[0].at<double>(0, 2) = h13Abs;
		matH[0].at<double>(1, 2) = h23Abs;
		matH[1].at<double>(0, 2) = -h13Abs;
		matH[1].at<double>(1, 2) = -h23Abs;
	}
	else {
		matH[0].at<double>(0, 2) = h13Abs;
		matH[0].at<double>(1, 2) = -h23Abs;
		matH[1].at<double>(0, 2) = -h13Abs;
		matH[1].at<double>(1, 2) = h23Abs;
	}
	return true;
}

static int selectMatH(cv::Mat matH[2], Hom3d5S::ZDirection dirction) {

	int idx = 0;
	double h13[2], h23[2];
	for (int i = 0; i < 2; i++) {
		h13[i] = matH[i].at<double>(0, 2);
		h23[i] = matH[i].at<double>(1, 2);
	}
	switch (dirction)
	{
	case Hom3d5S::ColIncr:
		return h13[0] > h13[1] ? 0 : 1;
	case Hom3d5S::ColDecr:
		return h13[0] < h13[1] ? 0 : 1;
	case Hom3d5S::RowIncr:
		return h23[0] > h23[1] ? 0 : 1;
	case Hom3d5S::RowDecr:
		return h23[0] < h23[1] ? 0 : 1;
	default:
		return 0;
	}
}


bool Hom3d5S::calibrate(const cv::Point2d* imgPos, const cv::Point2d* worldPos, int num, ZDirection dirction)
{
	cv::Mat matH[2];
	bool ok=Hom3d5SCalibrate(imgPos, worldPos, num, matH);
	if (!ok)
		return false;
	int idx=selectMatH(matH, dirction);
	*m_h = matH[idx];
	return true;
}

bool Hom3d5S::calibrateBottom(const cv::Point2d* imgPos, const cv::Point2d* worldPos, int num,
	cv::Point2d dirImgPos, cv::Point3d dirWorldPos)
{
	assert(fabs(dirWorldPos.z) > 0.00001);
	cv::Mat matH[2];
	bool ok = Hom3d5SCalibrate(imgPos, worldPos, num, matH);
	if (!ok)
		return false;

	cv::Mat vw(4, 1, CV_64F);
	vw.at<double>(0) = dirWorldPos.x;
	vw.at<double>(1) = dirWorldPos.y;
	vw.at<double>(2) = dirWorldPos.z;
	vw.at<double>(3) = 1;

	cv::Mat vi[2];
	double err[2];
	for (int i = 0; i < 2; i++) {
		vi[i] = matH[i] * vw;
		double dx = vi[i].at<double>(0) - dirImgPos.x;
		double dy = vi[i].at<double>(1) - dirImgPos.y;
		err[i] = dx*dx + dy*dy;
	}

	if (err[0] < err[1])
		*m_h = matH[0];
	else
		*m_h = matH[1];

	return true;
}

bool Hom3d5S::calibrate(const cv::Point2d* imgPos, const cv::Point3d* worldPos, int num, ZDirection dirction)
{
	PlaneSurface ps;
	ps.fit(worldPos, num);
	for (int i = 0; i < num; i++) {
		double dist=ps.pointDistance(worldPos[i]);
		if (dist> 0.00001)//not all pos in a  plane PlaneSurface
			return false;
	}
	double a, b, c, d;
	ps.getParam(a, b, c, d);
	cv::Mat matRT(3, 4, CV_64F);
	double r11= a*c - a*b;
	double r12 = b*c + c*c + a*a;
	double r13 = -b*b - b*c - a*a;
	double len = sqrt(r11*r11 + r12*r12 + r13*r13);
	matRT.at<double>(0, 0) = r11 / len;
	matRT.at<double>(0, 1) = r12 / len;
	matRT.at<double>(0, 2) = r13 / len;
	matRT.at<double>(0, 3) = 0;

	double r21 = -b - c;
	double r22 = a;
	double r23 = a;
	double len2 = sqrt(r21*r21 + r22*r22 + r23*r23);
	matRT.at<double>(1, 0) = r21 / len2;
	matRT.at<double>(1, 1) = r22 / len2;
	matRT.at<double>(1, 2) = r23 / len2;
	matRT.at<double>(1, 3) = 0;

	matRT.at<double>(2, 0) = a;
	matRT.at<double>(2, 1) = b;
	matRT.at<double>(2, 2) = c;
	matRT.at<double>(2, 3) = d;

	cv::Point2d* pos2d = new cv::Point2d[num];
	cv::Mat xya1(4, 1, CV_64F);
	xya1.at<double>(3) = 1;
	for (int i = 0; i < num; i++) {
		xya1.at<double>(0) = worldPos[i].x;
		xya1.at<double>(1) = worldPos[i].y;
		xya1.at<double>(2) = worldPos[i].z;
		cv::Mat xya = matRT*xya1;
		pos2d[i].x = xya.at<double>(0);
		pos2d[i].y = xya.at<double>(1);
	}
	cv::Mat matH[2];
	bool ok=Hom3d5SCalibrate(imgPos, pos2d, num, matH);
	delete []pos2d;
	if (!ok)
		return false;

	cv::Mat matRT01(4, 4, CV_64F, cv::Scalar(0));
	matRT.copyTo(matRT01(cv::Rect(0, 0, 4, 3)));
	matRT01.at<double>(3, 3) = 1;

	matH[0] = matH[0] * matRT01;
	matH[1] = matH[1] * matRT01;

	int idx=selectMatH(matH, dirction);

	*m_h = matH[idx];

	return true;
}





cv::Point2d Hom3d5S::calImgPos(const cv::Point3d& worldPos)
{
	cv::Mat v(4, 1, CV_64F);
	v.at<double>(0) = worldPos.x;
	v.at<double>(1) = worldPos.y;
	v.at<double>(2) = worldPos.z;
	v.at<double>(3) = 1;
	cv::Mat vi=*m_h*v;
	return cv::Point2d(vi.at<double>(0), vi.at<double>(1));
}

const cv::Mat& Hom3d5S::homMat() const
{
	return *m_h;
}

void Hom3d5S::getParam(double& h11, double& h12, double& h13, double& h14, 
	double& h21, double& h22, double& h23, double& h24)
{
	h11 = m_h->at<double>(0, 0);
	h12 = m_h->at<double>(0, 1);
	h13 = m_h->at<double>(0, 2);
	h14 = m_h->at<double>(0, 3);
	h21 = m_h->at<double>(1, 0);
	h22 = m_h->at<double>(1, 1);
	h23 = m_h->at<double>(1, 2);
	h24 = m_h->at<double>(1, 3);
}

bool Hom3d5S::calWorldPos(const Hom3d5S& hom1, const cv::Point2d& imgPos1, 
	const Hom3d5S& hom2, const cv::Point2d& imgPos2, cv::Point3d& worldPos)
{
	cv::Mat matCr(4, 1, CV_64F);
	
	matCr.at<double>(0) = imgPos1.x-hom1.homMat().at<double>(0, 3);
	matCr.at<double>(1) = imgPos1.y-hom1.homMat().at<double>(1, 3);
	matCr.at<double>(2) = imgPos2.x-hom2.homMat().at<double>(0, 3);
	matCr.at<double>(3) = imgPos2.y-hom2.homMat().at<double>(1, 3);

	cv::Mat H12(4, 3, CV_64F);
	hom1.homMat()(cv::Rect(0, 0, 3, 2)).copyTo(H12(cv::Rect(0, 0, 3, 2)));
	hom2.homMat()(cv::Rect(0, 0, 3, 2)).copyTo(H12(cv::Rect(0, 2, 3, 2)));

	cv::Mat xyz(3, 1, CV_64F);


	cv::solve(H12, matCr, xyz, CV_SVD);

	worldPos.x = xyz.at<double>(0);
	worldPos.y = xyz.at<double>(1);
	worldPos.z = xyz.at<double>(2);
	return true;
}



PlaneSurface::PlaneSurface(double a /*= 0*/, double b /*= 0*/, double c /*= 1*/, double d /*= 0*/)
{
	double aabbcc =sqrt(a*a + b*b + c*c);
	assert(aabbcc > 0.00001);
	double k = 1 / aabbcc;
	m_a = k*a;
	m_b = k*b;
	m_c = k*c;
	m_d = k*d;
}

bool PlaneSurface::fit(const cv::Point3d* points, int num)
{
	assert(num > 3);
	cv::Point3d avg=(0, 0, 0);
	for (int i = 0; i < num; i++) {
		avg += points[i];
	}
	avg.x/= num;
	avg.y/= num;
	avg.z/= num;

	cv::Mat matX(3, 1, CV_64F);
	cv::Mat matS(3, 3, CV_64F, cv::Scalar(0));

	for (int i = 0; i < num; i++) {
		matX.at<double>(0) = points[i].x - avg.x;
		matX.at<double>(1) = points[i].y - avg.y;
		matX.at<double>(2) = points[i].z - avg.z;
		matS += matX*matX.t();
	}

	cv::Mat eigenVals(3, 1, CV_64F);
	cv::Mat eigenVectors(3, 3, CV_64F);

	bool res=cv::eigen(matS, eigenVals, eigenVectors);
	
	if (res) {
		m_a = eigenVectors.at<double>(2, 0);
		m_b = eigenVectors.at<double>(2, 1);
		m_c = eigenVectors.at<double>(2, 2);
		m_d = -(m_a*avg.x + m_b*avg.y + m_c*avg.z);
		if (m_d < 0) {
			m_a *= -1;
			m_b *= -1;
			m_c *= -1;
			m_d *= -1;
		}
	}
	return res;
}

double PlaneSurface::pointDistance(const cv::Point3d& point)
{
	return fabs(m_a*point.x + m_b*point.y + m_c*point.z + m_d);
}

double PlaneSurface::calZ(double x, double y)
{
	return -(m_a*x + m_b*y + m_d)/m_c;
}

void PlaneSurface::getParam(double& a, double& b, double& c, double& d)const
{
	a = m_a;
	b = m_b;
	c = m_c;
	d = m_d;
}

cv::Mat anglestoMat(double angX /*= 0*/, double angY /*= 0*/, double angZ) {
	cv::Mat matX(3, 3, CV_64F, cv::Scalar(0));
	matX.at<double>(0, 0) = 1;
	matX.at<double>(1, 1) = matX.at<double>(2, 2) = cos(angX);
	matX.at<double>(1, 2) = -sin(angX);
	matX.at<double>(2, 1) = sin(angX);

	cv::Mat matY(3, 3, CV_64F, cv::Scalar(0));
	matY.at<double>(1, 1) = 1;
	matY.at<double>(0, 0) = matY.at<double>(2, 2) = cos(angY);
	matY.at<double>(0, 2) = sin(angY);
	matY.at<double>(2, 0) = -sin(angY);

	cv::Mat matZ(3, 3, CV_64F, cv::Scalar(0));
	matZ.at<double>(2, 2) = 1;
	matZ.at<double>(0, 0) = matZ.at<double>(1, 1) = cos(angZ);
	matZ.at<double>(0, 1) = -sin(angZ);
	matZ.at<double>(1, 0) = sin(angZ);

	cv::Mat matR=matZ*matY*matX;
	return matR;
}

RigidTrans3D::RigidTrans3D(double angX /*= 0*/, double angY /*= 0*/, double angZ /*= 0*/, double tx /*= 0*/, double ty /*= 0*/, double tz /*= 0*/)
{
	m_rt = new cv::Mat(4, 4, CV_64F, cv::Scalar(0));
	m_rt->at<double>(0, 3) = tx;
	m_rt->at<double>(1, 3) = ty;
	m_rt->at<double>(2, 3) = tz;
	m_rt->at<double>(3, 3) = 1;


	cv::Mat matR = anglestoMat(angX, angY, angZ);
	matR.copyTo((*m_rt)(cv::Rect(0, 0, 3, 3)));
}

bool RigidTrans3D::fit(const cv::Point3d* from, const cv::Point3d* to, int num)
{
	cv::Point3d fromCenter(0, 0, 0), toCenter(0, 0, 0);
	for (int i = 0; i < num; i++) {
		fromCenter += from[i];
		toCenter += to[i];
	}
	fromCenter.x /= num;
	fromCenter.y /= num;
	fromCenter.z /= num;

	toCenter.x /= num;
	toCenter.y /= num;
	toCenter.z /= num;

	
	cv::Mat vFrom(3, 1, CV_64F);
	cv::Mat vTo(1, 3, CV_64F);
	cv::Mat matH(3, 3, CV_64F, cv::Scalar(0));
	for (int i = 0; i < num; i++) {
		vFrom.at<double>(0) = from[i].x - fromCenter.x;
		vFrom.at<double>(1) = from[i].y - fromCenter.y;
		vFrom.at<double>(2) = from[i].z - fromCenter.z;
		vTo.at<double>(0) = to[i].x - toCenter.x;
		vTo.at<double>(1) = to[i].y - toCenter.y;
		vTo.at<double>(2) = to[i].z - toCenter.z;
		matH += vFrom*vTo;
	}

	cv::Mat matU, matW, matVt;
	cv::SVDecomp(matH, matW, matU, matVt);


	cv::Mat matRTmp = matVt.t()*matU.t();
	double det = cv::determinant(matRTmp);
	cv::Mat matSel = cv::Mat::eye(3, 3, CV_64F);
	matSel.at<double>(2, 2) = det;
	cv::Mat matR = (matVt.t())*matSel*(matU.t());
	matR.copyTo((*m_rt)(cv::Rect(0, 0, 3, 3)));

	cv::Mat vFromCenter(3, 1, CV_64F);
	vFromCenter.at<double>(0) = fromCenter.x;
	vFromCenter.at<double>(1) = fromCenter.y;
	vFromCenter.at<double>(2) = fromCenter.z;
	
	cv::Mat vRf = matR*vFromCenter;

	m_rt->at<double>(0, 3) = toCenter.x - vRf.at<double>(0);
	m_rt->at<double>(1, 3) = toCenter.y - vRf.at<double>(1);
	m_rt->at<double>(2, 3) = toCenter.z - vRf.at<double>(2);
	return true;
}


cv::Point3d RigidTrans3D::trans(const cv::Point3d& xyz)
{
	cv::Mat vx(4, 1, CV_64F);
	vx.at<double>(0) = xyz.x;
	vx.at<double>(1) = xyz.y;
	vx.at<double>(2) = xyz.z;
	vx.at<double>(3) = 1;

	cv::Mat vy = *m_rt*vx;
	return cv::Point3d(vy.at<double>(0), vy.at<double>(1), vy.at<double>(2));
}


static double inPi(double ang) {
	while (ang < -CV_PI)
		ang += 2 * CV_PI;
	while (ang > CV_PI)
		ang -= 2 * CV_PI;
	return ang;
}

void RigidTrans3D::getParam(double& angA, double& angB, double& angC, double& tx, double& ty, double& tz)
{
	tx = m_rt->at<double>(0, 3);
	ty = m_rt->at<double>(1, 3);
	tz = m_rt->at<double>(2, 3);

	double r32 = m_rt->at<double>(2, 1), r33 = m_rt->at<double>(2, 2);
	angA = inPi(atan2(r32, r33));
	angB = inPi(atan2(-m_rt->at<double>(2, 0), sqrt(r32*r32 + r33*r33)));
	angC = inPi(atan2(m_rt->at<double>(1, 0), m_rt->at<double>(0, 0)));

	double angA2 = inPi(angA + CV_PI);
	double angB2 = inPi(CV_PI - angB);
	double angC2 = inPi(angC + CV_PI);

	if (fabs(angA) + fabs(angB) + fabs(angC) > fabs(angA2) + fabs(angB2) + fabs(angC2)){
		angA = angA2;
		angB = angB2;
		angC = angC2;
	}
}
}