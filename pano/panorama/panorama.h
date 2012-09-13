#ifdef DLL_API 
#else 
#define DLL_API extern "C" __declspec(dllimport)
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
	int nPic1,nPic2;   //这对点是哪两幅图的匹配点
	int x1,y1;        //nPic1图上的匹配点的原始像素坐标
	int x2,y2;        //nPic2图上的匹配点的原始像素坐标
};


DLL_API int GenPano(char* aSrcPics[], //2个或多个鱼眼源图像文件名(带路径)
			int nPicNum,    //源图像的个数
			char* sDestPic,  //给生成的全景图像起的名字
			int nDestWidth,  //生成的图像宽度，nHeight = nWidth/2
struct TPatchPntInfo * pPatPtInfo, //匹配点集
	int nPatchPntsNum,  //匹配点对的个数
	int nCenterPixX=-1,int nCenterPixY=-1, //鱼眼图像圆的中心像素坐标. 
	int nRadius=-1,  //鱼眼图像圆的半径-1则是不给定又程序检测，上同
	bool bDisplay=true);//拼好后是否用OpenCV显示
//返回值，成功返回0，不成功返回负数，代表出错的原因


DLL_API int GenCubeFromSphere(char* sSphereFilePathName,   //球形全景的文件名
					  char* sCubeFilenPathPreName, //生成的方形全景文件名前缀，函数
					  //执行后产生6张图，命名为该文件名加上_top,
					  //_bottom, _front, _back, _left, _right
					  int nCubePicWidth,   //方形图的边长尺寸
					  bool bDisplay=true);  //拼好后是否用OpenCV显示


DLL_API int GenSphereFromCube (char* sCubeTopFilePathName,   //方形全景的文件名
					   char* sCubeBottomFilePathName,
					   char* sCubeFrontFilePathName,
					   char* sCubeBackFilePathName,
					   char* sCubeLeftFilePathName,
					   char* sCubeRightFilePathName,
					   char* sSphereFilenPathName, //生成的球形全景文件名
					   int nSpherePicWidth,   //球形图的宽度，height=width/2
					   bool bDisplay=true);   //拼好后是否用OpenCV显示