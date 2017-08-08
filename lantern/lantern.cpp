// lantern.cpp : 定义 DLL 应用程序的入口点。
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

HMODULE hMod;
int *pHmod;
char msg[]="Patched by JuncoJet";
char app[]="PATCH";
char file[]="lantern.ini";
char c[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
enum def{IDWIDTH=8,BUFFSIZE=2048};

DWORD  WINAPI ThreadProc(LPVOID lpParam){
	char *cc=(char*)malloc(BUFFSIZE);
	int method=GetPrivateProfileInt(app,"METHOD",3,file);
	int start=GetPrivateProfileInt(app,"STARTWAIT",3,file);
	int reset=GetPrivateProfileInt(app,"INTERVAL",480,file);
	GetPrivateProfileString(app,"PATTERN",c,cc,BUFFSIZE,file);
	if(!pHmod)
		Sleep(1000*start);
	while(1){
		pHmod=(int*)((int)hMod+0x00EAD00C);
		pHmod=(int*)(*(int*)(*pHmod+0x338));
		char *pChar=(char*)pHmod;
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
