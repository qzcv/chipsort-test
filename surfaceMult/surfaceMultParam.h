#pragma once
#include <QString>
#include <vector>

using std::vector;
#define MAX_Rec_Num 10 //7
#define MAX_Cir_Num 3  //3

#define MAX_Delete_Rec_Num 10   //7
#define MAX_Delete_Cir_Num 3    //5

class surfaceMultParam
{
public:
	explicit surfaceMultParam();
	~surfaceMultParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row;
		double col;
		double length1;
		double length2;
		double angle;
	}RRECReg;    //����ROI����

	typedef struct {
		double rrow;
		double rcol;
		double r;
	}RCirReg;   //Բ��ROI����

	typedef struct {
		double drow;
		double dcol;
		double dlength1;
		double dlength2;
		double dangle;
	}DRECReg;    //����delete����

	typedef struct {
		double drrow;
		double drcol;
		double dr;
	}DCirReg;   //Բ��DELETE����

				//�������
	int DetectModel;  //���ģʽ��0Ϊȱ��ģʽ��1ΪĿ��ģʽ,���������޸ĸ��������ƣ��Լ�δ��⵽Ŀ��ʱ�Ĵ���
	int IsChangeModel;//�Ƿ�����ⲿ������Ϣ�޸�ȱ�ݡ�Ŀ��ģʽ

	int deleteRefer;  //ʹ�ô�������ʱ�ų�����ʹ�òο�����
	int referSection; //��ת����ο�ģ��
	int pointRegionRefer;  //�����������ɵ���ɵ������������ת����
	int areaRegionRefer;  //��������������������������ת����

	int areaZoom;              //���������������

	int zoom_up;
	int zoom_down;
	int zoom_lf;
	int zoom_rt;  //��������������

	int IsSameParam;    //�Ƿ�ʹ��ͬһ������

						//������������
	int multAngle;     //Ĭ�ϵ����ö�ģ��ĽǶȣ�0-90,1-180,2-270
	double rotateRow1, rotateRow2, rotateCol1, rotateCol2;  //��ת����


															//�ų�����
	int deleteArea_section[3];  //�ų�ģ��ѡ��
	int deleteArea_zoom[3];

	int isShowDel;         //�Ƿ�����ʾ�ų�����

	int IsPixOrAct;  //�����سߴ绹��ʵ�ʳߴ磬0���أ�1ʵ��
	double pixSize;            //�����ߴ�С

							   //覴ö���
	double badArea_min[MAX_Rec_Num + MAX_Cir_Num];
	double badArea_max[MAX_Rec_Num + MAX_Cir_Num];
	int black_bad_detect[MAX_Rec_Num + MAX_Cir_Num];
	int white_bad_detect[MAX_Rec_Num + MAX_Cir_Num];
	double badLengthMin[MAX_Rec_Num + MAX_Cir_Num];
	double badLengthMax[MAX_Rec_Num + MAX_Cir_Num];
	double badWidthMax[MAX_Rec_Num + MAX_Cir_Num];
	double badWidthMin[MAX_Rec_Num + MAX_Cir_Num];
	//int IsFixThre[MAX_Rec_Num+MAX_Cir_Num];    //�Ƿ�ʹ�ù̶���ֵ
	int fixThreMin[MAX_Rec_Num + MAX_Cir_Num];
	int fixThreMax[MAX_Rec_Num + MAX_Cir_Num];
	int IsGauss[MAX_Rec_Num + MAX_Cir_Num];      //�Ƿ�Դ����ԭͼ���и�˹�˲�
	int GaussSize[MAX_Rec_Num + MAX_Cir_Num];    //��˹�˲��˵Ĵ�С

	int closeVal[MAX_Rec_Num + MAX_Cir_Num];
	int openVal[MAX_Rec_Num + MAX_Cir_Num];

	int meanSize[MAX_Rec_Num + MAX_Cir_Num];   //����ƽ������ʱ���
											   //int IsMeanSize[MAX_Rec_Num+MAX_Cir_Num]; //�Ƿ����ƽ������
	int IsFillOut[MAX_Rec_Num + MAX_Cir_Num];  //����ƽ��������ʱ���ⲿ�Ƿ����
											   //int IsDetHole[MAX_Rec_Num+MAX_Cir_Num];
	int holeAreaMin[MAX_Rec_Num + MAX_Cir_Num];
	int holeAreaMax[MAX_Rec_Num + MAX_Cir_Num];
	int holeNumMin[MAX_Rec_Num + MAX_Cir_Num];
	int holeNumMax[MAX_Rec_Num + MAX_Cir_Num];
	//int IsRagLoss[MAX_Rec_Num+MAX_Cir_Num];  //�Ƿ���ë�̺�ȱʧ��ë�̶�Ӧ��������ȱʧ��Ӧ�ղ���
	int minRag[MAX_Rec_Num + MAX_Cir_Num];     //����ë��ʱ��������ֵ
	int minLoss[MAX_Rec_Num + MAX_Cir_Num];    //����ȱʧʱ��ıղ�����ֵ
	int minRagLossArea[MAX_Rec_Num + MAX_Cir_Num]; //��������ë��ȱʧ����Сֵ


												   //��Χ
	int single_badArea_valid[MAX_Rec_Num + MAX_Cir_Num];
	double single_badArea_min[MAX_Rec_Num + MAX_Cir_Num];
	double single_badArea_max[MAX_Rec_Num + MAX_Cir_Num];
	int total_badArea_valid[MAX_Rec_Num + MAX_Cir_Num];
	double total_badArea_min[MAX_Rec_Num + MAX_Cir_Num];
	double total_badArea_max[MAX_Rec_Num + MAX_Cir_Num];
	int area_width_valid[MAX_Rec_Num + MAX_Cir_Num];
	double area_width_min[MAX_Rec_Num + MAX_Cir_Num];
	double area_width_max[MAX_Rec_Num + MAX_Cir_Num];
	int area_length_valid[MAX_Rec_Num + MAX_Cir_Num];
	double area_length_min[MAX_Rec_Num + MAX_Cir_Num];
	double area_length_max[MAX_Rec_Num + MAX_Cir_Num];

	int area_count_valid[MAX_Rec_Num + MAX_Cir_Num];
	int area_count_min[MAX_Rec_Num + MAX_Cir_Num];
	int area_count_max[MAX_Rec_Num + MAX_Cir_Num];

	RRECReg RecRoi[MAX_Rec_Num];    //�洢�ľ�������
	DRECReg DeleteRec[MAX_Delete_Rec_Num];    //�洢�ľ����ų�����

	RCirReg CirRoi[MAX_Cir_Num];       //�洢��Բ������
	DCirReg DeleteCir[MAX_Delete_Cir_Num];    //�洢��Բ���ų�����

	int RecRoiNum;             //����roi������Ŀ
	int RecDeleteNum;          //�����ų�������Ŀ
	int CirRoiNum;             //Բ��roi������Ŀ
	int CirDeleteNum;          //Բ���ų�������Ŀ
	int currentRoi;            //��ǰ��ʾ��ROIָ��
	int IsDirPattern;          //�Ƿ���ü������򣨲�����һ���������ȡ����ǰ�洫������ָ����
	int IsDirDelete;           //�ڲ��ü����������ʱ���ų�������һ��һ����Ӧ���Ի�����������ȫ���ų�,0�����ų���1ȫ���ų�
	int IsReferPtoDel;         //�Ƿ����òο���������Ϊ�ų�����
	int delPointZoom;          //���òο������������ 

	typedef enum { averageThre = 0, fixedThre, meanThre, detHole, ragAndLoss }TestType;    //�������
	TestType testType[MAX_Rec_Num + MAX_Cir_Num];   //ÿ��ROI�ļ������
	int tmptype[MAX_Rec_Num + MAX_Cir_Num];   //�����洢ö��ֵ��

	bool IsDispROI;
private:
	void iniData();
};

