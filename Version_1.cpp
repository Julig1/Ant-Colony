#include <SFML/Graphics.hpp>
#include <Windows.h>

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(1000, 600), "Ant Simultaion Version 0.1");

    //create shapes
    sf::CircleShape ameise(10.f);
    ameise.setFillColor(sf::Color::Green);
    ameise.setPosition(500.f,300.f);

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
        //move shapes
        float dx = 20* rand()/RAND_MAX -10;
        float dy = 20* rand()/RAND_MAX -11;
        ameise.move(dx, dy);
        Sleep(40);

        // clear the window with black color
        window.clear();
            
        // draw everything here...
        window.draw(ameise);
        
        // end the current frame
        window.display();
    }
          
    return 0;
}