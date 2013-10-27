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
#define MAX_DODGE_DISTANCE 100
#define MAX_ROTATION 60
#define DODGE_TIME 0.5

using namespace spine;


bool Player::init()
{
    return this->setUpPlayer((char*)"blue");
}

bool Player::setUpPlayer(char* skin){
    isTilted_ = false;
    skeletonNode = new CCSkeletonAnimation("skeleton.json", "skeleton.atlas");
    skeletonNode->setMix("walk", "jump", 0.0f);
    skeletonNode->setMix("walk", "idle", 0.2f);
    skeletonNode->setMix("jump", "walk", 0.4f);
    skeletonNode->setMix("jump", "idle", 0.4f);
    skeletonNode->setMix("idle", "walk", 0.4f);
    skeletonNode->setMix("idle", "jump", 0.0f);
    
    
    skeletonNode->timeScale = 1.0f;
    skeletonNode->debugBones = false;
    skeletonNode->setSkin(skin);
    skeletonNode->setSlotsToSetupPose();
    
    skeletonNode->setAnimation("walk", true);
    
    this->addChild(skeletonNode);
    skeletonNode->release();
    
//    CCSprite *block = new CCSprite();
//    block->initWithFile("blocks.png");
//    this->addChild(block);
    
    this->scheduleUpdate(); // Updates are handled by the game layer
    
    
    // Crosshair is created by the weapon, for visual representation
    
//    CCSprite *crosshair = new CCSprite();
//    crosshair->initWithFile("crosshair.png");
//    crosshair->setTag(1);
//    crosshair->getTexture()->setAliasTexParameters();
//    crosshair->setPosition(ccp( cos(aimAngle + M_PI_2) * CROSSHAIR_DISTANCE , sin(aimAngle + M_PI_2) * CROSSHAIR_DISTANCE ));
//    crosshair->setScale(2.0);
//    this->addChild(crosshair);
    return true;
}

void Player::update(float dt){
    // Move the player a step
    if(angle_ < -1 * MAX_ROTATION) angle_= -1 * MAX_ROTATION;
    if(angle_ > MAX_ROTATION) angle_= MAX_ROTATION;
    
    this->setRotation(angle_);
    
    
    // Find the nearest baddy
        // If it's in range
            // Aim at it
            // If no cooldown, fire
}