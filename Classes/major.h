//#pragma once
#ifndef __MAJOR_SCENE_H__
#define __MAJOR_SCENE_H__

#include "cocos2d.h"
#include "HelloWorldScene.h"

#define LINE 22
#define COLUMN 10

class major : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();
	major();
	~major();
	virtual bool init();
	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	bool is_paused;
	void play(cocos2d::Object* pSender);
	void pause(cocos2d::Object* pSender);
	void menuOKCallback(cocos2d::Ref* pSender);

	CREATE_FUNC(major);

	void clearLine(int start_line, int end_line);
	void copyline(int line_num);
	void newSquareType();
	void nextSquareType();
	void updateDown(float dt);
	void updateLeft();
	void updateRight();
	void checkfail();
	void updatescore();
	void gameover();

	cocos2d::CCSprite* square[LINE][COLUMN];
	int now_line;
	int now_column;
	int now_squaretype;
	int now_score;
	cocos2d::LabelTTF* score_label;
};

#endif // __MAJOR_SCENE_H__