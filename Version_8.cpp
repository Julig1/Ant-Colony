#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
using namespace std;

//Global variables
#define RAD_PER_DEG (float)(0.01745329238F)
#define NEW_DIR 10
#define SPEED (float)(3.0F)
#define MAX_X 1000
#define MAX_Y 1000
#define NUMBER_OF_ANTS 10
#define SENSERANGE 3
#define BACKGROUND_COLOR Black
#define AMEISEN_COLOR White
#define TOHOME_MARKER_COLOR g
#define PHEROMONE_STEP 25
#define TURNANGLE 10
#define ANGLE_OF_SENSORS 30
#define BRUSHSIZE 2 //(n+2) x (n+2) Feld gemalt //brushsize for drawing
#define BRUSHSIZE_DELETE 10 //(n+2) x (n+2) Feld gemalt //brushsize for deleting pixels
#define TRAILSIZE 1 //(n+2) x (n+2) Feld gemalt

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
    sf::CircleShape sensor_left[NUMBER_OF_ANTS];
    sf::CircleShape sensor_right[NUMBER_OF_ANTS];
    sf::CircleShape sensor_straight[NUMBER_OF_ANTS];
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
        ameisen[i].setRadius(2.f);
        ameisen[i].setFillColor(sf::Color::AMEISEN_COLOR);
        ameisen[i].setPosition(MAX_X/2,MAX_Y/2);
        dir[i]=i*30; //ants start in circle moving outwards
    }
    for(int i=0; i<NUMBER_OF_ANTS; i++)
    {
        float radius_sensor = 1;
        sensor_left[i].setRadius(radius_sensor);
        sensor_left[i].setFillColor(sf::Color::Red);
        sensor_right[i].setRadius(radius_sensor);
        sensor_right[i].setFillColor(sf::Color::Yellow);
        sensor_straight[i].setRadius(radius_sensor);
        sensor_straight[i].setFillColor(sf::Color::Blue);
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
            {
                window.close();
            }
        }

        // clear the window with black color
        window.clear();   

    // draw everything here...    
        //brush for TO home marker
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
        {
            x = sf::Mouse::getPosition(window).x;
            y = sf::Mouse::getPosition(window).y;
            for (int i=-BRUSHSIZE; i<(BRUSHSIZE+1); i++)
            {
                for(int j=-BRUSHSIZE; j<(BRUSHSIZE+1); j++)
                {
                    color.TOHOME_MARKER_COLOR = 200;
                    image.setPixel(x+i, y+j, color); 
                }
            }
        }
        //brush for deleting
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            x = sf::Mouse::getPosition(window).x;
            y = sf::Mouse::getPosition(window).y;
            for (int i=-BRUSHSIZE_DELETE; i<(BRUSHSIZE_DELETE+1); i++)
            {
                for(int j=-BRUSHSIZE_DELETE; j<(BRUSHSIZE_DELETE+1); j++)
                {
                    color.TOHOME_MARKER_COLOR = 0;
                    image.setPixel(x+i, y+j, color); 
                }
            }
        }
        //for schleife --> move ants
        for(int i=0; i<NUMBER_OF_ANTS; i++)
        {
            //position of ant i
            x = ameisen[i].getPosition().x;
            y = ameisen[i].getPosition().y;
            
            //sense straight ahead
            int sense_x = x + SENSERANGE * SPEED * cos(dir[i]*RAD_PER_DEG);
            int sense_y = y + SENSERANGE * SPEED * sin(dir[i]*RAD_PER_DEG);
            sense_color_straight = image.getPixel(sense_x,sense_y);

            //sense left ahead
            sense_x = x + SENSERANGE * SPEED * cos((dir[i]-ANGLE_OF_SENSORS)*RAD_PER_DEG);
            sense_y = y + SENSERANGE * SPEED * sin((dir[i]-ANGLE_OF_SENSORS)*RAD_PER_DEG);
            sense_color_left = image.getPixel(sense_x,sense_y);

            //sense right ahead
            sense_x = x + SENSERANGE * SPEED * cos((dir[i]+ANGLE_OF_SENSORS)*RAD_PER_DEG);
            sense_y = y + SENSERANGE * SPEED * sin((dir[i]+ANGLE_OF_SENSORS)*RAD_PER_DEG);
            sense_color_right = image.getPixel(sense_x,sense_y);
            
            //decide what to do when detecting pheromone
            /*if (sense_color_straight.TOHOME_MARKER_COLOR >= 100)
            {
                dir[i]=dir[i];
            }
            if (sense_color_left.TOHOME_MARKER_COLOR >= 100)
            {
                dir[i]=dir[i]-TURNANGLE;
            }
            if (sense_color_right.TOHOME_MARKER_COLOR >= 100)
            {
                dir[i]=dir[i]+TURNANGLE;
            }*/
            //compare pheromone density of all sensors and turn accordingly
            int R = sense_color_right.TOHOME_MARKER_COLOR;
            int S = sense_color_straight.TOHOME_MARKER_COLOR;
            int L = sense_color_left.TOHOME_MARKER_COLOR;

            if (S < R || S < L)    
            {   
                if(L > R)
                {
                    dir[i]=dir[i]-TURNANGLE; //-
                    //cout << "Ant turned left" << endl;
                }
                if(L < R)
                {
                    dir[i]=dir[i]+TURNANGLE; //+
                    //cout << "Ant turned right" << endl;
                }
            }
            //move shapes after checking for pheromons
            dir[i] = dir[i] + NEW_DIR*rand()/RAND_MAX -(NEW_DIR/2);
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
            sensor_left[i].setPosition( x + SENSERANGE * SPEED * cos((dir[i]-ANGLE_OF_SENSORS)*RAD_PER_DEG), \
                                        y + SENSERANGE * SPEED * sin((dir[i]-ANGLE_OF_SENSORS)*RAD_PER_DEG));
            sensor_right[i].setPosition(x + SENSERANGE * SPEED * cos((dir[i]+ANGLE_OF_SENSORS)*RAD_PER_DEG), \
                                        y + SENSERANGE * SPEED * sin((dir[i]+ANGLE_OF_SENSORS)*RAD_PER_DEG) );
            sensor_straight[i].setPosition( x + SENSERANGE * SPEED * cos(dir[i]*RAD_PER_DEG), \
                                            y + SENSERANGE * SPEED * sin(dir[i]*RAD_PER_DEG) );
            //cout << " " << dir[i] << " " << x-(sensor_left[i].getPosition().x) << " " << y-(sensor_left[i].getPosition().y) << endl;
            
            //check pixel color density and paint  
            for (int i=-TRAILSIZE; i<(TRAILSIZE+1); i++)
            {
                for(int j=-TRAILSIZE; j<(TRAILSIZE+1); j++)
                {
                    color = image.getPixel(x+i,y+j);
                    if(color.TOHOME_MARKER_COLOR < (250 - PHEROMONE_STEP))
                    {
                        color.TOHOME_MARKER_COLOR += PHEROMONE_STEP;
                        image.setPixel(x+i, y+j, color);
                    } 
                }
            }
        }
        //update image and texture of ant trail
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        window.draw(sprite);

        for(int i=0;i < NUMBER_OF_ANTS; i++)
        {
        window.draw(ameisen[i]);
        window.draw(sensor_left[i]);
        window.draw(sensor_right[i]);
        window.draw(sensor_straight[i]);
        }
        //leep(100);
             
        // end the current frame
        window.display();
    }
          
    return 0;
}