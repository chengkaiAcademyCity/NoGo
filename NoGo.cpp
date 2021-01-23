/*不围棋NoGo
作者：程凯
编制时间：2020年1月
*/
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cstring>
#include <graphics.h>
#include<mmsystem.h>
#include <vector>
#pragma comment(lib,"winmm.lib")
using namespace std;
MOUSEMSG msg;//鼠标信息
IMAGE menu;//菜单地图
IMAGE background;//背景底图
IMAGE board;//棋盘底图
IMAGE blackpiece;//黑棋图片
IMAGE whitepiece;//白棋图片
int maxDepth = 6;//最大搜索层数
int NOGO[81];//存储棋盘状态
int myColor;//我方棋子颜色
bool dfsAirVisit[81];//判断求气是否重复
const int cseq[] = { -9,-1,1,9 };//上下左右移动
int avaiList[81];//可行步
int valueList[81];//各点估值
int maxValue[81];//最大估值点
int chooseMode()//开始游戏，选择模式界面
{
	initgraph(800, 600);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	setlinecolor(BLACK);
	loadimage(&menu, L"./menu.jpg", 800, 600);
	putimage(0, 0, &menu);
	rectangle(300, 275, 500, 325);
	rectangle(300, 375, 500, 425);
	rectangle(350, 475, 450, 525);
	settextstyle(100, 0, L"黑体");
	outtextxy(250, 50, L"不围棋");
	settextstyle(100, 0, L"Times New Roman");
	outtextxy(300, 150, L"NoGo");
	settextstyle(50, 0, L"楷体");
	outtextxy(300, 275, L"从头开始");
	outtextxy(300, 375, L"继续游戏");
	outtextxy(350, 475, L"关于");
	while (1)
	{
		msg = GetMouseMsg();
		if (msg.mkLButton)
		{
			if (msg.x >= 300 && msg.y >= 275 && msg.x < 500 && msg.y < 325)
				return 1;
			else if (msg.x >= 300 && msg.y >= 375 && msg.x < 500 && msg.y < 425)
				return 2;
			else if (msg.x >= 350 && msg.y >= 475 && msg.x < 450 && msg.y < 525)
				return 3;
		}
		FlushMouseMsgBuffer();
	}
}
void initGame()//初始化游戏界面并选择是否播放音乐
{
	HWND hnd = GetHWnd();
	SetWindowText(hnd, L"不围棋NoGo");//改变窗口句柄
	int isok = MessageBox(hnd, L"是否播放背景音乐？", L"Music", MB_OKCANCEL);//选择是否播放音乐
	if (isok == IDOK)
	{
		PlaySound(L"./ALPHAGO.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	}
	else if (isok == IDCANCEL);
	initgraph(800, 600);
	setbkmode(TRANSPARENT);
	settextcolor(WHITE);
	setlinecolor(WHITE);
	loadimage(&background, L"./background.jpg", 800, 600);
	loadimage(&board, L"./board.jpg", 600, 600);
	loadimage(&blackpiece, L"./blackpiecetest.jpg", 60, 60);
	loadimage(&whitepiece, L"./whitepiecetest.jpg", 60, 60);
	putimage(0, 0, &background);
	putimage(0, 0, &board);
	rectangle(650, 100, 750, 150);
	rectangle(650, 200, 750, 250);
	rectangle(650, 300, 750, 350);
	rectangle(650, 400, 750, 450);
	settextstyle(50, 0, L"楷体");
	outtextxy(650, 100, L"存档");
	outtextxy(650, 200, L"读档");
	outtextxy(650, 300, L"重来");
	outtextxy(650, 400, L"退出");
	HWND blackorwhite = GetHWnd();
	SetWindowText(blackorwhite, L"不围棋NoGo");
	int tochoosecolor = MessageBox(blackorwhite, L"是否选择执黑？", L"选择棋色", MB_YESNO);
	if (tochoosecolor == IDYES)
	{
		myColor = -1;
	}
	else if (tochoosecolor == IDNO)
	{
		myColor = 1;
		srand((int)time(0));
		int seq = 0;
		seq = (rand() % (81 - 0) + 0);
		int i = seq / 9;
		int j = seq % 9;
		putimage(60 * j + 30, 60 * i + 30, &blackpiece);
		NOGO[seq] = -1;
	}
}
bool dfsAir(int NOGO[81], int fseq)//判断某处是否有气
{
	dfsAirVisit[fseq] = true;
	bool flag = false;
	for (int dir = 0; dir < 4; dir++)
	{
		if (fseq % 9 == 0 && dir == 1)continue;//第一列不向左寻气
		if (fseq % 9 == 8 && dir == 2)continue;//最后一列不向右寻气
		int dseq = fseq + cseq[dir];//移动
		if (dseq >= 0 && dseq <= 80)//有意义的邻步
		{
			if (NOGO[dseq] == 0)//空处即有气
				flag = true;
			if (NOGO[dseq] == NOGO[fseq] && !dfsAirVisit[dseq])//同色相邻棋子
				if (dfsAir(NOGO, dseq))//递归寻气
					flag = true;
		}
	}
	return flag;
}
bool finishJudge()//判断游戏是否结束
{
	for (int i = 0; i <= 80; i++)
	{
		memset(dfsAirVisit, false, sizeof(dfsAirVisit));
		if (NOGO[i]!=0&&(!dfsAir(NOGO,i)))//某处棋子无气则游戏结束
			return true;
	}
	return false;
}
bool avaiJudge(int fseq, int color)//判断某处可否落子
{
	if (NOGO[fseq] != 0)return false;//如果该点已有棋子，则不能落子 
	NOGO[fseq] = color;//假设落子于此处 
	memset(dfsAirVisit, false, sizeof(dfsAirVisit));//访问记录归零
	if (!dfsAir(NOGO, fseq))//无气不能落子
	{
		NOGO[fseq] = 0;
		return false;
	}
	for (int dir = 0; dir < 4; dir++)//考察该点上下左右四点是否有气 
	{
		int dseq = fseq + cseq[dir];
		if (dseq >= 0 && dseq <= 80)//在棋盘内 
		{
			if (NOGO[dseq] != 0 && !dfsAirVisit[dseq])//有棋子
			{
				if (!dfsAir(NOGO, dseq))//落子后导致无气 
				{
					NOGO[fseq] = 0;
					return false;
				}
			}
		}
	}
	NOGO[fseq] = 0;
	return true;
}
int calValue(int color)//场面估值
{
	int val = 0;
	int opcolour = -color;
	for (int fseq = 0; fseq <= 80; fseq++)//遍历
	{
		if (avaiJudge(fseq, color))val++;//己方可下
		if (avaiJudge(fseq, opcolour))val--;//对方可下
	}
	return val;
}
int search(int color, int depth)//步法搜索
{
	int max = -2147483648;
	bool flag = false;
	if (depth == maxDepth)//达到指定搜索层数
	{
		return calValue(-color);
	}
	else
	{
		for (int fseq = 0; fseq <= 80; fseq++)
		{

			{
				if (!avaiJudge(fseq, -color))continue;//不可下
				else
				{
					flag = true;
					NOGO[fseq] = -color;
					int val = search(-color, depth + 1);//搜下一层
					if (val > max)max = val;//更优估值
					NOGO[fseq] = 0;
				}
			}
		}
	}
	if (!flag)return 2147483647;//都不可下
	else return -max;
}
bool sameColor(int fseq, int color)//判断四周是否有同色棋子
{
	for (int dir = 0; dir < 4; dir++)
	{
		int dseq = fseq + cseq[dir];
		if (NOGO[dseq] == color)
			return true;
	}
	return false;
}
int botAI(int NOGO[81])//决策
{
	int color = -myColor;
	int totAvai = 0;
	int maxVal = -2147483647;
	int maxNum = 0;
	for (int fseq = 0; fseq <= 80; fseq++)
	{
		if (avaiJudge(fseq, color))//可行步
		{
			avaiList[totAvai++] = fseq;
			NOGO[fseq] = color;
			valueList[fseq] = calValue(color);
			if (sameColor(fseq, color))//避免下在同色棋子周围
				valueList[fseq]--;
			if (valueList[fseq] > maxVal)//更优步
				maxVal = valueList[fseq];
			NOGO[fseq] = 0;
		}
		else
			valueList[fseq] = -2147483648;
	}
	for (int fseq = 0; fseq <= 80; fseq++)
		if (valueList[fseq] == maxVal)
		{
			maxValue[maxNum] = fseq;
			maxNum++;
		}
	int pos = rand() % maxNum;//从最高估值点中随机选取
	int bestAction = maxValue[pos];
	return bestAction;
}
bool botResponse()//计算机回应
{
	srand((unsigned)time(0));
	memset(avaiList, 0, sizeof(avaiList));//可行步归零
	memset(valueList, 0, sizeof(valueList));//估值归零
	int bestAction = botAI(NOGO);//生成回应
	int i = bestAction / 9;
	int j = bestAction % 9;
	if (myColor>0)
	{
		putimage(60 * j + 30, 60 * i + 30, &blackpiece);
		NOGO[bestAction] = -1;
	}
	else
	{
		putimage(60 * j + 30, 60 * i + 30, &whitepiece);
		NOGO[bestAction] = 1;
	}
	if (finishJudge())return true;
	else return false;
}
void playNoGo()//游戏界面
{
	while (1)
	{
		msg = GetMouseMsg();
		if(msg.mkLButton)
		{
			int i = (msg.x + 30) / 60 * 60 - 30;
			int j = (msg.y + 30) / 60 * 60 - 30;
			int seq = (i / 60) + (j / 60) * 9;
			if (msg.x >= 30 && msg.x <= 570 && msg.y >= 30 && msg.y <= 570 && NOGO[seq] == 0)//落子在棋盘内
			{

				if (myColor < 0)//判断玩家所执棋色
				{
					putimage(i, j, &blackpiece);
					NOGO[seq] = -1;
				}
				else if (myColor > 0)
				{
					putimage(i, j, &whitepiece);
					NOGO[seq] = 1;
				}
				if (finishJudge())//判断是否终局
				{
					HWND finish = GetHWnd();
					SetWindowText(finish, L"不围棋NoGo");
					int check = MessageBox(finish, L"很遗憾你输了，再接再厉吧！", L"对局失败", MB_OK);
					return;
				}
				if (botResponse())//判断计算机回应后是否终局
				{
					HWND finish = GetHWnd();
					SetWindowText(finish, L"不围棋NoGo");
					int check = MessageBox(finish, L"恭喜，你赢了！", L"对局获胜", MB_OK);
					return;
				}
			}
			else if (msg.x >= 650 && msg.x <= 750 && msg.y >= 400 && msg.y <= 450)//选择退出
			{
				HWND exit = GetHWnd();
				int toexit = MessageBox(exit, L"请确认存档后退出，未存档的对局进度将会丢失", L"确认退出", MB_OKCANCEL);
				if (toexit == IDOK)
				{
					return;
				}
				else if (toexit == IDCANCEL);
			}
			else if (msg.x >= 650 && msg.x <= 750 && msg.y >= 100 && msg.y <= 150)//选择存档
			{
				char str[100] = { '\0' };
				sprintf(str, "del memory.txt");
				system(str);
				FILE* mem;
				if ((mem = fopen("memory.txt", "w+")) == NULL)
				{
					HWND save = GetHWnd();
					MessageBoxA(save, "存档失败", "不围棋NoGo", MB_OK || MB_SYSTEMMODAL);
				}
				else
				{
					for (int i = 0; i <= 80; i++)
						fwrite(&NOGO[i], sizeof(NOGO[i]), 1, mem);
					fwrite(&myColor, sizeof(myColor), 1, mem);
					fclose(mem);
					HWND save = GetHWnd();
					MessageBoxA(save, "存档完成！", "不围棋NoGo", MB_OK || MB_SYSTEMMODAL);
				}
			}
			else if (msg.x >= 650 && msg.x <= 750 && msg.y >= 200 && msg.y <= 250)//选择读档
			{
				FILE* mem;
				if ((mem = fopen("memory.txt", "r")) == NULL)
				{
					HWND save = GetHWnd();
					MessageBoxA(save, "存档为空", "不围棋NoGo", MB_OK || MB_SYSTEMMODAL);
				}
				else
				{
					for (int i = 0; i <= 80; i++)
						fread(&NOGO[i], sizeof(NOGO[i]), 1, mem);
					fread(&myColor, sizeof(myColor), 1, mem);
					fclose(mem);
					HWND save = GetHWnd();
					MessageBoxA(save, "读档完成！", "不围棋NoGo", MB_OK || MB_SYSTEMMODAL);
					putimage(0, 0, &board);
					for (int iseq = 0; iseq <= 80; iseq++)
					{
						int i = iseq / 9;
						int j = iseq % 9;
						if(NOGO[iseq]==-1)putimage(60 * j + 30, 60 * i + 30, &blackpiece);
						else if (NOGO[iseq] == 1)putimage(60 * j + 30, 60 * i + 30, &whitepiece);
					}
				}
			}
			else if (msg.x >= 650 && msg.x <= 750 && msg.y >= 300 && msg.y <= 350)//选择重来
			{
				HWND again = GetHWnd();
				int toagain = MessageBox(again, L"是否重新开始对局？未存档的对局进度将会丢失", L"重新开始", MB_OKCANCEL);
				if (toagain == IDOK)
				{
					memset(NOGO, 0, sizeof(NOGO));
					putimage(0, 0, &board);
				if (myColor == 1)//玩家执白，计算机随机下第一个黑子
				{
					srand((int)time(0));
					int seq = 0;
					seq = (rand() % (81 - 0) + 0);
					int i = seq / 9;
					int j = seq % 9;
					putimage(60 * j + 30, 60 * i + 30, &blackpiece);
					NOGO[seq] = -1;
				}
				}
				else if (toagain == IDCANCEL);
			}
		}
	}
}
void continueGame()//从存档继续游戏
{
	HWND hnd = GetHWnd();
	SetWindowText(hnd, L"不围棋NoGo");//改变窗口句柄
	int isok = MessageBox(hnd, L"是否播放背景音乐？", L"Music", MB_OKCANCEL);
	if (isok == IDOK)
	{
		PlaySound(L"./ALPHAGO.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	}
	else if (isok == IDCANCEL);
	initgraph(800, 600);
	setbkmode(TRANSPARENT);
	settextcolor(WHITE);
	setlinecolor(WHITE);
	loadimage(&background, L"./background.jpg", 800, 600);
	loadimage(&board, L"./board.jpg", 600, 600);
	loadimage(&blackpiece, L"./blackpiecetest.jpg", 60, 60);
	loadimage(&whitepiece, L"./whitepiecetest.jpg", 60, 60);
	putimage(0, 0, &background);
	putimage(0, 0, &board);
	rectangle(650, 100, 750, 150);
	rectangle(650, 200, 750, 250);
	rectangle(650, 300, 750, 350);
	rectangle(650, 400, 750, 450);
	settextstyle(50, 0, L"楷体");
	outtextxy(650, 100, L"存档");
	outtextxy(650, 200, L"读档");
	outtextxy(650, 300, L"重来");
	outtextxy(650, 400, L"退出");
	FILE* mem;
	if ((mem = fopen("memory.txt", "r")) == NULL)
		MessageBoxA(hnd, "存档为空", "不围棋NoGo", MB_OK || MB_SYSTEMMODAL);
	else
	{
		for (int i = 0; i <= 80; i++)
			fread(&NOGO[i], sizeof(NOGO[i]), 1, mem);
		fread(&myColor, sizeof(myColor), 1, mem);
		fclose(mem);
		HWND save = GetHWnd();
		MessageBoxA(save, "读档完成！", "不围棋NoGo", MB_OK || MB_SYSTEMMODAL);
		for (int iseq = 0; iseq <= 80; iseq++)
		{
			int i = iseq / 9;
			int j = iseq % 9;
			if (NOGO[iseq] == -1)putimage(60 * j + 30, 60 * i + 30, &blackpiece);
			else if (NOGO[iseq] == 1)putimage(60 * j + 30, 60 * i + 30, &whitepiece);
		}
	}
}
void showAbout()//关于界面
{
	initgraph(800, 600);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	setlinecolor(BLACK);
	IMAGE backgroundabout;
	loadimage(&backgroundabout, L"./backgroundabout.jpg", 800, 600);
	putimage(0, 0, &backgroundabout);
	HWND hnd = GetHWnd();
	SetWindowText(hnd, L"关于");
	int isok = MessageBox(hnd, L"是否播放背景音乐？", L"音乐", MB_OKCANCEL);
	if (isok == IDOK)
	{
		PlaySound(L"./SMILE.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	}
	else if (isok == IDCANCEL);

	rectangle(5,5, 125, 35);
	rectangle(5,150, 65, 180);
	rectangle(5, 295, 125, 325);
	//rectangle(650, 400, 750, 450);*/
	settextstyle(30, 0, L"楷体");
	outtextxy(5, 5, L"游戏规则");
	settextstyle(20, 0, L"楷体");
	outtextxy(15, 40, L"NoGo(不围棋),由围棋衍生而来，属于两人零和完备信息博弈，规则与围棋相反。");
	outtextxy(15, 60, L"黑子先手，双方轮流落子，落子后棋子不可移动。");
	outtextxy(15, 80, L"如果一方落子后吃掉了对方的棋子，则落子一方判负。");
	outtextxy(15, 100, L"落子自杀一方判负，空手一方判负。");
	outtextxy(15, 120, L"无气状态的棋子不能在棋盘上存在。");
	settextstyle(30, 0, L"楷体");
	outtextxy(5, 150, L"致谢");
	settextstyle(20, 0, L"楷体");
	outtextxy(15, 185, L"本游戏的诞生离不开北京大学20级本科生宁川若、刘明昊、罗润冬等同学的帮助，");
	outtextxy(15, 205, L"和Botzone用户@冷冻章鱼、@JeremyDon、@Halifuda等用户的开源贡献，");
	outtextxy(15, 225, L"在此对他们一并表示衷心的感谢！");
	outtextxy(15, 245, L"本游戏开发过程中主要的参考文献有：");
	outtextxy(15, 265, L"郭倩宇,陈优广.基于价值评估的不围棋递归算法[J].华东师范大学学报(自然科学版)");
	settextstyle(30, 0, L"楷体");
	outtextxy(5, 295, L"背景音乐");
	settextstyle(20, 0, L"楷体");
	outtextxy(15, 330, L"主页面背景音乐：AlphaGo BGM Music, China Background Music");
	outtextxy(15, 350, L"关于页面背景音乐：Smile -You&Me-, ELISA ");
	settextstyle(20, 0, L"黑体");
	outtextxy(320, 500, L"按任意键以退出");
	getchar();
}
int main()
{
	int choice=chooseMode();//选择模式
	if (choice == 1)//从头开始
	{
		initGame();
		playNoGo();
	}
	else if (choice == 2)//继续游戏
	{
		continueGame();
		playNoGo();
	}
	else if (choice == 3)//关于界面
	{
		showAbout();
	}
	return 0;
}