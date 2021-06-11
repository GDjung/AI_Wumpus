//Visual Studio 2019 (16.0)
//The Catholic University of Korea 
//그만보조
//정지용 고다영 김태진 이승희
//2021년도 1학기 인공지능
//WUMPUS WORLD
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<stdbool.h>
#define GridSize 6
#pragma warning(disable: 4996)

typedef enum { E, W, N, S } Direction;
typedef enum { NONE, TURN_LEFT, SHOOT, GOFORWARD, TURN_RIGHT, GRAB, CLIMB } actionList;
typedef struct WorldEnviron {  //WhatWorld
	bool unknown; 
	bool safe;
	bool walls;
	bool bumps;
	bool goals;
	bool glitter;
	bool golds;
	bool breezes;
	bool pits;
	bool stenches;
	bool wumpi;
	bool scream;
}WorldEnv;   // env

typedef struct agent_state { 
	WorldEnv Grid[6 * 6]; //state
	WorldEnv cur_percept;         //percet
	int x_pos;
	int y_pos;
	Direction MyHeading;
	actionList LastAction; 

	int arrow;
	bool gold;
	bool alive;
}agentMode;

//Func Set Map
void SetBumps(int x, int y, WorldEnv* Grid) { Grid[GridSize * x + y].bumps = true; }
void SetGoals(int x, int y, WorldEnv* Grid) { Grid[GridSize * x + y].goals = true; }
void SetGolds(int x, int y, WorldEnv* Grid) { Grid[GridSize * x + y].golds = true; }
void SetBreezes(int x, int y, WorldEnv* Grid) { Grid[GridSize * x + y].breezes = true; }
void SetPits(int x, int y, WorldEnv* Grid) { Grid[GridSize * x + y].pits = true; }
void SetStenches(int x, int y, WorldEnv* Grid) { Grid[GridSize * x + y].stenches = true; }
void SetWumpi(int x, int y, WorldEnv* Grid) { Grid[GridSize * x + y].wumpi = true; }
void SetSafe(int x, int y, WorldEnv* Grid) { Grid[GridSize * x + y].safe = true; }
WorldEnv GetState(int x, int y, WorldEnv* Grid) { return Grid[GridSize * x + y]; }
void SetMyPos(int x, int y, agentMode* ag) { ag->x_pos = x; ag->y_pos = y; }

//사용자 정의함수
void realWorldSet(WorldEnv* RealWorld);
actionList ChooseAction(agentMode* agent);
void UpdateState(agentMode* agentSt);
bool checkAlive(agentMode* agent);
void StateSet(agentMode* agentSt);
void percept(agentMode* agent, WorldEnv* RealWorld);
void showMap(WorldEnv* Map, int x_pos, int y_pos);
void printInfo(agentMode* agent);

////////////////////////////////////MAIN//////////////////////////////////
int main() {

	agentMode agentMan;				//agent 
	memset(agentMan.Grid, false, sizeof(WorldEnv) * 36); //에이전트 그리드 초기화
	StateSet(&agentMan);

	WorldEnv RealWorld[6 * 6] = { 0, };   //전지적 시점 map 생성 realWorld
	memset(RealWorld, false, sizeof(WorldEnv) * 36);
	realWorldSet(RealWorld);


	while (1)
	{
		char test[100];
		///////1.환경인식
		percept(&agentMan, RealWorld);
		if (!checkAlive(&agentMan))//에이전트 생존여부 체크
		{
			StateSet(&agentMan);
			puts("\n\n=================================================");
			puts("================Agent is dead====================");
			puts("=================================================\n\n");
			puts("Enter any key and retry");
			gets_s(test, 3);
			continue;

		}

		////2.환경업데이트
		UpdateState(&agentMan);   //에이전트가 인식하고있는 것을 토대로 에이전트가 가지고있는 맵을 업데이트해줌.

		////// 그리드 맵 출력
		puts("\n====RealWorld Map========");
		showMap(RealWorld, 9, 9);
		puts("\n=======Agent Map=========");
		showMap(agentMan.Grid, agentMan.x_pos, agentMan.y_pos);
		if (!agentMan.cur_percept.bumps)
			printInfo(&agentMan);


		//////////3. 액션
		agentMan.LastAction = ChooseAction(&agentMan);
		

		if (agentMan.cur_percept.bumps)
			printInfo(&agentMan);
	


		gets_s(test, 3);
	}

}

void printInfo(agentMode* agent)
{

	puts("=============current state================");
	printf("[Heading : ");
	switch (agent->MyHeading) {
	case N: printf("▲ ]"); break;
	case S: printf("▼ ]"); break;
	case E: printf("▶ ]"); break;
	case W: printf("◀ ]"); break;
	}
	printf("  [ Gold : %d ]  [ Arrow : %d ] [ Current position : %d,%d ]\n", agent->gold, agent->arrow, agent->x_pos, agent->y_pos);
	puts("==========================================");

}

void showMap(WorldEnv* Map, int x_pos, int y_pos)
{
	bool print = false;
	char buf[15] = { 0 };
	for (int y = 5; y >= 0; y--)
	{

		printf("\n  ");
		for (int x = 0; x < 6; x++)
		{
			print = false;
			memset(buf, 0, sizeof(char) * 15);
			if (Map[GridSize * x + y].walls)
			{
				strcat(buf, "W.");
				printf("%6s |", buf);

				continue;
			
			}

			//현재위치출력
			if (x == x_pos && y == y_pos)
				strcat(buf, "●");

			if (Map[GridSize * x + y].breezes)
			{
				strcat(buf, "B.");
			}
			if (Map[GridSize * x + y].pits)
			{
				strcat(buf, "P.");
			}

			if (Map[GridSize * x + y].safe)
			{
				strcat(buf, "S.");
				
			}
			if (Map[GridSize * x + y].golds)
			{
				strcat(buf, "G.");
			}
			if (Map[GridSize * x + y].stenches)
			{
				strcat(buf, "ST.");
				
			}
			if (Map[GridSize * x + y].wumpi)
			{
				strcat(buf, "WP.");
		
			}

			if ((int)*buf == 0)
				strcat(buf, "==?==");

			printf("%6s |", buf);
		}

		printf("  \n");
	}

}


///////////////////////////////////////////////////////////////////////////
void realWorldSet(WorldEnv* RealWorld) {

	// 0..11 : X Walls  
	for (int i = 0; i < 6; i++)
	{
		RealWorld[6 * 0 + i].walls = true;
		RealWorld[6 * 5 + i].walls = true;
	}


	// 0, 12, 24, ... : Y Walls
	for (int i = 0; i < 6; i++)
	{
		RealWorld[6 * i + 0].walls = true;
		RealWorld[6 * i + 5].walls = true;
	}

	//safe zone설정

	RealWorld[6 * 1 + 1].safe = true;  // 1,1 safe
	RealWorld[6 * 1 + 2].safe = true;  // 1,2 safe
	RealWorld[6 * 2 + 1].safe = true;  // 2,1 safe


	//
	printf("골드는 3,4\n");
	RealWorld[GridSize * 3 + 4].golds = true;  // 3,4 -> gold

	srand((unsigned int)time(NULL));

	for (int i = 1; i < 5; i++)
		for (int j = 1; j < 5; j++)
		{

			//WUMPUS 랜덤 생성
			int percent = rand() % 100;
			//printf("rand = %d",percent);

			if (percent < 15 && RealWorld[6 * i + j].golds != true && RealWorld[6 * i + j].safe != true) {
			//if (i == 4 && j == 1) {
				RealWorld[6 * i + j].wumpi = true;

				//양 사방에 STENCHES 설정
				RealWorld[6 * (i - 1) + j].stenches = true;
				RealWorld[6 * (i + 1) + j].stenches = true;
				RealWorld[6 * i + (j + 1)].stenches = true;
				RealWorld[6 * i + (j - 1)].stenches = true;

				//debug용 프린트
				printf("wumpus 생성 장소 : (%d,%d)\n", i, j);

			}


			//PITS 랜덤 생성

			percent = rand() % 100;
			if (percent < 15 && RealWorld[6 * i + j].golds != true && RealWorld[6 * i + j].safe != true) {
			//if (i == 4 && j == 1) {
				RealWorld[6 * i + j].pits = true;

				//양 사방에 breezes 설정
				RealWorld[6 * (i - 1) + j].breezes = true;
				RealWorld[6 * (i + 1) + j].breezes = true;
				RealWorld[6 * i + (j + 1)].breezes = true;
				RealWorld[6 * i + (j - 1)].breezes = true;

				printf("pits 생성 장소 : (%d,%d)\n", i, j);
			}

		}

}



void percept(agentMode* agent, WorldEnv* RealWorld)
{
	int x, y;

	x = agent->x_pos;
	y = agent->y_pos;




	agent->cur_percept = RealWorld[6 * x + y]; // 젤중요 

	//마지막으로 화살을 쐈을때 웜퍼스의 비명으로 인한 웜퍼스의 죽음을 인지해야함.
	if (agent->LastAction == SHOOT)
	{
		agent->arrow--;
		agent->cur_percept.scream = true;
		agent->cur_percept.stenches = false;
		RealWorld[GridSize * x + y].stenches = false;
		agent->Grid[GridSize * x + y].stenches = false;
		switch (agent->MyHeading) {
		case E:
			agent->Grid[GridSize * (x + 1) + y].wumpi = false;
			RealWorld[GridSize * (x + 1) + y].wumpi = false;
			
			break;
		case W:
			agent->Grid[GridSize * (x - 1) + y].wumpi = false;
			RealWorld[GridSize * (x - 1) + y].wumpi = false;
		
			break;
		case N:
			agent->Grid[GridSize * x + y + 1].wumpi = false;
			RealWorld[GridSize * x + y + 1].wumpi = false;
		
			break;
		case S:
			agent->Grid[GridSize * x + y - 1].wumpi = false;
			RealWorld[GridSize * x + y - 1].wumpi = false;
			break;
		}

		puts("kill wumpi!");

	}

	if (RealWorld[6 * x + y].walls)
	{
		agent->cur_percept.bumps = true;
	

	}

	if (RealWorld[6 * x + y].golds && !agent->gold)  
		agent->cur_percept.glitter = true;

	if (agent->gold)
		agent->cur_percept.golds = false;

}

//agent가 웜퍼스나 구덩이를 만나서 죽었는지 파악하는 함수
bool checkAlive(agentMode* agent)
{
	int x, y;
	x = agent->x_pos;
	y = agent->y_pos;

	if (agent->cur_percept.pits || agent->cur_percept.wumpi)
	{
		if (agent->cur_percept.pits)
			agent->Grid[GridSize * x + y].pits = true;
		if (agent->cur_percept.wumpi)
			agent->Grid[GridSize * x + y].wumpi = true;

		agent->alive = false;
		puts("agent is dead");

		return false;
	}


}



//에이전트 state 초기설정
void StateSet(agentMode* agentSt)
{
	
	WorldEnv* grid_array = agentSt->Grid;


	grid_array[6 * 1 + 1].safe = true; // 1,1은 safe 
	agentSt->MyHeading = E;


	agentSt->arrow = 3;
	agentSt->gold = false;
	agentSt->x_pos = 1;
	agentSt->y_pos = 1;
	//SetMyPos(1,1);
}


//Update State 
void UpdateState(agentMode* agentSt)
{
	int x, y;
	WorldEnv perception;

	x = agentSt->x_pos;
	y = agentSt->y_pos;
	perception = agentSt->cur_percept;


	//벽이랑 충돌했을때
	if (perception.bumps) {
		switch (agentSt->MyHeading) {
		case E: agentSt->Grid[6 * x + y].walls = true; break;
		case W: agentSt->Grid[6 * x + y].walls = true; break;
		case N: agentSt->Grid[6 * x + y].walls = true; break;
		case S: agentSt->Grid[6 * x + y].walls = true; break;
		}
		printf("[bump]  (%d,%d) is wall !!\n", x, y);
	}

	if ((agentSt->gold) && (x == 1) && (y == 1))
	{
		agentSt->Grid[6 * x + y].goals = true;
		agentSt->cur_percept.goals = true;

		puts("goal!!!!!!!!!!!!!!!!!!!!!");

	}

	if ((perception.glitter) && (agentSt->gold == false)) agentSt->Grid[6 * x + y].golds = true;

	if (perception.stenches) agentSt->Grid[6 * x + y].stenches = true;
	if (perception.breezes) agentSt->Grid[6 * x + y].breezes = true;
	if (!perception.breezes && !perception.pits && !perception.stenches && !perception.wumpi)
		agentSt->Grid[6 * x + y].safe = true;
}


actionList ChooseAction(agentMode* agent)
{

	static int bumpNum = 0;
	actionList action = NONE;
	int x, y;

	x = agent->x_pos;
	y = agent->y_pos;

	WorldEnv state = agent->cur_percept;

	if (state.goals)
	{
		action = CLIMB;
		puts("CLIMB!");
		puts("\n\n======================END==========================\n\n");
		exit(1);
	}
	if (state.golds)
	{
		puts("GRAB");
		puts("[Agent] I got gold!");
		action = GRAB;
		agent->gold = true;
		return action;
	}
	// BREEZES 랑 STENCHES 겹쳤을때
	if (state.breezes && state.stenches)
	{
		switch (agent->MyHeading)
		{
		case E: 
			if (agent->Grid[GridSize * (x + 1) + y].wumpi && !agent->Grid[6 * (x + 1) + y].pits)
			{
				action = SHOOT;
				agent->arrow--;
			}
			else if (agent->Grid[6 * (x + 1) + y].pits)
			{
				action = TURN_LEFT;
				agent->MyHeading = N;
			}
			else {
				action = GOFORWARD;
				SetMyPos(x + 1, y, agent);
			}
			break;

		case W:
			if (agent->Grid[GridSize * (x - 1) + y].wumpi && !agent->Grid[6 * (x - 1) + y].pits)
			{
				action = SHOOT;
				agent->arrow--;
			}
			else if (agent->Grid[6 * (x - 1) + y].pits)
			{
				action = TURN_LEFT;
				agent->MyHeading = S;
			}
			else {
				action = GOFORWARD;
				SetMyPos(x - 1, y, agent);
			}
			break;
		case N:
			if (agent->Grid[GridSize * x  + y + 1].wumpi && !agent->Grid[6 * x  + y + 1].pits)
			{
				action = SHOOT;
				agent->arrow--;
			}
			else if (agent->Grid[6 * x + y + 1].pits)
			{
				action = TURN_LEFT;
				agent->MyHeading = W;
			}
			else {
				action = GOFORWARD;
				SetMyPos(x , y+1, agent);
			}
			break;
		case S:
			if (agent->Grid[GridSize * x + y -1].wumpi && !agent->Grid[6 * x + y - 1].pits)
			{
				action = SHOOT;
				agent->arrow--;
			}
			else if (agent->Grid[6 * x + y - 1].pits)
			{
				action = TURN_LEFT;
				agent->MyHeading = E;
			}
			else {
				action = GOFORWARD;
				SetMyPos(x, y - 1, agent);
			}
			break;
		}
	}

	if (state.breezes && action == NONE ) {

		switch (agent->MyHeading) {
		case E:
			if (agent->Grid[6 * (x + 1) + y].pits) {
				action = TURN_LEFT;
				agent->MyHeading = N;
			}
			else {
				action = GOFORWARD;
				SetMyPos(x + 1, y, agent);
			}
			break;
		case W:
			if (agent->Grid[6 * (x - 1) + y].pits) {
				action = TURN_LEFT;
				agent->MyHeading = S;
			}
			else {
				action = GOFORWARD;
				SetMyPos(x - 1, y, agent);
			}
			break;
		case N:
			if (agent->Grid[6 * x + y + 1].pits) {
				action = TURN_LEFT;
				agent->MyHeading = W;
			}
			else {
				action = GOFORWARD;
				SetMyPos(x, y + 1, agent);
			}
			break;
		case S:
			if (agent->Grid[6 * x + y - 1].pits) {
				action = TURN_LEFT;
				agent->MyHeading = E;
			}
			else {
				action = GOFORWARD;
				SetMyPos(x, y - 1, agent);
			}
			break;
		}

	}
	if (state.stenches && action==NONE)
	{
		switch (agent->MyHeading) {
		case E:
			if ((GetState(x + 1, y, agent->Grid).wumpi) && (agent->arrow > 0)) action = SHOOT;
			else {
				action = GOFORWARD;
				SetMyPos(x + 1, y, agent);
			}
			break;
		case W:
			if ((GetState(x - 1, y, agent->Grid).wumpi) && (agent->arrow > 0)) action = SHOOT;
			else {
				action = GOFORWARD;
				SetMyPos(x - 1, y, agent);
			}
			break;
		case N:
			if ((GetState(x, y + 1, agent->Grid).wumpi) && (agent->arrow > 0)) action = SHOOT;
			else {
				action = GOFORWARD;
				SetMyPos(x, y + 1, agent);
			}
			break;
		case S:
			if ((GetState(x, y - 1, agent->Grid).wumpi) && (agent->arrow > 0)) action = SHOOT;
			else {
				action = GOFORWARD;
				SetMyPos(x, y - 1, agent);
			}
			break;
		}

	}
	///////////
	if (state.bumps)
	{
		bumpNum++;
		//printf("\n bumpNum = %d  =====================================\n", bumpNum);
		//벽에 박으면 왼쪽으로가는걸로 디폴트 설정.
		if (agent->LastAction == TURN_LEFT || agent->LastAction == TURN_RIGHT)
		{
			action = GOFORWARD;
			switch (agent->MyHeading) {
			case E: SetMyPos(x + 1, y, agent); break;
			case W: SetMyPos(x - 1, y, agent); break;
			case N: SetMyPos(x, y + 1, agent); break;
			case S: SetMyPos(x, y - 1, agent); break;
			}

		}
		//벽에 계속(10번) 박으면 오른쪽으로 가는걸로. 
		else if (bumpNum > 10 && agent->LastAction != TURN_LEFT) 
		{
			action = TURN_RIGHT;
			switch (agent->MyHeading) {
			case E: agent->MyHeading = S; SetMyPos(x - 1, y, agent); break;
			case W: agent->MyHeading = N; SetMyPos(x + 1, y, agent); break;
			case N: agent->MyHeading = E; SetMyPos(x, y - 1, agent);  break;
			case S: agent->MyHeading = W; SetMyPos(x, y + 1, agent); break;
			}
			
			//그래도 계속 박으면 bumpNum 초기화=
			if (bumpNum > 20) bumpNum = 0; 
		}
		else {

			action = TURN_LEFT;
			switch (agent->MyHeading) {
			case E: agent->MyHeading = N; SetMyPos(x - 1, y, agent); break;
			case W: agent->MyHeading = S; SetMyPos(x + 1, y, agent); break;
			case N: agent->MyHeading = W; SetMyPos(x, y - 1, agent);  break;
			case S: agent->MyHeading = E; SetMyPos(x, y + 1, agent); break;
			}
		}

		
	}
	//default
	if (action == NONE)
	{
		action = GOFORWARD;

		switch (agent->MyHeading) {
		case E:
			SetMyPos(x + 1, y, agent);
			break;
		case W: SetMyPos(x - 1, y, agent); break;
		case N: SetMyPos(x, y + 1, agent); break;
		case S: SetMyPos(x, y - 1, agent); break;
		}

	}



	switch (action) {
	case GOFORWARD: puts("GOFORWARD"); break;
	case TURN_LEFT: puts("TURN LEFT"); break;
	case TURN_RIGHT: puts("TURN RIGHT"); break;
	case SHOOT: puts("SHOOT Arrow"); break;
	}
	return action;
}
