// lantern.cpp : 定义 DLL 应用程序的入口点。
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

HMODULE hMod;
int *pHmod;
char msg[]="Patched by JuncoJet";
char app[]="PATCH";
char file[]="lantern.ini";
char c[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

DWORD  WINAPI ThreadProc(LPVOID lpParam){
	char *cc=(char*)malloc(2048);
	int method=GetPrivateProfileInt(app,"METHOD",3,file);
	int start=GetPrivateProfileInt(app,"STARTWAIT",3,file);
	int reset=GetPrivateProfileInt(app,"INTERVAL",480,file);
	GetPrivateProfileString(app,"PATTERN",c,cc,sizeof cc,file);
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
				for(int i=0;i<4;i++){
					pChar[i]=cc[rand()%strlen(cc)];
				}
				break;
			case 4:
				srand(GetTickCount());
				for(int i=0;i<4;i++){
					pChar[i]=rand()%255+1;
				}
				break;
			case 5:
				srand(GetTickCount());
				pHmod[0]=rand()%-1;
				for(int i=0;i<4;i++){
					if(!pChar[i])
						pChar[i]++;
				}
				break;
			case 2:
				pHmod[0]+=GetTickCount();
				for(int i=0;i<4;i++){
					while(!pChar[i]){
						pChar[i]++;
					}
				}
				break;
			case 1:
				pHmod[0]++;
				for(int i=0;i<4;i++){
					while(pChar[i]==0){
						pChar[i]++;
					}
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
