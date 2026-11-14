//自行引入头文件
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<string.h>
#include<time.h>
#include <graphics.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include<windows.h>
#include <math.h>
#include<Mmsystem.h>
#pragma comment(lib,"winmm.lib")
#define PI 3.1415926535
void generationoriginalfood();
void lract(int x, int y, int w, int h, int k, int m, IMAGE* image, IMAGE* mask, IMAGE* bg);//人物左右移动  k为次数 m为距离
void ouact(int x, int y, int w, int h, int k, int m, IMAGE* image, IMAGE* mask, IMAGE* bg);//人物上下移动  k为次数 m为距离
void ikun();
//表示黄苹果buff的持续时间
int YELLOWAPPLE_BUFF = 0;
//表示黄苹果buff生效前的速度
int NOW_speed = 0;
int start = 1;
int t = 0;
int cnt1 = 0;
int cnt2 = 0;
int cnt3 = 0;
IMAGE img_settings, img_win, img_stone,stonemask;
IMAGE guga[4];
IMAGE gugamask[4];
IMAGE gugahead[4];
IMAGE gugaheadmask[4];

//------------------------- 蛇皮肤 -------------------------
// 蛇头和蛇身各用一张图，完全沿用原素材的用法：
//   主体图 = 黑底彩色（SRCPAINT 叠加）
//   掩码图 = 白底黑剪影（SRCAND 抠图）
// 想加新皮肤：在表里加一行，图片放 image 目录即可。
struct SnakeSkin {
	const wchar_t* head;
	const wchar_t* headMask;
	const wchar_t* body;
	const wchar_t* bodyMask;
	const wchar_t* name;
};
const SnakeSkin SNAKE_SKINS[] = {
	{ L"image\\gugaheadw.png", L"image\\gugaheadmaskw.png", L"image\\gugaw.png", L"image\\gugamaskw.png", L"咕咕嘎嘎（原版）" },
	{ L"image\\chicken_head.png",        L"image\\chicken_head_mask.png",           L"image\\k1_m.png", L"image\\k1_b.png",      L"小鸡头＋ikun①" },
	{ L"image\\chicken_head.png",        L"image\\chicken_head_mask.png",           L"image\\k2_m.png", L"image\\k2_b.png",      L"小鸡头＋ikun②" },
	{ L"image\\chicken_head.png",        L"image\\chicken_head_mask.png",           L"image\\k3_m.png", L"image\\k3_b.png",      L"小鸡头＋ikun③" },
};
const int SNAKE_SKIN_COUNT = sizeof(SNAKE_SKINS) / sizeof(SNAKE_SKINS[0]);
int snakeSkinIndex = 0;   // 当前蛇皮肤

// 重新加载蛇头和蛇身（其它素材不动）
void loadSnakeSkin(int idx) {
	if (idx < 0 || idx >= SNAKE_SKIN_COUNT) idx = 0;
	snakeSkinIndex = idx;
	loadimage(&gugahead[0], SNAKE_SKINS[idx].head, 50, 50, true);
	loadimage(&gugaheadmask[0], SNAKE_SKINS[idx].headMask, 50, 50, true);
	loadimage(&guga[0], SNAKE_SKINS[idx].body, 50, 50, true);
	loadimage(&gugamask[0], SNAKE_SKINS[idx].bodyMask, 50, 50, true);
}
//----------------------------------------------------------
IMAGE gameback;
IMAGE ailis;
IMAGE ailishaixiu;
IMAGE ailisshengqi;
IMAGE ailisjinya;
IMAGE mask;
wchar_t text[100] = L"开始游戏";
wchar_t text1[100] = L"退出游戏";
wchar_t text2[100] = L"开发人员";
wchar_t text3[100] = L"排 行 榜";
wchar_t text4[100] = L"设    置";
wchar_t text5[100] = L"贪  吃  蛇";
int gamebegin = 0; //开始游戏
int gameexit = 0; //退出游戏
int member = 0; //开发人员
int rank = 0; //排行榜
int set = 0;//设置
int op = 1;
int kick = 0;
//------------------------数据设计------------------------
	//#define MAP_W    30  //地图宽度，x方向，列数
	//#define MAP_H    20  //地图高度，y方向，行数
	//#define SNAKE_MAX_LEN (MAP_W*MAP_H)    //蛇身最大长度
/*
* 坐标结构体
* x：列坐标，横向
* y: 行坐标，纵向
*/
typedef struct {
	int x;
	int y;
}point;
/*
* 苹果类型枚举：
* APPLE_RED 红苹果
* APPLE_YELLOW 黄苹果
* APPLE_BLUE 蓝苹果
*/
typedef enum {
	APPLE_RED = 0,
	APPLE_YELLOW,
	APPLE_BLUE
}AppleType;
/*
*食物结构体：
* pos表示食物坐标
* tyoe表示食物种类
*/
typedef struct {
	point pos;
	AppleType type;
}Food;
/*
* 方向枚举：DIR_UP 上，DIR_DOWN 下，DIR_LEFT 左，DIR_RIGHT 右
*/
typedef enum {
	DIR_UP = 0,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT
}Direction;
/*
*游戏状态枚举：
* STATE_RUNING 游戏中，STATE_PAUSE 暂停，STATE_OVER 游戏失败，STATE_WIN 游戏胜利
*/
typedef enum {
	STATE_RUNING = 0,
	STATE_PAUSE,
	STATE_OVER,
	STATE_WIN
}GameState;
/*
* 地图大小常量（可根据实际要求修改）
*/
#define MAP_W    20  
#define MAP_H    16  
#define SNAKE_MAX_LEN (14*18)    
/*
* 地图：board[j][i]表示坐标（i，j）的值
* 0表示空地，1表示蛇身，2表示红苹果，3表示黄苹果，4表示蓝苹果
* 如board[6][3]=2表示（3，6）的位置是红苹果
*/
int board[16][20];
/*
* 蛇身数据：snake数组存储蛇身坐标，snake[0]为蛇头，snake[len-1]为蛇尾
* 如snake[0].x=5,snake[0].y=5 表示蛇头坐标为（5，5）
*/
point snake[SNAKE_MAX_LEN];
//表示当前蛇的长度
//如：snakelen=3 表示当前蛇有3节（1个头，2个身子）
int snakelen;
//表示当前实际的移动方向
Direction dir;
//表示玩家输入的下一次移动方向
Direction nextdir;
//表示当前坐标上的食物（坐标和种类）
Food food;
//表示当前得分，如score=30 表示当前累计得分30
int score;
//表示游戏运行速度
int speed;
//表示游戏当前状态
GameState gamestate;
//音量大小0-100
int volumn = 100;
/*
* 难度级别：
* 0表示简单，1普通，2困难
*/
int difficulty;



//-----------------------数据设计-----------------------

//-----------------------service-----------------------
/*
* 负责人:keso
* 功能：init:初始化游戏数据
* 将地图的值初始化为0
* 初始化蛇的长度，位置，方向
* 当前分数设为0，速度设为20（可改），状态设为运行中
* 调用生产食物函数生成第一个食物
* 根据difficulty动态设置speed
* 参数 void
* 返回值 void
*/
void init();
/*
* 负责人：小新
* 功能 generationfood:生产食物
* 在界面的空地（值为0）随机生产一个食物
* 随机判断食物种类（红/黄/蓝）并更新到food结构体和board数组中
* 参数 void
* 返回值:void
*/
void generationfood();
/*
* 负责人：小禹
* 功能：checkcrash:判断传入的坐标是否会碰撞
* 参数：x:下一步蛇头的x坐标
*		y:下一步蛇头的y坐标
* 返回值：
* 0表示安全
* 1表示碰墙
* 2表示撞到自己
*/
int checkcrash(int y, int x);
/*
* 负责人：雪绒
* 功能：movesnake:蛇移动一步
* 根据当前方向判断新的蛇头坐标
* 若碰撞，返回0
* 若没有，将新蛇头加入，如果没吃到食物则删除蛇尾
* 如果吃到，根据不同苹果加分，加减速或变长，并重新生成食物
* 同步更新到board数组
* 参数void
* 返回值:0表示移动失败（游戏结束）1表示移动成功
*/
int movesnake();
/*
* 负责人：雪绒
* 功能：iswin：判断落点后胜利，如果蛇的长度等于地图总格子数，则视为获胜
* 参数:void
* 返回值：0表示没有获胜  1表示蛇占满屏幕获胜
*/
int iswin();
//-------------------------service--------------------------

//-------------------------view---------------------------

/*
* 负责人：惜悟
* 功能：menuview:展示选项，玩家可以在这里选择进入游戏，进入设置或退出游戏
* while(1){
*  1.展示选项
*  2.用户输入
*  3.根据输入进行对应处理
*	进入游戏：调用游戏界面函数gameview
*	进入设置：调用函数settingsview();
*	退出游戏：调用exit()
}
* 参数：void
* 返回值：void
*/
void menuview(ExMessage* msg);
/*
* 负责人：惜悟，四文鱼
* 功能：gameboardview:根据board数组打印初始界面，以#表示墙
* 打印蛇，以@@表示蛇头，*表示蛇身
* R/Y/B表示三种苹果
* 如果gamestate==STATE_PAUSE,在屏幕中央打印"已暂停，按p继续"
* 参数：void
* 返回值：void
*/
void gameboardview();
/*
* 负责人：澈
* 功能：winview:根据gamestate的值，打印游戏胜利或失败的界面
* 用户按p(调用getch函数）可随时退出当前界面
* 参数：void
* 返回值：void
*/
void winview();
/*
* 负责人：柒姩
* 功能:gameview:游戏界面整合
* 初始化游戏数据（调用函数init())
* while(gamestate==STATE_RUNNING||gamestate==STATE_PAUSE){
*打印游戏界面（调用函数gameboardview())
//-----暂停逻辑处理------
if(gamestate==STATE_PAUSE){
continue;//暂停时不执行后面的移动逻辑
}
//-----游戏运行逻辑--------
* 接受玩家输入（包含方向键以及P暂停键）
* 判断若与当前dir反向，则忽略
* 若按了P，将gamestate设为STATE_PAUSE;continue;
* 延时（sleep)
* 更新实际方向（dir=nextdir)
* 调用移动函数（movesnake())
* (如果移动失败，设gamestate=STATE_OVER,结束循环)
* （如果调用iswin（）成功，设gamestate=STATE_WIN,结束循环)}
* 调用胜利/失败界面函数（winview())
* 参数：void
* 返回值：void
*/
void gameview();
/*
* 负责人：澈
* 功能：settingsview:设置界面
* whuile(1){
* 打印当前音量和难度
* 接受输入（比如1调音量，2调难度，3返回主菜单)
* 修改volumn和difficulty的值
* }
* 参数：void
* 返回值：void
*/
void settingsview();
IMAGE red;
IMAGE redmask;
IMAGE yellow;
IMAGE yellowmask;
IMAGE man;
IMAGE manmask;
IMAGE gameMap;
//-----------------------view----------------------------
Direction d[1000];
int main() {
	initgraph(1000, 800);//初始化界面大小
	setbkmode(TRANSPARENT);//透明背景
	loadimage(&gameback, L"image\\gameBack.jpg", 1000, 800, true);
	loadimage(&ailis, L"image\\ailis.png", 400, 553, true);
	loadimage(&ailishaixiu, L"image\\ailishaixiu.png", 400, 553, true);
	loadimage(&ailisshengqi, L"image\\ailisshengqi.png", 400, 553, true);
	loadimage(&ailisjinya, L"image\\ailisjinya.png", 400, 553, true);
	loadimage(&mask, L"image\\mask.png", 400, 553, true);
	loadSnakeSkin(snakeSkinIndex);   // 蛇皮肤：蛇头 + 蛇身
	loadimage(&red, L"image\\red.png", 50, 50, true);
	loadimage(&redmask, L"image\\redmask.png", 50, 50, true);
	loadimage(&yellow, L"image\\yellow.png", 50, 50, true);
	loadimage(&yellowmask, L"image\\yellowmask.png", 50, 50, true);
	loadimage(&man, L"image\\man.png", 50, 50, true);
	loadimage(&manmask, L"image\\manmask.png", 50, 50, true);
	loadimage(&gameMap, L"image\\back.png", 1000, 800);
	loadimage(&img_settings, L"settings_bg.jpg", 1000, 800,true);
	loadimage(&img_win, L"win_bg.jpg", 1000, 800,true);
	loadimage(&img_stone, L"image\\stone.png", 50, 50, true);
	loadimage(&stonemask, L"image\\stonemask.png", 50, 50, true);
	wchar_t arr[100];
	menu:
	cnt1 = 0, cnt2 = 0, cnt3 = 0;
	d[0] = dir, d[1] = DIR_UP, d[2] = DIR_UP;
	ExMessage msg;
	menuview(&msg);
	if (gamebegin == 5) {
		gamebegin = 0;
		op = 1;
		cleardevice();
		gameboardview();
		generationfood();
		putimage(6 * 50, 8 * 50, 50, 50, &gugaheadmask[0], 0, 0, SRCAND);
		putimage(6 * 50, 8 * 50, 50, 50, &gugahead[0], 0, 0, SRCPAINT);
		putimage(6 * 50, 9 * 50, 50, 50, &gugamask[0], 0, 0, SRCAND);
		putimage(6 * 50, 9 * 50, 50, 50, &guga[0], 0, 0, SRCPAINT);
		putimage(6 * 50, 10 * 50, 50, 50, &gugamask[0], 0, 0, SRCAND);
		putimage(6 * 50, 10 * 50, 50, 50, &guga[0], 0, 0, SRCPAINT);
		//swprintf(arr, 100, L"setaudio BGM volume to %d", volumn);
		//mciSendString(L"open music// alias BGM type mpegvideo", 0, 0, 0);
		//mciSendString(arr, 0, 0, 0);
		//mciSendString(L"play BGM repeat", 0, 0, 0);
		gameview();
		mciSendString(L"close BGM", 0, 0, 0);
		mciSendString(L"open music//guga.wav alias guga type mpegvideo", 0, 0, 0);
		mciSendString(L"play guga from 3870", 0, 0, 0);
		Sleep(3300);
		mciSendString(L"close guga", 0, 0, 0);
		winview();
		goto menu;
	}
	if (gameexit) {
		gameexit = 0;
		op = 1;
		exit(0);
	}
	if (member) {
		member = 0;
		op = 1;
	}
	if (rank) {
		rank = 0;
		op = 1;
	}
	if (set) {
		set = 0;
		op = 1;
		settingsview();
		goto menu;
	}
	_getch();
	closegraph();
}
//下面是各个函数的实现部分
void init() {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 20; j++) {
			board[i][j] = 0;
		}
	}
	//for (int i = 0; i < 16; i++) {
	//	for (int j = 0; j < 20; j++) {
	//		if (i == 0 || i == 15) {
	//			board[i][j] = 1;
	//		}
	//		else {
	//			board[i][0] = 1;
	//			board[i][19] = 1;
	//		}
	//	}
	//}
	gamestate = STATE_RUNING;
	snakelen = 3;
	snake[0].x = 6;
	snake[0].y = 8;
	snake[1].x = 6;
	snake[1].y = 9;
	snake[2].x = 6;
	snake[2].y = 10;
	YELLOWAPPLE_BUFF = 0;
	NOW_speed = 0;
	dir = DIR_UP;
	nextdir = dir;
	score = 0;
	speed = 300;
	for (int i = 0; i < snakelen; i++) {
		board[snake[i].y][snake[i].x] = 1;
	}
	for (int i = 0;i < 15;i++) {
		int rx = rand() % 18+1; //20是列数
		int ry = rand() % 14+1;//16是行数
		//必须是空地才放石头
		if (board[ry][rx] == 0) {
			//防呆：如果离蛇的出生点太近，就重新抽
			if (abs(rx - 6) <= 2 && abs(ry - 8) <= 2) {
				i--;
			}
			else {
				board[ry][rx] = 5;//标记为石头
				//画石头
				putimage(50 * rx, 50 * ry, 50, 50, &stonemask, 0, 0, SRCAND);
				putimage(50 * rx, 50 * ry, 50, 50, &img_stone, 0, 0, SRCPAINT);
			}
		}
		else {
			i--;//如果这里已经有东西了（不是空地），也作废重抽
		}
	}


	generationfood();
	if (difficulty == 0) {
		speed = 150;
	}
	else if (difficulty == 1) {
		speed = 100;
	}
	else if (difficulty == 2) {
		speed = 70;
	}
}












void generationfood() {
	int x = 0, y = 0;
	while (1)
	{
		x = rand() % 18 + 1;
		y = rand() % 14 + 1;
		if (board[y][x] == 0) {
			food.pos.x = x;
			food.pos.y = y;
			break;
		}
	}
	food.pos.x = x;
	food.pos.y = y;
	food.type = (AppleType)(rand() % 3);
	if (food.type == APPLE_RED) {
		board[y][x] = 2;
		putimage(50 * x, 50 * y, 50, 50, &redmask, 0, 0, SRCAND);
		putimage(50 * x, 50 * y, 50, 50, &red, 0, 0, SRCPAINT);
	}
	if (food.type == APPLE_YELLOW) {
		board[y][x] = 3;
		putimage(50 * x, 50 * y, 50, 50, &yellowmask, 0, 0, SRCAND);
		putimage(50 * x, 50 * y, 50, 50, &yellow, 0, 0, SRCPAINT);
	}
	if (food.type == APPLE_BLUE) {
		board[y ][x ] = 4;
		putimage(50 * x, 50 * y, 50, 50, &manmask, 0, 0, SRCAND);
		putimage(50 * x, 50 * y, 50, 50, &man, 0, 0, SRCPAINT);
	}
}
void generationoriginalfood() {
	if (food.type == APPLE_RED) {
		board[food.pos.y][food.pos.x] = 2;
		putimage(50 * food.pos.x, 50 * food.pos.y, 50, 50, &redmask, 0, 0, SRCAND);
		putimage(50 * food.pos.x, 50 * food.pos.y, 50, 50, &red, 0, 0, SRCPAINT);
	}
	if (food.type == APPLE_YELLOW) {
		board[food.pos.y][food.pos.x] = 3;
		putimage(50 * food.pos.x, 50 * food.pos.y, 50, 50, &yellowmask, 0, 0, SRCAND);
		putimage(50 * food.pos.x, 50 * food.pos.y, 50, 50, &yellow, 0, 0, SRCPAINT);
	}
	if (food.type == APPLE_BLUE) {
		board[food.pos.y][food.pos.x] = 4;
		putimage(50 * food.pos.x, 50 * food.pos.y, 50, 50, &manmask, 0, 0, SRCAND);
		putimage(50 * food.pos.x, 50 * food.pos.y, 50, 50, &man, 0, 0, SRCPAINT);
	}
}










int checkcrash(int y, int x) {
	//蛇头的（下一步）坐标与墙重合时
	if (y == 0 || y == 15 || x == 0 || x == 19)
		return 1;
	//新增：蛇头的下一步坐标与石头（5）重合时
	if (board[y][x] == 5) {
		return 1;
	}
	//蛇头的（下一步）坐标与蛇身重合时
	for (int i = 1; i < snakelen; i++) {
		if (x == snake[i].x && y == snake[i].y) {
			return 1;
		}
	}
	//蛇头的（下一步）坐标合法时
	return 0;
}







int movesnake() {
	int i, eat_food = 0;
	//k数组存蛇尾的坐标
	int k[2] = { snake[snakelen - 1].x,snake[snakelen - 1].y };

	//更改蛇身
	for (i = snakelen-1; i >0; i--) {
		snake[i] = snake[i - 1];
	}



	//更改蛇头
	switch (dir) {
	case DIR_UP:
		snake[0].y--;
		break;
	case DIR_DOWN:
		snake[0].y++;
		break;
	case DIR_LEFT:
		snake[0].x--;
		break;
	case DIR_RIGHT:
		snake[0].x++;
		break;
	}
	if (checkcrash(snake[0].y, snake[0].x)) {
		return 0;
	}
	switch (board[snake[0].y][snake[0].x]) {
		//红苹果
	case 2:
		if (YELLOWAPPLE_BUFF) score += 2;//黄苹果增益期间积分翻倍
		else score++;
		snakelen++;
		snake[snakelen - 1].x = k[0];
		snake[snakelen - 1].y = k[1];
		eat_food = 1;
		//更新食物坐标，重新生成食物
		board[snake[0].y][snake[0].x] = 0;
		//generationfood();
		break;
		//黄苹果
	case 3:
		score += 2;
		//黄苹果增益:在一定时间内移动速度增加，积分翻倍
		YELLOWAPPLE_BUFF = 5;
		NOW_speed = speed;
		speed-=15;
		//更新食物坐标，重新生成食物
		board[snake[0].y][snake[0].x] = 0;
		//generationfood();
		break;
		//蓝苹果
	case 4:
		score++;
		speed+=20;
		//更新食物坐标，重新生成食物
		board[snake[0].y][snake[0].x] = 0;
		break;
		//generationfood();
	}
	YELLOWAPPLE_BUFF--;
	//黄苹果buff消失，蛇速度恢复正常
	if (YELLOWAPPLE_BUFF == 0) {
		speed = NOW_speed;
	}

	//如果没吃到苹果，清除旧的蛇尾

	if (!eat_food) board[k[1]][k[0]] = 0;

	//更新新的蛇身坐标
	for (i = 0; i < snakelen; i++) {
		board[snake[i].y][snake[i].x] = 1;
	}


	BeginBatchDraw();
	cleardevice();
	gameboardview();
	if (food.pos.x == snake[0].x && food.pos.y == snake[0].y) {
		generationfood();
	}
	else {
		generationoriginalfood();
	}
	putimage(50 * (snake[0].x), 50 * (snake[0].y), 50, 50, &gugaheadmask[0], 0, 0, SRCAND);
	putimage(50 * (snake[0].x), 50 * (snake[0].y), 50, 50, &gugahead[0], 0, 0, SRCPAINT);


	for (int i = 1; i < snakelen; i++) {
		putimage(50 * (snake[i].x), 50 * (snake[i].y), 50, 50, &gugamask[0], 0, 0, SRCAND);
		putimage(50 * (snake[i].x), 50 * (snake[i].y), 50, 50, &guga[0], 0, 0, SRCPAINT);
		FlushBatchDraw();
	}
	EndBatchDraw();




	return 1;
}




int iswin() {
	if (SNAKE_MAX_LEN == snakelen) {
		return 1;
	}
	else return 0;
}





void menuview(ExMessage* msg) {
	srand((int)time(0));//设置随机种子
	int x = 400, y = 200, w = 200, h = 100;
	if (cnt2 == 0) {
		initgraph(1000, 800);
		cnt2++;
	}
	setbkmode(TRANSPARENT);//背景透明
	BeginBatchDraw();
	//绘制开始游戏按键
	setfillcolor(0x000000);
	setlinecolor(0);
	fillrectangle(x + t, y + t, x + w + t, y + h + t);
	settextcolor(0xffffff);
	outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
	//退出游戏文本位置 
	settextcolor(0);
	settextstyle(25, 20, L"黑体");
	outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
	//开发人员文本位置
	outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
	//排行榜文本位置
	outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
	//菜单文本位置
	outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
	putimage(0, 0, &gameback);
	putimage(100, 433, 586, 533, &mask, 0, 0, SRCAND);
	putimage(100, 433, 586, 533, &ailis, 0, 0, SRCPAINT);
	FlushBatchDraw();
	Sleep(500);
	//人物跳动及音效
	if (start == 1) {
		mciSendString(L"open music//ailis1.wav alias ailis1 type mpegvideo", 0, 0, 0);
		ouact(100, 433, 486, 533, 2, 100, &ailis, &mask, &gameback);
		mciSendString(L"open music//ailis2.wav alias ailis2 type mpegvideo", 0, 0, 0);
		mciSendString(L"play ailis2 from 9000 ", 0, 0, 0);
		Sleep(3000);
		mciSendString(L"close ailis2", 0, 0, 0);
		mciSendString(L"play ailis1 from 18600 ", 0, 0, 0);
		Sleep(5000);
		mciSendString(L"close ailis1", 0, 0, 0);
		start++;
	}
	EndBatchDraw();
	while (op) {
		BeginBatchDraw();
		cleardevice();
		putimage(0, 0, &gameback);
		//贪吃蛇文本位置
		settextstyle(35, 40, L"楷体");
		settextcolor(0xFF0000);
		outtextxy(500 - textwidth(text5) / 2, 100 - textheight(text5) / 2, text5);
		settextstyle(25, 20, L"黑体");
		//根据人物点击次数设置人物live2d的图片
		if (kick == 0) {
			putimage(100, 433, 586, 533, &mask, 0, 0, SRCAND);
			putimage(100, 433, 586, 533, &ailis, 0, 0, SRCPAINT);
		}
		else if (kick == 1) {
			putimage(100, 433, 586, 533, &mask, 0, 0, SRCAND);
			putimage(100, 433, 586, 533, &ailisjinya, 0, 0, SRCPAINT);
		}
		else if (kick == 2 || kick == 3 || kick == 4) {
			putimage(100, 433, 586, 533, &mask, 0, 0, SRCAND);
			putimage(100, 433, 586, 533, &ailishaixiu, 0, 0, SRCPAINT);
		}
		else if (kick == 5) {
			putimage(100, 433, 586, 533, &mask, 0, 0, SRCAND);
			putimage(100, 433, 586, 533, &ailisshengqi, 0, 0, SRCPAINT);
		}
		//接受鼠标信息
		peekmessage(msg, EX_MOUSE);
		switch (msg->message) {
			//鼠标移动
		case WM_MOUSEMOVE:
			if (msg->x > x + t && msg->x < x + w + t && msg->y > y + t && msg->y < y + h + t) {
				setfillcolor(0xffffff);
				setlinecolor(0xffffff);
				fillrectangle(x + t, y + t, x + w + t, y + h + t);
				settextcolor(0x000000);
				outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
				settextcolor(0);
				outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
				outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
				outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
				outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
			}
			else if (msg->x > 26 && msg->x < 180 && msg->y > 588 && msg->y < 633) {
				setfillcolor(0x000000);
				setlinecolor(0xffffff);
				fillrectangle(x + t, y + t, x + w + t, y + h + t);
				settextcolor(0xffffff);
				outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
				settextcolor(0x456789);
				outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
				settextcolor(0);
				outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
				outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
				outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
			}
			else if (msg->x > 26 && msg->x < 180 && msg->y > 633 && msg->y < 673) {
				setfillcolor(0x000000);
				setlinecolor(0xffffff);
				fillrectangle(x + t, y + t, x + w + t, y + h + t);
				settextcolor(0xffffff);
				outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
				settextcolor(0);
				outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
				settextcolor(0x456789);
				outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
				settextcolor(0);
				outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
				outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
			}
			else if (msg->x > 26 && msg->x < 180 && msg->y > 673 && msg->y < 713) {
				setfillcolor(0x000000);
				setlinecolor(0xffffff);
				fillrectangle(x + t, y + t, x + w + t, y + h + t);
				settextcolor(0xffffff);
				outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
				settextcolor(0);
				outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
				outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
				settextcolor(0x456789);
				outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
				settextcolor(0);
				outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
			}
			else if (msg->x > 26 && msg->x < 180 && msg->y > 713 && msg->y < 753) {
				setfillcolor(0x000000);
				setlinecolor(0xffffff);
				fillrectangle(x + t, y + t, x + w + t, y + h + t);
				settextcolor(0xffffff);
				outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
				settextcolor(0);
				outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
				outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
				outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
				settextcolor(0x456789);
				outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
			}
			else {
				setfillcolor(0x000000);
				setlinecolor(0xffffff);
				fillrectangle(x + t, y + t, x + w + t, y + h + t);
				settextcolor(0xffffff);
				outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
				settextcolor(0);
				outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
				outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
				outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
				outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
			}
			break;
			//鼠标抬起
		case WM_LBUTTONUP:
			//进入游戏
			if (msg->x > x + t && msg->x < x + w + t && msg->y > y + t && msg->y < y + h + t) {
				setfillcolor(0xffffff);
				setlinecolor(0xffffff);
				fillrectangle(x + t, y + t, x + w + t, y + h + t);
				settextcolor(0x000000);
				outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
				settextcolor(0);
				outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
				outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
				outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
				outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
				gamebegin++;
				t = rand() % x + y + 1 - w;
				if (gamebegin == 5) {
					op = 0;
					t = 0;
				}
			}
			//退出游戏
			else if (msg->x > 26 && msg->x < 180 && msg->y > 588 && msg->y < 633) {
				setfillcolor(0x000000);
				setlinecolor(0xffffff);
				fillrectangle(x + t, y + t, x + w + t, y + h + t);
				settextcolor(0xffffff);
				outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
				settextcolor(0x456789);
				outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
				settextcolor(0);
				outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
				outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
				outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
				gameexit = 1;
				if (gameexit) {
					op = 0;
				}
			}
			//开发人员
			else if (msg->x > 26 && msg->x < 180 && msg->y > 633 && msg->y < 673) {
				setfillcolor(0x000000);
				setlinecolor(0xffffff);
				fillrectangle(x + t, y + t, x + w + t, y + h + t);
				settextcolor(0xffffff);
				outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
				settextcolor(0);
				outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
				settextcolor(0x456789);
				outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
				settextcolor(0);
				outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
				outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
				member = 1;
				if (member) {
					op = 0;
				}
			}
			//排行榜
			else if (msg->x > 26 && msg->x < 180 && msg->y > 673 && msg->y < 713) {
				setfillcolor(0x000000);
				setlinecolor(0xffffff);
				fillrectangle(x + t, y + t, x + w + t, y + h + t);
				settextcolor(0xffffff);
				outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
				settextcolor(0);
				outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
				outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
				settextcolor(0x456789);
				outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
				settextcolor(0);
				outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
				rank = 1;
				if (rank) {
					op = 0;
				}
			}
			//设置
			else if (msg->x > 26 && msg->x < 180 && msg->y > 713 && msg->y < 753) {
				setfillcolor(0x000000);
				setlinecolor(0xffffff);
				fillrectangle(x + t, y + t, x + w + t, y + h + t);
				settextcolor(0xffffff);
				outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
				settextcolor(0);
				outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
				outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
				outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
				settextcolor(0x456789);
				outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
				set = 1;
				if (set) {
					op = 0;
				}
			}
			//点击人物
			else if (msg->x > 323 && msg->x < 343 && msg->y > 473 && msg->y < 503) {
				kick++;
				mciSendString(L"open music//ailis.wav alias ailis type mpegvideo", 0, 0, 0);
				mciSendString(L"open music//niganma.wav alias niganma type mpegvideo", 0, 0, 0);
				if (kick == 1) {
					BeginBatchDraw();
					lract(100, 433, 486, 533, 2, 100, &ailisjinya, &mask, &gameback);
					EndBatchDraw();
					mciSendString(L"play ailis from 16100 ", 0, 0, 0);
					Sleep(500);
					mciSendString(L"close ailis ", 0, 0, 0);
				}
				else if (kick == 2) {
					BeginBatchDraw();
					lract(100, 433, 486, 533, 2, 100, &ailishaixiu, &mask, &gameback);
					EndBatchDraw();
					mciSendString(L"play ailis from 3500 ", 0, 0, 0);
					Sleep(500);
					mciSendString(L"close ailis ", 0, 0, 0);
				}
				else if (kick == 3) {
					BeginBatchDraw();
					lract(100, 433, 486, 533, 2, 100, &ailishaixiu, &mask, &gameback);
					EndBatchDraw();
					mciSendString(L"play ailis from 8000 ", 0, 0, 0);
					Sleep(1000);
					mciSendString(L"close ailis ", 0, 0, 0);
					mciSendString(L"play niganma from 0 ", 0, 0, 0);
					Sleep(1850);
					mciSendString(L"close niganma ", 0, 0, 0);
					ikun();
				}
				else if (kick == 4) {
					BeginBatchDraw();
					lract(100, 433, 486, 533, 2, 100, &ailishaixiu, &mask, &gameback);
					EndBatchDraw();
					mciSendString(L"play ailis from 5840 ", 0, 0, 0);
					Sleep(500);
					mciSendString(L"close ailis ", 0, 0, 0);
				}
				else if (kick == 5) {
					kick = 0;
					BeginBatchDraw();
					lract(100, 433, 486, 533, 2, 100, &ailisshengqi, &mask, &gameback);
					EndBatchDraw();
					mciSendString(L"play ailis from 17000 ", 0, 0, 0);
					Sleep(13000);
					mciSendString(L"close ailis ", 0, 0, 0);
				}
				setfillcolor(0x000000);
				setlinecolor(0xffffff);
				fillrectangle(x + t, y + t, x + w + t, y + h + t);
				settextcolor(0xffffff);
				outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
				settextcolor(0);
				outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
				outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
				outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
				outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
			}
			else {
				setfillcolor(0x000000);
				setlinecolor(0xffffff);
				fillrectangle(x + t, y + t, x + w + t, y + h + t);
				settextcolor(0xffffff);
				outtextxy(x + y / 2 + t - textwidth(text) / 2, y + h / 2 + t - textheight(text) / 2, text);
				settextcolor(0);
				outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
				outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
				outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
				outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
			}
			break;
		}
		FlushBatchDraw();
		EndBatchDraw();
	}
}



void gameboardview() {
	wchar_t scoreArry[100];
	swprintf(scoreArry, 100, L"%d", score);//outtextxy函数只能打印字符串，所以用这个把int 类的score转化为宽字符
	if (cnt1 == 0) {
		initgraph(1000, 900);
		cnt1++;
	}
	//加载游戏地图
	putimage(0, 0, &gameMap);
	//新增：把棋盘上的石头重画一遍（不然每帧被cleardevice覆盖）
	for (int y = 0;y < 16;y++) {
		for (int x = 0;x < 20;x++) {
			if (board[y][x] == 5) {
				putimage(50 * x, 50 * y, 50, 50, &stonemask, 0, 0, SRCAND);
				putimage(50 * x, 50 * y, 50, 50, &img_stone, 0, 0, SRCPAINT);
			}
		}
	}



	//填充剩余黑色区域
	setfillcolor(0xFFFFFF);
	fillrectangle(0, 800, 1000, 900);
	setfillcolor(0x000000);
	//设置打印积分的背景和颜色
	settextcolor(0x000000);
	setbkmode(TRANSPARENT);
	//打印积分
	outtextxy(200, 850, scoreArry);
	//打印文字提示
	outtextxy(300, 850, L"按P以暂停");
	//打印蛇头
	IMAGE sankeHade;
	//loadimage(&sankeHade, L"image\\gugaheadw.png", snake[0].x, snake[0].y);
	//打印蛇身体
	IMAGE sankeBody;
	//for (int i = snakelen - 1; i > 0; i--) {//要把全部的身体打印出来，所以直到蛇的长度不是0时都要打印
	//	loadimage(&sankeHade, L"image\\gugaw.png", snake[i].x, snake[i].y);
	//}
	//当游戏状态为暂停时循环该段以达成暂停时游戏不动的目的
	while (gamestate == STATE_PAUSE) {
		outtextxy(200, 850, L"已暂停，按p继续");
		ExMessage continueMsg;
		getmessage(&continueMsg, EX_KEY);
		if (continueMsg.vkcode == 86) {//判断是否输入的是p键，若是则跳出循环
			gamestate = STATE_RUNING;//将游戏状态重新改为继续状态
		}
	}

}









void settingsview() {
	int currentVol = volumn;
	int currentDiff = difficulty;
	BeginBatchDraw();
	while (1) {
		cleardevice();
		putimage(0, 0, &img_settings);//贴奶龙背景图
		settextcolor(BLACK);
		settextstyle(45, 0, L"黑体");
		wchar_t buf[100];//拼接文字和数字
		//打印音量
		wsprintf(buf, L"当前音量:%d", currentVol);
		outtextxy(280, 350, buf);
		outtextxy(500, 350, L"  (按W/S调节）");
		//显示难度
		wsprintf(buf, L"当前难度:%d", currentDiff);
		outtextxy(280, 420, buf);
		outtextxy(500, 420, L"  (按A/D调节)");
		//显示蛇皮肤（名称单独一行，提示放下一行避免重叠）
		wsprintf(buf, L"蛇皮肤:%s", SNAKE_SKINS[snakeSkinIndex].name);
		outtextxy(280, 490, buf);
		settextstyle(30, 0, L"黑体");
		outtextxy(300, 552, L"(按 Q / E 切换皮肤)");
		//底部提示语
		settextstyle(30, 0, L"宋体");
		settextcolor(RGB(80, 80, 80));
		outtextxy(320, 700, L"按空格键保存并返回主菜单");
		FlushBatchDraw();
		//接受键盘消息（非阻塞）
		flushmessage(EX_KEY | EX_CHAR);
		ExMessage msg;
		do {
			getmessage(&msg, EX_KEY);
		} while (msg.message != WM_KEYDOWN || msg.prevdown);
		if (msg.message == WM_KEYDOWN) {
			if (msg.vkcode == 'w' || msg.vkcode == 'W') {
				currentVol = currentVol + 10;
				if (currentVol > 100) { currentVol = 100; }
			}
			else if (msg.vkcode == 's' || msg.vkcode == 'S') {
				currentVol = currentVol - 10;
				if (currentVol < 0) { currentVol = 0; }
			}
			else if (msg.vkcode == 'a' || msg.vkcode == 'A') {
				currentDiff = currentDiff - 1;
				if (currentDiff < 0) { currentDiff = 0; };
			}
			else if (msg.vkcode == 'd' || msg.vkcode == 'D') {
				currentDiff = currentDiff + 1;
				if (currentDiff > 2) { currentDiff = 2; };
			}
			else if (msg.vkcode == 'q' || msg.vkcode == 'Q') {//切换蛇皮肤
				loadSnakeSkin((snakeSkinIndex - 1 + SNAKE_SKIN_COUNT) % SNAKE_SKIN_COUNT);
			}
			else if (msg.vkcode == 'e' || msg.vkcode == 'E') {
				loadSnakeSkin((snakeSkinIndex + 1) % SNAKE_SKIN_COUNT);
			}
			else if (msg.vkcode == ' ') {
				volumn = currentVol;
				difficulty = currentDiff;
				break;
			}
		}
		Sleep(10);
	}
	EndBatchDraw();
}



void winview() {
	if (cnt3 == 0) {
		initgraph(1000, 800);
		cnt3++;
	}
	cleardevice();//清屏
	flushmessage(EX_KEY | EX_CHAR);
	putimage(0, 0, &img_win);//贴张晓英图片
	//判断输赢，打印大字
	settextstyle(100, 0, L"黑体");
	if (gamestate == STATE_WIN) {
		settextcolor(RGB(255, 215, 0));//金色
		outtextxy(300, 100, L"大获全胜！");
	}
	else {
		settextcolor(RGB(255, 50, 50));//红色
		outtextxy(300, 100, L"行动失败...");
	}
	//打印最终得分
	settextstyle(60, 0, L"黑体");
	settextcolor(WHITE);
	wchar_t scoreText[100];
	wsprintf(scoreText, L"最终得分:%d", score);
	outtextxy(350, 250, scoreText);
	//底部提示
	settextstyle(35, 0, L"宋体");
	settextcolor(RGB(200, 200, 200));
	outtextxy(320, 700, L"按任意键返回主菜单...");
	//卡住界面，按任意键才退出
	ExMessage msg;
	do {
		getmessage(&msg, EX_KEY);
	} while (msg.message != WM_KEYDOWN || msg.prevdown);
}












void gameview() {
	//初始化游戏数据（调用 init 重置蛇、分数、食物）
	init();
	flushmessage(EX_KEY | EX_CHAR);

	//记录游戏开始时的时间（用于后续控制蛇的移动速度）
	DWORD last_move_time = GetTickCount();

	//进入游戏主循环（只要没死、没赢，就一直工作）
	while (gamestate == STATE_RUNING || gamestate == STATE_PAUSE) {
		// 打印游戏界面，把最新的地图、蛇、苹果画出来
		//gameboardview();
		//FlushBatchDraw(); // EasyX ：把画好的内容贴到屏幕上

		ExMessage msg;
		while (peekmessage(&msg, EX_KEY)) {
			if (msg.message == WM_KEYDOWN) {
				//  P 键暂停
				if (msg.vkcode == 'P') {
					if (gamestate == STATE_RUNING) {
						gamestate = STATE_PAUSE;
					}
					else {
						gamestate = STATE_RUNING;
						last_move_time = GetTickCount(); // 恢复时重置计时器，防止蛇瞬移
					}
				}

				//  ESC 键直接结束
				if (msg.vkcode == VK_ESCAPE) {
					gamestate = STATE_OVER;
					return; // 直接结束当前界面，去结算
				}

				//  WASD 改变方向（防反向自杀）
				if (!(msg.message != WM_KEYDOWN || msg.prevdown)) {
					if (gamestate == STATE_RUNING) {
						if (msg.vkcode == 'W' && dir != DIR_DOWN) nextdir = DIR_UP;
						if (msg.vkcode == 'S' && dir != DIR_UP) nextdir = DIR_DOWN;
						if (msg.vkcode == 'A' && dir != DIR_RIGHT) nextdir = DIR_LEFT;
						if (msg.vkcode == 'D' && dir != DIR_LEFT) nextdir = DIR_RIGHT;
					}
				}
			}
		}
		// 如果按了 P 处于暂停状态，直接跳过下面的移动代码，重新进入循环画图
		//-----暂停逻辑处理------
		//if (gamestate == STATE_PAUSE) {
		//	continue;//暂停时不执行后面的移动逻辑

		if (gamestate == STATE_PAUSE) {
			Sleep(10);
			continue;  // 跳过后面的移动逻辑
		}

		// 判断当前时间距离上一次移动的时间，是否超过了设定的速度
		if (GetTickCount() - last_move_time >= speed) {

			// 时间到了，记录新的时间
			last_move_time = GetTickCount();

			// 更新实际方向
			dir = nextdir;
			

			// 调用移动函数（如果返回0，说明撞死了）
			if (movesnake() == 0) {
				gamestate = STATE_OVER; // 状态改为游戏失败
				break; // 直接跳出 while 循环，去结算
			}

			// 判断是否胜利
			if (iswin()) {
				gamestate = STATE_WIN;// 状态改为游戏胜利
				break; // 跳出循环
			}
		}

		Sleep(10); // 循环尾部延时
	}
	// 跳出上面的 while 循环，死了或赢了
// 调用 winview 展示胜利或失败界面
}





void lract(int x, int y, int w, int h, int k, int m, IMAGE* image, IMAGE* mask, IMAGE* bg)
{
	const int off[8] = { -m, -2 * m, -m, 0, m, 2 * m, m, 0 };

	for (int i = 0; i < k; i++) {
		for (int s = 0; s < 8; s++) {
			putimage(0, 0, bg, SRCCOPY);
			setfillcolor(0x000000);
			setlinecolor(0xffffff);
			fillrectangle(400 + t, 200 + t, 400 + 200 + t, 200 + 100 + t);
			settextcolor(0xffffff);
			outtextxy(400 + 200 / 2 + t - textwidth(text) / 2, 200 + 100 / 2 + t - textheight(text) / 2, text);
			settextcolor(0);
			outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
			outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
			outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
			outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
			settextstyle(35, 40, L"楷体");
			settextcolor(0xFF0000);
			outtextxy(500 - textwidth(text5) / 2, 100 - textheight(text5) / 2, text5);
			settextstyle(25, 20, L"黑体");


			putimage(x + off[s], y, w, h, mask, 0, 0, SRCAND);
			putimage(x + off[s], y, w, h, image, 0, 0, SRCPAINT);

			FlushBatchDraw();
			Sleep(30);
		}
	}
}
void ouact(int x, int y, int w, int h, int k, int m, IMAGE* image, IMAGE* mask, IMAGE* bg) //k为次数  m为距离 
{
	const int arr[4] = { m, 2 * m, m, 0 };

	for (int i = 0; i < k; i++) {
		for (int s = 0; s < 4; s++) {
			putimage(0, 0, bg);
			setfillcolor(0x000000);
			setlinecolor(0xffffff);
			fillrectangle(400 + t, 200 + t, 400 + 200 + t, 200 + 100 + t);
			settextcolor(0xffffff);
			outtextxy(400 + 200 / 2 + t - textwidth(text) / 2, 200 + 100 / 2 + t - textheight(text) / 2, text);
			settextcolor(0);
			outtextxy(100 - textwidth(text1) / 2, 600 - textheight(text1) / 2, text1);
			outtextxy(100 - textwidth(text2) / 2, 650 - textheight(text2) / 2, text2);
			outtextxy(100 - textwidth(text3) / 2, 700 - textheight(text3) / 2, text3);
			outtextxy(100 - textwidth(text4) / 2, 740 - textheight(text4) / 2, text4);
			settextstyle(35, 40, L"楷体");
			settextcolor(0xFF0000);
			outtextxy(500 - textwidth(text5) / 2, 100 - textheight(text5) / 2, text5);
			settextstyle(25, 20, L"黑体");

			putimage(x, y - arr[s], w, h, mask, 0, 0, SRCAND);
			putimage(x, y - arr[s], w, h, image, 0, 0, SRCPAINT);

			FlushBatchDraw();
			Sleep(60);
		}
	}
}
void ikun() {
	IMAGE bg;
	loadimage(&bg, L"image/bg.jpg", 1000, 800, true);
	putimage(0, 0, &bg);
	IMAGE arr_m[4];
	IMAGE arr_b[4];

	//加载掩码图和主体图片
	for (int i = 0; i < 4; i++) {
		wchar_t cmd[128] = { 0 };
		wsprintf(cmd, L"image/k%d_m.png", i + 1);
		loadimage(&arr_m[i], cmd, 200, 350, true);
		wsprintf(cmd, L"image/k%d_b.png", i + 1);
		loadimage(&arr_b[i], cmd, 200, 350, true);
	}
	mciSendString(L"open music//ikun.wav alias ikun type mpegvideo", 0, 0, 0);
	mciSendString(L"play ikun from 23000", 0, 0, 0);
	int j = 0;
	BeginBatchDraw();
	while (1) {
		j += 4;
		for (int i = 0; i < 4; i++) {
			putimage(0, 0, &bg);

			putimage(407, 417, &arr_b[i], SRCAND);

			putimage(407, 417, &arr_m[i], SRCPAINT);

			FlushBatchDraw();
			Sleep(200);
			EndBatchDraw();
		}
		if (j == 60) goto out;
	}
out:
	mciSendString(L"close ikun", 0, 0, 0);
	cleardevice();
}
