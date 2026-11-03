#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <filesystem>


// FROM LECTURE:  std::shared_ptr<sf::Shape> shape = std::make_shared<sf::CircleShape>(20)

class Shape
{
protected:
    std::string m_name;
    sf::Vector2f m_velocity;
    std::shared_ptr<sf::Shape> m_shape; //Keeps our drawable alive for each shape
    sf::Text m_text;

public:
    Shape(std::string name, float xspeed, float yspeed)
        : m_name (name)
        , m_velocity (xspeed, yspeed)
    {
    }

    virtual ~Shape() = default;

    void setUpText(sf::Font& font, unsigned size, sf::Color& color)
    {
        m_text.setFont(font);
        m_text.setString(m_name);
        m_text.setFillColor(color);
        m_text.setCharacterSize(size);
    }

    void update(const unsigned winW, const unsigned winH, const std::vector<std::shared_ptr<Shape>> &shapeList)
    {
        m_shape->move(m_velocity);

        // Our current shape's bounds
        sf::FloatRect curbounds = m_shape->getGlobalBounds();

        // Shape changes direction off of the window boundary
        if ((curbounds.left < 0 && m_velocity.x < 0) || (curbounds.width + curbounds.left > winW && m_velocity.x > 0))
            m_velocity.x = -m_velocity.x;
        if ((curbounds.top < 0 && m_velocity.y < 0) || (curbounds.top + curbounds.height > winH && m_velocity.y > 0))
            m_velocity.y = -m_velocity.y;

        // Handle collision with other shapes for this shape
        for (auto& shape : shapeList)
        {
            if (shape->getShape() == m_shape)
            {
                continue;
            }
            sf::FloatRect bounds = shape->getShape()->getGlobalBounds();
            sf::FloatRect overlap;
            if (curbounds.intersects(bounds, overlap))
            {
                float curCenterX = curbounds.left + curbounds.width / 2.f;
                float curCenterY = curbounds.top + curbounds.height / 2.f;
                float centerX = bounds.left + bounds.width / 2.f;
                float centerY = bounds.top + bounds.height / 2.f;

                if (overlap.width < overlap.height)
                {
                    if (curCenterX < centerX)
                    {
                        m_shape->move(-overlap.width, 0.f);
                    }
                    else
                    {
                        m_shape->move(overlap.width, 0.f);
                    }
                    m_velocity.x = -m_velocity.x;
                }
                else 
                {
                    if (curCenterY < centerY)
                    {
                        m_shape->move(0.f, -overlap.height);
                    }
                    else
                    {
                        m_shape->move(0.f, overlap.height);
                    }

                    m_velocity.y = -m_velocity.y;
                }

                // Update our current shape bounds after every change
                curbounds = m_shape->getGlobalBounds();
            }
        }


        sf::FloatRect text = m_text.getLocalBounds();
        m_text.setOrigin(text.left + text.width / 2.f, text.top + text.height / 2.f);
        m_text.setPosition(curbounds.left + curbounds.width / 2.f, curbounds.top + curbounds.height / 2.f);
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(*m_shape);
        window.draw(m_text);
    }

    std::shared_ptr<sf::Shape> getShape() { return m_shape; }
};

class Circle : public Shape
{
    float m_radius;

public:
    Circle(std::string& name, float xpos, float ypos, float xspeed, float yspeed, int r, int g, int b, float radius)
        : Shape(name, xspeed, yspeed)
        , m_radius (radius)
    {
        auto c = std::make_shared<sf::CircleShape>(radius);
        c->setFillColor(sf::Color(r, g, b));
        c->setPosition(xpos, ypos);
        m_shape = std::move(c);
    }
};

class Rectangle : public Shape
{
public:
    Rectangle(std::string name, float xpos, float ypos, float xspeed, float yspeed, int r, int g, int b, float width, float height)
        : Shape(name, xspeed, yspeed)
    {
        auto rect = std::make_shared<sf::RectangleShape>(sf::Vector2f(width, height));
        rect->setFillColor(sf::Color(r, g, b));
        rect->setPosition(xpos, ypos);
        m_shape = std::move(rect);
    }
};



class ShapeList
{
    std::vector<std::shared_ptr<Shape>> m_shapeList;
    unsigned m_wWidth = 800, m_wHeight = 600;
    sf::Font m_font;
    unsigned m_fontSize = 12;
    sf::Color m_fontColor = sf::Color::White;

public:
        // Must be run after we load info from file
        sf::RenderWindow makeRenderWindow()
        {
            return sf::RenderWindow (sf::VideoMode(m_wWidth, m_wHeight), "Shapes");
        }
           
        bool loadFromFile(const std::string& filename)
        {
            std::ifstream fin(filename);

            std::string command;
            while (fin >> command)
            {
                if (command == "Window")
                {
                    fin >> m_wWidth >> m_wHeight;
                }
                else if (command == "Font")
                {
                    std::string file;
                    int r, g, b;
                    fin >> file >> m_fontSize >> r >> g >> b;
                    m_fontColor = sf::Color(static_cast<sf::Uint8>(r), static_cast<sf::Uint8>(g), static_cast<sf::Uint8>(b));
                    if (!m_font.loadFromFile(file))
                    {
                        std::cerr << "Could not load font!";
                        exit(-1);
                    }
                }
                else if (command == "Circle")
                {
                    std::string shapeName;
                    float xpos, ypos;
                    float xspeed, yspeed;
                    int r, g, b;
                    float radius;
                    fin >> shapeName >> xpos >> ypos >> xspeed >> yspeed >> r >> g >> b >> radius;

                    m_shapeList.push_back(std::make_shared<Circle>(shapeName, xpos, ypos, xspeed, yspeed, r, g, b, radius));
                }

                else if (command == "Rectangle")
                {
                    std::string shapeName;
                    float xpos, ypos;
                    float xspeed, yspeed;
                    int r, g, b;
                    float width, height;
                    fin >> shapeName >> xpos >> ypos >> xspeed >> yspeed >> r >> g >> b >> width >> height;

                    m_shapeList.push_back(std::make_shared<Rectangle>(shapeName, xpos, ypos, xspeed, yspeed, r, g, b, width, height));
                }
                else
                {
                    std::cerr << "Unkown command '" << command << "', skipping line\n";
                    std::string rest;
                    std::getline(fin, rest);
                } 
            }

            for (auto& s : m_shapeList)
            {
                s->setUpText(m_font, m_fontSize, m_fontColor);
            }

            return true;
        }

        unsigned width() const { return m_wWidth; }
        unsigned height() const { return m_wHeight; }

        void update()
        {
            for (auto& s : m_shapeList)
            {
                s->update(m_wWidth, m_wHeight, m_shapeList);
            }
        }

        void draw(sf::RenderWindow& window) const
        {
            for (const auto& s : m_shapeList)
            {
                s->draw(window);
            }
        }
};

int main(int argc, char * argv[])
{   
    std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    std::cout << "input.txt exists: " << std::filesystem::exists("input.txt") << "\n";


    const std::string inputFile = "input.txt";
    ShapeList shapes;
    if(!shapes.loadFromFile(inputFile))
        return -1;

    sf::RenderWindow window(sf::VideoMode(shapes.width(), shapes.height()), "Shapes");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        sf::Event event;
        // Check if an event needs to be processed
        while (window.pollEvent(event));
        {
            // If the event typee is to close then window goes bye
            if (event.type == sf::Event::Closed) 
                window.close();
        }

        shapes.update();
        window.clear();

        shapes.draw(window);
        window.display();
    }

    return 0;
}