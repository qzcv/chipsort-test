#include "pinLength1Mod.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include <opencv2/imgproc.hpp>
#include "halOpenCV.h"

#define AllOk "allok"
#define ObjEmpty "objectempty"  //如果区域内2值化的region为空，则直接ng,什么都不显1示
#define EmptyLead "emptyLead"   //当出现管脚为空的情况时那一个管脚的指数

#define AllPointsOk "allpointsOk" //所有的管脚点是否均找到
#define AngleIndex "angleIndex"   //

#define LeadOK "leadOk"  //管脚检测项目是否ng
#define SmallestRow "smallestRow"
#define SmallestCol "smallestCol"
#define SmallestLength1 "smallestlength1"
#define SmallestLength2 "smallestlength2"
#define SmallestAngle "smallestAngle"

#define WidthRow "widthrow"  //存储求取宽度时候的两个交点
#define WidthCol "widthcol"
#define LeadRow "leadrow"   //存储管脚尖的交点
#define LeadCol "leadcol"

#define OutLeadRow "outLeadRow"  //外传的管脚脚尖点
#define OutLeadCol "outLeadCol"  

#define BtEdgeRow "btEdgeRow"  //参考角度不对齐方式下的一维测量根部点
#define BtEdgeCol "btEdgeCol"

#define SweepWidRow "sweepWidRow" //管脚根宽度交点
#define SweepWidCol "sweepWidCol" //管脚根宽度交点
#define SweepCenterRow "sweepCenterRow" //管脚根宽度中心
#define SweepCenterCol "sweepCenterCol" //管脚根宽度中心

#define PointsDetectOk "pointsdetectOk" //是否检测到管脚交点等，0为ok，1为没找到宽度点，2为没找到脚尖点,3为没找到sweep边界点,4是没找到管脚不对齐方式的根点

#define LeadWidth "leadWidth"
#define LeadLength "leadLength"
#define LeadInter "leadInterval"
#define LeadWidthOk "leadWidthok"
#define LeadLengthOk "leadLengthok"
#define LeadInterOk "leadIntervalok"

#define LeadSweepOk "leadSweepOk"
#define LeadSweep "leadSweep"

#define LeadSpanOk "leadspanok"
#define LeadSpanVal "leadSpanVal"

#define LengthMaxDiff "LengthMaxDiff"
#define LengthMaxDiffOk "LengthMaxDiffOk"

#define AllMaxDiff "allMaxDiff"
#define AllMaxDiffOk "allMaxDiffOk"

#define TmpHomMat "TmpHomMat"  //用于保存显示的矩阵
#define TmpIndex "tmpIndex" //临时的传入指数

#define TmpSearchHomMat2D "tmpsearchHomMat2D" 

//传入的搜索区域
#define TmpSearchReferRow "TmpSearchReferRow"   //左上角顺时针顺序，传进来的区域4个点
#define TmpSearchReferCol "TmpSearchReferCol"

#define TmpReferRow "tmpReferRow"
#define TmpReferCol "tmpReferCol"

#define  ImageFull "imageFull"

#define LENGTH "length"
#define WIDTH "width"
#define PITCH "pitch"
#define LENGTHDIFF "lengthDiff"

pinLength1Mod::pinLength1Mod()
{
	iniData();
}

pinLength1Mod::pinLength1Mod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

pinLength1Mod::~pinLength1Mod()
{
	delete m_param;
	delete m_image;
}

int pinLength1Mod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int pinLength1Mod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void pinLength1Mod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void pinLength1Mod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int pinLength1Mod::run(const QString &funName)
{
// 	p_outLen->clear();
// 	p_outWid->clear();
// 	p_outPitch->clear();
// 	p_outLenDiff->clear();
// 	p_outCount->clear();

	p_outLen->clear();

	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
		m_testItemsStatus[i] = 1;
	}
	bool allok = true;
	if (p_im->empty()) {
		allok = false;
		setDetectOutData(AllOk, allok);
		setDetectOutData(ImageFull, false);
		//*p_resultOk = false;
		m_testItemsStatus[0] = 0;
		return -1;
		//setDetectOutData(ResultOk, false);
		//return allok;
	}
	else {
		setDetectOutData(ImageFull, true);
		HalOpenCV::cv2halImg(**p_im, m_image, false);
	}
	HTuple imgWidth, imgHeigth;
	get_image_size(*m_image, &imgWidth, &imgHeigth);
	double m_RatioSize[2];   //传入的标定数据，中左右上下
	if (m_param->IsInputSize) {
		HTuple pixelsize = **p_pixelSize;
		//getDetectInData(pixelSize, pixelsize);
		m_RatioSize[0] = pixelsize[0].D();
		m_RatioSize[1] = pixelsize[0].D();
	}
	else {
		m_RatioSize[0] = m_param->SizeRation[0];
		m_RatioSize[1] = m_param->SizeRation[1];
	}

	vector<HTuple> smallAngle[2], smallRow[2], smallCol[2], smallLen1[2], smallLen2[2];
	vector<double> edgeMidRow[2], edgeMidCol[2];
	vector<double> leadWidVec[2], leadlenVec[2];
	vector<double> leadTopRow[2], leadTopCol[2];  //脚尖的部分
	vector<double> widCenterRow[2], widCenterCol[2]; //管脚宽度中心点，当采用参考边界角度的时候，间距功能将由宽度中心点决定
	vector<double> scRow[2], scCol[2];   //sweepcenter中心点

	double referRow[2][2], referCol[2][2];  //边界参考的坐标

	Hobject roiRec, tmpImage;
	CvPoint2D32f edgeOffVector[4];  //求取的边界的外扩方向向量，左上角顺时针，为单位向量
	bool resultOk;
	if (m_param->IsReferSection != 0) {
		HTuple referrow, refercol;
		for (auto i = 0;i < p_corners->size();++i)
		{
			referrow[i] = p_corners->at(i).y;
			refercol[i] = p_corners->at(i).x;
		}
// 		getDetectInData(ReferRow, referrow);
// 		getDetectInData(ReferCol, refercol);
		try {
			for (int i = 0;i < 5;i++) {
				m_row[i] = referrow[i].D();
				m_col[i] = refercol[i].D();
			}
		}
		catch (HException& ex) {
			m_row[0] = m_col[0] = 50;
			m_row[1] = m_col[3] = 50;
			m_row[3] = m_col[1] = 150;
			m_row[2] = m_col[2] = 150;
			m_row[4] = m_row[0];
			m_col[4] = m_col[0];
		}
		//对传进来的坐标进行偏置
		//m_row[0]+=m_param->offsetUp;
		//m_row[1]+=m_param->offsetUp;
		//m_row[2]+=m_param->offsetDown;
		//m_row[3]+=m_param->offsetDown;
		//m_Col[0]+=m_param->offsetLeft;
		//m_Col[1]+=m_param->offsetRight;
		//m_Col[2]+=m_param->offsetRight;
		//m_Col[3]+=m_param->offsetLeft;

		for (int i = 0;i < 4;i++) {   //此处为一般形式的管脚角度求解方式，基于约定传入边界是左上顺时针的顺序
			double dy = m_row[i + 1] - m_row[i];
			double dx = m_col[i + 1] - m_col[i];
			double dxy = sqrt(dx*dx + dy*dy);
			dx = dx / dxy;
			dy = dy / dxy; //单位向量
			double da = PI;
			edgeOffVector[i].x = dx*cos(da) + dy*sin(da);   //旋转九十度垂直
			edgeOffVector[i].y = -dx*sin(da) + dy*cos(da);
		}
		//?????????????????????????????????
		//此处暂时未考虑边界方向向量在区域有角度属性时的情况，后续有需要时再行补充
		//??????????????????????????????????
		double edgeAngle[2];   //暂时保留以下未保证传入边界顺序的模式计算偏移边界一定距离的方式
							   //左右上下
		int eIdx[2][4] = { { 0,3,1,2 },{ 0,1,2,3 } };
		int tidx = m_param->LRorUD == 0 ? 0 : 1;
		edgeAngle[0] = -atan2((m_row[eIdx[tidx][0]] - m_row[eIdx[tidx][1]]), (m_col[eIdx[tidx][0]] - m_col[eIdx[tidx][1]]));
		edgeAngle[1] = -atan2((m_row[eIdx[tidx][2]] - m_row[eIdx[tidx][3]]), (m_col[eIdx[tidx][2]] - m_col[eIdx[tidx][3]]));
		for (int i = 0;i < 2;i++)
		{
			while (edgeAngle[i] > PI || edgeAngle[i] <= 0) {
				if (edgeAngle[i] > PI) {
					edgeAngle[i] -= PI;
				}
				if (edgeAngle[i] <= 0) {
					edgeAngle[i] += PI;
				}
			}
		}
		int edgePolar[2] = { 1,-1 };
		if (m_param->LRorUD == 1) {
			edgePolar[0] = (edgeAngle[0] > PI / 2.0) ? -1 : 1;
			edgePolar[1] = (edgeAngle[1] > PI / 2.0) ? 1 : -1;
		}
		edgeAngle[0] += edgePolar[0] * PI / 2.0;
		edgeAngle[1] += edgePolar[1] * PI / 2.0;

		int changeIdx[2][4] = { { 0,3,1,2 },{ 0,1,2,3 } };
		int changepolar[2] = { 1,1 };
		double offval[2] = { 0,0 };
		if (m_param->LRorUD == 0) { //左右
			if (m_param->offsetLeft != 0) {
				changepolar[0] = fabs(m_param->offsetLeft) / m_param->offsetLeft;
			}
			if (m_param->offsetRight != 0) {
				changepolar[1] = fabs(m_param->offsetRight) / m_param->offsetRight;
			}
			offval[0] = fabs(m_param->offsetLeft);
			offval[1] = fabs(m_param->offsetRight);
		}
		else if (m_param->LRorUD) {
			if (m_param->offsetUp != 0) {
				changepolar[0] = fabs(m_param->offsetUp) / m_param->offsetUp;
			}
			if (m_param->offsetDown != 0) {
				changepolar[1] = fabs(m_param->offsetDown) / m_param->offsetDown;
			}
			offval[0] = fabs(m_param->offsetUp);
			offval[1] = fabs(m_param->offsetDown);
		}
		m_row[changeIdx[tidx][0]] = m_row[changeIdx[tidx][0]] + -1.0*changepolar[0] * offval[0] * sin(edgeAngle[0]);
		m_row[changeIdx[tidx][1]] = m_row[changeIdx[tidx][1]] + -1.0*changepolar[0] * offval[0] * sin(edgeAngle[0]);
		m_row[changeIdx[tidx][2]] = m_row[changeIdx[tidx][2]] + -1.0*changepolar[1] * offval[1] * sin(edgeAngle[1]);
		m_row[changeIdx[tidx][3]] = m_row[changeIdx[tidx][3]] + -1.0*changepolar[1] * offval[1] * sin(edgeAngle[1]);

		m_col[changeIdx[tidx][0]] = m_col[changeIdx[tidx][0]] + changepolar[0] * offval[0] * cos(edgeAngle[0]);
		m_col[changeIdx[tidx][1]] = m_col[changeIdx[tidx][1]] + changepolar[0] * offval[0] * cos(edgeAngle[0]);
		m_col[changeIdx[tidx][2]] = m_col[changeIdx[tidx][2]] + changepolar[1] * offval[1] * cos(edgeAngle[1]);
		m_col[changeIdx[tidx][3]] = m_col[changeIdx[tidx][3]] + changepolar[1] * offval[1] * cos(edgeAngle[1]);

		HTuple tmprow, tmpcol;
		for (int i = 0;i < 4;i++) {
			tmprow[i] = m_row[i];
			tmpcol[i] = m_col[i];
		}
		setDetectOutData(TmpReferRow, tmprow);
		setDetectOutData(TmpReferCol, tmpcol);
	}
	if (m_param->PinAreaRefer) {
		if (p_homMat2D->size() != cv::Size(3, 2))
			hom_mat2d_identity(&m_homeMat2D);
		else
			HalOpenCV::cv2HalHomMat2D(*p_homMat2D, &m_homeMat2D);
		setDetectOutData(TmpHomMat, m_homeMat2D);
	}
	HTuple searchHommat;
	HTuple roireferrow, roirefercol; //外部传入搜索区域
	if (m_param->IsDelOffSearchRoi) {
		if (m_param->IsUseInputRegion) {
			for (auto i = 0;i < p_searchCorners->size();++i)
			{
				roireferrow[i] = p_searchCorners->at(i).y;
				roirefercol[i] = p_searchCorners->at(i).x;
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
			roireferrow[0] = roireferrow[0].D() + m_param->roiOffUp;
			roireferrow[1] = roireferrow[1].D() + m_param->roiOffUp;
			roireferrow[2] = roireferrow[2].D() + m_param->roiOffDown;
			roireferrow[3] = roireferrow[3].D() + m_param->roiOffDown;
			roireferrow[4] = roireferrow[0].D();
			roirefercol[0] = roirefercol[0].D() + m_param->roiOffLeft;
			roirefercol[1] = roirefercol[1].D() + m_param->roiOffRight;
			roirefercol[2] = roirefercol[2].D() + m_param->roiOffRight;
			roirefercol[3] = roirefercol[3].D() + m_param->roiOffLeft;
			roirefercol[4] = roirefercol[0].D();
			setDetectOutData(TmpSearchReferRow, roireferrow);
			setDetectOutData(TmpSearchReferCol, roirefercol);
		}
		else {
			if (p_searchHomMat2D->size() != cv::Size(2, 3))
				hom_mat2d_identity(&searchHommat);
			else
				HalOpenCV::cv2HalHomMat2D(*p_searchHomMat2D, &searchHommat);
// 			try {
// 				getDetectInData(SearchHomMat2D, searchHommat);
// 				double tv = searchHommat[0].D();
// 			}
// 			catch (HException& ex) {
// 				hom_mat2d_identity(&searchHommat);
// 			}
			setDetectOutData(TmpSearchHomMat2D, searchHommat);
		}
	}
	int inputIndex;
	if (m_param->IsUseDir) {
// 		try {
// 			HTuple indx;
// 			getDetectInData(InputIndex, indx);
// 			inputIndex = indx[0].I();
// 		}
// 		catch (HException& ex) {
// 			inputIndex = 0;
// 		}
		inputIndex = *p_inputIdx;
	}
	else {
		inputIndex = 0;
	}
	setDetectOutData(TmpIndex, inputIndex);
	bool allpointsOk = true;
	Hobject tosearchROI;
	Hobject imgRec;
	gen_rectangle1(&imgRec, 0, 0, imgHeigth[0].I() - 1, imgWidth[0].I() - 1);
	if (m_param->IsDelOffSearchRoi) {
		if (m_param->IsUseInputRegion) {
			Hobject inputroi;
			gen_region_polygon_filled(&tosearchROI, roireferrow, roirefercol);
		}
		else {
			Hobject rec2reg;
			gen_rectangle2(&rec2reg, m_param->SearchEdgeRoi.row, m_param->SearchEdgeRoi.col, m_param->SearchEdgeRoi.angle,
				m_param->SearchEdgeRoi.len1, m_param->SearchEdgeRoi.len2);
			affine_trans_region(rec2reg, &tosearchROI, searchHommat, "true");
		}
	}
	else {
		//copy_image(*m_image,&tosearchImg);
		gen_rectangle1(&tosearchROI, 0, 0, imgHeigth[0].I() - 1, imgWidth[0].I() - 1);
	}

	HTuple Outputlengthdiff[2];
	HTuple Outputlength[2], OutputPinwidth[2], OutputPininterval[2];  //用于传出的管脚的一些参数

	Hobject tosearchImg;
	reduce_domain(*m_image, tosearchROI, &tosearchImg);

	HTuple outLeadRow, outLeadCol; //传输到外部的管脚尖坐标

	for (int g = 0;g < 2;g++) {
		for (int i = 0;i < m_param->pinCont[g];i++) {   //左上管脚区域检1测
			HTuple vec;
			vec[0] = 0;
			smallRow[g].push_back(vec);
			smallCol[g].push_back(vec);
			smallAngle[g].push_back(vec);
			smallLen1[g].push_back(vec);
			smallLen2[g].push_back(vec);

			double vaal = 0;
			leadlenVec[g].push_back(vaal);
			leadWidVec[g].push_back(vaal);

			double pinrow1, pincol1, pinrow2, pincol2;
			double pinRow, pinCol, pinLength1, pinLength2;  //采用基准模式的时候需要切换至旋转矩1形
			double pinangle = 0;                                  //采用基准模式时的旋转角度
			int plusval = g > 0 ? m_param->pinCont[0] : 0;
			pinrow1 = m_param->segRegionVector[2 + i + plusval].row1;
			pincol1 = m_param->segRegionVector[2 + i + plusval].col1;
			pinrow2 = m_param->segRegionVector[2 + i + plusval].row2;
			pincol2 = m_param->segRegionVector[2 + i + plusval].col2;
			pinangle = atan((m_homeMat2D[3].D() / m_homeMat2D[0].D()));
			pinRow = (pinrow1 + pinrow2) / 2;
			pinCol = (pincol1 + pincol2) / 2;
			pinLength1 = (pincol2 - pincol1) / 2;
			pinLength2 = (pinrow2 - pinrow1) / 2;

			int mangleindex;
			// 			if(m_param->AngleType==0&&inputIndex==1){
			// 				mangleindex=2;
			// 			}
			// 			else{
			// 				mangleindex=inputIndex;
			// 			}
			if (m_param->IsUseDir)
			{
				if (m_param->AngleType == 0)
					mangleindex = (inputIndex - m_param->startAngle) * 2;
				else if (m_param->AngleType == 1)
					mangleindex = inputIndex - m_param->startAngle;

				if (mangleindex < 0)          //-1*90°=3*90°;-2*90°=2*90°;-3*90°=1*90°
					mangleindex += 4;
			}
			else
				mangleindex = inputIndex;
			setDetectOutData(AngleIndex, mangleindex);

			HTuple hommat;
			double rangle = PI*double(90.0*mangleindex) / 180.0;
			double rRow, rCol;   //旋转区域的中心点
			rRow = (m_param->rotateRoi.row1 + m_param->rotateRoi.row2) / 2.0;
			rCol = (m_param->rotateRoi.col1 + m_param->rotateRoi.col2) / 2.0;
			vector_angle_to_rigid(rRow, rCol, 0.0, rRow, rCol, rangle, &hommat);  //建立旋转关系矩阵
			affine_trans_pixel(hommat, pinRow, pinCol, &pinRow, &pinCol);  //旋转字符区域ROI

			gen_rectangle2(&roiRec, pinRow, pinCol, rangle, pinLength1, pinLength2);
			if (m_param->PinAreaRefer) {
				affine_trans_region(roiRec, &roiRec, m_homeMat2D, "false");
			}
			reduce_domain(tosearchImg, roiRec, &tmpImage);
			if (m_param->IsWhitePin) {
				threshold(tmpImage, &tmpImage, m_param->threValue, 255);
			}
			else {
				threshold(tmpImage, &tmpImage, 0, m_param->threValue);
			}
			if (m_param->openAndClosing > 0) {
				int vall = m_param->openAndClosing;
				closing_rectangle1(tmpImage, &tmpImage, vall, vall);
				opening_rectangle1(tmpImage, &tmpImage, vall, vall);
			}
			connection(tmpImage, &tmpImage);

			select_shape(tmpImage, &tmpImage, "area", "and", m_param->leadAreaMin, 999999);
			if (m_param->IsWidth) {
				select_shape(tmpImage, &tmpImage, "width", "and", m_param->widthMin, 9999);
				select_shape(tmpImage, &tmpImage, "height", "and", m_param->widthMin, 9999);
			}
			Hlong objCount = 0;
			count_obj(tmpImage, &objCount);
			if (objCount == 0) {
				setDetectOutData(ObjEmpty, false);
				int leadIndex = g*m_param->pinCont[0] + i;
				setDetectOutData(EmptyLead, leadIndex);
				setDetectOutData(AllOk, false);
				//*p_resultOk = false;
				return -1;
				//setDetectOutData(ResultOk, false);
				//return false;
			}
			union1(tmpImage, &tmpImage);

			int leadDir;   //管脚外侧朝向的方向，0123 左右上下
			int tmpDir;    //采用边界参考对调时的dir
			if (!m_param->LRorUD) {  //左右
				if (mangleindex == 0) {
					leadDir = (g == 0 ? 0 : 1);
				}
				else if (mangleindex == 1) {
					leadDir = (g == 0 ? 3 : 2);
				}
				else if (mangleindex == 2) {
					leadDir = (g == 0 ? 1 : 0);
				}
				else if (mangleindex == 3) {
					leadDir = (g == 0 ? 2 : 3);
				}
			}
			else if (m_param->LRorUD) {  //上下
				if (mangleindex == 0) {
					leadDir = (g == 0 ? 2 : 3);
				}
				else if (mangleindex == 1) {
					leadDir = (g == 0 ? 0 : 1);
				}
				else if (mangleindex == 2) {
					leadDir = (g == 0 ? 3 : 2);
				}
				else if (mangleindex == 3) {
					leadDir = (g == 0 ? 1 : 0);
				}
			}
			if (m_param->IsReferEdgeRev && (m_param->IsEdgeSopType == 0)) {
				if (leadDir == 0 || leadDir == 1) {
					tmpDir = leadDir == 0 ? 1 : 0;
				}
				else if (leadDir == 2 || leadDir == 3) {
					tmpDir = leadDir == 2 ? 3 : 2;
				}
			}
			else {
				tmpDir = leadDir;
			}
			int index1, index2;
			double mreferangle;  //基线的角度，用于检测宽度点
			if (m_param->IsReferSection != 0)   //左上角顺时针
			{
				if (leadDir == 0) {
					index1 = 0;
					index2 = 3;
				}
				else if (leadDir == 1) {
					index1 = 1;
					index2 = 2;
				}
				else if (leadDir == 2) {
					index1 = 0;
					index2 = 1;
				}
				else if (leadDir == 3) {
					index1 = 2;
					index2 = 3;
				}
				if (i == 0) {
					referRow[g][0] = m_row[index1];
					referRow[g][1] = m_row[index2];
					referCol[g][0] = m_col[index1];
					referCol[g][1] = m_col[index2];
				}
				mreferangle = -atan((referRow[g][1] - referRow[g][0]) / (referCol[g][1] - referCol[g][0]));
			}

			if (m_param->IsReferSection && (m_param->IsNoDetectTop || (m_param->IsOnlyReferEdge[g] && (m_param->IsEdgeSopType == 0)))) {  //在使用边界参考时，当不检测管脚尖或者仅参考边界角度时矫正外接矩形
				double rotaangle = mreferangle;
				if (leadDir == 0 || leadDir == 1) {
					while (!(rotaangle <= PI&&rotaangle >= 0)) {
						if (rotaangle > PI) {
							rotaangle -= PI;
						}
						else if (rotaangle < 0) {
							rotaangle += PI;
						}
					}
					rotaangle = (rotaangle - PI / 2.0);
				}
				else {
					while (!(rotaangle <= PI / 2.0&&rotaangle >= -PI / 2.0)) {
						if (rotaangle > PI / 2.0) {
							rotaangle -= PI;
						}
						else if (rotaangle < -PI / 2.0) {
							rotaangle += PI;
						}
					}
					rotaangle = rotaangle;
				}
				HTuple rhommat;
				vector_angle_to_rigid(pinRow, pinCol, 0.0, pinRow, pinCol
					, -rotaangle, &rhommat);  //建立旋转关系矩阵
				affine_trans_region(tmpImage, &tmpImage, rhommat, "true");
				HTuple r1row1, r1row2, r1col1, r1col2;
				smallest_rectangle1(tmpImage, &r1row1, &r1col1, &r1row2, &r1col2);
				objCount = r1row1.Num();
				if (objCount != 0) {
					int r1len1, r1len2;
					r1len1 = (r1col2[0].D() - r1col1[0].D()) / 2.0;
					r1len2 = (r1row2[0].D() - r1row1[0].D()) / 2.0;
					double r1row, r1col;
					r1row = (r1row2[0].D() + r1row1[0].D()) / 2.0;
					r1col = (r1col2[0].D() + r1col1[0].D()) / 2.0;
					vector_angle_to_rigid(pinRow, pinCol, 0.0, pinRow, pinCol
						, rotaangle, &rhommat);  //建立旋转关系矩阵
					affine_trans_pixel(rhommat, r1row, r1col, &r1row, &r1col);
					smallRow[g][i] = r1row;
					smallCol[g][i] = r1col;
					smallAngle[g][i] = rotaangle;
					smallLen1[g][i] = r1len1;
					smallLen2[g][i] = r1len2;
				}
			}
			else {
				smallest_rectangle2(tmpImage, &smallRow[g][i], &smallCol[g][i], &smallAngle[g][i], &smallLen1[g][i], &smallLen2[g][i]);
				objCount = smallRow[g][i].Num();
			}


			if (objCount == 0) {
				setDetectOutData(ObjEmpty, false);
				int leadIndex = g*m_param->pinCont[0] + i;
				setDetectOutData(EmptyLead, leadIndex);
				setDetectOutData(AllOk, false);
				//*p_resultOk = false;
				return -1;
				//setDetectOutData(ResultOk, false);
				//return false;
			}

			double smallrow = smallRow[g][i][0].D();
			double smallcol = smallCol[g][i][0].D();
			double smalllen1 = smallLen1[g][i][0].D();
			double smalllen2 = smallLen2[g][i][0].D();
			double smallangle = smallAngle[g][i][0].D();

			double widAngle; //检测宽度时一维测量的角度
			double lengthAngle;  //检测长度时候一维测量的角度

			vector_angle_to_rigid(smallrow, smallcol, 0.0, smallrow, smallcol
				, smallAngle[g][i][0].D(), &hommat);  //建立旋转关系矩阵

			double edgeRow, edgeCol;   //由外接矩形求到的管脚尖部分的线段中心点
			double leadWid, leadLength;
			bool isReverse;  //len1和wid len2和length是否相反
			if (abs(smallangle) <= (45.0 / 180.0*PI)) {
				if (tmpDir == 0) {
					edgeRow = smallrow;
					edgeCol = smallcol - smalllen1;
					leadWid = smalllen2*2.0;
					leadLength = smalllen1*2.0;
					lengthAngle = smallangle;
					isReverse = 1;
				}
				else if (tmpDir == 1) {
					edgeRow = smallrow;
					edgeCol = smallcol + smalllen1;
					leadWid = smalllen2*2.0;
					leadLength = smalllen1*2.0;
					lengthAngle = smallangle - PI;
					isReverse = 1;
				}
				else if (tmpDir == 2) {
					edgeRow = smallrow - smalllen2;
					edgeCol = smallcol;
					leadWid = smalllen1*2.0;
					leadLength = smalllen2*2.0;
					lengthAngle = smallangle - PI / 2.0;
					isReverse = 0;
				}
				else if (tmpDir == 3) {
					edgeRow = smallrow + smalllen2;
					edgeCol = smallcol;
					leadWid = smalllen1*2.0;
					leadLength = smalllen2*2.0;
					lengthAngle = smallangle + PI / 2.0;
					isReverse = 0;
				}
			}
			else if (smallangle <= (90.0 / 180.0*PI) && smallangle > (45.0 / 180.0*PI)) {
				if (tmpDir == 0) {
					edgeRow = smallrow - smalllen2;
					edgeCol = smallcol;
					leadWid = smalllen1*2.0;
					leadLength = smalllen2*2.0;
					lengthAngle = smallangle - PI / 2.0;
					isReverse = 0;
				}
				else if (tmpDir == 1) {
					edgeRow = smallrow + smalllen2;
					edgeCol = smallcol;
					leadWid = smalllen1*2.0;
					leadLength = smalllen2*2.0;
					lengthAngle = smallangle + PI / 2.0;
					isReverse = 0;
				}
				else if (tmpDir == 2) {
					edgeRow = smallrow;
					edgeCol = smallcol + smalllen1;
					leadWid = smalllen2*2.0;
					leadLength = smalllen1*2.0;
					lengthAngle = smallangle - PI;
					isReverse = 1;
				}
				else if (tmpDir == 3) {
					edgeRow = smallrow;
					edgeCol = smallcol - smalllen1;
					leadWid = smalllen2*2.0;
					leadLength = smalllen1*2.0;
					lengthAngle = smallangle;
					isReverse = 1;
				}
			}
			else if (smallangle >= (-90.0 / 180.0*PI) && smallangle < (-45.0 / 180.0*PI)) {
				if (tmpDir == 0) {
					edgeRow = smallrow + smalllen2;
					edgeCol = smallcol;
					leadWid = smalllen1*2.0;
					leadLength = smalllen2*2.0;
					lengthAngle = smallangle + PI / 2.0;
					isReverse = 0;
				}
				else if (tmpDir == 1) {
					edgeRow = smallrow - smalllen2;
					edgeCol = smallcol;
					leadWid = smalllen1*2.0;
					leadLength = smalllen2*2.0;
					lengthAngle = smallangle - PI / 2.0;
					isReverse = 0;
				}
				else if (tmpDir == 2) {
					edgeRow = smallrow;
					edgeCol = smallcol - smalllen1;
					leadWid = smalllen2*2.0;
					leadLength = smalllen1*2.0;
					lengthAngle = smallangle;
					isReverse = 1;
				}
				else if (tmpDir == 3) {
					edgeRow = smallrow;
					edgeCol = smallcol + smalllen1;
					leadWid = smalllen2*2.0;
					leadLength = smalllen1*2.0;
					lengthAngle = smallangle + PI;
					isReverse = 1;
				}
			}

			affine_trans_pixel(hommat, edgeRow, edgeCol, &edgeRow, &edgeCol);  //旋转ROI
			edgeMidRow[g].push_back(edgeRow);  //由外接矩形求到的管脚尖部分的线段中心点
			edgeMidCol[g].push_back(edgeCol);

			widAngle = lengthAngle + PI / 2.0;


			bool poinsfoundOk = true;
			double meaRow, meaCol; //一维测量的检测中心，脚尖点偏移一定距离后的检测点
			meaRow = edgeRow - sin(lengthAngle)*(double)m_param->widthOffset;
			meaCol = edgeCol + cos(lengthAngle)*(double)m_param->widthOffset;
			Hlong measureHandle;
			double detwidangle;
			if (m_param->IsReferSection) {
				detwidangle = mreferangle;
			}
			else {
				detwidangle = widAngle;
			}
			gen_measure_rectangle2(meaRow + 0.5, meaCol + 0.5, detwidangle/*widAngle*/, leadWid / 2.0 + 5, 5.0, imgWidth[0], imgHeigth[0], "nearest_neighbor", &measureHandle);
			HTuple rowEdge, colEdge, amp, distance;
			measure_pos(tosearchImg, measureHandle, 1, m_param->leadContrast, "all", "all", &rowEdge, &colEdge, &amp, &distance);
			close_measure(measureHandle);
			double widrow[2], widcol[2];  //管脚宽度求取的两个交点
			double widrow1[2], widcol1[2];
			double leadrow, leadcol;
			if (rowEdge.Num() >= 2) {
				HTuple nrowEdge, ncolEdge;
				if (m_param->CrossGrayValid)
				{
					for (int n = 0;n < rowEdge.Num();n++)
					{
						if (checkCrossGray(&tosearchImg, detwidangle, rowEdge[n].D(), colEdge[n].D(), m_param->crossGray, m_param->crossStep, "all"))
						{
							nrowEdge.Append(rowEdge[n].D());
							ncolEdge.Append(colEdge[n].D());
						}
					}
				}
				else {
					nrowEdge = rowEdge;ncolEdge = colEdge;
				}
				rowEdge.Reset();colEdge.Reset();
				rowEdge = nrowEdge;
				colEdge = ncolEdge;
			}
			if (rowEdge.Num() >= 2)
			{
				widrow[0] = rowEdge[0].D();
				widcol[0] = colEdge[0].D();
				widrow[1] = rowEdge[rowEdge.Num() - 1].D();
				widcol[1] = colEdge[rowEdge.Num() - 1].D();
				HTuple wid;
				distance_pp(widrow[0], widcol[0], widrow[1], widcol[1], &wid);
				leadWid = wid[0].D();
			}
			else {
				setDetectOutData(PointsDetectOk, 1, g, i);   //1 宽度没找到
				poinsfoundOk = false;
			}
			char edge[32];
			if (!m_param->IsWhitePin) {
				sprintf(edge, "negative");
			}
			else {
				sprintf(edge, "positive");
			}
			widCenterRow[g].push_back(0);
			widCenterCol[g].push_back(0);
			leadTopRow[g].push_back(0);
			leadTopCol[g].push_back(0);
			if (poinsfoundOk) {
				leadrow = (widrow[0] + widrow[1]) / 2.0;
				leadcol = (widcol[0] + widcol[1]) / 2.0;
				widCenterRow[g][i] = leadrow;
				widCenterCol[g][i] = leadcol;
				if (!m_param->IsNoDetectTop) {
					gen_measure_rectangle2(leadrow + 0.5, leadcol + 0.5, lengthAngle, m_param->widthOffset + 3, 2, imgWidth[0], imgHeigth[0], "nearest_neighbor", &measureHandle);
					measure_pos(tosearchImg, measureHandle, 1, m_param->leadContrast, edge, "first", &rowEdge, &colEdge, &amp, &distance);
					close_measure(measureHandle);
					if (rowEdge.Num() > 0) {
						leadrow = rowEdge[0].D();
						leadcol = colEdge[0].D();
						leadTopRow[g][i] = (leadrow);
						leadTopCol[g][i] = (leadcol);
						if (m_param->IsDelOffSearchRoi) {
							if (!IsPointInRegion(&tosearchROI, rowEdge[0].D(), colEdge[0].D())) {
								setDetectOutData(PointsDetectOk, 2, g, i);   //2 站高顶点没找到
								poinsfoundOk = false;
							}
						}
						if (m_param->CrossGrayValid)
						{
							if (!checkCrossGray(&tosearchImg, lengthAngle, rowEdge[0].D(), colEdge[0].D(), m_param->crossGray, m_param->crossStep, edge)) {
								setDetectOutData(PointsDetectOk, 2, g, i);   //2 站高顶点没找到
								poinsfoundOk = false;
							}
						}
						//需要根据新的顶点重新计算宽度点
						if (poinsfoundOk)
						{
							double meaRow1, meaCol1;
							meaRow1 = leadrow - sin(lengthAngle)*(double)m_param->widthOffset;
							meaCol1 = leadcol + cos(lengthAngle)*(double)m_param->widthOffset;
							Hlong measureHandle1;

							gen_measure_rectangle2(meaRow1 + 0.5, meaCol1 + 0.5, detwidangle/*widAngle*/, leadWid / 2.0 + 5.0, 5.0, imgWidth[0], imgHeigth[0], "nearest_neighbor", &measureHandle1);
							HTuple rowEdge1, colEdge1;
							measure_pos(tosearchImg, measureHandle1, 1, m_param->leadContrast, "all", "all", &rowEdge1, &colEdge1, &amp, &distance);
							close_measure(measureHandle1);
							if (rowEdge1.Num() >= 2) {
								HTuple nrowEdge, ncolEdge;
								if (m_param->CrossGrayValid)
								{
									for (int n = 0;n < rowEdge1.Num();n++)
									{
										if (checkCrossGray(&tosearchImg, detwidangle, rowEdge1[n].D(), colEdge1[n].D(), m_param->crossGray, m_param->crossStep, "all"))
										{
											nrowEdge.Append(rowEdge1[n].D());
											ncolEdge.Append(colEdge1[n].D());
										}
									}
								}
								else {
									nrowEdge = rowEdge1;ncolEdge = colEdge1;
								}
								rowEdge1.Reset();colEdge1.Reset();
								rowEdge1 = nrowEdge;
								colEdge1 = ncolEdge;
							}
							if (rowEdge1.Num() >= 2) {

								widrow1[0] = rowEdge1[0].D();
								widcol1[0] = colEdge1[0].D();
								widrow1[1] = rowEdge1[rowEdge1.Num() - 1].D();
								widcol1[1] = colEdge1[rowEdge1.Num() - 1].D();
								HTuple wid11;
								distance_pp(widrow1[0], widcol1[0], widrow1[1], widcol1[1], &wid11);
								leadWid = wid11[0].D();
								widCenterRow[g][i] = (widrow1[0] + widrow1[1]) / 2.0;
								widCenterCol[g][i] = (widcol1[0] + widcol1[1]) / 2.0;

							}
							else {
								setDetectOutData(PointsDetectOk, 1, g, i);   //1 宽度没找到
								poinsfoundOk = false;
							}
						}
					}
					else {
						setDetectOutData(PointsDetectOk, 2, g, i);   //2 站高顶点没找到
						poinsfoundOk = false;
					}
				}
				else {
					leadrow = edgeRow;  //由外接矩形求到的管脚尖部分的线段中心点
					leadcol = edgeCol;
					leadTopRow[g][i] = leadrow;
					leadTopCol[g][i] = leadcol;
				}
			}
			if (poinsfoundOk) {
				//先求出一般情况下的管脚长度
				double bottomrow, bottomcol;
				bottomrow = edgeRow - sin(lengthAngle)*(double)leadLength;
				bottomcol = edgeCol + cos(lengthAngle)*(double)leadLength;
				CvPoint2D32f vecA, vecB;  //向量a*b=(a,b)*cos
				vecA.x = bottomcol - edgeCol;
				vecA.y = bottomrow - edgeRow;
				vecB.x = bottomcol - leadcol;
				vecB.y = bottomrow - leadrow;
				double realLength;  //实际的管脚长度
				realLength = fabs((vecA.x*vecB.x + vecA.y*vecB.y) / double(leadLength));
				leadlenVec[g][i] = realLength;

				if (m_param->IsReferSection)   //左上角顺时针
				{
					double sRow, sCol; //一维测量的检测中心
					HTuple interRow, interCol, interParalle;
					double tmplen1;
					double interr, interc;
					if (!(m_param->IsOnlyReferEdge[g] && m_param->IsEdgeSopType == 0 && !m_param->IsNoDetectTop)) {
						sRow = leadrow - sin(lengthAngle)*10.0;
						sCol = leadcol + cos(lengthAngle)*10.0;

						intersection_ll(edgeRow, edgeCol, smallrow, smallcol, m_row[index1], m_col[index1], m_row[index2], m_col[index2], &interRow,
							&interCol, &interParalle);//显示外接矩形还是按照之前的来计算，但是后面实际计算还是以一维测量到的顶点为准
						smallRow[g][i][0] = (interRow[0].D() + edgeRow) / 2.0;
						smallCol[g][i][0] = (interCol[0].D() + edgeCol) / 2.0;

						distance_pp(interRow[0].D(), interCol[0].D(), edgeRow, edgeCol, &tmplen1);
						if (isReverse) {
							smallLen1[g][i][0] = tmplen1 / 2.0;
						}
						else {
							smallLen2[g][i][0] = tmplen1 / 2.0;
						}
						intersection_ll(sRow, sCol, leadrow, leadcol, m_row[index1], m_col[index1], m_row[index2], m_col[index2], &interRow,
							&interCol, &interParalle);  //此时计算管脚长度的直线：sRow->leadrow的角度为旋转矩形的角度
						interr = interRow[0].D();
						interc = interCol[0].D();
						distance_pp(interRow[0].D(), interCol[0].D(), leadrow, leadcol, &tmplen1);
						leadLength = tmplen1;  //管脚长度为真实的一维测量出的两点的长度
					}
					else {

						double meaBtRow, meaBtCol;  //底部测量长度点的起始坐标
						meaBtRow = leadrow - sin(lengthAngle)*(double)leadLength;
						meaBtCol = leadcol + cos(lengthAngle)*(double)leadLength;
						Hlong measureHandle1;

						gen_measure_rectangle2(meaBtRow + 0.5, meaBtCol + 0.5, lengthAngle + CV_PI/*widAngle*/, leadWid / 4.0, 3.0, imgWidth[0], imgHeigth[0], "nearest_neighbor", &measureHandle1);
						HTuple rowBtEdge, colBtEdge;
						measure_pos(tosearchImg, measureHandle1, 1, m_param->leadContrast, edge, "first", &rowBtEdge, &colBtEdge, &amp, &distance);
						close_measure(measureHandle1);
						bool pok = true;
						if (rowBtEdge.Num() > 0)
						{
							if (m_param->CrossGrayValid)
							{
								if (!checkCrossGray(&tosearchImg, lengthAngle + CV_PI, rowBtEdge[0].D(), colBtEdge[0].D(), m_param->crossGray, m_param->crossStep, edge)) {
									pok = false;
								}
							}
						}
						if (pok) {
							distance_pp(rowBtEdge[0].D(), colBtEdge[0].D(), leadrow, leadcol, &tmplen1);
							leadLength = tmplen1;  //管脚长度为真实的一维测量出的两点的长度

							setDetectOutData(BtEdgeRow, rowBtEdge, g, i);
							setDetectOutData(BtEdgeCol, colBtEdge, g, i);
							interc = rowBtEdge[0].D();
							interr = colBtEdge[0].D();
						}
						else {
							setDetectOutData(PointsDetectOk, 4, g, i);   //根部点没找到
							poinsfoundOk = false;
						}
					}
					if (m_param->errSweepValid[g]) {
						double sweepRow, sweepCol;  //左右上下
						int edgeIdx[4] = { 3,1,0,2 };  //传入边界的顺序
						int sweepPolar = (m_param->IsReferEdgeRev && (m_param->IsEdgeSopType == 0)) == true ? -1 : 1;
						sweepRow = interr + m_param->sweepOffset[g] * sweepPolar*edgeOffVector[edgeIdx[m_param->LRorUD * 2 + g]].y;
						sweepCol = interc + m_param->sweepOffset[g] * sweepPolar*edgeOffVector[edgeIdx[m_param->LRorUD * 2 + g]].x;

						gen_measure_rectangle2(sweepRow, sweepCol, detwidangle/*widAngle*/, leadWid / 2.0 + 5, 3, imgWidth[0], imgHeigth[0], "nearest_neighbor", &measureHandle);
						HTuple rowSweep, colSweep, ampSweep, disSweep;
						measure_pos(tosearchImg, measureHandle, 1, m_param->leadContrast, "all", "all", &rowSweep, &colSweep, &ampSweep, &disSweep);
						close_measure(measureHandle);
						double Sweeprow[2], Sweepcol[2];  //管脚宽度求取的两个交点
						double sweepcRow, sweepcCol;      //sweep存储的中心点

						if (rowSweep.Num() >= 2) {
							HTuple nrowSweep, ncolSweep;
							if (m_param->CrossGrayValid)
							{
								for (int n = 0;n < rowSweep.Num();n++)
								{
									if (checkCrossGray(&tosearchImg, detwidangle, rowSweep[n].D(), colSweep[n].D(), m_param->crossGray, m_param->crossStep, "all"))
									{
										nrowSweep.Append(rowSweep[n].D());
										ncolSweep.Append(colSweep[n].D());
									}
								}
							}
							else {
								nrowSweep = rowSweep;ncolSweep = colSweep;
							}
							rowSweep.Reset();colSweep.Reset();
							rowSweep = nrowSweep;
							colSweep = ncolSweep;
						}
						if (rowSweep.Num() >= 2) {
							Sweeprow[0] = rowSweep[0].D();
							Sweepcol[0] = colSweep[0].D();
							Sweeprow[1] = rowSweep[rowSweep.Num() - 1].D();
							Sweepcol[1] = colSweep[rowSweep.Num() - 1].D();
							scRow[g].push_back(0);
							scCol[g].push_back(0);
							scRow[g][i] = (Sweeprow[0] + Sweeprow[1]) / 2.0;
							scCol[g][i] = (Sweepcol[0] + Sweepcol[1]) / 2.0;
							HTuple outSrow, outScol;
							outSrow[0] = Sweeprow[0];
							outSrow[1] = Sweeprow[1];
							outScol[0] = Sweepcol[0];
							outScol[1] = Sweepcol[1];
							setDetectOutData(SweepWidRow, outSrow, g, i);
							setDetectOutData(SweepWidCol, outScol, g, i);
							setDetectOutData(SweepCenterRow, scRow[g][i], g, i);
							setDetectOutData(SweepCenterCol, scCol[g][i], g, i);
						}
						else {
							setDetectOutData(PointsDetectOk, 3, g, i);   //1 宽度没找到
							poinsfoundOk = false;
						}
					}
				}
				if (poinsfoundOk) {
					setDetectOutData(PointsDetectOk, 0, g, i);
				}
				HTuple mwidrow, mwidcol;
				mwidrow[0] = widrow1[0];
				mwidrow[1] = widrow1[1];
				mwidcol[0] = widcol1[0];
				mwidcol[1] = widcol1[1];
				setDetectOutData(WidthRow, mwidrow, g, i);
				setDetectOutData(WidthCol, mwidcol, g, i);
				setDetectOutData(LeadRow, leadrow, g, i);
				setDetectOutData(LeadCol, leadcol, g, i);
				outLeadRow.Append(leadrow);
				outLeadCol.Append(leadcol);
			}
			if (!poinsfoundOk)
			{
				allpointsOk = false;
			}
			setDetectOutData(SmallestRow, smallRow[g][i][0], g, i);
			setDetectOutData(SmallestCol, smallCol[g][i][0], g, i);
			setDetectOutData(SmallestLength1, smallLen1[g][i][0], g, i);
			setDetectOutData(SmallestLength2, smallLen2[g][i][0], g, i);
			setDetectOutData(SmallestAngle, smallAngle[g][i][0], g, i);
			double agle = smallAngle[g][i][0].D();
			double rw = smallRow[g][i][0].D();
			double cl = smallCol[g][i][0].D();
			leadlenVec[g][i] = leadLength;
			leadWidVec[g][i] = leadWid;
		}//end of loop i 
	}  //end of loop group
	setDetectOutData(ObjEmpty, true);
	setDetectOutData(AllPointsOk, allpointsOk);
	if (!allpointsOk) {
		setDetectOutData(AllOk, false);
		m_testItemsStatus[0] = 0;
		//*p_resultOk = false;
		return -1;
		//setDetectOutData(ResultOk, false);
		//return false;
	}
	setDetectOutData(OutLeadRow, outLeadRow);
	setDetectOutData(OutLeadCol, outLeadCol);
	vector<double> leadInter;
	double leadlengthMin[2] = { 500000,500000 };
	double leadLengthMax[2] = { 0,0 };
	for (int g = 0;g < 2;g++) {
		for (int i = 0;i < m_param->pinCont[g];i++) {
			leadlenVec[g][i] = m_RatioSize[g] * leadlenVec[g][i];
			leadWidVec[g][i] = m_RatioSize[g] * leadWidVec[g][i];
			edgeMidRow[g][i] = m_RatioSize[g] * edgeMidRow[g][i];
			edgeMidCol[g][i] = m_RatioSize[g] * edgeMidCol[g][i];
			bool leadOk = true;
			leadlenVec[g][i] += m_param->LeadLengthCorrNew[g][i];

			bool thisok = true;
			if (m_param->errLengthValid[g]) {
				if (leadlenVec[g][i] > m_param->errLengthMax[g][i] || leadlenVec[g][i] < m_param->errLengthMin[g][i]) {
					leadOk = false;
					thisok = false;
					allok = false;
					m_testItemsStatus[2] = 0;
				}
			}
			setDetectOutData(LeadLengthOk, thisok, g, i);
			setDetectOutData(LeadLength, leadlenVec[g][i], g, i);
			Outputlength[g][i] = leadlenVec[g][i];

			leadWidVec[g][i] += m_param->LeadWidthCorrNew[g][i];
			if (m_param->errWidthValid[g]) {
				bool thisok = true;
				if (leadWidVec[g][i] > m_param->errWidthMax[g][i] || leadWidVec[g][i] < m_param->errWidthMin[g][i]) {
					leadOk = false;
					thisok = false;
					allok = false;
					m_testItemsStatus[1] = 0;
				}
				setDetectOutData(LeadWidthOk, thisok, g, i);
				setDetectOutData(LeadWidth, leadWidVec[g][i], g, i);
				OutputPinwidth[g][i] = leadWidVec[g][i];
			}

			if (m_param->errSweepValid[g] && m_param->IsReferSection) {  //限定方式区分为角度偏移和距离偏移
																		 //bool thisok=true;
																		 //if (1){  //角度偏移
																		 //	int edgeIdx[4]={3,1,0,2};  //传入边界的顺序
																		 //	int sweepPolar=m_param->IsReferEdgeRev==1?-1:1;
																		 //	sweepRow=interr+m_param->sweepOffset[g]*sweepPolar*edgeOffVector[edgeIdx[m_param->LRorUD*2+g]].y;
																		 //	double tmpr=scRow[g][i]+10.0*edgeOffVector[g].y

																		 //	angle_ll(scRow[g][i],scCol[g][i],widCenterRow[g][i],widCenterCol[g][i],);
																		 //}
																		 //else{

																		 //}


																		 //if (leadWidVec[g][i]>m_param->errWidthMax[g][i] || leadWidVec[g][i]<m_param->errWidthMin[g][i]){
																		 //	leadOk=false;
																		 //	thisok=false;
																		 //	allok=false;
																		 //	m_testItemsStatus[1]=0;
																		 //}
																		 //setDetectOutData(LeadWidthOk,thisok,g,i);
																		 //setDetectOutData(LeadWidth,leadWidVec[g][i],g,i);
																		 //OutputPinwidth[g][i]=leadWidVec[g][i];
			}

			if (m_param->errInterValid[g] && i < m_param->pinCont[g] - 1) {
				double inter = 0;
				if (m_param->IsInterUseEdgeAngle && (m_param->IsEdgeSopType == 0)) {
					distance_pp(widCenterRow[g][i + 1], widCenterCol[g][i + 1], widCenterRow[g][i], widCenterCol[g][i]
						, &inter);
				}
				else {
					distance_pp(leadTopRow[g][i + 1], leadTopCol[g][i + 1], leadTopRow[g][i], leadTopCol[g][i]
						, &inter);
				}
				if (m_param->IsReferSection) {
					double anglell;
					if (m_param->IsInterUseEdgeAngle && (m_param->IsEdgeSopType == 0)) {
						angle_ll(widCenterRow[g][i + 1], widCenterCol[g][i + 1], widCenterRow[g][i], widCenterCol[g][i],
							referRow[g][0], referCol[g][0], referRow[g][1], referCol[g][1], &anglell);
					}
					else {
						angle_ll(leadTopRow[g][i + 1], leadTopCol[g][i + 1], leadTopRow[g][i], leadTopCol[g][i],
							referRow[g][0], referCol[g][0], referRow[g][1], referCol[g][1], &anglell);
					}
					inter = inter*abs(cos(anglell));
				}
				inter = inter*m_RatioSize[g];
				inter += m_param->LeadInterCorrNew[g][i];
				leadInter.push_back(inter);
				bool thisok = true;
				if (inter > m_param->errInterMax[g][i] || inter < m_param->errInterMin[g][i]) {
					leadOk = false;
					thisok = false;
					allok = false;
					m_testItemsStatus[3] = 0;
				}
				setDetectOutData(LeadInterOk, thisok, g, i);
				setDetectOutData(LeadInter, inter, g, i);
				OutputPininterval[g][i] = inter;
			}
			setDetectOutData(LeadOK, leadOk, g, i);
			if (leadLengthMax[g] < leadlenVec[g][i]) {
				leadLengthMax[g] = leadlenVec[g][i];
			}
			if (leadlengthMin[g] > leadlenVec[g][i]) {
				leadlengthMin[g] = leadlenVec[g][i];
			}
		}  //end of loop i
		if (m_param->errMaxDiffValid[g])
		{
			bool thisok = true;
			if ((leadLengthMax[g] - leadlengthMin[g]) > m_param->errMaxDiff[g]) {
				thisok = false;
				allok = false;
				m_testItemsStatus[5] = 0;
			}
			setDetectOutData(LengthMaxDiff, leadLengthMax[g] - leadlengthMin[g], g);
			setDetectOutData(LengthMaxDiffOk, thisok, g);
			Outputlengthdiff[g] = leadLengthMax[g] - leadlengthMin[g];
		}
	} //end of loop g
	int leadMaxNum;
	if (m_param->leadLossIndex == 0) {
		m_param->leadLossIndex = 1;
	}
	if (m_param->errSpanValid) {
		int g1, g2;
		if (m_param->pinCont[0] >= m_param->pinCont[1]) {
			g1 = 0;
			g2 = 1;
			leadMaxNum = m_param->pinCont[0];
		}
		else {
			g1 = 1;
			g2 = 0;
			leadMaxNum = m_param->pinCont[1];
		}
		bool IsSameNum = true; //管脚数目是否一致
		if (m_param->pinCont[0] != m_param->pinCont[1]) {
			IsSameNum = false;
		}
		if (m_param->IsHNW&&m_param->IsReferSection) {   //针对的是恒诺微的三个脚的产品的特殊跨距方式，不具备代表性
			double plasticWid, singleLeadLen;
			distance_pl(referRow[1][0], referCol[1][0], referRow[0][0], referCol[0][0], referRow[0][1], referCol[0][1],
				&plasticWid);
			distance_pl(leadTopRow[g2][0], leadTopCol[g2][0], referRow[g2][0], referCol[g2][0], referRow[g2][1], referCol[g2][1],
				&singleLeadLen);

			for (int i = 0;i < leadMaxNum;i++) {
				double spanval;
				double len1;
				distance_pl(leadTopRow[g1][i], leadTopCol[g1][i], referRow[g1][0], referCol[g1][0], referRow[g1][1], referCol[g1][1], &len1);
				spanval = singleLeadLen + plasticWid + len1;
				spanval *= m_RatioSize[0];
				spanval += m_param->LeadSpanCorr[i];
				bool thisok = true;
				if (spanval > m_param->errSpanMax[i] || spanval < m_param->errSpanMin[i]) {
					thisok = false;
					allok = false;
					m_testItemsStatus[4] = 0;
				}
				setDetectOutData(LeadSpanVal, spanval, i);
				setDetectOutData(LeadSpanOk, thisok, i);
			}  //end of loop i
		}
		else {
			if (!(!IsSameNum && (m_param->leadLossIndex > leadMaxNum))) {
				double plasticWid;
				if (m_param->IsReferSection) {
					double tmp1, tmp2;
					distance_pl(referRow[1][0], referCol[1][0], referRow[0][0], referCol[0][0], referRow[0][1], referCol[0][1],
						&tmp1);
					distance_pl(referRow[1][1], referCol[1][1], referRow[0][0], referCol[0][0], referRow[0][1], referCol[0][1],
						&tmp2);
					plasticWid = (tmp1 + tmp2) / 2.0;
				}
				for (int i = 0;i < leadMaxNum;i++) {
					double spanval;
					bool IsLossIndex = false;
					if (!IsSameNum&& i == (m_param->leadLossIndex - 1)) {
						IsLossIndex = true;
					}
					if (m_param->IsReferSection) {   //有参考边界时，对称脚型为两脚尖到边界的垂直投影，再加上塑封的宽度；非对称为脚尖到塑封中心
						double len1, len2;           //无参考边界时，对称脚型为脚尖到脚尖的距离，非对称为单独的脚长度
						distance_pl(leadTopRow[g1][i], leadTopCol[g1][i], referRow[g1][0], referCol[g1][0], referRow[g1][1], referCol[g1][1],
							&len1);
						int tmpi = i;
						if (!IsSameNum) {
							if (!IsLossIndex) {
								int plusval = i > (m_param->leadLossIndex - 1) ? 1 : 0;
								tmpi = i - plusval;
							}
							else {
								spanval = len1 + plasticWid / 2.0;
							}
						}
						if (!(!IsSameNum && (IsLossIndex))) {  //非独脚
							distance_pl(leadTopRow[g2][tmpi], leadTopCol[g2][tmpi], referRow[g2][0], referCol[g2][0], referRow[g2][1], referCol[g2][1],
								&len2);
							spanval = len1 + len2 + plasticWid;
						}
					}
					else {
						int tmpi = i;
						if (!IsSameNum&&IsLossIndex) {
							spanval = leadlenVec[g1][i];
						}
						else {
							if (!IsSameNum && !IsLossIndex) {
								int plusval = i > (m_param->leadLossIndex - 1) ? 1 : 0;
								tmpi = i - plusval;
							}
							distance_pp(leadTopRow[g1][i], leadTopCol[g1][i], leadTopRow[g2][tmpi], leadTopCol[g2][tmpi], &spanval);
						}
					}
					spanval *= m_RatioSize[0];
					spanval += m_param->LeadSpanCorr[i];
					bool thisok = true;
					if (spanval > m_param->errSpanMax[i] || spanval < m_param->errSpanMin[i]) {
						thisok = false;
						allok = false;
						m_testItemsStatus[4] = 0;
					}
					setDetectOutData(LeadSpanVal, spanval, i);
					setDetectOutData(LeadSpanOk, thisok, i);
				}  //end of loop i
			}
		}

	}
	if (m_param->errAllMaxDiffValid)
	{
		bool thisok = true;
		float allmin;
		allmin = leadlengthMin[1] <= leadlengthMin[0] ? leadlengthMin[1] : leadlengthMin[0];
		float allMax;
		allMax = leadLengthMax[1] >= leadLengthMax[0] ? leadLengthMax[1] : leadLengthMax[0];
		if ((allMax - allmin) > m_param->errAllMaxDiff) {
			thisok = false;
			allok = false;
			m_testItemsStatus[6] = 0;
		}
		setDetectOutData(AllMaxDiff, allMax - allmin);
		setDetectOutData(AllMaxDiffOk, thisok);
	}

	HTuple itemeenable;
	itemeenable[0] = (int)m_param->errWidthValid;
	itemeenable[1] = (int)m_param->errLengthValid;
	itemeenable[2] = (int)m_param->errInterValid;
	itemeenable[3] = (int)m_param->errSpanValid;
	if (m_param->errAllMaxDiffValid || m_param->errMaxDiff) {
		itemeenable[4] = 1;
	}
	else {
		itemeenable[4] = 0;
	}

	setDetectOutData(AllOk, allok);
	//*p_resultOk = allok;
	//setDetectOutData(ResultOk, allok);
	if (allok) {
		if (m_param->errMaxDiffValid) {
			HTuple diffout;
			if (m_param->pinCont[0] < 2) {
				Outputlengthdiff[0] = 0;
			}
			if (m_param->pinCont[1] < 2) {
				Outputlengthdiff[1] = 0;
			}
			tuple_concat(Outputlengthdiff[0], Outputlengthdiff[1], &diffout);
			p_item->insert(LENGTHDIFF, toQList(diffout));
			//setDetectOutData(LeadOutDiff, diffout);
		}
		if (m_param->errLengthValid) {
			HTuple offout;
			tuple_concat(Outputlength[0], Outputlength[1], &offout);
			p_item->insert(LENGTH, toQList(offout));
			QList<double> outLen;
			for (auto i = 0; i < offout.Num(); ++i)
				outLen.push_back(offout[i].D());
			*p_outLen = outLen;
			//setDetectOutData(OutLength, offout);
		}
		if (m_param->errWidthValid) {
			HTuple offout;
			tuple_concat(OutputPinwidth[0], OutputPinwidth[1], &offout);
			p_item->insert(WIDTH, toQList(offout));
			//setDetectOutData(LeadOutWidth, offout);
		}
		if (m_param->errInterValid) {
			HTuple offout;
			tuple_concat(OutputPininterval[0], OutputPininterval[1], &offout);
			p_item->insert(PITCH, toQList(offout));
			//setDetectOutData(LeadOutPitch, offout);
		}
// 		*p_lenEnable = m_param->errLengthValid[0] && m_param->errLengthValid[1];
// 		*p_widEnable = m_param->errWidthValid[0] && m_param->errWidthValid[1];
// 		*p_pitchEnable = m_param->errInterValid[0] && m_param->errInterValid[1];
// 		*p_lenDiffEnable = m_param->errMaxDiffValid[0] && m_param->errMaxDiffValid[1];
// 		p_outCount->push_back(m_param->pinCont[0]);
// 		p_outCount->push_back(m_param->pinCont[1]);
		//setDetectOutData(LengthEnable, m_param->errLengthValid[0] && m_param->errLengthValid[1]);
		//setDetectOutData(WidthEnable, m_param->errWidthValid[0] && m_param->errWidthValid[1]);
		//setDetectOutData(InterEnable, m_param->errInterValid[0] && m_param->errInterValid[1]);
		//setDetectOutData(LengthDiffEnable, m_param->errMaxDiffValid[0] && m_param->errMaxDiffValid[1]);
		//HTuple pincount;
		//pincount[0] = m_param->pinCont[0];
		//pincount[1] = m_param->pinCont[1];
		//setDetectOutData(LeadOutCount, pincount);
	}
	return allok ? 0 : -1;
}

void pinLength1Mod::viewResult(ImageView *iv, const QString &funName, int)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	HTuple objempty;
	getDispOutData(ObjEmpty, objempty);
	if (objempty[0].I() == 0) {
		return;
	}
	HTuple homeMat2D;
	if (m_param->PinAreaRefer) {
		getDispOutData(TmpHomMat, homeMat2D);
	}
	HTuple inputIndex;
	getDispOutData(TmpIndex, inputIndex);

	HTuple allpointsok;
	getDispOutData(AllPointsOk, allpointsok);

	HTuple alldiffok;
	if (m_param->errAllMaxDiffValid&&allpointsok[0].I()) {
		getDispOutData(AllMaxDiffOk, alldiffok);
	}

	if (m_param->IsDelOffSearchRoi) {
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
		iv->setFilled(false);
		RlRegion rlreg;
		HalOpenCV::hal2cvRegion(rec2reg, &rlreg);
		iv->dispRegion(&rlreg);
	}

	int mval;
	HTuple leadrow, leadcol, leadlength1, leadlength2, leadangle;
	for (int g = 0;g < 2;g++) {
		for (int i = 0;i < m_param->pinCont[g];i++)
		{
			bool leadallok = true;
			HTuple pointsok;
			HTuple leadok;
			getDispOutData(PointsDetectOk, g, i, pointsok);
			mval = pointsok[0].I();
			if (pointsok[0].I() != 0) {
				leadallok = false;
			}
			else {
				if (allpointsok[0].I()) {
					getDispOutData(LeadOK, g, i, leadok);
					bool mdiffok = true;
					if (m_param->errMaxDiffValid[g]) {
						HTuple diffok;
						getDispOutData(LengthMaxDiffOk, g, diffok);
						if (!diffok[0].I()) {
							leadallok = false;;
						}
					}
					if (m_param->errAllMaxDiffValid) {   //之前在站立度差异出问题的时候区域显示没有变红，现在改成变红
						if (!alldiffok[0].I()) {
							leadallok = false;
						}
					}
					if (!leadok[0].I()) {
						leadallok = false;
					}
					if (m_param->errSpanValid) {
						if (m_param->IsHNW&&m_param->IsReferSection) {
							int maxIndex;
							if (m_param->pinCont[0] >= m_param->pinCont[1]) {
								maxIndex = 0;
							}
							else {
								maxIndex = 1;
							}
							if (g == maxIndex) {
								for (int i = 0;i < m_param->pinCont[maxIndex];i++) {
									HTuple spanok;
									getDispOutData(LeadSpanOk, i, spanok);
									if (!spanok[0].I()) {
										leadallok = false;
									}
								}
							}

						}
						else {
							if (((m_param->pinCont[0] != m_param->pinCont[1]) && (m_param->leadLossIndex > m_param->pinCont[0] && m_param->leadLossIndex > m_param->pinCont[1]))) {
								return;
							}
							HTuple spanok;
							if (m_param->pinCont[0] == m_param->pinCont[1]) {
								getDispOutData(LeadSpanOk, i, spanok);
								if (!spanok[0].I()) {
									leadallok = false;
								}
							}
							else {
								int g1, g2;
								if ((m_param->pinCont[0] > m_param->pinCont[1] && g == 0) || (m_param->pinCont[1] > m_param->pinCont[0] && g == 1)) {
									getDispOutData(LeadSpanOk, i, spanok);
									if (!spanok[0].I()) {
										leadallok = false;
									}
								}
								else {
									if (i >= (m_param->leadLossIndex - 1)) {
										getDispOutData(LeadSpanOk, (i + 1)/*>lead?(i-1):0*/, spanok);
									}
									else if (i < (m_param->leadLossIndex - 1)) {
										getDispOutData(LeadSpanOk, i, spanok);
									}
									if (!spanok[0].I()) {
										leadallok = false;
									}
								}
							}
						}
					}
				}
				else {
					leadallok = false;
				}
			}
			if (!leadallok)
				iv->setColor(255, 0, 0);
			else {
				iv->setColor(0, 255, 0);
			}
			iv->setFilled(false);
			if (m_param->IsShowAfterDet) {
				int mangleindex;
				HTuple angleInd;
				getDispOutData(AngleIndex, angleInd);
				mangleindex = angleInd[0].I();
				// 				if(m_param->AngleType==0&&inputIndex[0].I()==1){
				// 					mangleindex=2;
				// 				}
				// 				else{
				// 					mangleindex=inputIndex[0].I();
				// 				}
				Hobject roirec;
				double pinrow1, pincol1, pinrow2, pincol2;
				double pinRow, pinCol, pinLength1, pinLength2;  //采用基准模式的时候需要切换至旋转矩1形
				double pinangle = 0;                                  //采用基准模式时的旋转角度
				pinrow1 = m_param->segRegionVector[2 + i + m_param->pinCont[0] * g].row1;
				pincol1 = m_param->segRegionVector[2 + i + m_param->pinCont[0] * g].col1;
				pinrow2 = m_param->segRegionVector[2 + i + m_param->pinCont[0] * g].row2;
				pincol2 = m_param->segRegionVector[2 + i + m_param->pinCont[0] * g].col2;
				//pinangle=atan( (homeMat2D[3].D()/homeMat2D[0].D()) );
				pinRow = (pinrow1 + pinrow2) / 2;
				pinCol = (pincol1 + pincol2) / 2;
				pinLength1 = (pincol2 - pincol1) / 2;
				pinLength2 = (pinrow2 - pinrow1) / 2;
				HTuple hommat;
				double rangle = PI*double(90.0*mangleindex) / 180.0;
				double rRow, rCol;   //旋转区域的中心点
				rRow = (m_param->rotateRoi.row1 + m_param->rotateRoi.row2) / 2.0;
				rCol = (m_param->rotateRoi.col1 + m_param->rotateRoi.col2) / 2.0;
				vector_angle_to_rigid(rRow, rCol, 0.0, rRow, rCol, rangle, &hommat);  //建立旋转关系矩阵
				affine_trans_pixel(hommat, pinRow, pinCol, &pinRow, &pinCol);  //旋转区域ROI

				gen_rectangle2(&roirec, pinRow, pinCol, rangle, pinLength1, pinLength2);
				if (m_param->PinAreaRefer) {
					affine_trans_region(roirec, &roirec, homeMat2D, "false");
				}
				RlRegion rlReg;
				HalOpenCV::hal2cvRegion(roirec, &rlReg);
				iv->dispRegion(&rlReg);
				getDispOutData(SmallestRow, g, i, leadrow);
				getDispOutData(SmallestCol, g, i, leadcol);
				getDispOutData(SmallestLength1, g, i, leadlength1);
				getDispOutData(SmallestLength2, g, i, leadlength2);
				getDispOutData(SmallestAngle, g, i, leadangle);
				iv->dispRect2(leadrow[0].D(), leadcol[0].D(), leadangle[0].D(), leadlength1[0].D(), leadlength2[0].D());
			}
			HTuple leadrow, leadcol, widrow, widcol;
			if (pointsok[0].I() == 0 && allpointsok[0].I()) {
				getDispOutData(WidthRow, g, i, widrow);
				getDispOutData(WidthCol, g, i, widcol);
				iv->dispLine(widrow[0].D(), widcol[0].D() - 3, widrow[0].D(), widcol[0].D() + 3);
				iv->dispLine(widrow[0].D() - 3, widcol[0].D(), widrow[0].D() + 3, widcol[0].D());
				iv->dispLine(widrow[1].D(), widcol[1].D() - 3, widrow[1].D(), widcol[1].D() + 3);
				iv->dispLine(widrow[1].D() - 3, widcol[1].D(), widrow[1].D() + 3, widcol[1].D());
				if (!(m_param->IsReferSection&&m_param->IsNoDetectTop)) {
					getDispOutData(LeadRow, g, i, leadrow);
					getDispOutData(LeadCol, g, i, leadcol);
					iv->dispLine(leadrow[0].D(), leadcol[0].D() - 3, leadrow[0].D(), leadcol[0].D() + 3);
					iv->dispLine(leadrow[0].D() - 3, leadcol[0].D(), leadrow[0].D() + 3, leadcol[0].D());
				}
				if (m_param->IsOnlyReferEdge[g] && m_param->IsEdgeSopType == 0 && !m_param->IsNoDetectTop)
				{
					HTuple btRow, btCol;
					getDispOutData(BtEdgeRow, g, i, btRow);
					getDispOutData(BtEdgeCol, g, i, btCol);
					iv->dispLine(btRow[0].D(), btCol[0].D() - 3, btRow[0].D(), btCol[0].D() + 3);
					iv->dispLine(btRow[0].D() - 3, btCol[0].D(), btRow[0].D() + 3, btCol[0].D());
				}
			}
		}
	}
	if (m_param->IsReferSection) {
		HTuple referrow, refercol;
		getDispOutData(TmpReferRow, referrow);  //左上角顺时针
		getDispOutData(TmpReferCol, refercol);
		iv->setColor(255, 255, 0);
		int n1[2], n2[2];
		int leadDir; //左右0，上下1
		if (m_param->AngleType == 0) {
			if (!m_param->LRorUD) {
				leadDir = 0;
			}
			else {
				leadDir = 1;
			}
		}
		else if (m_param->AngleType == 1) {
			int mangleindex;
			HTuple angleInd;
			getDispOutData(AngleIndex, angleInd);
			mangleindex = angleInd[0].I();
			if (!m_param->LRorUD) {
				// 				if (inputIndex[0].I()==0||inputIndex[0].I()==2){
				// 					leadDir=0;
				// 				}
				// 				else if(inputIndex[0].I()==1||inputIndex[0].I()==3){
				// 					leadDir=1;
				// 				}
				if (mangleindex % 2 == 0)    //mangleindex=0、2
					leadDir = 0;
				else
					leadDir = 1;
			}
			else {
				// 				if (inputIndex[0].I()==0||inputIndex[0].I()==2){
				// 					leadDir=1;
				// 				}
				// 				else if(inputIndex[0].I()==1||inputIndex[0].I()==3){
				// 					leadDir=0;
				// 				}
				if (mangleindex % 2 == 0)    //mangleindex=0、2
					leadDir = 1;
				else
					leadDir = 0;
			}
		}
		if (leadDir == 0) {            //左右管脚
			n1[0] = 0;
			n1[1] = 3;
			n2[0] = 1;
			n2[1] = 2;
		}
		else if (leadDir == 1) {        //上下管脚
			n1[0] = 0;
			n1[1] = 1;
			n2[0] = 2;
			n2[1] = 3;
		}
		iv->dispLine(referrow[n1[0]], refercol[n1[0]], referrow[n1[1]], refercol[n1[1]]);
		iv->dispLine(referrow[n2[0]], refercol[n2[0]], referrow[n2[1]], refercol[n2[1]]);
	}
}

void pinLength1Mod::textResult(ResultText *text, const QString &funName)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	HTuple objempty;
	getDispOutData(ObjEmpty, objempty);
	if (objempty[0].I() == 0) {
		HTuple emptyindex;
		getDispOutData(EmptyLead, emptyindex);
		int leadIndex = emptyindex[0].I();
		int grp;
		int indexL;
		if (leadIndex >= m_param->pinCont[0]) {
			grp = 1;
			indexL = leadIndex - m_param->pinCont[0];
		}
		else {
			grp = 0;
			indexL = leadIndex;
		}
		text->setTextColor(QColor(255, 0, 0));
		text->append(QObject::tr("found group%1 lead%2 failed").arg(grp).arg(indexL));
		return;
	}

	HTuple allpointsok;
	getDispOutData(AllPointsOk, allpointsok);
	if (!allpointsok[0].I()) {
		for (int g = 0;g < 2;g++) {
			for (int i = 0;i < m_param->pinCont[g];i++) {
				HTuple poinsok;
				getDispOutData(PointsDetectOk, g, i, poinsok);
				int pval = poinsok[0].I();
				if (poinsok[0].I() != 0) {
					text->setTextColor(QColor(255, 0, 0));
				}
				else
					text->setTextColor(QColor(0, 0, 0));
				if (poinsok[0].I() == 1) {
					text->append(QObject::tr("Group%1-Lead%2:width's edgePoints found failed").arg(QString::number(g + 1)).arg(QString::number(i + 1)));
				}
				else if (poinsok[0].I() == 2) {
					text->append(QObject::tr("Group%1-Lead%2:lead's top Points found failed").arg(QString::number(g + 1)).arg(QString::number(i + 1)));
				}
				else if (poinsok[0].I() == 3) {
					text->append(QObject::tr("Group%1-Lead%2:lead's span Points found failed").arg(QString::number(g + 1)).arg(QString::number(i + 1)));
				}
				else if (poinsok[0].I() == 4) {
					text->append(QObject::tr("Group%1-Lead%2:lead's bottom edge Points found failed").arg(QString::number(g + 1)).arg(QString::number(i + 1)));
				}

			}
		}
		return;
	}
	for (int g = 0;g < 2;g++) {
		char Gname[32];
		if (m_param->LRorUD == 0) {
			if (g == 0) {
				sprintf(Gname, "Left");
			}
			else {
				sprintf(Gname, "Right");
			}
		}
		else {
			if (g == 0) {
				sprintf(Gname, "Up");
			}
			else {
				sprintf(Gname, "Down");
			}
		}
		text->append(QObject::tr("Group:%1 ").arg(Gname));

		for (int i = 0;i < m_param->pinCont[g];i++)
		{
			HTuple thisok;
			getDispOutData(LeadOK, g, i, thisok);
			if (thisok[0].I() == 0) {
				text->setTextColor(QColor(255, 0, 0));
			}
			else
				text->setTextColor(QColor(0, 0, 0));
			char namebuf[64];
			text->append(QObject::tr("Lead%1: ").arg(QString::number(i + 1)));
			if (m_param->errWidthValid[g]) {
				HTuple width;
				getDispOutData(LeadWidth, g, i, width);
				text->insertPlainText(QObject::tr("width:%1(%2,%3)  ").arg(QString::number(width[0].D(), 'f', 4)).
					arg(QString::number(m_param->errWidthMin[g][i], 'f', 4)).arg(QString::number(m_param->errWidthMax[g][i], 'f', 4)));//QString::number(12.5, 'f', 3);
			}
			if (m_param->errLengthValid[g]) {
				HTuple length;
				getDispOutData(LeadLength, g, i, length);
				text->insertPlainText(QObject::tr("length:%1(%2,%3)  ").arg(QString::number(length[0].D(), 'f', 4)).
					arg(QString::number(m_param->errLengthMin[g][i], 'f', 4)).arg(QString::number(m_param->errLengthMax[g][i], 'f', 4)));
			}
			if (m_param->errInterValid[g] && i < m_param->pinCont[g] - 1) {
				HTuple inter, interok;
				getDispOutData(LeadInter, g, i, inter);
				getDispOutData(LeadInterOk, g, i, interok);
				if (!interok[0].I()) {
					text->setTextColor(QColor(255, 0, 0));
				}
				else
					text->setTextColor(QColor(0, 0, 0));
				text->append(QObject::tr("interval %1(Lead%2-Lead%3): ").arg(QString::number(i + 1)).arg(QString::number(i + 2)).
					arg(QString::number(i + 1)));
				text->insertPlainText(QObject::tr("%1(%2,%3)").arg(QString::number(inter[0].D(), 'f', 4)).
					arg(QString::number(m_param->errInterMin[g][i], 'f', 4)).arg(QString::number(m_param->errInterMax[g][i], 'f', 4)));
			}
		} //end of loop i
		if (m_param->errMaxDiffValid[g])
		{
			HTuple maxdiffok, maxdiff;
			getDispOutData(LengthMaxDiffOk, g, maxdiffok);
			getDispOutData(LengthMaxDiff, g, maxdiff);
			if (maxdiffok[0].I() == 0) {
				text->setTextColor(QColor(255, 0, 0));
			}
			else if (maxdiffok[0].I() == 1)
				text->setTextColor(QColor(0, 0, 0));
			text->append(QObject::tr("Group%1's Leads Max Length Diff: %2").arg(g + 1).arg(QString::number(maxdiff[0].D(), 'f', 4)));
			if (maxdiffok[0].I() == 0) {
				text->insertPlainText(QObject::tr(">(%1)Ng").arg(QString::number(m_param->errMaxDiff[g], 'f', 4)));
			}
			else if (maxdiffok[0].I() == 1)
				text->insertPlainText(QObject::tr("<(%1)Pass").arg(QString::number(m_param->errMaxDiff[g], 'f', 4)));
		}
	}
	if (m_param->errAllMaxDiffValid)
	{
		HTuple maxdiffok, maxdiff;
		getDispOutData(AllMaxDiffOk, maxdiffok);
		getDispOutData(AllMaxDiff, maxdiff);
		if (maxdiffok[0].I() == 0) {
			text->setTextColor(QColor(255, 0, 0));
		}
		else if (maxdiffok[0].I() == 1)
			text->setTextColor(QColor(0, 0, 0));
		text->append(QObject::tr("maxdiff of length of all leads: %1").arg(QString::number(maxdiff[0].D(), 'f', 4)));
		if (maxdiffok[0].I() == 0) {
			text->insertPlainText(QString::fromLocal8Bit(">(%1)Ng").arg(QString::number(m_param->errAllMaxDiff, 'f', 4)));
		}
		else if (maxdiffok[0].I() == 1)
			text->insertPlainText(QString::fromLocal8Bit("<(%1)Pass").arg(QString::number(m_param->errAllMaxDiff, 'f', 4)));
	}
	if (m_param->errSpanValid) {
		if (((m_param->pinCont[0] != m_param->pinCont[1]) && (m_param->leadLossIndex > m_param->pinCont[0] && m_param->leadLossIndex > m_param->pinCont[1]))) {
			text->setTextColor(QColor(255, 0, 0));
			text->append(QObject::tr("the index of missing lead is wrong,please check and reset it! "));
			return;
		}
		int maxleadnum;
		maxleadnum = m_param->pinCont[0] >= m_param->pinCont[1] ? m_param->pinCont[0] : m_param->pinCont[1];
		for (int i = 0;i < maxleadnum;i++) {
			HTuple spanok, spanval;
			getDispOutData(LeadSpanOk, i, spanok);
			getDispOutData(LeadSpanVal, i, spanval);
			if (spanok[0].I() == 0) {
				text->setTextColor(QColor(255, 0, 0));
			}
			else if (spanok[0].I() == 1)
				text->setTextColor(QColor(0, 0, 0));
			text->append(QObject::tr("span%4: %1(%2,%3): ").arg(QString::number(spanval[0].D(), 'f', 4)).arg(QString::number(m_param->errSpanMin[i], 'f', 4)).
				arg(QString::number(m_param->errSpanMax[i], 'f', 4)).arg(i + 1));
		}
	}
}

void pinLength1Mod::iniData()
{
	m_param = new pinLength1Param;
	m_image = new Hobject;
	hom_mat2d_identity(&m_homeMat2D);
		
	for (auto i = 0;i < 5;++i)
	{
		m_row[i] = m_col[i] = 0;
	}

	createPins();
}

void pinLength1Mod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2D, "hm2d");
	addPin(&p_corners, "corners");
	addPin(&p_pixelSize, "psz");
	addPin(&p_inputIdx, "inputIdx");
	addPin(&p_searchHomMat2D, "searchHm2d");
	addPin(&p_searchCorners, "searchCor");

	addPin(&p_outLen, "outLen");
	addPin(&p_item, "item");

// 	addPin(&p_resultOk, "resultOk");
// 	addPin(&p_lenEnable, "lenEn");
// 	addPin(&p_outLen, "outLen");
// 	addPin(&p_widEnable, "widEn");
// 	addPin(&p_outWid, "outWid");
// 	addPin(&p_pitchEnable, "pitchEn");
// 	addPin(&p_outPitch, "outPitch");
// 	addPin(&p_lenDiffEnable, "lenDiffEn");
// 	addPin(&p_outLenDiff, "outLenDiff");
// 	addPin(&p_outCount, "outCnt");

	p_inputIdx.setVisible(false);
	p_searchHomMat2D.setVisible(false);
	p_searchCorners.setVisible(false);

	p_item.setVisible(false);
	p_item->insert(LENGTH, "length");
	p_item->insert(WIDTH, "width");
	p_item->insert(PITCH, "pitch");
	p_item->insert(LENGTHDIFF, "lengthDiff");

// 	p_resultOk.setVisible(false);
// 	p_lenEnable.setVisible(false);
// 	p_outLen.setVisible(false);
// 	p_widEnable.setVisible(false);
// 	p_outWid.setVisible(false);
// 	p_pitchEnable.setVisible(false);
// 	p_outPitch.setVisible(false);
// 	p_lenDiffEnable.setVisible(false);
// 	p_outLenDiff.setVisible(false);
// 	p_outCount.setVisible(false);
}

bool pinLength1Mod::IsPointInRegion(const Hobject * InputReg, double row, double col)
{
	Hobject uobj;
	union1(*InputReg, &uobj);
	//dilation_rectangle1(uobj,&uobj,1,1);
	//erosion_rectangle1(uobj,&uobj,1,1);
	HTuple index;
	get_region_index(uobj, row, col, &index);
	if (index.Num() > 0) {
		return 1;
	}
	else {
		return 0;
	}
}