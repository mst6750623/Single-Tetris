#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__
#ifndef __HELP_H__
#define __HELP_H__
#include "cocos2d.h"
#include "major.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
	HelloWorld();
	~HelloWorld();
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
	void menuItemSettingCallback(cocos2d::Ref* pSender);
	void menuItemChangeslowspeed(cocos2d::Ref* pSender);
	void menuItemChangemediumspeed(cocos2d::Ref* pSender);
	void menuItemChangefastspeed(cocos2d::Ref* pSender);
	void switchtoHELP(cocos2d::Object* pSender);
	void swtichtorank(cocos2d::Ref* pSender);
	void swtichtospeed(cocos2d::Ref* pSender);
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	void onWin32KeyEvent(UINT message, WPARAM wParam, LPARAM lParam);
#endif
	bool is_paused;
	float rank;
	cocos2d::Sprite *sprite;
	cocos2d::Menu* ppslow;
	cocos2d::Menu* ppmedium;
	cocos2d::Menu* ppfast;
	cocos2d::Menu * ppause;
	void play(cocos2d::Object* pSender);
	void pause(cocos2d::Object* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};

class help : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	void menuOKCallback(cocos2d::Ref* pSender);
	CREATE_FUNC(help);
	cocos2d::Sprite* sprite2;
	cocos2d::MenuItemImage* back;
	cocos2d::Menu* pback;
};
#endif // __HELP_H__
#endif // __HELLOWORLD_SCENE_H__


