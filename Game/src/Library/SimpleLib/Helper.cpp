#include "SimpleLib.h"

using namespace SimpleLib;

namespace SimpleLib{

DWORD FpsProc()
{
	static DWORD fps,Nowfps,BaseTime,SaveTime;
	SaveTime = timeGetTime();
	fps++;
	if((SaveTime - BaseTime) >= 1000){
		BaseTime = timeGetTime();
		Nowfps = fps;
		fps = 0;
	}

	return Nowfps;
}

void AddLogFile(char *LogFileName,char *str, bool bData)
{
	time_t jikoku;
	struct tm *lt;
	time(&jikoku);              // Žž‚ðŽæ“¾‚µ
	lt = localtime(&jikoku);    // Œ»’nŽžŠÔ‚Ì\‘¢‘Ì‚É•ÏŠ·‚·‚é

	char s[256];
	FILE *fp;
	fp = fopen(LogFileName,"at");
	if(fp){
		if(bData){
			sprintf(s,"[%d/%d/%d %d:%d:%d]%s\n",lt->tm_year+1900,lt->tm_mon +1,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec,str);
		}
		else{
			sprintf(s,"%s\n",str);
		}
		fputs(s,fp);
		fclose(fp);
	}
}

static int		g_KeyFlag[256];

bool KeyCheck(int keyCode){
	if(GetAsyncKeyState(keyCode) & 0x8000){
		if(g_KeyFlag[keyCode] == 0){
			g_KeyFlag[keyCode] = 1;
			return true;
		}
		else{
			return false;
		}
	}
	else{
		g_KeyFlag[keyCode] = 0;
	}
	return false;
}

}
