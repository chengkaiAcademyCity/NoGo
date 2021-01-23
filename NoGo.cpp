/*��Χ��NoGo
���ߣ��̿�
����ʱ�䣺2020��1��
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
MOUSEMSG msg;//�����Ϣ
IMAGE menu;//�˵���ͼ
IMAGE background;//������ͼ
IMAGE board;//���̵�ͼ
IMAGE blackpiece;//����ͼƬ
IMAGE whitepiece;//����ͼƬ
int maxDepth = 6;//�����������
int NOGO[81];//�洢����״̬
int myColor;//�ҷ�������ɫ
bool dfsAirVisit[81];//�ж������Ƿ��ظ�
const int cseq[] = { -9,-1,1,9 };//���������ƶ�
int avaiList[81];//���в�
int valueList[81];//�����ֵ
int maxValue[81];//����ֵ��
int chooseMode()//��ʼ��Ϸ��ѡ��ģʽ����
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
	settextstyle(100, 0, L"����");
	outtextxy(250, 50, L"��Χ��");
	settextstyle(100, 0, L"Times New Roman");
	outtextxy(300, 150, L"NoGo");
	settextstyle(50, 0, L"����");
	outtextxy(300, 275, L"��ͷ��ʼ");
	outtextxy(300, 375, L"������Ϸ");
	outtextxy(350, 475, L"����");
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
void initGame()//��ʼ����Ϸ���沢ѡ���Ƿ񲥷�����
{
	HWND hnd = GetHWnd();
	SetWindowText(hnd, L"��Χ��NoGo");//�ı䴰�ھ��
	int isok = MessageBox(hnd, L"�Ƿ񲥷ű������֣�", L"Music", MB_OKCANCEL);//ѡ���Ƿ񲥷�����
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
	settextstyle(50, 0, L"����");
	outtextxy(650, 100, L"�浵");
	outtextxy(650, 200, L"����");
	outtextxy(650, 300, L"����");
	outtextxy(650, 400, L"�˳�");
	HWND blackorwhite = GetHWnd();
	SetWindowText(blackorwhite, L"��Χ��NoGo");
	int tochoosecolor = MessageBox(blackorwhite, L"�Ƿ�ѡ��ִ�ڣ�", L"ѡ����ɫ", MB_YESNO);
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
bool dfsAir(int NOGO[81], int fseq)//�ж�ĳ���Ƿ�����
{
	dfsAirVisit[fseq] = true;
	bool flag = false;
	for (int dir = 0; dir < 4; dir++)
	{
		if (fseq % 9 == 0 && dir == 1)continue;//��һ�в�����Ѱ��
		if (fseq % 9 == 8 && dir == 2)continue;//���һ�в�����Ѱ��
		int dseq = fseq + cseq[dir];//�ƶ�
		if (dseq >= 0 && dseq <= 80)//��������ڲ�
		{
			if (NOGO[dseq] == 0)//�մ�������
				flag = true;
			if (NOGO[dseq] == NOGO[fseq] && !dfsAirVisit[dseq])//ͬɫ��������
				if (dfsAir(NOGO, dseq))//�ݹ�Ѱ��
					flag = true;
		}
	}
	return flag;
}
bool finishJudge()//�ж���Ϸ�Ƿ����
{
	for (int i = 0; i <= 80; i++)
	{
		memset(dfsAirVisit, false, sizeof(dfsAirVisit));
		if (NOGO[i]!=0&&(!dfsAir(NOGO,i)))//ĳ��������������Ϸ����
			return true;
	}
	return false;
}
bool avaiJudge(int fseq, int color)//�ж�ĳ���ɷ�����
{
	if (NOGO[fseq] != 0)return false;//����õ��������ӣ��������� 
	NOGO[fseq] = color;//���������ڴ˴� 
	memset(dfsAirVisit, false, sizeof(dfsAirVisit));//���ʼ�¼����
	if (!dfsAir(NOGO, fseq))//������������
	{
		NOGO[fseq] = 0;
		return false;
	}
	for (int dir = 0; dir < 4; dir++)//����õ����������ĵ��Ƿ����� 
	{
		int dseq = fseq + cseq[dir];
		if (dseq >= 0 && dseq <= 80)//�������� 
		{
			if (NOGO[dseq] != 0 && !dfsAirVisit[dseq])//������
			{
				if (!dfsAir(NOGO, dseq))//���Ӻ������� 
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
int calValue(int color)//�����ֵ
{
	int val = 0;
	int opcolour = -color;
	for (int fseq = 0; fseq <= 80; fseq++)//����
	{
		if (avaiJudge(fseq, color))val++;//��������
		if (avaiJudge(fseq, opcolour))val--;//�Է�����
	}
	return val;
}
int search(int color, int depth)//��������
{
	int max = -2147483648;
	bool flag = false;
	if (depth == maxDepth)//�ﵽָ����������
	{
		return calValue(-color);
	}
	else
	{
		for (int fseq = 0; fseq <= 80; fseq++)
		{

			{
				if (!avaiJudge(fseq, -color))continue;//������
				else
				{
					flag = true;
					NOGO[fseq] = -color;
					int val = search(-color, depth + 1);//����һ��
					if (val > max)max = val;//���Ź�ֵ
					NOGO[fseq] = 0;
				}
			}
		}
	}
	if (!flag)return 2147483647;//��������
	else return -max;
}
bool sameColor(int fseq, int color)//�ж������Ƿ���ͬɫ����
{
	for (int dir = 0; dir < 4; dir++)
	{
		int dseq = fseq + cseq[dir];
		if (NOGO[dseq] == color)
			return true;
	}
	return false;
}
int botAI(int NOGO[81])//����
{
	int color = -myColor;
	int totAvai = 0;
	int maxVal = -2147483647;
	int maxNum = 0;
	for (int fseq = 0; fseq <= 80; fseq++)
	{
		if (avaiJudge(fseq, color))//���в�
		{
			avaiList[totAvai++] = fseq;
			NOGO[fseq] = color;
			valueList[fseq] = calValue(color);
			if (sameColor(fseq, color))//��������ͬɫ������Χ
				valueList[fseq]--;
			if (valueList[fseq] > maxVal)//���Ų�
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
	int pos = rand() % maxNum;//����߹�ֵ�������ѡȡ
	int bestAction = maxValue[pos];
	return bestAction;
}
bool botResponse()//�������Ӧ
{
	srand((unsigned)time(0));
	memset(avaiList, 0, sizeof(avaiList));//���в�����
	memset(valueList, 0, sizeof(valueList));//��ֵ����
	int bestAction = botAI(NOGO);//���ɻ�Ӧ
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
void playNoGo()//��Ϸ����
{
	while (1)
	{
		msg = GetMouseMsg();
		if(msg.mkLButton)
		{
			int i = (msg.x + 30) / 60 * 60 - 30;
			int j = (msg.y + 30) / 60 * 60 - 30;
			int seq = (i / 60) + (j / 60) * 9;
			if (msg.x >= 30 && msg.x <= 570 && msg.y >= 30 && msg.y <= 570 && NOGO[seq] == 0)//������������
			{

				if (myColor < 0)//�ж������ִ��ɫ
				{
					putimage(i, j, &blackpiece);
					NOGO[seq] = -1;
				}
				else if (myColor > 0)
				{
					putimage(i, j, &whitepiece);
					NOGO[seq] = 1;
				}
				if (finishJudge())//�ж��Ƿ��վ�
				{
					HWND finish = GetHWnd();
					SetWindowText(finish, L"��Χ��NoGo");
					int check = MessageBox(finish, L"���ź������ˣ��ٽ������ɣ�", L"�Ծ�ʧ��", MB_OK);
					return;
				}
				if (botResponse())//�жϼ������Ӧ���Ƿ��վ�
				{
					HWND finish = GetHWnd();
					SetWindowText(finish, L"��Χ��NoGo");
					int check = MessageBox(finish, L"��ϲ����Ӯ�ˣ�", L"�Ծֻ�ʤ", MB_OK);
					return;
				}
			}
			else if (msg.x >= 650 && msg.x <= 750 && msg.y >= 400 && msg.y <= 450)//ѡ���˳�
			{
				HWND exit = GetHWnd();
				int toexit = MessageBox(exit, L"��ȷ�ϴ浵���˳���δ�浵�ĶԾֽ��Ƚ��ᶪʧ", L"ȷ���˳�", MB_OKCANCEL);
				if (toexit == IDOK)
				{
					return;
				}
				else if (toexit == IDCANCEL);
			}
			else if (msg.x >= 650 && msg.x <= 750 && msg.y >= 100 && msg.y <= 150)//ѡ��浵
			{
				char str[100] = { '\0' };
				sprintf(str, "del memory.txt");
				system(str);
				FILE* mem;
				if ((mem = fopen("memory.txt", "w+")) == NULL)
				{
					HWND save = GetHWnd();
					MessageBoxA(save, "�浵ʧ��", "��Χ��NoGo", MB_OK || MB_SYSTEMMODAL);
				}
				else
				{
					for (int i = 0; i <= 80; i++)
						fwrite(&NOGO[i], sizeof(NOGO[i]), 1, mem);
					fwrite(&myColor, sizeof(myColor), 1, mem);
					fclose(mem);
					HWND save = GetHWnd();
					MessageBoxA(save, "�浵��ɣ�", "��Χ��NoGo", MB_OK || MB_SYSTEMMODAL);
				}
			}
			else if (msg.x >= 650 && msg.x <= 750 && msg.y >= 200 && msg.y <= 250)//ѡ�����
			{
				FILE* mem;
				if ((mem = fopen("memory.txt", "r")) == NULL)
				{
					HWND save = GetHWnd();
					MessageBoxA(save, "�浵Ϊ��", "��Χ��NoGo", MB_OK || MB_SYSTEMMODAL);
				}
				else
				{
					for (int i = 0; i <= 80; i++)
						fread(&NOGO[i], sizeof(NOGO[i]), 1, mem);
					fread(&myColor, sizeof(myColor), 1, mem);
					fclose(mem);
					HWND save = GetHWnd();
					MessageBoxA(save, "������ɣ�", "��Χ��NoGo", MB_OK || MB_SYSTEMMODAL);
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
			else if (msg.x >= 650 && msg.x <= 750 && msg.y >= 300 && msg.y <= 350)//ѡ������
			{
				HWND again = GetHWnd();
				int toagain = MessageBox(again, L"�Ƿ����¿�ʼ�Ծ֣�δ�浵�ĶԾֽ��Ƚ��ᶪʧ", L"���¿�ʼ", MB_OKCANCEL);
				if (toagain == IDOK)
				{
					memset(NOGO, 0, sizeof(NOGO));
					putimage(0, 0, &board);
				if (myColor == 1)//���ִ�ף����������µ�һ������
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
void continueGame()//�Ӵ浵������Ϸ
{
	HWND hnd = GetHWnd();
	SetWindowText(hnd, L"��Χ��NoGo");//�ı䴰�ھ��
	int isok = MessageBox(hnd, L"�Ƿ񲥷ű������֣�", L"Music", MB_OKCANCEL);
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
	settextstyle(50, 0, L"����");
	outtextxy(650, 100, L"�浵");
	outtextxy(650, 200, L"����");
	outtextxy(650, 300, L"����");
	outtextxy(650, 400, L"�˳�");
	FILE* mem;
	if ((mem = fopen("memory.txt", "r")) == NULL)
		MessageBoxA(hnd, "�浵Ϊ��", "��Χ��NoGo", MB_OK || MB_SYSTEMMODAL);
	else
	{
		for (int i = 0; i <= 80; i++)
			fread(&NOGO[i], sizeof(NOGO[i]), 1, mem);
		fread(&myColor, sizeof(myColor), 1, mem);
		fclose(mem);
		HWND save = GetHWnd();
		MessageBoxA(save, "������ɣ�", "��Χ��NoGo", MB_OK || MB_SYSTEMMODAL);
		for (int iseq = 0; iseq <= 80; iseq++)
		{
			int i = iseq / 9;
			int j = iseq % 9;
			if (NOGO[iseq] == -1)putimage(60 * j + 30, 60 * i + 30, &blackpiece);
			else if (NOGO[iseq] == 1)putimage(60 * j + 30, 60 * i + 30, &whitepiece);
		}
	}
}
void showAbout()//���ڽ���
{
	initgraph(800, 600);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	setlinecolor(BLACK);
	IMAGE backgroundabout;
	loadimage(&backgroundabout, L"./backgroundabout.jpg", 800, 600);
	putimage(0, 0, &backgroundabout);
	HWND hnd = GetHWnd();
	SetWindowText(hnd, L"����");
	int isok = MessageBox(hnd, L"�Ƿ񲥷ű������֣�", L"����", MB_OKCANCEL);
	if (isok == IDOK)
	{
		PlaySound(L"./SMILE.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	}
	else if (isok == IDCANCEL);

	rectangle(5,5, 125, 35);
	rectangle(5,150, 65, 180);
	rectangle(5, 295, 125, 325);
	//rectangle(650, 400, 750, 450);*/
	settextstyle(30, 0, L"����");
	outtextxy(5, 5, L"��Ϸ����");
	settextstyle(20, 0, L"����");
	outtextxy(15, 40, L"NoGo(��Χ��),��Χ������������������������걸��Ϣ���ģ�������Χ���෴��");
	outtextxy(15, 60, L"�������֣�˫���������ӣ����Ӻ����Ӳ����ƶ���");
	outtextxy(15, 80, L"���һ�����Ӻ�Ե��˶Է������ӣ�������һ���и���");
	outtextxy(15, 100, L"������ɱһ���и�������һ���и���");
	outtextxy(15, 120, L"����״̬�����Ӳ����������ϴ��ڡ�");
	settextstyle(30, 0, L"����");
	outtextxy(5, 150, L"��л");
	settextstyle(20, 0, L"����");
	outtextxy(15, 185, L"����Ϸ�ĵ����벻��������ѧ20��������������������껡����󶬵�ͬѧ�İ�����");
	outtextxy(15, 205, L"��Botzone�û�@�䶳���㡢@JeremyDon��@Halifuda���û��Ŀ�Դ���ף�");
	outtextxy(15, 225, L"�ڴ˶�����һ����ʾ���ĵĸ�л��");
	outtextxy(15, 245, L"����Ϸ������������Ҫ�Ĳο������У�");
	outtextxy(15, 265, L"��ٻ��,���Ź�.���ڼ�ֵ�����Ĳ�Χ��ݹ��㷨[J].����ʦ����ѧѧ��(��Ȼ��ѧ��)");
	settextstyle(30, 0, L"����");
	outtextxy(5, 295, L"��������");
	settextstyle(20, 0, L"����");
	outtextxy(15, 330, L"��ҳ�汳�����֣�AlphaGo BGM Music, China Background Music");
	outtextxy(15, 350, L"����ҳ�汳�����֣�Smile -You&Me-, ELISA ");
	settextstyle(20, 0, L"����");
	outtextxy(320, 500, L"����������˳�");
	getchar();
}
int main()
{
	int choice=chooseMode();//ѡ��ģʽ
	if (choice == 1)//��ͷ��ʼ
	{
		initGame();
		playNoGo();
	}
	else if (choice == 2)//������Ϸ
	{
		continueGame();
		playNoGo();
	}
	else if (choice == 3)//���ڽ���
	{
		showAbout();
	}
	return 0;
}