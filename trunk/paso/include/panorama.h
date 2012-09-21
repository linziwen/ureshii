#ifdef DLL_API 
#else 
	#ifdef __gnu_linux__
		#define DLL_API
	#else
		#define DLL_API extern "C" __declspec(dllimport)
	#endif
#endif 


//#include "util.h"

struct TPatchPntInfo {
public:
	TPatchPntInfo(int p1,int p2,int xx1,int xx2,int yy1,int yy2){
		nPic1=p1;
		nPic2=p2;
		x1=xx1;
		x2=xx2;
		y1=yy1;
		y2=yy2;
	};
	int nPic1,nPic2;   //��Ե���������ͼ��ƥ���
	int x1,y1;        //nPic1ͼ�ϵ�ƥ����ԭʼ�������
	int x2,y2;        //nPic2ͼ�ϵ�ƥ����ԭʼ�������
};


DLL_API int GenPano(char* aSrcPics[], //2����������Դͼ���ļ���(��·��)
			int nPicNum,    //Դͼ��ĸ���
			char* sDestPic,  //����ɵ�ȫ��ͼ���������
			int nDestWidth,  //��ɵ�ͼ���ȣ�nHeight = nWidth/2
struct TPatchPntInfo * pPatPtInfo, //ƥ��㼯
	int nPatchPntsNum,  //ƥ���Եĸ���
	int nCenterPixX=-1,int nCenterPixY=-1, //����ͼ��Բ�������������. 
	int nRadius=-1,  //����ͼ��Բ�İ뾶-1���ǲ����ֳ����⣬��ͬ
	bool bDisplay=true);//ƴ�ú��Ƿ���OpenCV��ʾ
//����ֵ���ɹ�����0�����ɹ����ظ���������ԭ��


DLL_API int GenCubeFromSphere(char* sSphereFilePathName,   //����ȫ�����ļ���
					  char* sCubeFilenPathPreName, //��ɵķ���ȫ���ļ���ǰ׺������
					  //ִ�к����6��ͼ������Ϊ���ļ������_top,
					  //_bottom, _front, _back, _left, _right
					  int nCubePicWidth,   //����ͼ�ı߳��ߴ�
					  bool bDisplay=true);  //ƴ�ú��Ƿ���OpenCV��ʾ


DLL_API int GenSphereFromCube (char* sCubeTopFilePathName,   //����ȫ�����ļ���
					   char* sCubeBottomFilePathName,
					   char* sCubeFrontFilePathName,
					   char* sCubeBackFilePathName,
					   char* sCubeLeftFilePathName,
					   char* sCubeRightFilePathName,
					   char* sSphereFilenPathName, //��ɵ�����ȫ���ļ���
					   int nSpherePicWidth,   //����ͼ�Ŀ�ȣ�height=width/2
					   bool bDisplay=true);   //ƴ�ú��Ƿ���OpenCV��ʾ
