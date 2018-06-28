#include "major.h"
#include "SimpleAudioEngine.h"
#include "HelloWorldScene.h"
#include "gameoverscene.h"
#include "windows.h"
#include <vector>

USING_NS_CC;
using namespace cocos2d;

major::major()
{
	for (int i = 0; i < LINE; i++)
	{
		for (int j = 0; j < COLUMN; j++)
		{
			square[i][j] = NULL;
		}
	}
	now_score = 0;
	next_squaretype = rand() % 19;
	score_label = NULL;
	is_game = true;
	if (CCUserDefault::sharedUserDefault()->getFloatForKey("speed") > 4.5f || CCUserDefault::sharedUserDefault()->getFloatForKey("speed") < 1.0f)
		speed = 3.0f;
	else
	{
		speed = CCUserDefault::sharedUserDefault()->getFloatForKey("speed");
	}
}

major::~major()
{  }

Scene* major::createScene()
{
	return major::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool major::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	//初始化背景颜色 
	auto rect = Director::getInstance()->getOpenGLView()->getVisibleRect();
	float x = rect.origin.x + rect.size.width / 2;
	float y = rect.origin.y + rect.size.height / 2;
	auto sprite = Sprite::create("game2.png");
	sprite->setPosition(Vec2(x, y));
	addChild(sprite, 0);

	//初始化分数
	char buf[4] = { 0 };
	sprintf(buf, "%d", now_score);
	score_label = LabelTTF::create(buf, "Arial", 50);
	score_label->setPosition(Vec2(335,280));
	score_label->setColor(ccc3(190, 190, 190));
	addChild(score_label);

	//初始化下一个方块显示
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			next_square[i][j] = CCSprite::create("square.png");
			next_square[i][j]->setPosition(Vec2(310+ 14 * j, 200 - 14 * i));
			next_square[i][j]->setScale(3.0f);
			next_square[i][j]->setColor(ccc3(255,255,255));
			addChild(next_square[i][j]);
		}
	}

	//初始化格子
	for (int i = 0; i < LINE; i++)
	{
		for (int j = 0; j < COLUMN; j++)
		{
		    square[i][j] = CCSprite::create("square.png");
			square[i][j]->setScale(3.0f);
			square[i][j]->setPosition(Vec2(160+14.5*j,310-14.5*i));
			square[i][j]->setTag(0);
			square[i][j]->setColor(ccc3(255, 255, 255));
			addChild(square[i][j],5);
		}
	}

	

	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
		"CloseSelected.png",
		"CloseSelected.png", this,
		menu_selector(major::menuCloseCallback));
	if (closeItem == nullptr ||
		closeItem->getContentSize().width <= 0 ||
		closeItem->getContentSize().height <= 0)
	{
		problemLoading("'CloseSelected.png' and 'CloseSelected.png'");
	}
	else
	{
		float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
		float y = origin.y + closeItem->getContentSize().height / 2;
		closeItem->setPosition(Vec2(x, y));
	}

	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	addChild(menu, 1);

	/////////////////////////////
    //添加回到主界面按钮
	auto home = MenuItemImage::create("home.png", "home.png", this, menu_selector(major::menuOKCallback));
	home->setPosition(Vec2(visibleSize.width / 2 + origin.x -345 , visibleSize.height / 2 + origin.y -20));
	home->setScale(0.4f);
	auto phome = Menu::create(home, NULL);
	addChild(phome, 2);

	//添加背景音乐按钮
	is_paused = true;
	auto* play = MenuItemImage::create("music1.png", "music1.png", this, menu_selector(major::pause));
	play->setPosition(Vec2(visibleSize.width / 2 + origin.x - 348.5, visibleSize.height / 2 + origin.y - 220));
	play->setScale(0.3f);
	auto* pplay = Menu::create(play, NULL);
	addChild(pplay, 2);

	//添加开始和暂停游戏按钮
	auto* play_game = MenuItemImage::create("start.png", "start.png", this, menu_selector(major::resume_game));
	play_game->setPosition(Vec2(visibleSize.width / 2 + origin.x - 340, visibleSize.height / 2 + origin.y - 310));
	play_game->setScale(1.3f);
	auto* pause_game = MenuItemImage::create("pause.png", "pause.png", this, menu_selector(major::pause_game));
	pause_game->setPosition(Vec2(visibleSize.width / 2 + origin.x - 350, visibleSize.height / 2 + origin.y - 280));
	pause_game->setScale(1.3f);
	auto* pplay_game = Menu::create(play_game, NULL);
	auto* ppause_game = Menu::create(pause_game, NULL);
	addChild(pplay_game, 2);
	addChild(ppause_game, 2);


	//添加键盘监听
	myKeyListener = EventListenerKeyboard::create();
	myKeyListener->onKeyReleased = [=](EventKeyboard::KeyCode keycode, cocos2d::Event *event)
	{
		key[keycode] = false;
	};

	myKeyListener->onKeyPressed = [=](EventKeyboard::KeyCode keycode, cocos2d::Event *event)
	{
		key[keycode] = true;
		if (keycode == EventKeyboard::KeyCode::KEY_SPACE)
			nextSquareType();
	};
	dispatcher = Director::getInstance()->getEventDispatcher();

	//添加到事件分发器  
	dispatcher->addEventListenerWithSceneGraphPriority(myKeyListener, this);
	this->schedule(schedule_selector(major::myupdate), 0.1f);
	
	newSquareType();
	schedule(schedule_selector(major::updateDown), speed / (now_score + 5));
	
	return true;
}

void major::myupdate(float delta)//重写update函数
{
	if (is_game)
	{
		if (key[EventKeyboard::KeyCode::KEY_DOWN_ARROW])
		{
			updateDown(0.0);
		}
		if (key[EventKeyboard::KeyCode::KEY_LEFT_ARROW])
		{
			updateLeft();
		}
		if (key[EventKeyboard::KeyCode::KEY_RIGHT_ARROW])
		{
			updateRight();
		}
		if (key[EventKeyboard::KeyCode::KEY_UP_ARROW])
		{
			updateDown(0.0);
			updateDown(0.0);
			updateDown(0.0);
		}
	}
}

void major::menuCloseCallback(Ref* pSender)
{
	//Close the cocos2d-x game scene and quit the application
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif

	/*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

	//EventCustom customEndEvent("game_scene_close_event");
	//_eventDispatcher->dispatchEvent(&customEndEvent);


}

void major::play(cocos2d::Object* pSender)
{
	if (is_paused)
	{
		CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
		removeChild(ppause);
	}
	else {
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("1.mp3",true);
		CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume();
	}
	is_paused = false;
}

void major::menuOKCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->replaceScene(TransitionMoveInT::create(1.0f,HelloWorld::create()));
}

void major::pause(cocos2d::Object* pSender)
{
	is_paused = true;
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	auto* pause = MenuItemImage::create("music2.png", "music2.png", this, menu_selector(major::play));
	pause->setPosition(Vec2(-105, -60));
	pause->setScale(0.3f);
	ppause = Menu::create(pause, NULL);
	addChild(ppause, 4);
}

void major::clearLine(int start_line, int end_line)
{
	int i;
	for (i = start_line; i <= end_line; i++)
	{
		int j;
		for (j = 0; j < COLUMN; j++)
		{
			if (square[i][j]->getTag() == 0)
				break;
		}

		if (j == COLUMN)
		{
			for (int a = i - 1; a > 0; a--)
				copyline(a);
			for (int b = 0; b < COLUMN; b++)
			{
				square[0][b]->setColor(ccc3(255, 255, 255));
				square[0][b]->setTag(0);
			}
			now_score++;
			Clearline_music();
		}

	}
	updatescore();
	for (int i = 0; i < LINE; i++)
	{
		for (int j = 0; j < COLUMN; j++)
		{
			if (square[i][j]->getTag() == 1)
				square[i][j]->setColor(ccc3(255, 0, 0));
		}
	}
}

void major::Clearline_music()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("6755.wav");
}

void major::copyline(int line_num)
{
	for (int j = 0; j < COLUMN; j++)
	{
		square[line_num + 1][j]->setColor(square[line_num][j]->getColor());
		square[line_num + 1][j]->setTag(square[line_num][j]->getTag());
	}
}

void major::newSquareType()
{
	srand(time(0));
	now_squaretype = next_squaretype;
	next_squaretype = (rand()*rand()) % 19;
	switch (now_squaretype)
	{
		case 0:
			now_column = 4;
			now_line = 0;
			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			square[now_line][now_column+1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column+1]->setTag(1);
			break;
		case 1:
			now_column = 4;
			now_line = 0;
			for (int i = -1; i < 3; i++)
			{
				square[now_line][now_column + i]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column + i]->setTag(1);
			}
			break;
		case 4:
			now_column = 4;
			now_line = 0;
			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			break;
		case 6:
			now_column = 4;
			now_line = 0;
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
			break;
		case 8:
			now_column = 4;
			now_line = 0;
			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
			break;
		case 10:
			now_column = 4;
			now_line = 0;
			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			break;
		case 12:
			now_column = 4;
			now_line = 0;
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
			break;
		case 14:
			now_column = 4;
			now_line = 0;
			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
			break;
		case 16:
			now_column = 4;
			now_line = 0;
			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			break;
		case 18:
			now_column = 4;
			now_line = 0;
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
			break;

		case 2:
			now_column = 3;
			now_line = 0;
		    square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
			break;
		case 3:
			now_column = 3;
			now_line = 0;
			for (int i = 0; i < 3; i++)
			{
				square[now_line][now_column + i]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column + i]->setTag(1);
			}
			break;
		case 5:
			now_column = 3;
			now_line = 0;
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
	        break;
		case 7:
			now_column = 3;
			now_line = 0;
			square[now_line][now_column + 2]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 2]->setTag(1);
			break;
		case 9:
			now_column = 3;
			now_line = 0;
			for (int i = 0; i < 3; i++)
			{
				square[now_line][now_column + i]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column + i]->setTag(1);
			}
			break;
		case 11:
			now_column = 3;
			now_line = 0;
			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			break;
		case 13:
			now_column = 3;
			now_line = 0;
			for (int i = 0; i < 3; i++)
			{
				square[now_line][now_column + i]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column + i]->setTag(1);
			}
			break;
		case 15:
			now_column = 3;
			now_line = 0;
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
			square[now_line][now_column + 2]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 2]->setTag(1);
			break;
		case 17:
			now_column = 3;
			now_line = 0;
			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
			break;
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			next_square[i][j]->setColor(ccc3(255,255,255));
		}
	}
	switch (next_squaretype)
	{
	case 0:
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[1][2]->setColor(ccc3(30, 144, 255));
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		next_square[2][2]->setColor(ccc3(30, 144, 255));
		break;

	case 1:
		next_square[1][0]->setColor(ccc3(30, 144, 255));
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[1][2]->setColor(ccc3(30, 144, 255));
		next_square[1][3]->setColor(ccc3(30, 144, 255));
		break;

	case 2:
		next_square[0][1]->setColor(ccc3(30, 144, 255));
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		next_square[3][1]->setColor(ccc3(30, 144, 255));
		break;

	case 3:
		next_square[1][2]->setColor(ccc3(30, 144, 255));
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		next_square[2][2]->setColor(ccc3(30, 144, 255));
		next_square[2][3]->setColor(ccc3(30, 144, 255));
		break;

	case 4:
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		next_square[3][1]->setColor(ccc3(30, 144, 255));
		next_square[2][2]->setColor(ccc3(30, 144, 255));
		break;

	case 5:
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[1][2]->setColor(ccc3(30, 144, 255));
		next_square[1][3]->setColor(ccc3(30, 144, 255));
		next_square[2][2]->setColor(ccc3(30, 144, 255));
		break;

	case 6:
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		next_square[1][2]->setColor(ccc3(30, 144, 255));
		next_square[2][2]->setColor(ccc3(30, 144, 255));
		next_square[3][2]->setColor(ccc3(30, 144, 255));
		break;

	case 7:
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[1][2]->setColor(ccc3(30, 144, 255));
		next_square[1][3]->setColor(ccc3(30, 144, 255));
		next_square[2][3]->setColor(ccc3(30, 144, 255));
		break;

	case 8:
		next_square[0][2]->setColor(ccc3(30, 144, 255));
		next_square[1][2]->setColor(ccc3(30, 144, 255));
		next_square[2][2]->setColor(ccc3(30, 144, 255));
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		break;

	case 9:
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		next_square[2][2]->setColor(ccc3(30, 144, 255));
		next_square[2][3]->setColor(ccc3(30, 144, 255));
		break;

	case 10:
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[1][2]->setColor(ccc3(30, 144, 255));
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		next_square[3][1]->setColor(ccc3(30, 144, 255));
		break;

	case 11:
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[1][2]->setColor(ccc3(30, 144, 255));
		next_square[1][3]->setColor(ccc3(30, 144, 255));
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		break;

	case 12:
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[1][2]->setColor(ccc3(30, 144, 255));
		next_square[2][2]->setColor(ccc3(30, 144, 255));
		next_square[3][2]->setColor(ccc3(30, 144, 255));
		break;

	case 13:
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		next_square[2][2]->setColor(ccc3(30, 144, 255));
		next_square[2][3]->setColor(ccc3(30, 144, 255));
		next_square[1][3]->setColor(ccc3(30, 144, 255));
		break;

	case 14:
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		next_square[3][1]->setColor(ccc3(30, 144, 255));
		next_square[3][2]->setColor(ccc3(30, 144, 255));
		break;

	case 15:
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[1][2]->setColor(ccc3(30, 144, 255));
		next_square[2][2]->setColor(ccc3(30, 144, 255));
		next_square[2][3]->setColor(ccc3(30, 144, 255));
		break;

	case 16:
		next_square[0][2]->setColor(ccc3(30, 144, 255));
		next_square[1][2]->setColor(ccc3(30, 144, 255));
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		break;
		
	case 17:
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		next_square[2][2]->setColor(ccc3(30, 144, 255));
		next_square[1][2]->setColor(ccc3(30, 144, 255));
		next_square[1][3]->setColor(ccc3(30, 144, 255));
		break;

	case 18:
		next_square[1][1]->setColor(ccc3(30, 144, 255));
		next_square[2][1]->setColor(ccc3(30, 144, 255));
		next_square[2][2]->setColor(ccc3(30, 144, 255));
		next_square[3][2]->setColor(ccc3(30, 144, 255));
		break;
	}
}

void major::nextSquareType()
{
	switch (now_squaretype)
	{
	case 0:
		break;

	case 1:
		//消除现有方块
		for (int i = -1; i < 3; i++)
		{
			square[now_line][now_column + i]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column + i]->setTag(0);
		}

		now_line++;
		now_column--;
		if (square[now_line][now_column + 1]->getTag() == 1)//判断若变换后下方会碰撞
		{
			now_line--;
		}
		for (int i = 0; i < 4; i++)
		{
			if (now_line - i >= 0) 
			{
				square[now_line - i][now_column + 1]->setColor(ccc3(52, 228, 249));
				square[now_line - i][now_column + 1]->setTag(1);
			}
		}
		
		now_squaretype = 2;
		break;

	case 2:
		//判断是否可以变换
		if (now_column < 0)//靠最左边时
		{
			for (int i = 2; i < 5; i++)
				if (square[now_line - 1][now_column + i]->getTag() == 1)
					return;
			//消除现有方块
			for (int i = 0; i < 4; i++)
			{
				if (now_line - i >= 0)
				{
					square[now_line - i][now_column + 1]->setColor(ccc3(255, 255, 255));
					square[now_line - i][now_column + 1]->setTag(0);
				}
			}
			now_line--;
			now_column += 2;
		}
		else if (now_column + 2 == COLUMN)//靠最右边时
		{
			if (square[now_line - 1][now_column]->getTag() == 1 || square[now_line - 1][now_column - 1]->getTag() == 1 || square[now_line - 1][now_column - 2]->getTag() == 1)
				return;
			//消除现有方块
			for (int i = 0; i < 4; i++)
			{
				if (now_line - i >= 0)
				{
					square[now_line - i][now_column + 1]->setColor(ccc3(255, 255, 255));
					square[now_line - i][now_column + 1]->setTag(0);
				}
			}
			now_line--;
			now_column--;
		}
		else if (now_column + 3 == COLUMN)// 靠最右边-1时
		{
			if (square[now_line - 1][now_column + 2]->getTag() == 1 || square[now_line - 1][now_column]->getTag() == 1 || square[now_line - 1][now_column - 1]->getTag() == 1)
				return;
			//消除现有方块
			for (int i = 0; i < 4; i++)
			{
				if (now_line - i >= 0)
				{
					square[now_line - i][now_column + 1]->setColor(ccc3(255, 255, 255));
					square[now_line - i][now_column + 1]->setTag(0);
				}
			}
			now_line--;
		}
		else//正常情况
		{
			if (square[now_line - 1][now_column]->getTag() == 1 || square[now_line - 1][now_column + 2]->getTag() == 1 || square[now_line - 1][now_column + 3]->getTag() == 1)
				return;
			//消除现有方块
			for (int i = 0; i < 4; i++)
			{
				if (now_line - i >= 0)
				{
					square[now_line - i][now_column + 1]->setColor(ccc3(255, 255, 255));
					square[now_line - i][now_column + 1]->setTag(0);
				}
			}
			now_line--;
			now_column++;
		}

		for (int j = -1; j < 3; j++)//生成新方块
		{
			square[now_line][now_column + j]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + j]->setTag(1);
		}

		now_squaretype = 1;
		break;

	case 3:
		square[now_line][now_column]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column]->setTag(0);

		square[now_line + 1][now_column + 1]->setColor(ccc3(52, 228, 249));
		square[now_line + 1][now_column + 1]->setTag(1);

		now_line++;
		now_column++;
		now_squaretype = 4;
		break;

	case 4:
		if (now_line - 2 >= 0)
		{//判断是否可以变换
			if (now_column - 1 < 0)
			{
				if (square[now_line - 1][now_column + 2]->getTag() == 1 || square[now_line][now_column + 1]->getTag() == 1)
					return;
				square[now_line][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column]->setTag(0);
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);

				square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column + 1]->setTag(1);
				square[now_line - 1][now_column + 2]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column + 2]->setTag(1);

				now_squaretype = 5;
				break;
			}
			else
			{
				if (square[now_line - 1][now_column - 1]->getTag() == 1)
					return;
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);

				square[now_line - 1][now_column - 1]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column - 1]->setTag(1);

				now_column--;
				now_squaretype = 5;
				break;
			}
		}
		break;
		
	case 5:
		if (now_line - 2 >= 0)
		{
			square[now_line - 1][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 2]->setTag(0);

			square[now_line - 2][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line - 2][now_column + 1]->setTag(1);

			now_squaretype = 6;
		}
		break;

	case 6:
		if (now_line - 2 >= 0)
		{//判断是否可以变换
			if (now_column + 2 > COLUMN - 1)
			{
				if (square[now_line - 1][now_column - 1]->getTag() == 1)
					return;
				square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column + 1]->setTag(0);
				square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column + 1]->setTag(0);

				square[now_line - 1][now_column - 1]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column - 1]->setTag(1);
				square[now_line - 2][now_column]->setColor(ccc3(52, 228, 249));
				square[now_line - 2][now_column]->setTag(1);

				now_line--;
				now_column--;
				now_squaretype = 3;
				break;
			}
			else
			{
				square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column + 1]->setTag(0);

				square[now_line - 1][now_column + 2]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column + 2]->setTag(1);

				now_line--;
				now_squaretype = 3;
				break;
			}
		}
		break;

	case 7:
		if (now_line - 2 >= 0)
		{
			if (square[now_line][now_column]->getTag() == 1 || square[now_line][now_column + 1]->getTag() == 1 || square[now_line - 2][now_column + 1]->getTag() == 1)
				return;
			square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column + 2]->setTag(0);
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
			square[now_line - 1][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 2]->setTag(0);

			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
			square[now_line - 2][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line - 2][now_column + 1]->setTag(1);

			now_squaretype = 8;
		}
		break;

	case 8:
		if (now_line - 2 >= 0)
		{//判断是否可以变换
			if (now_column + 2 > COLUMN - 1)
			{
				if (square[now_line - 1][now_column - 1]->getTag() == 1 || square[now_line - 2][now_column - 1]->getTag() == 1 || square[now_line - 1][now_column]->getTag() == 1)
					return;
				square[now_line][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column]->setTag(0);
				square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column + 1]->setTag(0);
				square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column + 1]->setTag(0);

				square[now_line - 1][now_column]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column]->setTag(1);
				square[now_line - 1][now_column - 1]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column - 1]->setTag(1);
				square[now_line - 2][now_column - 1]->setColor(ccc3(52, 228, 249));
				square[now_line - 2][now_column - 1]->setTag(1);

				now_line--;
				now_column--;
				now_squaretype = 9;
				break;
			}
			else
			{
				if (square[now_line - 1][now_column]->getTag() == 1 || square[now_line - 2][now_column]->getTag() == 1 || square[now_line - 1][now_column + 2]->getTag() == 1)
					return;
				square[now_line][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column]->setTag(0);
				square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column + 1]->setTag(0);
				square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column + 1]->setTag(0);

				square[now_line - 1][now_column]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column]->setTag(1);
				square[now_line - 1][now_column + 2]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column + 2]->setTag(1);
				square[now_line - 2][now_column]->setColor(ccc3(52, 228, 249));
				square[now_line - 2][now_column]->setTag(1);

				now_line--;
				now_squaretype = 9;
				break;
			}
		}
		break;
	case 9:
		if (now_line - 1 >= 0)
		{
			if (square[now_line + 1][now_column + 1]->getTag() == 1 || square[now_line - 1][now_column + 1]->getTag() == 1 || square[now_line - 1][now_column + 2]->getTag() == 1)
				return;

			square[now_line][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column]->setTag(0);
			square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column + 2]->setTag(0);
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);

			square[now_line - 1][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line - 1][now_column + 1]->setTag(1);
			square[now_line - 1][now_column + 2]->setColor(ccc3(52, 228, 249));
			square[now_line - 1][now_column + 2]->setTag(1);
			square[now_line + 1][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column + 1]->setTag(1);

			now_line++;
			now_column++;
			now_squaretype = 10;
		}
		break;

	case 10:
		if (now_line - 2 >= 0)
		{//判断是否可以变换
			if (now_column - 1 < 0)
			{
				if (square[now_line - 1][now_column + 2]->getTag() == 1 || square[now_line][now_column + 2]->getTag() == 1 || square[now_line - 1][now_column + 1]->getTag() == 1)
					return;
				square[now_line][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column]->setTag(0);
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);
				square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column + 1]->setTag(0);

				square[now_line - 1][now_column + 1]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column + 1]->setTag(1);
				square[now_line - 1][now_column + 2]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column + 2]->setTag(1);
				square[now_line][now_column + 2]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column + 2]->setTag(1);

				now_squaretype = 7;
				break;
			}
			else
			{
				if (square[now_line][now_column + 1]->getTag() == 1 || square[now_line - 1][now_column + 1]->getTag() == 1 || square[now_line - 1][now_column - 1]->getTag() == 1)
					return;
				square[now_line][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column]->setTag(0);
				if (now_line - 2 >= 0)
					square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);
				square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column + 1]->setTag(0);

				square[now_line - 1][now_column - 1]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column - 1]->setTag(1);
				square[now_line - 1][now_column + 1]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column + 1]->setTag(1);
				square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column + 1]->setTag(1);

				now_column--;
				now_squaretype = 7;
				break;
			}
		}
		break;
	case 11:
		if (now_line - 2 >= 0)
		{
			if (square[now_line][now_column + 1]->getTag() == 1 || square[now_line - 2][now_column]->getTag() == 1 || square[now_line - 2][now_column + 1]->getTag() == 1)
				return;
			square[now_line][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column]->setTag(0);
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
			square[now_line - 1][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 2]->setTag(0);

			square[now_line - 2][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line - 2][now_column]->setTag(1);
			square[now_line - 2][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line - 2][now_column + 1]->setTag(1);
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);

			now_squaretype = 12;
		}
		break;

	case 12:
		if (now_line - 2 >= 0)
		{
			if (now_column + 2 > COLUMN - 1)
			{
				if (square[now_line - 1][now_column - 1]->getTag() == 1 || square[now_line - 1][now_column]->getTag() == 1)
					return;
				square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column + 1]->setTag(0);
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);
				
				square[now_line - 1][now_column - 1]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column - 1]->setTag(1);
				square[now_line - 1][now_column]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column]->setTag(1);

				now_line--;
				now_column--;
				now_squaretype = 13;
				break;
			}
			else
			{
				if (square[now_line - 1][now_column]->getTag() == 1 || square[now_line - 1][now_column + 2]->getTag() == 1 || square[now_line - 2][now_column + 2]->getTag() == 1)
					return;
				square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column + 1]->setTag(0);
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);
				square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column + 1]->setTag(0);

				square[now_line - 2][now_column + 2]->setColor(ccc3(52, 228, 249));
				square[now_line - 2][now_column + 2]->setTag(1);
				square[now_line - 1][now_column]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column]->setTag(1);
				square[now_line - 1][now_column + 2]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column + 2]->setTag(1);

				now_line--;
				now_squaretype = 13;
				break;
			}
		}
		break;

	case 13:
		if (now_line - 1 >= 0)
		{
			if (square[now_line + 1][now_column + 1]->getTag() == 1 || square[now_line + 1][now_column + 2]->getTag() == 1 || square[now_line - 1][now_column + 1]->getTag() == 1)
				return;
			square[now_line][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column]->setTag(0);
			square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column + 2]->setTag(0);
			square[now_line - 1][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 2]->setTag(0);

			square[now_line - 1][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line - 1][now_column + 1]->setTag(1);
			square[now_line + 1][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column + 1]->setTag(1);
			square[now_line + 1][now_column + 2]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column + 2]->setTag(1);

			now_line++;
			now_column++;
			now_squaretype = 14;
		}
		break;

	case 14:
		if (now_line - 2 >= 0)
		{
			if (now_column - 1 < 0)
			{
				if (square[now_line - 1][now_column + 1]->getTag() == 1 || square[now_line - 1][now_column + 2]->getTag() == 1)
					return;
				square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column + 1]->setTag(0);
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);

			    square[now_line - 1][now_column + 1]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column + 1]->setTag(1);
				square[now_line - 1][now_column + 2]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column + 2]->setTag(1);

				now_squaretype = 11;
				break;
			}
			else
			{
				if (square[now_line - 1][now_column - 1]->getTag() == 1 || square[now_line][now_column - 1]->getTag() == 1 || square[now_line - 1][now_column + 1]->getTag() == 1)
					return;
				square[now_line][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column]->setTag(0);
				square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column + 1]->setTag(0);
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);


				square[now_line][now_column - 1]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column - 1]->setTag(1);
				square[now_line - 1][now_column - 1]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column - 1]->setTag(1);
				square[now_line - 1][now_column + 1]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column + 1]->setTag(1);

				now_column--;
				now_squaretype = 11;
				break;
			}
		}
		break;

	case 15:
		if (now_line - 2 >= 0)
		{
			if (square[now_line][now_column]->getTag() == 1 || square[now_line - 2][now_column + 1]->getTag() == 1)
				return;
			square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column + 1]->setTag(0);
			square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column + 2]->setTag(0);

			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			square[now_line - 2][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line - 2][now_column + 1]->setTag(1);
			now_squaretype = 16;
		}
		break;

	case 16:
		if (now_line - 2 >= 0)
		{
			if (now_column + 2 > COLUMN - 1)
			{
				if (square[now_line - 1][now_column - 1]->getTag() == 1 || square[now_line][now_column + 1]->getTag() == 1)
					return;
				square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column + 1]->setTag(0);
				square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column + 1]->setTag(0);

				square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column + 1]->setTag(1);
				square[now_line - 1][now_column - 1]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column - 1]->setTag(1);
				now_column--;
				now_squaretype = 15;
				break;
			}
			else
			{
				if (square[now_line][now_column + 1]->getTag() == 1 || square[now_line][now_column + 2]->getTag() == 1)
					return;
				square[now_line][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column]->setTag(0);
				square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column + 1]->setTag(0);

				square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column + 1]->setTag(1);
				square[now_line][now_column + 2]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column + 2]->setTag(1);
				now_squaretype = 15;
				break;
			}
		}
		break;

	case 17:
		if (now_line - 2 >= 0)
		{
			if (square[now_line][now_column + 2]->getTag() == 1 || square[now_line - 2][now_column + 1]->getTag() == 1)
				return;
			square[now_line][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column]->setTag(0);
			square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column + 1]->setTag(0);

			square[now_line][now_column + 2]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 2]->setTag(1);
			square[now_line - 2][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line - 2][now_column + 1]->setTag(1);
			now_column++;
			now_squaretype = 18;
		}
		break;

	case 18:
		if (now_line - 2 >= 0)
		{
			if (now_column - 1 < 0)
			{
				if (square[now_line][now_column]->getTag() == 1 || square[now_line - 1][now_column + 2]->getTag() == 1)
					return;
				square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column]->setTag(0);
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);

				square[now_line][now_column]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column]->setTag(1);
				square[now_line - 1][now_column + 2]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column + 2]->setTag(1);
				now_squaretype = 17;
				break;
			}
			else
			{
				if (square[now_line][now_column]->getTag() == 1 || square[now_line][now_column - 1]->getTag() == 1)
					return;
				square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line][now_column + 1]->setTag(0);
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);

				square[now_line][now_column - 1]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column - 1]->setTag(1);
				square[now_line][now_column]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column]->setTag(1);

				now_column--;
				now_squaretype = 17;
				break;
			}
		}
		break;
	}

}

void major::updateDown(float dt) 
{
	switch (now_squaretype)
	{
	case 0:
		if (now_line >= LINE - 1)//到底
		{
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column+1]->setColor(ccc3(190,190,190));
			square[now_line-1][now_column]->setColor(ccc3(190,190,190));
			square[now_line-1][now_column+1]->setColor(ccc3(190,190,190));
			clearLine(LINE - 2, LINE - 1);
			newSquareType();
			return;
		}
		if (square[now_line + 1][now_column]->getTag() == 1 || square[now_line + 1][now_column + 1]->getTag() == 1)//被下方挡住
		{
			is_fail();
			if (now_line - 1 >= 0)
			{
				square[now_line][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
				clearLine(now_line, now_line - 1);
			}
			newSquareType();
			return;
		}
		for (int i = 0; i < 2; i++)
		{
			if (now_line < LINE - 1)
			{
				square[now_line + 1][now_column + i]->setColor(ccc3(52, 228, 249));//创建新色
				square[now_line + 1][now_column + i]->setTag(1);

				if (now_line - 1 >= 0)
				{
					square[now_line - 1][now_column + i]->setColor(ccc3(255, 255, 255));//消除原色
					square[now_line - 1][now_column + i]->setTag(0);
				}
			}
		}
		now_line++;
		break;

	case 1:
		if (now_line >= LINE - 1)//到底
		{
			square[now_line][now_column-1]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column+1]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 1, LINE - 1);
			newSquareType();
			return;
		}
		for (int i = -1; i < 3; i++)//被下方挡住
		{
			if (square[now_line + 1][now_column + i]->getTag() == 1)
			{
				is_fail();
				square[now_line][now_column - 1]->setColor(ccc3(190, 190, 190));
				square[now_line][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
				clearLine(now_line, now_line);
				newSquareType();
				return;
			}
		}
		if (now_line < LINE - 1)
		{
			for (int i = -1; i < 3; i++)
			{

				square[now_line + 1][now_column + i]->setColor(ccc3(52, 228, 249));//创建新色
				square[now_line + 1][now_column + i]->setTag(1);

				square[now_line][now_column + i]->setColor(ccc3(255, 255, 255));//消除原色
				square[now_line][now_column + i]->setTag(0);
			}
		}
		now_line++;
		break;

	case 2:
		if (now_line >= LINE - 1)//到底
		{
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line-1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line-2][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line-3][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 4, LINE - 1);
			newSquareType();
			return;
		}
		if (square[now_line + 1][now_column + 1]->getTag() == 1)//被下方挡住
		{
			is_fail();
			if (now_line - 3 >= 0)
			{
				square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 3][now_column + 1]->setColor(ccc3(190, 190, 190));
				clearLine(now_line - 3, now_line);
			}
			newSquareType();
			return;
		}
		if (now_line < LINE - 1)
		{
			//创建新色
			square[now_line + 1][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column + 1]->setTag(1);

			if (now_line - 3 >= 0)//消除原色
			{
				square[now_line - 3][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 3][now_column + 1]->setTag(0);
			}
		}
		now_line++;
		break;

	case 3:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 2, LINE - 1);
			newSquareType();
			return;
		}
		for (int i = 0; i < 3; i++)
		{
			if (square[now_line + 1][now_column + i]->getTag() == 1)
			{
				is_fail();
				if (now_line - 1 >= 0)
				{
					square[now_line][now_column]->setColor(ccc3(190, 190, 190));
					square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
					square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
					square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));

					clearLine(now_line - 1, now_line);
				}
				newSquareType();
				return;
			}
		}

		if (now_line < LINE - 1)
		{
			for (int j = 0; j < 3; j++)//创建新色
			{
				square[now_line + 1][now_column + j]->setColor(ccc3(52, 228, 249));
				square[now_line + 1][now_column + j]->setTag(1);
			}
			square[now_line][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column]->setTag(0);
			square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column + 2]->setTag(0);
			if (now_line - 1 >= 0)
			{
				square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column + 1]->setTag(0);
			}
		}
		now_line++;
		break;

	case 4:
		if (now_line >= LINE - 1)//到底
		{
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line-1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line-1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 3, LINE - 1);
			newSquareType();
			return;
		}

		if (square[now_line + 1][now_column]->getTag() == 1 || square[now_line][now_column + 1]->getTag() == 1)//被下方挡住
		{
			is_fail();
			if (now_line - 2 >= 0)
			{
				square[now_line][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));

				clearLine(now_line - 2, now_line);
			}
			newSquareType();
			return;
		}

		if (now_line < LINE - 1)
		{
			//创建新色
			square[now_line + 1][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column]->setTag(1);
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
			//消除原色
			if (now_line - 2 >= 0)
			{
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);
			}
			if (now_line - 1 >= 0)
			{
				square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column + 1]->setTag(0);
			}
		}
		now_line++;
		break;

	case 5:
		if (now_line >= LINE - 1)//到底
		{
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 2, LINE - 1);
			newSquareType();
			return;
		}
		if (square[now_line][now_column]->getTag() == 1 || square[now_line + 1][now_column + 1]->getTag() == 1 || square[now_line][now_column + 2]->getTag() == 1)//被下方挡住
		{
			is_fail();
			if (now_line - 1 >= 0)
			{
				square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));

				clearLine(now_line - 1, now_line);
			}
			newSquareType();
			return;
		}

		if (now_line < LINE - 1)
		{
			//创建新色
			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			square[now_line][now_column + 2]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 2]->setTag(1);
			square[now_line + 1][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column + 1]->setTag(1);
			//消除原色
			if (now_line - 1 >= 0)
			{
				square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column]->setTag(0);
				square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column + 1]->setTag(0);
				square[now_line - 1][now_column + 2]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column + 2]->setTag(0);
			}
		}
		now_line++;
		break;

		//
	case 6:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 3, LINE - 1);
			newSquareType();
			return;
		}
		if (square[now_line][now_column]->getTag() == 1 || square[now_line + 1][now_column + 1]->getTag() == 1)
		{
			is_fail();
			if (now_line - 2 >= 0)
			{
				square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));

				clearLine(now_line - 2, now_line);
			}
			newSquareType();
			return;
		}


		if (now_line < LINE - 1)
		{
			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			square[now_line + 1][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column + 1]->setTag(1);

			if (now_line - 2 >= 0)
			{
				square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column + 1]->setTag(0);
			}
			if (now_line - 1 >= 0)
			{
				square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column]->setTag(0);
			}
		}
		now_line++;
		break;

		//
	case 7:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 2, LINE - 1);
			newSquareType();
			return;
		}
		if (square[now_line][now_column]->getTag() == 1 || square[now_line][now_column+1]->getTag()==1 || square[now_line+1][now_column+2]->getTag()==1)
		{
			is_fail();
			if (now_line - 1 >= 0)
			{
				square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));

				clearLine(now_line - 1, now_line);
			}
		    newSquareType();
			return;
		}

		if (now_line < LINE - 1)
		{
			for (int j = 0; j < 2; j++)
			{
				square[now_line][now_column + j]->setColor(ccc3(52, 228, 249));
				square[now_line][now_column + j]->setTag(1);
			}
			square[now_line + 1][now_column + 2]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column + 2]->setTag(1);

			if (now_line - 1 >= 0)
			{
				for (int k = 0; k < 3; k++)
				{
					square[now_line - 1][now_column + k]->setColor(ccc3(255, 255, 255));
					square[now_line - 1][now_column + k]->setTag(0);
				}
			}
		}
		now_line++;
		break;

		//
	case 8:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 3, LINE - 1);
			newSquareType();
			return;
		}
		for (int i = 0; i < 2; i++)
		{
			if (square[now_line + 1][now_column + i]->getTag() == 1)
			{
				is_fail();
				if (now_line - 2 >= 0)
				{
					square[now_line][now_column]->setColor(ccc3(190, 190, 190));
					square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
					square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
					square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));

					clearLine(now_line - 2, now_line);
				}
				newSquareType();
				return;
			}
		}

		if (now_line < LINE - 1)
		{
			for (int j = 0; j < 2; j++)
			{
				square[now_line + 1][now_column + j]->setColor(ccc3(52, 228, 249));
				square[now_line + 1][now_column + j]->setTag(1);
			}
			square[now_line][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column]->setTag(0);
			if (now_line - 2 >= 0)
			{
				square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column + 1]->setTag(0);
			}
		}
		now_line++;
		break;
		//
	case 9:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 2, LINE - 1);
			newSquareType();
			return;
		}
		for (int i = 0; i < 3; i++)
		{
			if (square[now_line + 1][now_column + i]->getTag() == 1)
			{
				is_fail();
				if (now_line - 1 >= 0)
				{
					square[now_line][now_column]->setColor(ccc3(190, 190, 190));
					square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
					square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
					square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));

					clearLine(now_line - 1, now_line);
				}
				newSquareType();
				return;
			}
		}

		if (now_line < LINE - 1)
		{
			for (int j = 0; j < 3; j++)
			{
				square[now_line + 1][now_column + j]->setColor(ccc3(52, 228, 249));
				square[now_line + 1][now_column + j]->setTag(1);
			}
			square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column + 1]->setTag(0);
			square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column + 2]->setTag(0);
			if (now_line - 1 >= 0)
			{
				square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column]->setTag(0);
			}
		}
		now_line++;
		break;
		//
	case 10:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line-1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line-2][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column+1]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 3, LINE - 1);
			newSquareType();
			return;
		}
		if (square[now_line + 1][now_column]->getTag() == 1)
		{
			is_fail();
			if (now_line - 2 >= 0)
			{
				square[now_line][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));

				clearLine(now_line - 2, now_line);
			}
			newSquareType();
			return;
		}
		if (now_line - 1 >= 0)//避免处于第0行时判断碰撞错误
		{
			if (square[now_line - 1][now_column + 1]->getTag() == 1)
			{
				square[now_line][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));
				clearLine(now_line - 2, now_line);
				newSquareType();
				return;
			}
		}

		if (now_line < LINE - 1)
		{
			square[now_line + 1][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column]->setTag(1);
			if (now_line - 1 >= 0) 
			{
				square[now_line - 1][now_column + 1]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column + 1]->setTag(1);
			}
			if (now_line - 2 >= 0)
			{
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);
				square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column + 1]->setTag(0);
			}
		}
		now_line++;
		break;
		//
	case 11:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 2, LINE - 1);
			newSquareType();
			return;
		}
		if (square[now_line + 1][now_column]->getTag() == 1 || square[now_line][now_column+1]->getTag()==1 || square[now_line][now_column+2]->getTag()==1)
		{
			is_fail();
			if (now_line - 1 >= 0)
			{
				square[now_line][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));

				clearLine(now_line - 1, now_line);
			}
			newSquareType();
			return;
		}

		if (now_line < LINE - 1)
		{
			square[now_line + 1][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column]->setTag(1);
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
			square[now_line][now_column + 2]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 2]->setTag(1);
			
			if (now_line - 1 >= 0)
			{
				for (int k = 0; k < 3; k++)
				{
					square[now_line - 1][now_column + k]->setColor(ccc3(255, 255, 255));
					square[now_line - 1][now_column + k]->setTag(0);
				}
			}
		}
		now_line++;
		break;
		//
	case 12:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 3, LINE - 1);
			newSquareType();
			return;
		}
		if (square[now_line+1][now_column+1]->getTag()==1)
     	{
			is_fail();
			if (now_line - 2 >= 0)
			{
				square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));

				clearLine(now_line - 2, now_line);
			}
			newSquareType();
			return;
		}
		if (now_line - 1 >= 0)//避免处于第0行时判断碰撞错误
		{
			if (square[now_line - 1][now_column]->getTag() == 1)
			{
				square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));
				clearLine(now_line - 2, now_line);
				newSquareType();
				return;
			}
		}

		if (now_line < LINE - 1)
		{
			if (now_line - 1 >= 0)
			{
				square[now_line - 1][now_column]->setColor(ccc3(52, 228, 249));
				square[now_line - 1][now_column]->setTag(1);
			}
			square[now_line + 1][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column + 1]->setTag(1);
			if (now_line - 2 >= 0)
			{
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);
				square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column + 1]->setTag(0);
			}
		}
		now_line++;
		break;
		//
	case 13:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 2, LINE - 1);
			newSquareType();
			return;
		}
		for (int i = 0; i < 3; i++)
		{
			if (square[now_line + 1][now_column + i]->getTag() == 1)
			{
				is_fail();
				if (now_line - 1 >= 0)
				{
					square[now_line][now_column]->setColor(ccc3(190, 190, 190));
					square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
					square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
					square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));

					clearLine(now_line - 1, now_line);
				}
				newSquareType();
				return;
			}
		}

		if (now_line < LINE - 1)
		{
			for (int j = 0; j < 3; j++)
			{
				square[now_line + 1][now_column + j]->setColor(ccc3(52, 228, 249));
				square[now_line + 1][now_column + j]->setTag(1);
			}
			square[now_line][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column]->setTag(0);
			square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column + 1]->setTag(0);
			if (now_line - 1 >= 0)
			{
				square[now_line - 1][now_column + 2]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column + 2]->setTag(0);
			}
		}
		now_line++;
		break;
		//
	case 14:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 3, LINE - 1);
			newSquareType();
			return;
		}
		for (int i = 0; i < 2; i++)
		{
			if (square[now_line + 1][now_column + i]->getTag() == 1)
			{
				is_fail();
				if (now_line - 2 >= 0)
				{
					square[now_line][now_column]->setColor(ccc3(190, 190, 190));
					square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
					square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
					square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));

					clearLine(now_line - 2, now_line);
				}
				newSquareType();
				return;
			}
		}

		if (now_line < LINE - 1)
		{
			for (int j = 0; j < 2; j++)
			{
				square[now_line + 1][now_column + j]->setColor(ccc3(52, 228, 249));
				square[now_line + 1][now_column + j]->setTag(1);
			}
			if (now_line - 2 >= 0)
			{
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);
			}
			square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column + 1]->setTag(0);
		}
		now_line++;
		break;
		//
	case 15:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 2, LINE - 1);
			newSquareType();
			return;
		}
		if (square[now_line][now_column]->getTag() == 1 || square[now_line+1][now_column+1]->getTag()==1 || square[now_line+1][now_column+2]->getTag()==1)
		{
			is_fail();
			if (now_line - 1 >= 0)
			{
				square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));

				clearLine(now_line - 1, now_line);
			}
		    newSquareType();
			return;
		}

		if (now_line < LINE - 1)
		{
			square[now_line + 1][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column + 1]->setTag(1);
			square[now_line + 1][now_column + 2]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column + 2]->setTag(1);
			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			if (now_line - 1 >= 0)
			{
				square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column]->setTag(0);
				square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column + 1]->setTag(0);
			}
			square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column + 2]->setTag(0);
		}
		now_line++;
		break;
		//
	case 16:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 3, LINE - 1);
			newSquareType();
			return;
		}
		if (square[now_line + 1][now_column]->getTag() == 1 || square[now_line][now_column+1]->getTag()==1)
		{
			is_fail();
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			if (now_line - 2 >= 0)
			{
				square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));

				clearLine(now_line - 2, now_line);
			}
			newSquareType();
			return;
		}

		if (now_line < LINE - 1)
		{
			square[now_line + 1][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column]->setTag(1);
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
			
			if (now_line - 1 >= 0)
			{
				square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column]->setTag(0);
			}
			if (now_line - 2 >= 0)
			{
				square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column + 1]->setTag(0);
			}
		}
		now_line++;
		return;
		//
	case 17:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 2, LINE - 1);
			newSquareType();
			return;
		}
	    if (square[now_line + 1][now_column]->getTag() == 1 || square[now_line+1][now_column+1]->getTag()==1 || square[now_line][now_column+2]->getTag()==1)
		{
			is_fail();
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			if (now_line - 1 >= 0)
			{
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));

				clearLine(now_line - 1, now_line);
			}
			newSquareType();
			return;
		}

		if (now_line < LINE - 1)
		{
			square[now_line + 1][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column]->setTag(1);
			square[now_line + 1][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column + 1]->setTag(1);
			square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 1]->setTag(1);
			square[now_line][now_column + 2]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column + 2]->setTag(1);

			square[now_line][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line][now_column]->setTag(0);
			if (now_line - 1 >= 0)
			{
				square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column + 1]->setTag(0);
				square[now_line - 1][now_column + 2]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column + 2]->setTag(0);
			}
		}
		now_line++;
		break;
		//
	case 18:
		if (now_line >= LINE - 1)
		{
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));
			clearLine(LINE - 3, LINE - 1);
			newSquareType();
			return;
		}
		if (square[now_line][now_column]->getTag() == 1 || square[now_line+1][now_column+1]->getTag()==1)
		{
			is_fail();
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			if (now_line - 1 >= 0)
			{
				square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			}
			if (now_line - 2 >= 0)
			{
				square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));

				clearLine(now_line - 2, now_line);
			}
			newSquareType();
			return;
		}

		if (now_line < LINE - 1)
		{
			square[now_line][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line][now_column]->setTag(1);
			square[now_line + 1][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column + 1]->setTag(1);
			
			if(now_line-1>=0)
			{
				square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - 1][now_column + 1]->setTag(0);
			}
			if (now_line - 2 >= 0)
			{
				square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - 2][now_column]->setTag(0);
			}
		}
		now_line++;
		break;
	}
} 

void major::updateLeft()
{
	switch (now_squaretype)
	{
	case 0:
		if (now_column - 1 < 0 || square[now_line][now_column - 1]->getTag() == 1)
		{
			return;
		}
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column - 1]->getTag() == 1)
				return;
		}
		square[now_line][now_column - 1]->setColor(ccc3(52, 228, 249));
		square[now_line][now_column - 1]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column - 1]->setColor(ccc3(52, 228, 249));
			square[now_line - 1][now_column - 1]->setTag(1);
		}

		square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 1]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 1]->setTag(0);
		}
		now_column--;
		break;

	case 1:
		if (now_column - 2 < 0 || square[now_line][now_column - 2]->getTag() == 1)
		{
			return;
		}
		square[now_line][now_column - 2]->setColor(ccc3(52, 228, 249));
		square[now_line][now_column - 2]->setTag(1);

		square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 2]->setTag(0);
		now_column--;
		break;

	case 2:
		for (int i = 0; i < 4; i++)
		{
			if (now_line - i >= 0)
			{
				if (now_column < 0 || square[now_line - i][now_column]->getTag() == 1)
					return;
			}
		}
		for (int j = 0; j < 4; j++)
		{
			if (now_line - j >= 0)
			{
				square[now_line - j][now_column]->setColor(ccc3(52, 228, 249));
				square[now_line - j][now_column]->setTag(1);

				square[now_line - j][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - j][now_column + 1]->setTag(0);
			}
		}
		now_column--;
		break;

	case 3:
		if (now_column - 1 < 0 || square[now_line][now_column - 1]->getTag() == 1)
			return;
		if (now_line - 1 >= 0) 
		{
			if (square[now_line - 1][now_column]->getTag() == 1)
				return;
		}
		square[now_line][now_column - 1]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column - 1]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column]->setTag(1);
		}

		square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 2]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 1]->setTag(0);
		}
		now_column--;
		break;

	case 4:
		for (int i = 0; i < 3; i++)
		{
			if (now_line - i >= 0)
			{
				if (now_column - 1 < 0 || square[now_line - i][now_column - 1]->getTag() == 1)
					return;
			}
		}
		for (int j = 0; j < 3; j++)
		{
			if (now_line - j >= 0)
			{
				square[now_line - j][now_column - 1]->setColor(ccc3(52, 225, 249));
				square[now_line - j][now_column - 1]->setTag(1);
			}
		}
		square[now_line][now_column]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column]->setTag(0);
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 2][now_column]->setTag(0);
		}
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 1]->setTag(0);
		}
		now_column--;
		break;

	case 5:
		if (now_column - 1 < 0 || square[now_line][now_column]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column - 1]->getTag() == 1)
				return;
		}
		square[now_line][now_column]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column - 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column - 1]->setTag(1);
		}
		square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 1]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 2]->setTag(0);
		}
		now_column--;
		break;

	case 6:
		if (now_column - 1 < 0 || square[now_line][now_column]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column - 1]->getTag() == 1)
				return;
		}
		if (now_line - 2 >= 0)
		{
			if (square[now_line - 2][now_column]->getTag() == 1)
				return;
		}
		square[now_line][now_column]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column - 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column - 1]->setTag(1);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column]->setColor(ccc3(52, 225, 249));
			square[now_line - 2][now_column]->setTag(1);
		}
		for (int i = 0; i < 3; i++)
		{
			if (now_line - i >= 0)
			{
				square[now_line - i][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - i][now_column + 1]->setTag(0);
			}
		}
		now_column--;
		break;

	case 7:
		if (now_column - 1 < 0 || square[now_line][now_column + 1]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column - 1]->getTag() == 1)
				return;
		}
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column - 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column - 1]->setTag(1);
		}
		square[now_line][now_column + 1]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column + 1]->setTag(1);

		square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 2]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 2]->setTag(0);
		}
		now_column--;
		break;

	case 8:
		if ( now_column - 1 < 0 || square[now_line][now_column - 1]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column]->getTag() == 1)
				return;
		}
		if (now_line - 2 >= 0)
		{
			if (square[now_line - 2][now_column]->getTag() == 1)
				return;
		}

		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column]->setColor(ccc3(52, 225, 249));
			square[now_line - 2][now_column]->setTag(1);
		}
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column]->setTag(1);
		}
		square[now_line][now_column - 1]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column - 1]->setTag(1);

		for (int i = 0; i < 3; i++)
		{
			if (now_line - i >= 0)
			{
				square[now_line - i][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - i][now_column + 1]->setTag(0);
			}
		}
		now_column--;
		break;

	case 9:
		if ( now_column - 1 < 0 || square[now_line][now_column - 1]->getTag() == 1 )
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column - 1]->getTag() == 1)
				return;
		}

		square[now_line][now_column - 1]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column - 1]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column - 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column - 1]->setTag(1);
		}

		square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 2]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
		}
		now_column--;
		break;

	case 10:
		for (int i = 0; i < 3; i++)
		{
			if (now_line - i >= 0)
			{
				if (now_column - 1 < 0 || square[now_line - i][now_column - 1]->getTag() == 1)
					return;
			}
		}
		for (int j = 0; j < 3; j++)
		{
			if (now_line - j >= 0)
			{
				square[now_line - j][now_column - 1]->setColor(ccc3(52, 225, 249));
				square[now_line - j][now_column - 1]->setTag(1);
			}
		}
		square[now_line][now_column]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 2][now_column + 1]->setTag(0);
		}
		now_column--;
		break;

	case 11:
		if (now_column - 1 < 0 || square[now_line][now_column - 1]->getTag() == 1 )
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column - 1]->getTag() == 1)
				return;
		}
		square[now_line][now_column - 1]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column - 1]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column - 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column - 1]->setTag(1);
		}

		square[now_line][now_column]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 2]->setTag(0);
		}
		now_column--;
		break;

	case 12:
		if (now_column - 1 < 0 || square[now_line][now_column]->getTag() == 1 )
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column]->getTag() == 1)
				return;
		}
		if (now_line - 2 >= 0)
		{
			if (square[now_line - 2][now_column - 1]->getTag() == 1)
				return;
		}
		square[now_line][now_column]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column]->setTag(1);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column - 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 2][now_column - 1]->setTag(1);
		}
		for (int i = 0; i < 3; i++)
		{
			if (now_line - i >= 0)
			{
				square[now_line - i][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - i][now_column + 1]->setTag(0);
			}
		}
		now_column--;
		break;

	case 13:
		if (now_column - 1 < 0 || square[now_line][now_column - 1]->getTag() == 1 )
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 1]->getTag() == 1)
				return;
		}
		square[now_line][now_column - 1]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column - 1]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 1]->setTag(1);
		}

		square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 2]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 2]->setTag(0);
		}
		now_column--;
		break;

	case 14:
		for (int i = 0; i < 3; i++)
		{
			if (now_line - i >= 0)
			{
				if (now_column - 1 < 0 || square[now_line - i][now_column - 1]->getTag() == 1)
					return;
			}
		}
		for (int j = 0; j < 3; j++)
		{
			if (now_line - j >= 0)
			{
				square[now_line - j][now_column - 1]->setColor(ccc3(52, 225, 249));
				square[now_line - j][now_column - 1]->setTag(1);
			}
		}

		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 2][now_column]->setTag(0);
		}
		square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 1]->setTag(0);
		now_column--;
		break;

	case 15:
		if (now_column - 1 < 0 || square[now_line][now_column]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column - 1]->getTag() == 1)
				return;
		}
		square[now_line][now_column]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column - 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column - 1]->setTag(1);
		}

		square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 2]->setTag(0);
		if (now_line - 1 >= 0) 
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 1]->setTag(0);
		}
		now_column--;
		break;

	case 16:
		if (now_column - 1 < 0 || square[now_line][now_column - 1]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column - 1]->getTag() == 1)
				return;
		}
		if (now_line - 2 >= 0)
		{
			if (square[now_line - 2][now_column]->getTag() == 1)
				return;
		}
		square[now_line][now_column - 1]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column - 1]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column - 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column - 1]->setTag(1);
		}
		if (now_line - 2 >= 0) 
		{
			square[now_line - 2][now_column]->setColor(ccc3(52, 225, 249));
			square[now_line - 2][now_column]->setTag(1);
		}

		square[now_line][now_column]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 1]->setTag(0);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 2][now_column + 1]->setTag(0);
		}
		now_column--;
		break;

	case 17:
		if (now_column - 1 < 0 || square[now_line][now_column - 1]->getTag() == 1 )
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column]->getTag() == 1)
				return;
		}
		square[now_line][now_column - 1]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column - 1]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column]->setTag(1);
		}

		square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 1]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 2]->setTag(0);
		}
		now_column--;
		break;

	case 18:
		if (now_column - 1 < 0 || square[now_line][now_column]->getTag() == 1 )
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column - 1]->getTag() == 1)
				return;
		}
		if (now_line - 2 >= 0)
		{
			if (square[now_line - 2][now_column - 1]->getTag() == 1)
				return;
		}
		square[now_line][now_column]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column - 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column - 1]->setTag(1);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column - 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 2][now_column - 1]->setTag(1);
		}

		square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 1]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 1]->setTag(0);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 2][now_column]->setTag(0);
		}
		now_column--;
		break;
	}
}
 
void major::updateRight()
{
	switch (now_squaretype)
	{
	case 0:
		if (now_column + 2 > COLUMN - 1 || square[now_line][now_column + 2]->getTag() == 1)
		{
			return;
		}
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 2]->getTag() == 1)
				return;
		}

		square[now_line][now_column + 2]->setColor(ccc3(52, 228, 249));
		square[now_line][now_column + 2]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 2]->setColor(ccc3(52,228,249));
			square[now_line - 1][now_column + 2]->setTag(1);
		}

		square[now_line][now_column]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
		}
		now_column++;
		break;

	case 1:
		if (now_column + 3 > COLUMN - 1 || square[now_line][now_column + 3]->getTag() == 1)
		{
			return;
		}
		square[now_line][now_column + 3]->setColor(ccc3(52, 228, 249));
		square[now_line][now_column + 3]->setTag(1);

		square[now_line][now_column - 1]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column - 1]->setTag(0);
		now_column++;
		break;

	case 2:
		for (int i = 0; i < 4; i++)
		{
			if (now_line - i >= 0)
			{
				if (now_column + 2 > COLUMN - 1 || square[now_line - i][now_column + 2]->getTag() == 1)
					return;
			}
		}
		for (int j = 0; j < 4; j++)
		{
			if (now_line - j >= 0)
			{
				square[now_line - j][now_column + 2]->setColor(ccc3(52, 228, 249));
				square[now_line - j][now_column + 2]->setTag(1);

				square[now_line - j][now_column + 1]->setColor(ccc3(255, 255, 255));
				square[now_line - j][now_column + 1]->setTag(0);
			}
		}
		now_column++;
		break;

	case 3:
		if (now_column + 3 > COLUMN - 1 || square[now_line][now_column + 3]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 2]->getTag() == 1)
				return;
		}
		square[now_line][now_column + 3]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column + 3]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 2]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 2]->setTag(1);
		}

		square[now_line][now_column]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 1]->setTag(0);
		}
		now_column++;
		break;

	case 4:
		if (now_column + 2 > COLUMN - 1 || square[now_line][now_column + 1]->getTag() == 1)
			return;;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 2]->getTag() == 1)
				return;
		}
		if (now_line - 2 >= 0)
		{
			if (square[now_line - 2][now_column + 1]->getTag() == 1)
				return;
		}

		square[now_line][now_column + 1]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column + 1]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 2]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 2]->setTag(1);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column + 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 2][now_column + 1]->setTag(1);
		}

		for (int i = 0; i < 3; i++)
		{
			if (now_line - i >= 0)
			{
				square[now_line - i][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - i][now_column]->setTag(0);
			}
		}
		now_column++;
		break;

	case 5:
		if (now_column + 3 > COLUMN - 1 || square[now_line][now_column + 2]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 3]->getTag() == 1)
				return;
		}
		square[now_line][now_column + 2]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column + 2]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 3]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 3]->setTag(1);
		}

		square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 1]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
		}
		now_column++;
		break;

	case 6:
		if (now_column + 2 > COLUMN - 1 || square[now_line][now_column + 2]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 2]->getTag() == 1)
				return;
		}
		if (now_line - 2 >= 0)
		{
			if (square[now_line - 2][now_column + 2]->getTag() == 1)
				return;
		}

		for (int i = 0; i < 3; i++)
		{
			if (now_line - i >= 0)
			{
				square[now_line - i][now_column + 2]->setColor(ccc3(52, 225, 249));
				square[now_line - i][now_column + 2]->setTag(1);
			}
		}

		square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 1]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 2][now_column + 1]->setTag(0);
		}
		now_column++;
		break;

	case 7:
		if (now_column + 3 > COLUMN - 1 || square[now_line][now_column + 3]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 3]->getTag() == 1)
				return;
		}
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 3]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 3]->setTag(1);
		}
		square[now_line][now_column + 3]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column + 3]->setTag(1);

		square[now_line][now_column + 2]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 2]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
		}
		now_column++;
		break;

	case 8:
		if (now_column + 2 > COLUMN - 1 || square[now_line][now_column + 2]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 2]->getTag() == 1)
				return;
		}
		if (now_line - 2 >= 0)
		{
			if (square[now_line - 2][now_column + 2]->getTag() == 1)
				return;
		}
		for (int i = 0; i < 3; i++)
		{
			if (now_line - i >= 0)
			{
				square[now_line - i][now_column + 2]->setColor(ccc3(52, 225, 249));
				square[now_line - i][now_column + 2]->setTag(1);
			}
		}

		square[now_line][now_column]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 1]->setTag(0);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 2][now_column + 1]->setTag(0);
		}

		now_column++;
		break;

	case 9:
		if (now_column + 3 > COLUMN - 1 || square[now_line][now_column + 3]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 1]->getTag() == 1)
				return;
		}
		square[now_line][now_column + 3]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column + 3]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 1]->setTag(1);
		}

		square[now_line][now_column]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
		}

		now_column++;
		break;

	case 10:
		if (now_column + 2 > COLUMN - 1 || square[now_line][now_column + 1]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 1]->getTag() == 1)
				return;
		}
		if (now_line - 2 >= 0)
		{
			if (square[now_line - 2][now_column + 3]->getTag() == 1)
				return;
		}

		square[now_line][now_column + 1]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column + 1]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 1]->setTag(1);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column + 2]->setColor(ccc3(52, 225, 249));
			square[now_line - 2][now_column + 2]->setTag(1);
		}

		for (int i = 0; i < 3; i++)
		{
			if (now_line - i >= 0)
			{
				square[now_line - i][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - i][now_column]->setTag(0);
			}
		}
		now_column++;
		break;

	case 11:
		if (now_column + 3 > COLUMN - 1 || square[now_line][now_column + 1]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 3]->getTag() == 1)
				return;
		}
		square[now_line][now_column + 1]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column + 1]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 3]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 3]->setTag(1);
		}

		square[now_line][now_column]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
		}
		now_column++;
		break;

	case 12:
		if (now_column + 2 > COLUMN - 1 || square[now_line][now_column + 2]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 2]->getTag() == 1)
				return;
		}
		if (now_line - 2 >= 0)
		{
			if (square[now_line - 2][now_column + 2]->getTag() == 1)
				return;
		}
		for (int i = 0; i < 3; i++)
		{
			if (now_line - i >= 0)
			{
				square[now_line - i][now_column + 2]->setColor(ccc3(52, 225, 249));
				square[now_line - i][now_column + 2]->setTag(1);
			}
		}

		square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 1]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 1]->setTag(0);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 2][now_column]->setTag(0);
		}

		now_column++;
		break;

	case 13:
		if (now_column + 3 > COLUMN - 1 || square[now_line][now_column + 3]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 3]->getTag() == 1)
				return;
		}
		square[now_line][now_column + 3]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column + 3]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 3]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 3]->setTag(1);
		}

		square[now_line][now_column]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 2]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 2]->setTag(0);
		}
		now_column++;
		break;

	case 14:
		if (now_column + 2 > COLUMN - 1 || square[now_line][now_column + 2]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 1]->getTag() == 1)
				return;
		}
		if (now_line - 2 >= 0)
		{
			if (square[now_line - 2][now_column + 1]->getTag() == 1)
				return;
		}

		square[now_line][now_column + 2]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column + 2]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 1]->setTag(1);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column + 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 2][now_column + 1]->setTag(1);
		}
		for (int i = 0; i < 3; i++)
		{
			if (now_line - i >= 0)
			{
				square[now_line - i][now_column]->setColor(ccc3(255, 255, 255));
				square[now_line - i][now_column]->setTag(0);
			}
		}
		now_column++;
		break;

	case 15:
		if (now_column + 3 > COLUMN - 1 || square[now_line][now_column + 3]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 2]->getTag() == 1)
				return;
		}
		square[now_line][now_column + 3]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column + 3]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 2]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 2]->setTag(1);
		}

		square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 1]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
		}
		now_column++;
		break;

	case 16:
		if (now_column + 2 > COLUMN-1 || square[now_line][now_column + 1]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 2]->getTag() == 1)
				return;
		}
		if (now_line - 2 >= 0)
		{
			if (square[now_line - 2][now_column + 2]->getTag() == 1)
				return;
		}
		square[now_line][now_column + 1]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column + 1]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 2]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 2]->setTag(1);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column + 2]->setColor(ccc3(52, 225, 249));
			square[now_line - 2][now_column + 2]->setTag(1);
		}

		square[now_line][now_column]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 2][now_column + 1]->setTag(0);
		}
		now_column++;
		break;

	case 17:
		if (now_column + 3 > COLUMN-1 || square[now_line][now_column + 2]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 3]->getTag() == 1)
				return;
		}
		square[now_line][now_column + 2]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column + 2]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 3]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 3]->setTag(1);
		}

		square[now_line][now_column]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 1]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column + 1]->setTag(0);
		}
		now_column++;
		break;

	case 18:
		if (now_column + 2 > COLUMN-1 || square[now_line][now_column+2]->getTag() == 1)
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line - 1][now_column + 2]->getTag() == 1)
				return;
		}
		if (now_line - 2 >= 0)
		{
			if (square[now_line - 2][now_column + 1]->getTag() == 1)
				return;
		}

		square[now_line][now_column+2]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column+2]->setTag(1);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column + 2]->setColor(ccc3(52, 225, 249));
			square[now_line - 1][now_column + 2]->setTag(1);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column + 1]->setColor(ccc3(52, 225, 249));
			square[now_line - 2][now_column + 1]->setTag(1);
		}

		square[now_line][now_column + 1]->setColor(ccc3(255, 255, 255));
		square[now_line][now_column + 1]->setTag(0);
		if (now_line - 1 >= 0)
		{
			square[now_line - 1][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 1][now_column]->setTag(0);
		}
		if (now_line - 2 >= 0)
		{
			square[now_line - 2][now_column]->setColor(ccc3(255, 255, 255));
			square[now_line - 2][now_column]->setTag(0);
		}
		now_column++;
		break;
	}
}

void major::is_fail()
{
	switch (now_squaretype)
	{
	case 0:
		if (now_line - 1 <= 0)
			gameover();
		break;

	case 1:
		if (now_line <= 0)
			gameover();
		break;

	case 2:
		if (now_line-3 <= 0)
			gameover();
		break;

	case 3:
		if (now_line-1 <= 0)
			gameover();
		break;

	case 4:
		if (now_line-2 <= 0)
			gameover();
		break;

	case 5:
		if (now_line-1 <= 0)
			gameover();
		break;

	case 6:
		if (now_line-2 <= 0)
			gameover();
		break;

	case 7:
		if (now_line-1 <= 0)
			gameover();
		break;

	case 8:
		if (now_line-2 <= 0)
			gameover();
		break;

	case 9:
		if (now_line-1 <= 0)
			gameover();
		break;

	case 10:
		if (now_line-2 <= 0)
			gameover();
		break;

	case 11:
		if (now_line-1 <= 0)
			gameover();
		break;

	case 12:
		if (now_line-2 <= 0)
			gameover();
		break;

	case 13:
		if (now_line-1 <= 0)
			gameover();
		break;

	case 14:
		if (now_line-2 <= 0)
			gameover();
		break;

	case 15:
		if (now_line-1 <= 0)
			gameover();
		break;

	case 16:
		if (now_line-2 <= 0)
			gameover();
		break;

	case 17:
		if (now_line-1 <= 0)
			gameover();
		break;


	case 18:
		if (now_line-2 <= 0)
			gameover();
		break;

	}
}

void major::updatescore()
{
	char buf[4] = { 0 };
	sprintf(buf, "%d", now_score);
	score_label->setString(buf);
}

void major::gameover()
{ 
	unschedule(schedule_selector(major::updateDown));
	CCUserDefault::sharedUserDefault()->setIntegerForKey("now_score", now_score);
	auto GameOverScene = gameover::create();
	Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0f,GameOverScene));
};

void major::pause_game(cocos2d::Object* pSender)
{
	if (is_game)
	{
		auto rect = Director::getInstance()->getOpenGLView()->getVisibleRect();
		float x = rect.origin.x + rect.size.width / 2;
		float y = rect.origin.y + rect.size.height / 2;
		pause_image = Sprite::create("pause.png");
		pause_image->setPosition(Vec2(x,y));
		pause_image->setScale(3.0f);
		this->addChild(pause_image, 10);
		Director::getInstance()->pause();
		//unschedule(schedule_selector(major::updateDown));
		//dispatcher->removeEventListener(myKeyListener);
		//removeEventListenerWithSceneGraphPriority(myKeyListener, this);
		is_game = false;
	}
	
}

void major::resume_game(cocos2d::Object* pSender)
{
	if (!is_game)
	{
		schedule(schedule_selector(major::updateDown), speed / (now_score + 5));
		this->removeChild(pause_image);
		Director::getInstance()->resume();
		//dispatcher->addEventListenerWithSceneGraphPriority(myKeyListener, this);
		//dispatcher->addEventListener(myKeyListener);
		is_game = true;
	}
}


