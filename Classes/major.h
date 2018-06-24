//#pragma once
#ifndef __MAJOR_SCENE_H__
#define __MAJOR_SCENE_H__

#include "cocos2d.h"
#include "HelloWorldScene.h"
#include "map"

#define LINE 22
#define COLUMN 10

class major : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();
	major();
	~major();
	virtual bool init();
	void menuCloseCallback(cocos2d::Ref* pSender);

	bool is_paused;
	void play(cocos2d::Object* pSender);
	void pause(cocos2d::Object* pSender);
	void menuOKCallback(cocos2d::Ref* pSender);
	void Clearline_music();
	void pause_game(cocos2d::Object* pSender);
	void resume_game(cocos2d::Object* pSender);
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
	void update(float delta);

	cocos2d::Sprite* square[LINE][COLUMN];
	cocos2d::Sprite* next_square[4][4];
	int now_line;
	int now_column;
	int now_squaretype;
	int next_squaretype;
	int now_score;
	bool is_game;
	float speed = 3.0f;
	cocos2d::Menu * ppause;
	cocos2d::Sprite* pause_image;
	cocos2d::LabelTTF* score_label;
	cocos2d::EventListenerKeyboard* myKeyListener;
	cocos2d::EventDispatcher* dispatcher ;
	std::map<cocos2d::EventKeyboard::KeyCode, bool> key;
};



#endif // __MAJOR_SCENE_H__