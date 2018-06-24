#include "gameoverscene.h"
#include "SimpleAudioEngine.h"
#include "major.h"
#include "windows.h"
#include "string"
#include "HelloWorldScene.h"
#include "rankinglist.h"

using namespace std;
USING_NS_CC;

gameover::gameover()
{ 
	now_score = CCUserDefault::sharedUserDefault()->getIntegerForKey("now_score");
}

gameover::~gameover()
{
}

Scene* gameover::createScene()
{
	return gameover::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool gameover::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}
	auto sprite = Sprite::create("gameover.jpg");
	sprite->setScale(2.0f);
	sprite->setPosition(Vec2(240,200));
	this->addChild(sprite, 0);

	
	
	if (now_score > top_score[2])
	{
		auto name = LabelTTF::create("enter your name:", "Arial", 20);
		name->setPosition(Vec2(240, 170));
		name->setColor(ccc3(99, 184, 255));
		this->addChild(name, 2);
		editbox1->setPosition(Vec2(240, 150));
		editbox1->setPlaceHolder("Name");
		editbox1->setMaxLength(15);
		editbox1->setFontColor(ccc3(99, 184, 255));
		editbox1->setFontSize(15);
		addChild(editbox1, 1);
		
		auto ok = MenuItemImage::create("ok.png", "ok2.png", CC_CALLBACK_1(gameover::save, this));
		auto pok = Menu::create(ok, NULL);
		pok->setPosition(Vec2(240, 100));
		pok->setScale(1.0f);
		addChild(pok, 10);
	}
	auto homebutton = MenuItemImage::create("home.png", "home.png",
		CC_CALLBACK_1(gameover::menuOKCallback, this));
	auto *phomebutton = Menu::create(homebutton, NULL);
	phomebutton->setPosition(Vec2(0, 200));
	phomebutton->setScale(0.4f);
	this->addChild(phomebutton, 1);
	return true;
}

void gameover::save(cocos2d::Object* pSender)
{
	if (now_score > top_score[2] && now_score <= top_score[1])
	{
		CCUserDefault::sharedUserDefault()->setIntegerForKey("score3", now_score);
		string name1 = "NULL";
		name1 = editbox1->getText();
		CCUserDefault::sharedUserDefault()->setStringForKey("name3", name1);
	}
	else if (now_score > top_score[1] && now_score <= top_score[0])
	{
		int i = CCUserDefault::sharedUserDefault()->getIntegerForKey("score2");
		char j[17] = { 0 };
		for (int i = 0; i < CCUserDefault::sharedUserDefault()->getStringForKey("name2").length(); i++)
		{
			j[i] = CCUserDefault::sharedUserDefault()->getStringForKey("name2")[i];
		}
		CCUserDefault::sharedUserDefault()->setIntegerForKey("score3", i);
		CCUserDefault::sharedUserDefault()->setStringForKey("name3", j);//将第二个数据移到第三个

		CCUserDefault::sharedUserDefault()->setIntegerForKey("score2", now_score);
		string name1 = "NULL";
		name1 = editbox1->getText();
		CCUserDefault::sharedUserDefault()->setStringForKey("name2", name1);
	}
	else if (now_score > top_score[0])
	{
		int i = CCUserDefault::sharedUserDefault()->getIntegerForKey("score2");
		char j[17] = { 0 };
		for (int i = 0; i < CCUserDefault::sharedUserDefault()->getStringForKey("name2").length(); i++)
		{
			j[i] = CCUserDefault::sharedUserDefault()->getStringForKey("name2")[i];
		}
		CCUserDefault::sharedUserDefault()->setIntegerForKey("score3", i);
		CCUserDefault::sharedUserDefault()->setStringForKey("name3", j);//将第二个数据移到第三个

		int a = CCUserDefault::sharedUserDefault()->getIntegerForKey("score1");
		char b[17] = { 0 };
		for (int a = 0; a < CCUserDefault::sharedUserDefault()->getStringForKey("name1").length(); a++)
		{
			b[a] = CCUserDefault::sharedUserDefault()->getStringForKey("name1")[a];
		}
		CCUserDefault::sharedUserDefault()->setIntegerForKey("score2", a);
		CCUserDefault::sharedUserDefault()->setStringForKey("name2", b);//将第一个数据移到第二个

		CCUserDefault::sharedUserDefault()->setIntegerForKey("score1", now_score);
		string name1 = "NULL";
		name1 = editbox1->getText();
		CCUserDefault::sharedUserDefault()->setStringForKey("name1", name1);

		Director::getInstance()->replaceScene(TransitionFadeDown::create(0.3f, RankingList::createScene()));
	}
}

void gameover::menuOKCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->replaceScene(TransitionFlipAngular::create(0.5f,HelloWorld::create()));
}
