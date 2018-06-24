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
	void menuCloseCallback(cocos2d::Ref* pSender);//关闭界面按钮
	void play(cocos2d::Object* pSender);//音乐播放
	void pause(cocos2d::Object* pSender);//音乐暂停
	void menuOKCallback(cocos2d::Ref* pSender);//回到主界面按钮
	void Clearline_music();//消除行的音乐
	void pause_game(cocos2d::Object* pSender);//游戏暂停
	void resume_game(cocos2d::Object* pSender);//游戏继续
	CREATE_FUNC(major);

	void clearLine(int start_line, int end_line);//消除行
	void copyline(int line_num);//向下复制
	void newSquareType();//生成新方块
	void nextSquareType();//方块变换
	void updateDown(float dt);//向下移动
	void updateLeft();//向左移动
	void updateRight();//向右移动
	void checkfail();//判定是否游戏结束
	void updatescore();//更新分数
	void gameover();//游戏结束
	void update(float delta);//重写update函数实现键盘按下事件

	cocos2d::Sprite* square[LINE][COLUMN];
	cocos2d::Sprite* next_square[4][4];
	int now_line;
	int now_column;
	int now_squaretype;
	int next_squaretype;
	int now_score;
	bool is_paused;
	bool is_game;
	float speed;
	cocos2d::Menu * ppause;
	cocos2d::Sprite* pause_image;
	cocos2d::LabelTTF* score_label;
	cocos2d::EventListenerKeyboard* myKeyListener;
	cocos2d::EventDispatcher* dispatcher ;
	std::map<cocos2d::EventKeyboard::KeyCode, bool> key;
};



#endif // __MAJOR_SCENE_H__