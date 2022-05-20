#include "locationGuiMod.h"
#include "locationGuiParam.h"
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
#define ErrWidth "Width"
#define ErrHeightOk "HeightOk"
#define ErrHeight "Height"
#define ErrAngleOk "angleOK"
#define ErrAngle "angle" //最大偏转角1度
#define ErrCenterShiftOk "ErrcentershiftOK"
#define ErrCenterShift "Errcentershift" //与模板中心相对应的中心偏移1值

#define RegionOut "regionout"
//InData
//站高部分
#define StandOk "StandOk" //左边站高检测ok,数目不和  0:成功， 1：求出的管脚数目不对， 2，管脚宽度计算错误，3顶点计算错误

#define StandValOK "standValOk"          //左部分站高内的各1元素是否OK,比较误差范围后是否ok

#define StandLfRtPointRow "StandLeftRightPointRow"    //右区域站高的宽度边界点
#define StandLfRtPointCol "StandLeftRightPointCol"   
#define StandPointTopRow "standPointTopRow" //站高区域的顶点坐标
#define StandPointTopCol "standPointTopCol"

#define PinInterVal "pinInterval"  //左管脚间1距
#define PinWidth "pinWidth"        //管脚宽1度
#define StandOff "standOff"        //管脚站1高
#define StandOffDiff "standoffDiff"  //管脚站高差1距


locationGuiMod::locationGuiMod()
{
	iniData();
}

locationGuiMod::locationGuiMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

locationGuiMod::~locationGuiMod()
{
	delete m_image;
	delete m_param;
}

int locationGuiMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int locationGuiMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void locationGuiMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void locationGuiMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int locationGuiMod::run(const QString &funName)
{
	//预处理最好对m_image进行一下腐蚀处理
	bool resultOutOK = true;
	bool allok = true;
	*p_ResultOutOk = false;
	*p_StandOffEnable = m_param->errPinStandOffValid;
	*p_PinWidthEnable = m_param->errPinWidthValid;
	*p_PinInterValEnable = m_param->errPinGapValid;
	*p_StandOffDiffEnable = m_param->errPinStandOffDiffValid;

	p_StandOffOut->clear();
	p_PinWidthOut->clear();
	p_PinInterValOut->clear();
	p_StandOffDiffOut->clear();

	p_PinCountOut->clear();
	p_corners->clear();

	if (p_im->empty()) {
		allok = false;
		setDetectOutData(AllOK, allok);
		*p_ResultOutOk = false;
		//setDetectOutData(ResultOutOk, false);
		return allok;
	}
	HalOpenCV::cv2halImg(*p_im, m_image, false);

	toHtuple(p_pixelSize, m_k);
	toHtuple(p_LineAngle, m_lineAngle);
	toHtuple(p_prjRatio, m_prjRatio);
	toHtuple(p_ActualDepth, m_actualDepth);
	//getDetectInData(pixelSize, m_k);
	//getDetectInData(LineAngle, m_lineAngle);
	//getDetectInData(prjRatio, m_prjRatio);
	//getDetectInData(ActualDepth, m_actualDepth);
	bool roiFound[4]; //各边界是否找1到
	for (int i = 0; i < 4;i++) {
		roiFound[i] = true;
	}
	//up
	int roirow1[4], roicol1[4], roirow2[4], roicol2[4], searchDir[4];
	if (m_param->edgeSearchOrder == 0) {   //上下左右                         //先暂时不用左右上下
		for (int i = 0;i < 4;i++) {
			roirow1[i] = m_param->RoiRegionVector[i + 1].row1;
			roirow2[i] = m_param->RoiRegionVector[i + 1].row2;
			roicol1[i] = m_param->RoiRegionVector[i + 1].col1;
			roicol2[i] = m_param->RoiRegionVector[i + 1].col2;
			searchDir[i] = m_param->searchDir[i];
		}
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
		if ((!m_param->UpDetect&&i == 0) || (!m_param->DwDetect&&i == 1) || (!m_param->LfDetect&&i == 2) || (!m_param->RtDetect&&i == 3))
			continue;
		HTuple rowEdge, colEdge, amp, distance;
		char edge[32];
		if (m_param->whiteBlackDir[i]) //1白黑
			sprintf(edge, "negative");
		else
			sprintf(edge, "positive");
		if (searchDir[i] == 1) {  //上下
			gen_measure_rectangle2((roirow1[i] + roirow2[i]) / 2, roicol1[i]
				, -PI / 2.0, (roirow2[i] - roirow1[i]) / 2, m_param->extendSearchLine[i] * 2, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measureHandle);
		}
		else if (searchDir[i] == 0) { //左右
			gen_measure_rectangle2(roirow1[i], (roicol1[i] + roicol2[i]) / 2
				, 0, (roicol2[i] - roicol1[i]) / 2, m_param->extendSearchLine[i] * 2, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measureHandle);
		}
		int pointN = 0;   //累计进行测试的点1数
		int usefulPoint = 0;  // 累1计有效得出结果的点数(结1果不为空)
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
				if (m_param->IsPaichuLine[i]) {
					if ((startPoint < ((roicol1[i] + edPoint) / 2 + m_param->paichuLine[i] / 2)) && (startPoint > ((roicol1[i] + edPoint) / 2 - m_param->paichuLine[i] / 2)))
						continue;
				}
				if (pointN != 0)
					translate_measure(measureHandle, (roirow1[i] + roirow2[i]) / 2, startPoint);
				measure_pos(*m_image, measureHandle, 1, m_param->minGrayDiff[i], edge, "first", &rowEdge, &colEdge, &amp, &distance);
				Hlong aa = rowEdge.Num();
				if (aa > 0) {
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
				if (m_param->IsPaichuLine[i]) {
					if ((startPoint < ((roirow1[i] + edPoint) / 2 + m_param->paichuLine[i] / 2)) && (startPoint > ((roirow1[i] + edPoint) / 2 - m_param->paichuLine[i] / 2)))
						continue;
				}
				if (pointN != 0)
					translate_measure(measureHandle, startPoint, (roicol1[i] + roicol2[i]) / 2);
				measure_pos(*m_image, measureHandle, 1, m_param->minGrayDiff[i], edge, "first", &rowEdge, &colEdge, &amp, &distance);
				Hlong aa = rowEdge.Num();
				if (aa > 0) {
					m_rowEdge[i][usefulPoint] = rowEdge[0];
					m_colEdge[i][usefulPoint] = colEdge[0];
					usefulPoint++;
				}
				pointN++;
			}
		} //采集边界点结1束
		Hlong ab = m_rowEdge[i].Num();
		HTuple nc, nr, dist;
		if (ab >= 5) {
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
				setDetectOutData(FindOk, 1, i);  //边界拟合失败
			}
			else {
				RowBegin11[i][0] = RowBegin[i][0];
				RowEnd11[i][0] = RowEnd[i][0];
				ColBegin11[i][0] = ColBegin[i][0];
				ColEnd11[i][0] = ColEnd[i][0];
				setDetectOutData(SearchBeginRow, RowBegin[i], i);   //得到的实际边线的起始点
				setDetectOutData(SearchBeginCol, ColBegin[i], i);
				setDetectOutData(SearchEndRow, RowEnd[i], i);
				setDetectOutData(SearchEndCol, ColEnd[i], i);
				if (m_param->edgeSearchOrder == 0) { //上下左1右
					if (i == 0 || i == 1) {
						RowBegin[i][0] = m_param->lineOffset[i] + RowBegin[i][0].D();
						RowEnd[i][0] = m_param->lineOffset[i] + RowEnd[i][0].D();
					}
					else if (i == 2 || i == 3) {
						ColBegin[i][0] = m_param->lineOffset[i] + ColBegin[i][0].D();
						ColEnd[i][0] = m_param->lineOffset[i] + ColEnd[i][0].D();
					}
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
			setDetectOutData(FindOk, 2, i); //边界点过少 2
		}
		close_measure(measureHandle);
	} //for loop end
	HTuple rows, cols;
	long totalNum = 0;
	for (int i = 0;i < 4;i++) {
		if ((i == 0 && m_param->UpDetect) || (i == 1 && m_param->DwDetect) || (i == 2 && m_param->LfDetect)
			|| (i == 3 && m_param->RtDetect)) {
			if (roiFound[i]) {
				double Rowval, Colval;
				int num = m_rowEdge[i].Num();
				for (int j = 0;j < num;j++) {
					rows[totalNum] = m_rowEdge[i][j];
					cols[totalNum] = m_colEdge[i][j];
					totalNum++;
				}
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
			*p_ResultOutOk = false;
			//setDetectOutData(ResultOutOk, false);
			return allok;
		}
	}
	setDetectOutData(MaxAngleOK, true);
	bool endpointFound = true;
	for (int i = 0;i < 4;i++) {
		if (!roiFound[i]) {
			allok = false;
			setDetectOutData(AllOK, allok);
			*p_ResultOutOk = false;
			//setDetectOutData(ResultOutOk, false);
			return allok;
		}
	}
	if (m_param->IsEndPoint) {
		for (int i = 0;i < 4;i++) {
			Hlong measureHandleEndPoint;
			HTuple rowEdge, colEdge, amp, distance;
			float anglehere;
			if (i == 0 || i == 1) {
				anglehere = EveryAngle[i] + PI;
			}
			else if (i == 2 || i == 3) {
				anglehere = EveryAngle[i];
			}
			gen_measure_rectangle2(RowBegin[i][0], ColBegin[i][0]
				, anglehere, m_param->edgesearchWidth[i], 5, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measureHandleEndPoint);
			measure_pos(*m_image, measureHandleEndPoint, 1, 40/*m_param->minGrayDiff[i]*/, "positive", "first", &rowEdge, &colEdge, &amp, &distance);
			Hlong length = rowEdge.Num();
			if (length == 0) {
				close_measure(measureHandleEndPoint);
				endpointFound = false;
				allok = false;
				setDetectOutData(EndPointOk, 0);
				setDetectOutData(EndNGIndex, i);
				setDetectOutData(AllOK, allok);
				*p_ResultOutOk = false;
				//setDetectOutData(ResultOutOk, false);
				return allok;
			}
			RowBegin[i][0] = rowEdge[0];
			ColBegin[i][0] = colEdge[0];
			if (i == 0 || i == 1) {
				close_measure(measureHandleEndPoint);
				gen_measure_rectangle2(RowEnd[i][0], ColEnd[i][0]
					, EveryAngle[i], m_param->edgesearchWidth[i], 5, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measureHandleEndPoint);
				measure_pos(*m_image, measureHandleEndPoint, 1, 40/*m_param->minGrayDiff[i]*/, "positive", "first", &rowEdge, &colEdge, &amp, &distance);
			}
			else if (i == 2 || i == 3) {
				translate_measure(measureHandleEndPoint, RowEnd[i][0], ColEnd[i][0]);
				measure_pos(*m_image, measureHandleEndPoint, 1, 40/*m_param->minGrayDiff[i]*/, "negative", "first", &rowEdge, &colEdge, &amp, &distance);
			}
			length = rowEdge.Num();
			if (length == 0) {
				close_measure(measureHandleEndPoint);
				endpointFound = false;
				allok = false;
				setDetectOutData(EndPointOk, endpointFound);
				setDetectOutData(EndNGIndex, i);
				setDetectOutData(AllOK, allok);
				*p_ResultOutOk = false;
				//setDetectOutData(ResultOutOk, false);
				return allok;
			}
			RowEnd[i][0] = rowEdge[0];
			ColEnd[i][0] = colEdge[0];
			close_measure(measureHandleEndPoint);
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
			int a = 0;
			if (m_param->edgeSearchOrder == 1) {  //左1右上下的顺序,实际上后续计算全部以上下左右顺1序
				a = i > 1 ? (i - 2) : (2 + i);
			}
			else
				a = i;
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
		center = GetLineIntersection(line1, line2);
		setDetectOutData(CenterRow, center->row);
		setDetectOutData(CenterCol, center->col);
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
			tmpInter = GetLineIntersection(edgeLine[aaa], edgeLine[bbb]);
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
		center = GetLineIntersection(line1, line2);
		setDetectOutData(CenterRow, center->row);
		setDetectOutData(CenterCol, center->col);
	}
	HTuple fnalRow, fnalCol;
	for (int i = 0;i < 4;i++) {
		fnalRow[i] = finalPointRow[i];
		fnalCol[i] = finalPointCol[i];
	}
	fnalRow[4] = fnalRow[0].D();
	fnalCol[4] = fnalCol[0].D();
	for (auto i = 0;i < 5;++i)
	{
		double x = fnalCol[i].D();
		double y = fnalRow[i].D();
		p_corners->push_back(cv::Point2d(x, y));
	}
	//setDetectOutData(FinalRow, fnalRow);  //左上角顺时针
	//setDetectOutData(FinalCol, fnalCol);
	//求站高部分,顺序上下左右
	HTuple pinLfrtRow, pinLfrtCol;  //左区域站高部分得出的左右点。是数组
	HTuple pinTopRow, pinTopCol; //站高部分得出的顶点。是数组
	HTuple InterRow, InterCol;  //右边两根线交点

	HTuple OutStandoffdiff[4];
	HTuple OutStandoff[4], OutPinwidth[4], OutPininterval[4];  //用于传出的管脚的一些参数

	for (int i = 0;i < 4;i++) {
		if (m_param->upDwOrLfRt == 0 && (i == 2 || i == 3))
			continue;
		else if (m_param->upDwOrLfRt == 1 && (i == 0 || i == 1))
			continue;
		if (!m_param->IsUseStand[i]) {
			continue;
		}
		Line line1, line2;
		HTuple searchbeginrow, searchbegincol, searchendrow, searchendcol;
		Line edgeline;
		if (i == 0 || i == 1) {
			line1.Start.row = roirow1[i] - m_param->pinSearchWidth[i];
			line1.Start.col = roicol1[i] - m_param->pinSearchWidth[i];
			line1.End.row = roirow2[i] + m_param->pinSearchWidth[i];
			line1.End.col = roicol1[i] - m_param->pinSearchWidth[i];
			line2.Start.row = roirow1[i] - m_param->pinSearchWidth[i];
			line2.Start.col = roicol2[i] + m_param->pinSearchWidth[i];
			line2.End.row = roirow2[i] + m_param->pinSearchWidth[i];
			line2.End.col = roicol2[i] + m_param->pinSearchWidth[i];
		}
		else if (i == 2 || i == 3) {
			line1.Start.row = roirow1[i] - m_param->pinSearchWidth[i];
			line1.Start.col = roicol1[i] - m_param->pinSearchWidth[i];
			line1.End.row = roirow1[i] - m_param->pinSearchWidth[i];
			line1.End.col = roicol2[i] + m_param->pinSearchWidth[i];
			line2.Start.row = roirow2[i] + m_param->pinSearchWidth[i];
			line2.Start.col = roicol1[i] - m_param->pinSearchWidth[i];
			line2.End.row = roirow2[i] + m_param->pinSearchWidth[i];
			line2.End.col = roicol2[i] - m_param->pinSearchWidth[i];
		}
		searchbeginrow[0] = RowBegin11[i][0];
		searchbegincol[0] = ColBegin11[i][0];
		searchendrow[0] = RowEnd11[i][0];
		searchendcol[0] = ColEnd11[i][0];


		edgeline.Start.row = searchbeginrow[0].D();
		edgeline.Start.col = searchbegincol[0].D();
		edgeline.End.row = searchendrow[0].D();
		edgeline.End.col = searchendcol[0].D();
		FloatPoint* toppoint = new FloatPoint;//上/左交点
		FloatPoint*	btmpoint = new FloatPoint;//下/右交点
		toppoint = GetLineIntersection(line1, edgeline);
		btmpoint = GetLineIntersection(line2, edgeline);
		Hobject polyregion;   //多边形区域
		HTuple polyrow, polycol;
		if (i == 0) {
			polyrow[0] = roirow1[i] - m_param->pinSearchWidth[i];
			polycol[0] = roicol1[i] - m_param->pinSearchWidth[i];
			polyrow[1] = roirow1[i] - m_param->pinSearchWidth[i];
			polycol[1] = roicol2[i] + m_param->pinSearchWidth[i];
			polyrow[2] = btmpoint->row;
			polycol[2] = btmpoint->col;
			polyrow[3] = toppoint->row;
			polycol[3] = toppoint->col;
		}
		else if (i == 1) {
			polyrow[3] = roirow2[i] + m_param->pinSearchWidth[i];
			polycol[3] = roicol1[i] - m_param->pinSearchWidth[i];
			polyrow[2] = roirow2[i] + m_param->pinSearchWidth[i];
			polycol[2] = roicol2[i] + m_param->pinSearchWidth[i];
			polyrow[1] = btmpoint->row;
			polycol[1] = btmpoint->col;
			polyrow[0] = toppoint->row;
			polycol[0] = toppoint->col;
		}
		else if (i == 2) { //左区域
			polyrow[0] = roirow1[i] - m_param->pinSearchWidth[i];
			polycol[0] = roicol1[i] - m_param->pinSearchWidth[i];
			polyrow[1] = roirow2[i] + m_param->pinSearchWidth[i];
			polycol[1] = roicol1[i] - m_param->pinSearchWidth[i];
			polyrow[2] = btmpoint->row;
			polycol[2] = btmpoint->col;
			polyrow[3] = toppoint->row;
			polycol[3] = toppoint->col;
		}
		else if (i == 3) {
			polyrow[3] = roirow1[i] - m_param->pinSearchWidth[i];
			polycol[3] = roicol2[i] + m_param->pinSearchWidth[i];
			polyrow[2] = roirow2[i] + m_param->pinSearchWidth[i];
			polycol[2] = roicol2[i] + m_param->pinSearchWidth[i];
			polyrow[1] = btmpoint->row;
			polycol[1] = btmpoint->col;
			polyrow[0] = toppoint->row;
			polycol[0] = toppoint->col;
		}
		polyrow[4] = polyrow[0];
		polycol[4] = polycol[0];
		gen_region_polygon_filled(&polyregion, polyrow, polycol);
		Hobject polyImg;
		reduce_domain(*m_image, polyregion, &polyImg);
		invert_image(polyImg, &polyImg);
		threshold(polyImg, &polyImg, m_param->grayVal[i], 255);
		opening_rectangle1(polyImg, &polyImg, 3, 3);   //防止边界处等地方的因为分割的问题出现的长条丝状对求取外接矩形造成影响
		connection(polyImg, &polyImg);
		select_shape(polyImg, &polyImg, "area", "and", m_param->minArea[i], 10000000);
		Hlong polynum;
		count_obj(polyImg, &polynum);
		setDetectOutObject(RegionOut, polyImg, i);
		HTuple srow1, srow2, scol1, scol2;  //求取用于后面检测管脚站高点，检测管脚宽度边界时gen_measure_rectangle1的length1长度
		HTuple sWid, sHei; //求出来的长宽值
		smallest_rectangle1(polyImg, &srow1, &scol1, &srow2, &scol2);
		for (int ss = 0;ss < srow1.Num();ss++) {
			sHei[ss] = srow2[ss].D() - srow1[ss].D();
			sWid[ss] = scol2[ss].D() - scol1[ss].D();
		}
		if (polynum != m_param->grpPinNum[(i + 1) % 2 == 1 ? 0 : 1]) {
			setDetectOutData(StandOk, 1, i);
			resultOutOK = false;
			allok = false;
			delete toppoint;
			delete btmpoint;
			continue;
		}
		else {
			setDetectOutData(StandOk, 0, i);
		}

		HTuple polyarea, polycenterrow, polycentercol;   //求切出来的区域的中心点坐标
		HTuple detectRow, detectCol;
		area_center(polyImg, &polyarea, &polycenterrow, &polycentercol);

		HTuple polycenterrow1, polycentercol1;     //将得到的区域按照一定的顺序排序，防止求间距的时候错误
		HTuple sortIndex;
		if (m_param->upDwOrLfRt == 1) {   //左右站高
			tuple_sort_index(polycenterrow, &sortIndex);
			tuple_sort(polycenterrow, &polycenterrow1);
			for (int r = 0;r < polycenterrow.Num();r++) {
				polycentercol1[sortIndex[r].I()] = polycentercol[r];
			}
			polycenterrow = polycenterrow1;
			polycentercol = polycentercol1;
		}
		else if (m_param->upDwOrLfRt == 0) {  //上下站高
			tuple_sort_index(polycentercol, &sortIndex);
			tuple_sort(polycentercol, &polycentercol1);
			for (int r = 0;r < polycentercol.Num();r++) {
				polycenterrow1[sortIndex[r].I()] = polycenterrow[r];
			}
			polycenterrow = polycenterrow1;
			polycentercol = polycentercol1;
		}
		double porow[20];
		double pocol[20];
		for (int r = 0;r < polycenterrow.Num();r++) {
			porow[r] = polycenterrow[r].D();
			pocol[r] = polycentercol[r].D();
		}
		double standoffmax = 0;  //用于求站高的落差
		double standoffmin = 1000000;
		bool iscontinueJloop = true;

		int grpIndex;  //误差范围参数所使用的组数
		if (i == 0 || i == 2) {
			grpIndex = 0;
		}
		else if (i == 1 || i == 3) {
			grpIndex = 1;
		}
		for (int j = 0;j < polynum;j++) {
			double Kangle; //垂直边界直线斜率对应的角度
			FloatPoint* interpoint = new FloatPoint;
			if (i == 0 || i == 1) {
				if (abs(EveryAngle[i]) < 0.00000001) {
					detectCol[j] = polycentercol[j];
					detectRow[j] = searchendrow[0].D() + (i == 0 ? (-1) : 1)*(double)m_param->standoffOffset[i];
					InterRow[j] = searchendrow[0].D();
					InterCol[j] = polycentercol[j].D();
					Kangle = 0;
				}
				else {
					double k11;  //垂直边界线的直线斜率；
					k11 = -1.0 / tan(EveryAngle[i]);
					double b11; //y=kx+b
					b11 = polycenterrow[j].D() - k11*polycentercol[j].D();
					HTuple tmppointrow, tmppointcol;   //根据上面求出来的线求另一个点，用于后面的四点求交点
					tmppointrow[0] = polycenterrow[j].D() + 50.0;
					tmppointcol[0] = (tmppointrow[0].D() - b11) / k11;

					Line chuizhiline;
					chuizhiline.Start.row = tmppointrow[0].D();
					chuizhiline.Start.col = tmppointcol[0].D();
					chuizhiline.End.row = polycenterrow[j].D();
					chuizhiline.End.col = polycentercol[j].D();
					interpoint = GetLineIntersection(chuizhiline, edgeline);
					Kangle = atan(k11);
					while (Kangle < 0) {
						Kangle += PI;
					}
					detectCol[j] = interpoint->col + (i == 0 ? 1 : (-1))*(cos(Kangle))*(double)m_param->standoffOffset[i];
					detectRow[j] = interpoint->row + (i == 0 ? (-1) : (1))*(sin(Kangle))*(double)m_param->standoffOffset[i];
					InterCol[j] = interpoint->col;
					InterRow[j] = interpoint->row;
				}
			}
			else if (i == 2 || i == 3) {
				if (abs(EveryAngle[i] - PI / 2.0) < 0.00000001) {
					detectRow[j] = polycenterrow[j];
					detectCol[j] = searchendcol[0].D() + (i == 2 ? (-1) : 1)*(double)m_param->standoffOffset[i];
					InterCol[j] = searchendcol[0].D();
					InterRow[j] = polycenterrow[j].D();
					Kangle = 0;
				}
				else {
					double k11;  //垂直边界线的直线斜率；
					k11 = -1.0 / tan(EveryAngle[i]);
					double b11; //y=kx+b
					b11 = polycenterrow[j].D() - k11*polycentercol[j].D();
					HTuple tmppointrow, tmppointcol;   //根据上面求出来的线求另一个点，用于后面的四点求交点
					tmppointcol[0] = polycentercol[j].D() + 50.0;
					tmppointrow[0] = k11*tmppointcol[0].D() + b11;

					Line chuizhiline;
					chuizhiline.Start.row = tmppointrow[0].D();
					chuizhiline.Start.col = tmppointcol[0].D();
					chuizhiline.End.row = polycenterrow[j].D();
					chuizhiline.End.col = polycentercol[j].D();
					interpoint = GetLineIntersection(chuizhiline, edgeline);
					Kangle = atan(k11);
					detectCol[j] = interpoint->col + (i == 2 ? (-1) : 1)*cos(Kangle)*(double)m_param->standoffOffset[i];
					detectRow[j] = interpoint->row + (i == 2 ? 1 : (-1))*sin(Kangle)*(double)m_param->standoffOffset[i];
					InterCol[j] = interpoint->col;
					InterRow[j] = interpoint->row;
				}
			}
			Hlong measurestand;
			HTuple rowEdge1, colEdge1, amp1, distance1/*,distance11,distance12*/;
			int widlen1, toplen1;   //检测管脚宽度和站高点中gen_measure_rectangle2的length1
			if (i == 0 || i == 1) {
				widlen1 = sWid[j].I();
				toplen1 = sHei[j].I();
			}
			else {
				widlen1 = sHei[j].I();
				toplen1 = sWid[j].I();
			}
			gen_measure_rectangle2(detectRow[j].D(), detectCol[j].D()
				, EveryAngle[i], widlen1, 5, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measurestand);
			measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], "all", "all", &rowEdge1, &colEdge1, &amp1, &distance1);
			Hlong num1 = rowEdge1.Num();
			if (num1 < 2) {
				setDetectOutData(StandOk, 2, i);
				resultOutOK = false;
				allok = false;
				delete interpoint;
				iscontinueJloop = false;
				close_measure(measurestand);
				break;
			}
			close_measure(measurestand);
			pinLfrtRow[2 * j] = rowEdge1[0];
			pinLfrtCol[2 * j] = colEdge1[0];
			pinLfrtRow[2 * j + 1] = rowEdge1[1];
			pinLfrtCol[2 * j + 1] = colEdge1[1];
			//求顶点
			HTuple rowedge2, rowedge21, rowedge22, coledge2, coledge21, coledge22, amp2, distance2, distance21, distance22;
			char edgeBuf[64];
			if (i == 0 || i == 3) {
				sprintf(edgeBuf, "positive");
			}
			else if (i == 1 || i == 2) {
				sprintf(edgeBuf, "negative");
			}
			if (i == 2 || i == 3) {
				gen_measure_rectangle2(detectRow[j].D() - 3, detectCol[j], Kangle, toplen1, 5, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measurestand);
				measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], edgeBuf, "first", &rowedge21, &coledge21, &amp2, &distance21);
				translate_measure(measurestand, detectRow[j].D() + 3, detectCol[j]);
				measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], edgeBuf, "first", &rowedge22, &coledge22, &amp2, &distance22);
				translate_measure(measurestand, detectRow[j], detectCol[j]);
				measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], edgeBuf, "first", &rowedge2, &coledge2, &amp2, &distance2);
			}
			else if (i == 0 || i == 1) {
				gen_measure_rectangle2(detectRow[j].D(), detectCol[j].D() - 3, Kangle, toplen1, 5, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measurestand);
				measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], edgeBuf, "first", &rowedge21, &coledge21, &amp2, &distance21);
				translate_measure(measurestand, detectRow[j].D(), detectCol[j].D() + 3);
				measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], edgeBuf, "first", &rowedge22, &coledge22, &amp2, &distance22);
				translate_measure(measurestand, detectRow[j], detectCol[j]);
				measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], edgeBuf, "first", &rowedge2, &coledge2, &amp2, &distance2);
			}
			num1 = rowedge2.Num();
			if (num1 < 1) {
				close_measure(measurestand);
				setDetectOutData(StandOk, 3, i);
				resultOutOK = false;
				allok = false;
				delete interpoint;
				iscontinueJloop = false;
				break;
			}
			pinTopRow[j] = rowedge2[0];
			pinTopCol[j] = coledge2[0];
			close_measure(measurestand);
			double pinstand;
			pinstand = sqrt(pow(interpoint->row - rowedge2[0].D(), 2) + pow(interpoint->col - coledge2[0].D(), 2));

			double ratioangle; //投影的实际角度
			double prjratioo = asin(m_prjRatio[i < 2 ? (i + 2) : (i - 2)/*i-2*/].D());
			double lineanglee = m_lineAngle[i].D();
			double minusaangle = EveryAngle[i < 2 ? (i + 2) : (i - 2)/*i-2*/] - lineanglee;
			if (i < 2)
			{  //shangxia
				ratioangle = prjratioo - (EveryAngle[i < 2 ? (i + 2) : (i - 2)] - lineanglee);   //lineAngle 顺序左右上下,everyAngle 上下左右
			}
			else
			{ //zuoyou
				ratioangle = prjratioo + (EveryAngle[i < 2 ? (i + 2) : (i - 2)] - lineanglee);   //lineAngle 顺序左右上下,everyAngle 上下左右
			}
			//ratioangle=prjratioo- ( EveryAngle[0/*i-2*/] -lineanglee );   //lineAngle 顺序左右上下,everyAngle 上下左右
			double deopth = m_actualDepth[i < 2 ? (i + 2) : (i - 2)].D();
			double sdsds = m_k[i < 2 ? (i + 3) : (i - 1)].D();
			double standbefore = pinstand;
			double kstand = pinstand*m_k[i < 2 ? (i + 3) : (i - 1)].D();

			pinstand = pinstand*m_k[i < 2 ? (i + 3) : (i - 1)].D() - m_param->pinlength*sin(ratioangle);   //减去标定中的间隙，并且乘上实际的投影比例

			pinstand += m_param->standOffCorr[(i + 1) % 2 == 1 ? 0 : 1];

			bool roiOK = true;

			if (m_param->errPinWidthValid)
			{
				distance1[0] = m_k[i < 2 ? (i + 3) : (i - 1)].D()*distance1[0].D();
				distance1[0] = m_param->widthCorr[(i + 1) % 2 == 1 ? 0 : 1] + distance1[0].D();
				if (distance1[0].D() > m_param->errPinWidthMax[grpIndex][j] || distance1[0].D() < m_param->errPinWidthMin[grpIndex][j]) {
					allok = false;
					roiOK = false;
				}
				setDetectOutData(PinWidth, distance1[0].D(), i, j);
				OutPinwidth[i][j] = distance1[0].D();
			}
			if (m_param->errPinStandOffValid)
			{
				if (pinstand > m_param->errPinStandOffMax[grpIndex][j] || pinstand < m_param->errPinStandOffMin[grpIndex][j]) {
					allok = false;
					roiOK = false;
				}
				setDetectOutData(StandOff, pinstand, i, j);
				OutStandoff[i][j] = pinstand;
			}
			if (m_param->errPinGapValid &&j > 0)
			{
				double gap = 0;
				gap = abs(sqrt(pow((InterRow[j].D() - InterRow[j - 1].D()), 2) + pow((InterCol[j].D() - InterCol[j - 1].D()), 2)));
				gap *= m_k[i < 2 ? (i + 3) : (i - 1)].D();
				gap += m_param->InterCorr[(i + 1) % 2 == 1 ? 0 : 1];
				setDetectOutData(PinInterVal, gap, i, j - 1);
				if (gap > m_param->errPinGapMax[grpIndex][j - 1] || gap < m_param->errPinGapMin[grpIndex][j - 1]) {
					allok = false;
				}
				OutPininterval[i][j - 1] = gap;
			}
			if (m_param->errPinStandOffDiffValid)
			{
				if (standoffmax < pinstand) {
					standoffmax = pinstand;
				}
				if (standoffmin > pinstand) {
					standoffmin = pinstand;
				}
				if (j == polynum - 1) {
					if ((standoffmax - standoffmin) > m_param->errPinStandOffDiffMax[grpIndex]) {
						allok = false;
					}
					setDetectOutData(StandOffDiff, standoffmax - standoffmin, i);
					OutStandoffdiff[i] = standoffmax - standoffmin;
				}
			}
			setDetectOutData(StandValOK, roiOK, i, j);
			delete interpoint;
		}
		delete toppoint;
		delete btmpoint;
		if (!iscontinueJloop) {
			continue;
		}
		setDetectOutData(StandLfRtPointRow, pinLfrtRow, i);
		setDetectOutData(StandLfRtPointCol, pinLfrtCol, i);
		setDetectOutData(StandPointTopRow, pinTopRow, i);
		setDetectOutData(StandPointTopCol, pinTopCol, i);
	}
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
		}
		setDetectOutData(ErrAngleOk, errAngleOk);
		setDetectOutData(ErrAngle, maxangle / PI * 180);
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
			allok = false;
		}
		setDetectOutData(ErrWidthOk, errwidthok);
		setDetectOutData(ErrWidth, maxwidth);
	}
	if (m_param->IsErrHeight)
	{
		bool errheightok = true;
		float maxheight = 0;
		float minheight = 10000;
		float dis;
		if (m_param->IsEndPoint)
		{
			dis = sqrt((finalPointCol[3] - finalPointCol[0])*(finalPointCol[3] - finalPointCol[0]) +
				(finalPointRow[3] - finalPointRow[0])*(finalPointRow[3] - finalPointRow[0]));
			if (maxheight < dis)
				maxheight = dis;
			if (minheight > dis)
				minheight = dis;
			dis = sqrt((finalPointCol[2] - finalPointCol[1])*(finalPointCol[2] - finalPointCol[1]) +
				(finalPointRow[2] - finalPointRow[1])*(finalPointRow[2] - finalPointRow[1]));
			if (maxheight < dis)
				maxheight = dis;
			if (minheight > dis)
				minheight = dis;
		}
		else {
			dis = sqrt((RowBegin[2][0].D() - RowEnd[2][0].D())*(RowBegin[2][0].D() - RowEnd[2][0].D()) +
				(ColBegin[2][0].D() - ColEnd[2][0].D())*(ColBegin[2][0].D() - ColEnd[2][0].D()));
			if (maxheight < dis)
				maxheight = dis;
			if (minheight > dis)
				minheight = dis;
			dis = sqrt((RowBegin[3][0].D() - RowEnd[3][0].D())*(RowBegin[3][0].D() - RowEnd[3][0].D()) +
				(ColBegin[3][0].D() - ColEnd[3][0].D())*(ColBegin[3][0].D() - ColEnd[3][0].D()));
			if (maxheight < dis)
				maxheight = dis;
			if (minheight > dis)
				minheight = dis;
		}
		if (maxheight > m_param->errHeightmax || minheight < m_param->errHeightMin) {
			errheightok = false;
			allok = false;
		}
		setDetectOutData(ErrHeightOk, errheightok);
		setDetectOutData(ErrHeight, maxheight);
	}
	if (m_param->IsErrCenterOffset)
	{
		float Centershift = 0;
		bool centerok = true;
		Centershift = sqrt(pow((center->row - m_param->TmpCenterRow), 2) + pow((center->col - m_param->TmpCenterCol), 2));
		if (Centershift > m_param->errCenterOffset) {
			allok = false;
			centerok = false;
		}
		setDetectOutData(ErrCenterShift, Centershift);
		setDetectOutData(ErrCenterShiftOk, centerok);
	}
	delete center;
	setDetectOutData(AllOK, allok);
	*p_ResultOutOk = resultOutOK;
	//setDetectOutData(ResultOutOk, resultOutOK);
	if (resultOutOK) {
		int index0, index1;
		if (m_param->upDwOrLfRt == 0) {
			index0 = 0;
			index1 = 1;
		}
		else if (m_param->upDwOrLfRt == 1) {
			index0 = 2;
			index1 = 3;
		}
		if (m_param->errPinStandOffDiffValid) {
			HTuple diffout;
			if (m_param->grpPinNum[0] < 2) {
				OutStandoffdiff[index0] = 0;
			}
			if (m_param->grpPinNum[1] < 2) {
				OutStandoffdiff[index1] = 0;
			}
			tuple_concat(OutStandoffdiff[index0], OutStandoffdiff[index1], &diffout);
			toQList(diffout, p_StandOffDiffOut);
			//setDetectOutData(StandOffDiffOut, diffout);
		}
		if (m_param->errPinStandOffValid) {
			HTuple offout;
			tuple_concat(OutStandoff[index0], OutStandoff[index1], &offout);
			toQList(offout, p_StandOffOut);
			//setDetectOutData(StandOffOut, offout);
		}
		if (m_param->errPinWidthValid) {
			HTuple widout;
			tuple_concat(OutPinwidth[index0], OutPinwidth[index1], &widout);
			toQList(widout, p_PinWidthOut);
			//setDetectOutData(PinWidthOut, widout);
		}
		if (m_param->errPinGapValid) {
			HTuple interout;
			tuple_concat(OutPininterval[index0], OutPininterval[index1], &interout);
			toQList(interout, p_PinInterValOut);
			//setDetectOutData(PinInterValOut, interout);
		}
		p_PinCountOut->push_back(m_param->grpPinNum[0]);
		p_PinCountOut->push_back(m_param->grpPinNum[1]);
		//放在最前面
		//setDetectOutData(StandOffEnable, m_param->errPinStandOffValid);
		//setDetectOutData(PinWidthEnable, m_param->errPinWidthValid);
		//setDetectOutData(PinInterValEnable, m_param->errPinGapValid);
		//setDetectOutData(StandOffDiffEnable, m_param->errPinStandOffDiffValid);
		//HTuple pincount;
		//pincount[0] = m_param->grpPinNum[0];
		//pincount[1] = m_param->grpPinNum[1];
		//setDetectOutData(PinCountOut, pincount);
	}
	return allok;
}

void locationGuiMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (p_im->empty())
		return;

	HTuple imgok;
	getDispOutData(DispPointsImgOk, imgok);
	iv->setFilled(true);
	iv->setColor(255, 0, 0);
	if (imgok[0].I() == 1) {
		iv->setColor(0, 255, 0);
		Hlong num;
		const Hobject *tmpReg = getDispObject(DispPointsImg);
		count_obj(*tmpReg, &num);
		if (num > 0) {
			Hobject dispReg;
			union1(*tmpReg, &dispReg);
			RlRegion reg;
			HalOpenCV::hal2cvRegion(dispReg, &reg);
			iv->dispRegion(&reg);
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
	for (int i = 0;i < 4;i++) {
		if (m_param->upDwOrLfRt == 0 && (i == 2 || i == 3)) {
			continue;
		}
		else if (m_param->upDwOrLfRt == 1 && (i == 0 || i == 1)) {
			continue;
		}
		if (!m_param->IsUseStand[i]) {
			continue;
		}
		getDispOutData(StandOk, i, standok0);
		if (standok0[0].I() != 0) {
			iv->setColor(255, 0, 0);
			iv->setFilled(false);
			iv->dispRect1(
				m_param->RoiRegionVector[i + 1].row1, m_param->RoiRegionVector[i + 1].col1,
				m_param->RoiRegionVector[i + 1].row2, m_param->RoiRegionVector[i + 1].col2);
			iv->setFilled(true);
			RlRegion reg;
			HalOpenCV::hal2cvRegion(*getDispObject(RegionOut, i), &reg);
			iv->dispRegion(&reg);
			return;
		}
	}

	HTuple allok;
	getDispOutData(AllOK, allok);
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
	RlRegion reg;
	HalOpenCV::hal2cvRegion(polyRec, &reg);
	iv->dispRegion(&reg);
	HTuple centerRow, centerCol;
	getDispOutData(CenterRow, centerRow);
	getDispOutData(CenterCol, centerCol);
	iv->dispLine(centerRow[0].D(), centerCol[0].D() - 3, centerRow[0].D(), centerCol[0].D() + 3);
	iv->dispLine(centerRow[0].D() - 3, centerCol[0].D(), centerRow[0].D() + 3, centerCol[0].D());
	HTuple strow, stcol, toprow, topcol;
	for (int s = 0;s < 4;s++) {
		if (m_param->upDwOrLfRt == 0 && (s == 2 || s == 3)) {
			continue;
		}
		else if (m_param->upDwOrLfRt == 1 && (s == 0 || s == 1)) {
			continue;
		}
		if (!m_param->IsUseStand[s]) {
			continue;
		}
		getDispOutData(StandLfRtPointRow, s, strow);
		getDispOutData(StandLfRtPointCol, s, stcol);
		getDispOutData(StandPointTopRow, s, toprow);
		getDispOutData(StandPointTopCol, s, topcol);
		int pinNum;
		if ((s + 1) % 2 == 1) {
			pinNum = m_param->grpPinNum[0];
		}
		else {
			pinNum = m_param->grpPinNum[1];
		}
		for (int i = 0;i < pinNum;i++) {
			HTuple standok;
			getDispOutData(StandValOK, s, i, standok);
			if (standok[0].I() == 0) {
				iv->setColor(255, 0, 0);
			}
			else {
				iv->setColor(0, 255, 0);
			}
			iv->dispLine(strow[2 * i].D(), stcol[2 * i].D() - 3, strow[2 * i].D(), stcol[2 * i].D() + 3);
			iv->dispLine(strow[2 * i].D() - 3, stcol[2 * i].D(), strow[2 * i].D() + 3, stcol[2 * i].D());
			iv->dispLine(strow[2 * i + 1].D(), stcol[2 * i + 1].D() - 3, strow[2 * i + 1].D(), stcol[2 * i + 1].D() + 3);
			iv->dispLine(strow[2 * i + 1].D() - 3, stcol[2 * i + 1].D(), strow[2 * i + 1].D() + 3, stcol[2 * i + 1].D());
			iv->dispLine(toprow[i].D(), topcol[i].D() - 3, toprow[i].D(), topcol[i].D() + 3);
			iv->dispLine(toprow[i].D() - 3, topcol[i].D(), toprow[i].D() + 3, topcol[i].D());
		}
	}
}

void locationGuiMod::textResult(ResultText *text, const QString &funName)
{
	if (p_im->empty())
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image!"));
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
	for (int i = 0;i < 4;i++) {
		if (m_param->upDwOrLfRt == 0 && (i == 2 || i == 3)) {
			continue;
		}
		else if (m_param->upDwOrLfRt == 1 && (i == 0 || i == 1)) {
			continue;
		}
		if (!m_param->IsUseStand[i]) {
			continue;
		}
		HTuple standok;
		getDispOutData(StandOk, i, standok);
		if (standok[0].I() != 0) {
			text->setTextColor(QColor(255, 0, 0));
			text->append(QObject::tr("%1Lead").arg(charbuf[i]));
			if (standok[0].I() == 1) {
				//text->insertPlainText(QObject::tr(" 管脚数目计算错误"));
				text->insertPlainText(QObject::tr(" The calculated lead number is wrong"));
			}
			else if (standok[0].I() == 2) {
				//text->insertPlainText(QString::fromLocal8Bit(" 宽度方向边界检测失败"));
				text->insertPlainText(QObject::tr(" Width direction boundary detection failed"));
			}
			else if (standok[0].I() == 3) {
				//text->insertPlainText(QString::fromLocal8Bit(" 顶点检测失败"));
				text->insertPlainText(QObject::tr(" top end detection failed"));
			}
			return;
		}
	}
	if (m_param->IsErrWidth) {
		HTuple errwidthok, errwidth;
		getDispOutData(ErrWidthOk, errwidthok);
		getDispOutData(ErrWidth, errwidth);
		if (errwidthok[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
		}
		text->append(QObject::tr("Body Width = %1(%2,%3)").arg(QString::number(errwidth[0].D(), 'g', 3)).
			arg(QString::number(m_param->errWidthMin)).arg(QString::number(m_param->errWidthMax)));
	}
	if (m_param->IsErrHeight) {
		HTuple errheightok, errheight;
		getDispOutData(ErrHeightOk, errheightok);
		getDispOutData(ErrHeight, errheight);
		if (errheightok[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
		}
		text->append(QObject::tr(" Body Height= %1(%2,%3)").arg(QString::number(errheight[0].D(), 'g', 3)).
			arg(QString::number(m_param->errHeightMin)).arg(QString::number(m_param->errHeightmax)));
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
	for (int s = 0;s < 4;s++) {
		if (m_param->upDwOrLfRt == 0 && (s == 2 || s == 3)) {
			continue;
		}
		else if (m_param->upDwOrLfRt == 1 && (s == 0 || s == 1)) {
			continue;
		}
		if (!m_param->IsUseStand[s]) {
			continue;
		}
		text->append(QString::fromLocal8Bit("%1:").arg(charbuf[s]));
		int pinNum;
		int grpIndex;
		if ((s + 1) % 2 == 1) {
			grpIndex = 0;
			pinNum = m_param->grpPinNum[0];
		}
		else {
			grpIndex = 1;
			pinNum = m_param->grpPinNum[1];
		}
		for (int i = 0;i < pinNum;i++) {
			HTuple thisok;
			getDispOutData(StandValOK, s, i, thisok);
			if (thisok[0].D() == 0) {
				text->setTextColor(QColor(255, 0, 0));
			}
			else
				text->setTextColor(QColor(0, 0, 0));
			text->append(QObject::tr("Lead%1:  ").arg(QString::number(i + 1)));
			if (m_param->errPinStandOffValid) {
				HTuple standoff;
				getDispOutData(StandOff, s, i, standoff);
				text->append(QObject::tr("standOff: %1(%2,%3)  ").arg(QString::number(standoff[0].D(), 'f', 4)).
					arg(QString::number(m_param->errPinStandOffMin[grpIndex][i], 'f', 4)).arg(QString::number(m_param->errPinStandOffMax[grpIndex][i], 'f', 4)));
			}
			if (m_param->errPinWidthValid) {
				HTuple pinwidth;
				getDispOutData(PinWidth, s, i, pinwidth);
				text->insertPlainText(QObject::tr("Lead Width: %1(%2,%3)  ").arg(QString::number(pinwidth[0].D(), 'f', 4)).
					arg(QString::number(m_param->errPinWidthMin[grpIndex][i], 'f', 4)).arg(QString::number(m_param->errPinWidthMax[grpIndex][i], 'f', 4)));
			}
		}
		if (m_param->errPinGapValid)
		{
			for (int i = 0;i < pinNum - 1;i++) {
				HTuple inter;
				getDispOutData(PinInterVal, s, i, inter);
				if (inter[0].D() > m_param->errPinGapMax[grpIndex][i] || inter[0].D() < m_param->errPinGapMin[grpIndex][i]) {
					text->setTextColor(QColor(255, 0, 0));
				}
				else
					text->setTextColor(QColor(0, 0, 0));
				text->append(QObject::tr("lead pitch%1(lead%2-lead%3): ").arg(QString::number(i + 1)).arg(QString::number(i + 1)).
					arg(QString::number(i + 2)));
				text->insertPlainText(QString::fromLocal8Bit("%1(%2,%3)").arg(QString::number(inter[0].D(), 'f', 4)).
					arg(QString::number(m_param->errPinGapMin[grpIndex][i], 'f', 4)).arg(QString::number(m_param->errPinGapMax[grpIndex][i], 'f', 4)));
			}
		}
		if (m_param->errPinStandOffDiffValid)
		{
			HTuple maxdiff;
			getDispOutData(StandOffDiff, s, maxdiff);
			if (maxdiff[0].D() > m_param->errPinStandOffDiffMax[grpIndex]) {
				text->setTextColor(QColor(255, 0, 0));
			}
			else
				text->setTextColor(QColor(0, 0, 0));
			text->append(QObject::tr("MaxDiff of standOff: %1").arg(QString::number(maxdiff[0].D(), 'f', 4)));
			if (maxdiff[0].D() > m_param->errPinStandOffDiffMax[grpIndex]) {
				text->insertPlainText(QString::fromLocal8Bit(">(%1)Ng").arg(QString::number(m_param->errPinStandOffDiffMax[grpIndex], 'f', 4)));
			}
			else
				text->insertPlainText(QString::fromLocal8Bit("<(%1)Pass").arg(QString::number(m_param->errPinStandOffDiffMax[grpIndex], 'f', 4)));
		}
	}
}

void locationGuiMod::iniData()
{
	m_param = new locationGuiParam;
	m_image = new Hobject;
	createPins();
}

void locationGuiMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_pixelSize, "sz");
	addPin(&p_LineAngle, "lineAngle");
	addPin(&p_ActualDepth, "actualDepth");
	addPin(&p_prjRatio, "prjRatio");

	addPin(&p_corners, "corners");
	addPin(&p_StandOffEnable, "StandOffEn");
	addPin(&p_StandOffOut, "StandOffOut");
	addPin(&p_PinWidthEnable, "PinWidthEn");
	addPin(&p_PinWidthOut, "PinWidthOut");
	addPin(&p_PinInterValEnable, "PinInterValEn");
	addPin(&p_PinInterValOut, "PinInterValOut");
	addPin(&p_StandOffDiffEnable, "StandOffDiffEn");
	addPin(&p_StandOffDiffOut, "StandOffDiffOut");
	addPin(&p_ResultOutOk, "ResultOutOk");
	addPin(&p_PinCountOut, "PinCountOut");

	p_StandOffEnable.setVisible(false);
	p_StandOffOut.setVisible(false);
	p_PinWidthEnable.setVisible(false);
	p_PinWidthOut.setVisible(false);
	p_PinInterValEnable.setVisible(false);
	p_PinInterValOut.setVisible(false);
	p_StandOffDiffEnable.setVisible(false);
	p_StandOffDiffOut.setVisible(false);
	p_ResultOutOk.setVisible(false);
	p_PinCountOut.setVisible(false);
}

FloatPoint * locationGuiMod::GetLineIntersection(Line RefLine1, Line RefLine2)
{
	FloatPoint* Intersection = new FloatPoint;
	float K1, K2, Delta_Col1, Delta_Row1, Delta_Col2, Delta_Row2;
	Delta_Col1 = RefLine1.End.col - RefLine1.Start.col;
	Delta_Row1 = RefLine1.End.row - RefLine1.Start.row;
	Delta_Col2 = RefLine2.End.col - RefLine2.Start.col;
	Delta_Row2 = RefLine2.End.row - RefLine2.Start.row;

	if (Delta_Col1 == 0)
	{
		if (Delta_Col2 == 0)
			return NULL;
		else {
			K2 = (float)Delta_Row2 / ((float)(Delta_Col2));
			Intersection->col = RefLine1.Start.col;
			Intersection->row = K2*(Intersection->col - RefLine2.Start.col) + RefLine2.Start.row;
			return Intersection;
		}
	}
	else if (Delta_Col2 == 0)
	{
		K1 = (float)Delta_Row1 / ((float)(Delta_Col1));
		Intersection->col = RefLine2.Start.col;
		Intersection->row = K1*(Intersection->col - RefLine1.Start.col) + RefLine1.Start.row;
		return Intersection;
	}
	else
	{
		K1 = (float)Delta_Row1 / ((float)(Delta_Col1));
		K2 = (float)Delta_Row2 / ((float)(Delta_Col2));
		if (K1 == K2)
			return NULL;
		else
		{
			Intersection->col = (RefLine2.Start.row - RefLine1.Start.row - (K2*RefLine2.Start.col - K1*RefLine1.Start.col)) / (K1 - K2);
			Intersection->row = K1*(Intersection->col - RefLine1.Start.col) + RefLine1.Start.row;
			return Intersection;
		}
	}
}

void locationGuiMod::toHtuple(const UnitInputPin<QList<double>>& p, HTuple &htuple)
{
	htuple.Reset();
	for (auto i = 0;i < p->size();++i)
		htuple[i] = p->at(i);
}

void locationGuiMod::toQList(const HTuple & htuple, UnitOutputPin<QList<double>>& p)
{
	p->clear();
	for (auto i = 0;i < htuple.Num();++i)
		p->push_back(htuple[i].D());
}