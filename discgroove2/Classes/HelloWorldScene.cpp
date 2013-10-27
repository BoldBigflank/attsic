//
//  HelloWorldScene.cpp
//  discgroove2
//
//  Created by Alex Swan on 10/19/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

#define PTM_RATIO 32
#define MAP_COUNT 5
#define MAX_DODGE_DISTANCE 72
#define DODGE_THRESHOLD 20.0
#define INITIAL_SCREEN_SPEED 250.0

enum {
    kTagParentNode = 1,
};

PhysicsSprite::PhysicsSprite()
: m_pBody(NULL)
{

}

void PhysicsSprite::setPhysicsBody(b2Body * body)
{
    m_pBody = body;
}

// this method will only get called if the sprite is batched.
// return YES if the physics values (angles, position ) changed
// If you return NO, then nodeToParentTransform won't be called.
bool PhysicsSprite::isDirty(void)
{
    return true;
}

// returns the transform matrix according the Chipmunk Body values
CCAffineTransform PhysicsSprite::nodeToParentTransform(void)
{
    b2Vec2 pos  = m_pBody->GetPosition();

    float x = pos.x * PTM_RATIO;
    float y = pos.y * PTM_RATIO;

    if ( isIgnoreAnchorPointForPosition() ) {
        x += m_obAnchorPointInPoints.x;
        y += m_obAnchorPointInPoints.y;
    }

    // Make matrix
    float radians = m_pBody->GetAngle();
    float c = cosf(radians);
    float s = sinf(radians);

    if( ! m_obAnchorPointInPoints.equals(CCPointZero) ){
        x += c*-m_obAnchorPointInPoints.x + -s*-m_obAnchorPointInPoints.y;
        y += s*-m_obAnchorPointInPoints.x + c*-m_obAnchorPointInPoints.y;
    }

    // Rot, Translate Matrix
    m_sTransform = CCAffineTransformMake( c,  s,
        -s,    c,
        x,    y );

    return m_sTransform;
}

string maps[] = {
    "intro.tmx",
    "map.tmx",
    "map1.tmx",
    "map2.tmx",
    "map3.tmx"
    
};

HelloWorld::HelloWorld()
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    screenPosition = 512; // Position in pixels
    screenSpeed = INITIAL_SCREEN_SPEED; // Pixels per second
    
    gameInProgress = false;
    
    setTouchEnabled( true );
    setAccelerometerEnabled( true );

    
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                                          "button.png",
                                                          "button-pressed.png",
                                                          this,
                                                          menu_selector(HelloWorld::newGame) );
    pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width /2, 128) );
    CCLabelTTF *newGameLabel = CCLabelTTF::create("NEW GAME", "Arial", 48);
    newGameLabel->setPosition(ccp(pCloseItem->getContentSize().width/2, pCloseItem->getContentSize().height/2 ));
    pCloseItem->addChild(newGameLabel);
    
    // create menu, it's an autorelease object
    _gameMenu = CCMenu::create(pCloseItem, NULL);
    _gameMenu->setPosition( CCPointZero );
    HelloWorld::addChild(_gameMenu, 10);
    
    // HUD Score Label
    _scoreLabel = new CCLabelTTF();
    _scoreLabel->initWithString("Score", "Arial", 32);
    _scoreLabel->setPosition(ccp(winSize.width * 0.2, winSize.height * 0.9));
    HelloWorld::addChild(_scoreLabel);
    
    _highScoreLabel = new CCLabelTTF();
    int highScore = CCUserDefault::sharedUserDefault()->getIntegerForKey("highScore");
    if(!highScore) highScore = 0;
    
    char scoreStr[17] = {0};
    sprintf(scoreStr, "High: %d", (int)highScore);
    _highScoreLabel->initWithString(scoreStr, "Arial", 32);
    _highScoreLabel->setPosition(ccp(winSize.width * 0.8, winSize.height * 0.9));
    HelloWorld::addChild(_highScoreLabel);

    
    
//    CCSize s = CCDirector::sharedDirector()->getWinSize();
    // init physics
    this->initPhysics();
    
    
    mapIndex = 0;
    mapHeight = 0;
    CCTMXTiledMap *tileMap;
    tileMap = new CCTMXTiledMap();
    tileMap->initWithTMXFile(maps[mapIndex].c_str());
    _background = tileMap->layerNamed("Background");
    
    CCTMXLayer *meta;
    meta = tileMap->layerNamed("Meta");
    meta->setVisible(false);
    
    
    mapIndex = (mapIndex + 1) % MAP_COUNT;
    mapHeight = tileMap->getMapSize().height * tileMap->getTileSize().height;
    
    tileMaps.push_back(tileMap);
    this->addChild(tileMap);
    
    CCTMXObjectGroup *objects = tileMap->objectGroupNamed("Objects");
    CCAssert(objects != NULL, "'Objects' object group not found");
    CCDictionary *spawnPoint = objects->objectNamed("Player");
    
    CCAssert(spawnPoint != NULL, "SpawnPoint object not found");
    float x = spawnPoint->valueForKey("x")->floatValue();
    float y = spawnPoint->valueForKey("y")->floatValue();
    
    this->player = new Player();
    player->init();
    player->setTag(543);
    this->player->setPosition(ccp(x, y));
    addChild(this->player);

    

    CCSpriteBatchNode *parent = CCSpriteBatchNode::create("blocks.png", 100);
    m_pSpriteTexture = parent->getTexture();

    addChild(parent, 0, kTagParentNode);


//    addNewSpriteAtPosition(ccp(s.width/2, s.height/2));
//
//    CCLabelTTF *label = CCLabelTTF::create("Tap screen", "Marker Felt", 32);
//    addChild(label, 0);
//    label->setColor(ccc3(0,0,255));
//    label->setPosition(ccp( s.width/2, s.height-50));
//    
    scheduleUpdate();
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("explode.wav");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("walking.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("tinkle.wav");
    gameInProgress = true;
    gameOver();
}

HelloWorld::~HelloWorld()
{
    delete world;
    world = NULL;
    
    //delete m_debugDraw;
}

void HelloWorld::initPhysics()
{

    CCSize s = CCDirector::sharedDirector()->getWinSize();

    b2Vec2 gravity;
    gravity.Set(0.0f, -10.0f);
    world = new b2World(gravity);

    // Do we want to let bodies sleep?
    world->SetAllowSleeping(true);

    world->SetContinuousPhysics(true);

//     m_debugDraw = new GLESDebugDraw( PTM_RATIO );
//     world->SetDebugDraw(m_debugDraw);

    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    //        flags += b2Draw::e_jointBit;
    //        flags += b2Draw::e_aabbBit;
    //        flags += b2Draw::e_pairBit;
    //        flags += b2Draw::e_centerOfMassBit;
    //m_debugDraw->SetFlags(flags);


    // Define the ground body.
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 0); // bottom-left corner

    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.
    b2Body* groundBody = world->CreateBody(&groundBodyDef);

    // Define the ground box shape.
    b2EdgeShape groundBox;

    // bottom

    groundBox.Set(b2Vec2(0,0), b2Vec2(s.width/PTM_RATIO,0));
    groundBody->CreateFixture(&groundBox,0);

    // top
    groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(s.width/PTM_RATIO,s.height/PTM_RATIO));
    groundBody->CreateFixture(&groundBox,0);

    // left
    groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(0,0));
    groundBody->CreateFixture(&groundBox,0);

    // right
    groundBox.Set(b2Vec2(s.width/PTM_RATIO,s.height/PTM_RATIO), b2Vec2(s.width/PTM_RATIO,0));
    groundBody->CreateFixture(&groundBox,0);
}

void HelloWorld::draw()
{
    //
    // IMPORTANT:
    // This is only for debug purposes
    // It is recommend to disable it
    //
    CCLayer::draw();

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );

    kmGLPushMatrix();

    world->DrawDebugData();

    kmGLPopMatrix();
}

void HelloWorld::addNewSpriteAtPosition(CCPoint p)
{
    CCLOG("Add sprite %0.2f x %02.f",p.x,p.y);
    CCNode* parent = getChildByTag(kTagParentNode);
    
    //We have a 64x64 sprite sheet with 4 different 32x32 images.  The following code is
    //just randomly picking one of the images
    int idx = (CCRANDOM_0_1() > .5 ? 0:1);
    int idy = (CCRANDOM_0_1() > .5 ? 0:1);
    PhysicsSprite *sprite = new PhysicsSprite();
    sprite->initWithTexture(m_pSpriteTexture, CCRectMake(32 * idx,32 * idy,32,32));
    sprite->autorelease();
    
    parent->addChild(sprite);
    
    sprite->setPosition( CCPointMake( p.x, p.y) );
    
    // Define the dynamic body.
    //Set up a 1m squared box in the physics world
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
    
    b2Body *body = world->CreateBody(&bodyDef);
    
    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(.5f, .5f);//These are mid points for our 1m box
    
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;    
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
    
    sprite->setPhysicsBody(body);
}


void HelloWorld::update(float dt)
{
    //It is recommended that a fixed time step is used with Box2D for stability
    //of the simulation, however, we are using a variable time step here.
    //You need to make an informed choice, the following URL is useful
    //http://gafferongames.com/game-physics/fix-your-timestep/
    
    int velocityIterations = 8;
    int positionIterations = 1;
    
    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    world->Step(dt, velocityIterations, positionIterations);
    
    //Iterate over the bodies in the physics world
    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
    {
        if (b->GetUserData() != NULL) {
            //Synchronize the AtlasSprites position and rotation with the corresponding body
            CCSprite* myActor = (CCSprite*)b->GetUserData();
            myActor->setPosition( CCPointMake( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO) );
            myActor->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );
        }    
    }

    // Update the screen position
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    if(gameInProgress)
        score += dt*10;
    
    // Update the player
    CCPoint playerPosition = player->getPosition();
    float playerRotation = player->getRotation();
    
    playerPosition.x = playerPosition.x +
        screenSpeed * dt * sin(CC_DEGREES_TO_RADIANS(playerRotation))
        + dt * player->getDodge();
    
    // Add dodge effects
    if(abs(player->getDiveAngle()) < DODGE_THRESHOLD){
        player->setIsTilted(false);
    } else if (!player->getIsTilted() && abs(player->getDiveAngle()) > DODGE_THRESHOLD ) {
        int sign = (player->getDiveAngle() < 0) ? -1 : 1;
        playerPosition.x += sign * MAX_DODGE_DISTANCE;
        player->setIsTilted(true);
        
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("tinkle.wav");
        CCParticleSystemQuad *explosion = new CCParticleSystemQuad();
        explosion->initWithFile("beeTail.plist");
        //    explosion->setScale(globalYScale);
        explosion->setEmissionRate(800);
        explosion->setPosition(player->getPosition());
        HelloWorld::addChild(explosion);
        explosion->setAutoRemoveOnFinish(true);
    }
    
//    player->setDodge( player->getDodge() - (playerPosition.x - player->getPositionX()));
    
    if( !canMoveToPosition(playerPosition)){
        // Reset it
        playerPosition.x = player->getPositionX();
    }
    
    playerPosition.y = playerPosition.y + screenSpeed * dt;
    
    if( !canMoveToPosition(playerPosition)){
        // Game over
        gameOver();
        return;
    }
    // Test the point to move
    
    
    player->setPosition(playerPosition);
    
    
    screenPosition += (screenSpeed * dt);
    this->setViewPoint();
//    float mapHeight = _tileMap->getMapSize().height * _tileMap->getTileSize().height;
    
    // Load the next screen
    if( screenPosition > mapHeight - winSize.height ){
        CCLog("Starting new one");
        CCTMXTiledMap *nextMap;
        nextMap = new CCTMXTiledMap();
        nextMap->initWithTMXFile(maps[mapIndex].c_str());
        nextMap->setPosition(ccp(0, mapHeight));

        CCTMXLayer *meta;
        meta = nextMap->layerNamed("Meta"); //meta->setLayerName("Meta");
        meta->setVisible(false);
        
        mapIndex = (mapIndex + 1) % MAP_COUNT;
        mapHeight += nextMap->getMapSize().height * nextMap->getTileSize().height;
        tileMaps.push_back(nextMap);
        this->addChild(nextMap, -1);
    }
    
    // Unload the previous screen
    if(tileMaps.size() > 3){
        CCLog("Erasing old one");
        // Unload the previous one
        CCTMXTiledMap *old = (CCTMXTiledMap*)tileMaps[0];
        this->removeChild(old);
        old->release();
        tileMaps.erase(tileMaps.begin());
    }
    _gameMenu->setPosition(ccp(0, screenPosition-512));
    _scoreLabel->setPosition(ccp(_scoreLabel->getPosition().x, _scoreLabel->getPosition().y + screenSpeed * dt));
    char scoreStr[17] = {0};
    sprintf(scoreStr, "Score: %d", (int)score);
    _scoreLabel->setString( scoreStr );
    
    
    _highScoreLabel->setPosition(ccp(_highScoreLabel->getPosition().x, _highScoreLabel->getPosition().y + screenSpeed * dt));
    
    
}

void HelloWorld::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
    //Add a new body/atlas sprite at the touched location
    CCSetIterator it;
    CCTouch* touch;
    
    for( it = touches->begin(); it != touches->end(); it++) 
    {
        touch = (CCTouch*)(*it);
        
        if(!touch)
            break;
        
        CCPoint location = touch->getLocationInView();
        
        location = CCDirector::sharedDirector()->convertToGL(location);
        
        location = this->convertToNodeSpace(location);
        
//        addNewSpriteAtPosition( location );
    }
}

void HelloWorld::setViewPoint() {
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    int x = winSize.width/2;
    int y = MAX(screenPosition, winSize.height/2);
    CCTMXTiledMap *map = (CCTMXTiledMap*)tileMaps[0];
    x = MIN(x, (map->getMapSize().width * map->getTileSize().width) - winSize.width / 2);
//    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height/2);
    CCPoint actualPosition = ccp(x, y);
    
    CCPoint centerOfView = ccp(winSize.width/2, winSize.height/2);
    CCPoint viewPoint = ccpSub(centerOfView, actualPosition);
    this->setPosition(viewPoint);
}

void HelloWorld::newGame(){
    if(!gameInProgress){
        gameInProgress = true;
        player->newGame();
        // Reset the score
        screenSpeed = INITIAL_SCREEN_SPEED;
        score = 0;
        // Hide the menu
        _gameMenu->setVisible(false);
        walkingInt = CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("walking.mp3", true);
    }
    
}

void HelloWorld::gameOver(){
    if(gameInProgress){
        gameInProgress = false;
        CocosDenshion::SimpleAudioEngine::sharedEngine()->stopEffect(walkingInt);
        // Blow up the player
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("explode.wav");
        CCParticleSystemQuad *explosion = new CCParticleSystemQuad();
        explosion->initWithFile("explosion.plist");
        //    explosion->setScale(globalYScale);
        explosion->setEmissionRate(800);
        explosion->setPosition(player->getPosition());
        HelloWorld::addChild(explosion);
        explosion->setAutoRemoveOnFinish(true);
        
        player->gameOver();
        
        
        screenSpeed = 0;
        
        int highScore = CCUserDefault::sharedUserDefault()->getIntegerForKey("highScore");
        if((int)score > highScore){
            CCUserDefault::sharedUserDefault()->setIntegerForKey("highScore", (int)score);
            CCUserDefault::sharedUserDefault()->flush();
            char scoreStr[17] = {0};
            sprintf(scoreStr, "High: %d", (int)score);
            
            _highScoreLabel->setString(scoreStr);
        }
        
        
        // Show the menu
        _gameMenu->setVisible(true);
    }
}

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // add layer as a child to scene
    CCLayer* layer = new HelloWorld();
    layer->setTag(443);
    scene->addChild(layer);
    layer->release();
    
    // add menu layer
    CCLayer *menuLayer = new CCLayer();
    menuLayer->setTag(8);
    menuLayer->init();
    
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.
    
    // add a "close" icon to exit the progress. it's an autorelease object

//    menuLayer->addChild(pMenu, 1);
    
    /////////////////////////////
    // 3. add your codes below...
    
    // add a label shows "Hello World"
    // create and initialize a label

    // ask director the window size
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
//    // add "HelloWorld" splash screen"
//    CCSprite* pSprite = CCSprite::create("HelloWorld.png");
//    
//    // position the sprite on the center of the screen
//    pSprite->setPosition( ccp(size.width/2, size.height/2) );
//    
//    // add the sprite as a child to this layer
//    menuLayer->addChild(pSprite, 0);
    scene->addChild(menuLayer);
    return scene;
}

bool HelloWorld::canMoveToPosition(CCPoint position)
{
    for(int i=0; i<tileMaps.size(); i++){
        CCTMXTiledMap *map = tileMaps[i];
        if(position.y > map->getPositionY() &&
           position.y < map->getPositionY() + map->getTileSize().height * map->getMapSize().height){
            int y = ((map->getMapSize().height * map->getTileSize().height + map->getPositionY()) - position.y) / map->getTileSize().height;
                int x = position.x / map->getTileSize().width;
            
            CCPoint tilePoint = ccp(x,y);
            if(x<0 || x>=map->getMapSize().width || y < 0 || y >= map->getMapSize().height) return false;
//            return ccp(x, y);
            
            
            map->layerNamed("Meta");
            CCTMXLayer *meta;
            meta = map->layerNamed("Meta");
            int tileGid = meta->tileGIDAt(tilePoint);
            if(tileGid) {
                CCDictionary *properties = map->propertiesForGID(tileGid);
                if(properties){
                    CCString *type = new CCString();
                    *type = *properties->valueForKey("type");
                    if(type && type->compare("block") == 0){
                        return false;
                    } else {
                        return true;
                    }
                }
            }
            return true;
            
        }
    }
    return true;
}
