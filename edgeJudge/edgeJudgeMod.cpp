#include "edgeJudgeMod.h"
#include "edgeJudgeParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"

#define DiffNotExist "diffNotExist" //与传入区域相减之后是否存在多出的区域
#define DiffRegion "diffRegion" //与传入区域相减之后的区域
#define OffAreaVal "offareaval" //超出的面积值
#define OffAreaOk "offAreaOk"   

#define AllOK "allok"
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

#define ErrHeightOk "HeightOk"
#define ErrHeightMin "HeightMin"
#define ErrHeightMax "HeightMax"
#define ErrAngleOk "angleOK"
#define ErrAngle "angle" //最大偏转角1度
#define ErrCenterShiftOk "ErrcentershiftOK"
#define ErrCenterShift "Errcentershift" //与模板中心相对应的中心偏移1值

#define ImageFull "Imagefull"  //图片是否ok

#define ThreRegionOk "threRegionOk" //分割的区域是否有内容传出
#define ThreRegion "threRegion"     //传出分割所得到的区域

#define TmpHomMat "TmpHomMat"  //用于保存显示的矩阵

edgeJudgeMod::edgeJudgeMod()
{
	iniData();
}

edgeJudgeMod::edgeJudgeMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

edgeJudgeMod::~edgeJudgeMod()
{
	delete m_param;
	delete m_image;
}

int edgeJudgeMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int edgeJudgeMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void edgeJudgeMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void edgeJudgeMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int edgeJudgeMod::run(const QString &funName)
{
	p_corners->clear();
	bool allok = true;
	if (p_im->empty()) {
		allok = false;
		setDetectOutData(AllOK, allok);
		setDetectOutData(ImageFull, false);
		//return allok;
		return -1;
	}
	else {
		setDetectOutData(ImageFull, true);
	}
	HalOpenCV::cv2halImg(*p_im, m_image, false);
	int enableNum = 0;
	for (int i = 0;i < 4;i++) {
		if (!m_param->IsFixWidth[i])
			enableNum++;
	}
	if (enableNum <= 1/*(m_param->IsFixWidth[0]&&m_param->IsFixWidth[2]) || (m_param->IsFixWidth[1]&&m_param->IsFixWidth[3])*/) {
		allok = false;
		setDetectOutData(AllOK, allok);
		setDetectOutData(ImageFull, false);
		return allok;
	}
	else {
		setDetectOutData(ImageFull, true);
	}

	double mk;//像素比例
	if (m_param->IsInputPixel) {
		try {
			HTuple k;
			k = **p_pixSize;
			/*getDetectInData(InputPixSize, k);*/
			mk = k[0].D();
		}
		catch (HException& ex) {
			mk = m_param->pixSize;
		}
	}
	else {
		mk = m_param->pixSize;
	}
	bool roiFound[4]; //各边界是否找1到
	for (int i = 0; i < 4;i++) {
		roiFound[i] = true;
	}
	//up
	int searchdir[4];
	HTuple hommat2d;
	double rotateAngle;
	if (m_param->IsRefer) {
		try {
			//getDetectInData(HomMat2D, hommat2d);
			HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &hommat2d);
			double tv = hommat2d[0].D();
		}
		catch (HException& ex) {
			hom_mat2d_identity(&hommat2d);
		}
		rotateAngle = atan((hommat2d[3].D() / hommat2d[0].D()));
		setDetectOutData(TmpHomMat, hommat2d);
	}

	HTuple Imgwidth, ImgHeight;
	get_image_size(*m_image, &Imgwidth, &ImgHeight);

	HTuple inputReferRow, inputReferCol;  //传入的参考点区域
	Hobject detectImg; //供检测用的图片
	if (m_param->IsUseCompare) {  //暂时必须使用此种方式
		try {
			//getDetectInData(InputRow, inputReferRow);
			//getDetectInData(InputCol, inputReferCol);
			for (auto i = 0;i < p_inCorners->size();++i)
			{
				inputReferRow[i] = p_inCorners->at(i).y;
				inputReferCol[i] = p_inCorners->at(i).x;
			}
			double tt = inputReferRow[4].D();
			double tq = inputReferCol[4].D();
		}
		catch (HException& ex) {
			inputReferCol[4] = inputReferRow[4] = inputReferCol[0] = inputReferRow[0] = 20;
			inputReferCol[1] = 50;inputReferRow[1] = 20;
			inputReferCol[2] = 50;inputReferRow[2] = 50;
			inputReferCol[3] = 20;
			inputReferRow[3] = 50;
		}

		Hobject cimg, cregion;
		Hobject searchRec, imgrec, polyregion;
		gen_rectangle1(&imgrec, 0, 0, ImgHeight[0].I() - 1, Imgwidth[0].I() - 1);
		gen_rectangle1(&searchRec, m_param->RoiRegionVector[0].row1, m_param->RoiRegionVector[0].col1,
			m_param->RoiRegionVector[0].row2, m_param->RoiRegionVector[0].col2);
		if (m_param->IsRefer) {
			affine_trans_region(searchRec, &searchRec, hommat2d, "false");
		}
		gen_region_polygon_filled(&polyregion, inputReferRow, inputReferCol);
		if (m_param->inputRegionLarge > 0) {
			dilation_rectangle1(polyregion, &polyregion, m_param->inputRegionLarge, m_param->inputRegionLarge);
		}
		else if (m_param->inputRegionLarge < 0) {
			erosion_rectangle1(polyregion, &polyregion, m_param->inputRegionLarge, m_param->inputRegionLarge);
		}

		Hobject ptmpimg;
		copy_image(*m_image, &ptmpimg);
		paint_region(polyregion, ptmpimg, &ptmpimg, m_param->backGray, "fill");
		copy_image(ptmpimg, &detectImg);

		//difference(imgrec,polyregion,&polyregion);
		//reduce_domain(*m_image,polyregion,&cimg);
		//reduce_domain(cimg,searchRec,&cimg);
		//threshold(cimg,&cregion,m_param->threMin,m_param->threMax>=m_param->threMin?m_param->threMax:m_param->threMin);
		//connection(cregion,&cregion);
		//select_shape(cregion,&cregion,"area","and",m_param->areaMin,m_param->areaMax>=m_param->areaMin?m_param->areaMax:m_param->areaMin);
		//Hlong cnum;
		//count_obj(cregion,&cnum);
		//if (cnum==0){
		//	allok=false;
		//	setDetectOutData(AllOK,allok);
		//	setDetectOutData(ThreRegionOk,false);
		//	return allok;
		//}
		//else {
		//	setDetectOutData(ThreRegionOk,1);
		//}
		//union1(cregion,&cregion);
		//setDetectOutObject(ThreRegion,cregion);
		//region_to_bin(cregion,&detectImg,255,0,Imgwidth[0].I(),ImgHeight[0].I());

	}
	else {
		copy_obj(*m_image, &detectImg, 1, 1);
	}
	vector<int> roiW[4], roiH[4];    //roi宽度 高度
	vector<double> roiTopMidRow[4], roiTopMidCol[4];
	vector<double> roiLfMidRow[4], roiLfMidCol[4];  //roi上方的中点,以及左边的中点，便于后面搜索用
	int beforeNum = 1;
	for (int i = 0;i < 4;i++) {
		for (int j = 0;j < m_param->roiNum[i];j++) {
			roiW[i].push_back(m_param->RoiRegionVector[j + beforeNum].col2 - m_param->RoiRegionVector[j + beforeNum].col1);
			roiH[i].push_back(m_param->RoiRegionVector[j + beforeNum].row2 - m_param->RoiRegionVector[j + beforeNum].row1);
			roiTopMidRow[i].push_back(m_param->RoiRegionVector[j + beforeNum].row1);
			roiTopMidCol[i].push_back((m_param->RoiRegionVector[j + beforeNum].col1 + m_param->RoiRegionVector[j + beforeNum].col2) / 2.0);
			roiLfMidRow[i].push_back((m_param->RoiRegionVector[j + beforeNum].row1 + m_param->RoiRegionVector[j + beforeNum].row2) / 2.0);
			roiLfMidCol[i].push_back(m_param->RoiRegionVector[j + beforeNum].col1);
			if (m_param->IsRefer) {
				affine_trans_pixel(hommat2d, roiTopMidRow[i][j], roiTopMidCol[i][j], &roiTopMidRow[i][j], &roiTopMidCol[i][j]);
				affine_trans_pixel(hommat2d, roiLfMidRow[i][j], roiLfMidCol[i][j], &roiLfMidRow[i][j], &roiLfMidCol[i][j]);
			}
		}
		searchdir[i] = m_param->searchDir[i];
		beforeNum += m_param->roiNum[i];
	}

	Hlong measureHandle;

	Hobject edgeObj[4];
	HTuple RowBegin[4], ColBegin[4], RowEnd[4], ColEnd[4];  //上右下左的顺序，拟合的四个边界起始点
	HTuple RowBegin11[4], ColBegin11[4], RowEnd11[4], ColEnd11[4];  //用于储存没有偏移之前的边界点
	HTuple m_rowEdge[4], m_colEdge[4];  //四条边的扫描到的点数
	for (int i = 0;i < 4;i++)
	{
		if (m_param->IsFixWidth[i]) {
			setDetectOutData(FindOk, true, i);
			continue;
		}
		HTuple rowEdge, colEdge, amp, distance;
		char edge[32];
		if (m_param->whiteBlackDir[i]) //1白黑
			sprintf(edge, "negative");
		else
			sprintf(edge, "positive");
		double meaRow, meaCol, meaAngle;
		int meaLength1;
		double aangle;
		int pointN = 0;   //累计进行测试的点1数
		int usefulPoint = 0;  // 累1计有效得出结果的点数(结1果不为空)
		for (int j = 0;j < m_param->roiNum[i];j++)
		{
			if (i == 0 || i == 2) {  //上下
				meaRow = roiLfMidRow[i][j];
				meaCol = roiLfMidCol[i][j];
				meaLength1 = roiH[i][j] / 2;
				if (i == 0 && searchdir[i] == 0) {
					aangle = PI / 2.0;
				}
				else if (i == 0 && searchdir[i] == 1) {
					aangle = -PI / 2.0;
				}
				if (i == 2 && searchdir[i] == 0) {
					aangle = -PI / 2.0;
				}
				else if (i == 2 && searchdir[i] == 1) {
					aangle = PI / 2.0;
				}
				if (m_param->IsRefer) {
					aangle += rotateAngle;
				}
			}
			else if (i == 1 || i == 3) { //右左
				meaRow = roiTopMidRow[i][j];
				meaCol = roiTopMidCol[i][j];
				meaLength1 = roiW[i][j] / 2;

				if (i == 1 && searchdir[i] == 0) {
					aangle = 0;
				}
				else if (i == 1 && searchdir[i] == 1) {
					aangle = PI;
				}
				if (i == 3 && searchdir[i] == 0) {
					aangle = PI;
				}
				else if (i == 3 && searchdir[i] == 1) {
					aangle = 0;
				}
				if (m_param->IsRefer) {
					aangle += rotateAngle;
				}
			}
			gen_measure_rectangle2(meaRow, meaCol, aangle, meaLength1, m_param->extendSearchLine[i] * 2, Imgwidth[0], ImgHeight[0],/*"bilinear"*/"nearest_neighbor", &measureHandle);
			if (i == 2 || i == 0) {  //上下
				for (int startPoint = 0;startPoint < roiW[i][j];startPoint += m_param->searchLineGap[i]) {
					double transRow, transCol;

					transRow = (double)(int)(roiLfMidRow[i][j] - startPoint*sin(rotateAngle));
					transCol = (double)(int)(roiLfMidCol[i][j] + startPoint*cos(rotateAngle));
					if (transRow<2 || transRow>(ImgHeight[0].I() - 2) || transCol<2 || transCol>(Imgwidth[0].I() - 2)) {
						pointN++;
						continue;
					}
					translate_measure(measureHandle, transRow, transCol);

					measure_pos(detectImg, measureHandle, 1, m_param->minGrayDiff[i], edge, "first", &rowEdge, &colEdge, &amp, &distance);
					Hlong aa = rowEdge.Num();
					if (aa > 0) {
						m_rowEdge[i][usefulPoint] = rowEdge[0];
						m_colEdge[i][usefulPoint] = colEdge[0];
						usefulPoint++;
					}
					pointN++;
				}
			}
			else if (i == 1 || i == 3) {   //左右
				for (int startPoint = 0;startPoint < roiH[i][j];startPoint += m_param->searchLineGap[i]) {
					double transRow, transCol;
					transRow = (double)(int)(roiTopMidRow[i][j] + startPoint*cos(rotateAngle));
					transCol = (double)(int)(roiTopMidCol[i][j] + startPoint*sin(rotateAngle));
					if (transRow<2 || transRow>(ImgHeight[0].I() - 2) || transCol<2 || transCol>(Imgwidth[0].I() - 2)) {
						pointN++;
						continue;
					}
					translate_measure(measureHandle, transRow, transCol);

					measure_pos(detectImg, measureHandle, 1, m_param->minGrayDiff[i], edge, "first", &rowEdge, &colEdge, &amp, &distance);
					Hlong aa = rowEdge.Num();
					if (aa > 0) {
						m_rowEdge[i][usefulPoint] = rowEdge[0];
						m_colEdge[i][usefulPoint] = colEdge[0];
						usefulPoint++;
					}
					pointN++;
				}
			} //采集边界点结1束
			close_measure(measureHandle);
		}  //end of loop roinum j

		Hlong ab = m_rowEdge[i].Num();
		HTuple nc, nr, dist;
		if (ab >= 3) {
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
				setDetectOutData(FindOk, roiFound[i], i);
			}
			else {
				RowBegin11[i][0] = RowBegin[i][0];
				RowEnd11[i][0] = RowEnd[i][0];
				ColBegin11[i][0] = ColBegin[i][0];
				ColEnd11[i][0] = ColEnd[i][0];
				if (i == 0 || i == 2) {
					RowBegin[i][0] = m_param->lineOffset[i] + RowBegin[i][0].D();
					RowEnd[i][0] = m_param->lineOffset[i] + RowEnd[i][0].D();
				}
				else if (i == 1 || i == 3) {
					ColBegin[i][0] = m_param->lineOffset[i] + ColBegin[i][0].D();
					ColEnd[i][0] = m_param->lineOffset[i] + ColEnd[i][0].D();
				}
				if (((RowEnd[i][0].D() < RowBegin[i][0].D()) && (i == 1 || i == 3)) || ((ColEnd[i][0].D() < ColBegin[i][0].D()) && (i == 2 || i == 0))) {   //调转开头点和结尾点，使全部符合图片上的正向顺1序
					double tmp;
					tmp = ColEnd[i][0].D();
					ColEnd[i][0] = ColBegin[i][0];
					ColBegin[i][0] = tmp;
					tmp = RowEnd[i][0].D();
					RowEnd[i][0] = RowBegin[i][0];
					RowBegin[i][0] = tmp;
				}
				setDetectOutData(FindOk, roiFound[i], i);
			}
		}
		else if (ab < 3) {
			roiFound[i] = false;
			allok = false;
			setDetectOutData(FindOk, roiFound[i], i);
		}
		//close_measure(measureHandle);
	} //for loop end
	HTuple rows, cols;
	long totalNum = 0;
	for (int i = 0;i < 4;i++) {
		if (m_param->IsFixWidth[i]) {
			continue;
		}
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
	if ((rows.Num() == cols.Num()) && rows.Num() > 10) {   //得到的轮廓数目不能太少
		Hobject dispPointsImg;
		gen_region_points(&dispPointsImg, rows, cols);
		setDetectOutData(DispPointsImgOk, true);
		setDetectOutObject(DispPointsImg, dispPointsImg);
	}
	else {
		allok = false;
		setDetectOutData(DispPointsImgOk, false);
	}
	for (int i = 0;i < 4;i++) {
		if (!roiFound[i]) {
			allok = false;
			setDetectOutData(AllOK, allok);
			return allok;
		}
	}
	float EveryAngle[4];
	for (int i = 0;i < 4;i++) {
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
		if (((EveryAngle[i] * 180.0 / PI < 90 + m_param->maxAngle) && (EveryAngle[i] * 180.0 / PI > 90 - m_param->maxAngle)) ||
			((EveryAngle[i] * 180.0 / PI < 0 + m_param->maxAngle) && (EveryAngle[i] * 180.0 / PI > 0 - m_param->maxAngle))) {
			//m_angleOK=true;
		}
		else {
			allok = false;
			setDetectOutData(MaxAngleOK, false);
			setDetectOutData(MaxAngle, EveryAngle[i] / M_PI*180.0);
			setDetectOutData(AllOK, allok);
			return allok;
		}
	}
	setDetectOutData(MaxAngleOK, true);

	if ((m_param->IsFixWidth[0] && m_param->IsFixWidth[2])) {   // 上下
		RowBegin[0][0] = RowBegin[3][0].D();
		RowEnd[0][0] = RowBegin[1][0].D();
		ColBegin[0][0] = ColBegin[3][0].D();
		ColEnd[0][0] = ColBegin[1][0].D();

		RowBegin[2][0] = RowEnd[3][0].D();
		RowEnd[2][0] = RowEnd[1][0].D();
		ColBegin[2][0] = ColEnd[3][0].D();
		ColEnd[2][0] = ColEnd[1][0].D();
	}
	else if (m_param->IsFixWidth[1] && m_param->IsFixWidth[3]) {  //左右
		RowBegin[1][0] = RowEnd[0][0].D();
		RowEnd[1][0] = RowEnd[2][0].D();
		ColBegin[1][0] = ColEnd[0][0].D();
		ColEnd[1][0] = ColEnd[2][0].D();

		RowBegin[3][0] = RowBegin[0][0].D();
		RowEnd[3][0] = RowBegin[2][0].D();
		ColBegin[3][0] = ColBegin[0][0].D();
		ColEnd[3][0] = ColBegin[2][0].D();
	}
	else {
		if (m_param->IsFixWidth[0]) {
			RowBegin[0][0] = RowBegin[2][0].D() - m_param->fixWidth[0];
			RowEnd[0][0] = RowEnd[2][0].D() - m_param->fixWidth[0];
			ColBegin[0][0] = ColBegin[2][0].D();
			ColEnd[0][0] = ColEnd[2][0].D();
		}
		if (m_param->IsFixWidth[2]) {
			RowBegin[2][0] = RowBegin[0][0].D() + m_param->fixWidth[2];
			RowEnd[2][0] = RowEnd[0][0].D() + m_param->fixWidth[2];
			ColBegin[2][0] = ColBegin[0][0].D();
			ColEnd[2][0] = ColEnd[0][0].D();
		}
		if (m_param->IsFixWidth[1]) {
			RowBegin[1][0] = RowBegin[3][0].D();
			RowEnd[1][0] = RowEnd[3][0].D();
			ColBegin[1][0] = ColBegin[3][0].D() + m_param->fixWidth[1];
			ColEnd[1][0] = ColEnd[3][0].D() + m_param->fixWidth[1];
		}
		if (m_param->IsFixWidth[3]) {
			RowBegin[3][0] = RowBegin[1][0].D();
			RowEnd[3][0] = RowEnd[1][0].D();
			ColBegin[3][0] = ColBegin[1][0].D() - m_param->fixWidth[3];
			ColEnd[3][0] = ColEnd[1][0].D() - m_param->fixWidth[3];
		}
	}
	for (int i = 0;i < 4;i++) {
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
	for (int i = 0;i < 4;i++) {
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
			setDetectOutData(AllOK, allok);
			return allok;
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
		setDetectOutData(AllOK, allok);
		return allok;
	}
	setDetectOutData(CenterRow, centerRoww);
	setDetectOutData(CenterCol, centerColl);

	HTuple fnalRow, fnalCol;
	double frow[5], fcol[5];   //finalEndRow 顺序是右上角顺时针
	for (int i = 0;i < 4;i++) {
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
	int numm = fnalRow.Num();
	for (auto i = 0;i < numm;++i)
	{
		double x = fnalCol[i].D();
		double y = fnalRow[i].D();
		p_corners->push_back(cv::Point2d(x, y));
	}
	//setDetectOutData(FinalRow, fnalRow);  //左上角顺时针
	//setDetectOutData(FinalCol, fnalCol);

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
			if (((tmpAngle[i] * 180.0) / PI < (44 + 90)) && ((tmpAngle[i] * 180.0) / PI > (-44 + 90))) {
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
		dis = sqrt((finalEndRow[0][0].D() - finalEndRow[3][0].D())*(finalEndRow[0][0].D() - finalEndRow[3][0].D()) +
			(finalEndCol[0][0].D() - finalEndCol[3][0].D())*(finalEndCol[0][0].D() - finalEndCol[3][0].D()));
		dis *= mk;
		if (maxwidth < dis)
			maxwidth = dis;
		if (minwidth > dis)
			minwidth = dis;
		dis = sqrt((finalEndRow[1][0].D() - finalEndRow[2][0].D())*(finalEndRow[1][0].D() - finalEndRow[2][0].D()) +
			(finalEndCol[1][0].D() - finalEndCol[2][0].D())*(finalEndCol[1][0].D() - finalEndCol[2][0].D()));
		dis *= mk;
		if (maxwidth < dis)
			maxwidth = dis;
		if (minwidth > dis)
			minwidth = dis;
		if (maxwidth > m_param->errWidthMax || minwidth < m_param->errWidthMin) {
			errwidthok = false;
			allok = false;
		}
		setDetectOutData(ErrWidthOk, errwidthok);
		setDetectOutData(ErrWidthMax, maxwidth);
		setDetectOutData(ErrWidthMin, minwidth);
	}
	if (m_param->IsErrHeight)
	{
		bool errheightok = true;
		float maxheight = 0;
		float minheight = 10000;
		float dis;
		dis = sqrt((finalEndRow[0][0].D() - finalEndRow[1][0].D())*(finalEndRow[0][0].D() - finalEndRow[1][0].D()) +
			(finalEndCol[0][0].D() - finalEndCol[1][0].D())*(finalEndCol[0][0].D() - finalEndCol[1][0].D()));
		dis *= mk;
		if (maxheight < dis)
			maxheight = dis;
		if (minheight > dis)
			minheight = dis;
		dis = sqrt((finalEndRow[2][0].D() - finalEndRow[3][0].D())*(finalEndRow[2][0].D() - finalEndRow[3][0].D()) +
			(finalEndCol[2][0].D() - finalEndCol[3][0].D())*(finalEndCol[2][0].D() - finalEndCol[3][0].D()));
		dis *= mk;
		if (maxheight < dis)
			maxheight = dis;
		if (minheight > dis)
			minheight = dis;
		if (maxheight > m_param->errHeightmax || minheight < m_param->errHeightMin) {
			errheightok = false;
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
		Centershift = sqrt(pow((centerRoww[0].D() - m_param->TmpCenterRow), 2) + pow((centerColl[0].D() - m_param->TmpCenterCol), 2));
		Centershift *= mk;
		if (Centershift > m_param->errCenterOffset) {
			allok = false;
			centerok = false;
		}
		setDetectOutData(ErrCenterShift, Centershift);
		setDetectOutData(ErrCenterShiftOk, centerok);
	}

	if (m_param->IsUseCompare && m_param->IsOffArea) {
		Hobject inputpolyrec;
		gen_region_polygon_filled(&inputpolyrec, inputReferRow, inputReferCol);
		Hobject polyrecLarge;
		if (m_param->offThickMin < 0) {
			dilation_rectangle1(inputpolyrec, &inputpolyrec, -m_param->offThickMin, -m_param->offThickMin);
		}
		//else{
		//	copy_obj(inputpolyrec,&polyrecLarge,1,1);
		//}
		Hobject mpolyrect, mpolyLarge;
		gen_region_polygon_filled(&mpolyrect, fnalRow, fnalCol);
		if (m_param->offThickMin > 0) {
			dilation_rectangle1(mpolyrect, &mpolyLarge, m_param->offThickMin, m_param->offThickMin);
		}
		else {
			copy_obj(mpolyrect, &mpolyLarge, 1, 1);
		}
		Hobject sub;
		difference(inputpolyrec, mpolyLarge, &sub);
		connection(sub, &sub);
		Hlong subNum;
		count_obj(sub, &subNum);
		if (subNum <= 0) {
			setDetectOutData(DiffNotExist, 1);
			setDetectOutData(AllOK, allok);
			return allok;
		}
		else {
			setDetectOutData(DiffNotExist, 0);
			Hobject sub1;
			difference(inputpolyrec, mpolyrect, &sub1);  //因为此处的被减数面积一定更小，所以只要前一次相减之后有面积此处必有
			connection(sub1, &sub1);
			Hobject usub1;
			union1(sub1, &usub1);
			setDetectOutObject(DiffRegion, usub1);
			Hobject singleMax;
			select_shape_std(sub1, &singleMax, "max_area", 95);
			HTuple subarea, subrow, subcol;
			area_center(singleMax, &subarea, &subrow, &subcol);
			int tmparea = subarea[0].I();
			if (m_param->IsOffArea) {
				bool thisok = true;
				if (tmparea > m_param->errOffAreaMin) {
					allok = false;
					thisok = false;
				}
				setDetectOutData(OffAreaVal, tmparea);
				setDetectOutData(OffAreaOk, thisok);
			}
		}
	}
	setDetectOutData(AllOK, allok);
	return allok;
}

void edgeJudgeMod::viewResult(ImageView *iv, const QString &funName, int)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}

	//HTuple threok;
	//getDispOutData(ThreRegionOk,threok);
	//if (threok[0].I()){
	//	if (m_param->IsShowSegDiff){
	//		iv->setColor(255,255,0);
	//		iv->setFilled(false);
	//		iv->dispRegion(*getDispObject(ThreRegion));
	//	}
	//}
	//else {
	//	return;
	//}

	HTuple imgok;
	getDispOutData(DispPointsImgOk, imgok);
	iv->setFilled(true);
	iv->setColor(255, 0, 0);
	if (imgok[0].I() == 1) {
		iv->setColor(0, 255, 0);
		Hlong num;
		const Hobject *obj = getDispObject(DispPointsImg);
		count_obj(*obj, &num);
		if (num > 0 && m_param->IsShowRegionAfterDet) {
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
				gen_rectangle1(&roirec, 
					m_param->RoiRegionVector[beforeNum + j].row1, m_param->RoiRegionVector[beforeNum + j].col1,
					m_param->RoiRegionVector[beforeNum + j].row2, m_param->RoiRegionVector[beforeNum + j].col2);
				if (m_param->IsRefer) {
					affine_trans_region(roirec, &roirec, homat, "false");
				}
				RlRegion rlReg;
				HalOpenCV::hal2cvRegion(roirec, &rlReg);
				iv->dispRegion(&rlReg);
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
			iv->dispLine(rowbegin[0].D(), colbegin[0].D(), rowend[0].D(), colend[0].D());

			//iv->dispLine(rowbegin[0].D(),colbegin[0].D()-3,rowbegin[0].D(),colbegin[0].D()+3);
			//iv->dispLine(rowbegin[0].D()-3,colbegin[0].D(),rowbegin[0].D()+3,colbegin[0].D());
			//iv->dispLine(rowend[0].D(),colend[0].D()-3,rowend[0].D(),colend[0].D()+3);
			//iv->dispLine(rowend[0].D()-3,colend[0].D(),rowend[0].D()+3,colend[0].D());
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
	if (m_param->IsShowRegionAfterDet) {
		RlRegion reg;
		HalOpenCV::hal2cvRegion(polyRec, &reg);
		iv->dispRegion(&reg);
		//iv->dispRegion(polyRec);
	}
	HTuple centerRow, centerCol;
	getDispOutData(CenterRow, centerRow);
	getDispOutData(CenterCol, centerCol);
	iv->dispLine(centerRow[0].D(), centerCol[0].D() - 3, centerRow[0].D(), centerCol[0].D() + 3);
	iv->dispLine(centerRow[0].D() - 3, centerCol[0].D(), centerRow[0].D() + 3, centerCol[0].D());
	if (m_param->IsUseCompare&&m_param->IsOffArea) {
		HTuple diffareaok, diffnotexist;
		getDispOutData(DiffNotExist, diffnotexist);
		if (diffnotexist[0].I() == 0) {
			if (m_param->IsShowSegDiff) {
				iv->setColor(255, 0, 255);
				iv->setFilled(1);
				RlRegion reg;
				HalOpenCV::hal2cvRegion(*getDispObject(DiffRegion), &reg);
				iv->dispRegion(&reg);
			}
		}
		else {
			return;
		}
	}
}

void edgeJudgeMod::textResult(ResultText *text, const QString &funName)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}

	//HTuple threok;
	//getDispOutData(ThreRegionOk,threok);
	//if (threok[0].I()){
	//}
	//else {
	//	return;
	//}

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
		text->append(QObject::tr("Body Width = (%1,%2) of (%3,%4)").arg(QString::number(errwidthmin[0].D(), 'f', 3)).arg(QString::number(errwidthmax[0].D(), 'f', 3)).
			arg(QString::number(m_param->errWidthMin, 'f', 3)).arg(QString::number(m_param->errWidthMax, 'f', 3)));
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
		text->append(QObject::tr("Body Height= (%1,%2) of (%3,%4)").arg(QString::number(errheightmin[0].D(), 'f', 3)).arg(QString::number(errheightmax[0].D(), 'f', 3)).
			arg(QString::number(m_param->errHeightMin, 'f', 3)).arg(QString::number(m_param->errHeightmax, 'f', 3)));
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
		text->append(QObject::tr("Body Shift= %1(%2)").arg(QString::number(errcenter[0].D(), 'f', 3)).
			arg(QString::number(m_param->errCenterOffset, 'g', 3)));
	}
	if (m_param->IsUseCompare&&m_param->IsOffArea) {
		HTuple diffareaok, diffnotexist;
		getDispOutData(DiffNotExist, diffnotexist);
		if (diffnotexist[0].I() == 0) {
			if (m_param->IsOffArea) {
				HTuple offareaok, offareaval;
				getDispOutData(OffAreaOk, offareaok);
				getDispOutData(OffAreaVal, offareaval);
				if (offareaok[0].I() == 0) {
					text->setTextColor(QColor(255, 0, 0));
				}
				else {
					text->setTextColor(QColor(0, 0, 0));
				}
				text->append(QObject::tr("area byond= %1(%2)").arg(QString::number(offareaval[0].I())).
					arg(QString::number(m_param->errOffAreaMin)));
			}
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
			text->append(QObject::tr("no area beyond."));
		}
	}

	text->setTextColor(QColor(0, 0, 0));
}

void edgeJudgeMod::iniData()
{
	m_param = new edgeJudgeParam;
	m_image = new Hobject;
	createPins();
}

void edgeJudgeMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2d, "hm2d");
	addPin(&p_pixSize, "sz");
	addPin(&p_inCorners, "inCorners");
	addPin(&p_inReferRegion, "referReg");

	addPin(&p_corners, "corners");

	p_pixSize.setVisible(false);
	p_inReferRegion.setVisible(false);

	p_corners.setVisible(false);
}

