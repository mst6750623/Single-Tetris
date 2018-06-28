#include "HelloWorldScene.h"
#include "gameoverscene.h"
#include "rankinglist.h"
#include "string"

using namespace std;
USING_NS_CC;

Scene* RankingList::createScene()
{
	return RankingList::create();
}

RankingList::RankingList()
{

}
RankingList::~RankingList()
{ }

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool RankingList::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto rect = Director::getInstance()->getOpenGLView()->getVisibleRect();
	float x = rect.origin.x + rect.size.width / 2;
	float y = rect.origin.y + rect.size.height / 2;
	auto sprite = Sprite::create("rankinglist21.jpg");
	sprite->setPosition(Vec2(x,y-20));
	addChild(sprite, 0);

	auto homebutton = MenuItemImage::create("home.png", "home.png",
		CC_CALLBACK_1(RankingList::menuOKCallback, this));
	auto *phomebutton = Menu::create(homebutton, NULL);
	phomebutton->setPosition(Vec2(-10, 200));
	phomebutton->setScale(0.4f);
	addChild(phomebutton, 4);

	char name[3][17] = {0};
	for (int i = 0; i < CCUserDefault::sharedUserDefault()->getStringForKey("name1").length(); i++)
		{
			name[0][i] = CCUserDefault::sharedUserDefault()->getStringForKey("name1")[i];
		}
	for (int i = 0; i < CCUserDefault::sharedUserDefault()->getStringForKey("name2").length(); i++)
	{
		name[1][i] = CCUserDefault::sharedUserDefault()->getStringForKey("name2")[i];
	}
	for (int i = 0; i < CCUserDefault::sharedUserDefault()->getStringForKey("name3").length(); i++)
	{
		name[2][i] = CCUserDefault::sharedUserDefault()->getStringForKey("name3")[i];
	}
	
	
	int score[3] = { CCUserDefault::sharedUserDefault()->getIntegerForKey("score1"),
		CCUserDefault::sharedUserDefault()->getIntegerForKey("score2"),
		CCUserDefault::sharedUserDefault()->getIntegerForKey("score3") };

	auto FIRSTN = LabelTTF::create(name[0], "Arial",30);
	FIRSTN->setAnchorPoint(Vec2(0,0));
	FIRSTN->setPosition(Vec2(240, 125));
	addChild(FIRSTN, 2);
	char a[100] = { 0 };
	sprintf(a, "%d", score[0]);
	auto FIRSTS = LabelTTF::create(a, "Arial", 30);
	FIRSTS->setPosition(Vec2(215, 140));
	addChild(FIRSTS, 2);

	auto SECONDN = LabelTTF::create(name[1], "Arial", 30);
	SECONDN->setAnchorPoint(Vec2(0, 0));
	SECONDN->setPosition(Vec2(240, 68));
	addChild(SECONDN, 2);
	char b[10] = { 0 };
	sprintf(b, "%d", score[1]);
	auto SECONDS = LabelTTF::create(b, "Arial", 30);
	SECONDS->setPosition(Vec2(215, 83));
	addChild(SECONDS, 2);

	auto THIRDN = LabelTTF::create(name[2], "Arial", 30);
	THIRDN->setAnchorPoint(Vec2(0, 0));
	THIRDN->setPosition(Vec2(240, 13));
	addChild(THIRDN, 2);
	char c[10] = { 0 };
	sprintf(c, "%d", score[2]);
	auto THIRDS = LabelTTF::create(c, "Arial", 30);
	THIRDS->setPosition(Vec2(215, 28));
	addChild(THIRDS, 2);

	auto clear = LabelTTF::create("CLEAR ALL", "Arial", 20);
	auto pclear = MenuItemLabel::create(clear, CC_CALLBACK_1(RankingList::clearAll,this));
	auto ppclear = Menu::create(pclear, NULL);
	ppclear->setPosition(Vec2(170,170));
	ppclear->setColor(ccc3(255,255,0));
	addChild(ppclear, 3);

	
	
	return true;
}


void RankingList::menuItemSettingCallback(Ref* pSender)
{

	MenuItem* item = (MenuItem*)pSender;
	Director::getInstance()->replaceScene(TransitionFade::create(0.2f, major::createScene()));
}

void RankingList::menuOKCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->replaceScene(TransitionFadeUp::create(0.5f,HelloWorld::create()));
}

void RankingList::clearAll(cocos2d::Object* pSender)
{
	remove(CCUserDefault::getInstance()->getXMLFilePath().c_str());
}