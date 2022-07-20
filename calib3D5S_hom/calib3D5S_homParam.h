#pragma once
#include <QString>

#define BottomNum 8 //6+2
#define BtTransNum 8
#define SideTransNum 6
#define SideNum 5

#define VerifyNum 8

#define PointsNum 4
#define BtPointsNum 5


class calib3D5S_homParam
{
public:
	explicit calib3D5S_homParam();
	~calib3D5S_homParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	int bottomRow1[BottomNum], bottomCol1[BottomNum], bottomRow2[BottomNum], bottomCol2[BottomNum];
	int btTransRow1[BtTransNum], btTransCol1[BtTransNum], btTransRow2[BtTransNum], btTransCol2[BtTransNum];
	int sideRow1[2][SideNum], sideCol1[2][SideNum], sideRow2[2][SideNum], sideCol2[2][SideNum];
	int sideTransRow1[SideTransNum], sideTransCol1[SideTransNum], sideTransRow2[SideTransNum], sideTransCol2[SideTransNum];

	double bottomMachineX[BtPointsNum], bottomMachineY[BtPointsNum];
	double bottomMachineZ_5; //b-5的纵坐标
	double sideMachineX[2][PointsNum], sideMachineY[2][PointsNum];
	double abcdMachineX[PointsNum], abcdMachineY[PointsNum], abcdMachineZ[PointsNum];

	int IsTransHom; //是否是变换标定
	int horOrVerWANG;  //竖直的王或者水平的王
	int minGraydiff;
	double BtHxy[8]; //得到的矩阵信息
	double SideHxy[2][8];
	double TransSideHxy[2][8];
	bool firstChanged;
	int IsBlackWang;

	int verRow1[VerifyNum], verCol1[VerifyNum], verRow2[VerifyNum], verCol2[VerifyNum];

private:
	void iniData();
};

