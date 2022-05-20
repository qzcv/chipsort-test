#include "pinDistanceMod.h"
#include "pinDistanceParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "HalOpenCV.h"

#define AllOK "allok"
#define FindOk "findok"  //是否有检测到管脚
#define RegionOk "regionSOk"   //比对误差范围后的各单个管脚区域是1否OK  

#define DistanceRow "distanceRow"   
#define DistanceCol "distanceCol"

#define PinDistance "pinDistance"  //边界距离
#define PinDistanceOK "pinDistanceOk"  //

#define ImageFull "Imagefull"  //图片是否ok
//InData
#define TmpHomMat2D "tmpHomma2d"

pinDistanceMod::pinDistanceMod()
{
	iniData();
}

pinDistanceMod::pinDistanceMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

pinDistanceMod::~pinDistanceMod()
{
	delete m_param;
	delete m_image;
}

int pinDistanceMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int pinDistanceMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void pinDistanceMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
}

void pinDistanceMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int pinDistanceMod::run(const QString &funName)
{
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
	}
	double mk[GRP_NUM];
	for (int i = 0;i < GRP_NUM;i++) {
		mk[i] = m_param->sizeRatio[i];
	}
	int roiW[GRP_NUM][10], roiH[GRP_NUM][10];    //roi宽度 高度
	double roiMidRow[GRP_NUM][10], roiMidCol[GRP_NUM][10];
	double roiangle[GRP_NUM][10];
	HTuple hommat;
	for (int i = 0;i < GRP_NUM;i++) {
		if (m_param->IsRefer) {
			 HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &hommat);
			//getDetectInData(HomMat2D, hommat);
			try {
				double va = hommat[3].D();
			}
			catch (HException& ex) {
				hom_mat2d_identity(&hommat);
			}
			setDetectOutData(TmpHomMat2D, hommat);
		}
		for (int j = 0;j < m_param->grpPinNum[i];j++) {
			roiW[i][j] = m_param->RoiVec[i][j].col2 - m_param->RoiVec[i][j].col1;
			roiH[i][j] = m_param->RoiVec[i][j].row2 - m_param->RoiVec[i][j].row1;
			roiMidRow[i][j] = (m_param->RoiVec[i][j].row1 + m_param->RoiVec[i][j].row2) / 2.0;
			roiMidCol[i][j] = (m_param->RoiVec[i][j].col1 + m_param->RoiVec[i][j].col2) / 2.0;
			roiangle[i][j] = 0;
			if (m_param->IsRefer) {
				affine_trans_pixel(hommat, roiMidRow[i][j], roiMidCol[i][j], &roiMidRow[i][j], &roiMidCol[i][j]);
				roiangle[i][j] = atan((hommat[3].D() / hommat[0].D()));
			}
		}
	}
	Hlong measureHandle;
	HTuple Imgwidth, ImgHeight;

	HTuple pinDis[GRP_NUM];                     //管脚站高差异
	get_image_size(*m_image, &Imgwidth, &ImgHeight);
	HTuple referrow, refercol;
	for (auto i = 0;i < p_corners->size();++i)
	{
		referrow[i] = p_corners->at(i).y;
		refercol[i] = p_corners->at(i).x;
	}
	//getDetectInData(ReferRow, referrow);  //左上角顺时针
	//getDetectInData(ReferCol, refercol);
	double m_referRow[4][2], m_referCol[4][2];
	try {
		m_referRow[0][0] = referrow[0].D();  //上
		m_referRow[0][1] = referrow[1].D();
		m_referCol[0][0] = refercol[0].D();
		m_referCol[0][1] = refercol[1].D();
		m_referRow[1][0] = referrow[3].D();  //下
		m_referRow[1][1] = referrow[2].D();
		m_referCol[1][0] = refercol[3].D();
		m_referCol[1][1] = refercol[2].D();
		m_referRow[2][0] = referrow[0].D();  //左
		m_referRow[2][1] = referrow[3].D();
		m_referCol[2][0] = refercol[0].D();
		m_referCol[2][1] = refercol[3].D();
		m_referRow[3][0] = referrow[1].D();  //右
		m_referRow[3][1] = referrow[2].D();
		m_referCol[3][0] = refercol[1].D();
		m_referCol[3][1] = refercol[2].D();
	}
	catch (HException& ex) {
		m_referRow[0][0] = 100;  //上
		m_referRow[0][1] = 100;
		m_referCol[0][0] = 100;
		m_referCol[0][1] = 300;
		m_referRow[1][0] = 300;  //下
		m_referRow[1][1] = 300;
		m_referCol[1][0] = 100;
		m_referCol[1][1] = 300;
		m_referRow[2][0] = 100;  //左
		m_referRow[2][1] = 300;
		m_referCol[2][0] = 100;
		m_referCol[2][1] = 100;
		m_referRow[3][0] = 100;  //右
		m_referRow[3][1] = 300;
		m_referCol[3][0] = 300;
		m_referCol[3][1] = 300;
	}
	for (int i = 0;i < GRP_NUM;i++) {
		if (i < 2) {
			m_referRow[i][0] += m_param->edgeOffset[i];
			m_referRow[i][1] += m_param->edgeOffset[i];
		}
		else {
			m_referCol[i][0] += m_param->edgeOffset[i];
			m_referCol[i][1] += m_param->edgeOffset[i];
		}
	}
	for (int i = 0;i < GRP_NUM;i++) {
		if (!m_param->grpEnable[i]) {
			continue;
		}
		for (int j = 0;j < m_param->grpPinNum[i];j++) {    //最后一个是检测底部 ，对各组的边界线进行检测
			char edge[32];
			sprintf(edge, "negative");
			double meaRow, meaCol, meaAngle;
			int meaLength1;
			meaRow = roiMidRow[i][j];
			meaCol = roiMidCol[i][j];
			if (i == 0) {  //上
				meaLength1 = roiH[i][j] / 2;
				meaAngle = PI / 2.0 + roiangle[i][j];
			}
			else if (i == 1) { //下
				meaLength1 = roiH[i][j] / 2;
				meaAngle = -PI / 2.0 + roiangle[i][j];
			}
			else if (i == 2) { //左
				meaLength1 = roiW[i][j] / 2;
				meaAngle = PI + roiangle[i][j];
			}
			else if (i == 3) { //右
				meaLength1 = roiW[i][j] / 2;
				meaAngle = roiangle[i][j];
			}
			int validNum = 0;  //检测三个点，有效点数目
			double disTotal = 0;
			gen_measure_rectangle2(meaRow, meaCol, meaAngle, meaLength1, 6, Imgwidth[0], ImgHeight[0],/*"bilinear"*/"nearest_neighbor", &measureHandle);
			double transRow, transCol;
			//transRow=meaRow;
			//transCol=meaCol;
			transRow = (meaRow - 10.0*cos(meaAngle)*-1);
			transCol = (meaCol - 10.0*sin(meaAngle));
			HTuple disRow, disCol;   //管脚的中心点
			for (int z = 0;z < 3;z++) {
				if (z > 0) {
					transRow += 10.0*cos(meaAngle)*-1;
					transCol += 10.0*sin(meaAngle);
				}
				translate_measure(measureHandle, transRow, transCol);
				HTuple rowEdge, colEdge, amp, distance;
				measure_pos(*m_image, measureHandle, 1, m_param->edgeThreshold[i], edge, "first", &rowEdge, &colEdge, &amp, &distance);
				if (rowEdge.Num() > 0) {
					disRow[validNum] = rowEdge[0].D();
					disCol[validNum] = colEdge[0].D();
					validNum++;
					double mdis;
					distance_pl(rowEdge[0].D(), colEdge[0].D(), m_referRow[i][0], m_referCol[i][0],
						m_referRow[i][1], m_referCol[i][1], &mdis);
					disTotal += mdis;
				}
			}
			close_measure(measureHandle);
			if (validNum == 0) {
				setDetectOutData(FindOk, 0, i, j);
				m_testItemsStatus[0] = 0;
				allok = false;
			}
			else {
				double averageDis = disTotal / validNum;
				averageDis *= m_param->sizeRatio[i];
				bool regionok = true;
				if (m_param->errDistanceValid) {
					bool thisok = true;
					if (averageDis<m_param->errDistanceMin[i] || averageDis>m_param->errDistanceMax[i]) {
						thisok = false;
						allok = false;
						regionok = false;
						m_testItemsStatus[1] = 0;
					}
					setDetectOutData(PinDistanceOK, thisok, i, j);
					setDetectOutData(PinDistance, averageDis, i, j);
				}
				setDetectOutData(RegionOk, regionok, i, j);
				setDetectOutData(FindOk, 1, i, j);
				Hlong disnum = disRow.Num();
				setDetectOutData(DistanceRow, disRow, i, j);
				setDetectOutData(DistanceCol, disCol, i, j);
			}
		}
	}
	setDetectOutData(AllOK, allok);
	return allok;
}

void pinDistanceMod::viewResult(ImageView *iv, const QString &funName, int)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	HTuple homeMat2D;
	if (m_param->IsRefer) {
		getDispOutData(TmpHomMat2D, homeMat2D);
	}
	for (int i = 0;i < GRP_NUM;i++) {
		if (!m_param->grpEnable[i])
			continue;
		for (int j = 0;j < m_param->grpPinNum[i];j++) {
			HTuple findok;
			getDispOutData(FindOk, i, j, findok);
			if (!findok[0].I()) {
				Hobject rec;
				gen_rectangle1(&rec, m_param->RoiVec[i][j].row1, m_param->RoiVec[i][j].col1, m_param->RoiVec[i][j].row2, m_param->RoiVec[i][j].col2);
				if (m_param->IsRefer) {
					affine_trans_region(rec, &rec, homeMat2D, "false");
				}
				iv->setColor(255, 0, 0);
				iv->setFilled(false);
				continue;
			}
			HTuple regionok;
			getDispOutData(RegionOk, i, j, regionok);
			if (regionok[0].I() == 0) {
				iv->setColor(255, 0, 0);
			}
			else {
				iv->setColor(255, 255, 0);
			}
			HTuple disrow, discol;
			getDispOutData(DistanceRow, i, j, disrow);
			getDispOutData(DistanceCol, i, j, discol);
			Hlong disnum = disrow.Num();
			for (int z = 0;z < disrow.Num();z++) {
				iv->dispLine(disrow[z].D(), discol[z].D() - 3, disrow[z].D(), discol[z].D() + 3);
				iv->dispLine(disrow[z].D() - 3, discol[z].D(), disrow[z].D() + 3, discol[z].D());
			}
		}
	}
}

void pinDistanceMod::textResult(ResultText *text, const QString &funName)
{
	HTuple imagefull;
	getDispOutData(ImageFull, imagefull);
	if (!imagefull[0].I()) {
		return;
	}
	//char gname[4][32] = { "up","down","left","right" };
	QStringList gname = { QObject::tr("up"),QObject::tr("down"),QObject::tr("left"),QObject::tr("right") };
	for (int i = 0;i < GRP_NUM;i++) {
		if (!m_param->grpEnable[i]) {
			continue;
		}
		text->setTextColor(QColor(0, 0, 0));
		text->append(gname[i]);
		for (int j = 0;j < m_param->grpPinNum[i];j++) {
			HTuple findok;
			getDispOutData(FindOk, i, j, findok);
			if (!findok[0].I()) {
				text->setTextColor(QColor(255, 0, 0));
				text->append(QObject::tr("%1-lead%2 found failed:").arg(gname[i]).arg(j + 1));
				continue;
			}
			HTuple thisok;
			getDispOutData(RegionOk, i, j, thisok);
			if (thisok[0].I() == 0) {
				text->setTextColor(QColor(255, 0, 0));
			}
			else
				text->setTextColor(QColor(0, 0, 0));
			text->append(QObject::tr("Lead%1:  ").arg(QString::number(j + 1)));
			if (m_param->errDistanceValid) {
				HTuple dis;
				getDispOutData(PinDistance, i, j, dis);
				HTuple itemok;
				getDispOutData(PinDistanceOK, i, j, itemok);
				if (itemok[0].I() == 0) {
					text->setTextColor(QColor(255, 0, 0));
				}
				else
					text->setTextColor(QColor(0, 0, 0));
				text->append(QObject::tr("edge distance:%1(%2,%3)  ")
					.arg(QString::number(dis[0].D(), 'f', 4))
					.arg(QString::number(m_param->errDistanceMin[i], 'f', 4))
					.arg(QString::number(m_param->errDistanceMax[i], 'f', 4)));
			}
		} //enf of j loop
	}
}

void pinDistanceMod::iniData()
{
	m_param = new pinDistanceParam;
	m_image = new Hobject;
	createPins();
}

void pinDistanceMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2d, "hm2d");
	addPin(&p_corners, "corners");
}

