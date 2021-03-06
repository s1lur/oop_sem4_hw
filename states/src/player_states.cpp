#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include "animation.hpp"
#include "player.hpp"
#include "player_states.hpp"
#include <cmath>

using std::cout, std::endl;

PlayerState::PlayerState() 
{
}


void PlayerState::updateSprite(sf::Sprite& sprite, bool isFacedRight, float scaleFactor)
{
    mAnimation.updateSprite(sprite);

    if (!isFacedRight)
    {
        sprite.setScale({-scaleFactor, scaleFactor});
    }
    else
    {
        sprite.setScale({scaleFactor, scaleFactor});
    }
}


void PlayerState::jump(Player* player)
{
    player->mPosition.y -= 1;
    player->mVelocity.y = -kJumpingVelocity;
    startFalling(player);
}

PlayerState::~PlayerState() 
{
}



sf::FloatRect operator*(float x, sf::FloatRect rect)
{
    return {x * rect.left, x * rect.top, x * rect.width, x * rect.height};
}



Idle::Idle(Player* player) 
{
    player->mVelocity = {0, 0};
    mAnimation = Animation();
    mAnimation.setAnimationSpeed(6);
    mAnimation.addTextureRect({ 14, 6, 21, 30});
    mAnimation.addTextureRect({ 64, 6, 21, 30});
    mAnimation.addTextureRect({114, 6, 21, 30});
    mAnimation.addTextureRect({164, 6, 21, 30});

    player->mCollisionRect =  player->mScaleFactor * sf::FloatRect(-10, -15, 20, 30);

    cout << "Creating Idle state" << endl;
}


void Idle::hook(Player* player) 
{  
}

void Idle::update(Player* player, float dt)
{
    mAnimation.update(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        player->setState(new Running(player));
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        player->setState(new Sitting(player));
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
    {
        player->setState(new FirstAttack(player));
    }
}

void Idle::handleEvents(Player* player, const sf::Event& event) 
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right)
        {
            player->setState(new Running(player));
        }

        else if (event.key.code == sf::Keyboard::Space)
        {
            jump(player);
        }

        else if (event.key.code == sf::Keyboard::LShift)
        {
            player->setState(new Sitting(player));
        }

        else if (event.key.code == sf::Keyboard::X)
        {
            player->setState(new FirstAttack(player));
        }
    }
}

void Idle::startFalling(Player* player)
{
    player->setState(new Falling(player));
}

void Idle::hitGround(Player* player)
{
}









Running::Running(Player* player) : PlayerState()
{
    mRunningSpeed = 900;
    mAnimation = Animation();
    mAnimation.setAnimationSpeed(12);
    mAnimation.addTextureRect({ 67, 45, 20, 27});
    mAnimation.addTextureRect({116, 46, 20, 27});
    mAnimation.addTextureRect({166, 48, 20, 27});
    mAnimation.addTextureRect({217, 45, 20, 27});
    mAnimation.addTextureRect({266, 46, 20, 27});
    mAnimation.addTextureRect({316, 48, 20, 27});

    player->mCollisionRect = player->mScaleFactor * sf::FloatRect(-10, -15, 20, 30);;

    cout << "Creating Running state" << endl;
}

void Running::hook(Player* player)
{
}


void Running::update(Player* player, float dt)
{
    mAnimation.update(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        player->mVelocity.x = -mRunningSpeed;
        player->mIsFacedRight = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        player->mVelocity.x = mRunningSpeed;
        player->mIsFacedRight = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
    {
        player->setState(new FirstAttack(player));
    }
}

void Running::handleEvents(Player* player, const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Space)
        {
            jump(player);
            return;
        }

        else if (event.key.code == sf::Keyboard::LShift)
        {
            player->setState(new Sliding(player));
        }

        else if (event.key.code == sf::Keyboard::X)
        {
            player->setState(new FirstAttack(player));
        }

    }
    else if (event.type == sf::Event::KeyReleased)
    {
        if (event.key.code == sf::Keyboard::Left && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            player->setState(new Idle(player));
            player->mVelocity.x = 0;
        }

        else if (event.key.code == sf::Keyboard::Right && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            player->setState(new Idle(player));
            player->mVelocity.x = 0;
        }
    }
}

void Running::startFalling(Player* player)
{
    player->setState(new Falling(player));
}

void Running::hitGround(Player* player)
{
}







Sliding::Sliding(Player* player) : PlayerState()
{
    if (player->mVelocity.x > 0)
        player->mVelocity.x = kSlidingVelocity;
    else if (player->mVelocity.x < 0)
        player->mVelocity.x = -kSlidingVelocity;

    mAnimation = Animation(Animation::AnimationType::OneIteration);
    mAnimation.setAnimationSpeed(10);
    mAnimation.addTextureRect({155, 119, 34, 28});
    mAnimation.addTextureRect({205, 119, 34, 28});
    mAnimation.addTextureRect({255, 119, 34, 28});
    mAnimation.addTextureRect({307, 119, 34, 28});
    mAnimation.addTextureRect({  9, 156, 34, 28});

    player->mCollisionRect = sf::FloatRect(-80, -20, 160, 80);
    player->mCollisionRect = player->mScaleFactor * sf::FloatRect(-20, -5, 40, 20);
    mCurrentTime = kSlidingTime;

    cout << "Creating Sliding state" << endl;    
}

void Sliding::hook(Player* player)
{
}


void Sliding::update(Player* player, float dt)
{
    mAnimation.update(dt);
    player->mVelocity.x *= kVelocityDecay;
    mCurrentTime -= dt;
    if (mCurrentTime < 0 && player->mIsColliding)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            player->setState(new Running(player));
        else
            player->setState(new Idle(player));
        return;
    }
}
void Sliding::handleEvents(Player* player, const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Space && player->mIsColliding)
        {
            jump(player);
            player->setState(new Falling(player));
        }
    }
}

void Sliding::startFalling(Player* player)
{

}

void Sliding::hitGround(Player* player)
{
}







Falling::Falling(Player* player) : PlayerState()
{
    mAnimation = Animation();
    mAnimation.setAnimationSpeed(12);
    mAnimation.addTextureRect({321, 155, 15, 26});

    player->mCollisionRect = player->mScaleFactor * sf::FloatRect(-10, -15, 20, 30);;

    mHasJumped = false;

    cout << "Creating Falling state" << endl;
}

void Falling::hook(Player* player)
{
    player->setState(new Hooked(player));
}


void Falling::update(Player* player, float dt)
{
    mAnimation.update(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        player->mVelocity.x = -kHorizontalVelocity;
        player->mIsFacedRight = false;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        player->mVelocity.x = kHorizontalVelocity;
        player->mIsFacedRight = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && mSpacePressedFrames < 100)
        mSpacePressedFrames += 1;
    else
        mSpacePressedFrames = 0;
}

void Falling::handleEvents(Player* player, const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Space
        && !mHasJumped
        && mSpacePressedFrames < 5) {
        player->mPosition.y -= 1;
        if (-player->mVelocity.y > kJumpingVelocity * 0.7)
            player->mVelocity.y -= kJumpingVelocity * 0.15;
        else 
            player->mVelocity.y = -kJumpingVelocity * 0.7;
        mHasJumped = true;
    }
}

void Falling::startFalling(Player* player)
{
}

void Falling::hitGround(Player* player)
{
    player->setState(new Idle(player));
}







Hooked::Hooked(Player* player) : PlayerState()
{
    mAnimation = Animation(Animation::AnimationType::OneIteration);
    mAnimation.setAnimationSpeed(12);
    mAnimation.addTextureRect({ 70, 151, 16, 34});
    mAnimation.addTextureRect({119, 151, 16, 34});
    mAnimation.addTextureRect({169, 151, 16, 34});
    mAnimation.addTextureRect({219, 151, 16, 34});

    player->mCollisionRect = player->mScaleFactor * sf::FloatRect(-10, -15, 20, 30);;

    cout << "Creating Hooked state" << endl;
}

void Hooked::hook(Player* player)
{
}


void Hooked::update(Player* player, float dt)
{
    player->mVelocity = {0, 0};
    mAnimation.update(dt);
}

void Hooked::handleEvents(Player* player, const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Space)
            jump(player);

        else if (event.key.code == sf::Keyboard::Down)
        {
            player->mVelocity.x = player->mIsFacedRight ? -100 : 100;
            player->setState(new Falling(player));
        }
    }
}

void Hooked::startFalling(Player* player)
{
    player->setState(new Falling(player));
}

void Hooked::hitGround(Player* player)
{
    player->setState(new Idle(player));
}


Sitting::Sitting(Player* player) 
{
    player->mVelocity = {0, 0};
    mAnimation = Animation(Animation::AnimationType::OneIteration);
    mAnimation.setAnimationSpeed(25);
    mAnimation.addTextureRect({ 65, 340, 19, 29});
    mAnimation.addTextureRect({116, 340, 22, 29});
    mAnimation.addTextureRect({168, 340, 20, 29});
    mAnimation.addTextureRect({221, 340, 15, 29});

    player->mCollisionRect = player->mScaleFactor * sf::FloatRect(-10, 0, 20, 15);
    
    cout << "Creating Sitting state" << endl;
}


void Sitting::hook(Player* player) 
{  
}

void Sitting::update(Player* player, float dt)
{
    mAnimation.update(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        player->setState(new Running(player));
    }
}

void Sitting::handleEvents(Player* player, const sf::Event& event) 
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right)
        {
            player->setState(new Running(player));
        }

        else if (event.key.code == sf::Keyboard::Space)
        {
            jump(player);
        }
    }
    else if (event.type == sf::Event::KeyReleased)
    {
        if (event.key.code == sf::Keyboard::LShift)
        {
            player->setState(new Idle(player));
        }
    }
}

void Sitting::startFalling(Player* player)
{
    player->setState(new Falling(player));
}

void Sitting::hitGround(Player* player)
{
}


FirstAttack::FirstAttack(Player* player) 
{
    mAnimation = Animation(Animation::AnimationType::OneIteration);
    mAnimation.setAnimationSpeed(12);
    mAnimation.addTextureRect({58, 238, 25, 20});
    mAnimation.addTextureRect({115, 222, 34, 36});
    mAnimation.addTextureRect({215, 226, 20, 32});
    mAnimation.addTextureRect({265, 232, 18, 26});

    player->mCollisionRect =  player->mScaleFactor * sf::FloatRect(-10, -15, 20, 30);
    if (player->mIsFacedRight)
        player->mSwordCollisionRect = player->mScaleFactor * sf::FloatRect(-15, -20, 38, 35);
    else
        player->mSwordCollisionRect = player->mScaleFactor * sf::FloatRect(-23, -20, 38, 35);
    mCurrentTime = kAttackTime;
    
    cout << "Creating First Attack state" << endl;
}


void FirstAttack::hook(Player* player) 
{  
}

void FirstAttack::update(Player* player, float dt)
{
    mAnimation.update(dt);
    player->mVelocity.x *= kVelocityDecay;
    mCurrentTime -= dt;
    if (mCurrentTime < 0 && player->mIsColliding)
    {
        if (secondAttackTriggered)
            player->setState(new SecondAttack(player));
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            player->setState(new Running(player));
            player->mSwordCollisionRect = sf::FloatRect(0, 0, 0, 0);
        }
        else
        {
            player->setState(new Idle(player));
            player->mSwordCollisionRect = sf::FloatRect(0, 0, 0, 0);
        }
        return;
    }
}

void FirstAttack::handleEvents(Player* player, const sf::Event& event) 
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Space && player->mIsColliding)
        {
            jump(player);
            player->setState(new Falling(player));
            player->mSwordCollisionRect = sf::FloatRect(0, 0, 0, 0);
        }
        else if (event.key.code == sf::Keyboard::X) {
            secondAttackTriggered = true;
        }
    }
}


void FirstAttack::startFalling(Player* player)
{
}

void FirstAttack::hitGround(Player* player)
{
}


SecondAttack::SecondAttack(Player* player) 
{
    mAnimation = Animation(Animation::AnimationType::OneIteration);
    mAnimation.setAnimationSpeed(12);
    mAnimation.addTextureRect({13, 268, 20, 27});
    mAnimation.addTextureRect({60, 266, 37, 29});
    mAnimation.addTextureRect({102, 274, 32, 21});
    mAnimation.addTextureRect({152, 273, 31, 22});

    player->mCollisionRect =  player->mScaleFactor * sf::FloatRect(-10, -15, 20, 30);
    if (player->mIsFacedRight)
        player->mSwordCollisionRect = player->mScaleFactor * sf::FloatRect(-15, -15, 45, 30);
    else
        player->mSwordCollisionRect = player->mScaleFactor * sf::FloatRect(-30, -15, 45, 30);
    mCurrentTime = kAttackTime;
    
    cout << "Creating Second Attack state" << endl;
}


void SecondAttack::hook(Player* player) 
{  
}

void SecondAttack::update(Player* player, float dt)
{
    mAnimation.update(dt);
    player->mVelocity.x *= kVelocityDecay;
    mCurrentTime -= dt;
    if (mCurrentTime < 0 && player->mIsColliding)
    {
        if (thirdAttackTriggered)
            player->setState(new ThirdAttack(player));
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            player->setState(new Running(player));
            player->mSwordCollisionRect = sf::FloatRect(0, 0, 0, 0);
        }
        else
        {
            player->setState(new Idle(player));
            player->mSwordCollisionRect = sf::FloatRect(0, 0, 0, 0);
        }
        return;
    }
}

void SecondAttack::handleEvents(Player* player, const sf::Event& event) 
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Space && player->mIsColliding)
        {
            jump(player);
            player->setState(new Falling(player));
            player->mSwordCollisionRect = sf::FloatRect(0, 0, 0, 0);
        }
        else if (event.key.code == sf::Keyboard::X) {
            thirdAttackTriggered = true;
        }
    }
}


void SecondAttack::startFalling(Player* player)
{
}

void SecondAttack::hitGround(Player* player)
{
}


ThirdAttack::ThirdAttack(Player* player) 
{
    mAnimation = Animation(Animation::AnimationType::OneIteration);
    mAnimation.setAnimationSpeed(14);
 
    mAnimation.addTextureRect({152, 273, 31, 22});
    mAnimation.addTextureRect({219, 269, 20, 26});
    mAnimation.addTextureRect({270, 269, 20, 26});
    mAnimation.addTextureRect({302, 272, 48, 23});

    mAnimation.addTextureRect({3, 313, 31, 19});
    mAnimation.addTextureRect({50, 312, 34, 20});
    mAnimation.addTextureRect({100, 312, 34, 20});

    player->mCollisionRect =  player->mScaleFactor * sf::FloatRect(-10, -15, 20, 30);
    if (player->mIsFacedRight)
        player->mSwordCollisionRect = player->mScaleFactor * sf::FloatRect(-23, -15, 65, 30);
    else
        player->mSwordCollisionRect = player->mScaleFactor * sf::FloatRect(-37, -15, 65, 30);
    mCurrentTime = kAttackTime;
    
    cout << "Creating Third Attack state" << endl;
}


void ThirdAttack::hook(Player* player) 
{  
}

void ThirdAttack::update(Player* player, float dt)
{
    mAnimation.update(dt);
    player->mVelocity.x *= kVelocityDecay;
    mCurrentTime -= dt;
    if (mCurrentTime < 0 && player->mIsColliding)
    {
        player->mSwordCollisionRect = sf::FloatRect(0, 0, 0, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            player->setState(new Running(player));
        else
            player->setState(new Idle(player));
        return;
    }
}

void ThirdAttack::handleEvents(Player* player, const sf::Event& event) 
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Space && player->mIsColliding)
        {
            player->mSwordCollisionRect = sf::FloatRect(0, 0, 0, 0);
            jump(player);
            player->setState(new Falling(player));
        }
    }
}


void ThirdAttack::startFalling(Player* player)
{
}

void ThirdAttack::hitGround(Player* player)
{
}