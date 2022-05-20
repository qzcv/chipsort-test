#include "dimensionMeasureMod.h"
#include "dimensionMeasureParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"

dimensionMeasureMod::dimensionMeasureMod()
{
	iniData();
}

dimensionMeasureMod::dimensionMeasureMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

dimensionMeasureMod::~dimensionMeasureMod()
{
	delete m_param;
	delete m_image;
}

int dimensionMeasureMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int dimensionMeasureMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void dimensionMeasureMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void dimensionMeasureMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int dimensionMeasureMod::run(const QString &funName)
{
// 	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
// 		m_testItemsStatus[i] = 1;
// 	}
// 	if (!m_image || m_image->Id() == H_EMPTY_REGION) {
// 		allok = false;
// 		setDetectOutData(ImageFull, false);
// 		m_testItemsStatus[0] = 0;
// 		return allok;
// 	}
// 	else {
// 		setDetectOutData(ImageFull, true);
// 	}
// 	bool allok = true;
	m_allok = 0;
	if (p_im->empty())
		return -1;
	if (m_param->IsReferSection&&p_homMat2d->size() != cv::Size(2, 3))
		return -1;

	HalOpenCV::cv2halImg(*p_im, m_image, false);

	bool regionOK[Max_Roi_Num];
	bool foundOK[Max_Roi_Num];
	for (int i = 0;i < Max_Roi_Num;i++) {
		regionOK[i] = true;
		foundOK[i] = true;
	}
	double rotateAngle = 0;
	HTuple homMat2D;
	if (m_param->IsReferSection) {
		try {
			HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &homMat2D);
			double tv = homMat2D[0].D();
		}
		catch (HException& ex) {
			hom_mat2d_identity(&homMat2D);
		}
		rotateAngle = atan((homMat2D[3].D() / homMat2D[0].D()));
	}
	HTuple imgWidth, imgHeight;
	get_image_size(*m_image, &imgWidth, &imgHeight);
	HTuple errUltraIndex;
	HTuple oklengthmax; //正常时候的最大长度
	HTuple oklengthmin; //正常时候的最小长度

	for (int i = 0;i < Max_Roi_Num;i++) {
		errUltraIndex[i] = 0;
		//errLength[i]=0;
	}
	HTuple errStartRow[Max_Roi_Num], errEndRow[Max_Roi_Num], errStartCol[Max_Roi_Num], errEndCol[Max_Roi_Num];   //记录长度异常坐标点
	HTuple StartRow[Max_Roi_Num], EndRow[Max_Roi_Num], StartCol[Max_Roi_Num], EndCol[Max_Roi_Num];   //记录长度异常坐标点
	for (int i = 0;i < m_param->regionCount;i++) {
		double lengthmin = 9999.0;
		double lengthmax = 0;  //正常值里面的最大最小值
		Hlong measureHandle1, measureHandle2;
		int errPointCount = 0;   //长度错误点计数
		int okPointCount = 0;    //长度正确点计数
		HTuple errOFFLength;  //记录错误的长度差距
		HTuple errRawLength;  //求出的实际长度
		double RoiRecLeftRow, RoiRecLeftCol, RoiRecRightRow, RoiRecRightCol;
		double angle1;
		angle1 = rotateAngle + m_param->RoiRegionVector[i].angle;
		while (!(angle1 <= PI / 2.0&&angle1 > (-PI / 2.0))) {
			if (angle1 > 0) {
				angle1 -= PI;
			}
			else if (angle1 < 0) {
				angle1 += PI;
			}
		}
		Hobject RoiRec;
		gen_rectangle2_contour_xld(&RoiRec, m_param->RoiRegionVector[i].row, m_param->RoiRegionVector[i].col
			, m_param->RoiRegionVector[i].angle, m_param->RoiRegionVector[i].length1, m_param->RoiRegionVector[i].length2);
		if (m_param->IsReferSection) {
			affine_trans_contour_xld(RoiRec, &RoiRec, homMat2D);
		}
		HTuple roiRow, roiCol;
		get_contour_xld(RoiRec, &roiRow, &roiCol);//右上开始逆时针顺序
		double dis[2];
		double roww[4], coll[4];
		for (int h = 0;h < 4;h++) {
			roww[h] = roiRow[h].D();
			coll[h] = roiCol[h].D();
		}
		distance_pp(roiRow[0].D(), roiCol[0].D(), roiRow[1].D(), roiCol[1].D(), &dis[0]);
		distance_pp(roiRow[2].D(), roiCol[2].D(), roiRow[1].D(), roiCol[1].D(), &dis[1]);
		double midRow[2], midCol[2];
		//if((int)(dis[0]*100)==(int)(m_param->RoiRegionVector[i].length1*100.0*2.0)){
		if (abs(dis[0] - m_param->RoiRegionVector[i].length1 * 2) < 1e-1) {
			midRow[0] = (roiRow[1].D() + roiRow[2].D()) / 2.0;
			midRow[1] = (roiRow[0].D() + roiRow[3].D()) / 2.0;
			midCol[0] = (roiCol[1].D() + roiCol[2].D()) / 2.0;
			midCol[1] = (roiCol[0].D() + roiCol[3].D()) / 2.0;
		}
		else if (abs(dis[1] - m_param->RoiRegionVector[i].length1 * 2) < 1e-1) {
			midRow[0] = (roiRow[0].D() + roiRow[1].D()) / 2.0;
			midRow[1] = (roiRow[2].D() + roiRow[3].D()) / 2.0;
			midCol[0] = (roiCol[0].D() + roiCol[1].D()) / 2.0;
			midCol[1] = (roiCol[2].D() + roiCol[3].D()) / 2.0;
		}
		else {
			midRow[0] = (roiRow[1].D() + roiRow[2].D()) / 2.0;
			midRow[1] = (roiRow[0].D() + roiRow[3].D()) / 2.0;
			midCol[0] = (roiCol[1].D() + roiCol[2].D()) / 2.0;
			midCol[1] = (roiCol[0].D() + roiCol[3].D()) / 2.0;
		}
		int index1;
		if ((midCol[1] - midCol[0]) > (1e-3))
			index1 = 0;
		else if ((midCol[0] - midCol[1]) > (1e-3))
			index1 = 1;
		else if (abs(midCol[0] - midCol[1]) <= (1e-3)) {
			if ((midRow[0] > midRow[1]))
				index1 = 1;
			else {
				index1 = 0;
			}
		}
		RoiRecLeftRow = midRow[0 + index1];
		RoiRecLeftCol = midCol[0 + index1];
		RoiRecRightRow = midRow[1 - index1];
		RoiRecRightCol = midCol[1 - index1];
		int pointN = 0;   //累计进行测试的点1数
		int usefulPoint = 0;  // 累1计有效得出结果的点数(结1果不为空)
		double startpoint0, endpoint0;
		double gapplus;
		bool isVertical;
		if (abs((abs(angle1) - PI / 2.0)) > (1e-3)) {
			startpoint0 = RoiRecLeftCol;
			endpoint0 = RoiRecRightCol;
			gapplus = double(cos(angle1)*m_param->searchGap);
			isVertical = false;
		}
		else {
			startpoint0 = RoiRecLeftRow;
			endpoint0 = RoiRecRightRow;
			gapplus = m_param->searchGap;
			isVertical = true;
		}
		//bool headPositive;  //角度的朝向
		//if (angle1>0){

		//}
		//setDetectOutData(IsVer,isVertical,i);
		gen_measure_rectangle2(RoiRecLeftRow, RoiRecLeftCol   //横向ROI的时候，上到下搜索 竖向左到右
			, angle1 - PI / 2.0, m_param->RoiRegionVector[i].length2, 4, imgWidth[0], imgHeight[0], "nearest_neighbor", &measureHandle1);
		gen_measure_rectangle2(RoiRecLeftRow, RoiRecLeftCol   //横向ROI的时候，下到上搜索 竖向右到左
			, angle1 + PI / 2.0, m_param->RoiRegionVector[i].length2, 4, imgWidth[0], imgHeight[0], "nearest_neighbor", &measureHandle2);
		HTuple rowEdge1, colEdge1, amp1, distance1;
		HTuple rowEdge2, colEdge2, amp2, distance2;
		int allNeedPoints;   //所有的需要检测的点总数
		allNeedPoints = (endpoint0 - startpoint0) / gapplus;
		for (double startpoint = startpoint0 + gapplus;startpoint < endpoint0;startpoint += gapplus) {
			if (isVertical) {
				translate_measure(measureHandle1, startpoint, RoiRecLeftCol);
				translate_measure(measureHandle2, startpoint, RoiRecLeftCol);
				if (startpoint<2 || startpoint>(imgHeight[0].I() - 2) || RoiRecLeftCol<2 || RoiRecLeftCol>(imgWidth[0].I() - 2)) {
					pointN++;
					continue;
				}
			}
			else {
				translate_measure(measureHandle1, RoiRecLeftRow - (pointN + 1)*double(sin(angle1)*m_param->searchGap), startpoint);
				translate_measure(measureHandle2, RoiRecLeftRow - (pointN + 1)*double(sin(angle1)*m_param->searchGap), startpoint);
				double strow, stcol;
				strow = RoiRecLeftRow - (pointN + 1)*double(sin(angle1)*m_param->searchGap);
				stcol = startpoint;
				if (strow<2 || strow>(imgHeight[0].I() - 2) || stcol<2 || stcol>(imgWidth[0].I() - 2)) {
					pointN++;
					continue;
				}
			}
			//strow+=1;
			//stcol+=1;
			//translate_measure(measureHandle1,strow,stcol);
			//translate_measure(measureHandle2,strow,stcol);

			char edgeBuf[32];
			if (!m_param->IsBlack) {
				sprintf(edgeBuf, "positive");
			}
			else {
				sprintf(edgeBuf, "negative");
			}
			measure_pos(*m_image, measureHandle1, 1, m_param->threshold, edgeBuf/*edge*/, "all", &rowEdge1, &colEdge1, &amp1, &distance1);
			measure_pos(*m_image, measureHandle2, 1, m_param->threshold, edgeBuf/*edge*/, "all", &rowEdge2, &colEdge2, &amp2, &distance2);
			int fir = rowEdge1.Num();
			int sec = rowEdge2.Num();
			int bestFirInx, bestSecInx;
			double sad;
			double sads;
			bool meaOk = true;
			HTuple sortInx;
			if (fir < 1 || sec < 1) {
				meaOk = false;
			}
			else {
				if (m_param->IsBestEdge) {
					for (int a = 0;a < amp1.Num();a++) {
						amp1[a] = abs(amp1[a].D());
						sad = amp1[a].D();
					}
					for (int a = 0;a < amp2.Num();a++) {
						amp2[a] = abs(amp2[a].D());
						sads = amp2[a].D();
					}
					tuple_sort_index(amp1, &sortInx);
					bestFirInx = sortInx[amp1.Num() - 1].I();
					tuple_sort_index(amp2, &sortInx);
					bestSecInx = sortInx[amp2.Num() - 1].I();
				}
				else {
					bestFirInx = 0;
					bestSecInx = sec - 1;
				}
				if (!isVertical) {   //横向的时候，边界1应该在上方
					if (rowEdge1[bestFirInx].D() > rowEdge2[bestSecInx].D()) {
						pointN++;
						continue;
					}
				}
				else {
					if (colEdge1[bestFirInx].D() > colEdge2[bestSecInx].D()) {
						pointN++;
						continue;
					}
				}
			}
			HTuple rowedge, coledge;
			if (meaOk) {
				rowedge[0] = rowEdge1[bestFirInx];
				coledge[0] = colEdge1[bestFirInx];
				rowedge[1] = rowEdge2[bestSecInx];
				coledge[1] = colEdge2[bestSecInx];

				StartRow[i][usefulPoint] = rowedge[0];
				EndRow[i][usefulPoint] = rowedge[1];
				StartCol[i][usefulPoint] = coledge[0];
				EndCol[i][usefulPoint] = coledge[1];

				double lineDis;
				distance_pp(rowedge[0].D(), coledge[0].D(), rowedge[1].D(), coledge[1].D(), &lineDis);
				if (lineDis > m_param->lengthMaxVec[i] || lineDis < m_param->lengthMinVec[i]) {
					double errlength;
					if (lineDis > m_param->lengthMaxVec[i])
						errlength = abs(lineDis - m_param->lengthMaxVec[i]);
					else if (lineDis < m_param->lengthMinVec[i])
						errlength = abs(lineDis - m_param->lengthMinVec[i]);
					errStartRow[i][errPointCount] = rowedge[0];
					errEndRow[i][errPointCount] = rowedge[1];
					errStartCol[i][errPointCount] = coledge[0];
					errEndCol[i][errPointCount] = coledge[1];
					errOFFLength[errPointCount] = errlength;
					errRawLength[errPointCount] = lineDis;
					errPointCount++;
				}
				else {
					if (lineDis > lengthmax) {
						lengthmax = lineDis;
					}
					if (lineDis < lengthmin) {
						lengthmin = lineDis;
					}
					okPointCount++;
				}

				usefulPoint++;
			}
			pointN++;
		}  //end of loop gapplus
		if (m_param->IsValidPoints) {
			double usePer = double((double)usefulPoint / (double)pointN);
			if (usePer<double((double)m_param->validPointsPer / 100.0)) {
				foundOK[i] = false;
				//m_testItemsStatus[2] = 0;
				//allok = false;
				m_allok = -3;
			}
			m_foundOk[i] = foundOK[i];
			m_usePer[i] = usePer;
			//setDetectOutData(PointsOK, foundOK[i], i);
			//setDetectOutData(PointsPer, usePer, i);
		}
		if (errOFFLength.Num() > 0) {
			regionOK[i] = false;
			HTuple sortLength, indexLength;
			tuple_sort(errOFFLength, &sortLength);
			tuple_sort_index(errOFFLength, &indexLength);
			int endindex = 0;  //最后一个非噪音的点坐标
			bool haveNoise = false;
			for (int j = 0;j < errOFFLength.Num();j++) {
				if (sortLength[j].D() > m_param->noiseMin) {  //噪点容忍值
					endindex = j - 1;
					haveNoise = true;
					//if (endindex>=0){
					//	regionOK[i]=false;
					//}
					break;
				}
			}
			if (haveNoise) {
				if (m_param->noiseMaxNum < (errOFFLength.Num() - endindex - 1)) {
					//regionOK[i]=false;
					errUltraIndex[i] = errOFFLength.Num() - m_param->noiseMaxNum - 1;
					m_errUltraIndex[i] = errUltraIndex[i].I();
					//setDetectOutData(ErrUltraIndex, errUltraIndex[i].I(), i);
				}
				else if (m_param->noiseMaxNum >= (errOFFLength.Num() - endindex - 1)) {
					if (endindex >= 0) {
						errUltraIndex[i] = endindex;
					}
					if (endindex == -1) {
						regionOK[i] = true;    //噪点全消除，并且没有非噪点错误长度
					}
					m_errUltraIndex[i] = errUltraIndex[i].I();
					//setDetectOutData(ErrUltraIndex, errUltraIndex[i].I(), i);
				}
			}
			else {
				errUltraIndex[i] = errOFFLength.Num() - 1;
				m_errUltraIndex[i] = errUltraIndex[i].I();
				//setDetectOutData(ErrUltraIndex, errUltraIndex[i].I(), i);
				int dddd = errUltraIndex[i].I();
			}
		}

		m_errLength[i] = errOFFLength;
		//setDetectOutData(ErrLength, errOFFLength, i);
		if (lengthmin > 9900) {
			lengthmin = 0;
		}
		m_lengthmin[i] = lengthmin;
		m_lengthmax[i] = lengthmax;
		//setDetectOutData(OkLengthMin, lengthmin, i);
		//setDetectOutData(OkLengthMax, lengthmax, i);

		m_StartRow[i] = StartRow[i];
		m_StartCol[i] = StartCol[i];
		m_EndRow[i] = EndRow[i];
		m_EndCol[i] = EndCol[i];
		//setDetectOutData(EdgeStartRow, StartRow[i], i);
		//setDetectOutData(EdgeEndRow, EndRow[i], i);
		//setDetectOutData(EdgeStartCol, StartCol[i], i);
		//setDetectOutData(EdgeEndCol, EndCol[i], i);

		//setDetectOutData(BadEdgeStartRow, errStartRow[i], i);
		//setDetectOutData(BadEdgeEndRow, errEndRow[i], i);
		//setDetectOutData(BadEdgeStartCol, errStartCol[i], i);
		//setDetectOutData(BadEdgeEndCol, errEndCol[i], i);
		//setDetectOutData(RegionOk, regionOK[i], i);
		m_errStartRow[i] = errStartRow[i];
		m_errStartCol[i] = errStartCol[i];
		m_errEndRow[i] = errEndRow[i];
		m_errEndCol[i] = errEndCol[i];
		m_regionOk[i] = regionOK[i];
		close_measure(measureHandle1);
		close_measure(measureHandle2);
	}  //end of loop i
	for (int i = 0;i < Max_Roi_Num;i++) {
		if (!regionOK[i]) {
			//m_testItemsStatus[1] = 0;
			//allok = false;
			m_allok = -2;
			break;
		}
	}
	return m_allok;
}

void dimensionMeasureMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (p_im->empty())
		return;
	if (m_param->IsReferSection&&p_homMat2d->size() != cv::Size(2, 3))
		return;

	for (int i = 0;i < m_param->regionCount;i++) {
		HTuple thisok;
		thisok[0] = m_regionOk[i];
		HTuple pointsok;
		if (m_param->IsValidPoints) {
			pointsok[0] = m_foundOk[i];
		}
		else {
			pointsok[0] = 1;
		}

		if (thisok[0].I() == 0 || pointsok == 0) {
			iv->setColor(255, 0, 0);
		}
		else {
			iv->setColor(0, 255, 0);
		}
		iv->setFilled(false);
		char buf[32];
		sprintf(buf, "%d", i);
		if (m_param->IsReferSection) {
			if (m_param->IsShowAfterDet) {
				Hobject roirec;
				gen_rectangle2(&roirec, m_param->RoiRegionVector[i].row, m_param->RoiRegionVector[i].col,
					m_param->RoiRegionVector[i].angle, m_param->RoiRegionVector[i].length1, m_param->RoiRegionVector[i].length2);
				HTuple hommat2d;
				HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &hommat2d);
				affine_trans_region(roirec, &roirec, hommat2d, "false");
				RlRegion reg;
				HalOpenCV::hal2cvRegion(roirec, &reg);
				iv->dispRegion(&reg, buf);
			}
		}
		else {
			if (m_param->IsShowAfterDet) {
				iv->dispRect2(m_param->RoiRegionVector[i].row, m_param->RoiRegionVector[i].col,
					m_param->RoiRegionVector[i].angle, m_param->RoiRegionVector[i].length1, m_param->RoiRegionVector[i].length2, buf);
			}
		}
		if (pointsok[0].I() == false) {
			if (m_usePer[i] < 0.1) {
				continue;
			}
		}

		HTuple EdgeStRow, EdgeEdRow, EdgeStCol, EdgeEdCol;
		EdgeStRow = m_StartRow[i];
		EdgeStCol = m_StartCol[i];
		EdgeEdRow = m_EndRow[i];
		EdgeEdCol = m_EndCol[i];
// 		getDispOutData(EdgeStartRow, i, EdgeStRow);
// 		getDispOutData(EdgeEndRow, i, EdgeEdRow);
// 		getDispOutData(EdgeStartCol, i, EdgeStCol);
// 		getDispOutData(EdgeEndCol, i, EdgeEdCol);
		int edgenum = EdgeStRow.Num();
		iv->setColor(0, 255, 0);
		for (int j = 0;j < edgenum;j++) {
			iv->dispLine(EdgeStRow[j].D(), EdgeStCol[j].D() - 2, EdgeStRow[j].D(), EdgeStCol[j].D() + 2);
			iv->dispLine(EdgeStRow[j].D() - 2, EdgeStCol[j].D(), EdgeStRow[j].D(), EdgeStCol[j].D());
			iv->dispLine(EdgeEdRow[j].D(), EdgeEdCol[j].D() - 2, EdgeEdRow[j].D(), EdgeEdCol[j].D() + 2);
			iv->dispLine(EdgeEdRow[j].D(), EdgeEdCol[j].D(), EdgeEdRow[j].D() + 2, EdgeEdCol[j].D());
		}
		if (thisok[0].I() == 0) {
			iv->setColor(255, 0, 0);
		}
		else {
			iv->setColor(0, 255, 0);
		}
		if (thisok[0].I() == 0) {
			HTuple errlength, errultIndex, badEdgeStRow, badEdgeEdRow, badEdgeStCol, badEdgeEdCol;
			errlength = m_errLength[i];
			errultIndex = m_errUltraIndex[i];
			badEdgeStRow = m_errStartRow[i];
			badEdgeStCol = m_errStartCol[i];
			badEdgeEdRow = m_errEndRow[i];
			badEdgeEdCol = m_errEndCol[i];
// 			getDispOutData(ErrLength, i, errlength);
// 			getDispOutData(ErrUltraIndex, i, errultIndex);
// 			getDispOutData(BadEdgeStartRow, i, badEdgeStRow);
// 			getDispOutData(BadEdgeEndRow, i, badEdgeEdRow);
// 			getDispOutData(BadEdgeStartCol, i, badEdgeStCol);
// 			getDispOutData(BadEdgeEndCol, i, badEdgeEdCol);
			int len = errlength.Num();
			HTuple sortlength, indexlength;
			tuple_sort(errlength, &sortlength);
			tuple_sort_index(errlength, &indexlength);

			int inde = errultIndex[0].I();
			for (int j = 0;j < errlength.Num();j++) {
				if (indexlength[j].I() <= errultIndex[0].I()) {
					iv->dispLine(badEdgeStRow[j].D(), badEdgeStCol[j].D() - 2,
						badEdgeStRow[j].D(), badEdgeStCol[j].D() + 2);
					iv->dispLine(badEdgeStRow[j].D() - 0.5, badEdgeStCol[j].D(),
						badEdgeStRow[j].D() + 2, badEdgeStCol[j].D());
					iv->dispLine(badEdgeEdRow[j].D(), badEdgeEdCol[j].D() - 2,
						badEdgeEdRow[j].D(), badEdgeEdCol[j].D() + 2);
					iv->dispLine(badEdgeEdRow[j].D() - 2, badEdgeEdCol[j].D(),
						badEdgeEdRow[j].D() + 2, badEdgeEdCol[j].D());
				}
			}
		}
	}

// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
// 	HTuple homMat2D;
// 	if (m_param->IsReferSection) {
// 		getDispOutData(TmpHomMat, homMat2D);
// 	}
// 	for (int i = 0;i < m_param->regionCount;i++) {
// 		HTuple thisok;
// 		getDispOutData(RegionOk, i, thisok);
// 		HTuple pointsok;
// 		if (m_param->IsValidPoints) {
// 			getDispOutData(PointsOK, i, pointsok);
// 		}
// 		else {
// 			pointsok[0] = 1;
// 		}
// 
// 		if (thisok[0].I() == 0 || pointsok == 0) {
// 			m_view->setColor(255, 0, 0);
// 		}
// 		else {
// 			m_view->setColor(0, 255, 0);
// 		}
// 		m_view->setFilled(false);
// 		char buf[32];
// 		sprintf(buf, "%d", i);
// 		if (m_param->IsReferSection) {
// 			if (m_param->IsShowAfterDet) {
// 				Hobject roirec;
// 				gen_rectangle2(&roirec, m_param->RoiRegionVector[i].row, m_param->RoiRegionVector[i].col,
// 					m_param->RoiRegionVector[i].angle, m_param->RoiRegionVector[i].length1, m_param->RoiRegionVector[i].length2);
// 				affine_trans_region(roirec, &roirec, homMat2D, "false");
// 				m_view->dispRegion(roirec, buf);
// 			}
// 		}
// 		else {
// 			if (m_param->IsShowAfterDet) {
// 				m_view->dispRect2(m_param->RoiRegionVector[i].row, m_param->RoiRegionVector[i].col,
// 					m_param->RoiRegionVector[i].angle, m_param->RoiRegionVector[i].length1, m_param->RoiRegionVector[i].length2, buf);
// 			}
// 		}
// 		if (pointsok[0].I() == false) {
// 			HTuple pointsnum;
// 			getDispOutData(PointsPer, i, pointsnum);
// 			double val = pointsnum[0].D();
// 			if (pointsnum[0].D() < 0.1) {
// 				continue;
// 			}
// 		}
// 
// 		HTuple EdgeStRow, EdgeEdRow, EdgeStCol, EdgeEdCol;
// 
// 		getDispOutData(EdgeStartRow, i, EdgeStRow);
// 		getDispOutData(EdgeEndRow, i, EdgeEdRow);
// 		getDispOutData(EdgeStartCol, i, EdgeStCol);
// 		getDispOutData(EdgeEndCol, i, EdgeEdCol);
// 		int edgenum = EdgeStRow.Num();
// 		m_view->setColor(0, 255, 0);
// 		for (int j = 0;j < edgenum;j++) {
// 			m_view->dispLine(EdgeStRow[j].D(), EdgeStCol[j].D() - 2,
// 				EdgeStRow[j].D(), EdgeStCol[j].D() + 2);
// 			m_view->dispLine(EdgeStRow[j].D() - 2, EdgeStCol[j].D(),
// 				EdgeStRow[j].D(), EdgeStCol[j].D());
// 			m_view->dispLine(EdgeEdRow[j].D(), EdgeEdCol[j].D() - 2,
// 				EdgeEdRow[j].D(), EdgeEdCol[j].D() + 2);
// 			m_view->dispLine(EdgeEdRow[j].D(), EdgeEdCol[j].D(),
// 				EdgeEdRow[j].D() + 2, EdgeEdCol[j].D());
// 		}
// 		if (thisok[0].I() == 0) {
// 			m_view->setColor(255, 0, 0);
// 		}
// 		else {
// 			m_view->setColor(0, 255, 0);
// 		}
// 		if (thisok[0].I() == 0) {
// 			HTuple errlength, errultIndex, badEdgeStRow, badEdgeEdRow, badEdgeStCol, badEdgeEdCol;
// 			getDispOutData(ErrLength, i, errlength);
// 			getDispOutData(ErrUltraIndex, i, errultIndex);
// 			getDispOutData(BadEdgeStartRow, i, badEdgeStRow);
// 			getDispOutData(BadEdgeEndRow, i, badEdgeEdRow);
// 			getDispOutData(BadEdgeStartCol, i, badEdgeStCol);
// 			getDispOutData(BadEdgeEndCol, i, badEdgeEdCol);
// 			int len = errlength.Num();
// 			HTuple sortlength, indexlength;
// 			tuple_sort(errlength, &sortlength);
// 			tuple_sort_index(errlength, &indexlength);
// 
// 			int inde = errultIndex[0].I();
// 			for (int j = 0;j < errlength.Num();j++) {
// 				if (indexlength[j].I() <= errultIndex[0].I()) {
// 					m_view->dispLine(badEdgeStRow[j].D(), badEdgeStCol[j].D() - 2,
// 						badEdgeStRow[j].D(), badEdgeStCol[j].D() + 2);
// 					m_view->dispLine(badEdgeStRow[j].D() - 0.5, badEdgeStCol[j].D(),
// 						badEdgeStRow[j].D() + 2, badEdgeStCol[j].D());
// 					m_view->dispLine(badEdgeEdRow[j].D(), badEdgeEdCol[j].D() - 2,
// 						badEdgeEdRow[j].D(), badEdgeEdCol[j].D() + 2);
// 					m_view->dispLine(badEdgeEdRow[j].D() - 2, badEdgeEdCol[j].D(),
// 						badEdgeEdRow[j].D() + 2, badEdgeEdCol[j].D());
// 				}
// 			}
// 		}
// 	}
}

void dimensionMeasureMod::textResult(ResultText *text, const QString &funName)
{
	if (p_im->empty())
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image!"));
		return;
	}
	if (m_param->IsReferSection&&p_homMat2d->size() != cv::Size(2, 3))
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Input matrix's size != 2*3!"));
		return;
	}
	for (int i = 0;i < m_param->regionCount;i++) {
		HTuple thisok;
		thisok[0] = m_regionOk[i];
		HTuple pointsok;
		if (m_param->IsValidPoints) {
			pointsok[0] = m_foundOk[i];
		}
		else {
			pointsok[0] = 1;
		}
		if (m_param->IsValidPoints) {
			double val = m_usePer[i];
			QString pointsres;
			if (pointsok[0].I() == 0) {
				text->setTextColor(QColor(255, 0, 0));
				pointsres = ("Effective point ratio NG");
			}
			else {
				text->setTextColor(QColor(0, 0, 0));
				pointsres = ("Effective point ratio OK");
			}
			text->append(QObject::tr("%1,ratio=%2%(%3%)")
				.arg(pointsres).arg(QString::number(val*100.0, 'g', 4)).arg(m_param->validPointsPer));
		}
		if (thisok[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
			HTuple errlength, errultIndex;
			errlength = m_errLength[i];
			errultIndex = m_errUltraIndex[i];
			//getDispOutData(ErrLength, i, errlength);
			//getDispOutData(ErrUltraIndex, i, errultIndex);
			HTuple sortlength, indexlength;
			tuple_sort(errlength, &sortlength);
			double ddf = sortlength[errultIndex[0].I()].D();
			text->append(QObject::tr("region%1 NG,max shift=%2").arg(i + 1)
				.arg(QString::number(sortlength[errultIndex[0].I()].D(), 'g', 6)));
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
			HTuple oklenmin, oklenmax;
			oklenmin[0] = m_lengthmin[i];
			oklenmax[0] = m_lengthmax[i];
			text->append(QObject::tr("region%1 OK,length=(%2,%3),normal range(%4,%5).").arg(i + 1)
				.arg(QString::number(oklenmin[0].D(), 'g', 6))
				.arg(QString::number(oklenmax[0].D(), 'g', 6))
				.arg(QString::number(m_param->lengthMinVec[i], 'g', 6))
				.arg(QString::number(m_param->lengthMaxVec[i], 'g', 6)));
		}
	}

// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
// 	for (int i = 0;i < m_param->regionCount;i++) {
// 		HTuple thisok;
// 		getDispOutData(RegionOk, i, thisok);
// 		HTuple pointsok;
// 		if (m_param->IsValidPoints) {
// 			getDispOutData(PointsOK, i, pointsok);
// 		}
// 		else {
// 			pointsok[0] = 1;
// 		}
// 		if (m_param->IsValidPoints) {
// 			HTuple pointsnum;
// 			getDispOutData(PointsPer, i, pointsnum);
// 			double val = pointsnum[0].D();
// 			QString pointsres;
// 			if (pointsok[0].I() == 0) {
// 				m_text->setTextColor(QColor(255, 0, 0));
// 				pointsres = ("Effective point ratio NG");
// 			}
// 			else {
// 				m_text->setTextColor(QColor(0, 0, 0));
// 				pointsres = ("Effective point ratio OK");
// 			}
// 			m_text->append(tr("%1,ratio=%2%(%3%)").arg(pointsres).arg(QString::number(val*100.0, 'g', 4)).arg(m_param->validPointsPer));
// 		}
// 		if (thisok[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 			HTuple errlength, errultIndex;
// 			getDispOutData(ErrLength, i, errlength);
// 			getDispOutData(ErrUltraIndex, i, errultIndex);
// 			HTuple sortlength, indexlength;
// 			tuple_sort(errlength, &sortlength);
// 			double ddf = sortlength[errultIndex[0].I()].D();
// 			m_text->append(tr("region%1 NG,max shift=%2").arg(i + 1)
// 				.arg(QString::number(sortlength[errultIndex[0].I()].D(), 'g', 6)));
// 		}
// 		else {
// 			m_text->setTextColor(QColor(0, 0, 0));
// 			HTuple oklenmin, oklenmax;
// 			getDispOutData(OkLengthMin, i, oklenmin);
// 			getDispOutData(OkLengthMax, i, oklenmax);
// 			m_view->setColor(0, 0, 0);
// 			m_text->append(tr("region%1 OK,length=(%2,%3),normal range(%4,%5).").arg(i + 1)
// 				.arg(QString::number(oklenmin[0].D(), 'g', 6)).arg(QString::number(oklenmax[0].D(), 'g', 6)).arg(QString::number(m_param->lengthMinVec[i], 'g', 6))
// 				.arg(QString::number(m_param->lengthMaxVec[i], 'g', 6)));
// 		}
// 	}
}

void dimensionMeasureMod::iniData()
{
	m_param = new dimensionMeasureParam;
	m_image = new Hobject;
	m_allok = 0;

	for (auto i = 0;i < Max_Roi_Num;++i)
	{
		m_foundOk[i] = true;
		m_usePer[i] = 0.0;
		m_errLength[i].Reset();
		m_errUltraIndex[i] = -1;
		m_regionOk[i] = true;
		m_lengthmax[i] = 0;
		m_lengthmin[i] = 0;

		m_StartRow[i].Reset();
		m_StartCol[i].Reset();
		m_EndRow[i].Reset();
		m_EndCol[i].Reset();

		m_errStartRow[i].Reset();
		m_errStartCol[i].Reset();
		m_errEndRow[i].Reset();
		m_errEndCol[i].Reset();
	}
	createPins();
}

void dimensionMeasureMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2d, "hm2d");
}

