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
	void menuCloseCallback(cocos2d::Ref* pSender);//�رս��水ť
	void play(cocos2d::Object* pSender);//���ֲ���
	void pause(cocos2d::Object* pSender);//������ͣ
	void menuOKCallback(cocos2d::Ref* pSender);//�ص������水ť
	void Clearline_music();//�����е�����
	void pause_game(cocos2d::Object* pSender);//��Ϸ��ͣ
	void resume_game(cocos2d::Object* pSender);//��Ϸ����
	CREATE_FUNC(major);

	void clearLine(int start_line, int end_line);//������
	void copyline(int line_num);//���¸���
	void newSquareType();//�����·���
	void nextSquareType();//����任
	void updateDown(float dt);//�����ƶ�
	void updateLeft();//�����ƶ�
	void updateRight();//�����ƶ�
	void checkfail();//�ж��Ƿ���Ϸ����
	void updatescore();//���·���
	void gameover();//��Ϸ����
	void update(float delta);//��дupdate����ʵ�ּ��̰����¼�

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