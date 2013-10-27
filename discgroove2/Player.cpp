//
//  Player.cpp
//  discgroove2
//
//  Created by Alex Swan on 10/23/13.
//
//

#include "Player.h"

// Box2D pixel to meters ratio
#define PTM_RATIO 32.0

// Spine animation states
#define SKELETON_STATE_TORSO 0
#define SKELETON_STATE_LEGS 1

// Player settings
#define MAX_COOLDOWN 1.0




bool Player::init()
{
    return this->setUpPlayer((char*)"green");
    
}

bool Player::setUpPlayer(char* skin){
    
//    skeletonNode = new CCSkeletonAnimation("skeleton.json", "skeleton.txt");
//    skeletonNode->setMix("walk", "shoot", 0.0f);
//    skeletonNode->setMix("walk", "idle-legs", 0.2f);
//    skeletonNode->setMix("shoot", "walk", 0.4f);
//    skeletonNode->setMix("shoot", "idle-torso", 0.4f);
//    skeletonNode->setMix("idle-legs", "walk", 0.4f);
//    skeletonNode->setMix("idle-torso", "shoot", 0.0f);
//    
//    
//    skeletonNode->timeScale = 1.0f;
//    skeletonNode->debugBones = true;
//    skeletonNode->setSkin(skin);
//    skeletonNode->setSlotsToSetupPose();
    
//    // Add a second state, so the legs act differently
//    skeletonNode->addAnimationState();
//    
//    skeletonNode->setAnimation("idle-torso", true, SKELETON_STATE_TORSO);
//    skeletonNode->setAnimation("idle-legs", true, SKELETON_STATE_LEGS);
//    
//    
//    this->addChild(skeletonNode);
//    skeletonNode->release();
//    
//    this->setHp(100);
//    this->setVelocity(CCPointZero);
//    this->isWalking = false;
//    this->isIdle = true;
//    this->isShooting = false;
//    this->willShoot = false;
//    this->scheduleUpdate(); // Updates are handled by the game layer
    
    
    // Crosshair is created by the weapon, for visual representation
    
    CCSprite *crosshair = new CCSprite();
    crosshair->initWithFile("crosshair.png");
    crosshair->setTag(1);
    crosshair->getTexture()->setAliasTexParameters();
    crosshair->setPosition(ccp( cos(aimAngle + M_PI_2) * CROSSHAIR_DISTANCE , sin(aimAngle + M_PI_2) * CROSSHAIR_DISTANCE ));
    crosshair->setScale(2.0);
    this->addChild(crosshair);
    return true;
}

void Player::update(float dt){
    // Move the player a step
    
    // Find the nearest baddy
        // If it's in range
            // Aim at it
            // If no cooldown, fire
}