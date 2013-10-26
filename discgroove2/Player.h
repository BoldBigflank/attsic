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
#include "cocos2d.h"
#include "Box2D.h"

USING_NS_CC;
using namespace std;

class Player : public cocos2d::CCNode{
private:
    float speed;
    float shootCooldown;
    CCSprite *shots[50];
    
public:
    virtual void update(float dt);
    virtual bool init();
    bool setUpPlayer(char* skin);
    
    CC_SYNTHESIZE(int, hp_, Hp);
    
};

#endif /* defined(__discgroove2__Player__) */
