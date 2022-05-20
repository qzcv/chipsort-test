
#include "nccMatch.h"
#include <vector>
using std::vector;

NCCMatcher::NCCMatcher(){
    m_degStart=-15/180*PI;
    m_degRange=30/180*PI;
    m_degStep=1/180*PI;
    m_pattern=NULL;
    m_roi=NULL;
    m_threshold=0.6;
	m_maxOverlap=0;
	m_num=1;
	m_patternWidth=0;
	m_patternHeight=0;
	m_patternX=m_patternY=0;
	m_modleId=-1;
}


void NCCMatcher::setPattern(Hobject& img,int& patternX,int& patternY,const Region* rejectRegion){
	m_patternX=patternX;
	m_patternY=patternY;
	copy_image(img,&m_pattern);
	Hobject Rectangle, rejectReg, patternReg;
	Hlong area; 
    Hobject hpattern;
	copy_image(img,&hpattern);
	HTuple ImgWidth,ImgHeight;
	get_image_size(img,&ImgWidth,&ImgHeight);
	m_patternHeight=ImgHeight[0].I();
	m_patternWidth=ImgWidth[0].I();
	if(rejectRegion){
		gen_rectangle1(&Rectangle, 0, 0, m_patternHeight-1, m_patternWidth-1);
		if(rejectRegion->isCircle)
			gen_circle(&rejectReg, rejectRegion->cy, rejectRegion->cx, rejectRegion->R);
		else
			gen_rectangle2(&rejectReg, rejectRegion->cy, rejectRegion->cx, rejectRegion->angle, rejectRegion->w/2, rejectRegion->h/2);
		difference(Rectangle, rejectReg, &patternReg);
	}
	else
		gen_rectangle1(&patternReg, 0, 0, m_patternHeight-1, m_patternWidth-1);

	Hobject patternImage;
	reduce_domain(hpattern, patternReg, &patternImage);
	area_center(patternReg, &area, &m_row, &m_col);
	if (m_modleId>=0){
		clear_ncc_model(m_modleId);
	}
	create_ncc_model(patternImage,"auto",m_degStart,m_degRange, m_degStep,"use_polarity",&m_modleId);
}

void NCCMatcher::setRoi(const QRect *roi){
	if(m_roi){
		delete m_roi;
		m_roi=NULL;
	}
	if(roi){
		m_roi=new QRect;
		*m_roi=*roi;
	}	
}

void NCCMatcher::setSimilar(float val){
	val=MAX(val, 0);
	val=MIN(val, 1);
   m_threshold=val;
}

void NCCMatcher::setMaxOverlap(float val)
{
	val=MAX(val, 0);
	val=MIN(val, 1);
	m_maxOverlap=val;
}


 const vector<NCCMatcher::Result>& NCCMatcher::findPattern(const Hobject *img){
	m_result.clear();
	if(m_pattern.Id()==H_EMPTY_REGION)
		return m_result;
	Hobject tmpImg, roiImage;
	copy_image(*img,&tmpImg);
    if(m_roi){
        Hobject Rectangle;
        gen_rectangle1(&Rectangle,m_roi->y(),m_roi->x(),
                       m_roi->y()+m_roi->height(),m_roi->x()+m_roi->width());
        reduce_domain(tmpImg, Rectangle, &roiImage);
    }
    else
        roiImage=tmpImg;

    HTuple   rowCheck, columnCheck, angleCheck, scoreCheck;
	//int num=0;
	//if (m_num==1){
	//	num=2;
	//}
	//else{
	//	num=m_num;
	//}
    find_ncc_model(roiImage,m_modleId,m_degStart,m_degRange,
		m_threshold, m_num, m_maxOverlap, "true",
		0, &rowCheck, &columnCheck,&angleCheck,&scoreCheck);
	Result result;
	if (m_num==1){
		if (rowCheck.Num()>0){
			//double maxscore=0;
			//int bestIndex=0;
			//for (int i=0;i<rowCheck.Num();i++){
			//	if (scoreCheck[i].D()>maxscore){
			//		maxscore=scoreCheck[i].D();
			//		bestIndex=i;
			//	}
			//}
			result.row=rowCheck[0].D();
			result.col=columnCheck[0].D();
			result.angle=angleCheck[0].D();
			result.score= scoreCheck[0].D();
			//getbox(result.row, result.col, result.angle, result.box);
			//void NCCMatcher::getbox(double row, double col, double angle, QPointF points[4])
			HTuple homMat2D;
			//hom_mat2d_identity(&homMat2D);
			vector_angle_to_rigid(m_row+m_patternY, m_col+m_patternX, 0, result.row, result.col, result
				.angle, &homMat2D);
			result.homMat2d=homMat2D;
			if(result.score>m_threshold)
				m_result.push_back(result);
		}
	}
	else{
		for(int i=0;i<rowCheck.Num(); i++){
			result.row=rowCheck[i].D();
			result.col=columnCheck[i].D();
			result.angle=angleCheck[i].D();
			result.score= scoreCheck[i].D();
			//getbox(result.row, result.col, result.angle, result.box);
			//void NCCMatcher::getbox(double row, double col, double angle, QPointF points[4])
			HTuple homMat2D;
			//hom_mat2d_identity(&homMat2D);
			vector_angle_to_rigid(m_row+m_patternY, m_col+m_patternX, 0, result.row, result.col, result
				.angle, &homMat2D);
			result.homMat2d=homMat2D;
			if(result.score>m_threshold)
				m_result.push_back(result);
		}
	}
	return m_result;
}

void NCCMatcher::getMinusRowCol(double &row,double &col)
{
	row=m_row;
	col=m_col;
}

const Hobject* NCCMatcher::getPatternImage(){
    return &m_pattern;
}

void NCCMatcher::setNum( int val )
{
	m_num=val;
}

void NCCMatcher::setRotateDeg( float range, float step/*=1*/ )
{
	m_degStart=-range/180*PI;
	m_degRange=2*range/180*PI;
	m_degStep=step/180*PI;
}

//void NCCMatcher::getbox(double row, double col, double angle, QPointF points[4])
//{
//	HTuple homMat2D;
//	vector_angle_to_rigid(m_oy, m_ox, 0, row, col, angle, &homMat2D);
//	double srcRow[4]={0, 0, m_patternHeight-1, m_patternHeight-1};
//	double srcCol[4]={0, m_patternWidth-1,m_patternWidth-1, 0};
//	for(int i=0;i<4;i++){
//		double dstRow, dstCol;
//		affine_trans_point_2d(homMat2D, srcRow[i], srcCol[i], &dstRow, &dstCol);
//		points[i].setX(srcCol[i]);
//		points[i].setY(srcRow[i]);
//	}
//}

NCCMatcher::~NCCMatcher()
{
	delete m_roi;
	if (m_modleId>=0){
		clear_ncc_model(m_modleId);
	}
}

