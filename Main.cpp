#include <SFML/Graphics.hpp>
#include "GraphicsBatch.hpp"
#include <SFML/Window.hpp>
#include <array>
#include <fstream>
#include <random>

struct TestInfo
{
    sf::RenderWindow*		 window;
    std::vector<sf::Texture> textures;
    std::vector<sf::Font>    fonts;
    int						 complexity;
    int						 randSeed;
};

void randomizeTransformable(const sf::View& view, sf::Transformable& transformable)
{
    std::uniform_int_distribution<int> random;

    transformable.setPosition(sf::Vector2f(std::rand() % (int)view.getSize().x, std::rand() % (int)view.getSize().y));
    transformable.setRotation(std::rand() % 360);
}

void drawBatchedSprites(TestInfo& testInfo)
{
    sf::GraphicsBatch batch;

    sf::Sprite sprite;

    int spritesLeft = testInfo.complexity;

    int drawOrder = 0;

    batch.begin(*testInfo.window);
    while (spritesLeft-- > 0)
    {
        sprite.setTexture(testInfo.textures[rand() % testInfo.textures.size()]);
        sprite.setTextureRect({ sf::Vector2i(0, 0), (sf::Vector2i)sprite.getTexture()->getSize() });

        randomizeTransformable(testInfo.window->getView(), sprite);

        batch.batch(sprite, sprite.getTexture(), sprite.getTransform(), drawOrder);

        if (!(spritesLeft % 30))
        {
            drawOrder++;
        }
    }
    batch.end();
}

void drawDefaultSprites(TestInfo& testInfo)
{
    sf::Sprite sprite;

    int spritesLeft = testInfo.complexity;

    int drawOrder = 0;

    while (spritesLeft-- > 0)
    {
        sprite.setTexture(testInfo.textures[rand() % testInfo.textures.size()]);
        sprite.setTextureRect({ sf::Vector2i(0, 0), (sf::Vector2i)sprite.getTexture()->getSize() });

        randomizeTransformable(testInfo.window->getView(), sprite);

        testInfo.window->draw(sprite);
    }
}

void drawBatchedText(TestInfo& testInfo)
{
    sf::GraphicsBatch batch;

    sf::Text text;
    text.setString("Hello World!");
    text.setCharacterSize(50);
    text.setOutlineColor(sf::Color::Blue);
    text.setOutlineThickness(6);


    int textsLeft = testInfo.complexity;

    int drawOrder = 0;

    batch.begin(*testInfo.window);
    while (textsLeft-- > 0)
    {
        text.setFont(testInfo.fonts[rand() % testInfo.fonts.size()]);

        randomizeTransformable(testInfo.window->getView(), text);

        batch.batch(text, &text.getFont()->getTexture(text.getCharacterSize()), text.getTransform(), drawOrder);

        if (!(textsLeft % 30))
        {
            drawOrder++;
        }
    }
    batch.end();
}

void drawDefaultText(TestInfo& testInfo)
{
    sf::Text text;
    text.setString("Hello World!");
    text.setCharacterSize(50);
    text.setOutlineColor(sf::Color::Blue);
    text.setOutlineThickness(6);

    int textsLeft = testInfo.complexity;

    int drawOrder = 0;

    while (textsLeft-- > 0)
    {
        text.setFont(testInfo.fonts[rand() % testInfo.fonts.size()]);

        randomizeTransformable(testInfo.window->getView(), text);

        testInfo.window->draw(text);
    }
}

int main()
{
    int seed = 13374201337;

    std::ofstream file("log.txt");

    sf::RenderWindow window;
    window.create(sf::VideoMode(1280, 720), "Hello SFML", sf::Style::Default);

    sf::View view;
    view.setCenter({ 0, 0 });
    view.setSize((sf::Vector2f)window.getSize());
    window.setView(view);


    TestInfo testInfo;
    testInfo.complexity = 100;
    testInfo.window = &window;

    testInfo.textures.resize(3);
    testInfo.fonts.resize(3);

    bool loadOk = true;
        
    loadOk = loadOk &&
        testInfo.textures[0].loadFromFile("Texture0.png") &&
        testInfo.textures[1].loadFromFile("Texture1.png") &&
        testInfo.textures[2].loadFromFile("Texture2.png");
    
    loadOk = loadOk &&
        testInfo.fonts[0].loadFromFile("Font0.ttf") &&
        testInfo.fonts[1].loadFromFile("Font1.ttf") &&
        testInfo.fonts[2].loadFromFile("Font2.ttf");

    if (!loadOk)
    {
        file << "Failed to load some of the assets!" << std::endl;
        return 0;
    }

    sf::Clock timer;

    ///////////////////////////////
    // Batched sprite test
    ///////////////////////////////

    window.clear(sf::Color::Black);

    srand(seed);
    timer.restart();

    drawBatchedSprites(testInfo);
    window.display();

    auto spritetime_batched = timer.restart().asMilliseconds();
    file << "Batched sprite draw finished in " << spritetime_batched << " milliseconds" << std::endl;

    sf::sleep(sf::seconds(6));

    ///////////////////////////////
    // Non-batched sprite test (regular draws)
    ///////////////////////////////

    window.clear(sf::Color::Black);

    srand(seed);
    timer.restart();

    drawDefaultSprites(testInfo);
    window.display();

    auto spritetime_normal = timer.restart().asMilliseconds();
    file << "Non-batched sprite draw finished in " << spritetime_normal << " milliseconds" << std::endl;

    ////////////////////////////////

    sf::sleep(sf::seconds(6));

    ///////////////////////////////
    // Batched text test
    ///////////////////////////////

    window.clear(sf::Color::Black);

    srand(seed);
    timer.restart();

    drawBatchedText(testInfo);
    window.display();

    auto texttime_batched = timer.restart().asMilliseconds();
    file << "Batched text draw finished in " << texttime_batched << " milliseconds" << std::endl;

    ////////////////////////////////

    sf::sleep(sf::seconds(6));

    ///////////////////////////////
    // Non-batched text test (regular draws)
    ///////////////////////////////

    window.clear(sf::Color::Black);

    srand(seed);
    timer.restart();

    drawDefaultText(testInfo);
    window.display();

    auto texttime_normal = timer.restart().asMilliseconds();
    file << "Non-batched text draw finished in " << texttime_normal << " milliseconds" << std::endl;

    sf::sleep(sf::seconds(6));

    file.close();
}
