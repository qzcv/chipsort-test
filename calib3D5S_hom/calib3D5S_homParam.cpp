#include "calib3D5S_homParam.h"
#include <iniParam.h>
#include "qvglobal.h"

calib3D5S_homParam::calib3D5S_homParam()
{
	iniData();
}

calib3D5S_homParam::~calib3D5S_homParam()
{

}

void calib3D5S_homParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/calib3D5S_hom.ini");

	if (r && (level != MachineLevel))
		return;

	for (int i = 0; i < BtTransNum; i++) {
		RW_VALUE1(r, btTransRow1[i], i);
		RW_VALUE1(r, btTransRow2[i], i);
		RW_VALUE1(r, btTransCol1[i], i);
		RW_VALUE1(r, btTransCol2[i], i);
	}
	for (int i = 0; i < SideTransNum; i++) {
		RW_VALUE1(r, sideTransRow1[i], i);
		RW_VALUE1(r, sideTransRow2[i], i);
		RW_VALUE1(r, sideTransCol1[i], i);
		RW_VALUE1(r, sideTransCol2[i], i);
	}
	for (int i = 0; i < BottomNum; i++) {
		RW_VALUE1(r, bottomRow1[i], i);
		RW_VALUE1(r, bottomCol1[i], i);
		RW_VALUE1(r, bottomRow2[i], i);
		RW_VALUE1(r, bottomCol2[i], i);
	}
	for (int i = 0; i < VerifyNum; i++) {
		RW_VALUE1(r, verRow1[i], i);
		RW_VALUE1(r, verCol1[i], i);
		RW_VALUE1(r, verRow2[i], i);
		RW_VALUE1(r, verCol2[i], i);
	}
	for (int i = 0; i < SideNum; i++) {
		for (int j = 0; j < 2; j++)
		{
			RW_VALUE2(r, sideRow1[j][i], j, i);
			RW_VALUE2(r, sideRow2[j][i], j, i);
			RW_VALUE2(r, sideCol1[j][i], j, i);
			RW_VALUE2(r, sideCol2[j][i], j, i);
		}
	}
	for (int i = 0; i < PointsNum; i++)
	{
		RW_VALUE1(r, abcdMachineX[i], i);
		RW_VALUE1(r, abcdMachineY[i], i);
		RW_VALUE1(r, abcdMachineZ[i], i);
		for (int j = 0; j < 2; j++)
		{
			RW_VALUE2(r, sideMachineX[j][i], j, i);
			RW_VALUE2(r, sideMachineY[j][i], j, i);
		}
	}
	for (int i = 0; i < BtPointsNum; i++)
	{
		RW_VALUE1(r, bottomMachineX[i], i);
		RW_VALUE1(r, bottomMachineY[i], i);
	}
	RW_VALUE(r, bottomMachineZ_5);
	for (int i = 0; i < 8; i++)
	{
		RW_VALUE1(r, BtHxy[i], i);
		for (int j = 0; j < 2; j++)
		{
			RW_VALUE2(r, SideHxy[j][i], j, i);
			RW_VALUE2(r, TransSideHxy[j][i], j, i);
		}
	}
	RW_VALUE(r, horOrVerWANG);
	RW_VALUE(r, minGraydiff);
	RW_VALUE(r, IsTransHom);
	RW_VALUE(r, IsBlackWang);
}

void calib3D5S_homParam::iniData()
{
	IsBlackWang = 0;
	firstChanged = false;
	IsTransHom = 0;
	minGraydiff = 20;
	horOrVerWANG = 0;
	for (int i = 0; i < BottomNum; i++) {
		bottomRow1[i] = 100;
		bottomRow2[i] = 150;
		bottomCol1[i] = 100;
		bottomCol2[i] = 200;
	}
	for (int i = 0; i < VerifyNum; i++) {
		verRow1[i] = 100;
		verRow2[i] = 150;
		verCol1[i] = 100;
		verCol2[i] = 200;
	}
	for (int i = 0; i < BtTransNum; i++) {
		btTransRow1[i] = 100;
		btTransRow2[i] = 150;
		btTransCol1[i] = 100;
		btTransCol2[i] = 200;
	}
	for (int i = 0; i < SideTransNum; i++) {
		sideTransRow1[i] = 100;
		sideTransRow2[i] = 150;
		sideTransCol1[i] = 100;
		sideTransCol2[i] = 200;
	}
	for (int i = 0; i < SideNum; i++) {
		for (int j = 0; j < 2; j++)
		{
			sideRow1[j][i] = 200;
			sideRow2[j][i] = 250;
			sideCol1[j][i] = 200;
			sideCol2[j][i] = 300;
		}
	}
	for (int i = 0; i < PointsNum; i++)
	{
		abcdMachineX[i] = 0;
		abcdMachineY[i] = 0;
		abcdMachineZ[i] = 0;
		for (int j = 0; j < 2; j++)
		{
			sideMachineX[j][i] = 0;
			sideMachineY[j][i] = 0;
		}
	}
	for (int i = 0; i < BtPointsNum; i++)
	{
		bottomMachineX[i] = 0;
		bottomMachineY[i] = 0;
	}
	bottomMachineZ_5 = 0;
	double hom[8] = { 1,0,0,0,0,1,0,0 };
	for (int i = 0; i < 8; i++)
	{
		BtHxy[i] = hom[i];
		SideHxy[0][i] = hom[i];
		SideHxy[1][i] = hom[i];
		TransSideHxy[0][i] = hom[i];
		TransSideHxy[1][i] = hom[i];
	}
}

