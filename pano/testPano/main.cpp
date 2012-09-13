#include "../pano/panorama.h"
#include <iostream>

int oldmain(){

	//Sleep(2000);
	//WaitForSingleObject(hMutex,INFINITE);
	//ReleaseMutex(hMutex);
	/////////test two image
	//char *imgName[2];
	//char *i1="front.jpg";
	//char *i2="back.jpg";
	//imgName[0]=i1;
	//imgName[1]=i2;
	//int pNum=2;
	//char *dest="pano.jpg";
	//int width=1000;
	//struct TPatchPntInfo pPatPtInfo[3]={
	//	TPatchPntInfo(1,2,220,1256,521,544),
	//	TPatchPntInfo(1,2,96,1469,1007,999),
	//	TPatchPntInfo(1,2,264,1255,1462,1454)
	//};
	//int ptNum=3;
	///////////////////////////////////
	//char *imgName[3];
	//char *i1="park1.jpg";
	//char *i2="park2.jpg";
	//char *i3="park3.jpg";
	//imgName[0]=i1;
	//imgName[1]=i2;
	//imgName[2]=i3;
	//int pNum=3;
	//char *dest="pano.jpg";
	//int width=1000;
	//struct TPatchPntInfo pPatPtInfo[9]={
	//	TPatchPntInfo(1,2,984,314,651,539),
	//	TPatchPntInfo(1,2,1188,258,935,919),
	//	TPatchPntInfo(1,2,1375,515,1284,1216),
	//	TPatchPntInfo(2,3,1259,546,587,655),
	//	TPatchPntInfo(2,3,1291,349,964,955),
	//	TPatchPntInfo(2,3,1413,519,1231,1166),
	//	TPatchPntInfo(3,1,1070,224,806,742),
	//	TPatchPntInfo(3,1,1029,118,954,924),
	//	TPatchPntInfo(3,1,1176,410,1376,1365)
	//};
	//int ptNum=9;
	///////////////////////////////////////

	char *imgName[2];
	char *i1="front.jpg";
	char *i2="back.jpg";
	imgName[0]=i1;
	imgName[1]=i2;
	int pNum=2;
	char *dest="panod.jpg";
	int width=1000;
	struct TPatchPntInfo pPatPtInfo[3]={
		//TPatchPntInfo(1,2,1230,478,285,492),     //1   //这个不知为何加入了会死
		TPatchPntInfo(1,2,247,504,1242,490),       //2
		TPatchPntInfo(1,2,92,1003,1461,995),        //3
		//TPatchPntInfo(1,2,1359,1329,175,1331),    //4
		TPatchPntInfo(1,2,227,1440,1271,1414),     //5
		//TPatchPntInfo(2,1,1220,478,286,481),      //6
		//TPatchPntInfo(2,1,1390,736,108,740),        //7
		//TPatchPntInfo(2,1,1434,940,67,950),         //8
		//TPatchPntInfo(2,1,1331,1331,165,1358)  ,   //9
		//TPatchPntInfo(2,1,1407,1172,99,1185)        //10
	};

	int ptNum=3;

	GenPano(imgName,pNum,dest,width,pPatPtInfo,ptNum);
	//GenCubeFromSphere("pano.jpg", "pre" ,1000);
	//GenSphereFromCube("pre_top.jpg"
	//	,"pre_bottom.jpg"
	//	,"pre_front.jpg"
	//	,"pre_back.jpg"
	//	,"pre_left.jpg"
	//	,"pre_right.jpg"
	//	,"cube2sphere.jpg"
	//	,1000);
	//GenSphereFromCube("./sphere/top.jpg"
	//	,"./sphere/bottom.jpg"
	//	,"./sphere/back.jpg"
	//	,"./sphere/front.jpg"
	//	,"./sphere/left.jpg"
	//	,"./sphere/right.jpg"
	//	,"./sphere/pano1.jpg"
	//	,500);
	return 1;
}

int main(){
	std::cout<<"hello"<<std::endl;
	return 0;
}


