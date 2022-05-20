#include "pinWarpMod.h"
#include "pinWarpParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"

#define AllOK "allok"
#define BaseFindOk "baseFindok"  //是否有检测到管脚底线
#define LeadFindOk "LeadFindOk"  //是否有检测到管脚
#define RegionOk "regionOk" //比对误差范围后的各管脚区域是1否OK   

#define BeginRow "BeginEdgeRow"   //检测端点时候的端点坐1标,包括底线的
#define BeginCol "BeginEdgeCol"
#define EndRow "EndEdgeRow"
#define EndCol "EndEdgeCol"

#define DispBasePointsImgOk "dispBaseImgOk"  //是否有轮廓检测到
#define DispBasePointsImg "dispBasePointsImg"
#define DispleadPointsImg "dispLeadPointsImg"

#define LeadWidthMax "leadWidthMax"      //
#define LeadWidthMin "leadWidthMin"      //
#define LeadWidthOk  "leadWidthok"
#define LeadWarpMax  "leadWarpMax"       //
#define LeadWarpOk   "leadwarpok"

#define errRow "errRow"     //错误点的坐标
#define errCol "errCol"

//标定数据进1入
#define ImageFull "Imagefull"  //图片是否ok

pinWarpMod::pinWarpMod()
{
	iniData();
}

pinWarpMod::pinWarpMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

pinWarpMod::~pinWarpMod()
{
	delete m_param;
	delete m_image;
}

int pinWarpMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int pinWarpMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void pinWarpMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void pinWarpMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int pinWarpMod::run(const QString &funName)
{
	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
		m_testItemsStatus[i] = 1;
	}

	//预处理最好对m_image进行一下腐蚀处1理
	bool allok = true;
	if (p_im->empty()) {
		allok = false;
		setDetectOutData(AllOK, allok);
		setDetectOutData(ImageFull, false);
		m_testItemsStatus[0] = 0;
		return allok;
	}
	else {
		setDetectOutData(ImageFull, true);
	}
	HalOpenCV::cv2halImg(*p_im, m_image, false);

	double mk[GRP_NUM];
	for (int i = 0;i < GRP_NUM;i++) {
		if (m_param->IsInputRatio[i]) {
			try {
				HTuple k;
				//getDetectInData(pixelSize, k);
				k = **p_pixSize;
				mk[i] = k[0].D();
			}
			catch (HException& ex) {
				mk[i] = m_param->sizeRatio[i];
			}
		}
		else {
			mk[i] = m_param->sizeRatio[i];
		}
	}

	bool baseFound[GRP_NUM]; //各底线边界是否找1到
	bool leadfindOk[GRP_NUM];    //每组是否有正常找出管脚
	bool regionok[GRP_NUM]; //找出来的每组管脚对照误差范围后是否正确
	for (int i = 0;i < GRP_NUM;i++) {
		baseFound[i] = true;
		regionok[i] = true;
		leadfindOk[i] = true;
	}
	int searchDir[GRP_NUM]; //0为检边界的框子，后续为管脚框
	int roiW[GRP_NUM][2], roiH[GRP_NUM][2];    //roi宽度 高度
	double roiTopMidRow[GRP_NUM][2], roiTopMidCol[GRP_NUM][2];
	double roiLfMidRow[GRP_NUM][2], roiLfMidCol[GRP_NUM][2];  //roi上方的中点,以及左边的中点，便于后面搜索用
	double roiangle[GRP_NUM];                 //保存求出来的底线直线的角度
	for (int i = 0;i < GRP_NUM;i++) {
		for (int j = 0;j < 2;j++) {
			roiW[i][j] = m_param->RoiReg[i][j].col2 - m_param->RoiReg[i][j].col1;
			roiH[i][j] = m_param->RoiReg[i][j].row2 - m_param->RoiReg[i][j].row1;
			roiTopMidRow[i][j] = m_param->RoiReg[i][j].row1;
			roiTopMidCol[i][j] = (m_param->RoiReg[i][j].col1 + m_param->RoiReg[i][j].col2) / 2.0;
			roiLfMidRow[i][j] = (m_param->RoiReg[i][j].row1 + m_param->RoiReg[i][j].row2) / 2.0;
			roiLfMidCol[i][j] = m_param->RoiReg[i][j].col1;
		}
		roiangle[i] = 0;
		searchDir[i] = m_param->searchDir[i];
	}
	Hlong measureHandle;
	HTuple Imgwidth, ImgHeight;
	HTuple RowBegin[GRP_NUM], ColBegin[GRP_NUM], RowEnd[GRP_NUM], ColEnd[GRP_NUM];  //找到的每组管脚的两个边界点,最后一个点为底线的两个点

	HTuple pinWid[GRP_NUM][10];                       //管脚宽度
	HTuple pinEdge1Row[GRP_NUM], pinEdge1Col[GRP_NUM];   //搜索到的上部管脚边界点
	HTuple pinEdge2Row[GRP_NUM], pinEdge2Col[GRP_NUM];   //搜索到的下部管脚边界点
	Hobject pointsobj[GRP_NUM];                           //每组的管脚底线点集合
	long validPoints[GRP_NUM];                            //每组管脚所检测的点数目
	get_image_size(*m_image, &Imgwidth, &ImgHeight);
	double baseAngle[GRP_NUM];                            //求出来的底线角度
	for (int i = 0;i < GRP_NUM;i++) {
		if (!m_param->grpEnable[i]) {
			continue;
		}
		HTuple m_rowEdge, m_colEdge;  //四组扫描到的管脚尖部点

									  //for (int j=0;j<m_param->grpPinNum[i]+1;j++){    //最后一个是检测底部
		if (m_param->IsDetBase[i]) {
			//先检测底线
			char edge[32];
			if (m_param->whiteBlackDir[i]) //1白黑
				sprintf(edge, "negative");
			else
				sprintf(edge, "positive");
			double meaRow, meaCol, meaAngle;
			int meaLength1;
			if (searchDir[i] == 0) {  //左右
				meaRow = roiTopMidRow[i][0];
				meaCol = roiTopMidCol[i][0];
				meaLength1 = roiW[i][0] / 2;
				meaAngle = 0;
			}
			else if (searchDir[i] == 1) { //右左
				meaRow = roiTopMidRow[i][0];
				meaCol = roiTopMidCol[i][0];
				meaLength1 = roiW[i][0] / 2;
				meaAngle = PI;
			}
			else if (searchDir[i] == 2) { //上下
				meaRow = roiLfMidRow[i][0];
				meaCol = roiLfMidCol[i][0];
				meaLength1 = roiH[i][0] / 2;
				meaAngle = -PI / 2.0;
			}
			else if (searchDir[i] == 3) { //下上
				meaRow = roiLfMidRow[i][0];
				meaCol = roiLfMidCol[i][0];
				meaLength1 = roiH[i][0] / 2;
				meaAngle = PI / 2.0;
			}
			gen_measure_rectangle2(meaRow, meaCol, meaAngle, meaLength1, m_param->extendSearchLine[i] * 2, Imgwidth[0], ImgHeight[0],/*"bilinear"*/"nearest_neighbor", &measureHandle);
			int pointN = 0;   //累计进行测试的点1数
			int usefulPoint = 0;  // 累1计有效得出结果的点数(结1果不为空)
			if (searchDir[i] == 1 || searchDir[i] == 0) {  //左右
				HTuple rowEdge, colEdge, amp, distance;
				for (int startPoint = m_param->deleteEdge[i];startPoint < roiH[i][0] - m_param->deleteEdge[i];startPoint += m_param->searchLineGap[i]) {
					double transRow, transCol;
					transRow = roiTopMidRow[i][0] + startPoint;
					transCol = roiTopMidCol[i][0];
					translate_measure(measureHandle, transRow, transCol);
					measure_pos(*m_image, measureHandle, 1, m_param->baseGrayDiff[i], edge, "first", &rowEdge, &colEdge, &amp, &distance);
					Hlong aa = rowEdge.Num();
					if (aa > 0) {
						m_rowEdge[usefulPoint] = rowEdge[0];
						m_colEdge[usefulPoint] = colEdge[0];
						usefulPoint++;
					}
					pointN++;
				}
			}
			else if (searchDir[i] == 2 || searchDir[i] == 3) {  //上下
				HTuple rowEdge, colEdge, amp, distance;
				for (int startPoint = m_param->deleteEdge[i];startPoint < roiW[i][0] - m_param->deleteEdge[i];startPoint += m_param->searchLineGap[i]) {
					double transRow, transCol;
					transRow = roiLfMidRow[i][0];
					transCol = roiLfMidCol[i][0] + startPoint;
					translate_measure(measureHandle, transRow, transCol);
					measure_pos(*m_image, measureHandle, 1.0, m_param->baseGrayDiff[i], edge, "first", &rowEdge, &colEdge, &amp, &distance);
					Hlong aa = rowEdge.Num();
					if (aa > 0) {
						m_rowEdge[usefulPoint] = rowEdge[0];
						m_colEdge[usefulPoint] = colEdge[0];
						usefulPoint++;
					}
					pointN++;
				}
			} //采集边界点结1束
			Hlong ab = m_rowEdge.Num();
			Hobject edgeObj;
			HTuple nc, nr, dist;
			HTuple lineRowBegin, lineColBegin, lineRowEnd, lineColEnd;
			if (ab >= 4) {
				gen_contour_polygon_xld(&edgeObj, m_rowEdge, m_colEdge);
				Hobject tmpobj;
				gen_region_points(&tmpobj, m_rowEdge, m_colEdge);
				setDetectOutData(DispBasePointsImgOk, true, i);
				setDetectOutObject(DispBasePointsImg, tmpobj, i);

				double factorval = m_param->clipfactor[i]; //下面使用的筛选因子
				if (int(factorval*10.0) % 5 != 0) {
					factorval = double((int)(factorval + 0.5));
				}
				fit_line_contour_xld(edgeObj, "tukey"/*"regression"*/, -1, 0, 5, factorval/*1.0*/, &lineRowBegin, &lineColBegin, &lineRowEnd,  //1.0, 1.5, 2.0, 2.5, 3.0
					&lineColEnd, &nr, &nc, &dist);
				Hlong contor = lineRowBegin.Num();
				if (contor == 0) {
					baseFound[i] = false;
					allok = false;
					regionok[i] = false;
					setDetectOutData(BaseFindOk, baseFound[i], i);
					setDetectOutData(RegionOk, regionok[i], i);

					m_testItemsStatus[0] = 0;
				}
				else if (contor > 0) {
					RowBegin[i][0] = lineRowBegin[0];
					ColBegin[i][0] = lineColBegin[0];
					RowEnd[i][0] = lineRowEnd[0];
					ColEnd[i][0] = lineColEnd[0];
					if (abs(ColBegin[i][0].D() - ColEnd[i][0].D()) < 1e-3) {
						baseAngle[i] = PI / 2.0;
					}
					else {
						baseAngle[i] = -atan((RowEnd[i][0].D() - RowBegin[i][0].D()) / (ColEnd[i][0].D() - ColBegin[i][0].D()));
					}
					setDetectOutData(BeginRow, RowBegin[i][0], i);
					setDetectOutData(BeginCol, ColBegin[i][0], i);
					setDetectOutData(EndRow, RowEnd[i][0], i);
					setDetectOutData(EndCol, ColEnd[i][0], i);
				}

			}
			else if (ab < 4) {
				baseFound[i] = false;
				allok = false;
				regionok[i] = false;
				setDetectOutData(BaseFindOk, baseFound[i], i);
				setDetectOutData(RegionOk, regionok[i], i);
				setDetectOutData(DispBasePointsImgOk, false, i);

				m_testItemsStatus[0] = 0;
			}
			close_measure(measureHandle);
			if (!baseFound[i]) {
				continue;
			}
			else {
				setDetectOutData(BaseFindOk, baseFound[i], i);
			}
		}   //end of if(m_param->IsDetBase[i])
		if (m_param->IsDetBase[i]) {
			while (baseAngle[i]<0 || baseAngle[i]>PI*2.0) {                               //左右搜寻方向则将底线角度调整至指向上方，上下则指向右方
				if (baseAngle[i] < 0) {
					baseAngle[i] += PI*2.0;
				}
				else {
					baseAngle[i] -= PI*2.0;
				}
			}
			if (searchDir[i] == 0 || searchDir[i] == 1) {
				if (baseAngle[i] > PI) {
					baseAngle[i] -= PI;
				}
			}
			else {
				while (baseAngle[i] > PI / 2.0) {
					baseAngle[i] -= PI;
				}
			}
		}
		else {
			if (searchDir[i] == 0 || searchDir[i] == 1) {
				baseAngle[i] = PI / 2.0;
			}
			else {
				baseAngle[i] = 0;
			}
		}
		double lineAngle = 0;
		char edge1[32], edge2[32];
		if (m_param->IsWhitePin[i]) { //白色positive
			sprintf(edge1, "positive");  //黑到白
			//sprintf(edge2,"negative");
		}
		else {
			sprintf(edge1, "negative");
			//sprintf(edge2,"positive");
		}
		double meaRow, meaCol, meaAngle1, meaAngle2;
		int meaLength1;
		Hlong meaHandle1, meaHandle2;
		if (searchDir[i] == 0 || searchDir[i] == 1) {  //左右
			meaRow = roiTopMidRow[i][1];
			meaCol = roiTopMidCol[i][1];
			meaLength1 = roiW[i][1] / 2;
			meaAngle1 = baseAngle[i] - PI / 2.0;//指向右方                           //检测管脚从左至右
			meaAngle2 = baseAngle[i] + PI / 2.0;//指向左方
		}
		else if (searchDir[i] == 2 || searchDir[i] == 3) { //上下
			meaRow = roiLfMidRow[i][1];
			meaCol = roiLfMidCol[i][1];
			meaLength1 = roiH[i][1] / 2;
			meaAngle1 = baseAngle[i] - PI / 2.0;//指向下方                          //检测管脚从上至下
			meaAngle2 = baseAngle[i] + PI / 2.0;//指向上方
		}
		gen_measure_rectangle2(meaRow, meaCol, meaAngle1, meaLength1, m_param->extendSearchLine[i] * 2, Imgwidth[0], ImgHeight[0],/*"bilinear"*/"nearest_neighbor", &meaHandle1);
		gen_measure_rectangle2(meaRow, meaCol, meaAngle2, meaLength1, m_param->extendSearchLine[i] * 2, Imgwidth[0], ImgHeight[0],/*"bilinear"*/"nearest_neighbor", &meaHandle2);
		int pointN = 0;   //累计进行测试的点1数
		int usefulPoint = 0;  // 累1计有效得出结果的点数(结1果不为空)
		HTuple rowEdge1, colEdge1, amp1, distance1;
		HTuple rowEdge2, colEdge2, amp2, distance2;
		if (searchDir[i] == 1 || searchDir[i] == 0) {  //左右
			for (int startPoint = m_param->deleteEdge[i];startPoint < roiH[i][1] - m_param->deleteEdge[i];startPoint += m_param->searchLineGap[i]) {
				double transRow, transCol;
				transRow = (double)(int)(roiTopMidRow[i][1] + startPoint);
				transCol = (double)(int)(roiTopMidCol[i][1]);
				translate_measure(meaHandle1, transRow, transCol);
				translate_measure(meaHandle2, transRow, transCol);
				measure_pos(*m_image, meaHandle1, 1, m_param->leadGrayDiff[i], edge1, "first", &rowEdge1, &colEdge1, &amp1, &distance1);
				measure_pos(*m_image, meaHandle2, 1, m_param->leadGrayDiff[i], edge1, "first", &rowEdge2, &colEdge2, &amp2, &distance2);

				Hlong aa = rowEdge1.Num();
				Hlong bb = rowEdge2.Num();
				if (aa > 0 && bb > 0) {
					pinEdge1Row[i][usefulPoint] = rowEdge1[0].D();
					pinEdge1Col[i][usefulPoint] = colEdge1[0].D();
					pinEdge2Row[i][usefulPoint] = rowEdge2[0].D();
					pinEdge2Col[i][usefulPoint] = colEdge2[0].D();
					usefulPoint++;
				}
				pointN++;
			}
		}
		else if (searchDir[i] == 2 || searchDir[i] == 3) {  //上下
			for (int startPoint = m_param->deleteEdge[i];startPoint < roiW[i][1] - m_param->deleteEdge[i];startPoint += m_param->searchLineGap[i]) {
				double transRow, transCol;
				transRow = (double)(roiLfMidRow[i][1]);
				transCol = (double)(roiLfMidCol[i][1] + startPoint);
				translate_measure(meaHandle1, transRow, transCol);
				translate_measure(meaHandle2, transRow, transCol);
				measure_pos(*m_image, meaHandle1, 1, m_param->leadGrayDiff[i], edge1, "first", &rowEdge1, &colEdge1, &amp1, &distance1);
				measure_pos(*m_image, meaHandle2, 1, m_param->leadGrayDiff[i], edge1, "first", &rowEdge2, &colEdge2, &amp2, &distance2);
				Hlong aa = rowEdge1.Num();
				Hlong bb = rowEdge2.Num();
				if (aa > 0 && bb > 0) {
					pinEdge1Row[i][usefulPoint] = rowEdge1[0].D();
					pinEdge1Col[i][usefulPoint] = colEdge1[0].D();
					pinEdge2Row[i][usefulPoint] = rowEdge2[0].D();
					pinEdge2Col[i][usefulPoint] = colEdge2[0].D();
					usefulPoint++;
				}
				pointN++;
			}
		} //采集边界点结1束
		close_measure(meaHandle1);
		close_measure(meaHandle2);
		validPoints[i] = usefulPoint;
		if (validPoints[i] < 1) {
			leadfindOk[i] = false;
			allok = false;
			regionok[i] = false;
			m_testItemsStatus[0] = 0;
			setDetectOutData(LeadFindOk, leadfindOk[i], i);
			setDetectOutData(RegionOk, regionok[i], i);
			continue;
		}
		else {
			Hobject tmplead1, tmplead2;
			gen_region_points(&tmplead1, pinEdge1Row[i], pinEdge1Col[i]);
			gen_region_points(&tmplead2, pinEdge2Row[i], pinEdge2Col[i]);
			union2(tmplead1, tmplead2, &tmplead1);
			setDetectOutObject(DispleadPointsImg, tmplead1, i);
			HTuple errWidRow, errWidCol;  //存储管脚宽度错误点的坐标
			int errWidCont, errWarpCont;  //存储的错误点数目
			HTuple errWarpRow, errWarpCol;  //存储管脚宽度错误点的坐标
			double maxWid, minWid;  //求出来的最小最大宽度
			double maxwarp;        //最大的翘曲
			maxWid = 0;
			minWid = 1000000;
			maxwarp = 0;
			errWidCont = errWarpCont = 0;
			bool widok = true;
			bool warpok = true;
			int nume1 = pinEdge1Row[i].Num();
			int nume2 = pinEdge2Row[i].Num();
			int nume3 = pinEdge1Col[i].Num();
			int nume4 = pinEdge2Col[i].Num();
			for (int j = 0;j < validPoints[i];j++) {

				if (m_param->errWidthValid[i]) {
					double wid;
					distance_pp(pinEdge1Row[i][j].D(), pinEdge1Col[i][j].D(), pinEdge2Row[i][j].D(), pinEdge2Col[i][j].D(), &wid);
					wid *= mk[i];   //乘上比例
					if (wid<m_param->errWidthMin[i] || wid>m_param->errWidthMax[i]) {
						errWidRow[errWidCont * 2] = pinEdge1Row[i][j];
						errWidRow[errWidCont * 2 + 1] = pinEdge2Row[i][j];
						errWidCol[errWidCont * 2] = pinEdge1Col[i][j];
						errWidCol[errWidCont * 2 + 1] = pinEdge2Col[i][j];
						errWidCont++;

						m_testItemsStatus[1] = 0;
					}
					if (wid > maxWid)
						maxWid = wid;
					if (wid < minWid) {
						minWid = wid;
					}
				}
				if (m_param->errWarpValid[i] && m_param->IsDetBase[i]) {
					double warp1, warp2;
					if (m_param->IsBaseSameWithLead[i]) {                            //靠近底线边界的为warp1,远离的为warp2
						distance_pl(pinEdge1Row[i][j].D(), pinEdge1Col[i][j].D(), RowBegin[i][0].D(), ColBegin[i][0].D(), RowEnd[i][0].D(), ColEnd[i][0].D(), &warp1);
						distance_pl(pinEdge2Row[i][j].D(), pinEdge2Col[i][j].D(), RowBegin[i][0].D(), ColBegin[i][0].D(), RowEnd[i][0].D(), ColEnd[i][0].D(), &warp2);

					}
					else {
						distance_pl(pinEdge1Row[i][j].D(), pinEdge1Col[i][j].D(), RowBegin[i][0].D(), ColBegin[i][0].D(), RowEnd[i][0].D(), ColEnd[i][0].D(), &warp2);
						distance_pl(pinEdge2Row[i][j].D(), pinEdge2Col[i][j].D(), RowBegin[i][0].D(), ColBegin[i][0].D(), RowEnd[i][0].D(), ColEnd[i][0].D(), &warp1);
					}
					warp1 = abs(warp1 - m_param->normalOffMin[i]);
					warp2 = abs(warp2 - m_param->normalOffMax[i]);
					warp1 *= mk[i];   //乘上比例
					warp2 *= mk[i];   //乘上比例
					if (warp1 > m_param->errWarp[i] || warp2 > m_param->errWarp[i]) {
						errWarpRow[errWarpCont * 2] = pinEdge1Row[i][j];
						errWarpRow[errWarpCont * 2 + 1] = pinEdge2Row[i][j];
						errWarpCol[errWarpCont * 2] = pinEdge1Col[i][j];
						errWarpCol[errWarpCont * 2 + 1] = pinEdge2Col[i][j];
						errWarpCont++;

						m_testItemsStatus[2] = 0;
					}
					if (warp1 > maxwarp)
						maxwarp = warp1;
					if (warp2 > maxwarp)
						maxwarp = warp2;
				}
			}   //end of j validpoints
			if (m_param->errWarpValid[i] && errWarpCont > m_param->noiseMax[i]) {
				warpok = false;
				regionok[i] = false;
				allok = false;
				m_testItemsStatus[0] = 0;
			}
			if (m_param->errWidthValid[i] && errWidCont > m_param->noiseMax[i]) {
				widok = false;
				regionok[i] = false;
				allok = false;
				m_testItemsStatus[0] = 0;
			}
			HTuple errrow, errcol;
			if (m_param->errWarpValid[i]) {
				setDetectOutData(LeadWarpOk, warpok, i);
				setDetectOutData(LeadWarpMax, maxwarp, i);
			}
			if (m_param->errWidthValid[i]) {
				setDetectOutData(LeadWidthOk, widok, i);
				if (validPoints == 0) {
					minWid = 0;
				}
				setDetectOutData(LeadWidthMax, maxWid, i);
				setDetectOutData(LeadWidthMin, minWid, i);
			}
			int warpnum = errWarpRow.Num() / 2;
			int widnum = errWidRow.Num() / 2;
			if (warpnum > m_param->noiseMax[i] && widnum <= m_param->noiseMax[i]) {
				//errrow=errWarpRow;
				//errcol=errWarpCol;
				setDetectOutData(errRow, errWarpRow, i);
				setDetectOutData(errCol, errWarpCol, i);
			}
			else if (warpnum <= m_param->noiseMax[i] && widnum > m_param->noiseMax[i]) {
				//errrow=errWidRow;
				//errcol=errWidCol;
				setDetectOutData(errRow, errWidRow, i);
				setDetectOutData(errCol, errWidCol, i);
			}
			else {
				tuple_concat(errWarpRow, errWidRow, &errrow);
				tuple_concat(errWarpCol, errWidCol, &errcol);
				setDetectOutData(errRow, errrow, i);
				setDetectOutData(errCol, errcol, i);
			}
			setDetectOutData(LeadFindOk, leadfindOk[i], i);
			setDetectOutData(RegionOk, regionok[i], i);
		} //end of validPoints[i]>1
	}//end of loop i(lead grp)

	setDetectOutData(AllOK, allok);
	return allok;
}

void pinWarpMod::viewResult(ImageView *iv, const QString &funName, int)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	for (int i = 0;i < GRP_NUM;i++) {
		if (!m_param->grpEnable[i]) {
			continue;
		}
		HTuple baseok;
		if (m_param->IsDetBase[i]) {
			getDispOutData(DispBasePointsImgOk, i, baseok);
			if (baseok[0].I()) {
				iv->setColor(255, 255, 0);
				iv->setFilled(false);
				const Hobject *reg = getDispObject(DispBasePointsImg, i);
				RlRegion dispReg;
				HalOpenCV::hal2cvRegion(*reg, &dispReg);
				iv->dispRegion(&dispReg);
			}
			else {
				continue;
			}
			HTuple basefindok;
			getDispOutData(BaseFindOk, i, basefindok);
			if (basefindok[0].I()) {
				HTuple beginrow, begincol, endrow, endcol;
				getDispOutData(BeginRow, i, beginrow);
				getDispOutData(BeginCol, i, begincol);
				getDispOutData(EndRow, i, endrow);
				getDispOutData(EndCol, i, endcol);
				iv->setColor(0, 255, 255);
				iv->dispLine(beginrow[0].D(), begincol[0].D(), endrow[0].D(), endcol[0].D());
			}
			else {
				iv->setColor(255, 0, 0);
				iv->setFilled(false);
				iv->dispRect1(m_param->RoiReg[i][0].row1, m_param->RoiReg[i][0].col1, m_param->RoiReg[i][0].row2, m_param->RoiReg[i][0].col2);
			}
		}
		HTuple leadok;
		getDispOutData(LeadFindOk, i, leadok);
		if (leadok[0].I()) {
			iv->setColor(255, 255, 0);
			iv->setFilled(false);
			const Hobject *reg = getDispObject(DispleadPointsImg, i);
			RlRegion dispReg;
			HalOpenCV::hal2cvRegion(*reg, &dispReg);
			iv->dispRegion(&dispReg);
			//iv->dispRegion(*getDispObject(DispleadPointsImg, i));
			HTuple regionok;
			getDispOutData(RegionOk, i, regionok);
			if (regionok[0].I() == 0) {
				iv->setColor(255, 0, 0);
				iv->setFilled(false);
				iv->dispRect1(m_param->RoiReg[i][1].row1, m_param->RoiReg[i][1].col1, m_param->RoiReg[i][1].row2, m_param->RoiReg[i][1].col2);
				HTuple errrow, errcol;
				getDispOutData(errRow, i, errrow);
				getDispOutData(errCol, i, errcol);
				int ddd = errrow.Num();
				for (int j = 0;j < errrow.Num();j++) {
					iv->dispLine(errrow[j].D(), errcol[j].D() - 1, errrow[j].D(), errcol[j].D() + 1);
					iv->dispLine(errrow[j].D() - 1, errcol[j].D(), errrow[j].D() + 1, errcol[j].D());
				}
			}
		}
		else {
			continue;
		}
	}//end of loop i
}

void pinWarpMod::textResult(ResultText *text, const QString &funName)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}

	for (int i = 0;i < GRP_NUM;i++) {
		if (!m_param->grpEnable[i]) {
			continue;
		}
		text->setTextColor(QColor(0, 0, 0));
		text->append(QObject::tr("lead group%1").arg(i + 1));
		if (m_param->IsDetBase[i]) {
			HTuple basefindok;
			getDispOutData(BaseFindOk, i, basefindok);
			if (basefindok[0].I() == 0) {
				text->setTextColor(QColor(255, 0, 0));
				text->append(QObject::tr("base line found failed"));
				continue;
			}
		}

		HTuple leadok;
		getDispOutData(LeadFindOk, i, leadok);
		if (leadok[0].I()) {
			if (m_param->errWidthValid[i]) {
				HTuple widok;
				getDispOutData(LeadWidthOk, i, widok);
				if (widok[0].I()) {
					text->setTextColor(QColor(0, 0, 0));
				}
				else {
					text->setTextColor(QColor(255, 0, 0));
				}
				HTuple widmin, widmax;
				getDispOutData(LeadWidthMax, i, widmax);
				getDispOutData(LeadWidthMin, i, widmin);
				text->append(QObject::tr("lead width:(%1,%2)[%3,%4]")
					.arg(QString::number(widmin[0].D(), 'f', 4))
					.arg(QString::number(widmax[0].D(), 'f', 4))
					.arg(QString::number(m_param->errWidthMin[i], 'f', 4))
					.arg(QString::number(m_param->errWidthMax[i], 'f', 4)));
			}
			if (m_param->errWarpValid[i]) {
				HTuple warpok;
				getDispOutData(LeadWarpOk, i, warpok);
				if (warpok[0].I()) {
					text->setTextColor(QColor(0, 0, 0));
				}
				else {
					text->setTextColor(QColor(255, 0, 0));
				}
				HTuple warpmax;
				getDispOutData(LeadWarpMax, i, warpmax);
				text->append(QObject::tr("lead warp:(%1)[%2]")
					.arg(QString::number(warpmax[0].D(), 'f', 4))
					.arg(QString::number(m_param->errWarp[i], 'f', 4)));
			}
		}
		else {
			text->setTextColor(QColor(255, 0, 0));
			text->append(QObject::tr("lead found failed"));
			continue;
		}
	}//end of loop i
}

void pinWarpMod::iniData()
{
	m_param = new pinWarpParam;
	m_image = new Hobject;
	createPins();
}

void pinWarpMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_pixSize, "sz");
}

