#include "pinWarpMod.h"
#include "pinWarpParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"

#define AllOK "allok"
#define BaseFindOk "baseFindok"  //�Ƿ��м�⵽�ܽŵ���
#define LeadFindOk "LeadFindOk"  //�Ƿ��м�⵽�ܽ�
#define RegionOk "regionOk" //�ȶ���Χ��ĸ��ܽ�������1��OK   

#define BeginRow "BeginEdgeRow"   //���˵�ʱ��Ķ˵���1��,�������ߵ�
#define BeginCol "BeginEdgeCol"
#define EndRow "EndEdgeRow"
#define EndCol "EndEdgeCol"

#define DispBasePointsImgOk "dispBaseImgOk"  //�Ƿ���������⵽
#define DispBasePointsImg "dispBasePointsImg"
#define DispleadPointsImg "dispLeadPointsImg"

#define LeadWidthMax "leadWidthMax"      //
#define LeadWidthMin "leadWidthMin"      //
#define LeadWidthOk  "leadWidthok"
#define LeadWarpMax  "leadWarpMax"       //
#define LeadWarpOk   "leadwarpok"

#define errRow "errRow"     //����������
#define errCol "errCol"

//�궨���ݽ�1��
#define ImageFull "Imagefull"  //ͼƬ�Ƿ�ok

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

	//Ԥ������ö�m_image����һ�¸�ʴ��1��
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

	bool baseFound[GRP_NUM]; //�����߽߱��Ƿ���1��
	bool leadfindOk[GRP_NUM];    //ÿ���Ƿ��������ҳ��ܽ�
	bool regionok[GRP_NUM]; //�ҳ�����ÿ��ܽŶ�����Χ���Ƿ���ȷ
	for (int i = 0;i < GRP_NUM;i++) {
		baseFound[i] = true;
		regionok[i] = true;
		leadfindOk[i] = true;
	}
	int searchDir[GRP_NUM]; //0Ϊ��߽�Ŀ��ӣ�����Ϊ�ܽſ�
	int roiW[GRP_NUM][2], roiH[GRP_NUM][2];    //roi��� �߶�
	double roiTopMidRow[GRP_NUM][2], roiTopMidCol[GRP_NUM][2];
	double roiLfMidRow[GRP_NUM][2], roiLfMidCol[GRP_NUM][2];  //roi�Ϸ����е�,�Լ���ߵ��е㣬���ں���������
	double roiangle[GRP_NUM];                 //����������ĵ���ֱ�ߵĽǶ�
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
	HTuple RowBegin[GRP_NUM], ColBegin[GRP_NUM], RowEnd[GRP_NUM], ColEnd[GRP_NUM];  //�ҵ���ÿ��ܽŵ������߽��,���һ����Ϊ���ߵ�������

	HTuple pinWid[GRP_NUM][10];                       //�ܽſ��
	HTuple pinEdge1Row[GRP_NUM], pinEdge1Col[GRP_NUM];   //���������ϲ��ܽű߽��
	HTuple pinEdge2Row[GRP_NUM], pinEdge2Col[GRP_NUM];   //���������²��ܽű߽��
	Hobject pointsobj[GRP_NUM];                           //ÿ��Ĺܽŵ��ߵ㼯��
	long validPoints[GRP_NUM];                            //ÿ��ܽ������ĵ���Ŀ
	get_image_size(*m_image, &Imgwidth, &ImgHeight);
	double baseAngle[GRP_NUM];                            //������ĵ��߽Ƕ�
	for (int i = 0;i < GRP_NUM;i++) {
		if (!m_param->grpEnable[i]) {
			continue;
		}
		HTuple m_rowEdge, m_colEdge;  //����ɨ�赽�Ĺܽżⲿ��

									  //for (int j=0;j<m_param->grpPinNum[i]+1;j++){    //���һ���Ǽ��ײ�
		if (m_param->IsDetBase[i]) {
			//�ȼ�����
			char edge[32];
			if (m_param->whiteBlackDir[i]) //1�׺�
				sprintf(edge, "negative");
			else
				sprintf(edge, "positive");
			double meaRow, meaCol, meaAngle;
			int meaLength1;
			if (searchDir[i] == 0) {  //����
				meaRow = roiTopMidRow[i][0];
				meaCol = roiTopMidCol[i][0];
				meaLength1 = roiW[i][0] / 2;
				meaAngle = 0;
			}
			else if (searchDir[i] == 1) { //����
				meaRow = roiTopMidRow[i][0];
				meaCol = roiTopMidCol[i][0];
				meaLength1 = roiW[i][0] / 2;
				meaAngle = PI;
			}
			else if (searchDir[i] == 2) { //����
				meaRow = roiLfMidRow[i][0];
				meaCol = roiLfMidCol[i][0];
				meaLength1 = roiH[i][0] / 2;
				meaAngle = -PI / 2.0;
			}
			else if (searchDir[i] == 3) { //����
				meaRow = roiLfMidRow[i][0];
				meaCol = roiLfMidCol[i][0];
				meaLength1 = roiH[i][0] / 2;
				meaAngle = PI / 2.0;
			}
			gen_measure_rectangle2(meaRow, meaCol, meaAngle, meaLength1, m_param->extendSearchLine[i] * 2, Imgwidth[0], ImgHeight[0],/*"bilinear"*/"nearest_neighbor", &measureHandle);
			int pointN = 0;   //�ۼƽ��в��Եĵ�1��
			int usefulPoint = 0;  // ��1����Ч�ó�����ĵ���(��1����Ϊ��)
			if (searchDir[i] == 1 || searchDir[i] == 0) {  //����
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
			else if (searchDir[i] == 2 || searchDir[i] == 3) {  //����
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
			} //�ɼ��߽���1��
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

				double factorval = m_param->clipfactor[i]; //����ʹ�õ�ɸѡ����
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
			while (baseAngle[i]<0 || baseAngle[i]>PI*2.0) {                               //������Ѱ�����򽫵��߽Ƕȵ�����ָ���Ϸ���������ָ���ҷ�
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
		if (m_param->IsWhitePin[i]) { //��ɫpositive
			sprintf(edge1, "positive");  //�ڵ���
			//sprintf(edge2,"negative");
		}
		else {
			sprintf(edge1, "negative");
			//sprintf(edge2,"positive");
		}
		double meaRow, meaCol, meaAngle1, meaAngle2;
		int meaLength1;
		Hlong meaHandle1, meaHandle2;
		if (searchDir[i] == 0 || searchDir[i] == 1) {  //����
			meaRow = roiTopMidRow[i][1];
			meaCol = roiTopMidCol[i][1];
			meaLength1 = roiW[i][1] / 2;
			meaAngle1 = baseAngle[i] - PI / 2.0;//ָ���ҷ�                           //���ܽŴ�������
			meaAngle2 = baseAngle[i] + PI / 2.0;//ָ����
		}
		else if (searchDir[i] == 2 || searchDir[i] == 3) { //����
			meaRow = roiLfMidRow[i][1];
			meaCol = roiLfMidCol[i][1];
			meaLength1 = roiH[i][1] / 2;
			meaAngle1 = baseAngle[i] - PI / 2.0;//ָ���·�                          //���ܽŴ�������
			meaAngle2 = baseAngle[i] + PI / 2.0;//ָ���Ϸ�
		}
		gen_measure_rectangle2(meaRow, meaCol, meaAngle1, meaLength1, m_param->extendSearchLine[i] * 2, Imgwidth[0], ImgHeight[0],/*"bilinear"*/"nearest_neighbor", &meaHandle1);
		gen_measure_rectangle2(meaRow, meaCol, meaAngle2, meaLength1, m_param->extendSearchLine[i] * 2, Imgwidth[0], ImgHeight[0],/*"bilinear"*/"nearest_neighbor", &meaHandle2);
		int pointN = 0;   //�ۼƽ��в��Եĵ�1��
		int usefulPoint = 0;  // ��1����Ч�ó�����ĵ���(��1����Ϊ��)
		HTuple rowEdge1, colEdge1, amp1, distance1;
		HTuple rowEdge2, colEdge2, amp2, distance2;
		if (searchDir[i] == 1 || searchDir[i] == 0) {  //����
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
		else if (searchDir[i] == 2 || searchDir[i] == 3) {  //����
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
		} //�ɼ��߽���1��
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
			HTuple errWidRow, errWidCol;  //�洢�ܽſ�ȴ���������
			int errWidCont, errWarpCont;  //�洢�Ĵ������Ŀ
			HTuple errWarpRow, errWarpCol;  //�洢�ܽſ�ȴ���������
			double maxWid, minWid;  //���������С�����
			double maxwarp;        //��������
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
					wid *= mk[i];   //���ϱ���
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
					if (m_param->IsBaseSameWithLead[i]) {                            //�������߽߱��Ϊwarp1,Զ���Ϊwarp2
						distance_pl(pinEdge1Row[i][j].D(), pinEdge1Col[i][j].D(), RowBegin[i][0].D(), ColBegin[i][0].D(), RowEnd[i][0].D(), ColEnd[i][0].D(), &warp1);
						distance_pl(pinEdge2Row[i][j].D(), pinEdge2Col[i][j].D(), RowBegin[i][0].D(), ColBegin[i][0].D(), RowEnd[i][0].D(), ColEnd[i][0].D(), &warp2);

					}
					else {
						distance_pl(pinEdge1Row[i][j].D(), pinEdge1Col[i][j].D(), RowBegin[i][0].D(), ColBegin[i][0].D(), RowEnd[i][0].D(), ColEnd[i][0].D(), &warp2);
						distance_pl(pinEdge2Row[i][j].D(), pinEdge2Col[i][j].D(), RowBegin[i][0].D(), ColBegin[i][0].D(), RowEnd[i][0].D(), ColEnd[i][0].D(), &warp1);
					}
					warp1 = abs(warp1 - m_param->normalOffMin[i]);
					warp2 = abs(warp2 - m_param->normalOffMax[i]);
					warp1 *= mk[i];   //���ϱ���
					warp2 *= mk[i];   //���ϱ���
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

