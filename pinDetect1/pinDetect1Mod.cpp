#include "pinDetect1Mod.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include <opencv2/imgproc.hpp>
#include "halOpenCV.h"
#include <random>
#include <vector>
#include <memory.h>
#include <set>

#define MAX_TESTITEMS_NUM 16

using namespace std;

#define AllOK "allok"
#define FindOk "findok"  //是否有检测到管脚
#define GrpFindOk "grpfindok"  //综合每个组的是否ok
#define RegionSingleOk "regionSingleOk"   //比对误差范围后的各单个管脚区域是1否OK  
#define RegionOk "regionOk" //比对误差范围后的各管脚区域是1否OK   

#define BeginRow "BeginEdgeRow"   //检测端点时候的端点坐1标,包括底线的
#define BeginCol "BeginEdgeCol"
#define EndRow "EndEdgeRow"
#define EndCol "EndEdgeCol"
//肩部坐标
#define ShRowBegin "SHRowBegin" 
#define ShRowEnd "SHRowEnd" 
#define ShColBegin "SHColBegin" 
#define ShColEnd "SHColEnd"
#define ShRowCenter "SHRowCenter" 
#define ShColCenter "SHColCenter" 

#define MaxWidthBeginRow "MaxWidthBeginEdgeRow"   //最大宽度的坐标
#define MaxWidthBeginCol "MaxWidthBeginEdgeCol"
#define MaxWidthEndRow "MaxWidthEndEdgeRow"
#define MaxWidthEndCol "MaxWidthEndEdgeCol"

#define MinSweepBeginRow "MinSweepBeginEdgeRow"   //最小sweep的坐标
#define MinSweepBeginCol "MinSweepBeginEdgeCol"
#define MinSweepEndRow "MinSweepEndEdgeRow"
#define MinSweepEndCol "MinSweepEndEdgeCol"

#define MaxSweepBeginRow "MaxSweepBeginEdgeRow"   //最大sweep的坐标
#define MaxSweepBeginCol "MaxSweepBeginEdgeCol"
#define MaxSweepEndRow "MaxSweepEndEdgeRow"
#define MaxSweepEndCol "MaxSweepEndEdgeCol"


#define PinTopRow "PinTopRow" //站高区域的顶点坐标
#define PinTopCol "PinTopCol" 

#define lineBegRow "lineBegRow"   
#define lineBegCol "lineBegCol"
#define lineEndRow "lineEndRow"
#define lineEndCol "lineEndCol"


#define DispPointsImgOk "dispImgOk"  //是否有轮廓检测到
#define DispPointsImg "displayPointsImg"

#define PinInterVal "pinInterval"  //管脚间1距
#define PinWidth "pinWidth"        //管脚宽1度
#define PinMaxWidth "pinMaxWidth"  //管脚最大宽度

#define PinMinSweep "pinMinSweep"        //管脚sweep min
#define PinMaxSweep "pinMaxSweep"  //管脚最大sweep


#define StandOff "standOff"        //管脚站1高
#define StandOffDiff "standoffDiff"  //管脚站高差1距

#define PinInterValOK "pinIntervalOk"  //管脚间1距
#define PinWidthOK "pinWidthOk"        //管脚宽1度
#define PinSweepOK "pinSweepOk"        //管脚sweep
#define StandOffOK "standOffOk"        //管脚站1高
#define StandOffDiffOK "standoffDiffOk"  //管脚站高差1距

//InData
#define ReferRow "referRow"   //前一模块传进来的基准坐标，左上角顺时针
#define ReferCol "referCol"

#define HomMat2D1 "homMat2d1"   //传入的旋转矩阵 
#define HomMat2D2 "homMat2d2"   //传入的旋转矩阵

//传入的搜索区域
#define SearchReferRow "searchReferRow"   //左上角顺时针顺序，传进来的区域4个点
#define SearchReferCol "searchReferCol"
#define TmpSearchReferRow "TmpSearchReferRow"   //左上角顺时针顺序，传进来的区域4个点
#define TmpSearchReferCol "TmpSearchReferCol"

#define TmpHomMat2D1 "tmphomMat2d1"   //传入的旋转矩阵 
#define TmpHomMat2D2 "tmphomMat2d2"   //传入的旋转矩阵

#define SearchHomMat2D "searchHomMat2D" //搜索区域旋转矩阵
#define TmpSearchHomMat2D "tmpsearchHomMat2D" 

//标定数据进1入
#define pixelSize "pixelSize"  //5,中左右上下
#define ImageFull "Imagefull"  //图片是否ok

#define DelReion "delRegion"

pinDetect1Mod::pinDetect1Mod()
{
	iniData();
}

pinDetect1Mod::pinDetect1Mod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

pinDetect1Mod::~pinDetect1Mod()
{
	delete m_image;
	delete m_param;
}

int pinDetect1Mod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int pinDetect1Mod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void pinDetect1Mod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void pinDetect1Mod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int pinDetect1Mod::run(const QString &funName)
{
	p_outLead->clear();
	p_outLen->clear();

	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
		m_testItemsStatus[i] = 1;
	}

	bool allok = true;
	if (p_im->empty()) {
		allok = false;
		setDetectOutData(AllOK, allok);
		setDetectOutData(ImageFull, false);
		m_testItemsStatus[0] = 0;
		return -1;
		//return allok;
	}
	else {
		setDetectOutData(ImageFull, true);
		HalOpenCV::cv2halImg(*p_im, m_image, false);
	}

	double mk[GRP_NUM];
	for (int i = 0;i < GRP_NUM;i++) {
		if (m_param->IsInputRatio[i]) {
// 			try {
// 				HTuple k;
// 				getDetectInData(pixelSize, k);
// 				mk[i] = k[0].D();
// 			}
// 			catch (HException& ex) {
// 				mk[i] = m_param->sizeRatio[i];
// 			}
			mk[i] = *p_pixelSize;
		}
		else {
			mk[i] = m_param->sizeRatio[i];
		}
	}
	int roiFound[GRP_NUM][11]; //各边界是否找1到 ,0-成功，1-边界拟合失败，2-肩部查找失败，3-宽度查找失败
	bool GrpfindOk[GRP_NUM];    //每组是否有正常找出管脚
	bool regionsingleok[GRP_NUM][11]; //找出来的每组管脚对照误差范围后是否正确
	bool regionok[GRP_NUM]; //找出来的每组管脚对照误差范围后是否正确
	for (int i = 0; i < GRP_NUM;i++) {
		for (int j = 0;j < 11;j++) {
			roiFound[i][j] = 0;
			regionsingleok[i][j] = true;
		}
		regionok[i] = true;
		GrpfindOk[i] = true;
	}
	double roirow[GRP_NUM][11], roicol[GRP_NUM][11];  //roi区域的中心
	int searchDir[GRP_NUM]; //0为检边界的框子，后续为管脚框
	int roiW[GRP_NUM][11], roiH[GRP_NUM][11];    //roi宽度 高度
	double roiTopMidRow[GRP_NUM][11], roiTopMidCol[GRP_NUM][11];
	double roiLfMidRow[GRP_NUM][11], roiLfMidCol[GRP_NUM][11];  //roi上方的中点,以及左边的中点，便于后面搜索用
	double roiangle[GRP_NUM][11];

	HTuple searchHommat;
	HTuple roireferrow, roirefercol; //外部传入搜索区域

	if (m_param->IsDelOffSearchRoi) {
		if (m_param->IsUseInputRegion) {
			for (auto i = 0;i < p_SearchCorners->size();++i)
			{
				roireferrow[i] = p_SearchCorners->at(i).y;
				roirefercol[i] = p_SearchCorners->at(i).x;
			}
			//getDetectInData(SearchReferRow, roireferrow);
			//getDetectInData(SearchReferCol, roirefercol);
			try {
				double testrow = roireferrow[3].D();
			}
			catch (HException& ex) {
				roireferrow[0] = roirefercol[0] = 50;
				roireferrow[1] = roirefercol[3] = 50;
				roireferrow[3] = roirefercol[1] = 150;
				roireferrow[2] = roirefercol[2] = 150;
			}
			//对传进来的坐标进行偏置
			roireferrow[0] = roireferrow[0].D() + m_param->offsetUp;
			roireferrow[1] = roireferrow[1].D() + m_param->offsetUp;
			roireferrow[2] = roireferrow[2].D() + m_param->offsetDown;
			roireferrow[3] = roireferrow[3].D() + m_param->offsetDown;
			roireferrow[4] = roireferrow[0].D();
			roirefercol[0] = roirefercol[0].D() + m_param->offsetLeft;
			roirefercol[1] = roirefercol[1].D() + m_param->offsetRight;
			roirefercol[2] = roirefercol[2].D() + m_param->offsetRight;
			roirefercol[3] = roirefercol[3].D() + m_param->offsetLeft;
			roirefercol[4] = roirefercol[0].D();
			setDetectOutData(TmpSearchReferRow, roireferrow);
			setDetectOutData(TmpSearchReferCol, roirefercol);
		}
		else {
// 			try {
// 				getDetectInData(SearchHomMat2D, searchHommat);
// 				double tv = searchHommat[0].D();
// 			}
// 			catch (HException& ex) {
// 				hom_mat2d_identity(&searchHommat);
// 			}
			if (p_SearchHomMat2D->size() == cv::Size(2, 3))
				HalOpenCV::cv2HalHomMat2D(*p_SearchHomMat2D, &searchHommat);
			else 
				hom_mat2d_identity(&searchHommat);
			setDetectOutData(TmpSearchHomMat2D, searchHommat);
		}

	}
	HTuple hommat;
	for (int i = 0;i < GRP_NUM;i++) {
		if (m_param->IsRefer[i]) {
// 			try {
// 				getDetectInData(m_outHomDefine[i], hommat);
// 				double tv = hommat[0].D();
// 			}
// 			catch (HException& ex) {
// 				hom_mat2d_identity(&hommat);
// 			}
			if (p_homMat2d[i]->size() == cv::Size(2, 3))
				HalOpenCV::cv2HalHomMat2D(*p_homMat2d[i], &hommat);
			else
				hom_mat2d_identity(&hommat);
			setDetectOutData(m_tmpHomDefine[i], hommat);
		}
		for (int j = 0;j < m_param->grpPinNum[i] + 1;j++) {
			int idx = j;
			roiW[i][j] = m_param->RoiVec[i][idx].col2 - m_param->RoiVec[i][idx].col1;
			roiH[i][j] = m_param->RoiVec[i][idx].row2 - m_param->RoiVec[i][idx].row1;
			roirow[i][j] = (m_param->RoiVec[i][idx].row2 + m_param->RoiVec[i][idx].row1) / 2.0;
			roicol[i][j] = (m_param->RoiVec[i][idx].col2 + m_param->RoiVec[i][idx].col1) / 2.0;
			roiTopMidRow[i][j] = m_param->RoiVec[i][idx].row1;
			roiTopMidCol[i][j] = (m_param->RoiVec[i][idx].col1 + m_param->RoiVec[i][idx].col2) / 2.0;
			roiLfMidRow[i][j] = (m_param->RoiVec[i][idx].row1 + m_param->RoiVec[i][idx].row2) / 2.0;
			roiLfMidCol[i][j] = m_param->RoiVec[i][idx].col1;
			roiangle[i][j] = 0;
			if (m_param->IsRefer[i]) {
				affine_trans_pixel(hommat, roiTopMidRow[i][j], roiTopMidCol[i][j], &roiTopMidRow[i][j], &roiTopMidCol[i][j]);
				affine_trans_pixel(hommat, roiLfMidRow[i][j], roiLfMidCol[i][j], &roiLfMidRow[i][j], &roiLfMidCol[i][j]);
				affine_trans_pixel(hommat, roirow[i][j], roicol[i][j], &roirow[i][j], &roicol[i][j]);
				roiangle[i][j] = atan((hommat[3].D() / hommat[0].D()));
			}
		}
		searchDir[i] = m_param->searchDir[i];
		if (m_param->IsReferEdgeRev) {
			if (searchDir[i] == 0 || searchDir[i] == 1) {
				searchDir[i] = searchDir[i] == 0 ? 1 : 0;
			}
			else {
				searchDir[i] = searchDir[i] == 2 ? 3 : 2;
			}
		}
	}
	Hlong measureHandle;
	HTuple Imgwidth, ImgHeight;
	HTuple RowBegin[GRP_NUM][11], ColBegin[GRP_NUM][11], RowEnd[GRP_NUM][11], ColEnd[GRP_NUM][11];  //找到的每组管脚的两个边界点,最后一个为底线的两个点，默认存储的是宽度最小处的坐标
	HTuple RowWidMaxBegin[GRP_NUM][11], ColWidMaxBegin[GRP_NUM][11], RowWidMaxEnd[GRP_NUM][11], ColWidMaxEnd[GRP_NUM][11];//同上，但是存储宽度最大处的坐标

	HTuple RowSweepMinBegin[GRP_NUM][11], ColSweepMinBegin[GRP_NUM][11], RowSweepMinEnd[GRP_NUM][11], ColSweepMinEnd[GRP_NUM][11];  //找到的每组管脚sweep最小处的坐标
	HTuple RowSweepMaxBegin[GRP_NUM][11], ColSweepMaxBegin[GRP_NUM][11], RowSweepMaxEnd[GRP_NUM][11], ColSweepMaxEnd[GRP_NUM][11];//同上，但是存储sweep最大处的坐标

	HTuple SHRowBegin[GRP_NUM][11], SHRowEnd[GRP_NUM][11], SHColBegin[GRP_NUM][11], SHColEnd[GRP_NUM][11]; //求管脚肩部时候的参数
	HTuple SHRowCenter[GRP_NUM][11], SHColCenter[GRP_NUM][11];   //肩部的中心点

	HTuple lineRowBegin[GRP_NUM][11], lineColBegin[GRP_NUM][11], lineRowEnd[GRP_NUM][11], lineColEnd[GRP_NUM][11];    //直线拟合出来的线的两个端点，与RowBegin[][](拟合线与管脚的交点)不同
	HTuple RowTop[GRP_NUM][11], ColTop[GRP_NUM][11];   //管脚的中心点
	HTuple pinWid[GRP_NUM][11];                       //管脚宽度最小值
	HTuple pinWidMax[GRP_NUM][11];                    //管脚宽度最大值

	HTuple pinSweepMin[GRP_NUM][11];                       //管脚sweep最小值
	HTuple pinSweepMax[GRP_NUM][11];                    //管脚sweep最大值

	HTuple pinStandOff[GRP_NUM][11];                  //管脚站高
	HTuple pinInterVal[GRP_NUM][10];                   //管脚间距
	HTuple pinStandDiff[GRP_NUM];                     //管脚站高差异
	Hobject pointsobj[GRP_NUM];


	HTuple outLength;  //传出用的长度变量
	HTuple outLeadRow, outLeadCol; //传输到外部的管脚尖坐标
	HTuple referrow, refercol;
	if (m_param->IsDetBase == 2) {
		//getDetectInData(ReferRow, referrow);
		//getDetectInData(ReferCol, refercol);
		for (auto i = 0;i < p_corners->size();++i)
		{
			referrow[i] = p_corners->at(i).y;
			refercol[i] = p_corners->at(i).x;
		}
	}
	get_image_size(*m_image, &Imgwidth, &ImgHeight);
	Hobject imgRec;
	gen_rectangle1(&imgRec, 0, 0, ImgHeight[0].I() - 1, Imgwidth[0].I() - 1);
	//Hobject tosearchImg;
	Hobject ToSearchRoi;
	if (m_param->IsDelOffSearchRoi) {
		if (m_param->IsUseInputRegion) {
			Hobject inputroi;
			gen_region_polygon_filled(&ToSearchRoi, roireferrow, roirefercol);
		}
		else {
			Hobject rec2reg;
			gen_rectangle2(&rec2reg, m_param->SearchEdgeRoi.row, m_param->SearchEdgeRoi.col, m_param->SearchEdgeRoi.angle,
				m_param->SearchEdgeRoi.len1, m_param->SearchEdgeRoi.len2);
			affine_trans_region(rec2reg, &ToSearchRoi, searchHommat, "true");
			//difference(imgRec,rec2reg,&rec2reg);
			//paint_region(rec2reg,*m_image,&tosearchImg,m_param->backGray[0],"fill");
		}
	}
	for (int i = 0;i < GRP_NUM;i++) {
		if (m_param->grpPinNum[i] == 0) {
			continue;
		}
		HTuple m_rowEdge[11], m_colEdge[11];  //四组扫描到的管脚尖部点
											  //int i1;  //新的i指数   
		bool redoCycle = false;   //在管脚检测失败的时候修改灰度并重复执行
		int redoCount = 0;
		for (int j = 0;j < m_param->grpPinNum[i] + 1;j++) {    //第一个先对底部边界线进行检测，其后是各管脚
			Hobject edgeObj;
			Hobject detImg; //用于检测的图片
			Hobject detDelRoi; //用于排除的区域
			Hobject roirec;
			HTuple nc, nr, dist;
			redoCycle = false;

			if (j == 0) {
				if (m_param->IsDetBase == 0) {                            //==0 不检底线
					continue;
				}
				else if (m_param->IsDetBase == 2) {         //==2 传入底线
					try {
						RowBegin[i][j][0] = referrow[m_param->InputBaseIndex[i]].D();
						RowEnd[i][j][0] = referrow[m_param->InputBaseIndex[i] + 1].D();
						ColBegin[i][j][0] = refercol[m_param->InputBaseIndex[i]].D();
						ColEnd[i][j][0] = refercol[m_param->InputBaseIndex[i] + 1].D();
						int num1 = referrow.Num();
						int num2 = refercol.Num();
						double Cool = refercol[m_param->InputBaseIndex[i] + 1].D();

						if (m_param->InputBaseIndex[i] == 0 || m_param->InputBaseIndex[i] == 2) {
							RowBegin[i][j][0] = RowBegin[i][j][0].D() + m_param->baseLineOffset[i];
							RowEnd[i][j][0] = RowEnd[i][j][0].D() + m_param->baseLineOffset[i];
						}
						else if (m_param->InputBaseIndex[i] == 1 || m_param->InputBaseIndex[i] == 3) {
							ColBegin[i][j][0] = ColBegin[i][j][0].D() + m_param->baseLineOffset[i];
							ColEnd[i][j][0] = ColEnd[i][j][0].D() + m_param->baseLineOffset[i];

						}
					}
					catch (HException& ex) {
						referrow[4] = refercol[4] = referrow[0] = refercol[0] = 50;
						referrow[1] = refercol[3] = 50;
						referrow[3] = refercol[1] = 150;
						referrow[2] = refercol[2] = 150;
						RowBegin[i][j][0] = referrow[m_param->InputBaseIndex[i]].D();
						RowEnd[i][j][0] = referrow[m_param->InputBaseIndex[i] + 1].D();
						ColBegin[i][j][0] = refercol[m_param->InputBaseIndex[i]].D();
						ColEnd[i][j][0] = refercol[m_param->InputBaseIndex[i] + 1].D();
						int num1 = referrow.Num();
						int num2 = refercol.Num();
						double Cool = refercol[m_param->InputBaseIndex[i] + 1].D();
						if (m_param->InputBaseIndex[i] == 0 || m_param->InputBaseIndex[i] == 2) {
							RowBegin[i][j][0] = RowBegin[i][j][0].D() + m_param->baseLineOffset[i];
							RowEnd[i][j][0] = RowEnd[i][j][0].D() + m_param->baseLineOffset[i];
						}
						else if (m_param->InputBaseIndex[i] == 1 || m_param->InputBaseIndex[i] == 3) {
							ColBegin[i][j][0] = ColBegin[i][j][0].D() + m_param->baseLineOffset[i];
							ColEnd[i][j][0] = ColEnd[i][j][0].D() + m_param->baseLineOffset[i];

						}
					}
					setDetectOutData(FindOk, roiFound[i][j], i, j);
					continue;
				}

			}
			char edge[32];
			if (m_param->whiteBlackDir[i]) //1白黑
				sprintf(edge, "negative");
			else
				sprintf(edge, "positive");
			double meaRow, meaCol, meaAngle;
			int meaLength1;
			if (searchDir[i] == 0) {  //左右
				meaRow = roiTopMidRow[i][j];
				meaCol = roiTopMidCol[i][j];
				meaLength1 = roiW[i][j] / 2;
				if (m_param->IsDetBase == 1 && j == 0 && m_param->IsBaseInvert[i])
					meaAngle = PI + roiangle[i][j];
				else
					meaAngle = 0 + roiangle[i][j];
			}
			else if (searchDir[i] == 1) { //右左
				meaRow = roiTopMidRow[i][j];
				meaCol = roiTopMidCol[i][j];
				meaLength1 = roiW[i][j] / 2;
				if (m_param->IsDetBase == 1 && j == 0 && m_param->IsBaseInvert[i])
					meaAngle = 0 + roiangle[i][j];
				else
					meaAngle = PI + roiangle[i][j];
			}
			else if (searchDir[i] == 2) { //上下
				meaRow = roiLfMidRow[i][j];
				meaCol = roiLfMidCol[i][j];
				meaLength1 = roiH[i][j] / 2;
				if (m_param->IsDetBase == 1 && j == 0 && m_param->IsBaseInvert[i])
					meaAngle = PI / 2.0 + roiangle[i][j];
				else
					meaAngle = -PI / 2.0 + roiangle[i][j];
			}
			else if (searchDir[i] == 3) { //下上
				meaRow = roiLfMidRow[i][j];
				meaCol = roiLfMidCol[i][j];
				meaLength1 = roiH[i][j] / 2;
				if (m_param->IsDetBase == 1 && j == 0 && m_param->IsBaseInvert[i])
					meaAngle = -PI / 2.0 + roiangle[i][j];
				else
					meaAngle = PI / 2.0 + roiangle[i][j];
			}
			gen_measure_rectangle2(meaRow, meaCol, meaAngle, meaLength1, m_param->extendSearchLine[i] * 2, Imgwidth[0], ImgHeight[0],/*"bilinear"*/"nearest_neighbor", &measureHandle);
			gen_rectangle2(&roirec, roirow[i][j], roicol[i][j], roiangle[i][j], roiW[i][j] / 2.0, roiH[i][j] / 2.0);
			if (j != 0) {
				reduce_domain(*m_image, roirec, &roirec);

				Hobject meanimg;
				mean_image(roirec, &meanimg, 31/*m_param->minArea[2]*/, 31/*m_param->minArea[2]*/);
				dyn_threshold(roirec, meanimg, &roirec, m_param->noiseGray[i], "light");

				//threshold(roirec,&roirec,m_param->noiseGray[i],255);
				opening_rectangle1(roirec, &roirec, 1, 1);
				connection(roirec, &roirec);
				select_shape(roirec, &roirec, "area", "and", 0, m_param->minArea[i]);
				Hlong noisenum;
				count_obj(roirec, &noisenum);
				if (noisenum > 0) {
					//paint_region(roirec,*m_image,&detImg,m_param->backGray[i],"fill");
					Hobject tmpUnion;
					int mvDis = 3/*m_param->backGray[i]*/;
					if (mvDis > 0) {
						for (int n = 0;n < noisenum;n++) { //进行指向性膨胀
							Hobject singleobj, moveObj;
							select_obj(roirec, &singleobj, n + 1);
							Hlong mvRow, mvCol;

							int krow[4] = { 0,0,-1,1 };
							int kcol[4] = { -1,1,0,0 };

							move_region(singleobj, &moveObj, mvDis*krow[m_param->searchDir[i]], mvDis*kcol[m_param->searchDir[i]]);
							union2(moveObj, singleobj, &singleobj);
							if (m_param->searchDir[i] == 0 || m_param->searchDir[i] == 1) {
								closing_rectangle1(singleobj, &singleobj, mvDis, 1);
							}
							else {
								closing_rectangle1(singleobj, &singleobj, 1, mvDis);
							}
							if (n == 0) {
								copy_obj(singleobj, &tmpUnion, 1, 1);
							}
							else {
								concat_obj(tmpUnion, singleobj, &tmpUnion);
							}
						}
					}
					else {
						union1(roirec, &tmpUnion);
					}
					union1(tmpUnion, &detDelRoi);
				}
				else {
					//copy_image(*m_image,&detImg);
					gen_empty_region(&detDelRoi);
				}
				setDetectOutObject(DelReion, detDelRoi, i, j);
			}
			//else{
			//	copy_image(*m_image,&detImg);
			//}


			int pointN = 0;   //累计进行测试的点1数
			int usefulPoint = 0;  // 累1计有效得出结果的点数(结1果不为空)
			if (searchDir[i] == 1 || searchDir[i] == 0) {  //左右
				HTuple rowEdge, colEdge, amp, distance;
				for (int startPoint = m_param->deleteEdge[i];startPoint < roiH[i][j] - m_param->deleteEdge[i];startPoint += m_param->searchLineGap[i]) {
					double transRow, transCol;
					transRow = (double)(int)(roiTopMidRow[i][j] + startPoint*cos(roiangle[i][j]));
					transCol = (double)(int)(roiTopMidCol[i][j] + startPoint*sin(roiangle[i][j]));
					if (transRow<2 || transRow>(ImgHeight[0].I() - 2) || transCol<2 || transCol>(Imgwidth[0].I() - 2)) {
						pointN++;
						continue;
					}
					translate_measure(measureHandle, transRow, transCol);
					measure_pos(/*detImg*/*m_image, measureHandle, 1, (m_param->minGrayDiff[i] - redoCount * 10) > 0 ? (m_param->minGrayDiff[i] - redoCount * 10) : 5, edge, "all", &rowEdge, &colEdge, &amp, &distance);
					Hlong aa = rowEdge.Num();
					if (aa > 0) {
						for (int e = 0;e < aa;e++) {
							if (e > 1) {
								break;
							}
							if (m_param->CrossGrayValid[i])
							{
								if (!checkCrossGray(m_image, meaAngle, rowEdge[e].D(), colEdge[e].D(), m_param->crossGray[i], m_param->crossStep[i], edge))
									continue;
							}

							if (m_param->IsDelOffSearchRoi) {
								if (!IsPointInRegion(&ToSearchRoi, rowEdge[e].D(), colEdge[e].D())) {
									continue;
								}
							}

							if (IsPointInRegion(&detDelRoi, rowEdge[e].D(), colEdge[e].D())) {

								continue;
							}
							else {
								m_rowEdge[j][usefulPoint] = rowEdge[e];
								m_colEdge[j][usefulPoint] = colEdge[e];
								usefulPoint++;
								break;
							}
						}
					}
					pointN++;
				}
			}
			else if (searchDir[i] == 2 || searchDir[i] == 3) {  //上下
				HTuple rowEdge, colEdge, amp, distance;
				for (int startPoint = m_param->deleteEdge[i];startPoint < roiW[i][j] - m_param->deleteEdge[i];startPoint += m_param->searchLineGap[i]) {
					double transRow, transCol;
					transRow = (double)(int)(roiLfMidRow[i][j] - startPoint*sin(roiangle[i][j]));
					transCol = (double)(int)(roiLfMidCol[i][j] + startPoint*cos(roiangle[i][j]));
					if (transRow<2 || transRow>(ImgHeight[0].I() - 2) || transCol<2 || transCol>(Imgwidth[0].I() - 2)) {
						pointN++;
						continue;
					}
					translate_measure(measureHandle, transRow, transCol);
					measure_pos(/*detImg*/*m_image, measureHandle, 1.0, (m_param->minGrayDiff[i] - redoCount * 10) > 0 ? (m_param->minGrayDiff[i] - redoCount * 10) : 5, edge, "all", &rowEdge, &colEdge, &amp, &distance);
					Hlong aa = rowEdge.Num();

					double trow;
					double tcol;
					if (aa > 0) {
						for (int e = 0;e < aa;e++) {
							if (e > 1) {
								break;
							}
							if (m_param->CrossGrayValid[i])
							{
								if (!checkCrossGray(m_image, meaAngle, rowEdge[e].D(), colEdge[e].D(), m_param->crossGray[i], m_param->crossStep[i], edge))
									continue;
							}
							if (m_param->IsDelOffSearchRoi) {
								if (!IsPointInRegion(&ToSearchRoi, rowEdge[e].D(), colEdge[e].D())) {
									continue;
								}
							}

							if (IsPointInRegion(&detDelRoi, rowEdge[e].D(), colEdge[e].D())) {
								trow = rowEdge[e].D();
								tcol = colEdge[e].D();
								continue;
							}
							else {
								m_rowEdge[j][usefulPoint] = rowEdge[e];
								m_colEdge[j][usefulPoint] = colEdge[e];
								usefulPoint++;
								break;
							}
						}
					}

					pointN++;
				}
			} //采集边界点结1束
			Hlong ab = m_rowEdge[j].Num();    //以下是进行边界线的拟合


			if (j == 0 && m_param->IsDetBase == 1) {    //此组是用来检测边界组边界线的拟合
				if (ab >= 4) {
					gen_contour_polygon_xld(&edgeObj, m_rowEdge[j], m_colEdge[j]);
					double factorval = m_param->clipfactor[i]; //下面使用的筛选因子
					if (int(factorval*10.0) % 5 != 0) {
						factorval = double((int)(factorval + 0.5));
					}
					fit_line_contour_xld(edgeObj, "tukey"/*"regression"*/, -1, 0, 5, 1.5/*factorval*//*1.0*/, &lineRowBegin[i][j], &lineColBegin[i][j], &lineRowEnd[i][j],  //1.0, 1.5, 2.0, 2.5, 3.0
						&lineColEnd[i][j], &nr, &nc, &dist);
					Hlong contor = lineRowBegin[i][j].Num();
					if (contor == 0) {
						roiFound[i][j] = 1;
						allok = false;
						GrpfindOk[i] = false;
						setDetectOutData(FindOk, roiFound[i][j], i, j);
						m_testItemsStatus[0] = 0;
					}
					else if (contor > 0) {
						if (m_param->IsDetBase&&j == 0) {
							RowBegin[i][j][0] = lineRowBegin[i][j][0];
							ColBegin[i][j][0] = lineColBegin[i][j][0];
							RowEnd[i][j][0] = lineRowEnd[i][j][0];
							ColEnd[i][j][0] = lineColEnd[i][j][0];
						}
					}
				}
				else if (ab < 4) {

					roiFound[i][j] = 1;
					allok = false;
					GrpfindOk[i] = false;
					setDetectOutData(FindOk, roiFound[i][j], i, j);
					m_testItemsStatus[0] = 0;
				}
			}
			else {                                     //此组是用来检测管脚组边界线的拟合
				if (ab >= 4) {
					int nNum = m_rowEdge[j].Num();
					vector<stPoint> v(nNum);
					for (int k = 0;k < nNum;k++) {
						v[k].x = m_colEdge[j][k].D();
						v[k].y = m_rowEdge[j][k].D();
					}
					double dA, dB, dC, dMeanError;
					dA = dB = dC = 0;
					int ress = RansacLinear(v.data(), v.size(), 4, 100, 5, 2/*m_param->clipfactor[i]*/, dA, dB, dC, dMeanError);
					//dA = dB = dC = 0;
					//2：得到模型最少需要的参数个数
					//50:最大的迭代次数
					//35:一致性个数阈值
					//0.1:最大误差阈值
					//RansacLinear(v.data(),v.size(),2,50,35,0.1,dA,dB,dC,dMeanError);
					if (ress == 0) {
						double abbs = abs(dA / dB);
						if (abbs < 0.0001) {
							lineColBegin[i][j][0] = 100.0;
							lineColEnd[i][j][0] = 200.0;
							lineRowBegin[i][j][0] = -dC / dB;
							lineRowEnd[i][j][0] = -dC / dB;
						}
						else if (abbs > 10000) {
							lineRowBegin[i][j][0] = 100.0;
							lineRowEnd[i][j][0] = 200.0;
							lineColBegin[i][j][0] = -dC / dA;
							lineColEnd[i][j][0] = -dC / dA;
						}
						else {
							lineRowBegin[i][j][0] = 40.0;
							lineRowEnd[i][j][0] = 70.0;
							lineColBegin[i][j][0] = (-dB*lineRowBegin[i][j][0].D() - dC) / dA;
							lineColEnd[i][j][0] = (-dB*lineRowEnd[i][j][0].D() - dC) / dA;
						}
						HTuple tmpRow, tmpCol;
						int validCnt = 0;
						for (int z = 0;z < nNum;z++) {
							HTuple dist;
							distance_pl(m_rowEdge[j][z].D(), m_colEdge[j][z].D(), lineRowBegin[i][j][0].D(), lineColBegin[i][j][0].D()
								, lineRowEnd[i][j][0].D(), lineColEnd[i][j][0].D(), &dist);
							double diss = dist[0].D();
							if (diss <= 5) {
								validCnt++;
								tmpRow[validCnt - 1] = m_rowEdge[j][z].D();
								tmpCol[validCnt - 1] = m_colEdge[j][z].D();
							}
						}
						if (validCnt >= 3) {
							gen_contour_polygon_xld(&edgeObj, tmpRow, tmpCol);
							double factorval = m_param->clipfactor[i]; //下面使用的筛选因子
							if (int(factorval*10.0) % 5 != 0) {
								factorval = double((int)(factorval + 0.5));
							}
							fit_line_contour_xld(edgeObj, "tukey"/*"regression"*/, -1, 0, 5, 1.5/*factorval*//*1.0*/, &lineRowBegin[i][j], &lineColBegin[i][j], &lineRowEnd[i][j],  //1.0, 1.5, 2.0, 2.5, 3.0
								&lineColEnd[i][j], &nr, &nc, &dist);
							Hlong contor = lineRowBegin[i][j].Num();
							if (contor == 0) {
								if (redoCount < m_param->multGrayTimes[i] - 1) {
									redoCycle = true;
									j--;
									redoCount++;
									close_measure(measureHandle);
									continue;
								}
								else {
									redoCount = 0;

								}
								roiFound[i][j] = 1;
								allok = false;
								GrpfindOk[i] = false;
								setDetectOutData(FindOk, roiFound[i][j], i, j);
								m_testItemsStatus[0] = 0;
							}
							else {
								//if (m_param->IsDetBase[i]==1&&j==m_param->grpPinNum[i1]){
								//	RowBegin[i][j][0]=lineRowBegin[0];
								//	ColBegin[i][j][0]=lineColBegin[0];
								//	RowEnd[i][j][0]=lineRowEnd[0];
								//	ColEnd[i][j][0]=lineColEnd[0];
								//}
								setDetectOutData(lineBegRow, lineRowBegin[i][j], i, j);
								setDetectOutData(lineBegCol, lineColBegin[i][j], i, j);
								setDetectOutData(lineEndRow, lineRowEnd[i][j], i, j);
								setDetectOutData(lineEndCol, lineColEnd[i][j], i, j);
							}
						}
						else {
							if (redoCount < m_param->multGrayTimes[i] - 1) {
								redoCycle = true;
								j--;
								redoCount++;
								close_measure(measureHandle);
								continue;
							}
							else {
								redoCount = 0;

							}
							roiFound[i][j] = 1;
							allok = false;
							GrpfindOk[i] = false;
							setDetectOutData(FindOk, roiFound[i][j], i, j);
							m_testItemsStatus[0] = 0;
						}
					}
					else if (ress == 1) {
						if (redoCount < m_param->multGrayTimes[i] - 1) {
							redoCycle = true;
							j--;
							redoCount++;
							close_measure(measureHandle);
							continue;
						}
						else {
							redoCount = 0;

						}

						roiFound[i][j] = 1;
						allok = false;
						GrpfindOk[i] = false;
						setDetectOutData(FindOk, roiFound[i][j], i, j);
						m_testItemsStatus[0] = 0;
					}
				}
				else if (ab < 4) {
					if (redoCount < m_param->multGrayTimes[i] - 1) {
						redoCycle = true;
						j--;
						redoCount++;
						close_measure(measureHandle);
						continue;
					}
					else {
						redoCount = 0;

					}
					roiFound[i][j] = 1;
					allok = false;
					GrpfindOk[i] = false;
					setDetectOutData(FindOk, roiFound[i][j], i, j);
					m_testItemsStatus[0] = 0;
				}
			}    //end of 检测管脚组边界线的拟合
			close_measure(measureHandle);
			if (roiFound[i][j] || j == 0) {
				setDetectOutData(FindOk, roiFound[i][j], i, j);
				continue;
			}
			double lineAngle = 0;
			char edge1[32], edge2[32];
			if (m_param->whiteBlackDir[i]) { //白色positive
				sprintf(edge1, "negative");
				sprintf(edge2, "positive");
			}
			else {
				sprintf(edge1, "positive");
				sprintf(edge2, "negative");
			}
			double colend, colbeg, rowend, rowbeg;
			colend = lineColEnd[i][j][0].D();
			colbeg = lineColBegin[i][j][0].D();
			rowend = lineRowEnd[i][j][0].D();
			rowbeg = lineRowBegin[i][j][0].D();
			double transRow, transCol;
			if (searchDir[i] == 0 || searchDir[i] == 1) {
				transRow = (lineRowBegin[i][j][0].D() + lineRowEnd[i][j][0].D()) / 2.0;
				transCol = (lineColBegin[i][j][0].D() + lineColEnd[i][j][0].D()) / 2.0;
				if (transRow<2 || transRow>(ImgHeight[0].I() - 2) || transCol<2 || transCol>(Imgwidth[0].I() - 2)) {
					roiFound[i][j] = 3;
					GrpfindOk[i] = false;
					allok = false;
					setDetectOutData(FindOk, roiFound[i][j], i, j);
					m_testItemsStatus[0] = 0;
					continue;
				}
				lineAngle = PI / 2 + atan((lineColEnd[i][j][0].D() - lineColBegin[i][j][0].D()) / (lineRowEnd[i][j][0].D() - lineRowBegin[i][j][0].D()));
				gen_measure_rectangle2((lineRowBegin[i][j][0].D() + lineRowEnd[i][j][0].D()) / 2.0, (lineColBegin[i][j][0].D() + lineColEnd[i][j][0].D()) / 2.0, lineAngle,
					roiH[i][j] / 2 + m_param->widSearchLength[i], 2/*m_param->extendSearchLine[i]*2*/, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measureHandle);
			}
			else if (searchDir[i] == 2 || searchDir[i] == 3) {
				transRow = (lineRowBegin[i][j][0].D() + lineRowEnd[i][j][0].D()) / 2.0;
				transCol = (lineColBegin[i][j][0].D() + lineColEnd[i][j][0].D()) / 2.0;
				if (transRow<2 || transRow>(ImgHeight[0].I() - 2) || transCol<2 || transCol>(Imgwidth[0].I() - 2)) {
					roiFound[i][j] = 3;
					GrpfindOk[i] = false;
					allok = false;
					setDetectOutData(FindOk, roiFound[i][j], i, j);
					m_testItemsStatus[0] = 0;
					continue;
				}
				lineAngle = -atan((lineRowEnd[i][j][0].D() - lineRowBegin[i][j][0].D()) / (lineColEnd[i][j][0].D() - lineColBegin[i][j][0].D()));
				gen_measure_rectangle2((lineRowBegin[i][j][0].D() + lineRowEnd[i][j][0].D()) / 2.0, (lineColBegin[i][j][0].D() + lineColEnd[i][j][0].D()) / 2.0, lineAngle,
					roiW[i][j] / 2 + m_param->widSearchLength[i], 2/*m_param->extendSearchLine[i]*2*/, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &measureHandle);
			}
			HTuple rowEdgeFirst, colEdgeFirst, rowEdgeSecond, colEdgeSecond, Ampfir, AmpSec, Dis;
			int nowWidLineOffset;
			bool widthgetok = false;  //是否正常获取
			bool onceMeaOK = false; //至少一次正常获取边界
			int multTimes = 1;
			int multStep = 2;
			if (m_param->widOffsetRange[i] <= 20) {
				multStep = 2;
			}
			else if (m_param->widOffsetRange[i] <= 50) {
				multStep = 4;
			}
			else {
				multStep = 8;
			}
			multTimes += m_param->widOffsetRange[i] / multStep;
			bool foundFirstWidCenter = false;  //是否查找到宽度中心的以查找肩部中心

			double interRow, interCol; //搜索区域中心点与参考边界交点平移肩部偏移距离后的点，用于求取肩部
			double edgex, edgey, edgelen; //边界向量
			double leadx, leady; //管脚垂直边界的向量
			bool shoulderFound = true;
			double edgeAngle;
			if (m_param->errPinSweepValid[i]) {  //求肩部点
				if (m_param->IsDetBase == 2 || (m_param->IsDetBase == 1 && !roiFound[i][0])) {
					edgey = RowEnd[i][0][0].D() - RowBegin[i][0][0].D();
					edgex = ColEnd[i][0][0].D() - ColBegin[i][0][0].D();
					edgelen = sqrt(edgex*edgex + edgey*edgey);
					edgex = edgex / edgelen;
					edgey = edgey / edgelen;
					leadx = edgex*cos(PI / 2.0) + edgey*sin(PI / 2.0);
					leady = -edgex*sin(PI / 2.0) + edgey*cos(PI / 2.0);
					double tmpRoiRow, tmpRoiCol;
					tmpRoiRow = roirow[i][j] + 10.0*leady;
					tmpRoiCol = roicol[i][j] + 10.0*leadx;
					HTuple interc, interr, ispa;
					intersection_ll(tmpRoiRow, tmpRoiCol, roirow[i][j], roicol[i][j], RowBegin[i][0][0].D(), ColBegin[i][0][0].D(),
						RowEnd[i][0][0].D(), ColEnd[i][0][0].D(), &interr, &interc, &ispa);
					leadx = abs(leadx)*(roicol[i][j] - interc[0].D()) / abs(roicol[i][j] - interc[0].D());
					leady = abs(leady)*(roirow[i][j] - interr[0].D()) / abs(roirow[i][j] - interr[0].D());
					interRow = interr[0].D() + m_param->shoulderShift[i] * leady;
					interCol = interc[0].D() + m_param->shoulderShift[i] * leadx;

					edgeAngle = atan2(-edgey, edgex);
					Hlong shoulderHandle;
					HTuple shoulderRowFirst, shoulderColFirst, shoulderRowEnd, shoulderColEnd;
					gen_measure_rectangle2(interRow, interCol, edgeAngle,
						roiW[i][j] / 2 + m_param->widSearchLength[i], 2, Imgwidth[0], ImgHeight[0], "nearest_neighbor", &shoulderHandle);
					HTuple af, ae, dis;
					measure_pos(*m_image, shoulderHandle, 1, m_param->WidthGrayDiff[i], edge1, "all", &shoulderRowFirst, &shoulderColFirst, &af, &dis);
					measure_pos(*m_image, shoulderHandle, 1, m_param->WidthGrayDiff[i], edge2, "all", &shoulderRowEnd, &shoulderColEnd, &ae, &dis);

					close_measure(shoulderHandle);
					int fir = shoulderRowFirst.Num();
					int sec = shoulderRowEnd.Num();
					int bestFirInx, bestSecInx;
					double sad;
					double sads;
					HTuple sortInx;
					if (fir < 1 || sec < 1) {
						shoulderFound = false;
					}
					else {
						if (m_param->IsAmpBest[i]) {
							for (int a = 0;a < af.Num();a++) {
								af[a] = abs(af[a].D());
								sad = af[a].D();
							}
							for (int a = 0;a < ae.Num();a++) {
								ae[a] = abs(ae[a].D());
								sads = ae[a].D();
							}
							tuple_sort_index(af, &sortInx);
							bestFirInx = sortInx[af.Num() - 1].I();
							tuple_sort_index(ae, &sortInx);
							bestSecInx = sortInx[ae.Num() - 1].I();
						}
						else {
							bestFirInx = 0;
							bestSecInx = sec - 1;
						}
					}
					if (shoulderFound && 0/*m_param->CrossGrayValid[i]*/)
					{
						if (!checkCrossGray(m_image, edgeAngle, shoulderRowFirst[bestFirInx].D(), shoulderColFirst[bestFirInx].D(), m_param->crossGray[i], m_param->crossStep[i], edge1))
							shoulderFound = false;
						if (!checkCrossGray(m_image, edgeAngle, shoulderRowEnd[bestSecInx].D(), shoulderColEnd[bestSecInx].D(), m_param->crossGray[i], m_param->crossStep[i], edge2))
							shoulderFound = false;
					}
					if (shoulderFound/*fir+sec>1*/) {
						double rowb = shoulderRowFirst[bestFirInx].D();
						double colb = shoulderColFirst[bestFirInx].D();
						double re = shoulderRowEnd[bestSecInx].D();
						double ce = shoulderColEnd[bestSecInx].D();
						SHRowBegin[i][j][0] = rowb;
						SHColBegin[i][j][0] = colb;
						SHRowEnd[i][j][0] = re;
						SHColEnd[i][j][0] = ce;
						SHRowCenter[i][j][0] = (rowb + re) / 2.0;
						SHColCenter[i][j][0] = (colb + ce) / 2.0;
						setDetectOutData(ShRowBegin, SHRowBegin[i][j], i, j);
						setDetectOutData(ShRowEnd, SHRowEnd[i][j], i, j);
						setDetectOutData(ShColBegin, SHColBegin[i][j], i, j);
						setDetectOutData(ShColEnd, SHColEnd[i][j], i, j);
						setDetectOutData(ShRowCenter, SHRowCenter[i][j], i, j);
						setDetectOutData(ShColCenter, SHColCenter[i][j], i, j);
					}
				}
			}

			double maxWRow[2], maxWCol[2], minWRow[2], minWCol[2];  //求到的最大最小宽度的坐标
			double maxSRow[2], maxSCol[2], minSRow[2], minSCol[2];  //求到的最大最小Sweep的坐标
			double maxTmpWid = -1, minTmpWid = 100000;
			double maxTmpSweep = -1, minTmpSweep = 100000;
			if ((shoulderFound&&m_param->errPinSweepValid[i]) || !m_param->errPinSweepValid[i]) {  //求宽度
				for (int e = 0;e < multTimes;e++) {
					int offpolar[4] = { 1,-1,1,-1 };
					if (searchDir[i] == 0 || searchDir[i] == 1) {
						translate_measure(measureHandle, (lineRowBegin[i][j][0].D() + lineRowEnd[i][j][0].D()) / 2.0,
							(lineColBegin[i][j][0].D() + lineColEnd[i][j][0].D()) / 2.0 + double(offpolar[searchDir[i]])*double(2.0 + m_param->widLineOffset[i] + e*multStep));
					}
					else if (searchDir[i] == 2 || searchDir[i] == 3) {
						translate_measure(measureHandle, (lineRowBegin[i][j][0].D() + lineRowEnd[i][j][0].D()) / 2.0 + double(offpolar[searchDir[i]])*double(2 + m_param->widLineOffset[i] + e*multStep),
							(lineColBegin[i][j][0].D() + lineColEnd[i][j][0].D()) / 2.0);
					}
					measure_pos(/*detImg*/*m_image, measureHandle, 1, m_param->WidthGrayDiff[i], edge1, "all", &rowEdgeFirst, &colEdgeFirst, &Ampfir, &Dis);
					measure_pos(/*detImg*/*m_image, measureHandle, 1, m_param->WidthGrayDiff[i], edge2, "all", &rowEdgeSecond, &colEdgeSecond, &AmpSec, &Dis);

					int fir = rowEdgeFirst.Num();
					int sec = rowEdgeSecond.Num();
					int bestFirInx, bestSecInx;
					double sad;
					double sads;
					HTuple sortInx;
					bool meaOk = true;
					if (fir < 1 || sec < 1) {
						meaOk = false;
					}
					else {
						HTuple newFirRow, newFirCol, newSecRow, newSecCol, newAmpFir, newAmpSec;
						int newFirNum = 0, newSecNum = 0;
						for (int b = 0;b < fir;b++) {
							bool needDel = false;
							if (m_param->IsDelOffSearchRoi) {
								if (!IsPointInRegion(&ToSearchRoi, rowEdgeFirst[b].D(), colEdgeFirst[b].D())) {
									needDel = true;
								}
							}
							if (IsPointInRegion(&detDelRoi, rowEdgeFirst[b].D(), colEdgeFirst[b].D(), 3)) {
								needDel = true;
							}
							if (!needDel) {
								newFirRow[newFirNum] = rowEdgeFirst[b];
								newFirCol[newFirNum] = colEdgeFirst[b];
								newAmpFir[newFirNum] = Ampfir[b];
								newFirNum++;
							}
						}
						for (int b = 0;b < sec;b++) {
							bool needDel = false;
							if (m_param->IsDelOffSearchRoi) {
								if (!IsPointInRegion(&ToSearchRoi, rowEdgeSecond[b].D(), colEdgeSecond[b].D())) {
									needDel = true;
								}
							}
							if (IsPointInRegion(&detDelRoi, rowEdgeSecond[b].D(), colEdgeSecond[b].D(), 3)) {
								needDel = true;
							}
							if (!needDel) {
								newSecRow[newSecNum] = rowEdgeSecond[b];
								newSecCol[newSecNum] = colEdgeSecond[b];
								newAmpSec[newSecNum] = AmpSec[b];
								newSecNum++;
							}
						}
						if (newFirNum < 1 || newSecNum < 1) {
							meaOk = false;
						}
						else {
							if (m_param->IsAmpBest[i]) {
								for (int a = 0;a < newAmpFir.Num();a++) {
									newAmpFir[a] = abs(newAmpFir[a].D());
									sad = newAmpFir[a].D();
								}
								for (int a = 0;a < newAmpSec.Num();a++) {
									newAmpSec[a] = abs(newAmpSec[a].D());
									sads = newAmpSec[a].D();
								}
								tuple_sort_index(newAmpFir, &sortInx);
								bestFirInx = sortInx[newAmpFir.Num() - 1].I();
								tuple_sort_index(newAmpSec, &sortInx);
								bestSecInx = sortInx[newAmpSec.Num() - 1].I();
							}
							else {
								bestFirInx = 0;
								bestSecInx = newSecNum - 1;
							}
							rowEdgeSecond.Reset();
							rowEdgeFirst.Reset();
							colEdgeFirst.Reset();
							colEdgeSecond.Reset();
							rowEdgeFirst = newFirRow;
							rowEdgeSecond = newSecRow;
							colEdgeFirst = newFirCol;
							colEdgeSecond = newSecCol;
						}
					}

					if (meaOk && m_param->CrossGrayValid[i])
					{
						if (!checkCrossGray(m_image, edgeAngle, rowEdgeFirst[bestFirInx].D(), colEdgeFirst[bestFirInx].D(), m_param->crossGray[i], m_param->crossStep[i], edge1))
							shoulderFound = false;
						if (!checkCrossGray(m_image, edgeAngle, rowEdgeSecond[bestSecInx].D(), colEdgeSecond[bestSecInx].D(), m_param->crossGray[i], m_param->crossStep[i], edge2))
							shoulderFound = false;
					}

					if (meaOk/*fir+sec>1*/) {
						double rowb = rowEdgeFirst[bestFirInx].D();
						double colb = colEdgeFirst[bestFirInx].D();
						double re = rowEdgeSecond[bestSecInx].D();
						double ce = colEdgeSecond[bestSecInx].D();
						RowBegin[i][j][0] = rowEdgeFirst[bestFirInx];
						ColBegin[i][j][0] = colEdgeFirst[bestFirInx];
						RowEnd[i][j][0] = rowEdgeSecond[bestSecInx];
						ColEnd[i][j][0] = colEdgeSecond[bestSecInx];
						if (!onceMeaOK || m_param->widthMethod == 0) {
							if (searchDir[i] == 0 || searchDir[i] == 1) {
								RowTop[i][j][0] = (rowEdgeFirst[bestFirInx].D() + rowEdgeSecond[bestSecInx].D()) / 2.0;
								ColTop[i][j][0] = (colEdgeFirst[bestFirInx].D() + colEdgeSecond[bestSecInx].D()) / 2.0
									- double(offpolar[searchDir[i]])*double(2.0 + m_param->widLineOffset[i] + e*multStep);
							}
							else if (searchDir[i] == 2 || searchDir[i] == 3) {
								RowTop[i][j][0] = (rowEdgeFirst[bestFirInx].D() + rowEdgeSecond[bestSecInx].D()) / 2.0
									- double(offpolar[searchDir[i]])*double(2.0 + m_param->widLineOffset[i] + e*multStep);
								ColTop[i][j][0] = (colEdgeFirst[bestFirInx].D() + colEdgeSecond[bestSecInx].D()) / 2.0;
							}
						}
						onceMeaOK = true;
						HTuple tmpleadWid;
						distance_pp(rowEdgeFirst[bestFirInx].D(), colEdgeFirst[bestFirInx].D(), rowEdgeSecond[bestSecInx].D(), colEdgeSecond[bestSecInx].D(), &tmpleadWid);
						double tmplwid = tmpleadWid[0].D()*mk[i] + m_param->widthCorrNew[i][j];

						HTuple tmpleadSweep;
						if ((m_param->IsDetBase == 2 || (m_param->IsDetBase == 1 && !roiFound[i][0])) && m_param->errPinSweepValid[i]) {   //应通过求出a(底线),b(前后管脚顶点形成的线)直线的夹角,再求出b*夹角的余弦值
							double anglell;
							angle_ll(roirow[i][j], roicol[i][j], interRow, interCol,
								SHRowCenter[i][j][0].D(), SHColCenter[i][j][0].D(),
								(rowEdgeFirst[bestFirInx].D() + rowEdgeSecond[bestSecInx].D()) / 2.0,
								(colEdgeFirst[bestFirInx].D() + colEdgeSecond[bestSecInx].D()) / 2.0, &anglell);
							distance_pp(SHRowCenter[i][j][0].D(), SHColCenter[i][j][0].D(),
								(rowEdgeFirst[bestFirInx].D() + rowEdgeSecond[bestSecInx].D()) / 2.0,
								(colEdgeFirst[bestFirInx].D() + colEdgeSecond[bestSecInx].D()) / 2.0, &tmpleadSweep);
							tmpleadSweep[0] = tmpleadSweep[0].D()*abs(sin(anglell));

							if (tmpleadSweep[0].D() < minTmpSweep) {
								minTmpSweep = tmpleadSweep[0].D();
								pinSweepMin[i][j][0] = tmpleadSweep[0].D();
								minSRow[0] = rowEdgeFirst[bestFirInx].D();
								minSRow[1] = rowEdgeSecond[bestSecInx].D();
								minSCol[0] = colEdgeFirst[bestFirInx].D();
								minSCol[1] = colEdgeSecond[bestSecInx].D();
							}
							if (tmpleadSweep[0].D() > maxTmpSweep) {
								maxTmpSweep = tmpleadSweep[0].D();
								pinSweepMax[i][j][0] = tmpleadSweep[0].D();
								maxSRow[0] = rowEdgeFirst[bestFirInx].D();
								maxSRow[1] = rowEdgeSecond[bestSecInx].D();
								maxSCol[0] = colEdgeFirst[bestFirInx].D();
								maxSCol[1] = colEdgeSecond[bestSecInx].D();
							}
						}
						if (tmplwid < minTmpWid) {
							minTmpWid = tmplwid;
							pinWid[i][j][0] = tmpleadWid[0].D();
							minWRow[0] = rowEdgeFirst[bestFirInx].D();
							minWRow[1] = rowEdgeSecond[bestSecInx].D();
							minWCol[0] = colEdgeFirst[bestFirInx].D();
							minWCol[1] = colEdgeSecond[bestSecInx].D();
						}
						if (tmplwid > maxTmpWid) {
							maxTmpWid = tmplwid;
							pinWidMax[i][j][0] = tmpleadWid[0].D();
							maxWRow[0] = rowEdgeFirst[bestFirInx].D();
							maxWRow[1] = rowEdgeSecond[bestSecInx].D();
							maxWCol[0] = colEdgeFirst[bestFirInx].D();
							maxWCol[1] = colEdgeSecond[bestSecInx].D();
						}
						if (m_param->errPinWidthValid[i] && m_param->widthMethod == 0) {
							minTmpWid = tmplwid;
							pinWid[i][j][0] = tmpleadWid[0].D();
							minWRow[0] = rowEdgeFirst[bestFirInx].D();
							minWRow[1] = rowEdgeSecond[bestSecInx].D();
							minWCol[0] = colEdgeFirst[bestFirInx].D();
							minWCol[1] = colEdgeSecond[bestSecInx].D();  //直到搜索到合格值，并将此坐标输出
							if (tmplwid > m_param->errPinWidthMax[i][j] || tmplwid < m_param->errPinWidthMin[i][j]) {
								continue;
							}
							else {
								widthgetok = true;
								break;
							}
						}
					}//end of meaok
				}  //end of loop e
			}  //end of if
			if (onceMeaOK) {
				RowBegin[i][j][0] = minWRow[0];
				RowEnd[i][j][0] = minWRow[1];
				ColBegin[i][j][0] = minWCol[0];
				ColEnd[i][j][0] = minWCol[1];

				RowWidMaxBegin[i][j][0] = maxWRow[0];
				RowWidMaxEnd[i][j][0] = maxWRow[1];
				ColWidMaxBegin[i][j][0] = maxWCol[0];
				ColWidMaxEnd[i][j][0] = maxWCol[1];
				if ((m_param->IsDetBase == 2 || (m_param->IsDetBase == 1 && !roiFound[i][0])) && m_param->errPinSweepValid[i]) {
					RowSweepMinBegin[i][j][0] = minSRow[0];
					RowSweepMinEnd[i][j][0] = minSRow[1];
					ColSweepMinBegin[i][j][0] = minSCol[0];
					ColSweepMinEnd[i][j][0] = minSCol[1];
					RowSweepMaxBegin[i][j][0] = maxSRow[0];
					RowSweepMaxEnd[i][j][0] = maxSRow[1];
					ColSweepMaxBegin[i][j][0] = maxSCol[0];
					ColSweepMaxEnd[i][j][0] = maxSCol[1];
				}
			}
			if (!onceMeaOK) {
				if (shoulderFound) {
					roiFound[i][j] = 3;
				}
				else {
					roiFound[i][j] = 2;
				}
				GrpfindOk[i] = false;
				allok = false;
				m_testItemsStatus[0] = 0;
			}
			setDetectOutData(FindOk, roiFound[i][j], i, j);   //此处测试不通过则所有的情况将混合无法区分，应该以case区分
			close_measure(measureHandle);
		}   //end of loop j ,对各组的边界线以及交点等进行检测

		Hobject tmpobj;
		if (GrpfindOk[i]) {
			for (int j = 0;j < m_param->grpPinNum[i] + 1;j++) {
				if (j != 0) {
					gen_region_points(&tmpobj, m_rowEdge[j], m_colEdge[j]);
					union2(pointsobj[i], tmpobj, &pointsobj[i]);
					setDetectOutData(PinTopRow, RowTop[i][j], i, j);
					setDetectOutData(PinTopCol, ColTop[i][j], i, j);				
					outLeadRow.Append(RowTop[i][j][0].D());
					outLeadCol.Append(ColTop[i][j][0].D());
					setDetectOutData(MaxWidthBeginRow, RowWidMaxBegin[i][j], i, j);
					setDetectOutData(MaxWidthBeginCol, ColWidMaxBegin[i][j], i, j);
					setDetectOutData(MaxWidthEndRow, RowWidMaxEnd[i][j], i, j);
					setDetectOutData(MaxWidthEndCol, ColWidMaxEnd[i][j], i, j);
					if (m_param->IsDetBase > 0 && m_param->errPinSweepValid[i]) {
						setDetectOutData(MinSweepBeginRow, RowSweepMinBegin[i][j], i, j);
						setDetectOutData(MinSweepBeginCol, ColSweepMinBegin[i][j], i, j);
						setDetectOutData(MinSweepEndRow, RowSweepMinEnd[i][j], i, j);
						setDetectOutData(MinSweepEndCol, ColSweepMinEnd[i][j], i, j);
						setDetectOutData(MaxSweepBeginRow, RowSweepMaxBegin[i][j], i, j);
						setDetectOutData(MaxSweepBeginCol, ColSweepMaxBegin[i][j], i, j);
						setDetectOutData(MaxSweepEndRow, RowSweepMaxEnd[i][j], i, j);
						setDetectOutData(MaxSweepEndCol, ColSweepMaxEnd[i][j], i, j);
					}
				}
				else if (j == 0 && !m_param->IsDetBase) {
					continue;
				}
				setDetectOutData(BeginRow, RowBegin[i][j], i, j);
				setDetectOutData(BeginCol, ColBegin[i][j], i, j);
				setDetectOutData(EndRow, RowEnd[i][j], i, j);
				setDetectOutData(EndCol, ColEnd[i][j], i, j);
			}
			double maxPinStand = 0;
			double minPinStand = 999999.0;
			for (int j = 0;j < m_param->grpPinNum[i];j++) {
				if (m_param->errPinWidthValid[i]) {
					bool widthok = true;
					double pinwidthmin, pinwidthmax;
					pinwidthmin = pinWid[i][j + 1][0].D()*mk[i] + m_param->widthCorrNew[i][j];
					pinwidthmax = pinWidMax[i][j + 1][0].D()*mk[i] + m_param->widthCorrNew[i][j];
					if (m_param->widthMethod == 0 || m_param->widthMethod == 1) {
						if (pinwidthmin > m_param->errPinWidthMax[i][j] || pinwidthmin < m_param->errPinWidthMin[i][j]) {
							widthok = false;
						}
					}
					else if (m_param->widthMethod == 2) {
						if (pinwidthmax > m_param->errPinWidthMax[i][j] || pinwidthmax < m_param->errPinWidthMin[i][j]) {
							widthok = false;
						}
					}
					else {
						if (pinwidthmin > m_param->errPinWidthMax[i][j] || pinwidthmin < m_param->errPinWidthMin[i][j]) {
							widthok = false;
						}
						if (pinwidthmax > m_param->errPinWidthMax[i][j] || pinwidthmax < m_param->errPinWidthMin[i][j]) {
							widthok = false;
						}
					}
					if (!widthok) {
						allok = false;
						regionsingleok[i][j] = false;
						regionok[i] = false;
						m_testItemsStatus[3] = 0;
					}
					setDetectOutData(PinWidth, pinwidthmin, i, j);
					setDetectOutData(PinMaxWidth, pinwidthmax, i, j);
					setDetectOutData(PinWidthOK, widthok, i, j);
				}
				if (m_param->errPinSweepValid[i] && m_param->IsDetBase) {
					bool sweepok = true;
					double pinsweepmin, pinsweepmax;
					pinsweepmin = pinSweepMin[i][j + 1][0].D()*mk[i];
					pinsweepmax = pinSweepMax[i][j + 1][0].D()*mk[i];

					if (pinsweepmin > m_param->errPinSweepMax[i][j] || pinsweepmin < m_param->errPinSweepMin[i][j]) {
						sweepok = false;
					}
					if (pinsweepmax > m_param->errPinSweepMax[i][j] || pinsweepmax < m_param->errPinSweepMin[i][j]) {
						sweepok = false;
					}

					if (!sweepok) {
						allok = false;
						regionsingleok[i][j] = false;
						regionok[i] = false;
						m_testItemsStatus[5] = 0;
					}
					setDetectOutData(PinMinSweep, pinsweepmin, i, j);
					setDetectOutData(PinMaxSweep, pinsweepmax, i, j);
					setDetectOutData(PinSweepOK, sweepok, i, j);
				}
				if (m_param->errPinLengthValid[i] && m_param->IsDetBase) {
					bool standok;
					distance_pl(RowTop[i][j + 1], ColTop[i][j + 1], RowBegin[i][0], ColBegin[i][0],
						RowEnd[i][0], ColEnd[i][0], &pinStandOff[i][j]);

					pinStandOff[i][j][0] = pinStandOff[i][j][0].D()*mk[i];
					pinStandOff[i][j][0] = pinStandOff[i][j][0].D() + m_param->lengthCorrNew[i][j];
					if (pinStandOff[i][j][0].D() > maxPinStand) {
						maxPinStand = pinStandOff[i][j][0].D();
					}
					if (pinStandOff[i][j][0].D() < minPinStand) {
						minPinStand = pinStandOff[i][j][0].D();
					}
					if (pinStandOff[i][j][0].D() > m_param->errPinLengthMax[i][j] || pinStandOff[i][j][0].D() < m_param->errPinLengthMin[i][j]) {
						standok = false;
						allok = false;
						regionsingleok[i][j] = false;
						regionok[i] = false;
						m_testItemsStatus[1] = 0;
					}
					else {
						standok = true;
					}
					int countval = 0;
					for (int g = 0;g < i;g++) {
						countval += m_param->grpPinNum[g];
					}
					outLength[countval + j] = pinStandOff[i][j][0];
					setDetectOutData(StandOff, pinStandOff[i][j], i, j);
					setDetectOutData(StandOffOK, standok, i, j);
				}
				if (m_param->errPinGapValid[i] && j < m_param->grpPinNum[i] - 1) {
					bool gapok;
					if (m_param->IsDetBase == 0) {
						distance_pp(RowTop[i][j + 2], ColTop[i][j + 2], RowTop[i][j + 1], ColTop[i][j + 1]
							, &pinInterVal[i][j]);
					}
					else if (m_param->IsDetBase > 0) {   //应通过求出a(底线),b(前后管脚顶点形成的线)直线的夹角,再求出b*夹角的余弦值
						double anglell;
						angle_ll(RowTop[i][j + 2][0].D(), ColTop[i][j + 2][0].D(), RowTop[i][j + 1][0].D(), ColTop[i][j + 1][0].D(),
							RowBegin[i][0][0].D(), ColBegin[i][0][0].D(),
							RowEnd[i][0][0].D(), ColEnd[i][0][0].D(), &anglell);
						distance_pp(RowTop[i][j + 2], ColTop[i][j + 2], RowTop[i][j + 1], ColTop[i][j + 1]
							, &pinInterVal[i][j]);
						pinInterVal[i][j][0] = pinInterVal[i][j][0].D()*abs(cos(anglell));
					}
					double pinter = pinInterVal[i][j][0].D();
					pinInterVal[i][j][0] = pinInterVal[i][j][0].D()*mk[i] + m_param->InterCorrNew[i][j];
					//pinInterVal[i][j][0]=pinInterVal[i][j][0].D()*0.7;
					if (pinInterVal[i][j][0].D() > m_param->errPinGapMax[i][j] || pinInterVal[i][j][0].D() < m_param->errPinGapMin[i][j]) {
						gapok = false;
						allok = false;
						regionsingleok[i][j] = false;
						regionok[i] = false;
						m_testItemsStatus[2] = 0;
					}
					else {
						gapok = true;
					}
					setDetectOutData(PinInterVal, pinInterVal[i][j], i, j);
					setDetectOutData(PinInterValOK, gapok, i, j);
				}
				setDetectOutData(RegionSingleOk, regionsingleok[i][j], i, j);
			}  //end of grppinmun j
			if (m_param->errPinStandOffDiffValid[i]) {
				bool DiffOk;
				pinStandDiff[i][0] = maxPinStand - minPinStand;
				//pinStandDiff[i][0]=pinStandDiff[i][0].D()*0.7;
				if (pinStandDiff[i][0].D() > m_param->errPinStandOffDiffMax[i]) {
					DiffOk = false;
					allok = false;
					regionok[i] = false;
					m_testItemsStatus[4] = 0;
				}
				else {
					DiffOk = true;
				}
				setDetectOutData(StandOffDiff, pinStandDiff[i], i);
				setDetectOutData(StandOffDiffOK, DiffOk, i);
			}
			setDetectOutData(RegionOk, regionok[i], i);
			setDetectOutData(GrpFindOk, true, i);
		}
		else {
			setDetectOutData(GrpFindOk, false, i);
		}
	}  //end of loop i
	Hobject pointsAllImg;
	bool pointsImgOk = false;
	for (int i = 0;i < GRP_NUM;i++) {
		if (GrpfindOk[i] && m_param->grpPinNum[i] > 0) {
			pointsImgOk = true;
			union2(pointsAllImg, pointsobj[i], &pointsAllImg);
		}
	}
	setDetectOutData(DispPointsImgOk, pointsImgOk);
	if (pointsImgOk) {
		setDetectOutObject(DispPointsImg, pointsAllImg);
	}
	setDetectOutData(AllOK, allok);  //allok=resultOK
	if (allok) {
		HTuple pincount;
		int pinp = 0;
		bool enable = true;
		for (int i = 0;i < GRP_NUM;i++) {
			if (m_param->grpPinNum[i]) {
				pincount[pinp] = m_param->grpPinNum[i];
				pinp++;
				if (!m_param->errPinLengthValid[i]) {
					enable = false;
				}
			}
		}
		if (m_param->IsDetBase == 0) {
			enable = false;
		}
//		setDetectOutData(LengthEnable, enable);
//		setDetectOutData(OutLength, outLength);
// 		setDetectOutData(OutLeadRow, outLeadRow);
// 		setDetectOutData(OutLeadCol, outLeadCol);
		*p_outLenEn = enable;
		for (auto i = 0;i < outLength.Num();++i)
			p_outLen->push_back(outLength[i].D());
		for (auto i = 0;i < outLeadCol.Num();++i)
		{
			double x = outLeadCol[i].D();
			double y = outLeadRow[i].D();
			p_outLead->push_back(cv::Point2d(x, y));
		}
	}
	return allok;
}

void pinDetect1Mod::viewResult(ImageView *iv, const QString &funName, int)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	HTuple imgok;
	getDispOutData(DispPointsImgOk, imgok);
	//iv->setFilled(false);
	iv->setColor(255, 0, 0);
	if (imgok[0].I() == 1) {
		iv->setColor(0, 255, 0);
		Hlong num;
		Hobject obj = *getDispObject(DispPointsImg);
		count_obj(obj, &num);
		if (num > 0) {
			if (num > 1)
				union1(obj, &obj);
			RlRegion dispReg;
			HalOpenCV::hal2cvRegion(obj, &dispReg);
			iv->dispRegion(&dispReg);
		}
		else {
			return;
		}
	}
	HTuple grpfindok;
	if (m_param->IsDelOffSearchRoi) {

		HTuple Imgwidth, ImgHeight;
		get_image_size(*m_image, &Imgwidth, &ImgHeight);
		//Hobject imgRec;
		//gen_rectangle1(&imgRec,0,0,ImgHeight[0].I()-1,Imgwidth[0].I()-1);
		Hobject rec2reg;
		if (m_param->IsUseInputRegion) {
			HTuple roireferrow, roirefercol; //外部传入搜索区域
			getDispOutData(TmpSearchReferRow, roireferrow);
			getDispOutData(TmpSearchReferCol, roirefercol);
			Hobject inputroi;
			gen_region_polygon_filled(&rec2reg, roireferrow, roirefercol);
		}
		else {
			HTuple tmpsearhom;
			getDispOutData(TmpSearchHomMat2D, tmpsearhom);

			gen_rectangle2(&rec2reg, m_param->SearchEdgeRoi.row, m_param->SearchEdgeRoi.col, m_param->SearchEdgeRoi.angle,
				m_param->SearchEdgeRoi.len1, m_param->SearchEdgeRoi.len2);
			affine_trans_region(rec2reg, &rec2reg, tmpsearhom, "true");
		}

		//difference(imgRec,rec2reg,&rec2reg);
		iv->setFilled(false);
		//iv->setColor(m_param->backGray[0],m_param->backGray[0],m_param->backGray[0]);
		RlRegion dispReg;
		HalOpenCV::hal2cvRegion(rec2reg, &dispReg);
		iv->dispRegion(&dispReg);
		//iv->setFilled(false);
	}
	for (int i = 0;i < GRP_NUM;i++) {
		if (m_param->grpPinNum[i] == 0)
			continue;
		getDispOutData(GrpFindOk, i, grpfindok);
		if (grpfindok[0].I() == 0) {
			HTuple mat;
			if (m_param->IsRefer[i]) {
				getDispOutData(m_tmpHomDefine[i], mat);
			}
			for (int j = 0;j < m_param->grpPinNum[i] + 1;j++) {
				if (!m_param->IsDetBase&&j == 0) {
					continue;
				}
				HTuple found;
				getDispOutData(FindOk, i, j, found);
				if (found[0].I() != 0) {
					Hobject rec;
					gen_rectangle1(&rec, m_param->RoiVec[i][j].row1, m_param->RoiVec[i][j].col1, m_param->RoiVec[i][j].row2,
						m_param->RoiVec[i][j].col2);
					iv->setColor(255, 0, 0);
					iv->setFilled(false);
					if (m_param->IsRefer[i]) {
						affine_trans_region(rec, &rec, mat, "false");
					}
					RlRegion rlreg;
					HalOpenCV::hal2cvRegion(rec, &rlreg);
					iv->dispRegion(&rlreg);
				}
			}
		}
		else {
			HTuple beginrow, begincol, endrow, endcol, toprow, topcol;
			for (int j = 0;j < m_param->grpPinNum[i] + 1;j++) {
				if (!m_param->IsDetBase&&j == 0) {
					continue;
				}
				getDispOutData(BeginRow, i, j, beginrow);
				getDispOutData(BeginCol, i, j, begincol);
				getDispOutData(EndRow, i, j, endrow);
				getDispOutData(EndCol, i, j, endcol);
				if (j == 0) {
					iv->setColor(255, 255, 0);
					iv->dispLine(beginrow[0].D(), begincol[0].D(), endrow[0].D(), endcol[0].D());
				}
				else {
					if (m_param->IsShowBlack) {
						iv->setColor(30, 30, 30);
						iv->setFilled(true);
						const Hobject *delReg = getDispObject(DelReion, i, j);
						RlRegion rlReg;
						HalOpenCV::hal2cvRegion(*delReg, &rlReg);
						iv->dispRegion(&rlReg);
					}
					HTuple regionok;
					getDispOutData(RegionSingleOk, i, j - 1, regionok);
					if (m_param->errPinSweepValid[i] && m_param->IsDetBase) {
						if (regionok[0].I() == 0) {
							iv->setColor(255, 0, 0);
						}
						else {
							iv->setColor(0, 255, 0);
						}
						HTuple shrowb, shrowe, shcolb, shcole, shrowc, shcolc;
						getDispOutData(ShRowBegin, i, j, shrowb);
						getDispOutData(ShRowEnd, i, j, shrowe);
						getDispOutData(ShColBegin, i, j, shcolb);
						getDispOutData(ShColEnd, i, j, shcole);
						getDispOutData(ShRowCenter, i, j, shrowc);
						getDispOutData(ShColCenter, i, j, shcolc);
						iv->dispLine(shrowb[0].D(), shcolb[0].D() - 3, shrowb[0].D(), shcolb[0].D() + 3);
						iv->dispLine(shrowb[0].D() - 3, shcolb[0].D(), shrowb[0].D() + 3, shcolb[0].D());
						iv->dispLine(shrowe[0].D(), shcole[0].D() - 3, shrowe[0].D(), shcole[0].D() + 3);
						iv->dispLine(shrowe[0].D() - 3, shcole[0].D(), shrowe[0].D() + 3, shcole[0].D());
						iv->dispLine(shrowc[0].D(), shcolc[0].D() - 3, shrowc[0].D(), shcolc[0].D() + 3);
						iv->dispLine(shrowc[0].D() - 3, shcolc[0].D(), shrowc[0].D() + 3, shcolc[0].D());
					}
					if (regionok[0].I() == 0) {
						iv->setColor(255, 0, 0);
					}
					else {
						iv->setColor(255, 255, 0);
					}
					if (m_param->errPinStandOffDiffValid[i]) {//如果长度差异ng，则显示所有的管脚为红色
						HTuple maxdiff;
						getDispOutData(StandOffDiffOK, i, maxdiff);
						if (maxdiff[0].I() == 0) {
							iv->setColor(255, 0, 0);
						}
					}
					getDispOutData(PinTopRow, i, j, toprow);
					getDispOutData(PinTopCol, i, j, topcol);
					iv->dispLine(toprow[0].D(), topcol[0].D() - 3, toprow[0].D(), topcol[0].D() + 3);
					iv->dispLine(toprow[0].D() - 3, topcol[0].D(), toprow[0].D() + 3, topcol[0].D());
					HTuple lbrow, lbcol, lerow, lecol;
					getDispOutData(lineBegRow, i, j, lbrow);
					getDispOutData(lineBegCol, i, j, lbcol);
					getDispOutData(lineEndRow, i, j, lerow);
					getDispOutData(lineEndCol, i, j, lecol);
					//iv->setColor(255,255,100);
					double rowb, colb, rowe, cole;
					rowb = lbrow[0].D();
					rowe = lerow[0].D();
					colb = lbcol[0].D();
					cole = lecol[0].D();
					iv->dispLine(rowb, colb, rowe, cole);
				}
				if ((j != 0 && (m_param->widthMethod >= 2))) {
					HTuple maxrowb, maxrowe, maxcolb, maxcole;
					getDispOutData(MaxWidthBeginRow, i, j, maxrowb);
					getDispOutData(MaxWidthBeginCol, i, j, maxcolb);
					getDispOutData(MaxWidthEndRow, i, j, maxrowe);
					getDispOutData(MaxWidthEndCol, i, j, maxcole);
					iv->dispLine(maxrowb[0].D(), maxcolb[0].D() - 3, maxrowb[0].D(), maxcolb[0].D() + 3);
					iv->dispLine(maxrowb[0].D() - 3, maxcolb[0].D(), maxrowb[0].D() + 3, maxcolb[0].D());
					iv->dispLine(maxrowe[0].D(), maxcole[0].D() - 3, maxrowe[0].D(), maxcole[0].D() + 3);
					iv->dispLine(maxrowe[0].D() - 3, maxcole[0].D(), maxrowe[0].D() + 3, maxcole[0].D());
				}
				if (!(j != 0 && m_param->widthMethod == 2)) {
					iv->dispLine(beginrow[0].D(), begincol[0].D() - 3, beginrow[0].D(), begincol[0].D() + 3);
					iv->dispLine(beginrow[0].D() - 3, begincol[0].D(), beginrow[0].D() + 3, begincol[0].D());
					iv->dispLine(endrow[0].D(), endcol[0].D() - 3, endrow[0].D(), endcol[0].D() + 3);
					iv->dispLine(endrow[0].D() - 3, endcol[0].D(), endrow[0].D() + 3, endcol[0].D());
				}
			}
		}
	}
}

void pinDetect1Mod::textResult(ResultText *text, const QString &funName)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	HTuple grpfound, found;
	for (int i = 0;i < GRP_NUM;i++) {
		if (m_param->grpPinNum[i] == 0) {
			continue;
		}
		getDispOutData(GrpFindOk, i, grpfound);
		if (grpfound[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
			text->append(QObject::tr(" leads group[%1] found failed").arg(i + 1));
			for (int j = 0;j < m_param->grpPinNum[i] + 1;j++) {
				if (!m_param->IsDetBase&&j == 0) {
					continue;
				}
				getDispOutData(FindOk, i, j, found);
				if (found[0].I() != 0) {
					if (j == 0) {
						text->append(QObject::tr(" edge found failed"));
					}
					else {
						if (found[0].I() == 1) {
							text->append(QObject::tr(" lead[%1]'s edge found failed").arg(j));
						}
						else if (found[0].I() == 2) {
							text->append(QObject::tr(" lead[%1]'s shoulder found failed").arg(j));
						}
						else {
							text->append(QObject::tr(" lead[%1]'s width found failed").arg(j));
						}
					}
				}
			}
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
			text->append(QObject::tr(" group[%1]:").arg(i + 1));
			char unit[32];
			if (m_param->IsInputRatio[i] || m_param->sizeRatio[i] != 1.0) {
				sprintf(unit, "mm");
			}
			else {
				sprintf(unit, "px");
			}
			for (int j = 0;j < m_param->grpPinNum[i];j++) {
				HTuple thisok;
				getDispOutData(RegionSingleOk, i, j, thisok);
				if (thisok[0].I() == 0) {
					text->setTextColor(QColor(255, 0, 0));
				}
				else
					text->setTextColor(QColor(0, 0, 0));
				text->append(QObject::tr("Lead%1:  ").arg(QString::number(j + 1)));
				if (m_param->errPinLengthValid[i] && m_param->IsDetBase) {
					HTuple standoff;
					getDispOutData(StandOff, i, j, standoff);
					HTuple itemok;
					getDispOutData(StandOffOK, i, j, itemok);
					if (itemok[0].I() == 0) {
						text->setTextColor(QColor(255, 0, 0));
					}
					else
						text->setTextColor(QColor(0, 0, 0));

					text->append(QObject::tr("length:%1 %4(%2,%3)  ").arg(QString::number(standoff[0].D(), 'f', 4)).
						arg(QString::number(m_param->errPinLengthMin[i][j], 'f', 4)).arg(QString::number(m_param->errPinLengthMax[i][j], 'f', 4))
						.arg(QString::fromLocal8Bit(unit)));
				}
				if (m_param->errPinSweepValid[i] && m_param->IsDetBase) {
					HTuple sweepmax, sweepmin;
					getDispOutData(PinMaxSweep, i, j, sweepmax);
					getDispOutData(PinMinSweep, i, j, sweepmin);
					HTuple itemok;
					getDispOutData(PinSweepOK, i, j, itemok);
					if (itemok[0].I() == 0) {
						text->setTextColor(QColor(255, 0, 0));
					}
					else
						text->setTextColor(QColor(0, 0, 0));

					text->append(QObject::tr("sweep:(%1,%2) %3(%4,%5)  ").arg(QString::number(sweepmin[0].D(), 'f', 4)).arg(QString::number(sweepmax[0].D(), 'f', 4))
						.arg(QString::fromLocal8Bit(unit)).arg(QString::number(m_param->errPinSweepMin[i][j], 'f', 4))
						.arg(QString::number(m_param->errPinSweepMax[i][j], 'f', 4))
					);
				}
				if (m_param->errPinWidthValid[i]) {
					HTuple pinwidthmin, pinwidthmax;
					getDispOutData(PinWidth, i, j, pinwidthmin);
					getDispOutData(PinMaxWidth, i, j, pinwidthmax);

					HTuple itemok;
					getDispOutData(PinWidthOK, i, j, itemok);
					if (itemok[0].I() == 0) {
						text->setTextColor(QColor(255, 0, 0));
					}
					else
						text->setTextColor(QColor(0, 0, 0));
					QString range;
					if (m_param->widthMethod == 0 || m_param->widthMethod == 1) {
						range = QObject::tr("%1").arg(QString::number(pinwidthmin[0].D(), 'f', 4));
					}
					else if (m_param->widthMethod == 2) {
						range = QObject::tr("%1").arg(QString::number(pinwidthmax[0].D(), 'f', 4));
					}
					else {
						range = QObject::tr("(%1,%2)").arg(QString::number(pinwidthmin[0].D(), 'f', 4)).arg(QString::number(pinwidthmax[0].D(), 'f', 4));
					}
					text->insertPlainText(QObject::tr("lead width:%1 %4(%2,%3)  ").arg(range).
						arg(QString::number(m_param->errPinWidthMin[i][j], 'f', 4)).arg(QString::number(m_param->errPinWidthMax[i][j], 'f', 4))
						.arg(QString::fromLocal8Bit(unit)));
				}
				if (m_param->errPinGapValid[i] && j < m_param->grpPinNum[i] - 1) {
					HTuple inter;
					getDispOutData(PinInterVal, i, j, inter);
					HTuple itemok;
					getDispOutData(PinInterValOK, i, j, itemok);
					if (itemok[0].I() == 0) {
						text->setTextColor(QColor(255, 0, 0));
					}
					else
						text->setTextColor(QColor(0, 0, 0));
					text->append(QObject::tr("lead pitch%1(lead%2-lead%3): ").arg(QString::number(i + 1)).arg(QString::number(i + 1)).
						arg(QString::number(i + 2)));
					text->insertPlainText(QString::fromLocal8Bit("%1 %4(%2,%3)").arg(QString::number(inter[0].D(), 'f', 4)).
						arg(QString::number(m_param->errPinGapMin[i][j], 'f', 4)).arg(QString::number(m_param->errPinGapMax[i][j], 'f', 4)).arg(QString::fromLocal8Bit(unit)));
				}
			} //enf of j loop
			if (m_param->errPinStandOffDiffValid[i])
			{
				HTuple maxdiff;
				getDispOutData(StandOffDiff, i, maxdiff);
				HTuple itemok;
				getDispOutData(StandOffDiffOK, i, itemok);
				if (itemok[0].I() == 0) {
					text->setTextColor(QColor(255, 0, 0));
				}
				else
					text->setTextColor(QColor(0, 0, 0));
				text->append(QObject::tr("maxdiff of length: %1 %2").arg(QString::number(maxdiff[0].D(), 'f', 4)).arg(unit));
				if (itemok[0].I() == 0) {
					text->insertPlainText(QString::fromLocal8Bit(">(%1)Ng").arg(QString::number(m_param->errPinStandOffDiffMax[i], 'f', 4)));
				}
				else
					text->insertPlainText(QString::fromLocal8Bit("<(%1)Pass").arg(QString::number(m_param->errPinStandOffDiffMax[i], 'f', 4)));
			}
		}
	}//end of i loop
}

void pinDetect1Mod::iniData()
{
	m_param = new pinDetect1Param;
	m_image = new Hobject;
	m_testItemsStatus.clear();
	m_testItemsStatus.resize(MAX_TESTITEMS_NUM);
	createPins();
}

void pinDetect1Mod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_pixelSize, "sz");
	for (auto i = 0;i < GRP_NUM;++i)
		addPin(&p_homMat2d[i], QString("hm2d%1").arg(i + 1));
	addPin(&p_corners, "corners");
	addPin(&p_SearchHomMat2D, "searchHm2d");
	addPin(&p_SearchCorners, "searchCors");

	addPin(&p_outLenEn, "lenEn");
	addPin(&p_outLen, "length");
	addPin(&p_outLead, "outLead");

	//addPin(&p_lenEn, "lenEn");
	//addPin(&p_outLen, "outLen");

	p_SearchHomMat2D.setVisible(false);
	p_SearchCorners.setVisible(false);

	p_outLenEn.setVisible(false);
	p_outLen.setVisible(false);
	p_outLead.setVisible(false);
}

bool pinDetect1Mod::IsPointInRegion(const Hobject * InputReg, double row, double col, int Larger)
{
	Hobject uobj;
	union1(*InputReg, &uobj);
	double dismin, dismax;
	distance_pr(uobj, row, col, &dismin, &dismax);
	bool IsIn;
	if ((dismin - Larger) < 0.001) {
		IsIn = true;
	}
	else {
		IsIn = false;
	}
	return IsIn;
}

int pinDetect1Mod::RansacLinear(stPoint * pstData, unsigned int nDataCnt, unsigned int nMinCnt, unsigned int nMaxIterCnt, unsigned int nConsesusCntThreshold, double dMaxErrorThreshold, double & dAA, double & dBB, double & dCC, double & dModelMeanError)
{
	//default_random_engine C++随机数产生器
	default_random_engine rng;
	//uniform_int_distribution ：产生均匀分布的随机数
	uniform_int_distribution<unsigned> uniform(0, nDataCnt - 1);
	rng.seed(10);
	set<unsigned int> sSelectIndexs;
	vector<stPoint> vecSelectPoints;
	//set<unsigned int> sConsensusIndexs;
	double dA = 0;
	double dB = 0;
	double dC = 0;

	dModelMeanError = 0;
	int nIsNonFind = 1;//
	unsigned int nBestConsensusCnt = 0;
	unsigned int nIter = 0;
	while (nIter < nMaxIterCnt)
	{
		sSelectIndexs.clear();
		vecSelectPoints.clear();

		//Step1:随机选择nMinCnt个点
		while (1)
		{
			unsigned int nIndex = uniform(rng);
			sSelectIndexs.insert(nIndex);
			if (sSelectIndexs.size() == nMinCnt)
			{
				break;
			}
		}

		//Step2:得出模型参数
		set<unsigned int>::iterator pSelectIter = sSelectIndexs.begin();
		while (pSelectIter != sSelectIndexs.end())
		{
			unsigned int nIndex = *pSelectIter;
			vecSelectPoints.push_back(pstData[nIndex]);
			pSelectIter++;
		}

		double dDeltaY = vecSelectPoints[1].y - vecSelectPoints[0].y;
		double dDeltaX = vecSelectPoints[1].x - vecSelectPoints[0].x;

		dA = dDeltaY;
		dB = -dDeltaX;

		dC = -dDeltaY*vecSelectPoints[1].x + dDeltaX*vecSelectPoints[1].y;

		//Step3：得到在阈值内的个数
		unsigned int nDataIter = 0;
		double dMeanError = 0;
		set<unsigned int >sTmpConsensusIndexs;
		while (nDataIter < nDataCnt)
		{
			double dDistance = (dA*pstData[nDataIter].x + dB*pstData[nDataIter].y + dC) / sqrt(dA*dA + dB*dB);
			dDistance = dDistance > 0 ? dDistance : -dDistance;
			if (dDistance < dMaxErrorThreshold)
			{
				sTmpConsensusIndexs.insert(nDataIter);
			}
			dMeanError += dDistance;
			nDataIter++;
		}

		//Step4：
		if (sTmpConsensusIndexs.size() >= nBestConsensusCnt && sTmpConsensusIndexs.size() >= nConsesusCntThreshold)
		{
			nBestConsensusCnt = sTmpConsensusIndexs.size();
			dModelMeanError = dMeanError / nDataCnt;
			//sConsensusIndexs.clear();
			//sConsensusIndexs = sTmpConsensusIndexs;
			nIsNonFind = 0;
			dAA = dA;
			dBB = dB;
			dCC = dC;

		}
		nIter++;
	}
	return nIsNonFind;
}

