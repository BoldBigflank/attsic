//
//  HelloWorldScene.h
//  discgroove2
//
//  Created by Alex Swan on 10/19/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//
#ifndef __HELLO_WORLD_H__
#define __HELLO_WORLD_H__

// When you import this file, you import all the cocos2d classes
#include "cocos2d.h"
#include "Box2D.h"
#include "Player.h"

using namespace cocos2d;

class PhysicsSprite : public cocos2d::CCSprite
{
public:
    PhysicsSprite();
    void setPhysicsBody(b2Body * body);
    virtual bool isDirty(void);
    virtual cocos2d::CCAffineTransform nodeToParentTransform(void);
private:
    b2Body* m_pBody;    // strong ref
};

class HelloWorld : public cocos2d::CCLayer {
public:
    ~HelloWorld();
    HelloWorld();
    
    // returns a Scene that contains the HelloWorld as the only child
    static cocos2d::CCScene* scene();
    
    void initPhysics();
    // adds a new sprite at a given coordinate
    void addNewSpriteAtPosition(cocos2d::CCPoint p);

    virtual void draw();
    virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    void update(float dt);
    void setViewPoint();
    void newGame();
    void gameOver();
    bool canMoveToPosition(CCPoint position);
    
private:
    b2World* world;
    cocos2d::CCTexture2D* m_pSpriteTexture; // weak ref
    int mapIndex;
    std::vector<CCTMXTiledMap*> tileMaps;
    int mapHeight;
    CCTMXLayer *_background;
    float screenPosition;
    float screenSpeed;
    Player *player;
    float score;
    bool gameInProgress;
    unsigned int walkingInt;
    int highScore;
    
    CCMenu *_gameMenu;
    CCLabelTTF *_highScoreLabel;
    CCLabelTTF *_scoreLabel;
    

};

#endif // __HELLO_WORLD_H__
