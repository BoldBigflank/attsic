//
//  Player.h
//  discgroove2
//
//  Created by Alex Swan on 10/23/13.
//
//

#ifndef __discgroove2__Player__
#define __discgroove2__Player__

#include <iostream>
#include <spine/spine-cocos2dx.h>

#include "cocos2d.h"
#include "Box2D.h"

USING_NS_CC;
using namespace std;

class Player : public cocos2d::CCNode{
private:
    float speed;
    float shootCooldown;
    CCSprite *shots[50];
    CCSprite *sprite;
    spine::CCSkeletonAnimation* skeletonNode;
    
public:
    virtual void update(float dt);
    virtual bool init();
    bool setUpPlayer(char* skin);
    void gameOver();
    void newGame();
    
    b2Body *_body;
    
    CC_SYNTHESIZE(int, hp_, Hp);
    CC_SYNTHESIZE(float, angle_, Angle);
    CC_SYNTHESIZE(float, diveAngle_, DiveAngle);
    CC_SYNTHESIZE(float, dodge_, Dodge);
    CC_SYNTHESIZE(bool, isTilted_, IsTilted);
    
    
    
};

#endif /* defined(__discgroove2__Player__) */
