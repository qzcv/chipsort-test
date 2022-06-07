#include "fitEdge.h"
#include <vector>
using std::vector;
#include "highgui.h"


namespace{
	int RansacLinear(CvPoint2D32f* pstData,unsigned int nDataCnt, unsigned int nMinCnt,unsigned int nMaxIterCnt, unsigned int nConsesusCntThreshold, double dMaxErrorThreshold, double& dAA,double& dBB,double& dCC,double& dModelMeanError)
	{
		//default_random_engine C++随机数产生器
		default_random_engine rng;
		//uniform_int_distribution ：产生均匀分布的随机数
		uniform_int_distribution<unsigned> uniform(0,nDataCnt-1);
		rng.seed(10);
		set<unsigned int> sSelectIndexs;
		vector<CvPoint2D32f> vecSelectPoints;
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
			dB = - dDeltaX;

			dC = -dDeltaY*vecSelectPoints[1].x + dDeltaX*vecSelectPoints[1].y;

			//Step3：得到在阈值内的个数
			unsigned int nDataIter = 0;
			double dMeanError = 0;
			set<unsigned int >sTmpConsensusIndexs;
			while (nDataIter < nDataCnt)
			{
				double dDistance = (dA*pstData[nDataIter].x+dB*pstData[nDataIter].y+dC) / sqrt(dA*dA+dB*dB);
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


}

namespace danke{

fitEdgeLine::fitEdgeLine()
{
	m_p[0]=NULL;
	m_p[1]=NULL;
	m_roiNum=1;
	m_ransacValid=false;
	m_sortValid=false;
	m_referHommat=0;
	m_referAngleValid=false;
	m_referAngle=0;
	m_threshold=20;
	m_factor=1.0;
	m_searchWidth=2;
	m_searchGap=3;
	m_angle=0;
	m_polar=ToWhite;
	m_orient=ToUp;
	m_roirectType=rect1;
	m_roiRect=NULL;
	m_roiAngle=NULL;
	for (int i=0;i<m_roiNum;i++){
		m_roiRect=new CvRect[m_roiNum];
		m_roiAngle=new double[m_roiNum];
	}
	m_continousReferValid=false;
	m_referPointsNumber=1;
}

fitEdgeLine::~fitEdgeLine()
{	
	for(int i=0;i<2;i++){
		if (m_p[i]){
			delete []m_p[i];
		}
	}
	if (m_roiRect){
		delete []m_roiRect;
	}
	if (m_roiAngle){
		delete []m_roiAngle;
	}
}

void fitEdgeLine::setRoi(int roiNum, const int* row1,const int* col1,const int* row2,const int* col2)
{
	if (m_roiRect){
		delete []m_roiRect;
	}
	if (m_roiAngle){
		delete []m_roiAngle;
	}
	m_roirectType=rect1;

	m_roiNum=roiNum;
	m_roiRect=new CvRect[roiNum];
	m_roiAngle=new double[roiNum];
	double angleVal[4]={PI/2.0,-PI/2.0,PI,0};
	for (int i=0;i<roiNum;i++){
		m_roiAngle[i]=angleVal[m_orient];
		int tmplen[2];//w,h
		tmplen[0]=col2[i]-col1[i];
		tmplen[1]=row2[i]-row1[i];
		if (m_orient==ToRight || m_orient==ToLeft){
			m_roiRect[i].x=col1[i];
			m_roiRect[i].y=row1[i];
			m_roiRect[i].width=tmplen[0];
			m_roiRect[i].height=tmplen[1];
		}
		else  if(m_orient==ToUp || m_orient==ToDown){
			int centerx,centery;
			centerx=(col1[i]+col2[i])/2.0;
			centery=(row1[i]+row2[i])/2.0;
			m_roiRect[i].x=centerx-tmplen[1]/2.0;
			m_roiRect[i].y=centery-tmplen[0]/2.0;
			m_roiRect[i].width=tmplen[1];
			m_roiRect[i].height=tmplen[0];
		}
	}
}

void fitEdgeLine::setRoi(int roiNum, const int* row, const int* col, const int* length1, const int* length2, const double* angle )
{
	if (m_roiRect){
		delete []m_roiRect;
	}
	if (m_roiAngle){
		delete []m_roiAngle;
	}
	m_roirectType=rect2;

	m_roiNum=roiNum;
	m_roiRect=new CvRect[roiNum];
	m_roiAngle=new double[roiNum];

	for (int i=0;i<roiNum;i++){
		m_roiAngle[i]=angle[i];
		m_roiRect[i].x=col[i]-length1[i];
		m_roiRect[i].y=row[i]-length2[i];
		m_roiRect[i].width=length1[i]*2;
		m_roiRect[i].height=length2[i]*2;
	}
}

void fitEdgeLine::setRefer(HTuple hommat)
{
	m_referHommat=hommat;
}

void fitEdgeLine::setEdgeReferAngle(bool referValid,double angle)
{
	m_referAngleValid=referValid;
	m_referAngle=angle;
}

void fitEdgeLine::setPolarAndOrient(Polarity pol,Orient orient)
{
	m_polar=pol;
	double angleVal[4]={PI/2.0,-PI/2.0,PI,0};
	if (m_roiRect && m_roirectType==rect1){
		for (int i=0;i<m_roiNum;i++){
			m_roiAngle[i]=angleVal[orient];
			if ( (int(m_orient)/2) != (int(orient)/2)){
				int centerx,centery;
				centerx=m_roiRect[i].x+m_roiRect[i].width/2.0;
				centery=m_roiRect[i].y+m_roiRect[i].height/2.0;
				m_roiRect[i].x=centerx-m_roiRect[i].height/2.0;
				m_roiRect[i].y=centery-m_roiRect[i].width/2.0;
				int tmpval=m_roiRect[i].height;
				m_roiRect[i].height=m_roiRect[i].width;
				m_roiRect[i].width=tmpval;
			}
		}
	}
	m_orient=orient;
}

void fitEdgeLine::setCrossGrayValid(bool valid,int crossGray,int crossStep)
{
	m_crossGrayValid=valid;
	m_crossGray=crossGray;
	m_crossStep=crossStep;
}

void fitEdgeLine::setThreshold(int thre)
{
	m_threshold=thre;
}

void fitEdgeLine::setSearchParam(int searchGap,int searchWidth)
{
	m_searchGap=searchGap;
	m_searchWidth=searchWidth;
}

void fitEdgeLine::setFitFactor(double factor)
{
	m_factor=factor;
}

void fitEdgeLine::getEdgePoints(HTuple& edgeRow,HTuple& edgeCol)
{
	edgeRow=m_edgeRow1;
	edgeCol=m_edgeCol1;
}

void fitEdgeLine::calRangePoints()
{
	double angle[4]={PI/2.0,-PI/2.0,PI,0};
	double rotateAngle=0;
	if (m_referHommat.Num()==6)
		rotateAngle=atan( (m_referHommat[3].D()/m_referHommat[0].D()) );

	for (int i=0;i<2;i++){
		if (m_p[i])
			delete []m_p[i];
		m_p[i]=new CvPoint2D32f[m_roiNum];
	}
	
	for (int i=0;i<m_roiNum;i++){
		double roiY,roiX;
		roiX=m_roiRect[i].x+m_roiRect[i].width/2.0;
		roiY=m_roiRect[i].y+m_roiRect[i].height/2.0;
		float cosa=cos(m_roiAngle[i]+PI/2.0);
		float sina=sin(m_roiAngle[i]+PI/2.0);
		float vx[2]={m_roiRect[i].height/2.0, -m_roiRect[i].height/2.0};
		float vy[2]={-m_roiRect[i].height/2.0, m_roiRect[i].height/2.0}; //0,1顺序为矢量方向（矢量方向为orient角度加PI/2.0之后的角度）
		for(int j=0;j<2;j++){ 
			m_p[j][i].x=roiX+cosa*vx[j];
			m_p[j][i].y=roiY+sina*vy[j];
			double tmpx,tmpy;
			if (m_referHommat.Num()==6){
				affine_trans_pixel(m_referHommat,m_p[j][i].y,m_p[j][i].x,&tmpy,&tmpx);
				m_p[j][i].y=tmpy;
				m_p[j][i].x=tmpx;
			}
		}
	}
}

void fitEdgeLine::setEdgeMethod(bool ransacValid,bool sortValid)
{
	m_ransacValid=ransacValid;
	m_sortValid=sortValid;
}

void fitEdgeLine::setContinuousRefer(bool valid ,int referPointsNumber)
{
	m_continousReferValid=valid;
	m_referPointsNumber=referPointsNumber;
}

fitEdgeLine::ResType fitEdgeLine::calculateLine(const Hobject* img, CvPoint2D32f line[2], int& lineNum)
{
	if (!img || img->Id()==H_EMPTY_REGION)
		return PointsNg;
	bool resok;
	calRangePoints();
	resok=measureEdgePoints(img);
	if(!resok)
		return PointsNg;
	if (m_sortValid){
		resok=sortEdgePointsGroup();
		if(!resok)
			return SortNg;
	}
	resok=fitEdgePoints();
	if(!resok)
		return FitNg;
	else{
		CvPoint2D32f le[2];
		for (int i=0;i<2;i++){
			line[i].x=le[i].x=m_lineCol[i][0].D();
			line[i].y=le[i].y=m_lineRow[i][0].D();
			//line[i].clear();
			//line[i].push_back(le[i]);
			
		}
		lineNum=1;
		return ResOK;
	}
}

//bool fitEdgeLine::calculateEdgePoints(const Hobject* img, int isWhite ,HTuple& row1,HTuple& col1,HTuple& row2,HTuple& col2,double percent)
//{
//	if (!img || img->Id()==H_EMPTY_REGION)
//		return false;
//	bool resok;
//	calRangePoints();
//	resok=measureEdgePoints(img);
//	if(!resok)
//		return PointsNg;
//	if (m_sortValid){
//		resok=sortEdgePointsGroup();
//		if(!resok)
//			return SortNg;
//	}
//	resok=fitEdgePoints();
//	if(!resok)
//		return FitNg;
//	else{
//		CvPoint2D32f le[2];
//		for (int i=0;i<2;i++){
//			le[i].x=m_lineCol[i][0].D();
//			le[i].y=m_lineRow[i][0].D();
//			line[i].clear();
//			line[i].push_back(le[i]);
//		}
//		lineNum=1;
//		return ResOK;
//	}
//}

bool fitEdgeLine::measureEdgePoints(const Hobject* img,bool isPair)
{
	bool resOk=true;   
	m_image=img;
	get_image_size(*m_image,&m_imgW,&m_imgH);
	m_edgeRow1.Reset();
	m_edgeCol1.Reset();
	m_edgeRow2.Reset();
	m_edgeCol2.Reset();
	char edge[32];
	if (m_polar==ToBlack) //1白黑
		sprintf(edge,"negative");
	else
		sprintf(edge,"positive");
	double meaRow,meaCol,meaAngle;
	int meaLength1;
	int pointN=0;   //累计进行测试的点1数
	int usefulPoint=0;  // 累1计有效得出结果的点数(结1果不为空)
	double detAngle;
	double rotateAngle=0;
	if (m_referHommat.Num()==6){
		rotateAngle=atan( (m_referHommat[3].D()/m_referHommat[0].D()) );

	}
	for (int i=0;i<m_roiNum;i++)
	{
		Hlong measureHandle;
		meaRow=m_p[0][i].y;
		meaCol=m_p[0][i].x;
		meaLength1=m_roiRect[i].width/2;
		detAngle=m_roiAngle[i]+rotateAngle;
		HTuple rowEdge,colEdge,amp,distance;
		gen_measure_rectangle2( meaRow,meaCol,detAngle,meaLength1,m_searchWidth*2,m_imgW,m_imgH,"nearest_neighbor",&measureHandle);
		for (int startPoint=0;startPoint<m_roiRect[i].height;startPoint+=m_searchGap){
			double transRow,transCol;
			transRow=(double)(int)(m_p[0][i].y-startPoint*sin(detAngle-PI/2.0));
			transCol=(double)(int)(m_p[0][i].x+startPoint*cos(detAngle-PI/2.0));
			if (transRow<2 || transRow>(m_imgH-2) || transCol<2 || transCol>(m_imgW-2) ){
				pointN++;
				continue;
			}
			translate_measure(measureHandle, transRow, transCol);
			measure_pos(*m_image,measureHandle,1,m_threshold,edge,"first",&rowEdge,&colEdge,&amp,&distance);
			Hlong aa=rowEdge.Num();
			if (aa>0){
				bool crossok=true;
				if (m_crossGrayValid)
				{
					crossok=checkCrossGray(m_image,detAngle,rowEdge[0].D(),colEdge[0].D(),m_crossGray,m_crossStep,edge);
				}
				if (crossok)
				{
					m_edgeRow1[usefulPoint]=rowEdge[0];
					m_edgeCol1[usefulPoint]=colEdge[0];
					usefulPoint++;
				}
			}
			pointN++;
		} //采集边界点结束
		close_measure(measureHandle);
	}  //end of loop roinum j
	if (usefulPoint<4){   //至少需要求出四个点，否则失败
		resOk=false;
	}
	return resOk;
}

bool fitEdgeLine::fitEdgePoints()
{
	bool resOk=true;
	HTuple tmpRow,tmpCol;
	int validCnt=0;
	int tmpNum=m_edgeRow1.Num();
	if (m_ransacValid){
		int nNum=m_edgeRow1.Num();
		vector<CvPoint2D32f> v(nNum);
		for (int k=0;k<nNum;k++){
			v[k].x=m_edgeCol1[k].D();
			v[k].y=m_edgeRow1[k].D();
		}
		double dA,dB,dC,dMeanError;
		dA=dB=dC=0;
		int ress=RansacLinear(v.data(),v.size(),4,100,5,2/*m_param->clipfactor[i]*/,dA,dB,dC,dMeanError);
		//dA = dB = dC = 0;
		//2：得到模型最少需要的参数个数
		//50:最大的迭代次数
		//35:一致性个数阈值
		//0.1:最大误差阈值
		//RansacLinear(v.data(),v.size(),2,50,35,0.1,dA,dB,dC,dMeanError);
		if (ress==0){
			double abbs=abs(dA/dB);
			double lRow1,lRow2,lCol1,lCol2;  //通过求出的一般式求取两个坐标点
			if (abbs<0.0001){
				lCol1=100.0;
				lCol2=200.0;
				lRow1=-dC/dB;
				lRow2=-dC/dB;
			}
			else if (abbs>10000){
				lRow1=100.0;
				lRow2=200.0;
				lCol1=-dC/dA;
				lCol2=-dC/dA;
			}
			else{
				lRow1=40.0;
				lRow2=70.0;
				lCol1=(-dB*lRow1-dC)/dA;
				lCol2=(-dB*lRow2-dC)/dA;
			}
			for (int z=0;z<nNum;z++){
				HTuple dist;
				distance_pl(m_edgeRow1[z].D(),m_edgeCol1[z].D(),lRow1,lCol1
					,lRow2,lCol2,&dist);
				double diss=dist[0].D();
				if (diss<=5/* m_NoiseeDistance*/){
					validCnt++;
					tmpRow[validCnt-1]=m_edgeRow1[z].D();
					tmpCol[validCnt-1]=m_edgeCol1[z].D();
				}
			}
			m_edgeRow1.Reset();
			m_edgeCol1.Reset();
			m_edgeRow1=tmpRow;
			m_edgeCol1=tmpCol;
		}
		else{
			resOk=false;
		}
	}
	if (m_edgeRow1.Num()>=3){
		Hobject edgeObj;
		gen_contour_polygon_xld(&edgeObj,m_edgeRow1,m_edgeCol1);
		double factorval=m_factor; //下面使用的筛选因子
		if (int(factorval*10.0)%5!=0){
			factorval=double((int)(factorval+0.5)); 
		}
		HTuple nc,nr,dist;
		HTuple rowbegin,colbegin,rowend,colend;
		fit_line_contour_xld(edgeObj,"tukey",-1,0,5,1.5,&rowbegin,&colbegin,&rowend,  //1.0, 1.5, 2.0, 2.5, 3.0
			&colend,&nr,&nc,&dist);
		Hlong contor=rowbegin.Num();
		if (contor==0){
			resOk=false;
		}
		else{
			for (int i=0;i<2;i++){
				m_lineRow[i].Reset();
				m_lineCol[i].Reset();
			}
			m_lineRow[0][0]=rowbegin[0].D();
			m_lineRow[1][0]=rowend[0].D();
			m_lineCol[0][0]=colbegin[0].D();
			m_lineCol[1][0]=colend[0].D();
		}
	}
	else{
		resOk=false;
	}
	return resOk;
}

bool fitEdgeLine::sortEdgePointsGroup()
{
	bool resOk=true;
	int pointsNum=m_edgeRow1.Num();
	vector<HTuple> sortRow,sortCol; //存储分成不同组别的边界点
	if (m_orient==ToUp || m_orient==ToDown){
		for (int i=0;i<pointsNum;i++){
			
		}
	}
	else if(m_orient==ToLeft || m_orient==ToRight){

	}
	return resOk;
}

bool fitEdgeLine::checkCrossGray(const Hobject* image, double Angle,double edgeRow,double edgeCol, int crossGray, int step,const char* polar)
{
	if (image==NULL || image->Id()==H_EMPTY_REGION)
	{
		return false;
	}
	Hlong imgWidth,imgHeight;
	get_image_size(*image,&imgWidth,&imgHeight);

	double stepRow,stepCol;
	if (step<=0 )
	{
		return false;
	}
	bool negOK[2]={false,false},posOk[2]={false,false};
	for (int i=0;i<step;i++)
	{
		int polVal[2]={1,-1};
		for (int j=0;j<2;j++)  //分别对应正负两个方向
		{
			stepRow=edgeRow-polVal[j]*(i+1)*sin(Angle);
			stepCol=edgeCol+polVal[j]*(i+1)*cos(Angle);
			int tmpRow,tmpCol;
			tmpRow=stepRow+0.5;
			tmpCol=stepCol+0.5;
			if (tmpRow<0 || tmpRow>(imgHeight-1) || tmpCol<0 || tmpCol>(imgWidth-1))
			{
				return false;
			}
			HTuple grayval;
			get_grayval(*image,tmpRow,tmpCol,&grayval);
			int grayValue=grayval[0].I();
			if(  strcmp(polar,"positive")==0 || strcmp(polar,"all")==0  )
			{
				if (j==0){
					if (!posOk[j])
						posOk[j]=grayValue>=crossGray;
				}
				else{
					if(!posOk[j])
						posOk[j]=grayValue<=crossGray;
				}
			}
			if ( strcmp(polar,"negative")==0 || strcmp(polar,"all")==0 )
			{
				if (j==0){
					if(!negOK[j])
						negOK[j]=grayValue<=crossGray;
				}
				else{
					if(!negOK[j])
						negOK[j]=grayValue>=crossGray;
				}
			}
		}
	}
	bool allok=true;
	if (strcmp(polar,"positive")==0)
		allok=(posOk[0]&&posOk[1]);
	else if (strcmp(polar,"negative")==0)
		allok=(negOK[0]&&negOK[1]);
	else{
		allok= (posOk[0]&&posOk[1])||(negOK[0]&&negOK[1]);
	}//polar=="all"
	return allok;
}

}