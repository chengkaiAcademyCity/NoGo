#include "./jsoncpp/json.h"
#include <iostream>
#include <cstring>
#include <string>
#include <ctime>
#include <random>
#include <cstdlib>
using namespace std;
int maxDepth = 6;//�����������
int NOGO[81];//�洢����״̬
int myColor;//�ҷ�������ɫ
bool dfsAirVisit[81];//�ж������Ƿ��ظ�
const int cseq[] = { -9,-1,1,9 };//���������ƶ�
int avaiList[81];//���в�
int valueList[81];//�����ֵ
int maxValue[81];//����ֵ��
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
int main() 
{
	srand((unsigned)time(0));//�������
	string str;
	int x, y;
	//����JSON
	getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);
	//�����Լ��յ���������Լ���������������ָ�״̬
	int turnID = input["responses"].size();//trunIDΪ�Լ����ߵĲ���
	for (int i = 0; i < turnID; i++)
	{
		x = input["requests"][i]["x"].asInt(), y = input["requests"][i]["y"].asInt();//requestsΪ�Է��Ĺ�������
		if (x != -1) NOGO[x + 9 * y] = 1;
		x = input["responses"][i]["x"].asInt(), y = input["responses"][i]["y"].asInt();//responsesΪ�����Ĺ������
		if (x != -1) NOGO[x + 9 * y] = -1;
	}
	x = input["requests"][turnID]["x"].asInt(), y = input["requests"][turnID]["y"].asInt();
	if (x != -1) NOGO[x + 9 * y] = 1;
	// �������JSON
	Json::Value ret;
	Json::Value action;

	//����Ϊ���� 
	memset(avaiList, 0, sizeof(avaiList));//���в�����
	memset(valueList, 0, sizeof(valueList));//��ֵ����
	myColor = x == -1 ? -1 : 1;//��ȡ�ҷ�������ɫ
	int bestAction = botAI(NOGO);//���ɻ�Ӧ
	action["x"] = bestAction % 9;
	action["y"] = bestAction / 9;
	ret["response"] = action;
	Json::FastWriter writer;

	cout << writer.write(ret) << endl;
	return 0;
}