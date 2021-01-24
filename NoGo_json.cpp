#include "./jsoncpp/json.h"
#include <iostream>
#include <cstring>
#include <string>
#include <ctime>
#include <random>
#include <cstdlib>
using namespace std;
int maxDepth = 6;//最大搜索层数
int NOGO[81];//存储棋盘状态
int myColor;//我方棋子颜色
bool dfsAirVisit[81];//判断求气是否重复
const int cseq[] = { -9,-1,1,9 };//上下左右移动
int avaiList[81];//可行步
int valueList[81];//各点估值
int maxValue[81];//最大估值点
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
int main() 
{
	srand((unsigned)time(0));//随机种子
	string str;
	int x, y;
	//读入JSON
	getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);
	//分析自己收到的输入和自己过往的输出，并恢复状态
	int turnID = input["responses"].size();//trunID为自己已走的步数
	for (int i = 0; i < turnID; i++)
	{
		x = input["requests"][i]["x"].asInt(), y = input["requests"][i]["y"].asInt();//requests为对方的过往输入
		if (x != -1) NOGO[x + 9 * y] = 1;
		x = input["responses"][i]["x"].asInt(), y = input["responses"][i]["y"].asInt();//responses为己方的过往输出
		if (x != -1) NOGO[x + 9 * y] = -1;
	}
	x = input["requests"][turnID]["x"].asInt(), y = input["requests"][turnID]["y"].asInt();
	if (x != -1) NOGO[x + 9 * y] = 1;
	// 输出决策JSON
	Json::Value ret;
	Json::Value action;

	//以下为策略 
	memset(avaiList, 0, sizeof(avaiList));//可行步归零
	memset(valueList, 0, sizeof(valueList));//估值归零
	myColor = x == -1 ? -1 : 1;//获取我方棋子颜色
	int bestAction = botAI(NOGO);//生成回应
	action["x"] = bestAction % 9;
	action["y"] = bestAction / 9;
	ret["response"] = action;
	Json::FastWriter writer;

	cout << writer.write(ret) << endl;
	return 0;
}