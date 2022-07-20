#include "mark1Mod.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include <QFileInfoList>
#include <QDir>
#include "halOpenCV.h"

#define ENT_WRITE_IO "writeIO"

//dir
#define AllOk "allok"
#define CharFoundOk "charok" //各自的charRegion内是否Ok
#define CharDetectOK "chardetectOK"  //经过检测项目后各字符是否还ok
#define Similar "similar" //得到的相似1度
#define RoiOk "roiok"  //印1字偏移是否超出
#define RoiOkNum "roioknum" //印1字偏移是否超出的编号
#define MatchRow "matchrow" //匹1配结果row
#define MatchCol "matchcol" //匹1配结果col
#define MatchAngle "matchangle" //匹1配结果angle
#define MatchScore "matchscore" //匹1配结果score

#define MatchIndex "matchIndex" //每个字在第几个模板匹配到的
#define LayerMatchIndex "layerMatchIndex" //多图层模式时候匹配到的模板数

#define AreaDiffVal "areaDiffVal" //面积偏差
#define AreaDiffOk "areaDiffOk"
#define AreaRegionDisp "arearegiondisp" //显示的面积偏差

#define ExcessOk "ExcessOk" //字符多余ok
#define ExcessVal "ExcessVal" //

#define CrackAreaDisp "CrackAreaDisp"  //替换之前的面积偏差区域显示
#define ExcessAreaDisp "ExcessAreaDisp"

#define CrackOk "CrackOk"   //字符缺失
#define CrackVal "CrackVal" //

#define OffSetOk "offsetOk"       //偏移值ok
#define OffSetval "offsetval"     //偏移值具体值

#define OffXOk "offXOk"       //偏移值Xok
#define OffXval "offXval"     //偏移值X具体值

#define OffYOk "offYOk"       //偏移值Yok
#define OffYval "offYval"     //偏移值Y具体值

#define TmpHomMat "TmpHomMat"  //用于保存显示的矩阵
#define RegionMatchOk "regionMatchOk" //区域匹配
#define RegionMatchScore "regionMatchScore" //区域匹配

#define NeedAutoSaveImg "needAutoSaveImage" //需要自动保存图片

//#define 
#define TmpReferRow "tmpreferRow"
#define TmpReferCol "tmpreferCol"

//outdata
#define ImageFull "imagefull" //传入图片是否ok

mark1Mod::mark1Mod()
{
	iniData();
}

mark1Mod::mark1Mod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

mark1Mod::~mark1Mod()
{
	for (int i = 0;i < m_param->PatternNum;i++) {
		for (int d = 0;d < 4;d++) {
			for (int j = 0;j < m_modelId[i][d].size();j++) {
				if (m_modelId[i][d][j] >= 0) {
					clear_ncc_model(m_modelId[i][d][j]);
				}
			}
			m_modelId[i][d].clear();
			if (m_RegionModelId[i][d] >= 0) {
				clear_ncc_model(m_RegionModelId[i][d]);
			}
		}
	}
	delete m_charRegion;
	delete m_image;
	delete m_param;
}

int mark1Mod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int mark1Mod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void mark1Mod::resetOutput()
{
	*p_charReg = RlRegion();
}

void mark1Mod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void mark1Mod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);

	if (level == ProductLevel)
	{
		m_PatternEnable = true;
		m_dirSame = true;

		creatNccTemp(true);
		for (int i = 0;i < m_param->PatternNum;i++) {
			if (m_param->MultPatternEnable[i]) {
				createRegionTemp(i);
			}
		}
	}
}

int mark1Mod::run(const QString &funName)
{
	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
		m_testItemsStatus[i] = 1;
	}
	bool allok = true;
	m_ocrTempOK = true;
	int isLF = 0;
	int isRT = 0;
	m_ocrOutReg = false;
	if (p_im->empty()) {
		allok = false;
		setDetectOutData(AllOk, allok);
		//gen_empty_region(m_charRegion);
		*p_charReg = RlRegion();
		//setDetectOutData(CharRegion, (Hlong)m_charRegion);
		setDetectOutData(ImageFull, false);
		m_testItemsStatus[0] = 0;
		return -1;
		//return allok;
	}
	else {
		setDetectOutData(ImageFull, true);
		HalOpenCV::cv2halImg(*p_im, m_image, false);
	}

	if (m_param->IsUsePinDirOut&&m_param->IsDefaultSig) {
		isLF = 0;
		isRT = 0;
		//setDetectOutData(LFOK, isLF);
		//setDetectOutData(RTOK, isRT);
		*p_lf = isLF;
		*p_rt = isRT;
		setDetectOutData(AllOk, allok);
		//setDetectOutData(PINOutEnable, m_param->IsUsePinDirOut);
		//setDetectOutData(PIN1OUT, m_param->pin1OutNum);
		//setDetectOutData(PIN2OUT, m_param->pin2OutNum);
		//gen_empty_region(m_charRegion);
		*p_charReg = RlRegion();
		return -1;
		//setDetectOutData(CharRegion, (Hlong)m_charRegion);
		//return allok;
	}

	if (m_param->OcrValid && m_producting) {  //生产过程中启用OCR自动模板功能
		HTuple isdoocr;
		isdoocr = **p_doOcr;
		//getDetectInData(IsDoOCR, isdoocr);
		try {
			bool doOcr = isdoocr[0].I();
			if (doOcr) {
				m_param->IsMult = false;  //如果启用多模板自动关闭多模板
				clearTemp(0);
				m_ocrTempOK = autoTemp();
			}
		}
		catch (HException& ex) {
			m_ocrTempOK = false;
		}
		if (!m_ocrTempOK) {
			allok = false;
			setDetectOutData(AllOk, allok);
			//gen_empty_region(m_charRegion);
			*p_autoOcrOk = false;
			*p_charReg = RlRegion();
			//setDetectOutData(AutoOCRTempOK, false);
			//setDetectOutData(CharRegion, (Hlong)m_charRegion);
			m_testItemsStatus[0] = 0;
			return -1;
			//return false;
		}
		*p_autoOcrOk = true;
		//setDetectOutData(AutoOCRTempOK, true);
	}

	if (m_param->IsMult &&m_param->IsFontOrLayer == 0) {
		for (int i = 1;i < m_param->PatternNum;i++) {
			if (m_param->MultPatternEnable[i]) {
				if (m_param->charCounts[i] != m_param->charCounts[0]) {
					m_charRight = 2;
					break;
				}
			}
		}
	}
	else {
		if (m_param->charCounts[0] == 0) {
			m_charRight = 1;
		}
		else {
			m_charRight = 0;
		}
	}
	if (m_charRight == 0) {
		m_charWrongIndex = -1;
		for (int i = 0;i < m_param->PatternNum;i++) {
			if (!m_param->IsMult&&i > 0)
				break;
			if (m_param->MultPatternEnable[i]) {
				for (int j = 0;j < m_param->charCounts[i];j++) {
					int regheight = m_param->RegionRoi[i].row2 - m_param->RegionRoi[i].row1;
					int regwid = m_param->RegionRoi[i].col2 - m_param->RegionRoi[i].col1;
					if (((m_param->segRegionVector[i][j].row1 - m_param->RegionRoi[i].row1) < 0) ||
						((m_param->segRegionVector[i][j].col1 - m_param->RegionRoi[i].col1) < 0) ||
						((m_param->segRegionVector[i][j].row2 - m_param->RegionRoi[i].row1) > regheight) ||
						((m_param->segRegionVector[i][j].col2 - m_param->RegionRoi[i].col1) > regwid)) {
						m_charWrongIndex = i;
						m_charRight = 3;
						break;
					}
				}
			}
			if (m_charWrongIndex >= 0) {
				break;
			}
		}
	}
	double mpixelsize;
	if (m_param->IsInputRatio) {
		HTuple pix;
		pix = **p_pixelSize;
		//getDetectInData(InputPixSize, pix);
		try {
			mpixelsize = pix[0].D();
		}
		catch (HException& ex) {
			mpixelsize = m_param->pixelSize;
		}
	}
	else {
		mpixelsize = m_param->pixelSize;
	}

	int m_inputPatIndex = 0;  //传入的指数
	if (m_param->IsMult&&m_param->IsInputPatIndex) {
		if (m_producting) {
			HTuple patIdx;
			try {
				patIdx = **p_patIdx;
				//getDetectInData(InputPatIndex, patIdx);
				m_inputPatIndex = patIdx[0].I();
			}
			catch (HException& ex) {
				m_inputPatIndex = 0;
			}
			if (m_inputPatIndex > m_param->PatternNum - 1) {
				m_inputPatIndex = m_param->PatternNum - 1;
			}
			else if (m_inputPatIndex < 0) {
				m_inputPatIndex = 0;
			}
		}
		else {
			m_inputPatIndex = m_param->NowPatternNum;
		}

	}

	m_PatternEnable = false;
	for (int i = 0;i < m_param->PatternNum;i++) {
		if (m_param->IsMult&&m_param->IsInputPatIndex&&i == m_inputPatIndex) {  //判断传入模板指数时候模板是否设置好
			if (!m_param->MultPatternEnable[i]) {
				m_PatternEnable = false;
				break;
			}
		}
		if (m_param->IsMult&&i > 0) {
			continue;  //采用多模板的时候只需要第一组模板有设置即可
		}
		if (m_param->MultPatternEnable[i]) {
			m_PatternEnable = true;
		}
		if (i == 0 && !m_param->MultPatternEnable[i]) {
			m_PatternEnable = false;
			break;
		}
	}
	m_dirSame = true;
	if (m_param->IsUsePinDirOut) {
		if (!m_param->IsDirPattern) {
			m_dirSame = false;
		}
		else {
			if (m_param->Is2Dor4D != m_param->angleType) {
				m_dirSame = false;
			}
		}
	}
	if (m_PatternEnable == false || m_dirSame == false || m_charRight != 0) {
		allok = false;
		setDetectOutData(AllOk, allok);
		//gen_empty_region(m_charRegion);
		*p_charReg = RlRegion();
		//setDetectOutData(CharRegion, (Hlong)m_charRegion);
		m_testItemsStatus[0] = 0;
		return -1;
		//return allok;
	}
	int dirIndex = 0;  //角度信息，注意当采用两方向的时候180度传入的是1，而此模块对应的指数应该是2（别问我为什么不改成一样的，其他的模块都是1暂时没统一。。）
	if (m_param->IsDirPattern && !m_param->IsUsePinDirOut) {
		HTuple dirI;
		try {
			dirI = **p_dirIdx;
			//getDetectInData(InputDirIndex, dirI);
			dirIndex = dirI[0].I();
		}
		catch (HException& ex) {
			dirIndex = 0;
		}
		if (m_param->angleType == 0 && dirIndex == 1) {
			dirIndex = 2;
		}
		//采用多角度怎么判断模板;
		*p_outDirIdx = dirIndex;
		//setDetectOutData(OutPutIndex, dirIndex);
	}
	int angleVal[4] = { 0,90,180,270 };
	int Matchindex[30];  //字符在哪个模板匹配到的
	for (int i = 0;i < 30;i++) {
		Matchindex[i] = 0;
	}
	//double rotateangle=0;                                  //采用基准模式时的旋转角度

	double centrRow[MAX_MARK_NUM], centrCol[MAX_MARK_NUM], centrLen1[MAX_MARK_NUM], centrLen2[MAX_MARK_NUM];  //采用基准模式的时候需要切换至旋转矩1形
	HTuple dirHommat[MAX_MARK_NUM];  //存储极性旋转的矩阵
	double rotateangle = 0;
	for (int i = 0;i < m_param->PatternNum;i++) {
		if (m_param->IsMult&&m_param->IsInputPatIndex && (m_inputPatIndex != i)) {
			continue;
		}
		if (!m_param->MultPatternEnable[i] || (!m_param->IsMult&&i > 0)) {
			continue;
		}
		centrRow[i] = (m_param->RegionRoi[i].row1 + m_param->RegionRoi[i].row2) / 2.0;
		centrCol[i] = (m_param->RegionRoi[i].col1 + m_param->RegionRoi[i].col2) / 2.0;
		centrLen2[i] = (m_param->RegionRoi[i].row2 - m_param->RegionRoi[i].row1 + 1) / 2.0;
		centrLen1[i] = (m_param->RegionRoi[i].col2 - m_param->RegionRoi[i].col1 + 1) / 2.0;
		if (!m_param->IsUsePinDirOut) {
			if (dirIndex == 1 || dirIndex == 3) {
				double tmpval;
				tmpval = centrLen1[i];
				centrLen1[i] = centrLen2[i];
				centrLen2[i] = tmpval;
			}
			double rangle = PI*angleVal[dirIndex] / 180.0;
			vector_angle_to_rigid(centrRow[i], centrCol[i], 0.0, centrRow[i], centrCol[i], rangle, &dirHommat[i]);  //建立旋转关系矩阵
		}
	}

	if (m_param->IsReferSection > 0) {
		try {
			HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &m_homeMat2D);
			//getDetectInData(HomMat2D, m_homeMat2D);
			double tv = m_homeMat2D[0].D();
		}
		catch (HException& ex) {
			hom_mat2d_identity(&m_homeMat2D);
		}
		rotateangle = atan((m_homeMat2D[3].D() / m_homeMat2D[0].D()));
		setDetectOutData(TmpHomMat, m_homeMat2D);
	}
	if (m_param->errOffXValid || m_param->errOffYValid) {
		HTuple referrow, refercol;
		for (auto i = 0;i < p_corners->size();++i)
		{
			referrow[i] = p_corners->at(i).y;
			refercol[i] = p_corners->at(i).x;
		}
		//getDetectInData(ReferRow, referrow);
		//getDetectInData(ReferCol, refercol);
		try {
			for (int i = 0;i < 4;i++) {
				m_row[i] = referrow[i].D();
				m_col[i] = refercol[i].D();
			}
			m_row[4] = m_row[0];
			m_col[4] = m_col[0];
		}
		catch (HException& ex) {
			m_row[0] = m_col[0] = 50;
			m_row[1] = m_col[3] = 50;
			m_row[3] = m_col[1] = 150;
			m_row[2] = m_col[2] = 150;
			m_row[4] = m_row[0];
			m_col[4] = m_col[0];
		}
		HTuple tmprow, tmpcol;
		for (int i = 0;i < 5;i++) {
			tmprow[i] = m_row[i];
			tmpcol[i] = m_col[i];
		}
		setDetectOutData(TmpReferRow, tmprow);
		setDetectOutData(TmpReferCol, tmpcol);
	}
	int bestPatten = -1;
	int bestDir = -1; //用于存储在进行角度属性识别时得到的最佳的结果
	if (m_param->IsReferSection == 2) {  //注意在使用角度属性以及采用多图层多模板的时候的时候必须使用此参考类型
		bool matchOK = false;
		int matchscore = 0;
		double maxScore = 0;   //最大得分数
		for (int i = 0;i < m_param->PatternNum;i++) {
			if (m_param->IsMult&&m_param->IsInputPatIndex && (m_inputPatIndex != i)) {
				continue;  //传入模板指数时只使用对应的模板
			}
			if (!m_param->MultPatternEnable[i] || (!m_param->IsMult&&i > 0)) {
				continue;
			}
			for (int j = 0;j < 4;j++) {
				if (!m_param->IsUsePinDirOut&&j != dirIndex) {
					continue;
				}
				if (m_param->IsUsePinDirOut&&m_param->Is2Dor4D == 0 && (j == 1 || j == 3)) {
					continue;
				}
				Hobject roirec, tmpImg;
				double roiangle;
				roiangle = 0 + angleVal[j];
				double mlen1, mlen2;
				mlen1 = centrLen1[i];
				mlen2 = centrLen2[i];
				if (m_param->IsUsePinDirOut) {
					if (j == 1 || j == 3) {
						mlen2 = centrLen1[i];
						mlen1 = centrLen2[i];
					}
					//double rangle=PI*angleVal[dirIndex]/180.0;
					//vector_angle_to_rigid(centrRow[i],centrCol[i],0.0,centrRow[i],centrCol[i],rangle,&dirHommat[i]);  //建立旋转关系矩阵
				}
				gen_rectangle2(&roirec, centrRow[i], centrCol[i], 0, mlen1, mlen2);
				affine_trans_region(roirec, &roirec, m_homeMat2D, "false");
				reduce_domain(*m_image, roirec, &tmpImg); //
				HTuple matchRow, matchCol, matchAngle, matchScore;
				if (m_param->searchAngleValid) {
					find_ncc_model(tmpImg, m_RegionModelId[i][j], (-(double)m_param->searchAngle / 180.0)*M_PI, (2.0*(double)m_param->searchAngle / 180.0)*M_PI, 0.35/*double(m_param->acceptSimilar/100.0)/2.0*/, 1, 0, "true", 0,
						&matchRow, &matchCol, &matchAngle, &matchScore);
				}
				else {
					find_ncc_model(tmpImg, m_RegionModelId[i][dirIndex], -0.20, 0.39, 0.35/*double(m_param->acceptSimilar/100.0)/2.0*/, 1, 0, "true", 0,
						&matchRow, &matchCol, &matchAngle, &matchScore);
				}
				int count;
				count = matchScore.Num();
				if (count > 0)
				{
					double tmpscore = matchScore[0].D() * 100;
					if (tmpscore > maxScore) {
						maxScore = tmpscore;
						bestDir = j;
						bestPatten = i;
						if (tmpscore >= m_param->acceptSimilar / 2.0) {    //整体字符识别的是否需要的分数比给的单字符接受值低10
							vector_angle_to_rigid(centrRow[i], centrCol[i], 0,
								matchRow[0].D(), matchCol[0].D(), matchAngle[0].D(), &m_homeMat2D);
							matchscore = tmpscore;
							matchOK = true;
						}
					}

				}
			}//end of loop direction j
		}   //end of loop pattern i
		int outputIndex = 0;  //
		if (m_param->IsUsePinDirOut) {
			if (matchOK == true) {
				dirIndex = bestDir;
				outputIndex = bestDir;
				if (m_param->Is2Dor4D == 0 && bestDir == 2) {
					outputIndex = 1;    //跟以前的其他模块的传出指数统一，两个方向的是0/1，四个方向的是0/1/2/3
				}
			}
			else {
				outputIndex = 0;
			}
			if (m_param->Is2Dor4D == 0) {
				if (outputIndex == 0) {
					isLF = !(1 ^ !m_param->IsInverse);
					isRT = !(0 ^ !m_param->IsInverse);
				}
				else if (outputIndex == 1) {
					isLF = !(0 ^ !m_param->IsInverse);
					isRT = !(1 ^ !m_param->IsInverse);
				}
			}
			else if (m_param->Is2Dor4D == 1) {
				if (outputIndex == 0) {
					isLF = 0;
					isRT = 0;
				}
				else if (outputIndex == 1) {
					isLF = 0;
					isRT = 1;
				}
				else if (outputIndex == 2) {
					isLF = 1;
					isRT = 0;
				}
				else if (outputIndex == 3) {
					isLF = 1;
					isRT = 1;
				}
			}
		}
		*p_outDirIdx = outputIndex;
		*p_lf = isLF;
		*p_rt = isRT;
		triggerEvent(ENT_WRITE_IO);

		//setDetectOutData(OutPutIndex, outputIndex);
		//setDetectOutData(LFOK, isLF);
		//setDetectOutData(RTOK, isRT);
		//setDetectOutData(PINOutEnable, m_param->IsUsePinDirOut);
		//setDetectOutData(PIN1OUT, m_param->pin1OutNum);
		//setDetectOutData(PIN2OUT, m_param->pin2OutNum);
		setDetectOutData(RegionMatchOk, matchOK);
		setDetectOutData(RegionMatchScore, matchscore);
		if (matchOK == true) {
			setDetectOutData(TmpHomMat, m_homeMat2D);
			rotateangle = atan((m_homeMat2D[3].D() / m_homeMat2D[0].D()));
			if (m_param->IsUsePinDirOut) {
				for (int i = 0;i < m_param->PatternNum;i++) {
					if (m_param->IsMult&&m_param->IsInputPatIndex && (m_inputPatIndex != i)) {
						continue;  //传入模板指数时只使用对应的模板
					}
					if (!m_param->MultPatternEnable[i] || (!m_param->IsMult&&i > 0)) {
						continue;
					}
					if (dirIndex == 1 || dirIndex == 3) {
						double tmpval;
						tmpval = centrLen1[i];
						centrLen1[i] = centrLen2[i];
						centrLen2[i] = tmpval;
					}
					double rangle = PI*angleVal[dirIndex] / 180.0;
					vector_angle_to_rigid(centrRow[i], centrCol[i], 0.0, centrRow[i], centrCol[i], rangle, &dirHommat[i]);  //建立旋转关系矩阵
				}
			}
		}
		else {
			allok = false;
			setDetectOutData(AllOk, allok);
			//gen_empty_region(m_charRegion);
			*p_charReg = RlRegion();
			//setDetectOutData(CharRegion, (Hlong)m_charRegion);
			m_testItemsStatus[0] = 0;
			return -1;
			//return allok;
		}
	}  //end of refer==2
	HTuple charRegionRow, charRegionCol, charRegionAngle, charLength1, charLength2;
	Hobject singleUnion, allUnion;
	bool firstok = false;

	int endPatIndex; //最后一个可用模组的指数
	for (int i = 0;i < m_param->PatternNum;i++) {
		if (!m_param->IsMult) {
			endPatIndex = 0;
		}
		else {
			if (m_param->MultPatternEnable[i]) {
				endPatIndex = i;
			}
		}
	}
	if (m_param->IsMult&&m_param->IsInputPatIndex) {
		endPatIndex = m_inputPatIndex;
	}
	if (m_param->IsMult && m_param->IsFontOrLayer == 1) {
		endPatIndex = bestPatten;
	}

	setDetectOutData(LayerMatchIndex, bestPatten);

	int TmpCharCount;
	if (m_param->IsMult && m_param->IsFontOrLayer == 1) {
		TmpCharCount = m_param->charCounts[bestPatten];  //只有在多图层模式才会有不同字体数目的情况
	}
	else {
		TmpCharCount = m_param->charCounts[0];
	}
	int backGray[MAX_MARK_NUM];
	//for (int i=0;i<MAX_MARK_NUM;i++){
	//	backGray[i]=50;
	//	if (m_param->MultPatternEnable[i]){
	//		Hobject patD;
	//		get_domain(m_param->SegImg,&patD);
	//		HTuple absHisto,relativeHisto;

	//		gray_histo(patD,patimg,&absHisto,&relativeHisto);
	//		HTuple func,smoothFunc;
	//		create_funct_1d_array(absHisto,&func);
	//		smooth_funct_1d_gauss(func,5,&smoothFunc);
	//		HTuple XVal,YVal;
	//		funct_1d_to_pairs(smoothFunc,&XVal,&YVal);
	//		HTuple Indices;
	//		tuple_sort_index(YVal,&Indices);
	//		HTuple Sel;
	//		tuple_select(Indices,255,&Sel);
	//		int dddd=Sel[0].I();
	//		int ssd=Indices.Num();

	//		Hobject segroi,compareSeg,meanPat;
	//		gen_image_proto(patimg,&meanPat,Sel[0].I());
	//	}

	//}

	for (int i = 0;i < TmpCharCount;i++) {
		bool thisfoundok = true;   //用于字符是否达标相似度
		bool chardetectOk = true;   //字符检测各项目是否ok
		HTuple matchRow, matchCol, matchAngle, matchScore;
		int num;

		for (int j = 0;j < m_param->PatternNum;j++) {
			if (m_param->IsMult&&m_param->IsInputPatIndex && (m_inputPatIndex != j)) {
				continue;  //传入模板指数时只使用对应的模板
			}
			if (m_param->IsMult&&m_param->IsFontOrLayer == 1 && (bestPatten != j)) {
				continue;  //多图层时只使用对应的模板
			}
			if (!m_param->MultPatternEnable[j] || (!m_param->IsMult&&j > 0)) {
				continue;
			}
			Hobject tmpImg;
			Hobject roirec;
			double charrow, charcol, charlength1, charlength2;  //检测区域的roi
			charrow = (m_param->segRegionVector[j][i].row1 + m_param->segRegionVector[j][i].row2) / 2;
			charcol = (m_param->segRegionVector[j][i].col1 + m_param->segRegionVector[j][i].col2) / 2;
			charlength1 = (m_param->segRegionVector[j][i].col2 - m_param->segRegionVector[j][i].col1 + 1) / 2 + m_param->charOffset;
			charlength2 = (m_param->segRegionVector[j][i].row2 - m_param->segRegionVector[j][i].row1 + 1) / 2 + m_param->charOffset;
			if (dirIndex > 0) {
				affine_trans_pixel(dirHommat[j], charrow, charcol, &charrow, &charcol);  //进行极性旋转
			}
			if (!m_param->charOffsetValid) {   //在搜寻roi内检1测
				gen_rectangle2(&roirec, centrRow[j], centrCol[j], 0, centrLen1[j], centrLen2[j]);
			}
			else if (m_param->charOffsetValid) { //在印字区域内检1测
				gen_rectangle2(&roirec, charrow, charcol, angleVal[dirIndex] / 180.0*PI, charlength1, charlength2);
			}
			if (m_param->IsReferSection) {
				affine_trans_region(roirec, &roirec, m_homeMat2D, "false");  //根据整体定位信息进行偏移
			}
			reduce_domain(*m_image, roirec, &tmpImg); //
			if (m_param->MultPatternEnable[j]) {
				if (m_param->searchAngleValid) {
					find_ncc_model(tmpImg, m_modelId[j][dirIndex][i], (-(double)m_param->searchAngle / 180.0)*M_PI + rotateangle, (2.0*(double)m_param->searchAngle / 180.0)*M_PI, double(m_param->acceptSimilar / 100.0) / 2.0, 1, 0, "true", 0,
						&matchRow, &matchCol, &matchAngle, &matchScore);
				}
				else {
					find_ncc_model(tmpImg, m_modelId[j][dirIndex][i], -0.39, 0.79, double(m_param->acceptSimilar / 100.0) / 2.0, 1, 0, "true", 0,
						&matchRow, &matchCol, &matchAngle, &matchScore);
				}
				int count;
				count = matchScore.Num();
				if (count > 0)
				{
					float tmpscore = matchScore[0].D() * 100;
					if (tmpscore >= m_param->acceptSimilar) {
						double rangle = PI*angleVal[dirIndex] / 180.0;
						Hobject patimg;

						crop_rectangle1(m_param->SegImg[j], &patimg, m_param->segRegionVector[j][i].row1 - m_param->RegionRoi[j].row1,
							m_param->segRegionVector[j][i].col1 - m_param->RegionRoi[j].col1,
							m_param->segRegionVector[j][i].row2 - m_param->RegionRoi[j].row1,
							m_param->segRegionVector[j][i].col2 - m_param->RegionRoi[j].col1);
						Hobject segroi, compareSeg;
						threshold(patimg, &segroi, m_param->charThreMin, m_param->charThreMax < m_param->charThreMin ? m_param->charThreMin : m_param->charThreMax);
						//closing_rectangle1(segroi,&segroi,3,3);
						//opening_rectangle1(segroi,&segroi,3,3);

						//Hobject patD;
						//get_domain(patimg,&patD);
						//HTuple absHisto,relativeHisto;

						//gray_histo(patD,patimg,&absHisto,&relativeHisto);
						//HTuple func,smoothFunc;
						//create_funct_1d_array(absHisto,&func);
						//smooth_funct_1d_gauss(func,5,&smoothFunc);
						//HTuple XVal,YVal;
						//funct_1d_to_pairs(smoothFunc,&XVal,&YVal);
						//HTuple Indices;
						//tuple_sort_index(YVal,&Indices);
						//HTuple Sel;
						//tuple_select(Indices,255,&Sel);
						//int dddd=Sel[0].I();
						//int ssd=Indices.Num();

						//Hobject segroi,compareSeg,meanPat;
						//gen_image_proto(patimg,&meanPat,Sel[0].I());
						//if(m_param->whiteChar){
						//	dyn_threshold(patimg, meanPat, &segroi, m_param->charThreMin, "light");
						//}
						//else{
						//	dyn_threshold(patimg, meanPat, &segroi, m_param->charThreMin, "dark");
						//}
						threshold(patimg, &segroi, m_param->charThreMin, m_param->charThreMax < m_param->charThreMin ? m_param->charThreMin : m_param->charThreMax);

						if (m_param->CharDefectTOL > 0) {
							closing_rectangle1(segroi, &segroi, m_param->CharDefectTOL, m_param->CharDefectTOL);
						}

						connection(segroi, &segroi);
						select_shape(segroi, &segroi, "area", "and", m_param->minArea, 10000000);
						HTuple recmat;

						vector_angle_to_rigid(
							(m_param->segRegionVector[j][i].row2 - m_param->segRegionVector[j][i].row1) / 2.0,
							(m_param->segRegionVector[j][i].col2 - m_param->segRegionVector[j][i].col1) / 2.0, 0,
							(m_param->segRegionVector[j][i].row2 + m_param->segRegionVector[j][i].row1) / 2.0,
							(m_param->segRegionVector[j][i].col2 + m_param->segRegionVector[j][i].col1) / 2.0, 0,
							&recmat);

						affine_trans_region(segroi, &segroi, recmat, "true");   //将模板切割出来的区域从左上角的零点位置还原为实际模板所在的原图位置
						if (dirIndex > 0) {
							affine_trans_region(segroi, &segroi, dirHommat[j], "true");
						}
						HTuple hommat;
						vector_angle_to_rigid(charrow, charcol, 0,
							matchRow[0].D(), matchCol[0].D(), matchAngle[0].D(), &hommat);
						affine_trans_region(segroi, &segroi, hommat, "true");     //根据匹配出来的位置，对模板位置再次进行旋转平移
						union1(segroi, &compareSeg);
						if (m_param->IsFillUp) {
							fill_up(segroi, &segroi);
						}
						if (firstok == false) {
							union1(segroi, &allUnion);
						}
						else {
							union1(segroi, &singleUnion);
							union2(singleUnion, allUnion, &allUnion);
						}
						firstok = true;
						if (m_param->errOffSetValid) {
							bool offok = true;
							double crow, ccol;   //字符模板所在的中心位置
							crow = charrow;
							ccol = charcol;
							if (m_param->IsReferSection) {
								affine_trans_pixel(m_homeMat2D, crow, ccol, &crow, &ccol);
							}
							double coffset;
							distance_pp(matchRow[0].D(), matchCol[0].D(), crow, ccol, &coffset);
							if (int(abs(coffset) + 0.5) > m_param->errOffSet) {
								offok = false;
								chardetectOk = false;
								m_testItemsStatus[3] = 0;
								allok = false;
							}
							setDetectOutData(OffSetOk, offok, i);
							setDetectOutData(OffSetval, int(abs(coffset) + 0.5), i);
						}
						if (m_param->errOffXValid || m_param->errOffYValid) {
							Hobject interrect;
							double interlen1, interlen2;
							interlen1 = (m_param->segRegionVector[j][i].col2 - m_param->segRegionVector[j][i].col1) / 2.0;
							interlen2 = (m_param->segRegionVector[j][i].row2 - m_param->segRegionVector[j][i].row1) / 2.0;
							gen_rectangle2(&interrect, matchRow[0].D(), matchCol[0].D(), matchAngle[0].D(), interlen1, interlen2);
							//遍历区域到四个边的距离，上右下左
							HTuple tmpXMax, tmpXMin;
							double disXMin = 100000;
							int minXIndex = -1;
							bool offxok = true;
							if (m_param->errOffXValid) {
								for (int e = 0;e < 2;e++) {
									distance_lr(interrect, m_row[e * 2 + 1], m_col[e * 2 + 1], m_row[e * 2 + 1 + 1], m_col[e * 2 + 1 + 1], &tmpXMin, &tmpXMax);
									if (disXMin > tmpXMin[0].D()) {
										disXMin = tmpXMin[0].D();
										minXIndex = e;
									}
								}
								if (disXMin > m_param->errOffXMax || disXMin < m_param->errOffXMin)
								{
									offxok = false;
									chardetectOk = false;
									m_testItemsStatus[7] = 0;
									allok = false;
								}
								setDetectOutData(OffXOk, offxok, i);
								setDetectOutData(OffXval, disXMin, i);
							}
							HTuple tmpYMax, tmpYMin;
							double disYMin = 100000;
							int minYIndex = -1;
							bool offyok = true;
							if (m_param->errOffYValid) {
								for (int e = 0;e < 2;e++) {
									distance_lr(interrect, m_row[e * 2], m_col[e * 2], m_row[e * 2 + 1], m_col[e * 2 + 1], &tmpYMin, &tmpYMax);
									if (disYMin > tmpYMin[0].D()) {
										disYMin = tmpYMin[0].D();
										minYIndex = e;
									}
								}
								if (disYMin > m_param->errOffYMax || disYMin < m_param->errOffYMin)
								{
									offyok = false;
									chardetectOk = false;
									m_testItemsStatus[8] = 0;
									allok = false;
								}
								setDetectOutData(OffYOk, offyok, i);
								setDetectOutData(OffYval, disYMin, i);
							}
						}

						//检测缺陷部分
						if (m_param->errAreaValid || m_param->errExcessValid || m_param->errCrackValid) {
							double clen1, clen2;
							clen1 = (m_param->segRegionVector[j][i].col2 - m_param->segRegionVector[j][i].col1 + 1) / 2.0;
							clen2 = (m_param->segRegionVector[j][i].row2 - m_param->segRegionVector[j][i].row1 + 1) / 2.0;

							Hobject defectROI, defectReg;
							gen_rectangle2(&defectROI, charrow, charcol, angleVal[dirIndex] / 180.0*PI, clen1, clen2);
							affine_trans_region(defectROI, &defectROI, hommat, "false");  //根据整体定位信息进行偏移
							reduce_domain(*m_image, defectROI, &defectReg);
							Hobject defectReg1;
							if (m_param->errAreaValid) {
								threshold(defectReg, &defectReg1, m_param->charThreMin, m_param->charThreMax < m_param->charThreMin ? m_param->charThreMin : m_param->charThreMax);
							}

							//HTuple absHisto1,relativeHisto1;

							//gray_histo(defectROI,*m_image,&absHisto1,&relativeHisto1);
							//
							//HTuple func1,smoothFunc1;
							//create_funct_1d_array(absHisto1,&func1);
							//smooth_funct_1d_gauss(func1,5,&smoothFunc1);
							//HTuple XVal1,YVal1;
							//funct_1d_to_pairs(smoothFunc1,&XVal1,&YVal1);
							//HTuple Indices1;
							//tuple_sort_index(YVal1,&Indices1);
							//HTuple Sel1;
							//tuple_select(Indices1,255,&Sel1);
							//int dddd1=Sel1[0].I();
							//int ssd1=Indices1.Num();

							//Hobject meanImg;
							//gen_image_proto(defectReg,&meanImg,Sel1[0].I());
							//reduce_domain(meanImg,defectROI,&meanImg);
							//mean_image(defectReg,&meanImg,(clen1+clen2)/2.0,(clen1+clen2)/2.0);
							if (m_param->whiteChar) {
								//threshold(defectReg,&defectReg,m_param->threshold,255);
								//dyn_threshold(defectReg, meanImg, &defectReg, m_param->charThreMin, "light");
							}
							else {
								//threshold(defectReg,&defectReg,0,m_param->threshold);
								//dyn_threshold(defectReg, meanImg, &defectReg, m_param->charThreMin, "dark");
								//threshold(defectReg,&defectReg,m_param->charThreMin,m_param->charThreMax);
							}
							threshold(defectReg, &defectReg, m_param->charThreMin, m_param->charThreMax < m_param->charThreMin ? m_param->charThreMin : m_param->charThreMax);
							Hobject defectRegCon, arearegion, compareU;
							if (m_param->errAreaValid || m_param->errExcessValid || m_param->errCrackValid) {
								connection(defectReg, &defectRegCon);
								select_shape(defectRegCon, &defectRegCon, "area", "and", m_param->minArea, 100000);
								union1(defectRegCon, &arearegion);
								union1(compareSeg, &compareU); //标准区域
								HTuple earea1, earea2, erow, ecol;
								area_center(arearegion, &earea1, &erow, &ecol);
								area_center(compareU, &earea2, &erow, &ecol);
								bool empok = true;
								if (earea1.Num() == 0 || earea2.Num() == 0) {
									Hobject emp;
									gen_empty_region(&emp);
									setDetectOutObject(AreaRegionDisp, emp, i);
									if (m_param->errAreaValid) {
										setDetectOutData(AreaDiffVal, 1 * (100.0), i);
										setDetectOutData(AreaDiffOk, false, i);
										m_testItemsStatus[6] = 0;
									}
									if (m_param->errCrackValid) {
										setDetectOutObject(CrackAreaDisp, emp, i);
										setDetectOutData(CrackVal, 1 * (100.0), i);
										setDetectOutData(CrackOk, false, i);
										m_testItemsStatus[9] = 0;
									}
									if (m_param->errExcessValid) {
										setDetectOutObject(ExcessAreaDisp, emp, i);
										setDetectOutData(ExcessVal, 1 * (100.0), i);
										setDetectOutData(ExcessOk, false, i);
										m_testItemsStatus[10] = 0;
									}
									chardetectOk = false;
									allok = false;
								}
								else {
									Hobject areamore, arealess;
									Hobject areamoreU, arealessU;
									int areaCount = 0;  //多了或少了的面积

									if (m_param->areaType <= 1 || m_param->errExcessValid) {  //多余的部分
										difference(arearegion, compareSeg, &areamore);
										opening_circle(areamore, &areamore, 1);
										connection(areamore, &areamore);
										select_shape(areamore, &areamore, "area", "and", 5, 1000000);
										union1(areamore, &areamoreU);
										HTuple moreval, morerow, morecol;
										area_center(areamore, &moreval, &morerow, &morecol);
										Hlong morenum;
										count_obj(areamore, &morenum);
										if (morenum > 0) {
											areaCount += moreval[0].I();
										}
										if (m_param->errExcessValid) {
											bool excessok = true;
											double rmin;
											rmin = 0.5*m_param->errExcess / mpixelsize + 0.5;
											select_shape(areamore, &areamore, "outer_radius", "and", (int)rmin, 99999);
											HTuple rrow, rcol, rr;
											smallest_circle(areamore, &rrow, &rcol, &rr);
											double maxExcess = 0;
											if (rr.Num() != 0) {
												tuple_sort(rr, &rr);
												maxExcess = rr[rr.Num() - 1].D()*2.0;
												if (maxExcess > m_param->errExcess) {
													excessok = false;
													m_testItemsStatus[10] = 0;
													chardetectOk = false;
													allok = false;
												}
											}
											setDetectOutObject(ExcessAreaDisp, areamore, i);
											setDetectOutData(ExcessVal, maxExcess*mpixelsize, i);
											setDetectOutData(ExcessOk, excessok, i);
										}
									}
									if (m_param->areaType == 0 || m_param->areaType == 2 || m_param->errCrackValid) {
										difference(compareSeg, arearegion, &arealess);
										opening_circle(arealess, &arealess, 1);
										connection(arealess, &arealess);
										select_shape(arealess, &arealess, "area", "and", 5, 1000000);
										union1(arealess, &arealessU);
										HTuple lessval, lessrow, lesscol;
										area_center(arealess, &lessval, &lessrow, &lesscol);
										Hlong lessnum;
										count_obj(arealess, &lessnum);
										if (lessnum > 0) {
											areaCount += lessval[0].I();
										}
										if (m_param->errCrackValid) {
											bool crackok = true;
											double rmin;
											rmin = 0.5*m_param->errCrack / mpixelsize + 0.5;
											select_shape(arealess, &arealess, "outer_radius", "and", (int)rmin, 99999);
											HTuple rrow, rcol, rr;
											smallest_circle(arealess, &rrow, &rcol, &rr);
											double maxCrack = 0;
											if (rr.Num() != 0) {
												tuple_sort(rr, &rr);
												maxCrack = rr[rr.Num() - 1].D()*2.0;
												if (maxCrack > m_param->errCrack) {
													crackok = false;
													m_testItemsStatus[9] = 0;
													chardetectOk = false;
													allok = false;
												}
											}
											setDetectOutObject(CrackAreaDisp, arealess, i);
											setDetectOutData(CrackVal, maxCrack*mpixelsize, i);
											setDetectOutData(CrackOk, crackok, i);
										}
									}
									union2(arealessU, areamoreU, &areamoreU);
									setDetectOutObject(AreaRegionDisp, areamoreU, i);  //三种情况均要显示，作为背景显示
									if (m_param->errAreaValid) {
										HTuple careaval, carearow, careacol;
										area_center(compareSeg, &careaval, &carearow, &careacol);
										int referareaval = careaval[0].I();
										double thisp = (double)areaCount / (double)referareaval;
										bool areadok = true;
										if (thisp*(100.0) > m_param->errAreaPercent) {
											areadok = false;
											m_testItemsStatus[6] = 0;
											chardetectOk = false;
											allok = false;
										}
										setDetectOutData(AreaDiffVal, thisp*(100.0), i);
										setDetectOutData(AreaDiffOk, areadok, i);
									}
								}
							}
							if (m_param->CharDefectTOL > 0) {
								closing_rectangle1(defectReg, &defectReg, m_param->CharDefectTOL, m_param->CharDefectTOL);
							}
							connection(defectReg, &defectReg);

							select_shape(defectReg, &defectReg, "area", "and", m_param->minArea, 100000);
							if (m_param->minHoles > 0) {
								fill_up_shape(defectReg, &defectReg, "area", 0, m_param->minHoles);
							}
							HTuple regionCount;
							count_obj(defectReg, &regionCount);
						}
						Matchindex[i] = j;
						break;
					}   //end of if (tmpscore>=m_param->acceptSimilar)
					else if (tmpscore < m_param->acceptSimilar) {
						if (j == endPatIndex) {
							Matchindex[i] = j;
						}
					}
				}
			}
			if (!m_param->IsMult&&i == 0) {
				break;
			}
		}  //end of loop j
		num = matchScore.Num();
		if (num == 0) {
			thisfoundok = false;
			allok = false;
			setDetectOutData(CharFoundOk, thisfoundok, i);
			setDetectOutData(CharDetectOK, false, i);
			setDetectOutData(MatchScore, 0, i);
			setDetectOutData(MatchIndex, 0, i);
			m_testItemsStatus[0] = 0;
		}
		else if (num > 0) {
			float tmpscore = matchScore[0].D() * 100;
			float tmpAngle;
			if (m_param->IsReferTempAngle) {
				tmpAngle = ((matchAngle[0].D() - rotateangle) / PI) * 180;
			}
			else {
				tmpAngle = ((matchAngle[0].D()) / PI) * 180;
			}

			if (tmpscore < m_param->acceptSimilar) {
				thisfoundok = false;
				allok = false;
				m_testItemsStatus[1] = 0;
				if (!m_param->IsMult) {
					Matchindex[i] = 0;
				}
			}
			if (m_param->errAngleValid) {
				if (abs(tmpAngle) > m_param->errAngle) {
					chardetectOk = false;
					allok = false;
					m_testItemsStatus[2] = 0;
				}
			}
			setDetectOutData(CharFoundOk, thisfoundok, i);
			setDetectOutData(CharDetectOK, chardetectOk, i);
			setDetectOutData(MatchRow, matchRow[0].D(), i);
			setDetectOutData(MatchCol, matchCol[0].D(), i);
			if (m_param->IsReferTempAngle) {
				setDetectOutData(MatchAngle, matchAngle[0].D() - rotateangle, i);
			}
			else {
				setDetectOutData(MatchAngle, matchAngle[0].D(), i);
			}

			setDetectOutData(MatchScore, matchScore[0].D(), i);
			setDetectOutData(MatchIndex, Matchindex[i], i);
		}
	}  //end of loop i
	if (allok) {
		copy_obj(allUnion, m_charRegion, 1, 1);
		HalOpenCV::hal2cvRegion(*m_charRegion, &(*p_charReg));
		//setDetectOutData(CharRegion, (Hlong)m_charRegion);
	}
	else {
		*p_charReg = RlRegion();
		//gen_empty_region(m_charRegion);		
		//setDetectOutData(CharRegion, (Hlong)m_charRegion);
	}
	setDetectOutData(AllOk, allok);
	return allok ? 0 : -1;
}

void mark1Mod::viewResult(ImageView *iv, const QString &funName, int)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	if (m_param->IsDefaultSig&&m_param->IsUsePinDirOut) {
		iv->setColor(0, 255, 255);
		if (m_param->isShowAngle) {
			if (m_param->IsInverse) {
				iv->dispText(400, 50, "Angle-180");
			}
			else {
				iv->dispText(400, 50, "Angle-0");
			}
		}
		return;
	}
	if (m_param->OcrValid && m_producting) {  //生产过程中启用OCR自动模板功能
		if (!m_ocrTempOK)
			return;
	}
	if (!m_PatternEnable || m_charRight || !m_dirSame)
		return;
	HTuple regionmatchok;
	if (m_param->IsReferSection == 2) {
		getDispOutData(RegionMatchOk, regionmatchok);
		if (!regionmatchok[0].I()) {
			return;
		}
	}
	int dirIndex = 0;
	if (m_param->IsDirPattern) {
		HTuple diri;
		diri = **p_outDirIdx;
		//getDispOutData(OutPutIndex, diri);
		dirIndex = diri[0].I();
		if (m_param->angleType == 0 && dirIndex == 1 && m_param->IsUsePinDirOut) {   //统一指数
			dirIndex = 2;
		}
	}
	if (m_param->IsReferSection == 2) {
		if (regionmatchok[0].I() && m_param->IsUsePinDirOut) {
			iv->setColor(0, 255, 255);
			if (m_param->isShowAngle) {
				if (dirIndex == 0)
					iv->dispText(400, 50, "Angle-0");
				else if (dirIndex == 1)
					iv->dispText(400, 50, "Angle-90");
				else if (dirIndex == 2)
					iv->dispText(400, 50, "Angle-180");
				else if (dirIndex == 3)
					iv->dispText(400, 50, "Angle-270");
			}
		}
	}
	double centrRow[MAX_MARK_NUM], centrCol[MAX_MARK_NUM], centrLen1[MAX_MARK_NUM], centrLen2[MAX_MARK_NUM];  //采用基准模式的时候需要切换至旋转矩1形
	HTuple dirHommat[MAX_MARK_NUM];  //存储极性旋转的矩阵
	int angleVal[4] = { 0,90,180,270 };
	for (int i = 0;i < m_param->PatternNum;i++) {
		if (!m_param->MultPatternEnable[i] || (!m_param->IsMult&&i > 0)) {
			continue;
		}
		centrRow[i] = (m_param->RegionRoi[i].row1 + m_param->RegionRoi[i].row2) / 2.0;
		centrCol[i] = (m_param->RegionRoi[i].col1 + m_param->RegionRoi[i].col2) / 2.0;
		centrLen2[i] = (m_param->RegionRoi[i].row2 - m_param->RegionRoi[i].row1 + 1) / 2.0;
		centrLen1[i] = (m_param->RegionRoi[i].col2 - m_param->RegionRoi[i].col1 + 1) / 2.0;
		if (dirIndex == 1 || dirIndex == 3) {
			double tmpval;
			tmpval = centrLen1[i];
			centrLen1[i] = centrLen2[i];
			centrLen2[i] = tmpval;
		}
		double rangle = PI*angleVal[dirIndex] / 180.0;
		vector_angle_to_rigid(centrRow[i], centrCol[i], 0.0, centrRow[i], centrCol[i], rangle, &dirHommat[i]);  //建立旋转关系矩阵
	}
	HTuple allok;
	getDispOutData(AllOk, allok);
	iv->setFilled(false);
	if (allok[0].I() == 0) {
		iv->setColor(255, 0, 0);
	}
	else {
		iv->setColor(0, 255, 0);
	}
	double rotateangle = 0;
	HTuple homeMat2D;
	if (m_param->IsReferSection) {
		getDispOutData(TmpHomMat, homeMat2D);
		rotateangle = atan((homeMat2D[3].D() / homeMat2D[0].D()));
	}
	else {
		hom_mat2d_identity(&homeMat2D);
	}
	if (allok[0].I() == 0) {
		if (m_param->IsReferSection == 0) {
			iv->dispRect2(centrRow[0], centrCol[0], 0, centrLen1[0], centrLen2[0], "search Roi");
		}
		else {
			affine_trans_pixel(homeMat2D, centrRow[0], centrCol[0], &centrRow[0], &centrCol[0]);
			iv->dispRect2(centrRow[0], centrCol[0], rotateangle, centrLen1[0], centrLen2[0], "search Roi");
		}
	}
	HTuple layIndex;
	int layerIndex;
	if (m_param->IsMult&&m_param->IsFontOrLayer == 1) {
		getDispOutData(LayerMatchIndex, layIndex);
		layerIndex = layIndex[0].I();
	}
	int tmpCharCounts;
	if (m_param->IsMult&&m_param->IsFontOrLayer == 1) {
		tmpCharCounts = m_param->charCounts[layerIndex];
	}
	else {
		tmpCharCounts = m_param->charCounts[0];
	}

	for (int i = 0;i < tmpCharCounts;i++) {
		HTuple thisok;
		getDispOutData(CharFoundOk, i, thisok);
		char buf[32];
		sprintf(buf, "%d", i + 1);
		HTuple matchindex;
		getDispOutData(MatchIndex, i, matchindex);
		int mIndex = matchindex[0].I();
		double recrow, reccol, reclen1, reclen2;
		recrow = (m_param->segRegionVector[mIndex][i].row1 + m_param->segRegionVector[mIndex][i].row2) / 2.0;
		reccol = (m_param->segRegionVector[mIndex][i].col1 + m_param->segRegionVector[mIndex][i].col2) / 2.0;
		reclen1 = (m_param->segRegionVector[mIndex][i].col2 - m_param->segRegionVector[mIndex][i].col1 + 1) / 2.0;
		reclen2 = (m_param->segRegionVector[mIndex][i].row2 - m_param->segRegionVector[mIndex][i].row1 + 1) / 2.0;
		if (dirIndex == 1 || dirIndex == 3) {
			double tmplen = reclen1;
			reclen1 = reclen2;
			reclen2 = tmplen;
		}
		if (thisok[0].I() == 1) {
			if (m_param->errAreaValid || m_param->errCrackValid || m_param->errExcessValid) {
				iv->setColor(0, 255, 0);
				if (m_param->errAreaValid) {
					HTuple areaok;
					getDispOutData(AreaDiffOk, i, areaok);
					if (areaok[0].I() == 0) {
						iv->setColor(255, 0, 0);
					}
				}
				iv->setFilled(false);
				const Hobject *tmpReg = getDispObject(AreaRegionDisp, i);
				Hobject reg;
				union1(*tmpReg, &reg);
				RlRegion dispReg;
				HalOpenCV::hal2cvRegion(reg, &dispReg);
				iv->dispRegion(&dispReg);
				if (m_param->errCrackValid) {
					HTuple crackok;
					getDispOutData(CrackOk, i, crackok);
					if (crackok[0].I() == 0) {
						iv->setColor(255, 0, 0);
					}
					else {
						iv->setColor(0, 255, 0);
					}
					iv->setFilled(false);
					const Hobject *t = getDispObject(CrackAreaDisp, i);
					Hobject tt;
					union1(*t, &tt);
					RlRegion r;
					HalOpenCV::hal2cvRegion(tt, &r);
					iv->dispRegion(&r);
				}
				if (m_param->errExcessValid) {
					HTuple excessok;
					getDispOutData(ExcessOk, i, excessok);
					if (excessok[0].I() == 0) {
						iv->setColor(255, 0, 0);
					}
					else {
						iv->setColor(0, 255, 0);
					}
					iv->setFilled(false);
					const Hobject *t = getDispObject(ExcessAreaDisp, i);
					Hobject tt;
					union1(*t, &tt);
					RlRegion r;
					HalOpenCV::hal2cvRegion(tt, &r);
					iv->dispRegion(&r);
				}
			}
		}
		if (m_param->charOffsetValid) {
			if (thisok[0].I() == 0) {
				iv->setColor(255, 0, 0);
				HTuple matchscore;
				getDispOutData(MatchScore, i, matchscore);
				double mScore = matchscore[0].D();
				affine_trans_pixel(dirHommat[mIndex], recrow, reccol, &recrow, &reccol);
				affine_trans_pixel(homeMat2D, recrow, reccol, &recrow, &reccol);
				if (matchscore[0].D() <= 0.2) {
					iv->dispRect2(recrow, reccol, rotateangle, reclen1 + m_param->charOffset, reclen2 + m_param->charOffset, buf);
				}
				else if (matchscore[0].D() > 0.2) {
					HTuple matchrow, matchcol, matchangle;
					getDispOutData(MatchRow, i, matchrow);
					getDispOutData(MatchCol, i, matchcol);
					getDispOutData(MatchAngle, i, matchangle);
					iv->dispRect2(matchrow[0].D(), matchcol[0].D(), matchangle[0].D() + rotateangle, reclen1, reclen2, buf);
				}
			}
			else if (thisok[0].I() == 1) {
				HTuple matchrow, matchcol, matchangle;
				getDispOutData(MatchRow, i, matchrow);
				getDispOutData(MatchCol, i, matchcol);
				getDispOutData(MatchAngle, i, matchangle);
				HTuple detectok;
				getDispOutData(CharDetectOK, i, detectok);
				if (detectok[0].I() == 0) {
					iv->setColor(255, 0, 0);
				}
				else {
					iv->setColor(0, 255, 0);
				}
				iv->dispRect2(matchrow[0].D(), matchcol[0].D(), matchangle[0].D() + rotateangle, reclen1,
					reclen2, buf);
			}
		}
		else if (!m_param->charOffsetValid) {
			if (thisok[0].I() == 0) {
				iv->setColor(255, 0, 0);
				HTuple matchscore;
				getDispOutData(MatchScore, i, matchscore);
				if (matchscore[0].D() > 0.2) {
					HTuple matchrow, matchcol, matchangle;
					getDispOutData(MatchRow, i, matchrow);
					getDispOutData(MatchCol, i, matchcol);
					getDispOutData(MatchAngle, i, matchangle);
					iv->dispRect2(matchrow[0].D(), matchcol[0].D(), matchangle[0].D() + rotateangle, reclen1, reclen2, buf);
				}
			}
			else if (thisok[0].I() == 1) {  //????
				HTuple matchrow, matchcol, matchangle;
				getDispOutData(MatchRow, i, matchrow);
				getDispOutData(MatchCol, i, matchcol);
				getDispOutData(MatchAngle, i, matchangle);
				HTuple detectok;
				getDispOutData(CharDetectOK, i, detectok);
				if (detectok[0].I() == 0) {
					iv->setColor(255, 0, 0);
				}
				else {
					iv->setColor(0, 255, 0);
				}
				iv->dispRect2(matchrow[0].D(), matchcol[0].D(), matchangle[0].D() + rotateangle, reclen1, reclen2, buf);
			}
		}
	} // end of loop i
}

void mark1Mod::textResult(ResultText *text, const QString &funName)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	if (m_param->IsDefaultSig&&m_param->IsUsePinDirOut) {
		return;
	}
	if (m_param->OcrValid && m_producting) {  //生产过程中启用OCR自动模板功能
		if (!m_ocrTempOK) {
			text->setTextColor(QColor(255, 0, 0));
			if (m_ocrOutReg)
				text->append(QObject::tr("char region out of search Region! please reset template."));
			text->append(QObject::tr(" auto template by OCR failed! "));
			return;
		}
	}
	if (!m_PatternEnable) {
		text->setTextColor(QColor(255, 0, 0));
		text->append(QObject::tr(" no template is ready,please set template!"));
		return;
	}
	if (m_charRight != 0) {
		text->setTextColor(QColor(255, 0, 0));
		if (m_charRight == 1) {
			text->append(QObject::tr("character's number is zero"));
		}
		else if (m_charRight == 2) {
			text->append(QObject::tr(" different templates' character's numbers varies when using multiple fonts"));
		}
		else if (m_charRight == 3) {
			text->append(QObject::tr(" the region of characters of template %1 is beyond Global character region").arg(m_charWrongIndex + 1));
		}
		return;
	}
	if (!m_dirSame) {
		text->setTextColor(QColor(255, 0, 0));
		text->append(QObject::tr("the set of Multi-directional templates when using the function of outputting direction is wrong,please check it"));
		return;
	}
	HTuple regionmatchok;
	if (m_param->IsReferSection == 2) {
		getDispOutData(RegionMatchOk, regionmatchok);
		HTuple score;
		getDispOutData(RegionMatchScore, score);
		int msc = score[0].I();
		if (!regionmatchok[0].I()) {
			text->setTextColor(QColor(255, 0, 0));
			text->append(QObject::tr("Global character region match fail"));
			text->append(QObject::tr("match score=%1").arg(score[0].I()));
			return;
		}
		else {
			text->setTextColor(QColor(0, 0, 0));
			text->append(QObject::tr("Global character region match score=%1").arg(score[0].I()));
			HTuple layIndex;
			int layerIndex;
			if (m_param->IsMult&&m_param->IsFontOrLayer == 1) {
				getDispOutData(LayerMatchIndex, layIndex);
				layerIndex = layIndex[0].I();
				text->append(QObject::tr("matched layer is template %1").arg(layerIndex + 1));
			}
		}
	}
	HTuple allok, thisok, matchscore, matchindex;
	getDispOutData(AllOk, allok);

	HTuple layIndex;
	int layerIndex;
	if (m_param->IsMult&&m_param->IsFontOrLayer == 1) {
		getDispOutData(LayerMatchIndex, layIndex);
		layerIndex = layIndex[0].I();
	}
	int tmpCharCounts;
	if (m_param->IsMult&&m_param->IsFontOrLayer == 1) {
		tmpCharCounts = m_param->charCounts[layerIndex];
	}
	else {
		tmpCharCounts = m_param->charCounts[0];
	}

	for (int i = 0;i < tmpCharCounts;i++)
	{
		getDispOutData(CharFoundOk, i, thisok);
		getDispOutData(MatchIndex, i, matchindex);
		if (thisok[0].I() == 0) {
			getDispOutData(MatchScore, i, matchscore);
			int score = (int)(matchscore[0].D() * 100);
			text->setTextColor(QColor(255, 0, 0));
			text->append(QObject::tr("char[%1]' match fail,").arg(QString::number(i + 1)));
			text->insertPlainText(QObject::tr(" score = %1(%2)").arg(QString::number(score)).
				arg(m_param->acceptSimilar));
			if (matchscore[0].D() > 0 && m_param->errAngleValid) {
				HTuple angle;
				getDispOutData(MatchAngle, i, angle);
				text->insertPlainText(QObject::tr(",angle =%1(%2)")
					.arg(QString::number(angle[0].D() / M_PI * 180, 'g', 3)).arg(QString::number(m_param->errAngle)));
			}
		}
		else {
			HTuple detectok;
			getDispOutData(CharDetectOK, i, detectok);
			if (detectok[0].I() == 0) {
				text->setTextColor(QColor(255, 0, 0));
			}
			else {
				text->setTextColor(QColor(0, 0, 0));
			}
			getDispOutData(MatchScore, i, matchscore);
			int score = (int)(matchscore[0].D() * 100);
			//text->setTextColor(QColor(0,0,0));
			text->append(QObject::tr("char[%1]'s score = %2(%3)").arg(QString::number(i + 1)).arg(QString::number(score)).
				arg(m_param->acceptSimilar));
			if (m_param->errAngleValid) {
				HTuple angle;
				getDispOutData(MatchAngle, i, angle);
				text->insertPlainText(QObject::tr(",angle =%1(%2)")
					.arg(QString::number(angle[0].D() / M_PI * 180, 'g', 3)).arg(QString::number(m_param->errAngle)));
			}
			if (m_param->errOffSetValid) {
				HTuple offset;
				getDispOutData(OffSetval, i, offset);
				text->insertPlainText(QObject::tr(",offset =%1(%2)")
					.arg(QString::number(offset[0].I())).arg(QString::number(m_param->errOffSet)));
			}
			if (m_param->errOffXValid) {
				HTuple offok, offval;
				getDispOutData(OffXval, i, offval);
				getDispOutData(OffXOk, i, offok);
				if (offok[0].I() == 0) {
					text->setTextColor(QColor(255, 0, 0));
				}
				else {
					text->setTextColor(QColor(0, 0, 0));
				}
				text->append(QObject::tr("Mark shift X: %1(%2,%3)").arg(QString::number(offval[0].D(), 'f', 3)).arg(m_param->errOffXMin).arg(m_param->errOffXMax));
			}
			if (m_param->errOffYValid) {
				HTuple offok, offval;
				getDispOutData(OffYval, i, offval);
				getDispOutData(OffYOk, i, offok);
				if (offok[0].I() == 0) {
					text->setTextColor(QColor(255, 0, 0));
				}
				else {
					text->setTextColor(QColor(0, 0, 0));
				}
				text->append(QObject::tr("Mark shift Y: %1(%2,%3)").arg(QString::number(offval[0].D(), 'f', 3)).arg(m_param->errOffYMin).arg(m_param->errOffYMax));
			}
			if (m_param->errAreaValid) {
				HTuple areaok, areaval;
				getDispOutData(AreaDiffVal, i, areaval);
				getDispOutData(AreaDiffOk, i, areaok);
				if (areaok[0].I() == 0) {
					text->setTextColor(QColor(255, 0, 0));
				}
				else {
					text->setTextColor(QColor(0, 0, 0));
				}
				text->append(QObject::tr("area difference:%1(%2)").arg(areaval[0].I()).arg(m_param->errAreaPercent));
			}
			if (m_param->errCrackValid) {
				HTuple crackok, crackval;
				getDispOutData(CrackVal, i, crackval);
				getDispOutData(CrackOk, i, crackok);
				if (crackok[0].I() == 0) {
					text->setTextColor(QColor(255, 0, 0));
				}
				else {
					text->setTextColor(QColor(0, 0, 0));
				}
				text->append(QObject::tr("max char crack :%1(%2)").arg(QString::number(crackval[0].D(), 'f', 4)).arg(QString::number(m_param->errCrack, 'f', 4)));
			}
			if (m_param->errExcessValid) {
				HTuple excessok, excessval;
				getDispOutData(ExcessVal, i, excessval);
				getDispOutData(ExcessOk, i, excessok);
				if (excessok[0].I() == 0) {
					text->setTextColor(QColor(255, 0, 0));
				}
				else {
					text->setTextColor(QColor(0, 0, 0));
				}
				text->append(QObject::tr("max char excess :%1(%2)(%)").arg(QString::number(excessval[0].D(), 'f', 4)).arg(QString::number(m_param->errExcess, 'f', 4)));
			}
		}
	}
	if (allok[0].I() == 0) {
		text->setTextColor(QColor(255, 0, 0));
		text->append(QObject::tr("match failed"));

	}
	else if (allok[0].I() == 1) {
		text->setTextColor(QColor(0, 255, 0));
		text->append(QObject::tr("match success"));
	}
}

void mark1Mod::iniData()
{
	m_param = new mark1Param;
	m_image = new Hobject;
	m_charRegion = new Hobject;
	for (int i = 0;i < MAX_MARK_NUM;i++) {
		for (int j = 0;j < 4;j++) {
			m_modelId[i][j].clear();
			m_RegionModelId[i][j] = -1;
		}
	}
	m_ocrTempOK = true;
	m_charRight = 0;
	m_charWrongIndex = 0;
	m_PatternEnable = true;
	m_dirSame = true;
	m_producting = true;
	m_ocrOutReg = false;
	for (int i = 0;i < 4;i++) {
		m_row[i] = 0;
		m_col[i] = 0;
	}
	createPins();
}

void mark1Mod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2d, "hm2d");
	addPin(&p_pixelSize, "sz");
	addPin(&p_corners, "corners");
	addPin(&p_dirIdx, "dirIdx");
	addPin(&p_patIdx, "patIdx");
	addPin(&p_doOcr, "doOcr");
	addPin(&p_ocrNum, "ocrNum");
	addPin(&p_ocrRow1, "ocrRow1");
	addPin(&p_ocrRow2, "ocrRow2");
	addPin(&p_ocrCol1, "ocrCol1");
	addPin(&p_ocrCol2, "ocrCol2");

	addPin(&p_charReg, "charReg");
	addPin(&p_segImg, "segImg");
	addPin(&p_lfIdx, "lfIdx");
	addPin(&p_rtIdx, "rtIdx");
	addPin(&p_lf, "lf");
	addPin(&p_rt, "rt");
	addPin(&p_outDirIdx, "outPatIdx");
	addPin(&p_autoOcrOk, "autoOcrOk");

	p_pixelSize.setVisible(false);
	p_dirIdx.setVisible(false);
	p_patIdx.setVisible(false);
	p_corners.setVisible(false);

	p_doOcr.setVisible(false);
	p_ocrNum.setVisible(false);
	p_ocrRow1.setVisible(false);
	p_ocrRow2.setVisible(false);
	p_ocrCol1.setVisible(false);
	p_ocrCol2.setVisible(false);

	p_lfIdx.setVisible(false);
	p_rtIdx.setVisible(false);
	p_lf.setVisible(false);
	p_rt.setVisible(false);
	p_outDirIdx.setVisible(false);
	p_autoOcrOk.setVisible(false);
}

void mark1Mod::createEvents()
{
	addEvent(ENT_WRITE_IO);
}

void mark1Mod::creatNccTemp(bool createAll, int index)
{
	int startIndex = 0;
	int endIndex = m_param->PatternNum;
	if (!createAll) {
		startIndex = index;
		endIndex = index + 1;
	}
	for (int i = startIndex;i < endIndex;i++) {
		if (!m_param->MultPatternEnable[i] && createAll) {
			continue;
		}

		bool iscontinue = true;
		if (m_param->SegImg[i].Id() == H_EMPTY_REGION) {
			iscontinue = false;
			m_param->MultPatternEnable[i] = false;
			return;
		}
		int angleVal[4] = { 0,90,180,270 };
		for (int d = 0;d < 4;d++) {
			for (int j = 0;j < m_modelId[i][d].size();j++) {
				if (m_modelId[i][d][j] >= 0) {
					clear_ncc_model(m_modelId[i][d][j]);
				}
			}
			m_modelId[i][d].clear();
			if (!m_param->IsDirPattern&&d > 0) {
				break;
			}
			if (m_param->IsDirPattern) {
				if (m_param->angleType == 0 && (d == 1 || d == 3)) {
					continue;
				}
			}
			double rRow, rCol;   //旋转区域的中心点
			rRow = (m_param->RegionRoi[i].row1 + m_param->RegionRoi[i].row2) / 2.0;
			rCol = (m_param->RegionRoi[i].col1 + m_param->RegionRoi[i].col2) / 2.0;

			HTuple hommat;
			double rangle = PI*angleVal[d] / 180.0;
			vector_angle_to_rigid(rRow, rCol, 0.0, rRow, rCol, rangle, &hommat);  //建立旋转关系矩阵

			for (int j = 0;j < m_param->charCounts[i];j++) {
				Hobject patimg;
				double tmpr1 = m_param->segRegionVector[i][j].row1;
				double tmpr2 = m_param->segRegionVector[i][j].row2;
				double tmpc1 = m_param->segRegionVector[i][j].col1;
				double tmpc2 = m_param->segRegionVector[i][j].col2;

				double ttmpr1 = m_param->RegionRoi[i].row1;
				double ttmpr2 = m_param->RegionRoi[i].row2;
				double ttmpc1 = m_param->RegionRoi[i].col1;
				double ttmpc2 = m_param->RegionRoi[i].col2;
				crop_rectangle1(m_param->SegImg[i], &patimg, 
					(tmpr1 - ttmpr1) >= 0 ? (tmpr1 - ttmpr1) : 0,
					(tmpc1 - ttmpc1) >= 0 ? (tmpc1 - ttmpc1) : 0,
					(tmpr2 - ttmpr1) >= 0 ? (tmpr2 - ttmpr1) : 0,
					(tmpc2 - ttmpc1) >= 0 ? (tmpc2 - ttmpc1) : 0);
				if (d > 0) {
					rotate_image(patimg, &patimg, angleVal[d], "bilinear");
				}
				Hlong modelID = -1;
				create_ncc_model(patimg, "auto", -0.39, 0.79, "auto", "use_polarity", &modelID);
				m_modelId[i][d].push_back(modelID);
			}
		} //end of loop d
		if (!createAll)
			m_param->MultPatternEnable[m_param->NowPatternNum] = true;
	} //end of loop pattern i
}

void mark1Mod::createRegionTemp(int index)
{
	if (m_param->SegImg[index].Id() == H_EMPTY_REGION) {
		return;
	}
	int angleVal[4] = { 0,90,180,270 };
	for (int d = 0;d < 4;d++) {
		if (m_RegionModelId[index][d] >= 0) {
			clear_ncc_model(m_RegionModelId[index][d]);
			m_RegionModelId[index][d] = -1;
		}
		if (!m_param->IsDirPattern&&d > 0) {
			break;
		}
		if (m_param->IsDirPattern) {
			if (m_param->angleType == 0 && (d == 1 || d == 3)) {
				continue;
			}
		}
		Hobject patimg;
		if (d > 0) {
			rotate_image(m_param->SegImg[index], &patimg, angleVal[d], "bilinear");
		}
		else {
			copy_image(m_param->SegImg[index], &patimg);
		}
		create_ncc_model(patimg, "auto", -0.20, 0.39, "auto", "use_polarity", &m_RegionModelId[index][d]);
	}
}

bool mark1Mod::autoTemp()
{
	HTuple ocrnum;
	HTuple ocrRow1, ocrRow2, ocrCol1, ocrCol2;
	int num;
	try {
		//getDetectInData(OCRNum, ocrnum);
		ocrnum = **p_ocrNum;
		num = ocrnum[0].I();
		if (!num) {
			return false;
		}
		//ocrRow1 = *p_ocrRow1;
		//ocrRow2 = *p_ocrRow2;
		//ocrCol1 = *p_ocrCol1;
		//ocrCol2 = *p_ocrCol2;
		//getDetectInData(OCRRow1, ocrRow1);
		//getDetectInData(OCRRow2, ocrRow2);
		//getDetectInData(OCRCol1, ocrCol1);
		//getDetectInData(OCRCol2, ocrCol2);
// 		if (ocrRow1.Num() != num + 1 || ocrRow2.Num() != num + 1 || 
// 			ocrCol1.Num() != num + 1 || ocrCol2.Num() != num + 1) {
// 			return false;
// 		}
		if (p_ocrRow1->size() != num + 1 || p_ocrRow2->size() != num + 1 ||
			p_ocrCol1->size() != num + 1 || p_ocrCol2->size() != num + 1) {
			return false;
		}
		for (auto i = 0;i < p_ocrRow1->size();++i)
		{
			ocrRow1[i] = p_ocrRow1->at(i);
			ocrRow2[i] = p_ocrRow2->at(i);
			ocrCol1[i] = p_ocrCol1->at(i);
			ocrCol2[i] = p_ocrCol2->at(i);
		}
	}
	catch (HException& ex) {
		return false;
	}
	int index = 0/*m_param->NowPatternNum*/;
	m_param->RegionRoi[index].row1 = ocrRow1[0].D();
	m_param->RegionRoi[index].col1 = ocrCol1[0].D();
	m_param->RegionRoi[index].row2 = ocrRow2[0].D();
	m_param->RegionRoi[index].col2 = ocrCol2[0].D();

	if (!p_im->empty()) {
		HalOpenCV::cv2halImg(*p_im, m_image, false);
		crop_rectangle1(*m_image, &m_param->SegImg[index], 
			ocrRow1[0].D(), ocrCol1[0].D(), ocrRow2[0].D(), ocrCol2[0].D());
		if (index == 0) {
			HalOpenCV::hal2cvImg(m_param->SegImg[0], *p_segImg, true);
			//setDetectOutData(OutSegImg, (Hlong)&m_param->SegImg[0]);
		}
	}
	else {
		gen_empty_obj(&m_param->SegImg[index]);
// 		QMessageBox::warning(this, QObject::tr("Warning"), QObject::tr("no image for setting template!"),
// 			QMessageBox::Yes);
		return false;
	}
	m_param->MultPatternEnable[index] = false;

	//分割模板
	double row1[MAX_CHAR_NUM], col1[MAX_CHAR_NUM], row2[MAX_CHAR_NUM], col2[MAX_CHAR_NUM];
	for (int i = 0;i < num;i++) {
		row1[i] = ocrRow1[i + 1].D();
		row2[i] = ocrRow2[i + 1].D();
		col1[i] = ocrCol1[i + 1].D();
		col2[i] = ocrCol2[i + 1].D();
	}

	mark1Param::segRegion seg;
	seg.row1 = seg.col1 = 100;
	seg.row2 = seg.col2 = 150;  //给新添加的roi固定的开始位1置

	int stindex = 0/*m_param->NowPatternNum*/;
	int endindex = 0/*m_param->NowPatternNum*/ + 1;    //目前ocr不支持多模板模式，所以暂时全部使用第一个模板
													   //if (m_param->IsMult&&m_param->IsFontOrLayer==0){
													   //	stindex=0;endindex=m_param->PatternNum;
													   //}
	if (num >= m_param->charCounts[0/*m_param->NowPatternNum*/]) {
		for (int i = m_param->charCounts[0/*m_param->NowPatternNum*/];i < num;i++) {
			seg.row1 = seg.col1 += 10;
			seg.row2 = seg.col2 += 10;
			for (int j = stindex;j < endindex;j++) {
				m_param->segRegionVector[j].push_back(seg);
			}
		}
	}
	else if (num < m_param->charCounts[0/*m_param->NowPatternNum*/]) {
		for (int j = stindex;j < endindex;j++) {
			vector<mark1Param::segRegion>::iterator itbegin = m_param->segRegionVector[j].begin() + m_param->charCounts[m_param->NowPatternNum]/*ui.sp_charCounts->value()*/;
			vector<mark1Param::segRegion>::iterator itEnd = m_param->segRegionVector[j].end();
			m_param->segRegionVector[j].erase(itbegin, itEnd);
		}
	}
	for (int i = stindex;i < endindex;i++) {
		m_param->charCounts[i] = num;
	}
	if (m_param->charCounts[0/*m_param->NowPatternNum*/] > 0) {
		//m_advanceDialog->ui.cob_charIndex->setCurrentIndex(0);
	}
	else if (m_param->charCounts[0/*m_param->NowPatternNum*/] == 0) {

	}
	for (int i = stindex;i < endindex;i++) {
		m_param->MultPatternEnable[i] = 0;
	}
	for (int i = 0;i < num;i++) {
		m_param->segRegionVector[0][i].row1 = row1[i];
		m_param->segRegionVector[0][i].row2 = row2[i];
		m_param->segRegionVector[0][i].col1 = col1[i];
		m_param->segRegionVector[0][i].col2 = col2[i];
	}
	for (int i = 0;i < m_param->charCounts[index];i++) {
		if (row1[i]<m_param->RegionRoi[index].row1 || row2[i]>m_param->RegionRoi[index].row2 ||
			col1[i]<m_param->RegionRoi[index].col1 || col2[i]>m_param->RegionRoi[index].col2) {
// 			QMessageBox::warning(this, QObject::tr("Warning"), QObject::tr("char region out of search Region! please reset template."),
// 				QMessageBox::Yes);
			m_ocrOutReg = true;
			return false;
		}
	}
	creatNccTemp(false, index);
	if (m_param->MultPatternEnable[index]) {
		createRegionTemp(index);
	}
	//log(LogL1, QObject::tr("auto reset template by OCR"));
	return true;
}

void mark1Mod::clearTemp(int stationIdx)
{
	int stIdx = 0, edIdx = MAX_MARK_NUM;
	if (stationIdx != -1) {
		stIdx = stationIdx;
		edIdx = stationIdx + 1;
	}
	for (int i = stIdx;i < edIdx;i++) {
		for (int d = 0;d < 4;d++) {
			for (int j = 0;j < m_modelId[i][d].size();j++) {
				if (m_modelId[i][d][j] >= 0) {
					clear_ncc_model(m_modelId[i][d][j]);
				}
				m_modelId[i][d][j] = -1;
			}
			if (m_RegionModelId[i][d] >= 0) {
				clear_ncc_model(m_RegionModelId[i][d]);
				m_RegionModelId[i][d] = -1;
			}
		}
		m_param->MultPatternEnable[i] = false;
		//m_param->deleteImg(i);
		gen_empty_obj(&m_param->SegImg[i]);
	}
}

void mark1Mod::saveTemp(bool all)
{
	QStringList filters;
	filters << "*.bmp";
	filters << "*.jpg";

	QString dirPath;
	dirPath = QString::fromLocal8Bit(m_param->AutoSaveTmpImgDir);
	QDir dir;
	dir.mkpath(dirPath);
	QFileInfoList infos = dir.entryInfoList(/*filters*/);
	for (int i = 0;i < infos.size();i++) {
		//dir.remove(infos[i].fileName());
	}
	for (int i = 0;i < MAX_MARK_NUM;i++)
	{
		if (m_param->MultPatternEnable[i])
		{
			if (m_param->SegImg[i].Id() != H_EMPTY_REGION)
			{
				char imgPath[256];
				sprintf(imgPath, "%s/1.bmp", m_param->AutoSaveTmpImgDir);
				set_check("~give_error");
				write_image(m_param->SegImg[i], "bmp", 0, imgPath);
				set_check("give_error");
				if (!all)
					break;
			}
		}
	}
}

