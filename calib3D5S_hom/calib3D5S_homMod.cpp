#include "calib3D5S_homMod.h"
#include "calib3D5S_homParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "hom3d5s.h"

#define BtLineOk "btLineOk"      //底部线求取ok
#define SideLineOk "sideLineOk" //侧面线求取ok

#define BtTransLineOk "btTransLineOk" 
#define SideTransLineOk "sideTransLineOk"

#define BtLineX "btLineX"        //底部直线坐标
#define BtLineY "btLineY"
#define BtPointsX "btPointsX"    //交点坐标
#define BtPointsY "btPointsY"
#define BtPointsOK "btPointsOk"
#define BtCalibOK "btCalibOk"
#define BtAllOK "btAllOk" //底部总体ok

#define BtTransLineX "btTransLineX"        //底部直线坐标
#define BtTransLineY "btTransLineY"
#define BtTransPointsX "btTransPointsX"    //交点坐标
#define BtTransPointsY "btTransPointsY"
#define BtTransPointsOK "btTransPointsOk"
#define BtTransCalibOK "btTransCalibOk"
#define BtTransAllOK "btAllOk" //底部总体ok

#define SideTransLineX "SideTransLineX"        //底部直线坐标
#define SideTransLineY "SideTransLineY"
#define SideTransPointsX "SideTransPointsX"    //交点坐标
#define SideTransPointsY "SideTransPointsY"
#define SideTransPointsOK "SideTransPointsOk"
#define SideTransCalibOK "SideTransCalibOk"
#define SideTransAllOK "SideAllOk" //底部总体ok

#define CalABCDOK "calABCDOK"      //通过传入hom计算abcd实际坐标及刚体变换ok

#define Side0LineX "side0LineX"
#define Side0LineY "side0LineY"
#define Side1LineX "side1LineX"
#define Side1LineY "side1LineY"

#define SidePointsX "sidePointsX"
#define SidePointsY "sidePointsY"
#define SidePointsOK "sidePointsOk"
#define SideCalibOK "sideCalibOk"
#define SideAllOK "sideAllOk"

#define ALLOK "allok"

#define Side1Hxy "side1Hxy"
#define Side2Hxy "side2Hxy"
#define BottomHxy "bottomHxy"

#define OutTuple "outTuple" //辅助出标定报告的信息
#define VerSionName "V3.0.0_220420"

calib3D5S_homMod::calib3D5S_homMod()
{
	iniData();
}

calib3D5S_homMod::calib3D5S_homMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

calib3D5S_homMod::~calib3D5S_homMod()
{
	for (int i = 0; i < BottomNum; i++)
		delete m_BottomFitLine[i];
	for (int i = 0; i < SideNum; i++)
		for (int j = 0; j < 2; j++)
			delete m_sideFitLine[j][i];

	for (int i = 0; i < BtTransNum; i++)
		delete m_transBtFitLine[i];
	for (int i = 0; i < SideTransNum; i++)
		delete m_transSideFitLine[i];

	for (int i = 0; i < VerifyNum; i++)
		delete m_veryFitLine[i];

	for (int i = 0; i < 3; i++)
		delete m_hom3D5S[i];
	for (int i = 0; i < 2; i++)
		if (m_homTrans[i])
			delete m_homTrans[i];
	if (m_rigidTrans)
		delete m_rigidTrans;
	if (m_plane)
		delete m_plane;

	delete m_param;
}

int calib3D5S_homMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int calib3D5S_homMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void calib3D5S_homMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
}

void calib3D5S_homMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);

	if (level == MachineLevel)
	{
		if (m_param->IsTransHom)
		{
			HTuple fullOutHom;
			for (int i = 0; i < 8; i++)
				fullOutHom.Append(m_param->BtHxy[i]);

			for (int s = 0; s < 2; s++)
			{
				for (int i = 0; i < 8; i++)
				{
					if (m_param->horOrVerWANG == 1)
						fullOutHom.Append(m_param->SideHxy[s][i]);
					else
						fullOutHom.Append(m_param->TransSideHxy[s][i]);
				}
			}
			for (int s = 0; s < 2; s++)
			{
				for (int i = 0; i < 8; i++)
				{
					if (m_param->horOrVerWANG == 1)
						fullOutHom.Append(m_param->TransSideHxy[s][i]);
					else
						fullOutHom.Append(m_param->SideHxy[s][i]);
				}
			}
			toQList(fullOutHom, **p_3d5s);
			//setDetectOutData(FullOutHom, fullOutHom);
		}
		else {
			HTuple firstOut3Hom;
			for (int i = 0; i < 8; i++)
			{
				firstOut3Hom.Append(m_param->BtHxy[i]);
			}
			for (int s = 0; s < 2; s++)
			{
				for (int i = 0; i < 8; i++)
					firstOut3Hom.Append(m_param->SideHxy[s][i]);
			}
			toQList(firstOut3Hom, **p_first3dOut);
			//setDetectOutData(FirstOut3Hom, firstOut3Hom);
		}
	}
}

int calib3D5S_homMod::run(const QString &funName)
{
	if (p_im->empty())
		return -1;

	if(m_param->IsTransHom)
		return detectTrans();
	else
		return detectNormal();
}

void calib3D5S_homMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (p_im->empty())
		return;

	if (m_param->IsTransHom)
		viewResultTrans(iv);
	else
		viewResultNormal(iv);
}

void calib3D5S_homMod::textResult(ResultText *text, const QString &funName)
{
	if (p_im->empty())
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image!"));
		return;
	}

	if (m_param->IsTransHom)
		textResultTrans(text);
	else
		textResultNormal(text);
}

void calib3D5S_homMod::iniData()
{
	m_param = new calib3D5S_homParam;
	m_image = new Hobject;

	for (int i = 0; i < BottomNum; i++) {
		m_BottomFitLine[i] = NULL;
		m_BottomFitLine[i] = new fitEdgeLine();
	}
	for (int i = 0; i < SideNum; i++) {
		for (int j = 0; j < 2; j++)
		{
			m_sideFitLine[j][i] = NULL;
			m_sideFitLine[j][i] = new fitEdgeLine();
		}
	}

	for (int i = 0; i < 4; i++)
	{
		m_sideWorldTrans[i].x = 0;
		m_sideWorldTrans[i].y = 0;
		m_sideWorldTrans[i].z = 0;
	}

	for (int i = 0; i < BtTransNum; i++) {
		m_transBtFitLine[i] = NULL;
		m_transBtFitLine[i] = new fitEdgeLine();
	}
	for (int i = 0; i < SideTransNum; i++) {
		m_transSideFitLine[i] = NULL;
		m_transSideFitLine[i] = new fitEdgeLine();
	}
	for (int i = 0; i < VerifyNum; i++) {
		m_veryFitLine[i] = NULL;
		m_veryFitLine[i] = new fitEdgeLine();
	}

	m_hom3D5S[0] = m_hom3D5S[1] = m_hom3D5S[2] = nullptr;
	m_hom3D5S[0] = new Hom3d5S(
		m_param->BtHxy[0], m_param->BtHxy[1], m_param->BtHxy[2], m_param->BtHxy[3], 
		m_param->BtHxy[4], m_param->BtHxy[5], m_param->BtHxy[6], m_param->BtHxy[7]);

	for (int i = 0; i < 2; i++) {
		m_hom3D5S[i + 1] = new Hom3d5S(
			m_param->SideHxy[i][0], m_param->SideHxy[i][1], m_param->SideHxy[i][2], m_param->SideHxy[i][3], 
			m_param->SideHxy[i][4], m_param->SideHxy[i][5], m_param->SideHxy[i][6], m_param->SideHxy[i][7]);
		m_homTrans[i] = new Hom3d5S(
			m_param->TransSideHxy[i][0], m_param->TransSideHxy[i][1], m_param->TransSideHxy[i][2], m_param->TransSideHxy[i][3],
			m_param->TransSideHxy[i][4], m_param->TransSideHxy[i][5], m_param->TransSideHxy[i][6], m_param->TransSideHxy[i][7]);
	}
	m_rigidTrans = new RigidTrans3D();
	m_plane = new PlaneSurface();

	createPins();
}

void calib3D5S_homMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_first3dIn, "f3Di");

	addPin(&p_3d5s, "3d5s");
	addPin(&p_first3dOut, "f3Do");
}

void calib3D5S_homMod::createEvents()
{
	addEvent(ENT_CALIB);
}

void calib3D5S_homMod::iniDetector()
{
	QList<fitEdgeLine::Orient> btOrient;
	QList<fitEdgeLine::Orient> sideOrient[2];
	QList<fitEdgeLine::Orient> btTransOrient;
	QList<fitEdgeLine::Orient> sideTransOrient;
	QList<fitEdgeLine::Orient> VeryOrient;

	if (m_param->horOrVerWANG == 0) {
		btOrient << fitEdgeLine::ToRight << fitEdgeLine::ToDown << fitEdgeLine::ToLeft << fitEdgeLine::ToRight <<
			fitEdgeLine::ToUp << fitEdgeLine::ToLeft << fitEdgeLine::ToLeft << fitEdgeLine::ToUp;
		sideOrient[0] << fitEdgeLine::ToDown << fitEdgeLine::ToRight << fitEdgeLine::ToDown << fitEdgeLine::ToLeft <<
			fitEdgeLine::ToDown;
		sideOrient[1] << fitEdgeLine::ToUp << fitEdgeLine::ToRight << fitEdgeLine::ToUp << fitEdgeLine::ToLeft <<
			fitEdgeLine::ToUp;
		btTransOrient << fitEdgeLine::ToUp << fitEdgeLine::ToLeft << fitEdgeLine::ToLeft << fitEdgeLine::ToDown <<
			fitEdgeLine::ToUp << fitEdgeLine::ToRight << fitEdgeLine::ToRight << fitEdgeLine::ToDown;
		sideTransOrient << fitEdgeLine::ToDown << fitEdgeLine::ToRight << fitEdgeLine::ToUp <<
			fitEdgeLine::ToDown << fitEdgeLine::ToLeft << fitEdgeLine::ToUp;

		VeryOrient << fitEdgeLine::ToRight << fitEdgeLine::ToDown << fitEdgeLine::ToLeft << fitEdgeLine::ToUp <<
			fitEdgeLine::ToLeft << fitEdgeLine::ToDown << fitEdgeLine::ToRight << fitEdgeLine::ToUp;
	}
	else {
		btOrient << fitEdgeLine::ToDown << fitEdgeLine::ToRight << fitEdgeLine::ToUp << fitEdgeLine::ToDown <<
			fitEdgeLine::ToLeft << fitEdgeLine::ToUp << fitEdgeLine::ToLeft << fitEdgeLine::ToUp;
		sideOrient[0] << fitEdgeLine::ToRight << fitEdgeLine::ToDown << fitEdgeLine::ToRight << fitEdgeLine::ToUp <<
			fitEdgeLine::ToRight;
		sideOrient[1] << fitEdgeLine::ToLeft << fitEdgeLine::ToDown << fitEdgeLine::ToLeft << fitEdgeLine::ToUp <<
			fitEdgeLine::ToLeft;
		btTransOrient << fitEdgeLine::ToLeft << fitEdgeLine::ToUp << fitEdgeLine::ToUp << fitEdgeLine::ToRight <<
			fitEdgeLine::ToLeft << fitEdgeLine::ToDown << fitEdgeLine::ToDown << fitEdgeLine::ToRight;
		sideTransOrient << fitEdgeLine::ToRight << fitEdgeLine::ToDown << fitEdgeLine::ToLeft <<
			fitEdgeLine::ToRight << fitEdgeLine::ToUp << fitEdgeLine::ToLeft;

		VeryOrient << fitEdgeLine::ToRight << fitEdgeLine::ToDown << fitEdgeLine::ToLeft << fitEdgeLine::ToUp <<
			fitEdgeLine::ToUp << fitEdgeLine::ToRight << fitEdgeLine::ToDown << fitEdgeLine::ToLeft;
	}

	if (m_param->IsTransHom)
	{
		for (int i = 0; i < BtTransNum; i++)
		{
			if (m_param->IsBlackWang == 0)
			{
				m_transBtFitLine[i]->setPolarAndOrient(fitEdgeLine::ToBlack, btTransOrient[i]);
				m_transBtFitLine[i]->setThreshold(m_param->minGraydiff);
			}
			else {
				m_transBtFitLine[i]->setPolarAndOrient(fitEdgeLine::ToWhite, btTransOrient[i]);
				m_transBtFitLine[i]->setThreshold(m_param->minGraydiff + 20);
			}

			m_transBtFitLine[i]->setSearchParam(2, 1);
			m_transBtFitLine[i]->setFitFactor(2.0);
			m_transBtFitLine[i]->setRoi(1, &m_param->btTransRow1[i], &m_param->btTransCol1[i], &m_param->btTransRow2[i], &m_param->btTransCol2[i]);
		}
		for (int i = 0; i < SideTransNum; i++)
		{
			m_transSideFitLine[i]->setPolarAndOrient(fitEdgeLine::ToBlack, sideTransOrient[i]);
			m_transSideFitLine[i]->setThreshold(m_param->minGraydiff);
			m_transSideFitLine[i]->setSearchParam(2, 1);
			m_transSideFitLine[i]->setFitFactor(2.0);
			m_transSideFitLine[i]->setRoi(1, &m_param->sideTransRow1[i], &m_param->sideTransCol1[i], &m_param->sideTransRow2[i], &m_param->sideTransCol2[i]);
		}
	}
	else {
		for (int i = 0; i < BottomNum; i++)
		{
			if (i >= 6 && m_param->IsBlackWang == 0)
			{
				m_BottomFitLine[i]->setPolarAndOrient(fitEdgeLine::ToBlack, btOrient[i]);
				m_BottomFitLine[i]->setThreshold(m_param->minGraydiff);
			}
			else if (i >= 6 && m_param->IsBlackWang == 1) {
				m_BottomFitLine[i]->setThreshold(m_param->minGraydiff + 20);
				m_BottomFitLine[i]->setPolarAndOrient(fitEdgeLine::ToWhite, btOrient[i]);
			}

			//if (i>=6)
			//{
			//	if (m_param->IsBlackWang==0)
			//	{
			//		m_BottomFitLine[i]->setThreshold(m_param->minGraydiff);
			//	}
			//	else{
			//		m_BottomFitLine[i]->setThreshold(m_param->minGraydiff+20);
			//	}
			//	m_BottomFitLine[i]->setPolarAndOrient(fitEdgeLine::ToBlack,btOrient[i]);
			//	
			//}
			else {
				m_BottomFitLine[i]->setPolarAndOrient(fitEdgeLine::ToWhite, btOrient[i]);
				m_BottomFitLine[i]->setThreshold(m_param->minGraydiff);
			}

			m_BottomFitLine[i]->setSearchParam(2, 1);
			m_BottomFitLine[i]->setFitFactor(2.0);
			m_BottomFitLine[i]->setRoi(1, &m_param->bottomRow1[i], &m_param->bottomCol1[i], &m_param->bottomRow2[i], &m_param->bottomCol2[i]);
		}
	}
	for (int i = 0; i < SideNum; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			m_sideFitLine[j][i]->setPolarAndOrient(fitEdgeLine::ToBlack, sideOrient[j][i]);
			m_sideFitLine[j][i]->setThreshold(m_param->minGraydiff);
			m_sideFitLine[j][i]->setSearchParam(2, 1);
			m_sideFitLine[j][i]->setFitFactor(2.0);
			m_sideFitLine[j][i]->setRoi(1, &m_param->sideRow1[j][i], &m_param->sideCol1[j][i], &m_param->sideRow2[j][i], &m_param->sideCol2[j][i]);
		}
	}
	for (int i = 0; i < VerifyNum; i++)
	{
		if (m_param->IsBlackWang == 1 && (i == 3 || i == 5) && m_param->horOrVerWANG == 0)
		{
			m_veryFitLine[i]->setPolarAndOrient(fitEdgeLine::ToWhite, VeryOrient[i]);
		}
		else if (m_param->IsBlackWang == 1 && (i == 2 || i == 5) && m_param->horOrVerWANG == 1) {
			m_veryFitLine[i]->setPolarAndOrient(fitEdgeLine::ToWhite, VeryOrient[i]);
		}
		else {
			m_veryFitLine[i]->setPolarAndOrient(fitEdgeLine::ToBlack, VeryOrient[i]);
		}

		m_veryFitLine[i]->setThreshold(m_param->minGraydiff);
		m_veryFitLine[i]->setSearchParam(2, 1);
		m_veryFitLine[i]->setFitFactor(2.0);
		m_veryFitLine[i]->setRoi(1, &m_param->verRow1[i], &m_param->verCol1[i], &m_param->verRow2[i], &m_param->verCol2[i]);
	}
}

bool calib3D5S_homMod::detectNormal()
{
	bool allok = true;
	CvPoint2D32f *btLine[2];
	CvPoint2D32f *sideLine[2][2];

	bool btLineOK[BottomNum];
	bool sideLineOK[2][SideNum];
	bool btOK = true; //区域计算ok,lineok->interok->calibok
	bool sideOK[2] = { true,true };
	cv::Point2d btInterP[BtPointsNum];
	cv::Point2d sideInterP[2][PointsNum];
	for (int i = 0; i < 2; i++)
	{
		btLine[i] = new CvPoint2D32f[BottomNum];
		sideLine[0][i] = new CvPoint2D32f[SideNum];
		sideLine[1][i] = new CvPoint2D32f[SideNum];
	}

	for (int i = 0; i < BottomNum; i++)
	{
		btLineOK[i] = true;
		CvPoint2D32f line[2];
		int lineNum = 0;
		fitEdgeLine::ResType res = m_BottomFitLine[i]->calculateLine(m_image, line, lineNum);
		if (res != fitEdgeLine::ResOK) {
			btLineOK[i] = false;
			btOK = false;
			allok = false;
		}
		else {
			for (int j = 0; j < 2; j++)
			{
				btLine[j][i].x = line[j].x;
				btLine[j][i].y = line[j].y;
				setDetectOutData(BtLineX, btLine[j][i].x, j, i);
				setDetectOutData(BtLineY, btLine[j][i].y, j, i);
			}
		}
		setDetectOutData(BtLineOk, btLineOK[i], i);
	}
	if (btOK)
	{
		for (int i = 0; i < BtPointsNum; i++)
		{
			bool interok = true;
			HTuple pa;
			HTuple interRow, interCol;
			int Idx;
			if (i < (BtPointsNum - 1))
			{
				Idx = (i < 2 ? i : (i + 1));
			}
			else {
				Idx = BtPointsNum + 1;
			}

			double x1, y1, x2, y2, x3, y3, x4, y4;
			y1 = btLine[0][Idx].y; x1 = btLine[0][Idx].x; y2 = btLine[1][Idx].y; x2 = btLine[1][Idx].x;
			y3 = btLine[0][Idx + 1].y; x3 = btLine[0][Idx + 1].x; y4 = btLine[1][Idx + 1].y; x4 = btLine[1][Idx + 1].x;
			intersection_ll(btLine[0][Idx].y, btLine[0][Idx].x, btLine[1][Idx].y, btLine[1][Idx].x,
				btLine[0][Idx + 1].y, btLine[0][Idx + 1].x, btLine[1][Idx + 1].y, btLine[1][Idx + 1].x, &interRow, &interCol, &pa);
			int paval = pa[0].I();
			if (paval == 0)
			{
				btInterP[i].x = interCol[0].D();
				btInterP[i].y = interRow[0].D();
				setDetectOutData(BtPointsX, btInterP[i].x, i);
				setDetectOutData(BtPointsY, btInterP[i].y, i);
			}
			else {
				interok = false;
				btOK = false;
				allok = false;
			}
			setDetectOutData(BtPointsOK, interok, i);
		}
	}

	//cv::Point2d btImg[PointsNum];
	if (btOK)
	{
		cv::Point2d btWorld[PointsNum];
		for (int i = 0; i < PointsNum; i++)
		{
			btWorld[i].x = m_param->bottomMachineX[i];
			btWorld[i].y = m_param->bottomMachineY[i];
		}
		cv::Point3d dirworld;
		dirworld.x = m_param->bottomMachineX[PointsNum];
		dirworld.y = m_param->bottomMachineY[PointsNum];
		dirworld.z = m_param->bottomMachineZ_5;
		bool calibBTOk = m_hom3D5S[0]->calibrateBottom(btInterP, (cv::Point2d*)btWorld, PointsNum, btInterP[PointsNum], dirworld);
		if (calibBTOk)
		{
			const cv::Mat mat = m_hom3D5S[0]->homMat();
			for (int i = 0; i < 8; i++)
			{
				int row = i > 3 ? 1 : 0;
				int col = i % 4;
				m_param->BtHxy[i] = mat.at<double>(row, col);
			}
		}
		else {
			calibBTOk = false;
			btOK = false;
			allok = false;
		}
		setDetectOutData(BtCalibOK, calibBTOk);
	}
	setDetectOutData(BtAllOK, btOK);

	for (int s = 0; s < 2; s++)
	{
		for (int i = 0; i < SideNum; i++)
		{
			sideLineOK[s][i] = true;
			CvPoint2D32f line[2];
			int lineNum = 0;
			fitEdgeLine::ResType res = m_sideFitLine[s][i]->calculateLine(m_image, line, lineNum);
			if (res != fitEdgeLine::ResOK) {
				sideLineOK[s][i] = false;
				sideOK[s] = false;
				allok = false;
			}
			else {
				for (int j = 0; j < 2; j++)
				{
					sideLine[s][j][i].x = line[j].x;
					sideLine[s][j][i].y = line[j].y;
					if (s == 0)
					{
						setDetectOutData(Side0LineX, line[j].x, j, i);
						setDetectOutData(Side0LineY, line[j].y, j, i);
					}
					else {
						setDetectOutData(Side1LineX, line[j].x, j, i);
						setDetectOutData(Side1LineY, line[j].y, j, i);
					}
				}
			}
			setDetectOutData(SideLineOk, sideLineOK[s][i], s, i);
		}
		if (sideOK[s])
		{
			for (int i = 0; i < PointsNum; i++)
			{
				bool interok = true;
				HTuple pa;
				HTuple interRow, interCol;
				int Idx = i;
				intersection_ll(sideLine[s][0][Idx].y, sideLine[s][0][Idx].x, sideLine[s][1][Idx].y, sideLine[s][1][Idx].x,
					sideLine[s][0][Idx + 1].y, sideLine[s][0][Idx + 1].x, sideLine[s][1][Idx + 1].y, sideLine[s][1][Idx + 1].x, &interRow, &interCol, &pa);
				if (pa[0].I() == 0)
				{
					sideInterP[s][i].x = interCol[0].D();
					sideInterP[s][i].y = interRow[0].D();
					setDetectOutData(SidePointsX, sideInterP[s][i].x, s, i);
					setDetectOutData(SidePointsY, sideInterP[s][i].y, s, i);
				}
				else {
					interok = false;
					sideOK[s] = false;
					allok = false;
				}
				setDetectOutData(SidePointsOK, interok, s, i);
			}
		}
		if (sideOK[s])
		{
			cv::Point2d sideWorld[PointsNum];
			for (int i = 0; i < PointsNum; i++)
			{
				sideWorld[i].x = m_param->sideMachineX[s][i];
				sideWorld[i].y = m_param->sideMachineY[s][i];
			}
			Hom3d5S::ZDirection tmpdir;
			if (m_param->horOrVerWANG == 0)
			{
				if (s == 0)
				{
					tmpdir = Hom3d5S::ZDirection::RowDecr;
				}
				else if (s == 1) {
					tmpdir = Hom3d5S::ZDirection::RowIncr;
				}
			}
			else {
				if (s == 0)
				{
					tmpdir = Hom3d5S::ZDirection::ColDecr;
				}
				else if (s == 1) {
					tmpdir = Hom3d5S::ZDirection::ColIncr;
				}
			}
			bool calibSideOk = m_hom3D5S[s + 1]->calibrate((cv::Point2d*)sideInterP[s], (cv::Point2d*)sideWorld, PointsNum, tmpdir);
			if (calibSideOk)
			{
				const cv::Mat mat = m_hom3D5S[s + 1]->homMat();
				for (int i = 0; i < 8; i++)
				{
					int row = i > 3 ? 1 : 0;
					int col = i % 4;
					m_param->SideHxy[s][i] = mat.at<double>(row, col);
				}
			}
			else {
				calibSideOk = false;
				sideOK[s] = false;
				allok = false;
			}
			setDetectOutData(SideCalibOK, calibSideOk, s);
		}
		setDetectOutData(SideAllOK, sideOK[s], s);
	}
	setDetectOutData(ALLOK, allok);
	if (allok)
	{
		HTuple firstOut3Hom;
		for (int i = 0; i < 8; i++)
		{
			firstOut3Hom.Append(m_param->BtHxy[i]);
		}
		for (int s = 0; s < 2; s++)
		{
			for (int i = 0; i < 8; i++)
			{
				firstOut3Hom.Append(m_param->SideHxy[s][i]);
			}
		}
		toQList(firstOut3Hom, **p_first3dOut);
		//setDetectOutData(FirstOut3Hom, firstOut3Hom);
	}

	for (int i = 0; i < 2; i++)
	{
		delete[]btLine[i];
		delete[]sideLine[0][i];
		delete[]sideLine[1][i];
	}

	return allok;  //标定的返回结果无影响
}

bool calib3D5S_homMod::detectTrans()
{
	bool allok = true;

	HTuple firstInHom;
	toHtuple(p_first3dIn, firstInHom);
	//getDetectInData(FirstIn3Hom, firstInHom);
	try
	{
		if (firstInHom.Num() != (3 * 8))
		{
			m_inDataOk = false;
			return false;
		}
		else {
			for (int i = 0; i < 3; i++)
			{
				if (m_hom3D5S[i])
				{
					delete m_hom3D5S[i];
					m_hom3D5S[i] = new Hom3d5S(firstInHom[i * 8 + 0].D(), firstInHom[i * 8 + 1].D(), firstInHom[i * 8 + 2].D(), firstInHom[i * 8 + 3].D(),
						firstInHom[i * 8 + 4].D(), firstInHom[i * 8 + 5].D(), firstInHom[i * 8 + 6].D(), firstInHom[i * 8 + 7].D());
				}
				for (int j = 0; j < 8; j++)
				{
					int row = j > 3 ? 1 : 0;
					int col = j % 4;
					if (i == 0)
					{
						m_param->BtHxy[j] = firstInHom[i * 8 + j];
					}
					else {
						m_param->SideHxy[i - 1][j] = firstInHom[i * 8 + j];
					}
				}
			}
		}
	}
	catch (HException& ex) {
		m_inDataOk = false;
		return false;
	}

	CvPoint2D32f *btTransLine[2];
	CvPoint2D32f *sideLine[2][2];
	CvPoint2D32f *sideTransLine[2];

	bool btTransLineOK[BtTransNum];
	bool sideTransLineOK[SideTransNum];
	bool sideLineOK[2][SideNum];

	bool btTransOK = true; //区域计算ok,lineok->interok->calibok
	bool sideTransOK = true;
	bool sideOK[2] = { true,true };

	cv::Point2d btTransInterP[PointsNum];
	cv::Point2d sideTransInterP[PointsNum];
	cv::Point2d sideInterP[2][PointsNum];
	for (int i = 0; i < 2; i++)
	{
		btTransLine[i] = new CvPoint2D32f[BtTransNum];
		sideTransLine[i] = new CvPoint2D32f[SideTransNum];
		sideLine[0][i] = new CvPoint2D32f[SideNum];
		sideLine[1][i] = new CvPoint2D32f[SideNum];
	}

	for (int i = 0; i < BtTransNum; i++)
	{
		btTransLineOK[i] = true;
		CvPoint2D32f line[2];
		int lineNum = 0;
		fitEdgeLine::ResType res = m_transBtFitLine[i]->calculateLine(m_image, line, lineNum);
		if (res != fitEdgeLine::ResOK) {
			btTransLineOK[i] = false;
			btTransOK = false;
			allok = false;
		}
		else {
			for (int j = 0; j < 2; j++)
			{
				btTransLine[j][i].x = line[j].x;
				btTransLine[j][i].y = line[j].y;
				setDetectOutData(BtTransLineX, btTransLine[j][i].x, j, i);
				setDetectOutData(BtTransLineY, btTransLine[j][i].y, j, i);
			}
		}
		setDetectOutData(BtTransLineOk, btTransLineOK[i], i);
	}
	if (btTransOK)
	{
		for (int i = 0; i < PointsNum; i++)
		{
			bool interok = true;
			HTuple pa;
			HTuple interRow, interCol;
			int Idx = i * 2;
			double x1, y1, x2, y2, x3, y3, x4, y4;
			y1 = btTransLine[0][Idx].y; x1 = btTransLine[0][Idx].x; y2 = btTransLine[1][Idx].y; x2 = btTransLine[1][Idx].x;
			y3 = btTransLine[0][Idx + 1].y; x3 = btTransLine[0][Idx + 1].x; y4 = btTransLine[1][Idx + 1].y; x4 = btTransLine[1][Idx + 1].x;
			intersection_ll(btTransLine[0][Idx].y, btTransLine[0][Idx].x, btTransLine[1][Idx].y, btTransLine[1][Idx].x,
				btTransLine[0][Idx + 1].y, btTransLine[0][Idx + 1].x, btTransLine[1][Idx + 1].y, btTransLine[1][Idx + 1].x, &interRow, &interCol, &pa);
			int paval = pa[0].I();
			if (paval == 0)
			{
				btTransInterP[i].x = interCol[0].D();
				btTransInterP[i].y = interRow[0].D();
				setDetectOutData(BtTransPointsX, btTransInterP[i].x, i);
				setDetectOutData(BtTransPointsY, btTransInterP[i].y, i);
			}
			else {
				interok = false;
				btTransOK = false;
				allok = false;
			}
			setDetectOutData(BtTransPointsOK, interok, i);
		}
	}
	setDetectOutData(BtTransAllOK, btTransOK);
	if (btTransOK == false)
	{
		setDetectOutData(ALLOK, allok);
		for (int i = 0; i < 2; i++)
		{
			delete[]btTransLine[i];
			delete[]sideTransLine[i];
			delete[]sideLine[0][i];
			delete[]sideLine[1][i];
		}
		return allok;
	}

	for (int i = 0; i < SideTransNum; i++)
	{
		sideTransLineOK[i] = true;
		CvPoint2D32f line[2];
		int lineNum = 0;
		fitEdgeLine::ResType res = m_transSideFitLine[i]->calculateLine(m_image, line, lineNum);
		if (res != fitEdgeLine::ResOK) {
			sideTransLineOK[i] = false;
			sideTransOK = false;
			allok = false;
		}
		else {
			for (int j = 0; j < 2; j++)
			{
				sideTransLine[j][i].x = line[j].x;
				sideTransLine[j][i].y = line[j].y;
				setDetectOutData(SideTransLineX, sideTransLine[j][i].x, j, i);
				setDetectOutData(SideTransLineY, sideTransLine[j][i].y, j, i);
			}
		}
		setDetectOutData(SideTransLineOk, sideTransLineOK[i], i);
	}
	if (sideTransOK)
	{
		for (int i = 0; i < PointsNum; i++)
		{
			bool interok = true;
			HTuple pa;
			HTuple interRow, interCol;
			int Idx = i < 2 ? i : (i + 1);
			double x1, y1, x2, y2, x3, y3, x4, y4;
			y1 = sideTransLine[0][Idx].y; x1 = sideTransLine[0][Idx].x; y2 = sideTransLine[1][Idx].y; x2 = sideTransLine[1][Idx].x;
			y3 = sideTransLine[0][Idx + 1].y; x3 = sideTransLine[0][Idx + 1].x; y4 = sideTransLine[1][Idx + 1].y; x4 = sideTransLine[1][Idx + 1].x;
			intersection_ll(sideTransLine[0][Idx].y, sideTransLine[0][Idx].x, sideTransLine[1][Idx].y, sideTransLine[1][Idx].x,
				sideTransLine[0][Idx + 1].y, sideTransLine[0][Idx + 1].x, sideTransLine[1][Idx + 1].y, sideTransLine[1][Idx + 1].x, &interRow, &interCol, &pa);
			int paval = pa[0].I();
			if (paval == 0)
			{
				sideTransInterP[i].x = interCol[0].D();
				sideTransInterP[i].y = interRow[0].D();
				setDetectOutData(SideTransPointsX, sideTransInterP[i].x, i);
				setDetectOutData(SideTransPointsY, sideTransInterP[i].y, i);
			}
			else {
				interok = false;
				sideTransOK = false;
				allok = false;
			}
			setDetectOutData(SideTransPointsOK, interok, i);
		}
	}
	setDetectOutData(SideTransAllOK, sideTransOK);
	if (sideTransOK == false)
	{
		for (int i = 0; i < 2; i++)
		{
			delete[]btTransLine[i];
			delete[]sideTransLine[i];
			delete[]sideLine[0][i];
			delete[]sideLine[1][i];
		}
		setDetectOutData(ALLOK, allok);
		return allok;
	}

	//cv::Point2d btImg[PointsNum];
	bool calABCDok = true;
	if (btTransOK && sideTransOK)
	{
		cv::Point3d Pabcd[PointsNum];

		cv::Point3d btWorld[PointsNum];
		for (int i = 0; i < PointsNum; i++)
		{
			btWorld[i].x = m_param->abcdMachineX[i];
			btWorld[i].y = m_param->abcdMachineY[i];
			btWorld[i].z = m_param->abcdMachineZ[i];
		}
		for (int i = 0; i < PointsNum; i++)
		{
			int hIdx = i / 2 + 1;
			calABCDok = m_hom3D5S[0]->calWorldPos(*m_hom3D5S[hIdx], sideTransInterP[i], *m_hom3D5S[0], btTransInterP[i], Pabcd[i]);
			if (calABCDok == false)
			{
				allok = false;
				break;
			}
		}
		if (calABCDok)
		{
			calABCDok = m_rigidTrans->fit(btWorld, Pabcd, 4);
			if (calABCDok == false) {
				allok = false;
			}
		}

		setDetectOutData(CalABCDOK, calABCDok);
		if (calABCDok == false)
		{
			for (int i = 0; i < 2; i++)
			{
				delete[]btTransLine[i];
				delete[]sideTransLine[i];
				delete[]sideLine[0][i];
				delete[]sideLine[1][i];
			}
			setDetectOutData(ALLOK, allok);
			return allok;
		}
	}

	for (int s = 0; s < 2; s++)
	{
		for (int i = 0; i < SideNum; i++)
		{
			sideLineOK[s][i] = true;
			CvPoint2D32f line[2];
			int lineNum = 0;
			fitEdgeLine::ResType res = m_sideFitLine[s][i]->calculateLine(m_image, line, lineNum);
			if (res != fitEdgeLine::ResOK) {
				sideLineOK[s][i] = false;
				sideOK[s] = false;
				allok = false;
			}
			else {
				for (int j = 0; j < 2; j++)
				{
					sideLine[s][j][i].x = line[j].x;
					sideLine[s][j][i].y = line[j].y;
					if (s == 0)
					{
						setDetectOutData(Side0LineX, line[j].x, j, i);
						setDetectOutData(Side0LineY, line[j].y, j, i);
					}
					else {
						setDetectOutData(Side1LineX, line[j].x, j, i);
						setDetectOutData(Side1LineY, line[j].y, j, i);
					}
				}
			}
			setDetectOutData(SideLineOk, sideLineOK[s][i], s, i);
		}
		if (sideOK[s])
		{
			for (int i = 0; i < PointsNum; i++)
			{
				bool interok = true;
				HTuple pa;
				HTuple interRow, interCol;
				int Idx = i;
				intersection_ll(sideLine[s][0][Idx].y, sideLine[s][0][Idx].x, sideLine[s][1][Idx].y, sideLine[s][1][Idx].x,
					sideLine[s][0][Idx + 1].y, sideLine[s][0][Idx + 1].x, sideLine[s][1][Idx + 1].y, sideLine[s][1][Idx + 1].x, &interRow, &interCol, &pa);
				if (pa[0].I() == 0)
				{
					sideInterP[s][i].x = interCol[0].D();
					sideInterP[s][i].y = interRow[0].D();
					setDetectOutData(SidePointsX, sideInterP[s][i].x, s, i);
					setDetectOutData(SidePointsY, sideInterP[s][i].y, s, i);
				}
				else {
					interok = false;
					sideOK[s] = false;
					allok = false;
				}
				setDetectOutData(SidePointsOK, interok, s, i);
			}
		}
		if (sideOK[s])
		{
			cv::Point3d *sideWorld;
			sideWorld = new cv::Point3d[PointsNum];
			for (int i = 0; i < PointsNum; i++)
			{
				sideWorld[i].x = m_param->sideMachineX[s][i];
				sideWorld[i].y = m_param->sideMachineY[s][i];
				sideWorld[i].z = 0;
			}
			cv::Point3d *sideWorldTrans;
			sideWorldTrans = new cv::Point3d[PointsNum];
			double sidewt[3][PointsNum];
			double sidet[3][PointsNum];

			for (int i = 0; i < PointsNum; i++)
			{
				sideWorldTrans[i] = m_rigidTrans->trans(sideWorld[i]);
				sidewt[0][i] = sideWorldTrans[i].x;
				sidewt[1][i] = sideWorldTrans[i].y;
				sidewt[2][i] = sideWorldTrans[i].z;

				if (i == 1 || i == 2)
				{
					m_sideWorldTrans[s * 2 + i - 1] = sideWorldTrans[i];
				}

				sidet[0][i] = sideWorld[i].x;
				sidet[1][i] = sideWorld[i].y;
				sidet[2][i] = sideWorld[i].z;
			}
			double aX, aY, aZ, tX, tY, tZ;
			m_rigidTrans->getParam(aX, aY, aZ, tX, tY, tZ);

			Hom3d5S::ZDirection tmpdir;
			if (m_param->horOrVerWANG == 0)
			{
				if (s == 0)
				{
					tmpdir = Hom3d5S::ZDirection::RowDecr;
				}
				else if (s == 1) {
					tmpdir = Hom3d5S::ZDirection::RowIncr;
				}
			}
			else {
				if (s == 0)
				{
					tmpdir = Hom3d5S::ZDirection::ColDecr;
				}
				else if (s == 1) {
					tmpdir = Hom3d5S::ZDirection::ColIncr;
				}
			}

			bool calibSideOk = m_homTrans[s]->calibrate((cv::Point2d*)sideInterP[s], sideWorldTrans, PointsNum, tmpdir);
			if (calibSideOk)
			{
				const cv::Mat mat = m_homTrans[s]->homMat();
				for (int i = 0; i < 8; i++)
				{
					int row = i > 3 ? 1 : 0;
					int col = i % 4;
					m_param->TransSideHxy[s][i] = mat.at<double>(row, col);
				}
			}
			else {
				calibSideOk = false;
				sideOK[s] = false;
				allok = false;
			}
			delete[]sideWorld;
			delete[]sideWorldTrans;
			setDetectOutData(SideCalibOK, calibSideOk, s);
		}
		setDetectOutData(SideAllOK, sideOK[s], s);
	}
	setDetectOutData(ALLOK, allok);
	if (allok)
	{
		HTuple fullOutHom;
		for (int i = 0; i < 8; i++)
		{
			fullOutHom.Append(m_param->BtHxy[i]);
		}

		for (int s = 0; s < 2; s++)
		{
			for (int i = 0; i < 8; i++)
			{
				if (m_param->horOrVerWANG == 1)
				{
					fullOutHom.Append(m_param->SideHxy[s][i]);
				}
				else {
					fullOutHom.Append(m_param->TransSideHxy[s][i]);
				}
			}
		}
		for (int s = 0; s < 2; s++)
		{
			for (int i = 0; i < 8; i++)
			{
				if (m_param->horOrVerWANG == 1)
				{
					fullOutHom.Append(m_param->TransSideHxy[s][i]);
				}
				else {
					fullOutHom.Append(m_param->SideHxy[s][i]);
				}
			}
		}
		toQList(fullOutHom, **p_3d5s);
		//setDetectOutData(FullOutHom, fullOutHom);
	}
	for (int i = 0; i < 2; i++)
	{
		delete[]btTransLine[i];
		delete[]sideTransLine[i];
		delete[]sideLine[0][i];
		delete[]sideLine[1][i];
	}
	return allok;  //标定的返回结果无影响
}

void calib3D5S_homMod::viewResultNormal(ImageView *iv)
{
	iv->setColor(255, 0, 0);
	bool btok = true;
	for (int i = 0; i < BottomNum; i++)
	{
		HTuple lineok;
		getDispOutData(BtLineOk, i, lineok);
		if (lineok[0].I())
		{
			iv->setColor(0, 255, 0);
			CvPoint2D32f line[2];
			for (int j = 0; j < 2; j++) {
				HTuple btlinex, btliney;
				getDispOutData(BtLineX, j, i, btlinex);
				getDispOutData(BtLineY, j, i, btliney);
				line[j].x = btlinex[0].D();
				line[j].y = btliney[0].D();
			}
			iv->dispLine(line[0].y, line[0].x, line[1].y, line[1].x);
		}
		else {
			btok = false;
		}
	}
	if (btok)
	{
		for (int i = 0; i < BtPointsNum; i++)
		{
			HTuple pok;
			getDispOutData(BtPointsOK, i, pok);
			if (pok[0].I())
			{
				iv->setColor(0, 255, 0);
				HTuple btpx, btpy;
				getDispOutData(BtPointsX, i, btpx);
				getDispOutData(BtPointsY, i, btpy);
				CvPoint2D32f p;
				p.x = btpx[0].D();
				p.y = btpy[0].D();

				iv->dispLine(p.y - 3.0, p.x, p.y + 3.0, p.x);
				iv->dispLine(p.y, p.x - 3.0, p.y, p.x + 3.0);
			}
			else {
				btok = false;
			}
		}
	}
	for (int s = 0; s < 2; s++)
	{
		bool sideok = true;
		for (int i = 0; i < SideNum; i++)
		{
			HTuple lineok;
			getDispOutData(SideLineOk, s, i, lineok);
			if (lineok[0].I())
			{
				iv->setColor(0, 255, 0);
				CvPoint2D32f line[2];
				for (int j = 0; j < 2; j++) {
					HTuple sidelinex, sideliney;
					if (s == 0)
					{
						getDispOutData(Side0LineX, j, i, sidelinex);
						getDispOutData(Side0LineY, j, i, sideliney);
					}
					else {
						getDispOutData(Side1LineX, j, i, sidelinex);
						getDispOutData(Side1LineY, j, i, sideliney);
					}
					line[j].x = sidelinex[0].D();
					line[j].y = sideliney[0].D();
				}
				iv->dispLine(line[0].y, line[0].x, line[1].y, line[1].x);
			}
			else {
				sideok = false;
			}
		}
		if (sideok)
		{
			for (int i = 0; i < PointsNum; i++)
			{
				HTuple pok;
				getDispOutData(SidePointsOK, s, i, pok);
				if (pok[0].I())
				{
					iv->setColor(0, 255, 0);
					HTuple sidepx, sidepy;
					getDispOutData(SidePointsX, s, i, sidepx);
					getDispOutData(SidePointsY, s, i, sidepy);
					CvPoint2D32f p;
					p.x = sidepx[0].D();
					p.y = sidepy[0].D();

					iv->dispLine(p.y - 3.0, p.x, p.y + 3.0, p.x);
					iv->dispLine(p.y, p.x - 3.0, p.y, p.x + 3.0);
				}
				else {
					btok = false;
				}
			}
		}
	}
}

void calib3D5S_homMod::viewResultTrans(ImageView *iv)
{
	if (!m_inDataOk)
		return;
	iv->setColor(255, 0, 0);
	bool btok = true;
	for (int i = 0; i < BtTransNum; i++)
	{
		HTuple lineok;
		getDispOutData(BtTransLineOk, i, lineok);
		if (lineok[0].I())
		{
			iv->setColor(0, 255, 0);
			CvPoint2D32f line[2];
			for (int j = 0; j < 2; j++) {
				HTuple btlinex, btliney;
				getDispOutData(BtTransLineX, j, i, btlinex);
				getDispOutData(BtTransLineY, j, i, btliney);
				line[j].x = btlinex[0].D();
				line[j].y = btliney[0].D();
			}
			iv->dispLine(line[0].y, line[0].x, line[1].y, line[1].x);
		}
		else {
			btok = false;
		}
	}
	if (btok)
	{
		for (int i = 0; i < PointsNum; i++)
		{
			HTuple pok;
			getDispOutData(BtTransPointsOK, i, pok);
			if (pok[0].I())
			{
				iv->setColor(0, 255, 0);
				HTuple btpx, btpy;
				getDispOutData(BtTransPointsX, i, btpx);
				getDispOutData(BtTransPointsY, i, btpy);
				CvPoint2D32f p;
				p.x = btpx[0].D();
				p.y = btpy[0].D();

				iv->dispLine(p.y - 3.0, p.x, p.y + 3.0, p.x);
				iv->dispLine(p.y, p.x - 3.0, p.y, p.x + 3.0);
			}
			else {
				btok = false;
			}
		}
	}
	HTuple bttansallok;
	getDispOutData(BtTransAllOK, bttansallok);
	if (bttansallok[0].I() == 0)
	{
		return;
	}

	bool sideTok = true;
	for (int i = 0; i < SideTransNum; i++)
	{
		HTuple lineok;
		getDispOutData(SideTransLineOk, i, lineok);
		if (lineok[0].I())
		{
			iv->setColor(0, 255, 0);
			CvPoint2D32f line[2];
			for (int j = 0; j < 2; j++) {
				HTuple btlinex, btliney;
				getDispOutData(SideTransLineX, j, i, btlinex);
				getDispOutData(SideTransLineY, j, i, btliney);
				line[j].x = btlinex[0].D();
				line[j].y = btliney[0].D();
			}
			iv->dispLine(line[0].y, line[0].x, line[1].y, line[1].x);
		}
		else {
			sideTok = false;
		}
	}
	if (sideTok)
	{
		for (int i = 0; i < PointsNum; i++)
		{
			HTuple pok;
			getDispOutData(SideTransPointsOK, i, pok);
			if (pok[0].I())
			{
				iv->setColor(0, 255, 0);
				HTuple btpx, btpy;
				getDispOutData(SideTransPointsX, i, btpx);
				getDispOutData(SideTransPointsY, i, btpy);
				CvPoint2D32f p;
				p.x = btpx[0].D();
				p.y = btpy[0].D();

				iv->dispLine(p.y - 3.0, p.x, p.y + 3.0, p.x);
				iv->dispLine(p.y, p.x - 3.0, p.y, p.x + 3.0);
			}
			else {
				sideTok = false;
			}
		}
	}
	HTuple sidetransallok;
	getDispOutData(SideTransAllOK, sidetransallok);
	if (sidetransallok[0].I() == 0)
		return;

	HTuple calibabcdok;
	getDispOutData(CalABCDOK, calibabcdok);
	if (calibabcdok[0].I() == 0)
		return;

	for (int s = 0; s < 2; s++)
	{
		bool sideok = true;
		for (int i = 0; i < SideNum; i++)
		{
			HTuple lineok;
			getDispOutData(SideLineOk, s, i, lineok);
			if (lineok[0].I())
			{
				iv->setColor(0, 255, 0);
				CvPoint2D32f line[2];
				for (int j = 0; j < 2; j++) {
					HTuple sidelinex, sideliney;
					if (s == 0)
					{
						getDispOutData(Side0LineX, j, i, sidelinex);
						getDispOutData(Side0LineY, j, i, sideliney);
					}
					else {
						getDispOutData(Side1LineX, j, i, sidelinex);
						getDispOutData(Side1LineY, j, i, sideliney);
					}
					line[j].x = sidelinex[0].D();
					line[j].y = sideliney[0].D();
				}
				iv->dispLine(line[0].y, line[0].x, line[1].y, line[1].x);
			}
			else {
				sideok = false;
			}
		}
		if (sideok)
		{
			for (int i = 0; i < PointsNum; i++)
			{
				HTuple pok;
				getDispOutData(SidePointsOK, s, i, pok);
				if (pok[0].I())
				{
					iv->setColor(0, 255, 0);
					HTuple sidepx, sidepy;
					getDispOutData(SidePointsX, s, i, sidepx);
					getDispOutData(SidePointsY, s, i, sidepy);
					CvPoint2D32f p;
					p.x = sidepx[0].D();
					p.y = sidepy[0].D();

					iv->dispLine(p.y - 3.0, p.x, p.y + 3.0, p.x);
					iv->dispLine(p.y, p.x - 3.0, p.y, p.x + 3.0);
				}
				else {
					btok = false;
				}
			}
		}
	}
}

void calib3D5S_homMod::textResultNormal(ResultText *text)
{
	text->setTextColor(QColor(0, 0, 0));
	HTuple allres;
	getDispOutData(ALLOK, allres);
	if (!allres[0].I())
	{
		text->setTextColor(QColor(255, 0, 0));
		text->append(QObject::tr("calibrate Failed!"));
	}
	else {
		text->append(QObject::tr("calibrate success!"));
		return;
	}
	bool btok = true;
	for (int i = 0; i < BottomNum; i++)
	{
		HTuple lineok;
		getDispOutData(BtLineOk, i, lineok);
		if (!lineok[0].I())
		{
			text->setTextColor(QColor(255, 0, 0));
			text->append(QObject::tr("bottom : line%1 measure failed!").arg(i + 1));
			btok = false;
		}
	}
	if (btok)
	{
		for (int i = 0; i < BtPointsNum; i++)
		{
			HTuple pok;
			getDispOutData(BtPointsOK, i, pok);
			if (!pok[0].I())
			{
				text->setTextColor(QColor(255, 0, 0));
				text->append(QObject::tr("bottom : intersection point %1 measure failed!").arg(i + 1));
				btok = false;
			}
		}
	}
	if (btok)
	{
		HTuple calibok;
		getDispOutData(BtCalibOK, calibok);
		if (!calibok[0].I())
		{
			text->setTextColor(QColor(255, 0, 0));
			text->append(QObject::tr("bottom : calibrate data failed!"));
			btok = false;
		}
	}
	for (int s = 0; s < 2; s++)
	{
		bool sideok = true;
		for (int i = 0; i < SideNum; i++)
		{
			HTuple lineok;
			getDispOutData(SideLineOk, s, i, lineok);
			if (!lineok[0].I())
			{
				text->setTextColor(QColor(255, 0, 0));
				text->append(QObject::tr("side %1 : line%2 measure failed!").arg(s + 1).arg(i + 1));
				sideok = false;
			}
		}
		if (sideok)
		{
			for (int i = 0; i < PointsNum; i++)
			{
				HTuple pok;
				getDispOutData(SidePointsOK, s, i, pok);
				if (!pok[0].I())
				{
					text->setTextColor(QColor(255, 0, 0));
					text->append(QObject::tr("side %1 : intersection point %2 measure failed!").arg(s + 1).arg(i + 1));
					sideok = false;
				}
			}
		}
		if (sideok)
		{
			HTuple calibok;
			getDispOutData(SideCalibOK, s, calibok);
			if (!calibok[0].I())
			{
				text->setTextColor(QColor(255, 0, 0));
				text->append(QObject::tr("side %1 : calibrate data failed!").arg(s + 1));
				sideok = false;
			}
		}
	}
}

void calib3D5S_homMod::textResultTrans(ResultText *text)
{
	text->setTextColor(QColor(0, 0, 0));
	HTuple allres;
	getDispOutData(ALLOK, allres);
	if (!allres[0].I())
	{
		text->setTextColor(QColor(255, 0, 0));
		text->append(QObject::tr("calibrate Failed!"));
	}
	else {
		text->append(QObject::tr("calibrate success!"));
		return;
	}
	bool btok = true;
	for (int i = 0; i < BtTransNum; i++)
	{
		HTuple lineok;
		getDispOutData(BtTransLineOk, i, lineok);
		if (!lineok[0].I())
		{
			text->setTextColor(QColor(255, 0, 0));
			text->append(QObject::tr("bottom : line%1 measure failed!").arg(i + 1));
			btok = false;
		}
	}
	if (btok)
	{
		for (int i = 0; i < PointsNum; i++)
		{
			HTuple pok;
			getDispOutData(BtTransPointsOK, i, pok);
			if (!pok[0].I())
			{
				text->setTextColor(QColor(255, 0, 0));
				text->append(QObject::tr("bottom : intersection point %1 measure failed!").arg(i + 1));
				btok = false;
			}
		}
	}
	HTuple bttransok;
	getDispOutData(BtTransAllOK, bttransok);
	if (bttransok[0].I() == 0)
	{
		return;
	}

	bool sideTransok = true;
	for (int i = 0; i < SideTransNum; i++)
	{
		HTuple lineok;
		getDispOutData(SideTransLineOk, i, lineok);
		if (!lineok[0].I())
		{
			text->setTextColor(QColor(255, 0, 0));
			text->append(QObject::tr("side trans : line%1 measure failed!").arg(i + 1));
			sideTransok = false;
		}
	}
	if (sideTransok)
	{
		for (int i = 0; i < PointsNum; i++)
		{
			HTuple pok;
			getDispOutData(SideTransPointsOK, i, pok);
			if (!pok[0].I())
			{
				text->setTextColor(QColor(255, 0, 0));
				text->append(QObject::tr("side trans : intersection point %1 measure failed!").arg(i + 1));
				sideTransok = false;
			}
		}
	}

	HTuple sidetransok;
	getDispOutData(SideTransAllOK, sidetransok);
	if (sidetransok[0].I() == 0)
	{
		return;
	}

	HTuple calabcdok;
	getDispOutData(CalABCDOK, calabcdok);
	if (calabcdok[0].I() == 0)
	{
		text->setTextColor(QColor(255, 0, 0));
		text->append(QObject::tr("translate calibration failed!"));
		return;
	}

	for (int s = 0; s < 2; s++)
	{
		bool sideok = true;
		for (int i = 0; i < SideNum; i++)
		{
			HTuple lineok;
			getDispOutData(SideLineOk, s, i, lineok);
			if (!lineok[0].I())
			{
				text->setTextColor(QColor(255, 0, 0));
				text->append(QObject::tr("side %1 : line%2 measure failed!").arg(s + 1).arg(i + 1));
				sideok = false;
			}
		}
		if (sideok)
		{
			for (int i = 0; i < PointsNum; i++)
			{
				HTuple pok;
				getDispOutData(SidePointsOK, s, i, pok);
				if (!pok[0].I())
				{
					text->setTextColor(QColor(255, 0, 0));
					text->append(QObject::tr("side %1 : intersection point %2 measure failed!").arg(s + 1).arg(i + 1));
					sideok = false;
				}
			}
		}
		if (sideok)
		{
			HTuple calibok;
			getDispOutData(SideCalibOK, s, calibok);
			if (!calibok[0].I())
			{
				text->setTextColor(QColor(255, 0, 0));
				text->append(QObject::tr("side %1 : calibrate data failed!").arg(s + 1));
				sideok = false;
			}
		}
	}
}
