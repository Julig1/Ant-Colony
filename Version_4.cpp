#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
using namespace std;

//Global variables
#define RAD_PER_DEG (float)(0.01745329238F)
#define SPEED (float)(2.0F)
#define MAX_X 1000
#define MAX_Y 1000
#define NUMBER_OF_ANTS 10


int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(MAX_X, MAX_Y), "Ant Simultaion Version 0.1");
    
    //create image
    sf::Image image;
    image.create(MAX_X, MAX_Y, sf::Color::Black);

    // create texture
    sf::Texture texture;

    //create sprite
    sf::Sprite sprite;

    //create shapes
    sf::CircleShape ameisen[NUMBER_OF_ANTS];
    float dir[NUMBER_OF_ANTS];
    float dx;
    float dy;

    int x,y;

    // Make the ameisen pixel green
    sf::Color color = sf::Color::White;

    //for schleife --> create all ants 
    for(int i=0; i<NUMBER_OF_ANTS; i++)
    {
        ameisen[i].setRadius(5.f);
        ameisen[i].setFillColor(sf::Color::Green);
        ameisen[i].setPosition(MAX_X/2,MAX_Y/2);
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
        window.clear();
            
    // draw everything here...
        //for schleife --> move ants
        for(int i=0; i<NUMBER_OF_ANTS; i++)
        {
            //move shapes
            dir[i] = dir[i] + 20*rand()/RAND_MAX -10;
            dx = SPEED* cos(dir[i]*RAD_PER_DEG);
            dy = SPEED* sin(dir[i]*RAD_PER_DEG);
            ameisen[i].move(dx, dy);
            
            //boarder collision
            if ((ameisen[i].getPosition().x >= MAX_X-20) || (ameisen[i].getPosition().x <= 0+20))
            { 
                ameisen[i].move(-dx, -dy);
                dir[i]=180-dir[i];
            }
            if ((ameisen[i].getPosition().y >= MAX_Y-20) || (ameisen[i].getPosition().y <= 0+20))
            { 
                ameisen[i].move(-dx, -dy);
                dir[i]= -dir[i];
            }
            x = ameisen[i].getPosition().x;
            y = ameisen[i].getPosition().y;
            image.setPixel(x, y, color);
            image.setPixel(x-1, y, color);
            image.setPixel(x+1, y, color);
            image.setPixel(x, y-1, color);
            image.setPixel(x, y+1, color);
            image.setPixel(x+1, y+1, color);
            image.setPixel(x-1, y-1, color);
            image.setPixel(x+1, y-1, color);
            image.setPixel(x-1, y+1, color);
        }
        //update image and texture of ant trail
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        window.draw(sprite);

        for(int i=0;i < NUMBER_OF_ANTS; i++)
        {
        window.draw(ameisen[i]);
        }
        //Sleep(100);
        //cout << ameisen[i].getPosition().x << " " << ameisen[i].getPosition().y << " " << dir[i] << endl;
             
        // end the current frame
        window.display();
    }
          
    return 0;
}