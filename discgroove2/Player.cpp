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

void Player::update(float dt){
    // Move the player a step
    
    // Find the nearest baddy
        // If it's in range
            // Aim at it
            // If no cooldown, fire
}