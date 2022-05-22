#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include "player.hpp"
#include "player_states.hpp"



Player::Player(sf::Vector2f position) : mPosition{position}
{
    if (!mTexture.loadFromFile("./hero.png"))
    {
        std::cerr << "Can't load image ./hero.png for Player class" << std::endl;
        std::exit(1);
    }

    setState(new Idle(this));

    mSprite.setTexture(mTexture);
    mSprite.setOrigin(mSprite.getLocalBounds().width / 2, mSprite.getLocalBounds().height / 2);
    mSprite.setPosition(mPosition);

    
    mScaleFactor = 4;
    mSprite.setScale(mScaleFactor, mScaleFactor);
}

void Player::setState(PlayerState* pNewState)
{
    delete mpState;
    mpState = pNewState;
}


sf::Vector2f Player::getCenter() const
{
    return mPosition;
}


void Player::applyVelocity(sf::Vector2f velocity)
{
    mVelocity += velocity;
}

void Player::update(float dt)
{
    mpState->update(this, dt);
    mPosition += mVelocity * dt;

    mSprite.setOrigin(mSprite.getLocalBounds().width / 2, mSprite.getLocalBounds().height / 2);
    mSprite.setPosition(mPosition);
    mpState->updateSprite(mSprite, mIsFacedRight, mScaleFactor);
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(mSprite);

    if (false) // For debuging
    {
        sf::RectangleShape shape {{mCollisionRect.width, mCollisionRect.height}};
        shape.setPosition(mPosition.x + mCollisionRect.left, mPosition.y + mCollisionRect.top);
        shape.setFillColor(sf::Color(150, 50, 50, 50));
        window.draw(shape);

        sf::RectangleShape swordShape {{mSwordCollisionRect.width, mSwordCollisionRect.height}};
        swordShape.setPosition(mPosition.x + mSwordCollisionRect.left, mPosition.y + mSwordCollisionRect.top);
        swordShape.setFillColor(sf::Color(50, 150, 50, 50));
        window.draw(swordShape);

        sf::Font font;
        font.loadFromFile("HelveticaRegular.ttf");
        sf::Text text;
        text.setFont(font);
        text.setString("x: " + std::to_string(static_cast<int>(mPosition.x)) + "; y: " + std::to_string(static_cast<int>(mPosition.y)));
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        text.setPosition({mPosition.x - 20, mPosition.y + 10});
        window.draw(text);

        sf::CircleShape center {6};
        center.setFillColor(sf::Color::Red);
        center.setOrigin(center.getRadius(), center.getRadius());
        center.setPosition(mPosition);
        window.draw(center);
    }
}


void Player::handleEvents(const sf::Event& event) 
{
    mpState->handleEvents(this, event);
}

bool Player::handleCollision(const sf::FloatRect& rect)
{
    sf::FloatRect playerRect = {mPosition.x + mCollisionRect.left, mPosition.y + mCollisionRect.top, mCollisionRect.width, mCollisionRect.height};

    float overlapx1 = playerRect.left + playerRect.width - rect.left;
    float overlapx2 = rect.left + rect.width - playerRect.left;
    float overlapy1 = playerRect.top + playerRect.height - rect.top;
    float overlapy2 = rect.top + rect.height - playerRect.top;

    if (overlapx1 < 0 || overlapx2 < 0 || overlapy1 < 0 || overlapy2 < 0)
        return false;


    int minOverlapDirection = 0;
    float minOvelap = overlapx1;
    if (overlapx2 < minOvelap) {minOverlapDirection = 1; minOvelap = overlapx2;}
    if (overlapy1 < minOvelap) {minOverlapDirection = 2; minOvelap = overlapy1;}
    if (overlapy2 < minOvelap) {minOverlapDirection = 3; minOvelap = overlapy2;}

    switch (minOverlapDirection)
    {
        case 0:
            mPosition.x -= overlapx1 - 1;
            if (mVelocity.y > 0 && playerRect.top < rect.top + Hooked::kMaxHookOffset && playerRect.top > rect.top - Hooked::kMaxHookOffset)
            {
                mpState->hook(this);
            }
            break;
        case 1:
            mPosition.x += overlapx2 - 1;
            if (mVelocity.y > 0 && playerRect.top < rect.top + Hooked::kMaxHookOffset && playerRect.top > rect.top - Hooked::kMaxHookOffset)
            {
                mpState->hook(this);
            }
            break;
        case 2:
            mPosition.y -= overlapy1 - 1;
            mVelocity.y = 0;
            mVelocity.y = 0;
            mpState->hitGround(this);
            break;
        case 3:
            mPosition.y += overlapy2 - 1;
            if (mVelocity.y < 0)
            {
                mVelocity.y = 0;
                mVelocity.y = 0;
            }
            break;
    }
    return true;
}

void Player::handleAllCollisions(const std::vector<sf::FloatRect>& blocks, const std::vector<sf::FloatRect>& enemies)
{
    mIsColliding = false;

    for (const sf::FloatRect& block : blocks)
    {
        if (handleCollision(block))
            mIsColliding = true;
    }
    for (const sf::FloatRect& enemy: enemies)
    {
        if (handleCollision(enemy))
            mIsColliding = true;
    }

    if (!mIsColliding)
        mpState->startFalling(this);
}

bool Player::handleAttackCollision(const sf::FloatRect& enemy)
{
    sf::FloatRect swordRect = {mPosition.x + mSwordCollisionRect.left, mPosition.y + mSwordCollisionRect.top, mSwordCollisionRect.width, mSwordCollisionRect.height};
    float overlapx1 = swordRect.left + swordRect.width - enemy.left;
    float overlapx2 = enemy.left + enemy.width - swordRect.left;
    float overlapy1 = swordRect.top + swordRect.height - enemy.top;
    float overlapy2 = enemy.top + enemy.height - swordRect.top;
    return (overlapx1 >= 0 && overlapx2 >= 0 && overlapy1 >= 0 && overlapy2 >= 0);
}


Player::~Player()
{
    delete mpState;
}