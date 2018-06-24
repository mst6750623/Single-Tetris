#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "major.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
	void menuItemSettingCallback(cocos2d::Ref* pSender);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	void onWin32KeyEvent(UINT message, WPARAM wParam, LPARAM lParam);
#endif
	bool is_paused;
	void play(cocos2d::Object* pSender);
	void stop(cocos2d::Object* pSender);
	void pause(cocos2d::Object* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
