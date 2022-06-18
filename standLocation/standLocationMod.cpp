#include "standLocationMod.h"
#include "standLocationParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"

#define AllOK "allok"

#define RegionOut "regionout"

//站高部分
#define StandOk "StandOk" //左边站高检测ok,数目不和  0:成功， 1：求出的管脚数目不对， 2，管脚宽度计算错误，3顶点计算错误

#define StandValOK "standValOk"          //左部分站高内的各1元素是否OK,比较误差范围后是否ok
#define CoplanarOk "coplanarok"          //共线性差异ok(包含共面性，共面ng则两个共线ng)

#define StandLfRtPointRow "StandLeftRightPointRow"    //右区域站高的宽度边界点
#define StandLfRtPointCol "StandLeftRightPointCol"   
#define StandPointTopRow "standPointTopRow" //站高区域的顶点坐标
#define StandPointTopCol "standPointTopCol"

#define PinInterVal "pinInterval"  //左管脚间1距
#define PinWidth "pinWidth"        //管脚宽1度
#define StandOff "standOff"        //管脚站1高
#define StandOffDiff "standoffDiff"  //管脚站高差1距
#define StandCoplanar "standcoplanar"//共面性

#define ResultOutOk "resultOutOk"

#define ImageFull "Imagefull"  //图片是否ok

#define STANDOFF "standOff"
#define WIDTH "width"
#define PITCH "pitch"
#define STANDDIFF "standDiff"

standLocationMod::standLocationMod()
{
	iniData();
}

standLocationMod::standLocationMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

standLocationMod::~standLocationMod()
{
	delete m_param;
	delete m_image;
}

int standLocationMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int standLocationMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void standLocationMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void standLocationMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int standLocationMod::run(const QString &funName)
{
// 	p_PinWidthOut->clear();
// 	p_StandOffOut->clear();
// 	p_PinInterValOut->clear();
// 	p_StandOffDiffOut->clear();
// 	p_PinCountOut->clear();

	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
		m_testItemsStatus[i] = 1;
	}
	//预处理最好对m_image进行一下腐蚀处理
	bool resultOutOK = true;
	bool allok = true;
	m_standUseOk = true;
	if (m_param->grpPinNum[0] == 0 && m_param->grpPinNum[1] == 0)
	{
		m_standUseOk = false;
	}

	if (!m_standUseOk || p_im->empty()) {
		allok = false;
		setDetectOutData(AllOK, allok);
		setDetectOutData(ImageFull, false);
		setDetectOutData(ResultOutOk, false);
		//*p_ResultOutOk = false;
		m_testItemsStatus[0] = 0;
		return allok;
	}
	else {
		setDetectOutData(ImageFull, true);
	}
	HalOpenCV::cv2halImg(*p_im, m_image, false);

	toHtuple(p_pixSize, m_k);
	toHtuple(p_lAng, m_lineAngle);
	toHtuple(p_ratio, m_prjRatio);
	toHtuple(p_aDepth, m_actualDepth);
	//getDetectInData(pixelSize, m_k);
	//getDetectInData(LineAngle, m_lineAngle);
	//getDetectInData(prjRatio, m_prjRatio);
	//getDetectInData(ActualDepth, m_actualDepth);

	//up
	int roirow1[4], roicol1[4], roirow2[4], roicol2[4], searchDir[4];

	for (int i = 0;i < 4;i++) {
		roirow1[i] = m_param->RoiRegionVector[i + 1].row1;
		roirow2[i] = m_param->RoiRegionVector[i + 1].row2;
		roicol1[i] = m_param->RoiRegionVector[i + 1].col1;
		roicol2[i] = m_param->RoiRegionVector[i + 1].col2;
	}
	HTuple Imgwidth, ImgHeight;
	get_image_size(*m_image, &Imgwidth, &ImgHeight);
	HTuple RowBegin[4], ColBegin[4], RowEnd[4], ColEnd[4];  //上下左右的顺序
	HTuple RowBegin11[4], ColBegin11[4], RowEnd11[4], ColEnd11[4];  //用于储存没有偏移之前的边界点
	HTuple RowB, RowE, ColB, ColE;
	toHtuple(p_edgeRowBeg, RowB);
	toHtuple(p_edgeRowEnd, RowE);
	toHtuple(p_edgeColBeg, ColB);
	toHtuple(p_edgeColEnd, ColE);
	//getDetectInData(InEdgeRowBegin, RowB);
	//getDetectInData(InEdgeRowEnd, RowE);
	//getDetectInData(InEdgeColBegin, ColB);
	//getDetectInData(InEdgeColEnd, ColE);

	try
	{
		for (int i = 0;i < 4;i++) {
			RowBegin[i][0] = RowB[i].D();
			RowEnd[i][0] = RowE[i].D();
			ColBegin[i][0] = ColB[i].D();
			ColEnd[i][0] = ColE[i].D();
			RowBegin11[i][0] = RowB[i].D();
			RowEnd11[i][0] = RowE[i].D();
			ColBegin11[i][0] = ColB[i].D();
			ColEnd11[i][0] = ColE[i].D();
		}
	}
	catch (HException& ex)
	{
		for (int i = 0;i < 4;i++) {
			RowBegin[i][0] = 100 + 30 * i;
			RowEnd[i][0] = 200 + 30 * i;
			ColBegin[i][0] = 100 + 30 * i;
			ColEnd[i][0] = 200 + 30 * i;
			RowBegin11[i][0] = 100 + 30 * i;
			RowEnd11[i][0] = 200 + 30 * i;
			ColBegin11[i][0] = 100 + 30 * i;
			ColEnd11[i][0] = 200 + 30 * i;
		}
	}


	HTuple m_rowEdge[4], m_colEdge[4];  //四条边的扫描到的点数


	for (int i = 0;i < 4;i++)
	{
		//进行检测到的边界点的纠正
		if (((RowEnd[i][0].D() < RowBegin[i][0].D()) && (i == 2 || i == 3)) || ((ColEnd[i][0].D() < ColBegin[i][0].D()) && (i == 1 || i == 0))) {   //调转开头点和结尾点，使全部符合图片上的正向顺1序
			double tmp;
			tmp = ColEnd[i][0].D();
			ColEnd[i][0] = ColBegin[i][0];
			ColBegin[i][0] = tmp;
			tmp = RowEnd[i][0].D();
			RowEnd[i][0] = RowBegin[i][0];
			RowBegin[i][0] = tmp;
		}
	} //for loop end
	double EveryAngle[4];
	for (int i = 0;i < 4;i++) {
		if (i > 1) {
			EveryAngle[i] = PI / 2.0 + atan((ColEnd[i][0].D() - ColBegin[i][0].D()) / (RowEnd[i][0].D() - RowBegin[i][0].D()));
		}
		else {
			EveryAngle[i] = -atan((RowEnd[i][0].D() - RowBegin[i][0].D()) / (ColEnd[i][0].D() - ColBegin[i][0].D()));
		}
	}
	//求站高部分,顺序上下左右
	HTuple pinLfrtRow, pinLfrtCol;  //左区域站高部分得出的左右点。是数组
	HTuple pinTopRow, pinTopCol; //站高部分得出的顶点。是数组
	HTuple InterRow, InterCol;  //右边两根线交点

	HTuple OutStandoffdiff[4];
	HTuple OutStandoff[4], OutPinwidth[4], OutPininterval[4];  //用于传出的管脚的一些参数

	double standoffmax[2] = { 0,0 };  //用于求站高的落差
	double standoffmin[2] = { 1000000,1000000 };
	bool coplanarOk[2];
	coplanarOk[0] = coplanarOk[1] = true;

	HTuple inputlength;    //输入的管脚长度
	toHtuple(p_leadLen, inputlength);
	//getDetectInData(InputLeadLength, inputlength);
	double m_realLeadLen[2][GRP_MAX_LEADNUM];
	for (int i = 0;i < 2;i++) {
		for (int j = 0;j < GRP_MAX_LEADNUM;j++) {
			m_realLeadLen[i][j] = 1.0;
			if (j < m_param->grpPinNum[i] && (inputlength.Num() > (m_param->grpPinNum[0] * i + j))) {
				m_realLeadLen[i][j] = inputlength[m_param->grpPinNum[0] * i + j].D();
			}
		}
	}

	for (int i = 0;i < 4;i++) {
		if (m_param->upDwOrLfRt == 0 && (i == 2 || i == 3))
			continue;
		else if (m_param->upDwOrLfRt == 1 && (i == 0 || i == 1))
			continue;
		if (m_param->grpPinNum[i % 2] == 0) {
			continue;
		}
		Line line1, line2;
		HTuple searchbeginrow, searchbegincol, searchendrow, searchendcol;
		Line edgeline;
		if (i == 0 || i == 1) {
			line1.Start.row = roirow1[i];
			line1.Start.col = roicol1[i];
			line1.End.row = roirow2[i];
			line1.End.col = roicol1[i];
			line2.Start.row = roirow1[i];
			line2.Start.col = roicol2[i];
			line2.End.row = roirow2[i];
			line2.End.col = roicol2[i];
		}
		else if (i == 2 || i == 3) {
			line1.Start.row = roirow1[i];
			line1.Start.col = roicol1[i];
			line1.End.row = roirow1[i];
			line1.End.col = roicol2[i];
			line2.Start.row = roirow2[i];
			line2.Start.col = roicol1[i];
			line2.End.row = roirow2[i];
			line2.End.col = roicol2[i];
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
											  //toppoint=GetLineIntersection(line1,edgeline);
											  //btmpoint=GetLineIntersection(line2,edgeline);

		HTuple llrow1, llcol1, llparallel1;
		intersection_ll(line1.Start.row, line1.Start.col, line1.End.row, line1.End.col,
			edgeline.Start.row, edgeline.Start.col, edgeline.End.row, edgeline.End.col, &llrow1, &llcol1, &llparallel1);
		toppoint->row = llrow1[0].D();
		toppoint->col = llcol1[0].D();

		HTuple llrow2, llcol2, llparallel2;
		intersection_ll(line2.Start.row, line2.Start.col, line2.End.row, line2.End.col,
			edgeline.Start.row, edgeline.Start.col, edgeline.End.row, edgeline.End.col, &llrow2, &llcol2, &llparallel2);
		btmpoint->row = llrow2[0].D();
		btmpoint->col = llcol2[0].D();

		Hobject polyregion;   //多边形区域
		HTuple polyrow, polycol;
		if (i == 0) {
			polyrow[0] = roirow1[i];
			polycol[0] = roicol1[i];
			polyrow[1] = roirow1[i];
			polycol[1] = roicol2[i];
			polyrow[2] = btmpoint->row;
			polycol[2] = btmpoint->col;
			polyrow[3] = toppoint->row;
			polycol[3] = toppoint->col;
		}
		else if (i == 1) {
			polyrow[3] = roirow2[i];
			polycol[3] = roicol1[i];
			polyrow[2] = roirow2[i];
			polycol[2] = roicol2[i];
			polyrow[1] = btmpoint->row;
			polycol[1] = btmpoint->col;
			polyrow[0] = toppoint->row;
			polycol[0] = toppoint->col;
		}
		else if (i == 2) { //左区域
			polyrow[0] = roirow1[i];
			polycol[0] = roicol1[i];
			polyrow[1] = roirow2[i];
			polycol[1] = roicol1[i];
			polyrow[2] = btmpoint->row;
			polycol[2] = btmpoint->col;
			polyrow[3] = toppoint->row;
			polycol[3] = toppoint->col;
		}
		else if (i == 3) {
			polyrow[3] = roirow1[i];
			polycol[3] = roicol2[i];
			polyrow[2] = roirow2[i];
			polycol[2] = roicol2[i];
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
		char sortBuf[32];
		if (m_param->upDwOrLfRt == 1) {   //左右站高
			sprintf(sortBuf, "column");
		}
		else {
			sprintf(sortBuf, "row");
		}
		sort_region(polyImg, &polyImg, "character", "true", sortBuf);

		setDetectOutObject(RegionOut, polyImg, i);
		HTuple srow1, srow2, scol1, scol2;  //求取用于后面检测管脚站高点，检测管脚宽度边界时gen_measure_rectangle1的length1长度
		HTuple sWid, sHei; //求出来的长宽值
		smallest_rectangle1(polyImg, &srow1, &scol1, &srow2, &scol2);
		double thei, twid;
		for (int ss = 0;ss < srow1.Num();ss++) {
			thei = sHei[ss] = srow2[ss].D() - srow1[ss].D();
			twid = sWid[ss] = scol2[ss].D() - scol1[ss].D();
		}
		if (polynum != m_param->grpPinNum[(i + 1) % 2 == 1 ? 0 : 1]) {
			setDetectOutData(StandOk, 1, i);
			resultOutOK = false;
			m_testItemsStatus[0] = 0;
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
					//interpoint=GetLineIntersection(chuizhiline,edgeline);

					HTuple llrow2, llcol2, llparallel2;
					intersection_ll(chuizhiline.Start.row, chuizhiline.Start.col, chuizhiline.End.row, chuizhiline.End.col,
						edgeline.Start.row, edgeline.Start.col, edgeline.End.row, edgeline.End.col, &llrow2, &llcol2, &llparallel2);
					interpoint->row = llrow2[0].D();
					interpoint->col = llcol2[0].D();

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
					//interpoint=GetLineIntersection(chuizhiline,edgeline);

					HTuple llrow2, llcol2, llparallel2;
					intersection_ll(chuizhiline.Start.row, chuizhiline.Start.col, chuizhiline.End.row, chuizhiline.End.col,
						edgeline.Start.row, edgeline.Start.col, edgeline.End.row, edgeline.End.col, &llrow2, &llcol2, &llparallel2);
					interpoint->row = llrow2[0].D();
					interpoint->col = llcol2[0].D();

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
			double troww = detectRow[j].D();
			double tcoll = detectCol[j].D();
			gen_measure_rectangle2(detectRow[j].D(), detectCol[j].D()
				, EveryAngle[i], widlen1 / 2.0 + 3, m_param->extendSearchLine[i], Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measurestand);
			measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], "all", "all", &rowEdge1, &colEdge1, &amp1, &distance1);
			Hlong num1 = rowEdge1.Num();
			if (num1 < 2) {
				setDetectOutData(StandOk, 2, i);
				resultOutOK = false;
				allok = false;
				m_testItemsStatus[0] = 0;
				delete interpoint;
				iscontinueJloop = false;
				close_measure(measurestand);
				break;
			}
			close_measure(measurestand);
			pinLfrtRow[2 * j] = rowEdge1[0];
			pinLfrtCol[2 * j] = colEdge1[0];
			pinLfrtRow[2 * j + 1] = rowEdge1[num1 - 1];
			pinLfrtCol[2 * j + 1] = colEdge1[num1 - 1];
			double mnewrow, mnewcol;  //根据新求出来的宽度点重新确定中心点
			mnewrow = (rowEdge1[0].D() + rowEdge1[num1 - 1].D()) / 2.0;
			mnewcol = (colEdge1[0].D() + colEdge1[num1 - 1].D()) / 2.0;
			distance_pp(rowEdge1[0].D(), colEdge1[0].D(), rowEdge1[num1 - 1].D(), colEdge1[num1 - 1].D(), &distance1);
			InterRow[j] = mnewrow;
			InterCol[j] = mnewcol;

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
				gen_measure_rectangle2(mnewrow - 3, mnewcol, Kangle, toplen1 + 3, 2, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measurestand);
				measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], edgeBuf, "first", &rowedge21, &coledge21, &amp2, &distance21);
				translate_measure(measurestand, mnewrow + 3, mnewcol);
				measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], edgeBuf, "first", &rowedge22, &coledge22, &amp2, &distance22);
				translate_measure(measurestand, mnewrow, mnewcol);
				measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], edgeBuf, "first", &rowedge2, &coledge2, &amp2, &distance2);
			}
			else if (i == 0 || i == 1) {
				gen_measure_rectangle2(mnewrow, mnewcol - 3, Kangle, toplen1 + 3, 2, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measurestand);
				measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], edgeBuf, "first", &rowedge21, &coledge21, &amp2, &distance21);
				translate_measure(measurestand, mnewrow, mnewcol + 3);
				measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], edgeBuf, "first", &rowedge22, &coledge22, &amp2, &distance22);
				translate_measure(measurestand, mnewrow, mnewcol);
				measure_pos(*m_image, measurestand, 1, m_param->minGrayDiff[i], edgeBuf, "first", &rowedge2, &coledge2, &amp2, &distance2);
			}
			num1 = rowedge2.Num();
			if (num1 < 1) {
				close_measure(measurestand);
				setDetectOutData(StandOk, 3, i);
				resultOutOK = false;
				m_testItemsStatus[0] = 0;
				allok = false;
				delete interpoint;
				iscontinueJloop = false;
				break;
			}
			pinTopRow[j] = rowedge2[0];
			pinTopCol[j] = coledge2[0];
			double tmptoprow = rowedge2[0].D();
			double tmptopcol = coledge2[0].D();
			if (i == 0 && j == 8) {
				int dfsdf = 0;
			}
			close_measure(measurestand);
			double pinstand;
			pinstand = sqrt(pow(interpoint->row - rowedge2[0].D(), 2) + pow(interpoint->col - coledge2[0].D(), 2));

			double ratioangle; //投影的实际角度
			double prjratioo = asin(m_prjRatio[i < 2 ? (i + 2) : (i - 2)/*i-2*/].D());
			double lineanglee = m_lineAngle[i].D();
			double minusaangle = EveryAngle[i < 2 ? (i + 2) : (i - 2)/*i-2*/] - lineanglee;
			if (i < 2)   //i 上下左右
			{  //shangxia
				ratioangle = prjratioo + (EveryAngle[i < 2 ? (i + 2) : (i - 2)] - lineanglee);   //lineAngle 顺序左右上下,everyAngle 上下左右
			}
			else
			{ //zuoyou
				ratioangle = prjratioo - (EveryAngle[i < 2 ? (i + 2) : (i - 2)] - lineanglee);   //lineAngle 顺序左右上下,everyAngle 上下左右
			}
			double everangle = EveryAngle[i < 2 ? (i + 2) : (i - 2)] / PI*180.0;
			double lagle = lineanglee / PI*180.0;
			//ratioangle=prjratioo- ( EveryAngle[0/*i-2*/] -lineanglee );   //lineAngle 顺序左右上下,everyAngle 上下左右
			double deopth = m_actualDepth[i < 2 ? (i + 2) : (i - 2)].D();
			double sdsds = m_k[i < 2 ? (i + 3) : (i - 1)].D();
			double standbefore = pinstand;
			double kstand = pinstand*m_k[i < 2 ? (i + 3) : (i - 1)].D()*(1.0 / cos(ratioangle));

			//pinstand=pinstand*m_k[i<2?(i+3):(i-1)].D()-m_param->pinlength*sin(ratioangle);   //减去标定中的间隙，并且乘上实际的投影比例

			//stand=计算出的总站高*(1/cos@)-tan@*管脚实际长度
			double rl = m_realLeadLen[(i + 1) % 2 == 1 ? 0 : 1][j];
			pinstand = pinstand*(1.0 / cos(ratioangle))*m_k[i < 2 ? (i + 3) : (i - 1)].D() - (m_realLeadLen[(i + 1) % 2 == 1 ? 0 : 1][j] + m_param->packageOff)*tan(ratioangle);

			pinstand += m_param->standOffCorr[(i + 1) % 2 == 1 ? 0 : 1][j];
			pinstand += m_param->offStandOffCorr[(i + 1) % 2 == 1 ? 0 : 1][j];

			pinstand += m_param->selfStandOffCorr[(i + 1) % 2 == 1 ? 0 : 1];
			OutStandoff[i][j] = pinstand;

			bool roiOK = true;
			if (m_param->errPinStandOffValid)
			{
				if (pinstand > m_param->errPinStandOffMax[grpIndex][j] || pinstand < m_param->errPinStandOffMin[grpIndex][j]) {
					allok = false;
					m_testItemsStatus[1] = 0;
					roiOK = false;
				}
				setDetectOutData(StandOff, pinstand, i, j);
				//setDataBaseData(QObject::tr("standoff"), pinstand, i, j);
			}
			if (m_param->errPinWidthValid)
			{
				distance1[0] = m_k[i < 2 ? (i + 3) : (i - 1)].D()*distance1[0].D();
				distance1[0] = m_param->widthCorr[(i + 1) % 2 == 1 ? 0 : 1][j] + m_param->offWidthCorr[(i + 1) % 2 == 1 ? 0 : 1][j] + distance1[0].D();
				if (distance1[0].D() > m_param->errPinWidthMax[grpIndex][j] || distance1[0].D() < m_param->errPinWidthMin[grpIndex][j]) {
					allok = false;
					roiOK = false;
					m_testItemsStatus[2] = 0;
				}
				setDetectOutData(PinWidth, distance1[0].D(), i, j);
				//setDataBaseData(QObject::tr("leadWidth"), distance1[0].D(), i, j);
				OutPinwidth[i][j] = distance1[0].D();
			}
			if ((m_param->IsLMSValid || m_param->errPinGapValid) && j > 0) //使用LMS算法时需要辅以间距求取后续站高点的具体位置
			{
				double gap = 0;
				gap = abs(sqrt(pow((InterRow[j].D() - InterRow[j - 1].D()), 2) + pow((InterCol[j].D() - InterCol[j - 1].D()), 2)));
				gap *= m_k[i < 2 ? (i + 3) : (i - 1)].D();
				gap += m_param->InterCorr[(i + 1) % 2 == 1 ? 0 : 1][j];
				gap += m_param->offInterCorr[(i + 1) % 2 == 1 ? 0 : 1][j];
				if (m_param->errPinGapValid) {
					setDetectOutData(PinInterVal, gap, i, j - 1);
					//setDataBaseData(QObject::tr("leadPitch"), gap, i, j - 1);
					if (gap > m_param->errPinGapMax[grpIndex][j - 1] || gap < m_param->errPinGapMin[grpIndex][j - 1]) {
						roiOK = false;
						allok = false;
						m_testItemsStatus[3] = 0;
					}
				}
				OutPininterval[i][j - 1] = gap;
			}
			if (m_param->errPinStandOffDiffValid || m_param->errCoplanar) {
				if (standoffmax[(i + 1) % 2 == 1 ? 0 : 1] < pinstand) {
					standoffmax[(i + 1) % 2 == 1 ? 0 : 1] = pinstand;
				}
				if (standoffmin[(i + 1) % 2 == 1 ? 0 : 1] > pinstand) {
					standoffmin[(i + 1) % 2 == 1 ? 0 : 1] = pinstand;
				}
			}
			if (m_param->errPinStandOffDiffValid)
			{
				if (j == polynum - 1) {
					if ((standoffmax[(i + 1) % 2 == 1 ? 0 : 1] - standoffmin[(i + 1) % 2 == 1 ? 0 : 1]) > m_param->errPinStandOffDiffMax[grpIndex]) {
						coplanarOk[(i + 1) % 2 == 1 ? 0 : 1] = false;
						allok = false;
						m_testItemsStatus[4] = 0;
					}
					setDetectOutData(StandOffDiff, standoffmax[(i + 1) % 2 == 1 ? 0 : 1] - standoffmin[(i + 1) % 2 == 1 ? 0 : 1], i);
					//setDataBaseData(QObject::tr("standoffDiff"), standoffmax[(i + 1) % 2 == 1 ? 0 : 1] - standoffmin[(i + 1) % 2 == 1 ? 0 : 1], i);
					OutStandoffdiff[i] = standoffmax[(i + 1) % 2 == 1 ? 0 : 1] - standoffmin[(i + 1) % 2 == 1 ? 0 : 1];
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
	int index[2];
	if (m_param->upDwOrLfRt == 0) {
		index[0] = 0;
		index[1] = 1;
	}
	else if (m_param->upDwOrLfRt == 1) {
		index[0] = 2;
		index[1] = 3;
	}
	if (resultOutOK && m_param->errCoplanarValid) {
		double tmpmin, tmpmax;
		tmpmin = standoffmin[0] < standoffmin[1] ? standoffmin[0] : standoffmin[1];
		tmpmax = standoffmax[0] > standoffmax[1] ? standoffmax[0] : standoffmax[1];
		double coplan;
		if (m_param->IsLMSValid && ((m_param->grpPinNum[0] + m_param->grpPinNum[1]) >= 4)) {
			double copRow[GRP_MAX_LEADNUM * 2], copCol[GRP_MAX_LEADNUM * 2];  //拟合出
			HTuple coprow, copcol;
			int totalPin = 0;
			for (int g = 0;g < 2;g++) {
				double stCol = 0;
				for (int i = 0;i < m_param->grpPinNum[g];i++) {
					coprow[totalPin] = copRow[totalPin] = OutStandoff[index[g]][i].D();
					copcol[totalPin] = copCol[totalPin] = 0 + stCol;
					if (i != (m_param->grpPinNum[g] - 1)) {
						stCol += OutPininterval[index[g]][i].D();
					}
					totalPin++;
				}
			}

			Hobject edgeObj;
			gen_contour_polygon_xld(&edgeObj, coprow, copcol);
			HTuple nc, nr, dist;
			HTuple rowbegin, colbegin, rowend, colend;
			double factorval = m_param->LMSFactor; //下面使用的筛选因子
			if (int(factorval*10.0) % 5 != 0) {
				factorval = double((int)(factorval + 0.5));
			}
			fit_line_contour_xld(edgeObj, "tukey", -1, 0, 5, factorval/*2.5*/, &rowbegin, &colbegin, &rowend,  //1.0, 1.5, 2.0, 2.5, 3.0
				&colend, &nr, &nc, &dist);
			Hlong contor = rowbegin.Num();
			if (contor == 0) {
				coplan = fabs(tmpmax - tmpmin);
			}
			else {
				double maxCop = -50000;
				double minCop = 50000;
				for (int i = 0;i < totalPin;i++) {
					HTuple dis;
					distance_pl(copRow[i], copCol[i], rowbegin[0], colbegin[0], rowend[0], colend[0], &dis);
					double disval = dis[0].D();
					if (maxCop < disval) {
						maxCop = disval;
					}
					if (minCop > disval) {
						minCop = disval;
					}
				}
				coplan = fabs(maxCop - minCop);
			}
		}
		else {
			coplan = fabs(tmpmax - tmpmin);
		}
		if (coplan > m_param->errCoplanar) {
			coplanarOk[0] = coplanarOk[1] = false;
			allok = false;
			m_testItemsStatus[5] = 0;
		}
		setDetectOutData(StandCoplanar, coplan);
		//setDataBaseData(StandCoplanar, coplan);
	}
	if (m_param->errCoplanarValid || m_param->errPinStandOffDiffValid) {
		setDetectOutData(CoplanarOk, coplanarOk[0], 0);
		setDetectOutData(CoplanarOk, coplanarOk[1], 1);
	}
	setDetectOutData(AllOK, allok);
	//*p_ResultOutOk = resultOutOK;
	setDetectOutData(ResultOutOk, resultOutOK);
	if (resultOutOK) {

		if (m_param->errPinStandOffDiffValid) {
			HTuple diffout;
			if (m_param->grpPinNum[0] < 2) {
				OutStandoffdiff[index[0]] = 0;
			}
			if (m_param->grpPinNum[1] < 2) {
				OutStandoffdiff[index[1]] = 0;
			}
			tuple_concat(OutStandoffdiff[index[0]], OutStandoffdiff[index[1]], &diffout);
			p_item->insert(STANDDIFF, toQList(diffout));
			//toQList(diffout, p_StandOffDiffOut);
			//setDetectOutData(StandOffDiffOut, diffout);
		}
		if (m_param->errPinStandOffValid) {
			HTuple offout;
			tuple_concat(OutStandoff[index[0]], OutStandoff[index[1]], &offout);
			p_item->insert(STANDOFF, toQList(offout));
			//toQList(offout, p_StandOffOut);
			//setDetectOutData(StandOffOut, offout);
		}
		if (m_param->errPinWidthValid) {
			HTuple widout;
			tuple_concat(OutPinwidth[index[0]], OutPinwidth[index[1]], &widout);
			p_item->insert(WIDTH, toQList(widout));
			//toQList(widout, p_PinWidthOut);
			//setDetectOutData(PinWidthOut, widout);
		}
		if (m_param->errPinGapValid) {
			HTuple interout;
			tuple_concat(OutPininterval[index[0]], OutPininterval[index[1]], &interout);
			p_item->insert(PITCH, toQList(interout));
			//toQList(interout, p_PinInterValOut);
			//setDetectOutData(PinInterValOut, interout);
		}
		//*p_StandOffEnable = m_param->errPinStandOffValid;
		//*p_PinWidthEnable = m_param->errPinWidthValid;
		//*p_PinInterValEnable = m_param->errPinGapValid;
		//*p_StandOffDiffEnable = m_param->errPinStandOffDiffValid;
		//p_PinCountOut->push_back(m_param->grpPinNum[0]);
		//p_PinCountOut->push_back(m_param->grpPinNum[1]);

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

void standLocationMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (!m_standUseOk) {
		return;
	}
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	//for (int i=0;i<4;i++){
	//	HTuple rowbegin,colbegin,rowend,colend;
	//	getDispOutData(SearchBeginRow,i,rowbegin);
	//	getDispOutData(SearchBeginCol,i,colbegin);
	//	getDispOutData(SearchEndRow,i,rowend);
	//	getDispOutData(SearchEndCol,i,colend);
	//	iv->setColor(0,0,255);
	//	iv->dispLine(rowbegin[0].D(),colbegin[0].D(),rowend[0].D(),colend[0].D());
	//}
	HTuple standok0;

	for (int i = 0;i < 4;i++) {
		if (m_param->upDwOrLfRt == 0 && (i == 2 || i == 3)) {
			continue;
		}
		else if (m_param->upDwOrLfRt == 1 && (i == 0 || i == 1)) {
			continue;
		}
		if (m_param->grpPinNum[i % 2] == 0) {
			continue;
		}
		getDispOutData(StandOk, i, standok0);

		//iv->dispRegion(*getDispObject(RegionOut,i));
		iv->setFilled(true);

		if (standok0[0].I() != 0) {
			iv->setColor(255, 0, 0);
			iv->setFilled(false);
			iv->dispRect1(m_param->RoiRegionVector[i + 1].row1, m_param->RoiRegionVector[i + 1].col1, m_param->RoiRegionVector[i + 1].row2,
				m_param->RoiRegionVector[i + 1].col2);
			iv->setFilled(true);
			const Hobject *obj = getDispObject(RegionOut, i);
			RlRegion rlReg;
			HalOpenCV::hal2cvRegion(*obj, &rlReg);
			iv->dispRegion(&rlReg);
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
	HTuple strow, stcol, toprow, topcol;
	HTuple copok[2];
	if (m_param->errCoplanarValid || m_param->errPinStandOffDiffValid) {
		getDispOutData(CoplanarOk, 0, copok[0]);
		getDispOutData(CoplanarOk, 1, copok[1]);
	}
	for (int s = 0;s < 4;s++) {
		if (m_param->upDwOrLfRt == 0 && (s == 2 || s == 3)) {
			continue;
		}
		else if (m_param->upDwOrLfRt == 1 && (s == 0 || s == 1)) {
			continue;
		}
		if (m_param->grpPinNum[s % 2] == 0) {
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
			if (m_param->errCoplanarValid || m_param->errPinStandOffDiffValid) {
				if (!copok[s % 2][0].I()) {
					iv->setColor(255, 0, 0);
				}
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

void standLocationMod::textResult(ResultText *text, const QString &funName)
{
	if (!m_standUseOk) {
		text->setTextColor(QColor(255, 0, 0));
		text->append(QObject::tr("The number of leads cannot be zero in all groups"));
		return;
	}
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	HTuple found;
	char charbuf[4][32] = { "Top","Bottom","Left","Right" };
	for (int i = 0;i < 4;i++) {
		if (m_param->upDwOrLfRt == 0 && (i == 2 || i == 3)) {
			continue;
		}
		else if (m_param->upDwOrLfRt == 1 && (i == 0 || i == 1)) {
			continue;
		}
		if (m_param->grpPinNum[i % 2] == 0) {
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
	for (int s = 0;s < 4;s++) {
		if (m_param->upDwOrLfRt == 0 && (s == 2 || s == 3)) {
			continue;
		}
		else if (m_param->upDwOrLfRt == 1 && (s == 0 || s == 1)) {
			continue;
		}
		if (m_param->grpPinNum[s % 2] == 0) {
			continue;
		}
		text->setTextColor(QColor(0, 0, 0));
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
		if (pinNum <= 0) {
			continue;
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
	HTuple resultok;
	//resultok = **p_ResultOutOk;
	getDispOutData(ResultOutOk, resultok);
	if (resultok[0].I() && m_param->errCoplanarValid)
	{
		HTuple maxdiff;
		getDispOutData(StandCoplanar, maxdiff);
		if (maxdiff[0].D() > m_param->errCoplanar) {
			text->setTextColor(QColor(255, 0, 0));
		}
		else
			text->setTextColor(QColor(0, 0, 0));
		text->append(QObject::tr("coplanar of all lead: %1").arg(QString::number(maxdiff[0].D(), 'f', 4)));
		if (maxdiff[0].D() > m_param->errCoplanar) {
			text->insertPlainText(QString::fromLocal8Bit(">(%1)Ng").arg(QString::number(m_param->errCoplanar, 'f', 4)));
		}
		else
			text->insertPlainText(QString::fromLocal8Bit("<(%1)Pass").arg(QString::number(m_param->errCoplanar, 'f', 4)));
	}
}

void standLocationMod::iniData()
{
	m_param = new standLocationParam;
	m_image = new Hobject;
	createPins();
}

void standLocationMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_pixSize, "Hsz");
	addPin(&p_lAng, "lAng");
	addPin(&p_aDepth, "aDepth");
	addPin(&p_ratio, "ratio");
	addPin(&p_edgeRowBeg, "edgeRowB");
	addPin(&p_edgeColBeg, "edgeColB");
	addPin(&p_edgeRowEnd, "edgeRowE");
	addPin(&p_edgeColEnd, "edgeColE");
	addPin(&p_leadLen, "leadLen");

	addPin(&p_item, "item");
	p_item.setVisible(false);

	p_item->insert(STANDOFF, 0);
	p_item->insert(WIDTH, 0);
	p_item->insert(PITCH, 0);
	p_item->insert(STANDDIFF, 0);

// 	addPin(&p_StandOffEnable, "standOffEn");
// 	addPin(&p_StandOffOut, "standOffOut");
// 	addPin(&p_PinWidthEnable, "pinWidthEn");
// 	addPin(&p_PinWidthOut, "pinWidthOut");
// 	addPin(&p_PinInterValEnable, "pinInterEn");
// 	addPin(&p_PinInterValOut, "pinInterValOut");
// 	addPin(&p_StandOffDiffEnable, "standOffDiffEn");
// 	addPin(&p_StandOffDiffOut, "standOffDiffOut");
// 	addPin(&p_ResultOutOk, "resultOutOk");
// 	addPin(&p_PinCountOut, "pinCountOut");

// 	p_StandOffEnable.setVisible(false);
// 	p_StandOffOut.setVisible(false);
// 	p_PinWidthEnable.setVisible(false);
// 	p_PinWidthOut.setVisible(false);
// 	p_PinInterValEnable.setVisible(false);
// 	p_PinInterValOut.setVisible(false);
// 	p_StandOffDiffEnable.setVisible(false);
// 	p_StandOffDiffOut.setVisible(false);
// 	p_ResultOutOk.setVisible(false);
// 	p_PinCountOut.setVisible(false);
}

