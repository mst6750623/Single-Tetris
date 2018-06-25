#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "major.h"
#include "gameoverscene.h"
#include "rankinglist.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

HelloWorld::HelloWorld()
{
	is_paused = false;
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("1(2).mp3", true);
}
HelloWorld::~HelloWorld()
{ }

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
		                                  "CloseNormal.png",
		                                  "CloseSelected.png", 
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
		closeItem->setScale(1.5f);
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
   
	auto* play = MenuItemImage::create("music1.png","music1.png" ,this, menu_selector(HelloWorld::pause));
	play->setPosition(Vec2(visibleSize.width / 2 + origin.x - 340, visibleSize.height / 2 + origin.y - 300));
	play->setScale(0.3f);
	auto* pplay = Menu::create(play, NULL);
	addChild(pplay,2);
    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

   auto label = Label::createWithTTF("TETRIS", "fonts/Marker Felt.ttf", 50);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }

	auto start = LabelTTF::create("START", "Arial", 40);
	auto pstart = MenuItemLabel::create(start, this, menu_selector(HelloWorld::menuItemSettingCallback));
	auto ppstart = Menu::create(pstart, NULL);
	ppstart->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - start->getContentSize().height - 110   ));
	this->addChild(ppstart, 2);

	auto Help = LabelTTF::create("HELP", "Arial", 30);
	auto pHelp = MenuItemLabel::create(Help, CC_CALLBACK_1(HelloWorld::switchtoHELP,this));
	auto ppHelp = Menu::create(pHelp, NULL);
	ppHelp->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - start->getContentSize().height - 160));
	this->addChild(ppHelp, 2);
	
	auto ranking = LabelTTF::create("Ranking List", "Arial", 25);
	auto pranking = MenuItemLabel::create(ranking, CC_CALLBACK_1(HelloWorld::swtichtorank, this));
	auto ppranking = Menu::create(pranking, NULL);
	ppranking->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - start->getContentSize().height - 210));
	this->addChild(ppranking, 2);

	auto speed_change = LabelTTF::create("Change Speed", "Arial", 25);
	auto pspeed_change = MenuItemLabel::create(speed_change, CC_CALLBACK_1(HelloWorld::swtichtospeed, this));
	auto ppspeed_change = Menu::create(pspeed_change, NULL);
	ppspeed_change->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - start->getContentSize().height - 260));
	this->addChild(ppspeed_change, 2);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("game1.jpg");
    if (sprite == nullptr)
    {
        problemLoading("'background.jpg'");
    }
    else
    {
        // position the sprite on the center of the screen
		sprite->setScale(1.0f);
        sprite->setPosition(Vec2(visibleSize.width/2 +origin.x, visibleSize.height/2 +origin.y));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }

	//this->addChild(help::create());
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
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


void HelloWorld::play(cocos2d::Object* pSender)
{
	if (is_paused)
	{
		CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
		removeChild(ppause);
	}
	else {
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("1(2).mp3" ,true );
		CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume();
	}
	is_paused = false;
}

void HelloWorld::pause(cocos2d::Object* pSender)
{
	is_paused = true;
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	auto* pause = MenuItemImage::create("music2.png", "music2.png", this, menu_selector(HelloWorld::play));
	pause->setPosition(Vec2(-96.5,-140));
	pause->setScale(0.3f);
	ppause = Menu::create(pause, NULL);
	addChild(ppause, 4);
}

void HelloWorld::menuItemSettingCallback(Ref* pSender)
{
	
	MenuItem* item = (MenuItem*)pSender;
	Director::getInstance()->replaceScene(TransitionSlideInB::create(0.5f,major::createScene()));
}

void HelloWorld::swtichtorank(cocos2d::Ref* pSender)
{
	Director::getInstance()->replaceScene(TransitionFadeDown::create(0.5f, RankingList::createScene()));
}


void HelloWorld::switchtoHELP(cocos2d::Object* pSender)
{
	this->addChild(help::create(),5);
	//Director::getInstance()->pushScene(help::create());
}

void HelloWorld::swtichtospeed(cocos2d::Ref* pSender)
{
	auto rect = Director::getInstance()->getOpenGLView()->getVisibleRect();
	float x = rect.origin.x + rect.size.width / 2;
	float y = rect.origin.y + rect.size.height / 2 - 20;
	sprite = Sprite::create("changespeed.jpg");
	sprite->setPosition(Vec2(x, y));
	sprite->setScale(2.0f);
	this->addChild(sprite, 5);

	auto slow = LabelTTF::create("SLOW", "Arial", 15);
	auto pslow = MenuItemLabel::create(slow, CC_CALLBACK_1(HelloWorld::menuItemChangeslowspeed,this));
	ppslow = Menu::create(pslow, NULL);
	ppslow->setPosition(Vec2(x,y+10));
	this->addChild(ppslow, 5);

	auto medium = LabelTTF::create("MEDIUM", "Arial", 15);
	auto pmedium = MenuItemLabel::create(medium, CC_CALLBACK_1(HelloWorld::menuItemChangemediumspeed, this));
	ppmedium = Menu::create(pmedium, NULL);
	ppmedium->setPosition(Vec2(x , y - 20));
	this->addChild(ppmedium, 5);

	auto fast = LabelTTF::create("FAST", "Arial", 15);
	auto pfast = MenuItemLabel::create(fast, CC_CALLBACK_1(HelloWorld::menuItemChangefastspeed, this));
    ppfast = Menu::create(pfast, NULL);
	ppfast->setPosition(Vec2(x, y - 50));
	this->addChild(ppfast, 5);
}


void HelloWorld::menuItemChangeslowspeed(cocos2d::Ref* pSender)
{
	removeChild(sprite, false);
	removeChild(ppslow, false);
	removeChild(ppmedium, false);
	removeChild(ppfast, false);
	CCUserDefault::sharedUserDefault()->setFloatForKey("speed", 4.5f);
}

void HelloWorld::menuItemChangemediumspeed(cocos2d::Ref* pSender)
{
	removeChild(sprite, false);
	removeChild(ppslow, false);
	removeChild(ppmedium, false);
	removeChild(ppfast, false);
	CCUserDefault::sharedUserDefault()->setFloatForKey("speed", 2.5f);
}

void HelloWorld::menuItemChangefastspeed(cocos2d::Ref* pSender)
{
	removeChild(sprite, false);
	removeChild(ppslow, false);
	removeChild(ppmedium, false);
	removeChild(ppfast, false);
	CCUserDefault::sharedUserDefault()->setFloatForKey("speed", 1.0f);
}

Scene* help::createScene()
{
	auto scene = Scene::create();
	auto first_layer = help::create();
	scene->addChild(first_layer);
	return scene;
}

bool help::init()
{
	auto rect = Director::getInstance()->getOpenGLView()->getVisibleRect();
	float x = rect.origin.x + rect.size.width / 2;
	float y = rect.origin.y + rect.size.height / 2-20; 
	sprite2 = Sprite::create("help.png");
	sprite2->setPosition(Vec2(x,y));
	sprite2->setScale(2.0f);
	this->addChild(sprite2,5);

	back = MenuItemImage::create("back1.png", "back2.png",
		CC_CALLBACK_1(help::menuOKCallback, this));
	pback = Menu::create(back, NULL);
	pback->setPosition(Vec2(-30, 100));
	pback->setScale(0.3f);
	this->addChild(pback, 5);

	return true;
}

void help::menuOKCallback(cocos2d::Ref* pSender)
{
	removeChild(sprite2, false);
	removeChild(pback, false);
	//Director::getInstance()->popScene();
}
