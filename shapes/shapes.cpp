#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


// FROM LECTURE:  std::shared_ptr<sf::Shape> shape = std::make_shared<sf::CircleShape>(20);

class Shape
{
protected:
    std::string m_shapeName;
    float m_xpos, m_ypos;
    float m_xspeed, m_yspeed;
    int m_r, m_g, m_b;
    std::unique_ptr<sf::Shape> m_shape; //Keeps our drawable alive for each shape

public:
    Shape(std::string shapeName, float xpos, float ypos, float xspeed, float yspeed, int r, int g, int b)
        : m_shapeName(shapeName)
        , m_xpos(xpos)
        , m_ypos(ypos)
        , m_xspeed(xspeed)
        , m_yspeed(yspeed)
        , m_r(r)
        , m_g(g)
        , m_b(b)
    {
    }

    virtual ~Shape()
    {
        std::cout << "Destroy Shape\n";
    }

    void virtual buildShape(){}
};

class Circle : public Shape
{
    float m_radius;

public:
    Circle(std::string shapeName, float xpos, float ypos, float xspeed, float yspeed, int r, int g, int b, float radius)
        : Shape(shapeName, xpos, ypos, xspeed, yspeed, r, g, b)
        , m_radius (radius)
    {
    }

    void buildShape()
    {
        sf::CircleShape circle(m_radius);
        circle.setFillColor(sf::Color(m_r, m_g, m_b));
        circle.setPosition(m_xpos, m_ypos);
    }
};

class Rectangle : public Shape
{
    float m_width, m_height;

public:
    Rectangle(std::string shapeName, float xpos, float ypos, float xspeed, float yspeed, int r, int g, int b, float width, float height)
        : Shape(shapeName, xpos, ypos, xspeed, yspeed, r, g, b)
        , m_width (width)
        , m_height (height)
    {
    }

    void buildShape()
    {
        sf::RectangleShape rectangle(sf::Vector2f(m_width, m_height));
        rectangle.setFillColor(sf::Color(m_r, m_g, m_b));
        rectangle.setPosition(m_xpos, m_ypos);
    }
};



class ShapeList
{
    std::vector<Shape> m_shapeList;
    int m_wWidth;
    int m_wHeight;
    sf::Font m_font;
    int m_fontSize, m_fontR, m_fontG, m_fontB;

public:
        ShapeList() 
        {
            m_wWidth = 0;
            m_wHeight = 0;
            m_fontSize = 0;
            m_fontR = 0;
            m_fontG = 0;
            m_fontB = 0;
        }

        // Must be run after we load info from file
        sf::RenderWindow makeRenderWindow()
        {
            return sf::RenderWindow (sf::VideoMode(m_wWidth, m_wHeight), "Shapes");
        }
           
        void loadFromFile(const std::string& filename)
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
                    std::string font;
                    fin >> font >> m_fontSize >> m_fontR >> m_fontG >> m_fontB;
                    if (!m_font.loadFromFile(font))
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

                    m_shapeList.push_back(Circle(shapeName, xpos, ypos, xspeed, yspeed, r, g, b, radius));
                    m_shapeList.back().buildShape();
                }

                else if (command == "Rectangle")
                {
                    std::string shapeName;
                    float xpos, ypos;
                    float xspeed, yspeed;
                    int r, g, b;
                    float width, height;
                    fin >> shapeName >> xpos >> ypos >> xspeed >> yspeed >> r >> g >> b >> width >> height;

                    m_shapeList.push_back(Rectangle(shapeName, xpos, ypos, xspeed, yspeed, r, g, b, width, height));
                    m_shapeList.back().buildShape();
                }
                else 
                {
                    break;
                }
            }
        }


};

int main(int argc, char * argv[])
{
    const std::string inputFile = "input.txt";
    ShapeList shapes = ShapeList();
    shapes.loadFromFile(inputFile);

    sf::RenderWindow window = shapes.makeRenderWindow();

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
    }

    return 0;
}