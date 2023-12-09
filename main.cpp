/*
* 天天酷跑开发日志-------星河一叶.Ck[UV](薛)
	1.创建项目
	2.导入素材
	3.创建游戏界面
	4.开发流程
		对于初学者，最好的方式，建议从用户界面入手

		选择图形库或其它引擎
		本次采用的是“easyx”图形库
----------------------------星河一叶.CK(薛)[UV]

		1） 创建游戏窗口
		2） 设计游戏背景
			a.三重背景不同速度同时移动
			b.循环滚动背景的实现，让背景就是循环但不那么单调
		3） 实现游戏背景
			a.加载背景资源
			b.渲染(背景知识：坐标）
			遇到问题：背景图片的png格式图片出现黑色
		4） 实现玩家奔跑
			a.加载图片资源
		5） 实现玩家的跳跃
		6)	实现障碍物-小乌龟 随机出现
		7)	对障碍物进行封装 代码优化
				a.先定义枚举 和 结构体 
				b. 对障碍物进行初始化
				c.代码的更换 换成封装代码
		8)	实现玩家下蹲
		9)	实现柱子障碍物
		10）实现障碍物的频率优化
		11)  实现碰撞检测
		12） 实现音效
		13） 修复音效bug，优化下蹲
		14） 增加血条功能
		15)	  添加游戏死亡的判断（结束判断）
		16） 添加背景音乐以及游戏初始化界面
*/

#include<stdio.h>
#include<graphics.h>
#include<conio.h>
#include<vector>
#include"tools.h"

using namespace std;

// 这里采用宏的预编译 为的是更好的维护
#define WIN_SCORE 100

#define WIN_WIDTH 1012
#define	 WIN_HEIGHT 396 
#define OBSTACLE_COUNT 10

IMAGE imaBgs[3]; // 因为在其他地方肯定也要用到这个 所以直接定义为全局变量					存储背景图片
int bgX[3];		// 存放背景图片X坐标
int bgspeed[3] = { 1,2,4 }; // 背景移动的速度进行控制 里面的单位是像素

IMAGE bgHeros[12];
int HeroX;
int HeroY;
int HeroIndex;

bool heroJump;
bool herodown;

int jumpHeightMax;
int jumpHeightOff;

int update; // 用来判断是否要刷新

//IMAGE Tortoise[7]; // 小乌龟
//int TortoiseX;	// 小乌龟的水平坐标
//int TortoiseY;	// 小乌龟的垂直坐标
//bool torToiseExist;	// 小乌龟是否存在
//int torToiseIndex;

typedef enum {
	// 枚举
	TORTOISE,	// 乌龟 0
	LION,			// 狮子 1
	HOOK1,
	HOOK2,
	HOOK3,
	HOOK4,
	OBSTACLE_TYPE_COUNT			// 6    数字自动加 枚举特性  最后一个就可以直到这个全部的个数了
}	obstacle_type; // 这样子就可以直接用这个表示障碍物类型了

// 存放各个障碍物的图片
vector<vector<IMAGE>> obstacleImags; // 在vector一个数组内有成员 每个成员又都是数组 此乃二维数组

// 这里typedef 是用于别名 也就是可以别的名字也可以调用这个结构体
typedef struct obstacle
{
	int type;	// 障碍物类型
	int imaIndex;  //当前显示图片的序号
	int x, y;	// 障碍物的水平坐标和垂直坐标
	int speed;
	int power;  // 杀伤力
	bool exist;
	bool hited;	// 表示是否已经发生了碰撞
	bool passed; // 表示是否以及被通过
	// IMAGE img[12];
} obstacle_t;

obstacle_t obstacles[OBSTACLE_COUNT];
int lastObsIndex;

IMAGE imgHeroDown[2];

int heroBlood;
int score;  // 分数

IMAGE imgSZ[10];

// 游戏初始化 
void init()
{
	// 创建游戏界面
	initgraph(WIN_WIDTH, WIN_HEIGHT);// 对应的宽和高

	// 加载背景资源 
	// 因为是用循环来读取 所以需要合适的文件名
	char name[64];
	for (int i = 0; i < 3; i++) {
		// "res/bg001.png"   "res/bg002.png"  "res/bg003.png"
		sprintf(name, "res/bg%03d.png", i + 1); // 保留3位 不够补0
		// 因为有三重 所以用for循环来读取
		loadimage(&imaBgs[i],name);

		bgX[i] = 0;
	}

	for (int i = 0; i < 12; i++) {
		// "res/hero1.png"...... "res/hero12.png"
		sprintf(name, "res/hero%d.png", i + 1);
		loadimage(&bgHeros[i], name);
	}
	HeroX = WIN_WIDTH * 0.5 - bgHeros[0].getwidth() * 0.5;
	HeroY = 345 - bgHeros[0].getheight();
	HeroIndex = 0;

	heroJump = false;

	jumpHeightMax = 345 - bgHeros[0].getheight() -130;
	jumpHeightOff = -5; // 高度的偏移量

	update = true;

	// 加载小乌龟素材
	//for (int i = 0; i < 7; i++) {
	//	// "res/t1.png" ... "res/t7.png"
	//	sprintf(name, "res/t%d.png", i + 1);
	//	loadimage(&Tortoise[i],name);
	//}
	//torToiseIndex = 0;
	//torToiseExist = false; 
	//TortoiseY = 345 - Tortoise[0].getheight() + 5;
	IMAGE imaTort;
	vector<IMAGE> imaTortArray;
	for (int i = 0; i < 7; i++) {
		sprintf(name, "res/t%d.png", i + 1);
		loadimage(&imaTort, name); 
		imaTortArray.push_back(imaTort);// 一维数组
	}
	obstacleImags.push_back(imaTortArray); // 放入到刚刚定义的二维数组内

	// 加载小狮子
	IMAGE imaLion;
	vector<IMAGE> imaLionArray;
	for (int i = 0; i < 6; i++) {
		sprintf(name, "res/p%d.png", i + 1);
		loadimage(&imaLion, name);
		imaLionArray.push_back(imaLion); 
	}
	obstacleImags.push_back(imaLionArray);

	//  初始化障碍物池
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		obstacles[i].exist = false;
	}

	// 加载下蹲素材
	loadimage(&imgHeroDown[0], "res/d1.png");
	loadimage(&imgHeroDown[1], "res/d2.png");
	herodown = false;

	// 加载柱子的素材
	IMAGE imgH;

	for (int i = 0; i < 4; i++) {
		vector<IMAGE> imgHookArray;
		sprintf(name, "res/h%d.png", i + 1);
		loadimage(&imgH, name,75,267,true);
		imgHookArray.push_back(imgH);
		obstacleImags.push_back(imgHookArray);
	}

	// 血量定义
	heroBlood = 100;

	// 预加载音效
	preLoadSound("res/hit.mp3");

	// 添加背景音乐
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);

	lastObsIndex = -1;

	// 分数初始化
	score = 0;

	// 加载数字图片
	for (int i = 0; i < 10; i++) {
		sprintf(name, "res/sz/%d.png", i);
		loadimage(&imgSZ[i], name);
	}
}

void creatObstacle()
{
	int i;
	for (i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist == false) {
			break;
		}
	}
	if (i >= OBSTACLE_COUNT) {
		return;
	}

	// 随机生成障碍物
	// 一些基本信息的统一
	obstacles[i].exist = true;
	obstacles[i].hited = false;
	obstacles[i].imaIndex = 0; 
	/*obstacles[i].type = (obstacle_type)(rand() % OBSTACLE_TYPE_COUNT);*/ // 因为就2个类型 所以对2取余 
	obstacles[i].type = (obstacle_type)(rand() % 3);

	// 判断上一个障碍物类型是否为柱子 这里做死亡障碍的优化
	if(lastObsIndex >= 0 &&
			obstacles[lastObsIndex].type >= HOOK1 &&
			obstacles[lastObsIndex].type <= HOOK4 &&
			obstacles[i].type == LION &&
			obstacles[lastObsIndex].x > (WIN_WIDTH - 500)) {
		obstacles[i].type = TORTOISE;
	}
	lastObsIndex = i;

	if (obstacles[i].type == HOOK1)
	{
		obstacles[i].type +=  rand() % 4; //0-3
	}
	obstacles[i].x = WIN_WIDTH;  // 让它从屏幕右边出来
	obstacles[i].y = 350 - obstacleImags[obstacles[i].type][0].getheight();

	// 乌龟
	if (obstacles[i].type == TORTOISE) {
		obstacles[i].speed = 0;
		obstacles[i].power = 5;		
	}

	// 狮子
	else if (obstacles[i].type == LION) {
		obstacles[i].speed = 4;
		obstacles[i].power = 20;
	}

	// 柱子 因为是用类型范围来选 所以在1和4之间包括他们 就统一的柱子一样的数值
	else if (obstacles[i].type >= HOOK1 && obstacles[i].type <= HOOK4) {
		obstacles[i].speed = 0;
		obstacles[i].power = 20;
		obstacles[i].y = 0; // 柱子在顶上
	}

	obstacles[i].passed == false;
}

void checkHit()
{
	// 这里简单粗暴一下 采用循环
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist && obstacles[i].hited == false) {
			int a1x, a1y, a2x, a2y;
			int off = 30; // 偏移量
			// 简单的用矩形来计算区域（玩家）
			if (!herodown) {		// 非下蹲状态（奔跑和跳跃）
				a1x = HeroX + off;
				a1y = HeroY + off;
				// 用索引来定位第几张图片
				a2x = HeroX + bgHeros[HeroIndex].getwidth() - off;
				a2y = HeroY + bgHeros[HeroIndex].getheight();
			}
			else
			{
				a1x = HeroX + off;
				a1y = 345 - imgHeroDown[HeroIndex].getheight();
				a2x = HeroX + imgHeroDown[HeroIndex].getwidth() - off;
				a2y = 345;
			}

			// 障碍物的区域
			IMAGE img = obstacleImags[obstacles[i].type][obstacles[i].imaIndex];
			int b1x = obstacles[i].x + off;
			int b1y = obstacles[i].y + off;
			int b2x = obstacles[i].x + img.getwidth() - off;
			int b2y = obstacles[i].y + img.getheight() - off;

			// 调用接口 判断是否有相交
			if (rectIntersect(a1x,a1y,a2x,a2y,b1x,b1y,b2x,b2y)) {
				heroBlood -= obstacles[i].power;
				printf("血量剩余:%d\n", heroBlood);
				playSound("res/hit.mp3");
				obstacles[i].hited = true;
			}
		}
	}
}


void function()
{
	for (int i = 0; i < 3; i++) {
		bgX[i] -= bgspeed[i];
		// 这个用来重制 因为图片的宽度是有限的 所以如果超过了窗口宽度 则会进行一个重置
		if (bgX[i] < -WIN_WIDTH) {
			bgX[i] = 0;
		}
	}

	// 实现跳跃
	if (heroJump) {
		if (HeroY < jumpHeightMax) {
			jumpHeightOff = 5; // 如果玩家跳的高度小于那条线了，那就已经跳到那么高了 重置偏移了 让玩家进行跳跃下降
		}
		HeroY += jumpHeightOff;  // 进行玩家位置的偏移量移动

		if (HeroY > 345 - bgHeros[0].getheight()) {
			heroJump = false;  // 这个地方就是如果跳跃下降到原来的高度 则关闭跳跃的开关了
			jumpHeightOff = -5; // 跳跃偏移量的初始化 用于下次继续使用
		}
	}

	// 实现下蹲
	else if (herodown) {
		static int count = 0;
		count++;
		int delays[2] = { 8,30 };
		if (count >= delays[HeroIndex]) {
			count = 0;
			HeroIndex++;
			if (HeroIndex == 2) {
				HeroIndex = 0;
				herodown = false;
			}
		}
		
	}
	else {
		// 不跳跃 再执行奔跑
		HeroIndex = (HeroIndex + 1) % 12; // 因为只有12张图片 所以取值对12的取余来使用，这个地方是用来更新序号的
	}

	// 创建小乌龟
	static int frameCount = 0; // 这里静态是因为 函数每次调用完就会销毁 这样子就不会销毁
	static int enemyFre = 50;
	frameCount++;
	if (frameCount > enemyFre) {
		frameCount = 0; // 计数器清零
		//if (!torToiseExist) {	// 这个确保只有一个小乌龟生成
		//	torToiseExist = true;
		//	TortoiseX = WIN_WIDTH;
		enemyFre = 50 + rand() % 51; // 50-100 帧随机数
		creatObstacle();
	}
	/*torToiseIndex = (torToiseIndex + 1) % 7;*/ // 因为就七张图片

	// 更新小乌龟的位置
	// TortoiseX -= bgspeed[2];
	// 为了让小乌龟完整离开窗口后才消失 且为了省资源 判断小乌龟存在时候才执行
		// if (torToiseExist) {
		// if (TortoiseX < -Tortoise[0].getwidth())
		// {
		//	torToiseExist = false;
		//	}
 //}

	// 更新障碍物的坐标
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist) {
			obstacles[i].x -= obstacles[i].speed + bgspeed[2];
			if (obstacles[i].x < -obstacleImags[obstacles[i].type][0].getwidth() * 2) {
				obstacles[i].exist = false;
			}

			int len = obstacleImags[obstacles[i].type].size(); // 数组的size用法可以计算出有多少个元素
			obstacles[i].imaIndex = (obstacles[i].imaIndex + 1) % len;
		}
	}

	// 玩家和障碍物“碰撞检测”处理
	checkHit();
}
	

// 渲染“游戏背景”
void updateBg()
{
	// 坐标位置根据实际情况自己调整 微调什么的
	putimagePNG2(bgX[0], 0, &imaBgs[0]);
	putimagePNG2(bgX[1], 119, &imaBgs[1]);
	putimagePNG2(bgX[2], 330, &imaBgs[2]);
}

void jump()
{
	heroJump = true;  // 这个函数功能是用来启动跳跃这个按钮的
	update = true; // 若启动跳跃开关 则立马刷新 启动功能
}

void down()
{
	herodown = true;
	update = true;
	HeroIndex = 0;
}

void keyEvent()
{
	char ch;
	if (_kbhit()) {  // 如果有按键按下 kbhit返回 true
		ch = _getch(); //  getch() 不需要按空格就可以直接读取用户输入的内容
		if (ch == ' ') {
			jump();  // 因为跳跃是一个功能 功能相对独立我们封装成一个函数
		}
		else if (ch == 's' || ch == 'S') {
			down();
		}
	}
}

void updateEnemy()
{
	// 渲染小乌龟
	/*if (torToiseExist) {
		putimagePNG2(TortoiseX, TortoiseY,WIN_WIDTH,&Tortoise[torToiseIndex]);
	}*/

	// 渲染障碍物
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist) {
			putimagePNG2(obstacles[i].x, obstacles[i].y,WIN_WIDTH, &obstacleImags[obstacles[i].type][obstacles[i].imaIndex]);
			// 这里就可以渲染不止是小乌龟 而是序号是几就是渲染几 因为这个是池子，然后序号就是对应的类型 例如1就是狮子这样子 然后就是渲染第几个 第几张图片这样子 图片都存在了一起了
		}
	}
}

void updateHero()
{
	if (!herodown) {
		putimagePNG2(HeroX, HeroY, &bgHeros[HeroIndex]);
	}
	else {
		int y = 345 - imgHeroDown[HeroIndex].getheight();
		putimagePNG2(HeroX,y,&imgHeroDown[HeroIndex]);
	}
}

void updateBloodBar()
{
	drawBloodBar(10,10,200,10,2,BLUE,DARKGRAY,RED,heroBlood/100.0);
}

void checkOver()
{
	if (heroBlood <= 0)
	{
		loadimage(0, "res/over.png");
		FlushBatchDraw(); // 刷新缓存
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		system("pause");
			
		// 暂停之后 若选择直接开始下一局
		heroBlood = 100;
		score = 0;
		mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	}
}

void gameBeginScreen()
{
	// 初始时候显示游戏画面
	loadimage(0, "res/over.png");
	system("pause");
}

void checkScore()
{
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist &&
			obstacles[i].passed == false &&
			obstacles[i].hited == false &&
			obstacles[i].x + obstacleImags[obstacles[i].type][0].getwidth() < HeroX) {
			/*score++;
			obstacles[i].passed = true;
			printf("score:%d\n", score);*/
			if (obstacles[i].type == TORTOISE) {
				score++;
				obstacles[i].passed = true;
				printf("分数:%d\n", score);
			}

			else if (obstacles[i].type == LION) {
				score += 4;
				obstacles[i].passed = true;
				printf("分数:%d\n", score);
			}

			// 不用else 是因为怕以后有添加新的障碍物添加
			else if (obstacles[i].type >= HOOK1 && obstacles[i].type <= HOOK4) {
				score += 3;
				obstacles[i].passed = true;
				printf("分数:%d\n", score);
			}
		}
	}
}

void updateScore()
{
	// 50 => "50"  '5'     '5' - '0' = 5;
	char str[8];
	sprintf(str, "%d", score);

	int x = 20;
	int y = 25;

	for (int i = 0; str[i]; i++) {
		int sz = str[i] - '0';
		putimagePNG(x, y, &imgSZ[sz]);
		x += imgSZ[sz].getwidth() + 5;
	}
}

void checkWin()
{
	if (score >= WIN_SCORE)
	{
		FlushBatchDraw();
		mciSendString("play res/win.mp3", 0, 0, 0);
		Sleep(2000);
		loadimage(0, "res/win.png");
		FlushBatchDraw(); // 刷新画面
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		system("pause");

		heroBlood = 100;
		score = 0;
		mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	}
}

int main(void)
{
	init(); // 这个是先一个初始化 为的是更方便维护

	gameBeginScreen(); // 启动界面

	int timer = 0;

	while (1) {
		keyEvent();
		timer += getDelay();
		if (timer > 30) {
			timer = 0;
			update = true;	// 30次刷新一次
		}

		if (update) {
			update = false; // 使用完 然后关闭开关
			BeginBatchDraw();
			updateBg(); // 图片背景
			// putimagePNG2(HeroX, HeroY, &bgHeros[HeroIndex]);
			updateHero();
			updateEnemy();
			updateBloodBar();
			updateScore();
			checkWin(); 
			EndBatchDraw();

			checkOver();
			checkScore();

			function();
		}
	}

	system("pause");

	return 0;
}