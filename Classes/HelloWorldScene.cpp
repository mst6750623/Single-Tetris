#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "major.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

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

	is_paused = false;
	auto* play = MenuItemImage::create("start.png","start.png" ,this, menu_selector(HelloWorld::play));
	play->setPosition(Vec2(visibleSize.width / 2 + origin.x - 320, visibleSize.height / 2 + origin.y - 310));
	play->setScale(1.0f);
	auto* pause = MenuItemImage::create("pause.png", "pause.png", this, menu_selector(HelloWorld::pause));
	pause->setPosition(Vec2(visibleSize.width / 2 + origin.x - 305, visibleSize.height / 2 + origin.y - 308));
	pause->setScale(1.0f);
	auto* pplay = Menu::create(play, NULL);
	auto* ppause = Menu::create(pause, NULL);
	addChild(pplay,2);
	addChild(ppause, 2);
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

	auto start = LabelTTF::create("START", "Arial", 30);
	auto pstart = MenuItemLabel::create(start, this, menu_selector(HelloWorld::menuItemSettingCallback));
	auto ppstart = Menu::create(pstart, NULL);
	ppstart->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - start->getContentSize().height - 150));
	this->addChild(ppstart, 2);


    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("background.jpg");
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
	}
	else {
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("mobanche.wav");
		CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume();
	}
	is_paused = false;
}

void HelloWorld::pause(cocos2d::Object* pSender)
{
	is_paused = true;
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

void HelloWorld::menuItemSettingCallback(Ref* pSender)
{
	
	MenuItem* item = (MenuItem*)pSender;
	auto sc = major::create();
	Director::getInstance()->pushScene(sc);
}