#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
using namespace std;
#define RAD_PER_DEG (float)(0.01745329238F)


int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(1000, 600), "Ant Simultaion Version 0.1");

    //create shapes
    sf::CircleShape ameisen[10];
    float dir[10];
    float dx;
    float dy;

    for(int i=0; i<10; i++)
    {
        float radius = 0.5;
        ameisen[i].setRadius(radius);
        ameisen[i].setFillColor(sf::Color::Green);
        ameisen[i].setPosition(500.f,300.f);
        dir[i]=i*30;

    }    
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

        // clear the window with black color
        //window.clear();
            
        // draw everything here...
        for(int i=0; i<10; i++)
        {
        //move shapes
        //float dx = 20* rand()/RAND_MAX -10;
        //float dy = 20* rand()/RAND_MAX -10;
        dir[i] = dir[i] + 20*rand()/RAND_MAX -10;
        dx = 2* cos(dir[i]*RAD_PER_DEG);
        dy = 2* sin(dir[i]*RAD_PER_DEG);
        ameisen[i].move(dx, dy);
        //Sleep(1);
        }
        for(int i=0; i<10; i++)
        {
        window.draw(ameisen[i]);
        }
        
        // end the current frame
        window.display();
    }
          
    return 0;
}