// lantern.cpp : 定义 DLL 应用程序的入口点。
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

HMODULE hMod;
int *pHmod;
char msg[]="Patched by JuncoJet";
char app[]="PATCH";
char file[]="lantern.ini";
char filepath[MAX_PATH];
char c[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
char *v[]={"3.7.6","4.3.2","4.4.0"};
enum {IDWIDTH=8,BUFFSIZE=2048,VERSIZE=10,VSIZE=3};
enum {PNONE=0,PALL=-1,PDEV=2,PUSR=4,PDBG=8,PVER=16};
typedef struct _LTVER{
	DWORD addr;
	string str;
	string newstr;
}LTVER;
LTVER ltvv[]={
	{0x8B6B30,"","4.4.2"},
	{0x8B9420,"","20171222.161751"},
	{0x8B9430,"","20171222.161626"},
};
void dbgx(char *x,void *y){
	char z[200];
	sprintf(z,"%s 0x%08X",x,y);
	OutputDebugString(z);
}
template <class T>
void dbg(T x){
	stringstream z;
	z<<"ul: "<<x;
	OutputDebugString(z.str().data());
}
template <class I,class T>
void dbg(I x,T y){
	stringstream z;
	z<<x<<" "<<y;
	OutputDebugString(z.str().data());
}
DWORD  WINAPI ThreadProc(LPVOID lpParam){
	char ver[VERSIZE];
	GetModuleFileName(hMod,filepath,MAX_PATH);
	for(int i=strlen(filepath);i>0;i--){
		if(filepath[i]=='\\'){
			filepath[i+1]='\0';
			strcat(filepath,file);
			break;
		}
	}
	int enable=GetPrivateProfileInt(app,"ENABLE",1,filepath);
	dbg("ul: enable",enable);
	if(!enable)
		return 0;
	else if(enable==1)
		enable=-1;
	GetPrivateProfileString(app,"VERSION",v[VSIZE-1],ver,VERSIZE,filepath);
	if(enable&PDBG)
		dbg("ul: version",ver);
	if(!strlen(ver))
		return 0;//配置文件中没有版本号退出线程
	char *cc=(char*)malloc(BUFFSIZE);
	int method=GetPrivateProfileInt(app,"METHOD",3,filepath);
	int start=GetPrivateProfileInt(app,"STARTWAIT",3,filepath);
	int reset=GetPrivateProfileInt(app,"INTERVAL",480,filepath);
	GetPrivateProfileString(app,"PATTERN",c,cc,BUFFSIZE,filepath);
	if(enable&PDBG){
		dbg("ul: method",method);
		dbg("ul: start",start);
		dbg("ul: reset",reset);
		dbg("ul: pattern",cc);
	}
	int r=-1;
	for(int i=0;i<VSIZE;i++){
		if(!strcmp(v[i],ver)){
			r=i;
			break;
		}
	}
	if(enable&PDBG)
		dbg("ul: ver_r",r);
	if(r<=-1)
		return 0;//找不到适合的版本号退出线程
	if(r==2&&enable&PVER)
		for(int i=0;i<3;i++){
			char pv[20];
			stringstream pvn;
			pvn<<"PV"<<i;
			GetPrivateProfileString(app,pvn.str().data(),ltvv[i].newstr.data(),pv,20,filepath);
			ltvv[i].newstr=pv;
			ltvv[i].addr+=(int)hMod;
			ltvv[i].str=(char*)(ltvv[i].addr);
			if(enable&PDBG){
				stringstream inf;
				inf<<"ltvv["<<i<<"].str "<<ltvv[i].str;
				dbg(inf.str().data());
			}
			SIZE_T s;
			WriteProcessMemory((HANDLE)-1,(char*)(ltvv[i].addr),ltvv[i].newstr.data(),ltvv[i].newstr.size(),&s);
			if(enable&PDBG){
				stringstream inf;
				inf<<"ul: ltvv["<<i<<"].str";
				dbgx((char*)(inf.str().data()),(void*)ltvv[i].addr);
			}
		}
	if(!pHmod)
		Sleep(1000*start);
	while(1){
		switch(r){
			case 0:
				//version 3.7.6
				pHmod=(int*)((int)hMod+0x00EAD00C);
				pHmod=(int*)(*(int*)(*pHmod+0x338));
				break;
			case 1:
				//version 4.3.2
				pHmod=(int*)((int)hMod+0x00F9E6E8);
				pHmod=(int*)(*(int*)(*pHmod+0x240));
				break;
			case 2:
				//version 4.4.0
				pHmod=(int*)((int)hMod+0x00FB46C4);
				pHmod=(int*)(*(int*)(*pHmod+0x240));
				if(enable&PDBG){
					dbgx("ul: pHomd",pHmod);
					if(enable&PUSR)
						dbg("ul: USRID",pHmod[2]);
				}
				if(enable&PUSR)
					pHmod[2]++;
				break;
		}
		if(enable&PDBG&&r!=2){
			dbgx("ul: pHmod",pHmod);
		}
		char *pChar=(char*)pHmod;//通用指针
		if(enable&PDEV){
			if(enable&PDBG){
				char x[10];
				memcpy(x,pChar,IDWIDTH);
				x[IDWIDTH]='\0';
				dbg("ul: DEVID",x);
			}
			switch(method){
				case 0:
				case 3:
					srand(GetTickCount());
					for(int i=0;i<IDWIDTH;i++){
						pChar[i]=cc[rand()%strlen(cc)];
					}
					break;
				case 4:
					srand(GetTickCount());
					for(int i=0;i<IDWIDTH;i++){
						pChar[i]=rand()%255+1;
					}
					break;
				case 5:
					srand(GetTickCount());
					((uint64_t*)pHmod)[0]=rand()%-1;
					for(int i=0;i<IDWIDTH;i++){
						if(!pChar[i])
							pChar[i]++;
					}
					break;
				case 2:
					((uint64_t*)pHmod)[0]+=GetTickCount();
					for(int i=0;i<IDWIDTH;i++){
						if(!pChar[i])
							pChar[i]++;
					}
					break;
				case 1:
					((uint64_t*)pHmod)[0]++;
					for(int i=0;i<IDWIDTH;i++){
						if(!pChar[i])
							pChar[i]++;
					}
					break;
			}
		}
		Sleep(1000*reset);
	}
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	if(ul_reason_for_call==DLL_PROCESS_ATTACH){
		hMod=GetModuleHandle(NULL);
		CreateThread(NULL,0,ThreadProc,NULL,0,NULL);  
	}
    return TRUE;
}

void patch(){
	MessageBox(0,msg,msg,0);
}
