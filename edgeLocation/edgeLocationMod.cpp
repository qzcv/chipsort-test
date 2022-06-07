#include "edgeLocationMod.h"
#include "edgeLocationParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"

#define FindOk "findok"  //是否有区域内未检1测到内容，如果有则什么都不显示直接NG
#define RegionOk "regionOk" //上下左右区域是1否OK
#define MaxAngleOK "maxAngleok"   //最大角度是1否OK，如果不OK直接NG
#define MaxAngle "maxAngle"       //最大角度的1值
#define EndPointOk "endpointOK"   //端1点检测是否成功
#define BeginRow "BeginEdgeRow"   //检测端点时候的端点坐1标
#define BeginCol "BeginEdgeCol"
#define EndRow "EndEdgeRow"
#define EndCol "EndEdgeCol"

#define DispPointsImgOk "dispImgOk"  //是否有轮廓检测到
#define DispPointsImg "displayPointsImg"
#define ErrWidthOk "WidthOK"
#define ErrWidthMax "Widthmax"
#define ErrWidthMin "Widthmin"

#define ErrWidthDiffOk "WidthDiffOK"
#define ErrWidthDiff "WidthDiff"
#define ErrHeightDiffOk "HeightDiffOK"
#define ErrHeightDiff "HeightDiff"

#define ErrHeightOk "HeightOk"
#define ErrHeightMin "HeightMin"
#define ErrHeightMax "HeightMax"
#define ErrAngleOk "angleOK"
#define ErrAngle "angle" //最大偏转角1度
#define ErrCenterShiftOk "ErrcentershiftOK"
#define ErrCenterShift "Errcentershift" //与模板中心相对应的中心偏移1值

#define ImageFull "Imagefull"  //图片是否ok

#define TmpHomMat "TmpHomMat"  //用于保存显示的矩阵

edgeLocationMod::edgeLocationMod()
{
	iniData();
}

edgeLocationMod::edgeLocationMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

edgeLocationMod::~edgeLocationMod()
{
	delete m_param;
	delete m_image;
	for (auto i = 0;i < 4;++i)
		delete m_edgeLine[i];
}

int edgeLocationMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int edgeLocationMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void edgeLocationMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void edgeLocationMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
	if (level == ProductLevel)
		iniDetector();
}

int edgeLocationMod::run(const QString &funName)
{
	p_corners->clear();
	*p_offCenter = cv::Point2d(0, 0);

	for (int i = 0;i<MAX_TESTITEMS_NUM;i++) {
		m_testItemsStatus[i] = 1;
	}
	bool allok = true;
	if (p_im->empty()) {
		allok = false;
		//setDetectOutData(AllOK, allok);
		setDetectOutData(ImageFull, false);
		m_testItemsStatus[0] = 0;
		*p_allok = false;
		return -1;
		//return allok;
	}
	else {
		setDetectOutData(ImageFull, true);
		HalOpenCV::cv2halImg(*p_im, m_image, false);
	}

	double mk;//像素比例
	if (m_param->IsInputPixel) {
// 		try {
// 			HTuple k;
// 			getDetectInData(InputPixSize, k);
// 			mk = k[0].D();
// 		}
// 		catch (HException& ex) {
// 			mk = m_param->pixSize;
// 		}
		mk = *p_pixSize;
	}
	else {
		mk = m_param->pixSize;
	}
	bool roiFound[4]; //各边界是否找1到
	for (int i = 0; i<4;i++) {
		roiFound[i] = true;
	}
	//up
	int searchdir[4];
	HTuple hommat2d;
	double rotateAngle;
	if (m_param->IsRefer) {
// 		try {
// 			getDetectInData(HomMat2D, hommat2d);
// 			double tv = hommat2d[0].D();
// 		}
// 		catch (HException& ex) {
// 			hom_mat2d_identity(&hommat2d);
// 		}
		if (p_homMat2d->size() == cv::Size(3, 2))
			HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &hommat2d);
		else
			hom_mat2d_identity(&hommat2d);
		rotateAngle = atan((hommat2d[3].D() / hommat2d[0].D()));
		setDetectOutData(TmpHomMat, hommat2d);
	}
	HTuple RowBegin[4], ColBegin[4], RowEnd[4], ColEnd[4];  //上右下左的顺序，拟合的四个边界起始点
	HTuple m_rowEdge[4], m_colEdge[4];  //四条边的扫描到的点数
	CvPoint2D32f edgeOffVector[4];  //求取的边界的外扩方向向量，左上角顺时针，为单位向量

	for (int i = 0;i<4;i++)
	{
		if (m_param->IsFixWidth[i]) {
			setDetectOutData(FindOk, true, i);
			continue;
		}

		CvPoint2D32f line[2];
		int lineNum;
		if (m_param->IsRefer) {
			m_edgeLine[i]->setRefer(hommat2d);
		}
		else {
			hom_mat2d_identity(&hommat2d);
			m_edgeLine[i]->setRefer(hommat2d);
		}
		fitEdgeLine::ResType res = m_edgeLine[i]->calculateLine(m_image, line, lineNum);


		if (res != fitEdgeLine::ResOK) {
			roiFound[i] = false;
			allok = false;
			setDetectOutData(FindOk, roiFound[i], i);
		}
		else {
			RowBegin[i][0] = line[0].y;
			RowEnd[i][0] = line[1].y;
			ColBegin[i][0] = line[0].x;
			ColEnd[i][0] = line[1].x;
			if (((RowEnd[i][0].D()<RowBegin[i][0].D()) && (i == 1 || i == 3)) || ((ColEnd[i][0].D()<ColBegin[i][0].D()) && (i == 2 || i == 0))) {   //调转开头点和结尾点，使全部符合图片上的正向顺1序
				double tmp;
				tmp = ColEnd[i][0].D();
				ColEnd[i][0] = ColBegin[i][0];
				ColBegin[i][0] = tmp;
				tmp = RowEnd[i][0].D();
				RowEnd[i][0] = RowBegin[i][0];
				RowBegin[i][0] = tmp;
			}

			double dy = RowEnd[i][0].D() - RowBegin[i][0].D();
			double dx = ColEnd[i][0].D() - ColBegin[i][0].D();
			double dxy = sqrt(dx*dx + dy*dy);
			dx = dx / dxy;
			dy = dy / dxy; //单位向量
			double da = PI;
			if (i == 0 || i == 1) {
				da = PI / 2.0; //边顺时针向
			}
			else {
				da = -PI / 2.0;//边逆时针向
			}
			edgeOffVector[i].x = dx*cos(da) + dy*sin(da);   //旋转九十度垂直
			edgeOffVector[i].y = -dx*sin(da) + dy*cos(da);

			RowBegin[i][0] = RowBegin[i][0].D() + m_param->lineOffset[i] * edgeOffVector[i].y;
			RowEnd[i][0] = RowEnd[i][0].D() + m_param->lineOffset[i] * edgeOffVector[i].y;

			ColBegin[i][0] = ColBegin[i][0].D() + m_param->lineOffset[i] * edgeOffVector[i].x;
			ColEnd[i][0] = ColEnd[i][0].D() + m_param->lineOffset[i] * edgeOffVector[i].x;

			//if (i==0||i==2){
			//	RowBegin[i][0]=m_param->lineOffset[i]+RowBegin[i][0].D();
			//	RowEnd[i][0]=m_param->lineOffset[i]+RowEnd[i][0].D();
			//}
			//else if(i==1||i==3){
			//	ColBegin[i][0]=m_param->lineOffset[i]+ColBegin[i][0].D();
			//	ColEnd[i][0]=m_param->lineOffset[i]+ColEnd[i][0].D();
			//}
			setDetectOutData(FindOk, roiFound[i], i);
		}
	} //for loop end

	HTuple rows, cols;
	long totalNum = 0;
	for (int i = 0;i<4;i++) {
		if (m_param->IsFixWidth[i]) {
			continue;
		}
		if (roiFound[i]) {
			double Rowval, Colval;
			HTuple tmprowE, tmpcolE;
			m_edgeLine[i]->getEdgePoints(tmprowE, tmpcolE);
			int num = tmprowE.Num();
			for (int j = 0;j<num;j++) {
				rows[totalNum] = tmprowE[j];
				cols[totalNum] = tmpcolE[j];
				totalNum++;
			}
		}
	}
	if ((rows.Num() == cols.Num()) && rows.Num()>10) {   //得到的轮廓数目不能太少
		Hobject dispPointsImg;
		gen_region_points(&dispPointsImg, rows, cols);
		setDetectOutData(DispPointsImgOk, true);
		setDetectOutObject(DispPointsImg, dispPointsImg);
	}
	else {
		allok = false;
		setDetectOutData(DispPointsImgOk, false);
	}
	for (int i = 0;i<4;i++) {
		if (!roiFound[i]) {
			allok = false;
			m_testItemsStatus[0] = 0;
			*p_allok = allok;
			return -1;
			//setDetectOutData(AllOK, allok);
			//return allok;
		}
	}
	float EveryAngle[4];
	for (int i = 0;i<4;i++) {
		if (m_param->IsFixWidth[i]) {
			continue;
		}
		if (!roiFound[i]) {
			continue;
		}
		if (i == 1 || i == 3) {
			EveryAngle[i] = PI / 2 + atan((ColEnd[i][0].D() - ColBegin[i][0].D()) / (RowEnd[i][0].D() - RowBegin[i][0].D()));
		}
		else {
			EveryAngle[i] = -atan((RowEnd[i][0].D() - RowBegin[i][0].D()) / (ColEnd[i][0].D() - ColBegin[i][0].D()));
		}
		if (((EveryAngle[i] * 180.0 / PI<90 + m_param->maxAngle) && (EveryAngle[i] * 180.0 / PI>90 - m_param->maxAngle)) ||
			((EveryAngle[i] * 180.0 / PI<0 + m_param->maxAngle) && (EveryAngle[i] * 180.0 / PI>0 - m_param->maxAngle))) {
			//m_angleOK=true;
		}
		else {
			allok = false;
			setDetectOutData(MaxAngleOK, false);
			setDetectOutData(MaxAngle, EveryAngle[i] / M_PI*180.0);
			m_testItemsStatus[3] = 0;
			*p_allok = false;
			return -1;
			//setDetectOutData(AllOK, allok);
			//return allok;
		}
	}
	setDetectOutData(MaxAngleOK, true);
	Hlong imgWid, imgHei;
	get_image_size(*m_image, &imgWid, &imgHei);
	if (m_param->IsSideNoDetect)
	{
		if (m_param->SideDetectType == 0 || m_param->SideDetectType == 1)
		{
			int pIndex[2][2] = { { 0,2 },{ 1,3 } };
			/*if (m_param->SideDetectType==0)
			{*/
			int rIndex[2][4] = { { 0,1,0,1 },{ 0,0,1,1 } }; //上下左右，两条线对应两个数组
			int cIndex[2][4] = { { 0,0,1,1 },{ 0,1,0,1 } };
			for (int i = 0;i<2;i++)  //两个区域
			{
				int roiIndex = 0;
				for (int j = 0;j<pIndex[m_param->SideDetectType][i] + 1;j++)
				{
					roiIndex += m_param->roiNum[j];
				}
				cv::Point2d p[2][2];  //对应两个边，起始两个点
				double rowVal[2], colVal[2];
				rowVal[0] = m_param->RoiRegionVector[roiIndex].row1;
				rowVal[1] = m_param->RoiRegionVector[roiIndex].row2;
				colVal[0] = m_param->RoiRegionVector[roiIndex].col1;
				colVal[1] = m_param->RoiRegionVector[roiIndex].col2;

				for (int j = 0;j<2;j++) //两条边
				{
					for (int z = 0;z<2;z++)//两个点
					{
						p[j][z].x = colVal[cIndex[m_param->SideDetectType][j * 2 + z]];
						p[j][z].y = rowVal[rIndex[m_param->SideDetectType][j * 2 + z]];
						if (m_param->IsRefer)
						{
							affine_trans_pixel(hommat2d, p[j][z].y, p[j][z].x, &p[j][z].y, &p[j][z].x);
						}
					}
				}

				for (int j = 0;j<2;j++)
				{
					HTuple interR, interC, interP;
					int idx = pIndex[m_param->SideDetectType][i];  //
					double rb, re, cb, ce;
					rb = RowBegin[idx][0].D();cb = ColBegin[idx][0].D();re = RowEnd[idx][0].D();ce = ColEnd[idx][0].D();
					intersection_ll(RowBegin[idx][0].D(), ColBegin[idx][0].D(), RowEnd[idx][0].D(), ColEnd[idx][0].D(),
						p[j][0].y, p[j][0].x, p[j][1].y, p[j][1].x, &interR, &interC, &interP);
					double ir, ic;
					ir = interR[0].D();ic = interC[0].D();
					if (interP[0].I() == 0)
					{
						if ((interR[0].D()<imgHei - 1) && (interR[0].D()>0) && (interC[0].D()<imgWid - 1) && (interC[0].D()>0))
						{
							if (j == 0)
							{
								RowBegin[idx][0] = interR[0].D();
								ColBegin[idx][0] = interC[0].D();
							}
							else {
								RowEnd[idx][0] = interR[0].D();
								ColEnd[idx][0] = interC[0].D();
							}
						}
					}
				}
			}
			if (m_param->SideDetectType == 1) {   // 上下
				RowBegin[0][0] = RowBegin[3][0].D();
				RowEnd[0][0] = RowBegin[1][0].D();
				ColBegin[0][0] = ColBegin[3][0].D();
				ColEnd[0][0] = ColBegin[1][0].D();

				RowBegin[2][0] = RowEnd[3][0].D();
				RowEnd[2][0] = RowEnd[1][0].D();
				ColBegin[2][0] = ColEnd[3][0].D();
				ColEnd[2][0] = ColEnd[1][0].D();
			}
			else {  //左右
				RowBegin[1][0] = RowEnd[0][0].D();
				RowEnd[1][0] = RowEnd[2][0].D();
				ColBegin[1][0] = ColEnd[0][0].D();
				ColEnd[1][0] = ColEnd[2][0].D();

				RowBegin[3][0] = RowBegin[0][0].D();
				RowEnd[3][0] = RowBegin[2][0].D();
				ColBegin[3][0] = ColBegin[0][0].D();
				ColEnd[3][0] = ColBegin[2][0].D();
			}


		}
		else {
			if (m_param->IsFixWidth[0]) {
				RowBegin[0][0] = RowBegin[2][0].D() - m_param->fixWidth;
				RowEnd[0][0] = RowEnd[2][0].D() - m_param->fixWidth;
				ColBegin[0][0] = ColBegin[2][0].D();
				ColEnd[0][0] = ColEnd[2][0].D();
			}
			else if (m_param->IsFixWidth[2]) {
				RowBegin[2][0] = RowBegin[0][0].D() + m_param->fixWidth;
				RowEnd[2][0] = RowEnd[0][0].D() + m_param->fixWidth;
				ColBegin[2][0] = ColBegin[0][0].D();
				ColEnd[2][0] = ColEnd[0][0].D();
			}
			else if (m_param->IsFixWidth[1]) {
				RowBegin[1][0] = RowBegin[3][0].D();
				RowEnd[1][0] = RowEnd[3][0].D();
				ColBegin[1][0] = ColBegin[3][0].D() + m_param->fixWidth;
				ColEnd[1][0] = ColEnd[3][0].D() + m_param->fixWidth;
			}
			else if (m_param->IsFixWidth[3]) {
				RowBegin[3][0] = RowBegin[1][0].D();
				RowEnd[3][0] = RowEnd[1][0].D();
				ColBegin[3][0] = ColBegin[1][0].D() - m_param->fixWidth;
				ColEnd[3][0] = ColEnd[1][0].D() - m_param->fixWidth;
			}
		}
	}

	for (int i = 0;i<4;i++) {
		if (!roiFound[i]) {
			continue;
		}
		setDetectOutData(BeginRow, RowBegin[i][0], i);
		setDetectOutData(BeginCol, ColBegin[i][0], i);
		setDetectOutData(EndRow, RowEnd[i][0], i);
		setDetectOutData(EndCol, ColEnd[i][0], i);
	}
	HTuple finalEndRow[4], finalEndCol[4], isparel;
	bool interok[4];
	for (int i = 0;i<4;i++) {
		if (i != 3) {
			intersection_ll(RowBegin[i], ColBegin[i], RowEnd[i], ColEnd[i],
				RowBegin[i + 1], ColBegin[i + 1], RowEnd[i + 1], ColEnd[i + 1], &finalEndRow[i], &finalEndCol[i], &isparel);
		}
		else {
			intersection_ll(RowBegin[3], ColBegin[3], RowEnd[3], ColEnd[3],
				RowBegin[0], ColBegin[0], RowEnd[0], ColEnd[0], &finalEndRow[i], &finalEndCol[i], &isparel);
		}
		if (isparel[0].I()) {
			interok[i] = 0;
			allok = false;
			m_testItemsStatus[0] = 0;
			*p_allok = false;
			return -1;
			//setDetectOutData(AllOK, allok);
			//return allok;
		}
		else {
			interok[i] = 1;
		}
	}

	HTuple centerRoww, centerColl;
	intersection_ll(finalEndRow[0], finalEndCol[0], finalEndRow[2], finalEndCol[2],
		finalEndRow[1], finalEndCol[1], finalEndRow[3], finalEndCol[3], &centerRoww, &centerColl, &isparel);
	if (isparel[0].I()) {
		allok = false;
		m_testItemsStatus[0] = 0;
		*p_allok = false;
		return -1;
		//setDetectOutData(AllOK, allok);
		//return allok;
	}
	setDetectOutData(CenterRow, centerRoww);
	setDetectOutData(CenterCol, centerColl);

	HTuple fnalRow, fnalCol;
	double frow[5], fcol[5];   //finalEndRow 顺序是右上角顺时针
	for (int i = 0;i<4;i++) {
		frow[i] = finalEndRow[i][0].D();
		fcol[i] = finalEndCol[i][0].D();
		if (i != 3) {
			fnalRow[i + 1] = finalEndRow[i];
			fnalCol[i + 1] = finalEndCol[i];
		}
		else if (i == 3) {
			fnalRow[0] = finalEndRow[3];
			fnalCol[0] = finalEndCol[3];
		}
	}
	fnalRow[4] = fnalRow[0].D();
	fnalCol[4] = fnalCol[0].D();
	//double tmpr,tmpc;
	//tmpr=fnalRow[1].D();
	//tmpc=fnalCol[1].D();

	//fnalRow[1]=fnalRow[2].D();
	//fnalCol[1]=fnalCol[2].D();
	//fnalRow[2]=tmpr;
	//fnalCol[2]=tmpc;

	p_corners->clear();
	int numm = fnalRow.Num();
	for (auto i = 0;i < numm;++i)
	{
		double y = fnalRow[i].D();
		double x = fnalCol[i].D();
		p_corners->push_back(cv::Point2d(x, y));
	}
	//setDetectOutData(FinalRow, fnalRow);  //左上角顺时针
	//setDetectOutData(FinalCol, fnalCol);

	double linemidcol[2][2], linemidrow[2][2];
	linemidrow[0][0] = (fnalRow[0].D() + fnalRow[1].D()) / 2.0;
	linemidcol[0][0] = (fnalCol[0].D() + fnalCol[1].D()) / 2.0;
	linemidrow[0][1] = (fnalRow[2].D() + fnalRow[3].D()) / 2.0;
	linemidcol[0][1] = (fnalCol[2].D() + fnalCol[3].D()) / 2.0;
	linemidrow[1][0] = (fnalRow[1].D() + fnalRow[2].D()) / 2.0;
	linemidcol[1][0] = (fnalCol[1].D() + fnalCol[2].D()) / 2.0;
	linemidrow[1][1] = (fnalRow[3].D() + fnalRow[4].D()) / 2.0;
	linemidcol[1][1] = (fnalCol[3].D() + fnalCol[4].D()) / 2.0;
	if (m_param->IsCsp) {
		HTuple incrow, inccol;
		intersection_ll(linemidrow[0][0], linemidcol[0][0], linemidrow[0][1], linemidcol[0][1],
			linemidrow[1][0], linemidcol[1][0], linemidrow[1][1], linemidcol[1][1], &incrow, &inccol, &isparel);
		Hlong imageW, imageH;
		get_image_size(*m_image, &imageW, &imageH);
		double offcx = inccol[0].D() - double(imageW - 1)*0.5;
		double offcy = double(imageH - 1)*0.5 - incrow[0].D();  //输出的轴正负关系按常规笛卡尔
		*p_offCenter = cv::Point2d(offcx*mk, offcy*mk);
		//HTuple offcenter;
		//offcenter[0] = offcx*mk;
		//offcenter[1] = offcy*mk;
		//setDetectOutData(OFFCenter, offcenter);
	}
	float tmpAngle[4];
	if (m_param->IsErrAngle)
	{
		bool errAngleOk = true;
		float maxangle = 0;

		for (int i = 0;i<4;i++) {
			if (i>1) {
				tmpAngle[i] = PI / 2 + atan((fnalCol[2 * i - 3] - fnalCol[6 - 2 * i]) / (fnalRow[2 * i - 3] - fnalRow[6 - 2 * i]));//1,2   3,0
			}
			else {
				tmpAngle[i] = -atan((fnalRow[2 * i] - fnalRow[2 * i + 1]) / (fnalCol[2 * i] - fnalCol[2 * i + 1]));   //0,1     2,3
			}
		}
		for (int i = 0;i<4;i++) {
			if (((tmpAngle[i] * 180.0) / PI<(44 + 90)) && ((tmpAngle[i] * 180.0) / PI>(-44 + 90))) {
				tmpAngle[i] -= 90.0*PI / 180.0;
			}
			if (abs(tmpAngle[i])>maxangle) {
				maxangle = abs(tmpAngle[i]);
			}
		}
		if (maxangle>m_param->errAngle) {
			m_testItemsStatus[3] = 0;
			errAngleOk = false;
			allok = false;
		}
		setDetectOutData(ErrAngleOk, errAngleOk);
		setDetectOutData(ErrAngle, maxangle / PI * 180);
	}
	if (m_param->IsErrWidth || m_param->IsErrWidthDiff)
	{
		bool errwidthok = true;
		bool errwidthdiffok = true;
		float maxwidth = 0;
		float minwidth = 10000;
		float dis1, dis2;
		dis1 = sqrt((finalEndRow[0][0].D() - finalEndRow[3][0].D())*(finalEndRow[0][0].D() - finalEndRow[3][0].D()) +
			(finalEndCol[0][0].D() - finalEndCol[3][0].D())*(finalEndCol[0][0].D() - finalEndCol[3][0].D()));
		dis1 *= mk;
		if (maxwidth<dis1)
			maxwidth = dis1;
		if (minwidth>dis1)
			minwidth = dis1;
		dis2 = sqrt((finalEndRow[1][0].D() - finalEndRow[2][0].D())*(finalEndRow[1][0].D() - finalEndRow[2][0].D()) +
			(finalEndCol[1][0].D() - finalEndCol[2][0].D())*(finalEndCol[1][0].D() - finalEndCol[2][0].D()));
		dis2 *= mk;
		if (maxwidth<dis2)
			maxwidth = dis2;
		if (minwidth>dis2)
			minwidth = dis2;
		if (m_param->IsErrWidth) {
			if (maxwidth>m_param->errWidthMax || minwidth<m_param->errWidthMin) {
				errwidthok = false;
				m_testItemsStatus[1] = 0;
				allok = false;
			}
			setDetectOutData(ErrWidthOk, errwidthok);
			setDetectOutData(ErrWidthMax, maxwidth);
			setDetectOutData(ErrWidthMin, minwidth);
		}
		if (m_param->IsErrWidthDiff) {
			double widdiff = fabs(dis1 - dis2);
			if (widdiff>m_param->errWidthDiff) {
				errwidthdiffok = false;
				m_testItemsStatus[5] = 0;
				allok = false;
			}
			setDetectOutData(ErrWidthDiffOk, errwidthdiffok);
			setDetectOutData(ErrWidthDiff, widdiff);
		}
	}
	if (m_param->IsErrHeight || m_param->IsErrHeightDiff)
	{
		bool errheightok = true;
		bool errheightdiffok = true;
		float maxheight = 0;
		float minheight = 10000;
		float dis1, dis2;
		dis1 = sqrt((finalEndRow[0][0].D() - finalEndRow[1][0].D())*(finalEndRow[0][0].D() - finalEndRow[1][0].D()) +
			(finalEndCol[0][0].D() - finalEndCol[1][0].D())*(finalEndCol[0][0].D() - finalEndCol[1][0].D()));
		dis1 *= mk;
		if (maxheight<dis1)
			maxheight = dis1;
		if (minheight>dis1)
			minheight = dis1;
		dis2 = sqrt((finalEndRow[2][0].D() - finalEndRow[3][0].D())*(finalEndRow[2][0].D() - finalEndRow[3][0].D()) +
			(finalEndCol[2][0].D() - finalEndCol[3][0].D())*(finalEndCol[2][0].D() - finalEndCol[3][0].D()));
		dis2 *= mk;
		if (maxheight<dis2)
			maxheight = dis2;
		if (minheight>dis2)
			minheight = dis2;
		if (m_param->IsErrHeight) {
			if (maxheight>m_param->errHeightmax || minheight<m_param->errHeightMin) {
				errheightok = false;
				m_testItemsStatus[2] = 0;
				allok = false;
			}
			setDetectOutData(ErrHeightOk, errheightok);
			setDetectOutData(ErrHeightMax, maxheight);
			setDetectOutData(ErrHeightMin, minheight);
		}
		if (m_param->IsErrHeightDiff) {
			double heidiff = fabs(dis1 - dis2);
			if (heidiff>m_param->errHeightDiff) {
				errheightdiffok = false;
				m_testItemsStatus[6] = 0;
				allok = false;
			}
			setDetectOutData(ErrHeightDiffOk, errheightdiffok);
			setDetectOutData(ErrHeightDiff, heidiff);
		}
	}
	if (m_param->IsErrCenterOffset)
	{
		float Centershift = 0;
		bool centerok = true;
		Centershift = sqrt(pow((centerRoww[0].D() - m_param->TmpCenterRow), 2) + pow((centerColl[0].D() - m_param->TmpCenterCol), 2));
		Centershift *= mk;
		if (Centershift>m_param->errCenterOffset) {
			allok = false;
			m_testItemsStatus[4] = 0;
			centerok = false;
		}
		setDetectOutData(ErrCenterShift, Centershift);
		setDetectOutData(ErrCenterShiftOk, centerok);
	}
	*p_allok = allok;
	return allok ? 0 : -1;
	//setDetectOutData(AllOK, allok);
	//return allok;
}

void edgeLocationMod::viewResult(ImageView *iv, const QString &funName, int)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	HTuple imgok;
	getDispOutData(DispPointsImgOk, imgok);
	iv->setFilled(true);
	iv->setColor(255, 0, 0);
	if (imgok[0].I() == 1) {
		iv->setColor(0, 255, 0);
		Hlong num;
		Hobject obj = *getDispObject(DispPointsImg);
		count_obj(obj, &num);
		if (num > 0 && m_param->IsShowRegionAfterDet == 2) {
			if (num > 1)
				union1(obj, &obj);
			RlRegion reg;
			HalOpenCV::hal2cvRegion(obj, &reg);
			iv->dispRegion(&reg);
		}
	}
	HTuple findok;
	for (int i = 0;i < 4;i++) {
		getDispOutData(FindOk, i, findok);
		if (findok[0].I() == 0) {
			iv->setColor(255, 0, 0);
			iv->setFilled(false);
			HTuple homat;
			if (m_param->IsRefer) {
				getDispOutData(TmpHomMat, homat);
			}
			int beforeNum = 1;
			for (int z = 0;z < i;z++) {
				beforeNum += m_param->roiNum[z];
			}
			for (int j = 0;j < m_param->roiNum[i];j++) {
				Hobject roirec;
				gen_rectangle1(&roirec, m_param->RoiRegionVector[beforeNum + j].row1, m_param->RoiRegionVector[beforeNum + j].col1,
					m_param->RoiRegionVector[beforeNum + j].row2, m_param->RoiRegionVector[beforeNum + j].col2);
				if (m_param->IsRefer) {
					affine_trans_region(roirec, &roirec, homat, "false");
				}
				RlRegion dispReg;
				HalOpenCV::hal2cvRegion(roirec, &dispReg);
				iv->dispRegion(&dispReg);
			}
			return;
		}
	}
	HTuple maxangleok;
	getDispOutData(MaxAngleOK, maxangleok);
	if (maxangleok[0].I() == 0) {
		return;
	}
	if (m_param->IsShowRegionAfterDet) {
		for (int i = 0;i < 4;i++) {
			HTuple rowbegin, colbegin, rowend, colend;
			getDispOutData(BeginRow, i, rowbegin);
			getDispOutData(BeginCol, i, colbegin);
			getDispOutData(EndRow, i, rowend);
			getDispOutData(EndCol, i, colend);
			iv->setColor(0, 0, 255);
			if (m_param->IsShowRegionAfterDet >= 1) {
				iv->dispLine(rowbegin[0].D(), colbegin[0].D(), rowend[0].D(), colend[0].D());
			}
			//iv->dispLine(rowbegin[0].D(),colbegin[0].D()-3,rowbegin[0].D(),colbegin[0].D()+3);
			//iv->dispLine(rowbegin[0].D()-3,colbegin[0].D(),rowbegin[0].D()+3,colbegin[0].D());
			//iv->dispLine(rowend[0].D(),colend[0].D()-3,rowend[0].D(),colend[0].D()+3);
			//iv->dispLine(rowend[0].D()-3,colend[0].D(),rowend[0].D()+3,colend[0].D());
		}
	}

	HTuple allok;
	allok = **p_allok;
	//getDispOutData(AllOK, allok);
	if (allok[0].I() == 0) {
		iv->setColor(255, 0, 0);
	}
	else if (allok[0].I() == 1) {
		iv->setColor(0, 255, 0);
	}
	HTuple polyRow, polyCol;
	for (auto i = 0;i < p_corners->size();++i)
	{
		polyRow[i] = p_corners->at(i).y;
		polyCol[i] = p_corners->at(i).x;
	}
	//getDispOutData(FinalRow, polyRow);
	//getDispOutData(FinalCol, polyCol);
	polyRow[4] = polyRow[0];
	polyCol[4] = polyCol[0];
	Hobject polyRec;
	gen_contour_polygon_xld(&polyRec, polyRow, polyCol);
	gen_region_contour_xld(polyRec, &polyRec, "filled");
	iv->setFilled(false);
	if (m_param->IsShowRegionAfterDet) {
		RlRegion dispReg;
		HalOpenCV::hal2cvRegion(polyRec, &dispReg);
		iv->dispRegion(&dispReg);
	}
	HTuple centerRow, centerCol;
	getDispOutData(CenterRow, centerRow);
	getDispOutData(CenterCol, centerCol);
	iv->dispLine(centerRow[0].D(), centerCol[0].D() - 3, centerRow[0].D(), centerCol[0].D() + 3);
	iv->dispLine(centerRow[0].D() - 3, centerCol[0].D(), centerRow[0].D() + 3, centerCol[0].D());
}

void edgeLocationMod::textResult(ResultText *text, const QString &funName)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	HTuple found;
	char charbuf[4][32] = { "Top","Right","Bottom","Left" };
	for (int i = 0;i < 4;i++) {
		getDispOutData(FindOk, i, found);
		if (found[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
			text->append(QString::fromLocal8Bit(charbuf[i]));
			text->insertPlainText(QObject::tr(" edge found failed"));
			return;
		}
	}
	HTuple Imgok;
	getDispOutData(DispPointsImgOk, Imgok);
	if (Imgok[0].I() == 0) {
		text->setTextColor(QColor(255, 0, 0));
		text->append(QObject::tr("Acquire Points Image failed"));
		return;
	}
	HTuple maxangleok;
	getDispOutData(MaxAngleOK, maxangleok);
	if (maxangleok[0].I() == 0) {
		text->setTextColor(QColor(255, 0, 0));
		text->append(QObject::tr("max angle failed:"));
		HTuple maxangle;
		getDispOutData(MaxAngle, maxangle);
		text->insertPlainText(QObject::tr(" max angle = %1(%2)").arg(QString::number(maxangle[0].D(), 'f', 3)).
			arg(QString::number(m_param->maxAngle)));
		return;
	}
	if (m_param->IsErrWidth) {
		HTuple errwidthok, errwidthmin, errwidthmax;
		getDispOutData(ErrWidthOk, errwidthok);
		getDispOutData(ErrWidthMin, errwidthmin);
		getDispOutData(ErrWidthMax, errwidthmax);
		if (errwidthok[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
		}
		text->append(QObject::tr("Body Width = (%1,%2) of (%3,%4)")
			.arg(QString::number(errwidthmin[0].D(), 'f', 3))
			.arg(QString::number(errwidthmax[0].D(), 'f', 3))
			.arg(QString::number(m_param->errWidthMin, 'f', 3))
			.arg(QString::number(m_param->errWidthMax, 'f', 3)));
	}
	if (m_param->IsErrWidthDiff) {
		HTuple errwidthdiffok, errwidthdiff;
		getDispOutData(ErrWidthDiffOk, errwidthdiffok);
		getDispOutData(ErrWidthDiff, errwidthdiff);
		if (errwidthdiffok[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
		}
		text->append(QObject::tr("Body Width difference= %1 (%2)")
			.arg(QString::number(errwidthdiff[0].D(), 'f', 3))
			.arg(QString::number(m_param->errWidthDiff, 'f', 3)));
	}
	if (m_param->IsErrHeight) {
		HTuple errheightok, errheightmax, errheightmin;
		getDispOutData(ErrHeightOk, errheightok);
		getDispOutData(ErrHeightMax, errheightmax);
		getDispOutData(ErrHeightMin, errheightmin);
		if (errheightok[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
		}
		text->append(QObject::tr("Body Height= (%1,%2) of (%3,%4)")
			.arg(QString::number(errheightmin[0].D(), 'f', 3))
			.arg(QString::number(errheightmax[0].D(), 'f', 3))
			.arg(QString::number(m_param->errHeightMin, 'f', 3))
			.arg(QString::number(m_param->errHeightmax, 'f', 3)));
	}
	if (m_param->IsErrHeightDiff) {
		HTuple errheightdiffok, errheightdiff;
		getDispOutData(ErrHeightDiffOk, errheightdiffok);
		getDispOutData(ErrHeightDiff, errheightdiff);
		if (errheightdiffok[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
		}
		text->append(QObject::tr("Body Height difference= %1 (%2)")
			.arg(QString::number(errheightdiff[0].D(), 'f', 3))
			.arg(QString::number(m_param->errHeightDiff, 'f', 3)));
	}
	if (m_param->IsErrAngle) {
		HTuple errangleok, errangle;
		getDispOutData(ErrAngleOk, errangleok);
		getDispOutData(ErrAngle, errangle);
		if (errangleok[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
		}
		text->append(QObject::tr("Body Angle= %1(%2)").arg(QString::number(errangle[0].D(), 'f', 3)).
			arg(QString::number(m_param->errAngle)));
	}
	if (m_param->IsErrCenterOffset) {
		HTuple errcenterok, errcenter;
		getDispOutData(ErrCenterShiftOk, errcenterok);
		getDispOutData(ErrCenterShift, errcenter);
		if (errcenterok[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
		}
		text->append(QObject::tr("Body Shift= %1(%2)")
			.arg(QString::number(errcenter[0].D(), 'f', 3))
			.arg(QString::number(m_param->errCenterOffset, 'f', 3)));
	}
	text->setTextColor(QColor(0, 0, 0));
	if (m_param->IsCsp) {
		text->setTextColor(Qt::black);
		text->append(QObject::tr(" image center offset: x=%1, y=%2")
			.arg(p_offCenter->x, 0, 'f', 4).arg(p_offCenter->y, 0, 'f', 4));
		//HTuple centeroff;
		//getDispOutData(OFFCenter, centeroff);
		//text->setTextColor(Qt::black);
		//text->append(QObject::tr(" image center offset: "));
		//text->insertPlainText(QString(" x= %1; y=%2 ").arg(QString::number(centeroff[0].D(), 'f', 4)).arg(QString::number(centeroff[1].D(), 'f', 4)));
	}
}

void edgeLocationMod::iniData()
{
	m_param = new edgeLocationParam;
	for (int i = 0;i < 4;i++)
		m_edgeLine[i] = new fitEdgeLine();
	m_image = new Hobject;
	createPins();
}

void edgeLocationMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2d, "hm2d");
	addPin(&p_pixSize, "psz");

	addPin(&p_allok, "allok");
	addPin(&p_corners, "corners");
	addPin(&p_offCenter, "offCen");

	p_pixSize.setVisible(false);
	p_allok.setVisible(false);
	p_offCenter.setVisible(false);
}

void edgeLocationMod::iniDetector()
{
	int roiCount = 1;
	for (int i = 0;i < 4;i++) {
		// up right dw left
		m_edgeLine[i]->setPolarAndOrient((fitEdgeLine::Polarity)m_param->whiteBlackDir[i], m_dir[m_param->searchDir[i]][i]);
		m_edgeLine[i]->setThreshold(m_param->minGrayDiff[i]);
		m_edgeLine[i]->setSearchParam(m_param->searchLineGap[i], m_param->extendSearchLine[i]);
		m_edgeLine[i]->setFitFactor(m_param->clipfactor[i]);
		m_edgeLine[i]->setCrossGrayValid(m_param->CrossGrayValid[i], m_param->crossGray[i], m_param->crossStep[i]);
		m_edgeLine[i]->setEdgeMethod(0, 0);
		int drawRow1[20], drawRow2[20], drawCol1[20], drawCol2[20];  //4*5
		for (int j = 0;j < m_param->roiNum[i];j++) {
			drawRow1[j] = m_param->RoiRegionVector[j + roiCount].row1;
			drawRow2[j] = m_param->RoiRegionVector[j + roiCount].row2;
			drawCol1[j] = m_param->RoiRegionVector[j + roiCount].col1;
			drawCol2[j] = m_param->RoiRegionVector[j + roiCount].col2;
		}
		m_edgeLine[i]->setRoi(m_param->roiNum[i], drawRow1, drawCol1, drawRow2, drawCol2);
		roiCount += m_param->roiNum[i];
	}
}