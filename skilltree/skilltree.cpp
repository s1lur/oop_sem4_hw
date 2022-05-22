#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <memory>
#include "sfline.hpp"
#include <iostream>
#include <vector>

using std::cout, std::endl;



/*
    Icons from Ken111
    https://www.flaticon.com/ru/packs/game-skill?k=1650700359068

*/


class Node
{
public:
    Node(sf::Vector2f& position) 
        : mPosition{position}
    {
    }

    enum class State
    {
        Blocked,
        Unblocked,
        Activated
    };


    void addChild(const std::shared_ptr<Node>& child)
    {
        mChildren.push_back(child);
    }

    sf::Vector2f getPosition()
    {
        return mPosition;
    }

    void unblock()
    {
        mState = State::Unblocked;
    }

    virtual int block()
    {
        int pointsChange = 0;
        if (mState == State::Activated)
            pointsChange += 1;
        mState = State::Blocked;
        for (const auto& child : mChildren)
            pointsChange += child->block();
        return pointsChange;
    }

    virtual bool collisionTest(sf::Vector2f mouseCoords) = 0;

    virtual int onMousePressed(sf::Vector2f mouseCoords, sf::Mouse::Button mouseButton)
    {
        if (mState == State::Blocked)
            return 0;
        
        int pointsChange = 0;

        if (collisionTest(mouseCoords)) 
        {
            if (mState == State::Unblocked) 
            {
                pointsChange = -1;
                mState = State::Activated;
                for (const auto& child : mChildren)
                    child->unblock();
            }

            else if (mState == State::Activated)
            {
                pointsChange = 1;
                mState = State::Unblocked;
                for (const auto& child : mChildren)
                    pointsChange += child->block();
            }
        }

        for (const auto& child : mChildren)
        {
            pointsChange += child->onMousePressed(mouseCoords, mouseButton);
        }
        return pointsChange;
    }
    virtual void draw(sf::RenderWindow& window) const = 0;

protected:

    sf::Vector2f mPosition {0, 0};
    State mState = State::Blocked;

    std::vector<std::shared_ptr<Node>> mChildren {};

    inline static sf::Color sBlockedColor   {40, 40, 40};
    inline static sf::Color sUnlockedColor  {80, 80, 40};
    inline static sf::Color sActivatedColor {160, 160, 40};
};


class HitNode : public Node
{
public:

    HitNode(sf::Vector2f position) 
        : Node{position}
    {
    }

    virtual sf::String getIconPath() = 0;

    void loadTexture()
    {
        sf::String texturePath = getIconPath();
        if (!mTexture.loadFromFile(texturePath))
        {
            cout << "Error! Can't load file " << texturePath.toAnsiString() << endl;
            std::exit(1);
        }
        mSprite.setTexture(mTexture);
        mSprite.setOrigin({mRadius, mRadius});
        mSprite.setPosition(mPosition);
    }


    sf::Color getCurrentColor() const
    {
        if (mState == State::Unblocked)
            return sUnlockedColor;
        else if (mState == State::Activated)
            return sActivatedColor;
        return sBlockedColor;
    }

    void draw(sf::RenderWindow& window) const override
    {
        for (const auto& el : mChildren)
        {
            sfLine connectionLine {mPosition, el->getPosition(), getCurrentColor(), 2};
            connectionLine.draw(window);
            el->draw(window);
        }

        static sf::CircleShape shape(mRadius);
        shape.setOrigin({mRadius, mRadius});
        shape.setFillColor(getCurrentColor());
        shape.setPosition(mPosition);
        window.draw(shape);

        window.draw(mSprite);
    }

    bool collisionTest(sf::Vector2f mouseCoords) override
    {
        sf::Vector2f d = mPosition - mouseCoords;
        return d.x * d.x + d.y * d.y < mRadius * mRadius;
    }

private:

    sf::Texture mTexture;
    sf::Sprite mSprite;

    float mRadius = 24;
    bool mIsActivated = false;
};


class AccumulativeNode: public Node
{
public:
    AccumulativeNode(sf::Vector2f position) 
        : Node{position}
    {
    }

    virtual sf::String getIconPath() = 0;

    void loadTexture()
    {
        sf::String texturePath = getIconPath();
        if (!mTexture.loadFromFile(texturePath))
        {
            cout << "Error! Can't load file " << texturePath.toAnsiString() << endl;
            std::exit(1);
        }
        mSprite.setTexture(mTexture);
        mSprite.setOrigin({mRadius, mRadius});
        mSprite.setPosition(mPosition);
    }

    sf::Color getCurrentColor() const
    {
        if (mState == State::Unblocked)
        {
            if (mCurrentPoints == 0)
               return sUnlockedColor;
            else 
                return sPartlyActivatedColor;
        }
        else if (mState == State::Activated)
            return sActivatedColor;
        return sBlockedColor;
    }

    void unblock()
    {
        mState = State::Unblocked;
    }

    int block() override
    {
        int pointsChange = mCurrentPoints;
        mState = State::Blocked;
        mCurrentPoints = 0;
        for (const auto& child : mChildren)
            pointsChange += child->block();
        return pointsChange;
    }

    int onMousePressed(sf::Vector2f mouseCoords, sf::Mouse::Button mouseButton) override
    {
        if (mState == State::Blocked)
            return 0;
        int pointsChange = 0;

        if (collisionTest(mouseCoords)) 
        {
            if (mouseButton == sf::Mouse::Left)
            {
                if (mState == State::Unblocked) 
                {
                    pointsChange = -1;
                    if (mCurrentPoints == 0)
                    {
                        for (const auto& child : mChildren)
                            child->unblock();
                    }
                    if (mCurrentPoints < mMaxPoints)
                        mCurrentPoints += 1;
                    
                    if (mCurrentPoints == mMaxPoints)
                        mState = State::Activated;
                }
            }
            else if (mouseButton == sf::Mouse::Right)
            {
                pointsChange = 1;
                if (mCurrentPoints == 0)
                    return 0;
                mCurrentPoints--;
                mState = State::Unblocked;
                if (mCurrentPoints == 0)
                {
                    for (const auto& child : mChildren)
                        pointsChange += child->block();
                }
            }
        }
        for (const auto& child : mChildren)
        {
            pointsChange += child->onMousePressed(mouseCoords, mouseButton);
        }
        return pointsChange;
    }

    void draw(sf::RenderWindow& window) const override
    {
        for (const auto& el : mChildren)
        {
            sfLine connectionLine {mPosition, el->getPosition(), getCurrentColor(), 2};
            connectionLine.draw(window);
            el->draw(window);
        }

        static sf::RectangleShape shape;
        shape.setSize(sf::Vector2f(mRadius * 2 + 2, mRadius * 2 + 2));
        shape.setPosition(mPosition);
        shape.setFillColor(getCurrentColor());
        shape.setPosition(mPosition - sf::Vector2f(mRadius + 1, mRadius + 1));
        window.draw(shape);

        window.draw(mSprite);
        if (mState != State::Blocked)
        {
            sf::Font font;
            font.loadFromFile("HelveticaRegular.ttf");
            sf::Text currentPointsText;
            currentPointsText.setFont(font);
            currentPointsText.setString(std::to_string(mCurrentPoints) + "/" + std::to_string(mMaxPoints));
            currentPointsText.setPosition(mPosition + sf::Vector2f(-currentPointsText.getLocalBounds().width / 2 + 4, mRadius + 3));
            currentPointsText.setFillColor(sf::Color::White);
            currentPointsText.setCharacterSize(24);
            window.draw(currentPointsText);
        }
    }

    bool collisionTest(sf::Vector2f mouseCoords) override
    {
        sf::Vector2f d = mPosition - mouseCoords;
        return -mRadius - 1 < d.x && d.x < mRadius + 1 && -mRadius - 1 < d.y && d.y < mRadius + 1;
    }
private:
    sf::Texture mTexture;
    sf::Sprite mSprite;

    inline static sf::Color sPartlyActivatedColor {140, 140, 40};

    float mRadius = 32;
    unsigned int mCurrentPoints = 0;
    unsigned int mMaxPoints = 0;
protected:
    void setMaxPoints(int maxPoints) {
        mMaxPoints = maxPoints;
    }

};

class BombSkillNode : public HitNode
{
public:
    BombSkillNode(sf::Vector2f position) : HitNode{position} 
    {
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_bomb.png"};
    }
};

class SpikesSkillNode : public HitNode
{
public:
    SpikesSkillNode(sf::Vector2f position) : HitNode{position} 
    {
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_spikes.png"};
    }
};


class LightningSkillNode : public HitNode
{
public:
    LightningSkillNode(sf::Vector2f position) : HitNode{position} 
    {
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_lightning.png"};
    }
};


class EyeSkillNode : public HitNode
{
public:
    EyeSkillNode(sf::Vector2f position) : HitNode{position} 
    {
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_eye.png"};
    }
};


class ClawsSkillNode : public HitNode
{
public:
    ClawsSkillNode(sf::Vector2f position) : HitNode{position} 
    {
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_claws.png"};
    }
};

class ShieldSkillNode : public HitNode
{
public:
    ShieldSkillNode(sf::Vector2f position) : HitNode{position} 
    {
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_shield.png"};
    }
};


class SwordSkillNode : public HitNode
{
public:
    SwordSkillNode(sf::Vector2f position) : HitNode{position} 
    {
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_sword.png"};
    }
};


class ShurikenSkillNode : public HitNode
{
public:
    ShurikenSkillNode(sf::Vector2f position) : HitNode{position} 
    {
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_shuriken.png"};
    }
};

class WindSkillNode : public HitNode
{
public:
    WindSkillNode(sf::Vector2f position) : HitNode{position} 
    {
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_shuriken.png"};
    }
};


class MeteoriteSkillNode : public HitNode
{
public:
    MeteoriteSkillNode(sf::Vector2f position) : HitNode{position} 
    {
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_meteorite.png"};
    }
};

class HandSkillNode : public HitNode
{
public:
    HandSkillNode(sf::Vector2f position) : HitNode{position} 
    {
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_hand.png"};
    }
};

class EarthquakeSkillNode : public HitNode
{
public:
    EarthquakeSkillNode(sf::Vector2f position) : HitNode{position} 
    {
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_earthquake.png"};
    }
};

class FireballSkillNode : public HitNode
{
public:
    FireballSkillNode(sf::Vector2f position) : HitNode{position} 
    {
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_fireball.png"};
    }
};

class ChainAccumulativeSkillNode : public AccumulativeNode
{
public:
    ChainAccumulativeSkillNode(sf::Vector2f position) : AccumulativeNode{position}
    {
        setMaxPoints(6);
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_rect_chain.png"};
    }
};

class SwordAccumulativeSkillNode : public AccumulativeNode
{
public:
    SwordAccumulativeSkillNode(sf::Vector2f position) : AccumulativeNode{position}
    {
        setMaxPoints(5);
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_rect_sword.png"};
    }
};

class FreezeAccumulativeSkillNode : public AccumulativeNode
{
public:
    FreezeAccumulativeSkillNode(sf::Vector2f position) : AccumulativeNode{position}
    {
        setMaxPoints(7);
        loadTexture();
    }

    sf::String getIconPath() override
    {
        return sf::String{"icons/icon_rect_freeze.png"};
    }
};

class SkillTree 
{
public:
    SkillTree(float rootXPosition)
        : mRootXPosition{rootXPosition}
    { }

    void onMousePressed(sf::Vector2f mouseCoords, sf::Mouse::Button mouseButton)
    {
        int res = mRootNode->onMousePressed(mouseCoords, mouseButton);
        if (mCurrentPoints + res < 0)
            mRootNode->onMousePressed(mouseCoords, sf::Mouse::Right);
        else
            mCurrentPoints += res;
    }
    virtual void draw(sf::RenderWindow& window)
    {
        mRootNode->draw(window);
        sf::Font font;
        font.loadFromFile("HelveticaRegular.ttf");

        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setString(std::to_string(mCurrentPoints));
        scoreText.setPosition(sf::Vector2f(mRootXPosition - scoreText.getLocalBounds().width / 2 - 5, 530));
        scoreText.setFillColor(sf::Color::White);
        scoreText.setCharacterSize(40);
        window.draw(scoreText);

         sf::Text classNameText;
        classNameText.setFont(font);
        classNameText.setString(mClassName);
        classNameText.setPosition(sf::Vector2f(mRootXPosition - classNameText.getLocalBounds().width / 2 - 5, 600));
        classNameText.setFillColor(sf::Color::White);
        classNameText.setCharacterSize(40);
        window.draw(classNameText);
    }
private:
    int mCurrentPoints = 0;
    std::string mClassName = "";
protected:
    std::shared_ptr<Node> mRootNode;
    float mRootXPosition;

    void setCurrentPoints(int maxPoints) {
        mCurrentPoints = maxPoints;
    }
    void setClassName(const std::string& className) {
        mClassName = className;
    }
};


class MageSkillTree : public SkillTree 
{
public:
    MageSkillTree(float rootXPosition) : SkillTree{rootXPosition}
    {
        setCurrentPoints(10);
        setClassName("Mage");
        std::shared_ptr<Node> root {new FireballSkillNode({rootXPosition, 500})};
        std::shared_ptr<Node> wind {new WindSkillNode({rootXPosition - 200, 400})};
        std::shared_ptr<Node> earthquake {new EarthquakeSkillNode({rootXPosition + 100, 400})};
        root->addChild(wind);
        root->addChild(earthquake);
        wind->addChild(std::shared_ptr<Node>{new FreezeAccumulativeSkillNode({rootXPosition - 300, 200})});
        wind->addChild(std::shared_ptr<Node>{new LightningSkillNode({rootXPosition - 100, 200})});
        earthquake->addChild(std::shared_ptr<Node>{new MeteoriteSkillNode({rootXPosition + 100, 200})});

        mRootNode = root;
        mRootNode->unblock();
    }
};

class WarriorSkillTree : public SkillTree 
{
public:
    WarriorSkillTree(float rootXPosition) : SkillTree{rootXPosition}
    {
        setCurrentPoints(7);
        setClassName("Warrior");

        std::shared_ptr<Node> root {new SwordSkillNode({rootXPosition, 500})};
        std::shared_ptr<Node> accumulativeSword {new SwordAccumulativeSkillNode({rootXPosition - 100, 400})};
        std::shared_ptr<Node> shield {new ShieldSkillNode({rootXPosition + 100, 400})};
        root->addChild(accumulativeSword);
        root->addChild(shield);
        accumulativeSword->addChild(std::shared_ptr<Node>{new ClawsSkillNode({rootXPosition - 100, 200})});
        shield->addChild(std::shared_ptr<Node>{new BombSkillNode({rootXPosition + 100, 200})});

        mRootNode = root;
        mRootNode->unblock();
    }
};

class RogueSkillTree : public SkillTree 
{
public:
    RogueSkillTree(float rootXPosition) : SkillTree{rootXPosition}
    {
        setCurrentPoints(8);
        setClassName("Rogue");

        std::shared_ptr<Node> root {new HandSkillNode({rootXPosition, 500})};
        std::shared_ptr<Node> eye {new EyeSkillNode({rootXPosition - 100, 400})};
        std::shared_ptr<Node> shuriken {new ShurikenSkillNode({rootXPosition + 100, 400})};
        root->addChild(eye);
        root->addChild(shuriken);
        eye->addChild(std::shared_ptr<Node>{new ChainAccumulativeSkillNode({rootXPosition - 100, 200})});
        shuriken->addChild(std::shared_ptr<Node>{new SpikesSkillNode({rootXPosition + 100, 200})});

        mRootNode = root;
        mRootNode->unblock();
    }
};


int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Skill Tree", sf::Style::Close, settings);
    window.setFramerateLimit(60);

    std::vector<std::shared_ptr<SkillTree>> skillTrees {
        std::make_shared<MageSkillTree>(400),
        std::make_shared<WarriorSkillTree>(700),
        std::make_shared<RogueSkillTree>(1000)
    };

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();


            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2f mouseCoords = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                for (const auto& skillTree : skillTrees)
                    skillTree->onMousePressed(mouseCoords, event.mouseButton.button);
            }
        }


        window.clear(sf::Color::Black);
        for (const auto& skillTree : skillTrees)
            skillTree->draw(window);
        window.display();
    }

    return 0;
}