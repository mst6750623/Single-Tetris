#include "major.h"
#include "SimpleAudioEngine.h"
#include "HelloWorldScene.h"
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
	score_label = NULL;
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

	//初始化分数
	char buf[4] = { 0 };
	sprintf(buf, "%d", now_score);
	score_label = LabelTTF::create(buf, "Arial", 30);
	score_label->setPosition(Vec2(330,300));
	score_label->setColor(ccc3(190, 190, 190));
	addChild(score_label);

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
			this->addChild(square[i][j]);
		}
	}

	auto* backgroud = LayerColor::create(ccc4(0,0,0,0));
	this->addChild(backgroud);
	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
		"CloseSelected.png",
		"CloseSelected.png", this,
		menu_selector(major::menuCloseCallback));
	//CC_CALLBACK_1(Login::menuCloseCallback, this)
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
	this->addChild(menu, 1);

	/////////////////////////////
	// 3. add your codes below...

	// add a label shows "Hello World"
	// create and initialize a label

	
	auto home = MenuItemImage::create("home.png", "home.png", this, menu_selector(major::menuOKCallback));
	home->setPosition(Vec2(visibleSize.width / 2 + origin.x -345 , visibleSize.height / 2 + origin.y -20));
	home->setScale(0.4f);
	auto phome = Menu::create(home, NULL);
	addChild(phome, 2);

	is_paused = true;
	auto* play = MenuItemImage::create("start.png", "start.png", this, menu_selector(major::play));
	play->setPosition(Vec2(visibleSize.width / 2 + origin.x - 340, visibleSize.height / 2 + origin.y - 310));
	play->setScale(1.3f);
	auto* pause = MenuItemImage::create("pause.png", "pause.png", this, menu_selector(major::pause));
	pause->setPosition(Vec2(visibleSize.width / 2 + origin.x - 350, visibleSize.height / 2 + origin.y - 280));
	pause->setScale(1.3f);
	auto* pplay = Menu::create(play, NULL);
	auto* ppause = Menu::create(pause, NULL);
	addChild(pplay, 2);
	addChild(ppause, 2);

	auto myKeyListener = EventListenerKeyboard::create();
	myKeyListener->onKeyPressed = ([=](EventKeyboard::KeyCode keycode, cocos2d::Event *event)
	{
		switch (keycode)
		{
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW: //down
			updateDown(0.0);
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW: //right
			updateRight();
			break; 
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW: //left
			updateLeft();
			break;
		case EventKeyboard::KeyCode::KEY_SPACE://change
			nextSquareType();
			break;
		}
	});
	auto dispatcher = Director::getInstance()->getEventDispatcher();

	//添加到事件分发器  
	dispatcher->addEventListenerWithSceneGraphPriority(myKeyListener, this);

	//newSquareType();
	now_squaretype = 2;
	now_column = 3;
	now_line = 0;
	square[now_line][now_column + 1]->setColor(ccc3(52, 228, 249));
	square[now_line][now_column + 1]->setTag(1);
	schedule(schedule_selector(major::updateDown), 0.3f);

	return true;
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
	}
	else {
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("mobanche.wav");
		CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume();
	}
	is_paused = false;
}

void major::menuOKCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->popScene();
}

void major::pause(cocos2d::Object* pSender)
{
	is_paused = true;
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

void major::clearLine(int start_line, int end_line)
{
	//checkfail();
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
		}

	}
	updatescore();
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
	now_squaretype = (rand()*rand()) % 19;
	log("%d", now_squaretype);
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
			CCLOG("%d", now_line);
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
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(now_line, now_line - 1);
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
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 3][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(now_line - 3, now_line);
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
				square[now_line][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
				clearLine(now_line - 1, now_line);
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
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));
			clearLine(now_line - 2, now_line);
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
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));
			clearLine(now_line - 1, now_line);
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
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(now_line - 2, now_line);
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
			square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));
			clearLine(now_line - 1, now_line);
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
				square[now_line][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));
				clearLine(now_line - 2, now_line);
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
				square[now_line][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
				clearLine(now_line - 1, now_line);
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
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(now_line - 2, now_line);
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
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));
			clearLine(now_line - 1, now_line);
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
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(now_line - 2, now_line);
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
				square[now_line][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));
				clearLine(now_line - 1, now_line);
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
				square[now_line][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
				square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
				square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));
				clearLine(now_line - 2, now_line);
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
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 2]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(now_line - 1, now_line);
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
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column + 1]->setColor(ccc3(190, 190, 190));
			clearLine(now_line - 2, now_line);
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
			square[now_line][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 2]->setColor(ccc3(190, 190, 190));
			clearLine(now_line - 1, now_line);
			newSquareType();
			return;
		}

		if (now_line < LINE - 1)
		{
			square[now_line + 1][now_column]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column]->setTag(1);
			square[now_line + 1][now_column + 1]->setColor(ccc3(52, 228, 249));
			square[now_line + 1][now_column + 1]->setTag(1);
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
			square[now_line][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column]->setColor(ccc3(190, 190, 190));
			square[now_line - 1][now_column + 1]->setColor(ccc3(190, 190, 190));
			square[now_line - 2][now_column]->setColor(ccc3(190, 190, 190));
			clearLine(now_line - 2, now_line);
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
		if (now_column - 1 < 0 || square[now_line - 1][now_column]->getTag() == 1 )
			return;
		if (now_line - 1 >= 0)
		{
			if (square[now_line][now_column - 1]->getTag() == 1)
				return;
		}
		square[now_line][now_column - 1]->setColor(ccc3(52, 225, 249));
		square[now_line][now_column - 1]->setTag(1);
		if (now_line - 1 > 0)
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
		if (now_column + 3 > COLUMN - 1 || square[now_line][now_column + 2]->getTag() == 1)
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
		if (now_column + 3 > COLUMN-1 || square[now_line][now_column + 1]->getTag() == 1)
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

void major::checkfail()
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
