#include "standEdgeMod.h"
#include "standEdgeParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"

#define AllOK "allok"
#define FindOk "findok"  //是否有区域内未检1测到内容，如果有则什么都不显示直接NG  0：成功，1：边界拟合失败，2：边界点过少
#define RegionOk "regionOk" //上下左右区域是1否OK
#define MaxAngleOK "maxAngleok"   //最大角度是1否OK，如果不OK直接NG
#define MaxAngle "maxAngle"       //最大角度的1值  
#define EndPointOk "endpointOK"   //端1点检测是否成功
#define EndNGIndex "endpointindex"   //端点失败的区域
#define BeginRow "BeginEdgeRow"   //检测端点时候的端点坐1标
#define BeginCol "BeginEdgeCol"
#define EndRow "EndEdgeRow"
#define EndCol "EndEdgeCol"

#define SearchBeginRow "searchBeginRow"
#define SearchBeginCol "searchBeginCol"
#define SearchEndRow "searchEndRow"
#define SearchEndCol "searchEndCol"

#define DispPointsImgOk "dispImgOk"  //是否有轮廓检测到
#define DispPointsImg "displayPointsImg"
#define ErrWidthOk "WidthOK"
#define ErrWidthMin "WidthMin"
#define ErrWidthMax "WidthMax"
#define ErrHeightOk "HeightOk"
#define ErrHeightMin "HeightMin"
#define ErrHeightMax "HeightMax"
#define ErrAngleOk "angleOK"
#define ErrAngle "angle" //最大偏转角1度
#define ErrCenterShiftOk "ErrcentershiftOK"
#define ErrCenterShift "Errcentershift" //与模板中心相对应的中心偏移1值

#define ImageFull "Imagefull"  //图片是否ok

standEdgeMod::standEdgeMod()
{
	iniData();
}

standEdgeMod::standEdgeMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

standEdgeMod::~standEdgeMod()
{
	delete m_param;
	delete m_image;
}

int standEdgeMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int standEdgeMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void standEdgeMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void standEdgeMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int standEdgeMod::run(const QString &funName)
{
	p_corners->clear();
	p_edgeRowBeg->clear();
	p_edgeRowEnd->clear();
	p_edgeColBeg->clear();
	p_edgeColEnd->clear();
	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
		m_testItemsStatus[i] = 1;
	}
	//预处理最好对m_image进行一下腐蚀处理
	bool resultOutOK = true;
	bool allok = true;
	if (p_im->empty()) {
		allok = false;
		setDetectOutData(AllOK, allok);
		setDetectOutData(ImageFull, false);
		m_testItemsStatus[0] = 0;
		//return allok;
		return -1;
	}
	else {
		setDetectOutData(ImageFull, true);
	}
	HalOpenCV::cv2halImg(*p_im, m_image, false);
	bool roiFound[4]; //各边界是否找1到
	for (int i = 0; i < 4;i++) {
		roiFound[i] = true;
	}
	//up
	int roirow1[4], roicol1[4], roirow2[4], roicol2[4], searchDir[4];

	for (int i = 0;i < 4;i++) {
		roirow1[i] = m_param->RoiRegionVector[i + 1].row1;
		roirow2[i] = m_param->RoiRegionVector[i + 1].row2;
		roicol1[i] = m_param->RoiRegionVector[i + 1].col1;
		roicol2[i] = m_param->RoiRegionVector[i + 1].col2;
		searchDir[i] = m_param->searchDir[i];
	}

	Hlong measureHandle;
	HTuple Imgwidth, ImgHeight;
	get_image_size(*m_image, &Imgwidth, &ImgHeight);
	Hobject edgeObj[4];
	HTuple RowBegin[4], ColBegin[4], RowEnd[4], ColEnd[4];  //上下左右的顺序
	HTuple RowBegin11[4], ColBegin11[4], RowEnd11[4], ColEnd11[4];  //用于储存没有偏移之前的边界点
	HTuple m_rowEdge[4], m_colEdge[4];  //四条边的扫描到的点数


	for (int i = 0;i < 4;i++)
	{
		HTuple rowEdge, colEdge, amp, distance;
		char edge[32];
		if (m_param->whiteBlackDir[i]) //1白黑
			sprintf(edge, "negative");
		else
			sprintf(edge, "positive");
		if (searchDir[i] == 1) {  //上下
			gen_measure_rectangle2((roirow1[i] + roirow2[i]) / 2, roicol1[i]
				, -PI / 2.0, (roirow2[i] - roirow1[i]) / 2, 2/*m_param->extendSearchLine[i]*2*/, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measureHandle);
		}
		else if (searchDir[i] == 0) { //左右
			gen_measure_rectangle2(roirow1[i], (roicol1[i] + roicol2[i]) / 2
				, 0, (roicol2[i] - roicol1[i]) / 2, 2/*m_param->extendSearchLine[i]*2*/, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measureHandle);
		}
		int pointN = 0;   //累计进行测试的点1数
		int usefulPoint = 0;  // 累1计有效得出结果的点数(结1果不为空)

		Hobject mdetectImg;  //处理后用来检测边界的图片

		if (m_param->isLeadSize[i] && m_param->leadSize[i] > 0) {
			Hobject opolyImg;
			Hobject orecregion;
			gen_rectangle1(&orecregion, roirow1[i], roicol1[i], roirow2[i], roicol2[i]);
			reduce_domain(*m_image, orecregion, &opolyImg);
			invert_image(opolyImg, &opolyImg);
			threshold(opolyImg, &opolyImg, m_param->grayVal[i], 255);
			connection(opolyImg, &opolyImg);
			Hlong polynum;
			count_obj(opolyImg, &polynum);
			Hlong areaval;double rowval, colval;
			area_center(opolyImg, &areaval, &rowval, &colval);
			int owid, ohei;
			if (i == 0 || i == 1) {
				owid = m_param->leadSize[i];
				ohei = 10;
			}
			else {
				ohei = m_param->leadSize[i];
				owid = 10;
			}
			Hobject openobj, unionobj;
			opening_rectangle1(opolyImg, &openobj, owid, ohei);
			//connection(openobj,&openobj);
			union1(openobj, &openobj);
			union1(opolyImg, &unionobj);
			difference(unionobj, openobj, &openobj);
			dilation_circle(openobj, &openobj, 1);
			connection(openobj, &openobj);
			union1(openobj, &openobj);
			count_obj(openobj, &polynum);
			bool isempty = true;
			if (polynum > 0)
			{
				area_center(openobj, &areaval, &rowval, &colval);
				if (areaval > 10)
				{
					isempty = false;
				}
			}
			if (isempty)
			{
				gen_empty_region(&mdetectImg);
			}
			else {
				copy_obj(openobj, &mdetectImg, 1, 1);
			}
			//paint_region(openobj,*m_image,&mdetectImg,255,"fill");
		}
		//else{
		//	copy_image(*m_image,&mdetectImg);
		//}
		if (searchDir[i] == 1) {  //上下
			int edPoint = 0;
			if (m_param->IsCutLine[i]) {
				if ((roicol1[i] + 130 + m_param->extendSearchLine[i]) < Imgwidth[0].I())
					edPoint = roicol1[i] + 130;
				else
					edPoint = Imgwidth[0].I() - m_param->extendSearchLine[i];
			}
			else
				edPoint = roicol2[i];
			for (int startPoint = roicol1[i];startPoint < edPoint;startPoint += m_param->searchLineGap[i]) {
				//if(m_param->IsPaichuLine[i]){
				//	if((startPoint<((roicol1[i]+edPoint)/2+m_param->paichuLine[i]/2))&&(startPoint>((roicol1[i]+edPoint)/2-m_param->paichuLine[i]/2)))
				//		continue;
				//}
				if (pointN != 0)
					translate_measure(measureHandle, (roirow1[i] + roirow2[i]) / 2, startPoint);
				measure_pos(*m_image, measureHandle, 1, m_param->minGrayDiff[i], edge, "first", &rowEdge, &colEdge, &amp, &distance);
				Hlong aa = rowEdge.Num();
				if (aa > 0) {
					if (m_param->isLeadSize[i] && m_param->leadSize[i] > 0) {
						if (IsPointInRegion(&mdetectImg, rowEdge[0].D(), colEdge[0].D(), 2)) {
							continue;
						}
					}
					m_rowEdge[i][usefulPoint] = rowEdge[0];
					m_colEdge[i][usefulPoint] = colEdge[0];
					usefulPoint++;
				}
				pointN++;
			}
		}
		else if (searchDir[i] == 0) {   //左右
			int edPoint = 0;
			if (m_param->IsCutLine[i]) {
				if ((roirow1[i] + 130 + m_param->extendSearchLine[i]) < ImgHeight[0].I())
					edPoint = roirow1[i] + 130;
				else
					edPoint = ImgHeight[0].I() - m_param->extendSearchLine[i];
			}
			else
				edPoint = roirow2[i];
			for (int startPoint = roirow1[i];startPoint < edPoint;startPoint += m_param->searchLineGap[i]) {
				if (pointN != 0)
					translate_measure(measureHandle, startPoint, (roicol1[i] + roicol2[i]) / 2);
				measure_pos(*m_image, measureHandle, 1, m_param->minGrayDiff[i], edge, "first", &rowEdge, &colEdge, &amp, &distance);
				Hlong aa = rowEdge.Num();
				if (aa > 0) {
					if (m_param->isLeadSize[i] && m_param->leadSize[i] > 0) {
						if (IsPointInRegion(&mdetectImg, rowEdge[0].D(), colEdge[0].D(), 2)) {
							continue;
						}
					}
					m_rowEdge[i][usefulPoint] = rowEdge[0];
					m_colEdge[i][usefulPoint] = colEdge[0];
					usefulPoint++;
				}
				pointN++;
			}
		} //采集边界点结1束
		Hlong ab = m_rowEdge[i].Num();
		QList<CvPoint2D32f > listP;
		for (int p = 0;p < ab;p++)
		{
			CvPoint2D32f np;
			np.y = m_rowEdge[i][p].D();
			np.x = m_colEdge[i][p].D();
			listP.push_back(np);
		}

		HTuple nc, nr, dist;
		if (ab >= 5) {
			//进行检测到的边界点的纠正
			if (m_param->IsClearOutlier[i]) {  //清除连续性不超过三个点的离群点
				HTuple tmpoRow = m_rowEdge[i], tmpoCol = m_colEdge[i];
				int newab = ab;
				int conum;
				for (int n = 0;n < newab;n++) {
					double dispp;
					if (i == 0 || i == 1) {//上下
						dispp = fabs(tmpoRow[n].D() - tmpoRow[(n - 1) >= 0 ? (n - 1) : 0].D());
					}
					else {
						dispp = fabs(tmpoCol[n].D() - tmpoCol[(n - 1) >= 0 ? (n - 1) : 0].D());
					}
					//distance_pp(tmpoRow[n].D(),tmpoCol[n].D(),tmpoRow[(n-1)>=0?(n-1):0].D(),tmpoCol[(n-1)>=0?(n-1):0].D(),&dispp);
					if (fabs(dispp) >= 3) {  //点距离大于3
						bool needOut = false;
						for (int g = 0;g < 3/*连续点的数目*/;g++) {
							if ((n + g + 1 < newab)) {
								//distance_pp(tmpoRow[n+g+1].D(),tmpoCol[n+g+1].D(),tmpoRow[n+g].D(),tmpoCol[n+g].D(),&dispp);
								if (i == 0 || i == 1) {//上下
									dispp = fabs(tmpoRow[n + g + 1].D() - tmpoRow[n + g].D());
								}
								else {
									dispp = fabs(tmpoCol[n + g + 1].D() - tmpoCol[n + g].D());
								}
								if (fabs(dispp) >= 3) {
									needOut = true;
								}
							}
							else {
								needOut = true;
							}
							if (needOut) {
								HTuple reIndex;
								for (int z = 0;z < g + 1;z++) {
									reIndex[z] = n + z;
								}
								tuple_remove(tmpoRow, reIndex, &tmpoRow);
								tuple_remove(tmpoCol, reIndex, &tmpoCol);
								conum = tmpoRow.Num();
								newab -= g + 1;
								needOut = false;
								break;
							}
						}
					}
				}
				if (newab != ab) {
					m_rowEdge[i].Reset();
					m_colEdge[i].Reset();
					m_rowEdge[i] = tmpoRow;
					m_colEdge[i] = tmpoCol;
					ab = newab;
				}
			}
			if (m_param->IsPaichuLine[i]) {
				if (m_param->scalePaichu[i] > 0) {
					int endindex = ab*(1.0 - m_param->scalePaichu[i]) - 1;
					tuple_select_range(m_rowEdge[i], 0, endindex, &m_rowEdge[i]);
					tuple_select_range(m_colEdge[i], 0, endindex, &m_colEdge[i]);
					usefulPoint = endindex + 1;
				}
				HTuple sortPosition, sortIndex;
				if (searchDir[i] == 0) {  //左右
					tuple_sort(m_colEdge[i], &sortPosition);
					tuple_sort_index(m_colEdge[i], &sortIndex);
				}
				else if (searchDir[i] == 1) { //上下
					tuple_sort(m_rowEdge[i], &sortPosition);
					tuple_sort_index(m_rowEdge[i], &sortIndex);
				}
				int lowlfNum = 0;  //跳变前左边持续的点数
				int lowrtNum = 0;
				int jumpIndex = -1; //跳变点指数
				for (int n = 0;n < sortPosition.Num() - 1;n++) {
					lowlfNum = 0;
					lowrtNum = 0;
					if (abs(sortPosition[n + 1].D() - sortPosition[n].D()) > m_param->paichuLine[i]) {
						if (n <= 6 || n >= (sortPosition.Num() - 7)) {   //保证前后各有7个点存在
							continue;
						}
						for (int g = 0;g < 6;g++) {
							if (abs(sortPosition[n - g].D() - sortPosition[n - g - 1].D()) < 3) {  //跳变不超过3个像素即视为连续点
								lowlfNum++;
							}
							if (abs(sortPosition[n + 2 + g].D() - sortPosition[n + 1 + g].D()) < 3) {  //跳变不超过3个像素即视为连续点
								lowrtNum++;
							}
						}
						if (lowlfNum >= 4 && lowrtNum >= 4) {//保证前后各有五个连续的连续点
							jumpIndex = n;
						}
					}
				}
				if (jumpIndex > 0) {  //i=0123,上下左右
					HTuple newRow, newCol;
					if (searchDir[i] == 0 && i == 2) {  //左边界
						for (int n = 0;n < sortPosition.Num() - jumpIndex - 1;n++) {
							newCol[n] = sortPosition[jumpIndex + n + 1].D();
							newRow[n] = m_rowEdge[i][sortIndex[jumpIndex + n + 1].I()].D();
						}
					}
					else if (searchDir[i] == 0 && i == 3) {//右边界
						for (int n = 0;n < jumpIndex + 1;n++) {
							newCol[n] = sortPosition[n].D();
							newRow[n] = m_rowEdge[i][sortIndex[n].I()].D();
						}
					}
					else if (searchDir[i] == 1 && i == 0) {//上边界
						for (int n = 0;n < sortPosition.Num() - jumpIndex - 1;n++) {
							newRow[n] = sortPosition[jumpIndex + n + 1].D();
							newCol[n] = m_colEdge[i][sortIndex[jumpIndex + n + 1].I()].D();
						}
					}
					else if (searchDir[i] == 1 && i == 1) { //下边界
						for (int n = 0;n < jumpIndex + 1;n++) {
							newRow[n] = sortPosition[n].D();
							newCol[n] = m_colEdge[i][sortIndex[n].I()].D();
						}
					}
					if (newRow.Num() > 5) {   //数目超过5就重新赋值边界点变量
						m_rowEdge[i] = newRow;
						m_colEdge[i] = newCol;
					}
				}

			}

			QList<QList<CvPoint2D32f>> nlist;
			if (m_param->IsPaichuLine[i])
			{
				int threVal;
				threVal = (double(listP.size()) / 4.0);
				int nIndex = 0;
				if (i == 0 || i == 2)
				{
					nIndex = 1;
				}
				else {
					nIndex = 0;
				}
				if (sortPointsByHisto(listP, nlist, 2, threVal, 15, true, 3))
				{
					m_rowEdge[i].Reset();
					m_colEdge[i].Reset();
					for (int n = 0;n < nlist[nIndex].size();n++)
					{
						m_rowEdge[i][n] = nlist[nIndex][n].y;
						m_colEdge[i][n] = nlist[nIndex][n].x;
					}
					ab = nlist[nIndex].size();
				}
			}

			gen_contour_polygon_xld(&edgeObj[i], m_rowEdge[i], m_colEdge[i]);
			double factorval = m_param->clipfactor[i]; //下面使用的筛选因子
			if (int(factorval*10.0) % 5 != 0) {
				factorval = double((int)(factorval + 0.5));
			}
			fit_line_contour_xld(edgeObj[i], "tukey"/*"regression"*/, -1, 0, 5, factorval/*1.0*/, &RowBegin[i], &ColBegin[i], &RowEnd[i],  //1.0, 1.5, 2.0, 2.5, 3.0
				&ColEnd[i], &nr, &nc, &dist);
			Hlong contor = RowBegin[i].Num();
			if (contor == 0) {
				roiFound[i] = false;
				allok = false;
				m_testItemsStatus[0] = 0;
				setDetectOutData(FindOk, 1, i);  //边界拟合失败
			}
			else {
				HTuple inrow[2], incol[2], isparel; //求出来的边线与所给的搜索区域的交点
				if (i == 0 || i == 1) {    //上下
					intersection_ll(RowBegin[i][0], ColBegin[i][0], RowEnd[i][0], ColEnd[i][0], roirow1[i], roicol1[i], roirow2[i], roicol1[i],
						&inrow[0], &incol[0], &isparel);
					intersection_ll(RowBegin[i][0], ColBegin[i][0], RowEnd[i][0], ColEnd[i][0], roirow1[i], roicol2[i], roirow2[i], roicol2[i],
						&inrow[1], &incol[1], &isparel);

				}
				else if (i == 2 || i == 3) {
					intersection_ll(RowBegin[i][0], ColBegin[i][0], RowEnd[i][0], ColEnd[i][0], roirow1[i], roicol1[i], roirow1[i], roicol2[i],
						&inrow[0], &incol[0], &isparel);
					intersection_ll(RowBegin[i][0], ColBegin[i][0], RowEnd[i][0], ColEnd[i][0], roirow2[i], roicol1[i], roirow2[i], roicol2[i],
						&inrow[1], &incol[1], &isparel);
				}
				RowBegin[i][0] = inrow[0][0].D();
				ColBegin[i][0] = incol[0][0].D();
				RowEnd[i][0] = inrow[1][0].D();
				ColEnd[i][0] = incol[1][0].D();

				RowBegin11[i][0] = RowBegin[i][0];
				RowEnd11[i][0] = RowEnd[i][0];
				ColBegin11[i][0] = ColBegin[i][0];
				ColEnd11[i][0] = ColEnd[i][0];

				setDetectOutData(SearchBeginRow, RowBegin[i], i);   //得到的实际边线的起始点
				setDetectOutData(SearchBeginCol, ColBegin[i], i);
				setDetectOutData(SearchEndRow, RowEnd[i], i);
				setDetectOutData(SearchEndCol, ColEnd[i], i);
				if (i == 0 || i == 1) {
					RowBegin[i][0] = m_param->lineOffset[i] + RowBegin[i][0].D();
					RowEnd[i][0] = m_param->lineOffset[i] + RowEnd[i][0].D();
				}
				else if (i == 2 || i == 3) {
					ColBegin[i][0] = m_param->lineOffset[i] + ColBegin[i][0].D();
					ColEnd[i][0] = m_param->lineOffset[i] + ColEnd[i][0].D();
				}
				if (((RowEnd[i][0].D() < RowBegin[i][0].D()) && (i == 2 || i == 3)) || ((ColEnd[i][0].D() < ColBegin[i][0].D()) && (i == 1 || i == 0))) {   //调转开头点和结尾点，使全部符合图片上的正向顺1序
					double tmp;
					tmp = ColEnd[i][0].D();
					ColEnd[i][0] = ColBegin[i][0];
					ColBegin[i][0] = tmp;
					tmp = RowEnd[i][0].D();
					RowEnd[i][0] = RowBegin[i][0];
					RowBegin[i][0] = tmp;
				}
				setDetectOutData(FindOk, 0, i); //边界成功 0
			}
		}
		else if (ab < 5) {
			roiFound[i] = false;
			allok = false;
			m_testItemsStatus[0] = 0;
			setDetectOutData(FindOk, 2, i); //边界点过少 2
		}
		close_measure(measureHandle);
	} //for loop end
	HTuple rows, cols;
	long totalNum = 0;
	for (int i = 0;i < 4;i++) {
		if (roiFound[i]) {
			int num = m_rowEdge[i].Num();
			for (int j = 0;j < num;j++) {
				rows[totalNum] = m_rowEdge[i][j];
				cols[totalNum] = m_colEdge[i][j];
				totalNum++;
			}
		}

	}

	if ((rows.Num() == cols.Num()) && rows.Num() > 10) {   //得到的轮廓数目不能太少
		Hobject dispPointsImg;
		gen_region_points(&dispPointsImg, rows, cols);
		union1(dispPointsImg, &dispPointsImg);
		setDetectOutData(DispPointsImgOk, true);
		setDetectOutObject(DispPointsImg, dispPointsImg);
	}
	else {
		allok = false;
		m_testItemsStatus[0] = 0;
		setDetectOutData(DispPointsImgOk, false);
	}
	float EveryAngle[4];
	for (int i = 0;i < 4;i++) {
		if (!roiFound[i]) {
			continue;
		}
		if (i > 1) {
			EveryAngle[i] = PI / 2 + atan((ColEnd[i][0].D() - ColBegin[i][0].D()) / (RowEnd[i][0].D() - RowBegin[i][0].D()));
		}
		else {
			EveryAngle[i] = -atan((RowEnd[i][0].D() - RowBegin[i][0].D()) / (ColEnd[i][0].D() - ColBegin[i][0].D()));
		}
		if (((EveryAngle[i] * 180.0 / PI < 90 + m_param->maxAngle) && (EveryAngle[i] * 180.0 / PI > 90 - m_param->maxAngle)) ||
			((EveryAngle[i] * 180.0 / PI < 0 + m_param->maxAngle) && (EveryAngle[i] * 180.0 / PI > 0 - m_param->maxAngle))) {
			//m_angleOK=true;
		}
		else {
			allok = false;
			setDetectOutData(MaxAngleOK, false);
			setDetectOutData(MaxAngle, EveryAngle[i] / M_PI*180.0);
			setDetectOutData(AllOK, allok);
			m_testItemsStatus[3] = 0;
			//return allok;
			return -1;
		}
	}
	setDetectOutData(MaxAngleOK, true);
	bool endpointFound = true;
	for (int i = 0;i < 4;i++) {
		if (!roiFound[i]) {
			allok = false;
			setDetectOutData(AllOK, allok);
			m_testItemsStatus[0] = 0;
			//return allok;
			return -1;
		}
	}
	if (m_param->IsEndPoint) {
		for (int i = 0;i < 4;i++) {
			Hlong measureHandleEndPoint;
			HTuple rowEdge, colEdge, amp, distance;
			double anglehere;
			double edgelen;
			distance_pp(RowBegin[i][0].D(), ColBegin[i][0].D(), RowEnd[i][0].D(), ColEnd[i][0].D(), &edgelen);
			if (i == 0 || i == 1) {
				anglehere = EveryAngle[i] + PI;
			}
			else if (i == 2 || i == 3) {
				anglehere = EveryAngle[i];
			}
			gen_measure_rectangle2(RowBegin[i][0], ColBegin[i][0]
				, anglehere, edgelen / 2.0, 3, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measureHandleEndPoint);
			measure_pos(*m_image, measureHandleEndPoint, 1, m_param->edgeGray[i], "positive", "first", &rowEdge, &colEdge, &amp, &distance);
			close_measure(measureHandleEndPoint);
			Hlong length = rowEdge.Num();
			if (length == 0) {
				endpointFound = false;
				allok = false;
				setDetectOutData(EndPointOk, 0);
				setDetectOutData(EndNGIndex, i);
				setDetectOutData(AllOK, allok);
				m_testItemsStatus[0] = 0;
				//return allok;
				return -1;
			}
			RowBegin[i][0] = rowEdge[0];
			ColBegin[i][0] = colEdge[0];
			if (i == 0 || i == 1) {
				anglehere = EveryAngle[i];
			}
			else if (i == 2 || i == 3) {
				anglehere = EveryAngle[i] + PI;
			}
			gen_measure_rectangle2(RowEnd[i][0], ColEnd[i][0]
				, anglehere, edgelen / 2.0, 5, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measureHandleEndPoint);
			measure_pos(*m_image, measureHandleEndPoint, 1, m_param->edgeGray[i], "positive", "first", &rowEdge, &colEdge, &amp, &distance);
			close_measure(measureHandleEndPoint);

			length = rowEdge.Num();
			if (length == 0) {
				endpointFound = false;
				allok = false;
				setDetectOutData(EndPointOk, endpointFound);
				setDetectOutData(EndNGIndex, i);
				setDetectOutData(AllOK, allok);
				m_testItemsStatus[0] = 0;
				//return allok;
				return -1;
			}
			RowEnd[i][0] = rowEdge[0];
			ColEnd[i][0] = colEdge[0];
		}
		setDetectOutData(EndPointOk, true);
	}
	else {       //不检测边界点时候也要输出各线条的起始终1止points(为了后面模块使1用)
		for (int i = 0;i < 4;i++) {
			setDetectOutData(BeginRow, RowBegin[i][0], i);
			setDetectOutData(BeginCol, ColBegin[i][0], i);
			setDetectOutData(EndRow, RowEnd[i][0], i);
			setDetectOutData(EndCol, ColEnd[i][0], i);
		}
	}
	double finalPointRow[4], finalPointCol[4];  //从1左上角顺时针开始遍历
	FloatPoint* center = new FloatPoint; //中1心的坐标
	if (m_param->IsEndPoint) {    //求需要求端点时内部的本体1框
		for (int i = 0;i < 4;i++) {
			int a = i;
			if (a == 0) {
				finalPointRow[0] = RowBegin[2][0].D();
				finalPointCol[0] = ColBegin[0][0].D();
				finalPointRow[0] += (double)m_param->offsetPlus[2];
				finalPointCol[0] += (double)m_param->offsetPlus[0];
			}
			else if (a == 1) {
				finalPointRow[1] = RowBegin[3][0].D();
				finalPointCol[1] = ColEnd[0][0].D();
				finalPointRow[1] += (double)m_param->offsetPlus[3];
				finalPointCol[1] += (double)m_param->offsetPlus[0];
			}
			else if (a == 2) {
				finalPointRow[2] = RowEnd[3][0].D();
				finalPointCol[2] = ColEnd[1][0].D();
				finalPointRow[2] += (double)m_param->offsetPlus[3];
				finalPointCol[2] += (double)m_param->offsetPlus[1];
			}
			else if (a == 3) {
				finalPointRow[3] = RowEnd[2][0].D();
				finalPointCol[3] = ColBegin[1][0].D();
				finalPointRow[3] += (double)m_param->offsetPlus[2];
				finalPointCol[3] += (double)m_param->offsetPlus[1];
			}
			setDetectOutData(BeginRow, RowBegin[i][0], i);
			setDetectOutData(BeginCol, ColBegin[i][0], i);

			setDetectOutData(EndRow, RowEnd[i][0], i);
			setDetectOutData(EndCol, ColEnd[i][0], i);
		}
		//setDetectOutObject("polyRec",polyRec);   ????设置展示的多边形
		//求矩1形中心
		Line line1, line2;
		line1.Start.row = finalPointRow[0];
		line1.Start.col = finalPointCol[0];
		line1.End.row = finalPointRow[2];
		line1.End.col = finalPointCol[2];
		line2.Start.row = finalPointRow[3];
		line2.Start.col = finalPointCol[3];
		line2.End.row = finalPointRow[1];
		line2.End.col = finalPointCol[1];
		HTuple llrow, llcol, llparallel;
		intersection_ll(line1.Start.row, line1.Start.col, line1.End.row, line1.End.col,
			line2.Start.row, line2.Start.col, line2.End.row, line2.End.col, &llrow, &llcol, &llparallel);
		center->row = llrow[0].D();
		center->col = llcol[0].D();
		//center=GetLineIntersection(line1,line2);
		//setDetectOutData(CenterRow,center->row);
		//setDetectOutData(CenterCol,center->col);
		setDetectOutData(CenterRow, llrow[0].D());
		setDetectOutData(CenterCol, llcol[0].D());
	}
	else if (!m_param->IsEndPoint) {
		Line edgeLine[4];
		for (int i = 0;i < 4;i++) {
			edgeLine[i].Start.row = RowBegin[i][0].D();
			edgeLine[i].Start.col = ColBegin[i][0].D();
			edgeLine[i].End.row = RowEnd[i][0].D();
			edgeLine[i].End.col = ColEnd[i][0].D();
		}
		for (int i = 0;i < 4;i++) {
			FloatPoint* tmpInter = new FloatPoint;
			int aaa, bbb;
			aaa = i > 1 ? 1 : 0;
			if (i == 1 || i == 2)
				bbb = 3;
			else
				bbb = 2;

			HTuple llrow, llcol, llparallel;
			intersection_ll(edgeLine[aaa].Start.row, edgeLine[aaa].Start.col, edgeLine[aaa].End.row, edgeLine[aaa].End.col,
				edgeLine[bbb].Start.row, edgeLine[bbb].Start.col, edgeLine[bbb].End.row, edgeLine[bbb].End.col, &llrow, &llcol, &llparallel);

			tmpInter->row = llrow[0].D();
			tmpInter->col = llcol[0].D();
			//tmpInter=GetLineIntersection(edgeLine[aaa],edgeLine[bbb]);
			finalPointRow[i] = tmpInter->row;
			finalPointCol[i] = tmpInter->col;
			delete tmpInter;
		}
		Line line1, line2;
		line1.Start.row = finalPointRow[0];
		line1.Start.col = finalPointCol[0];
		line1.End.row = finalPointRow[2];
		line1.End.col = finalPointCol[2];
		line2.Start.row = finalPointRow[3];
		line2.Start.col = finalPointCol[3];
		line2.End.row = finalPointRow[1];
		line2.End.col = finalPointCol[1];
		//center=GetLineIntersection(line1,line2);
		//setDetectOutData(CenterRow,center->row);
		//setDetectOutData(CenterCol,center->col);
		HTuple llrow, llcol, llparallel;
		intersection_ll(line1.Start.row, line1.Start.col, line1.End.row, line1.End.col,
			line2.Start.row, line2.Start.col, line2.End.row, line2.End.col, &llrow, &llcol, &llparallel);
		center->row = llrow[0].D();
		center->col = llcol[0].D();
		//center=GetLineIntersection(line1,line2);
		//setDetectOutData(CenterRow,center->row);
		//setDetectOutData(CenterCol,center->col);
		setDetectOutData(CenterRow, llrow[0].D());
		setDetectOutData(CenterCol, llcol[0].D());
	}
 	HTuple fnalRow, fnalCol;
// 	HTuple outRowBegin, outRowEnd, outColBegin, outColEnd;
	for (int i = 0;i < 4;i++) {
 		fnalRow[i] = finalPointRow[i];
 		fnalCol[i] = finalPointCol[i];
// 		outRowBegin[i] = RowBegin11[i][0].D();
// 		outRowEnd[i] = RowEnd11[i][0].D();
// 		outColBegin[i] = ColBegin11[i][0].D();
// 		outColEnd[i] = ColEnd11[i][0].D();

		p_corners->push_back(cv::Point2d(finalPointCol[i], finalPointRow[i]));
		p_edgeRowBeg->push_back(RowBegin11[i][0].D());
		p_edgeRowEnd->push_back(RowEnd11[i][0].D());
		p_edgeColBeg->push_back(ColBegin11[i][0].D());
		p_edgeColEnd->push_back(ColEnd11[i][0].D());
	}
	p_corners->push_back(cv::Point2d(finalPointCol[0], finalPointRow[0]));

	//fnalRow[4] = fnalRow[0].D();
	//fnalCol[4] = fnalCol[0].D();
	//setDetectOutData(FinalRow, fnalRow);  //左上角顺时针
	//setDetectOutData(FinalCol, fnalCol);

	//setDetectOutData(OutEdgeRowBegin, outRowBegin);  //上下左右
	//setDetectOutData(OutEdgeRowEnd, outRowEnd);
	//setDetectOutData(OutEdgeColBegin, outColBegin);
	//setDetectOutData(OutEdgeColEnd, outColEnd);

	//求站高部分,顺序上下左右
	float tmpAngle[4];
	if (m_param->IsErrAngle)
	{
		bool errAngleOk = true;
		float maxangle = 0;
		for (int i = 0;i < 4;i++) {
			if (i > 1) {
				tmpAngle[i] = PI / 2 + atan((fnalCol[2 * i - 3] - fnalCol[6 - 2 * i]) / (fnalRow[2 * i - 3] - fnalRow[6 - 2 * i]));//1,2   3,0
			}
			else {
				tmpAngle[i] = -atan((fnalRow[2 * i] - fnalRow[2 * i + 1]) / (fnalCol[2 * i] - fnalCol[2 * i + 1]));   //0,1     2,3
			}
		}
		for (int i = 0;i < 4;i++) {

			if ((tmpAngle[i] * 180.0 / PI < 44 + 90) && (tmpAngle[i] * 180.0 / PI > -44 + 90)) {
				tmpAngle[i] -= 90.0*PI / 180.0;
			}
			if (abs(tmpAngle[i]) > maxangle) {
				maxangle = abs(tmpAngle[i]);
			}
		}
		if (maxangle > m_param->errAngle) {
			errAngleOk = false;
			allok = false;
			m_testItemsStatus[3] = 0;
		}
		setDetectOutData(ErrAngleOk, errAngleOk);
		setDetectOutData(ErrAngle, maxangle / PI * 180);
		//setDataBaseData(ErrAngle, maxangle / PI * 180);
	}
	if (m_param->IsErrWidth)
	{
		bool errwidthok = true;
		float maxwidth = 0;
		float minwidth = 10000;
		float dis;
		if (m_param->IsEndPoint)
		{
			dis = sqrt((finalPointCol[1] - finalPointCol[0])*(finalPointCol[1] - finalPointCol[0]) +
				(finalPointRow[1] - finalPointRow[0])*(finalPointRow[1] - finalPointRow[0]));
			if (maxwidth < dis)
				maxwidth = dis;
			if (minwidth > dis)
				minwidth = dis;
			dis = sqrt((finalPointCol[2] - finalPointCol[3])*(finalPointCol[2] - finalPointCol[3]) +
				(finalPointRow[2] - finalPointRow[3])*(finalPointRow[2] - finalPointRow[3]));
			if (maxwidth < dis)
				maxwidth = dis;
			if (minwidth > dis)
				minwidth = dis;
		}
		else {
			dis = sqrt((RowBegin[0][0].D() - RowEnd[0][0].D())*(RowBegin[0][0].D() - RowEnd[0][0].D()) +
				(ColBegin[0][0].D() - ColEnd[0][0].D())*(ColBegin[0][0].D() - ColEnd[0][0].D()));
			if (maxwidth < dis)
				maxwidth = dis;
			if (minwidth > dis)
				minwidth = dis;
			dis = sqrt((RowBegin[1][0].D() - RowEnd[1][0].D())*(RowBegin[1][0].D() - RowEnd[1][0].D()) +
				(ColBegin[1][0].D() - ColEnd[1][0].D())*(ColBegin[1][0].D() - ColEnd[1][0].D()));
			if (maxwidth < dis)
				maxwidth = dis;
			if (minwidth > dis)
				minwidth = dis;
		}
		if (maxwidth > m_param->errWidthMax || minwidth < m_param->errWidthMin) {
			errwidthok = false;
			m_testItemsStatus[1] = 0;
			allok = false;
		}
		setDetectOutData(ErrWidthOk, errwidthok);
		setDetectOutData(ErrWidthMin, minwidth);
		setDetectOutData(ErrWidthMax, maxwidth);
	}
	if (m_param->IsErrHeight)
	{
		bool errheightok = true;
		float maxheight = 0;
		float minheight = 10000;
		float dis;
		if (m_param->IsEndPoint)
		{
			dis = sqrt(
				(finalPointCol[3] - finalPointCol[0])*(finalPointCol[3] - finalPointCol[0]) +
				(finalPointRow[3] - finalPointRow[0])*(finalPointRow[3] - finalPointRow[0]));
			if (maxheight < dis)
				maxheight = dis;
			if (minheight > dis)
				minheight = dis;
			dis = sqrt(
				(finalPointCol[2] - finalPointCol[1])*(finalPointCol[2] - finalPointCol[1]) +
				(finalPointRow[2] - finalPointRow[1])*(finalPointRow[2] - finalPointRow[1]));
			if (maxheight < dis)
				maxheight = dis;
			if (minheight > dis)
				minheight = dis;
		}
		else {
			dis = sqrt(
				(RowBegin[2][0].D() - RowEnd[2][0].D())*(RowBegin[2][0].D() - RowEnd[2][0].D()) +
				(ColBegin[2][0].D() - ColEnd[2][0].D())*(ColBegin[2][0].D() - ColEnd[2][0].D()));
			if (maxheight < dis)
				maxheight = dis;
			if (minheight > dis)
				minheight = dis;
			dis = sqrt(
				(RowBegin[3][0].D() - RowEnd[3][0].D())*(RowBegin[3][0].D() - RowEnd[3][0].D()) +
				(ColBegin[3][0].D() - ColEnd[3][0].D())*(ColBegin[3][0].D() - ColEnd[3][0].D()));
			if (maxheight < dis)
				maxheight = dis;
			if (minheight > dis)
				minheight = dis;
		}
		if (maxheight > m_param->errHeightmax || minheight < m_param->errHeightMin) {
			errheightok = false;
			m_testItemsStatus[2] = 0;
			allok = false;
		}
		setDetectOutData(ErrHeightOk, errheightok);
		setDetectOutData(ErrHeightMax, maxheight);
		setDetectOutData(ErrHeightMin, minheight);
	}
	if (m_param->IsErrCenterOffset)
	{
		float Centershift = 0;
		bool centerok = true;
		Centershift = sqrt(pow((center->row - m_param->TmpCenterRow), 2) + pow((center->col - m_param->TmpCenterCol), 2));
		if (Centershift > m_param->errCenterOffset) {
			allok = false;
			centerok = false;
			m_testItemsStatus[4] = 0;
		}
		setDetectOutData(ErrCenterShift, Centershift);
		setDetectOutData(ErrCenterShiftOk, centerok);
	}
	delete center;
	setDetectOutData(AllOK, allok);
	return allok ? 0 : -1;
}

void standEdgeMod::viewResult(ImageView *iv, const QString &funName, int)
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
		const Hobject *obj = getDispObject(DispPointsImg);
		count_obj(*obj, &num);
		if (num > 0) {
			RlRegion rlReg;
			HalOpenCV::hal2cvRegion(*obj, &rlReg);
			iv->dispRegion(&rlReg);
		}
		else {
			return;
		}
	}
	HTuple findok;
	for (int i = 0;i < 4;i++) {
		getDispOutData(FindOk, i, findok);
		if (findok[0].I() != 0) {
			iv->setColor(255, 0, 0);
			iv->setFilled(false);
			iv->dispRect1(m_param->RoiRegionVector[i + 1].row1, m_param->RoiRegionVector[i + 1].col1, m_param->RoiRegionVector[i + 1].row2,
				m_param->RoiRegionVector[i + 1].col2);

			return;
		}
	}
	for (int i = 0;i < 4;i++) {
		HTuple rowbegin, colbegin, rowend, colend;
		getDispOutData(SearchBeginRow, i, rowbegin);
		getDispOutData(SearchBeginCol, i, colbegin);
		getDispOutData(SearchEndRow, i, rowend);
		getDispOutData(SearchEndCol, i, colend);
		iv->setColor(0, 0, 255);
		iv->dispLine(rowbegin[0].D(), colbegin[0].D(), rowend[0].D(), colend[0].D());
	}
	HTuple maxangleok;
	getDispOutData(MaxAngleOK, maxangleok);
	if (maxangleok[0].I() == 0) {
		return;
	}
	if (m_param->IsEndPoint) {
		HTuple endpointok;
		getDispOutData(EndPointOk, endpointok);
		int vala = endpointok[0].I();
		if (endpointok[0].I() == 0) {
			return;
		}
		HTuple beginrow, begincol, endrow, endcol;
		iv->setColor(255, 255, 0);
		for (int i = 0;i < 4;i++) {
			getDispOutData(BeginRow, i, beginrow);
			getDispOutData(BeginCol, i, begincol);
			getDispOutData(EndRow, i, endrow);
			getDispOutData(EndCol, i, endcol);
			iv->dispLine(beginrow[0].D(), begincol[0].D() - 3, beginrow[0].D(), begincol[0].D() + 3);
			iv->dispLine(beginrow[0].D() - 3, begincol[0].D(), beginrow[0].D() + 3, begincol[0].D());
			iv->dispLine(endrow[0].D(), endcol[0].D() - 3, endrow[0].D(), endcol[0].D() + 3);
			iv->dispLine(endrow[0].D() - 3, endcol[0].D(), endrow[0].D() + 3, endcol[0].D());
		}
	}
	HTuple standok0;

	HTuple allok;
	getDispOutData(AllOK, allok);
	if (allok[0].I() == 0) {
		iv->setColor(255, 0, 0);
	}
	else if (allok[0].I() == 1) {
		iv->setColor(0, 255, 0);
	}
	HTuple polyRow, polyCol;
	//getDispOutData(FinalRow, polyRow);
	//getDispOutData(FinalCol, polyCol);
	for (auto i = 0;i < p_corners->size();++i)
	{
		polyRow[i] = p_corners->at(i).y;
		polyCol[i] = p_corners->at(i).x;
	}
	polyRow[4] = polyRow[0];
	polyCol[4] = polyCol[0];
	Hobject polyRec;
	gen_contour_polygon_xld(&polyRec, polyRow, polyCol);
	gen_region_contour_xld(polyRec, &polyRec, "filled");
	iv->setFilled(false);
	RlRegion rlReg;
	HalOpenCV::hal2cvRegion(polyRec, &rlReg);
	iv->dispRegion(/*polyRec*/&rlReg);
	HTuple centerRow, centerCol;
	getDispOutData(CenterRow, centerRow);
	getDispOutData(CenterCol, centerCol);
	iv->dispLine(centerRow[0].D(), centerCol[0].D() - 3, centerRow[0].D(), centerCol[0].D() + 3);
	iv->dispLine(centerRow[0].D() - 3, centerCol[0].D(), centerRow[0].D() + 3, centerCol[0].D());
}

void standEdgeMod::textResult(ResultText *text, const QString &funName)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	HTuple found;
	char charbuf[4][32] = { "Top","Bottom","Left","Right" };
	for (int i = 0;i < 4;i++) {
		getDispOutData(FindOk, i, found);
		if (found[0].I() != 0) {
			text->setTextColor(QColor(255, 0, 0));
			text->append(QString::fromLocal8Bit(charbuf[i]));
			//text->insertPlainText(QString::fromLocal8Bit(" edge found failed"));
			if (found[0].I() == 1) {
				text->insertPlainText(QObject::tr(" edge found failed"));  //" edge found failed"
																	//text->insertPlainText(QObject::tr(" 边界查找失败"));  //" edge found failed"
			}
			else if (found[0].I() == 2) {
				//text->insertPlainText(QString::fromLocal8Bit(" 边界点过少"));
				text->insertPlainText(QObject::tr(" edge points found aren't enough "));
			}
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
		text->insertPlainText(QObject::tr(" max angle = %1(%2)").arg(QString::number(maxangle[0].D(), 'g', 3)).
			arg(QString::number(m_param->maxAngle)));
		return;
	}
	if (m_param->IsEndPoint) {
		HTuple endpointok;
		getDispOutData(EndPointOk, endpointok);
		if (endpointok[0].I() == 0) {
			HTuple endindex;
			getDispOutData(EndNGIndex, endindex);
			text->setTextColor(QColor(255, 0, 0));
			text->append(charbuf[endindex[0].I()]);
			text->insertPlainText(QObject::tr("detect end points failed."));
			return;
		}
	}
	if (m_param->IsErrWidth) {
		HTuple errwidthok, errwidthmax, errwidthmin;
		getDispOutData(ErrWidthOk, errwidthok);
		getDispOutData(ErrWidthMax, errwidthmax);
		getDispOutData(ErrWidthMin, errwidthmin);
		if (errwidthok[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
		}
		text->append(QObject::tr("Body Width = (%1,%2) (%3,%4)").arg(QString::number(errwidthmin[0].D(), 'g', 3)).
			arg(QString::number(errwidthmax[0].D(), 'g', 3)).arg(QString::number(m_param->errWidthMin)).arg(QString::number(m_param->errWidthMax)));
	}
	if (m_param->IsErrHeight) {
		HTuple errheightok, errheightmin, errheightmax;
		getDispOutData(ErrHeightOk, errheightok);
		getDispOutData(ErrHeightMax, errheightmax);
		getDispOutData(ErrHeightMin, errheightmin);
		if (errheightok[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
		}
		text->append(QObject::tr(" Body Height= %1(%2,%3)").arg(QString::number(errheightmin[0].D(), 'g', 3)).
			arg(QString::number(errheightmax[0].D(), 'g', 3)).arg(QString::number(m_param->errHeightMin)).arg(QString::number(m_param->errHeightmax)));
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
		text->append(QObject::tr(" Body angle= %1(%2)").arg(QString::number(errangle[0].D(), 'g', 3)).
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
		text->append(QObject::tr(" Body shift= %1(%2)").arg(QString::number(errcenter[0].I())).
			arg(QString::number(m_param->errCenterOffset)));
	}
	text->setTextColor(QColor(0, 0, 0));
}

void standEdgeMod::iniData()
{
	m_param = new standEdgeParam;
	m_image = new Hobject;
	createPins();
}

void standEdgeMod::createPins()
{
	addPin(&p_im, "im");

	addPin(&p_corners, "corners");
	addPin(&p_edgeRowBeg, "outRowB");
	addPin(&p_edgeRowEnd, "outRowE");
	addPin(&p_edgeColBeg, "outColB");
	addPin(&p_edgeColEnd, "outColE");
}

bool standEdgeMod::IsPointInRegion(const Hobject * InputReg, double row, double col, int larger)
{
	if (InputReg == NULL || InputReg->Id() == H_EMPTY_REGION)
	{
		return false;
	}
	Hobject uobj;
	union1(*InputReg, &uobj);
	double dismin, dismax;
	distance_pr(uobj, row, col, &dismin, &dismax);
	bool IsIn;
	if ((dismin - larger) < 0.001) {
		IsIn = true;
	}
	else {
		IsIn = false;
	}
	return IsIn;
}

bool standEdgeMod::sortPointsByHisto(const QList<CvPoint2D32f>& inputPoints, QList<QList<CvPoint2D32f>>& outputPoints, 
	int sortGroup, int threshold, int angleRange, bool horOrVer, int grayStep)
{
	bool resOk = true;
	int stepAngle = 1;

	QList<CvPoint2D32f> bestPoints;
	int minPositionSize = 9999999;
	double bestMinIndex = 9999999;
	QList<int> bestCount;

	for (int a = -abs(angleRange);a < abs(angleRange);a += stepAngle)
	{
		HTuple hommat;
		double rangle = double((double)a / 180.0)*CV_PI;
		vector_angle_to_rigid(0, 0, 0, 0, 0, rangle, &hommat);

		QList<CvPoint2D32f> newPoints;
		double minIndex, maxIndex;
		minIndex = 999999;
		maxIndex = -999999;
		foreach(CvPoint2D32f p, inputPoints) {
			double row, col;
			affine_trans_pixel(hommat, double(p.y), double(p.x), &row, &col);
			CvPoint2D32f np;
			np.x = col;
			np.y = row;
			newPoints.push_back(np);
			double tmpval;
			if (horOrVer == 0) {
				tmpval = row;
			}
			else {
				tmpval = col;
			}
			if (tmpval > maxIndex)
				maxIndex = tmpval;
			if (tmpval < minIndex)
				minIndex = tmpval;
		}
		QList<int> pCount;
		int positionSize = (int)(maxIndex + 0.5) - (int)(minIndex + 0.5) + 1;

		for (int i = 0;i < positionSize;i++) {
			pCount.push_back(0);
		}
		foreach(CvPoint2D32f  p, newPoints)
		{
			double tmpval;
			if (horOrVer == 0) {
				tmpval = p.y;
			}
			else {
				tmpval = p.x;
			}
			int tmpindex = int(tmpval + 0.5) - int(minIndex + 0.5);
			pCount[tmpindex]++;
		}
		if (minPositionSize > positionSize)
		{
			minPositionSize = positionSize;
			bestPoints.clear();
			bestPoints = newPoints;
			bestCount.clear();
			bestCount = pCount;
			bestMinIndex = minIndex;
		}
		int ccc = pCount[0];
		//for (int i=0;)
		//{
		//}
	}

	QList<int> lastCount;
	QList<int> nextCount;

	for (int i = 0;i < grayStep;i++) {
		lastCount << 0;
		nextCount << 0;
	}
	bool isThre = false;
	int clusterNum = 0;

	QList<QList<int>> threEdgePoints;  //分割出来的边界点,每一个区间一前一后两个点
	for (int i = 0;i < bestCount.size();i++) {
		int lastSum, nextSum;
		lastSum = 0;nextSum = 0;
		for (int j = 0;j < grayStep;j++)
		{
			if ((i - j - 1) >= 0) {
				lastCount[grayStep - 1 - j] = bestCount[i - j - 1];
			}
			else {
				lastCount[grayStep - 1 - j] = 0;
			}

			if ((i + j) <= (bestCount.size() - 1)) {
				nextCount[j] = bestCount[i + j];
			}
			else {
				nextCount[j] = 0;
			}
		}
		for (int j = 0;j < grayStep;j++) {
			lastSum += lastCount[j];
			nextSum += nextCount[j];
		}

		if (isThre == false)
		{
			if ((nextSum - lastSum - threshold) > 0)
			{
				isThre = true;
				QList<int> edgep;
				edgep << 0 << 0;
				edgep[0] = i;
				threEdgePoints.push_back(edgep);
			}
		}
		else if (isThre == true) {
			if ((lastSum - nextSum - threshold) > 0)
			{
				int mi = i;
				for (int j = 0;j < grayStep;j++)
				{
					if ((i + grayStep - 1 - j) > (bestCount.size() - 1))
					{
						continue;
					}
					if (bestCount[i + grayStep - 1 - j] <= 1)
					{
						continue;
					}
					mi = i + grayStep - 1 - j;
				}
				isThre = false;
				threEdgePoints[clusterNum][1] = mi;
				clusterNum++;

			}
		}
	}
	if (clusterNum != sortGroup)
	{
		return false;
	}

	QList<QList<int>> clusterPoints;  //各个簇对应分进去的点的序号
	for (int i = 0;i < clusterNum;i++)
	{
		QList<int> newindex;
		clusterPoints.push_back(newindex);
		QList<CvPoint2D32f> newlist;
		outputPoints.push_back(newlist);
	}
	for (int i = 0;i < bestPoints.size();i++)
	{
		double tmpval;
		if (horOrVer == 0) {
			tmpval = bestPoints[i].y;
		}
		else {
			tmpval = bestPoints[i].x;
		}
		for (int j = 0;j < clusterNum;j++)
		{
			int val = int(tmpval + 0.5) - int(bestMinIndex + 0.5);
			if (val <= threEdgePoints[j][1] && val >= threEdgePoints[j][0])
			{
				clusterPoints[j].push_back(i);
				CvPoint2D32f tmpcv;
				tmpcv.x = inputPoints[i].x;
				outputPoints[j].push_back(inputPoints.at(i));
				continue;
			}
		}
	}

	return resOk;
}

