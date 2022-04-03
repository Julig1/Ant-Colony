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
#define SENSERANGE 2
#define BACKGROUND_COLOR Black
#define AMEISEN_COLOR White
#define TOHOME_MARKER_COLOR g
#define PHEROMONE_STEP 25

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(MAX_X, MAX_Y), "Ant Simultaion Version 0.1");
    
    //create image
    sf::Image image;
    image.create(MAX_X, MAX_Y, sf::Color::BACKGROUND_COLOR);

    // create texture
    sf::Texture texture;

    //create sprite
    sf::Sprite sprite;

    //create sensing areas
    sf::Color sense_color_straight;
    sf::Color sense_color_left;
    sf::Color sense_color_right;

    //create shapes
    sf::CircleShape ameisen[NUMBER_OF_ANTS];
    float dir[NUMBER_OF_ANTS];
    float dx;
    float dy;

    int x,y;

    // Make the ameisen pixel specific color
    sf::Color color(0,0,0);
    color.TOHOME_MARKER_COLOR == 0;

    //for schleife --> create all ants 
    for(int i=0; i<NUMBER_OF_ANTS; i++)
    {
        ameisen[i].setRadius(5.f);
        ameisen[i].setFillColor(sf::Color::AMEISEN_COLOR);
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
            //position of ant i
            x = ameisen[i].getPosition().x;
            y = ameisen[i].getPosition().y;
            
            //sense straight ahead
            int sense_x = x + SENSERANGE * SPEED * cos(dir[i]);
            int sense_y = y + SENSERANGE * SPEED * sin(dir[i]);
            sense_color_straight = image.getPixel(sense_x,sense_y);

            float winkel = 30;
            //sense left ahead
            sense_x = x + SENSERANGE * SPEED * cos(dir[i]-winkel);
            sense_y = y + SENSERANGE * SPEED * sin(dir[i]-winkel);
            sense_color_left = image.getPixel(sense_x,sense_y);

            //sense right ahead
            sense_x = x + SENSERANGE * SPEED * cos(dir[i]+winkel);
            sense_y = y + SENSERANGE * SPEED * sin(dir[i]+winkel);
            sense_color_right = image.getPixel(sense_x,sense_y);
            
            //decide what to do when detecting pheromone
            if (sense_color_straight.TOHOME_MARKER_COLOR >= 100)
            {
                dir[i]=dir[i];
            }
            if (sense_color_left.TOHOME_MARKER_COLOR >= 100)
            {
                dir[i]=dir[i]-winkel;
            }
            if (sense_color_right.TOHOME_MARKER_COLOR >= 100)
            {
                dir[i]=dir[i]+winkel;
            }

            //move shapes after checking for pheromons
            dir[i] = dir[i] + 20*rand()/RAND_MAX -10;
            dx = SPEED* cos(dir[i]*RAD_PER_DEG);
            dy = SPEED* sin(dir[i]*RAD_PER_DEG);
            ameisen[i].move(dx, dy);

            //border collision
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
        //draw pheromons 3x3 field
            //new position of ant i
            x = ameisen[i].getPosition().x;
            y = ameisen[i].getPosition().y;
            
            //check pixel color density and paint  
            for (int i=-1; i<2; i++)
            {
                for(int j=-1; j<2; j++)
                {
                    color = image.getPixel(x+i,y+j);
                    if(color.TOHOME_MARKER_COLOR < (250 - PHEROMONE_STEP))
                    {
                        color.TOHOME_MARKER_COLOR += PHEROMONE_STEP;
                        image.setPixel(x+i, y+j, color);
                    } 
                }
            }
            /*image.setPixel(x-1, y, color);
            image.setPixel(x+1, y, color);
            image.setPixel(x, y-1, color);
            image.setPixel(x, y+1, color);
            image.setPixel(x+1, y+1, color);
            image.setPixel(x-1, y-1, color);
            image.setPixel(x+1, y-1, color);
            image.setPixel(x-1, y+1, color);*/
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
             
        // end the current frame
        window.display();
    }
          
    return 0;
}