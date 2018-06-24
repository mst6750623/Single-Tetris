
#ifndef __GAMEOVER_SCENE_H__
#define __GAMEOVER_SCENE_H__
#ifndef EDIT_BOX_MANAGER
#define EDIT_BOX_MANAGER
#include "cocos2d.h"
#include "major.h"
#include "extensions\cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class gameover : public cocos2d::Scene
{
public:
	gameover();
	~gameover();
	static cocos2d::Scene* createScene();

	virtual bool init();
	void save(cocos2d::Ref* pSender);
	void menuOKCallback(cocos2d::Ref* pSender);
	void menuItemSettingCallback(cocos2d::Ref* pSender);
	// implement the "static create()" method manually
	CREATE_FUNC(gameover);

	cocos2d::ui::EditBox* editbox1 = EditBox::create(Size(200, 20), Scale9Sprite::create("editbox.png"));
	std::map< char[15], int> name;
	int top_score[3] = { CCUserDefault::sharedUserDefault()->getIntegerForKey("score1"),
		CCUserDefault::sharedUserDefault()->getIntegerForKey("score2") ,
		CCUserDefault::sharedUserDefault()->getIntegerForKey("score3") };
	int now_score;
};
#endif // EDIT_BOX_MANAGER
#endif // __GAMEOVER_SCENE_H__

