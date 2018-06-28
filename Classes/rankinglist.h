#ifndef __RANKINGLIST_SCENE_H__
#define __RANKINGLIST_SCENE_H__

#include "cocos2d.h"
#include "major.h"
#include "map"
#include "string"

using namespace std;
USING_NS_CC;


class RankingList : public cocos2d::Scene
{
public:
	RankingList();
	~RankingList();
	static cocos2d::Scene* createScene();

	virtual bool init();
	void save(cocos2d::Ref* pSender);
	void menuOKCallback(cocos2d::Ref* pSender);
	void menuItemSettingCallback(cocos2d::Ref* pSender);
	void clearAll(cocos2d::Object* pSender);
	// implement the "static create()" method manually
	CREATE_FUNC(RankingList);

	std::map< int, string> rank;
	int scoretop[3];
	string nametop[3];
};
#endif // __RANKINGLIST_SCENE_H__


