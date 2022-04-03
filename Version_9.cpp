#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include <math.h>
using namespace std;

//Global variables
#define RAD_PER_DEG (float)(0.01745329238F) //180/pi
#define NEW_DIR 10 //cone infront of ant in which ant chooses random new direction
#define SPEED (float)(3.0F) //steplength of ant
#define MAX_X 1000
#define MAX_Y 1000
#define NUMBER_OF_ANTS 100
#define SENSERANGE 3 //distance sensors from ant
#define BACKGROUND_COLOR Black
#define AMEISEN_COLOR White
#define TOHOME_MARKER_COLOR b
#define TOFOOD_MARKER_COLOR r
#define FOOD_COLOR g
#define GOING_HOME 0
#define SEEKING_FOOD 1
#define PHEROMONE_STEP 100 //discrete steps of color. higher value is lighter color and more intense pheromone
#define TURNANGLE 10 //angle of turn when ant senses pheromone
#define ANGLE_OF_SENSORS 30 //angle in which the sensors are apart of the ant
#define BRUSHSIZE 2 //(n+2) x (n+2) Feld gemalt //brushsize for drawing
#define BRUSHSIZE_DELETE 10 //(n+2) x (n+2) Feld gemalt //brushsize for deleting pixels
#define TRAILSIZE 1 //(n+2) x (n+2) Feld gemalt
#define COLONY_SIZE 30
#define DECAY_TIME 300
#define SPLIT_YCOMP 500
#define PHEROMONE_DECAY 1

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(MAX_X, MAX_Y), "Ant Simultaion Version 0.1");
    
    //create image
    sf::Image image;
    image.create(MAX_X, MAX_Y, sf::Color::BACKGROUND_COLOR);
    //clear image used for deleting all pixels
    sf::Image image_clear;
    image_clear.create(MAX_X, MAX_Y, sf::Color::BACKGROUND_COLOR);

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
    int status_ant[NUMBER_OF_ANTS];
    float dx;
    float dy;
    int x,y;
    int decay[SPLIT_YCOMP];
    int decay_loop_cnt = 0;

    // Make the ameisen pixel specific color
    sf::Color color(0,0,0);

    //Keys
    cout << "F - Food brush" << endl << "D - Delete brush " << endl << "H - To home Marker brush " << endl << "T - To food Marker brush" << endl << "N - Delete all pixels " << endl << "R - Teleport Ants to  random Location (not implemented) " << endl;
    //for schleife --> create all ants 
    for(int i=0; i<NUMBER_OF_ANTS; i++)
    {
        ameisen[i].setRadius(2.f);
        ameisen[i].setFillColor(sf::Color::AMEISEN_COLOR);
        ameisen[i].setPosition(MAX_X/2,MAX_Y/2);
        dir[i]=i*30; //ants start in circle moving outwards
        status_ant[i] = SEEKING_FOOD;
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
            //delete every pixel
            if (event.type == sf::Event::EventType::KeyPressed && event.key.code == sf::Keyboard::N)
            {
                //image_clear.saveToFile("C:\\SFMLProjekt\\black.png");
                //image.loadFromFile("C:\\SFMLProjekt\\black.png");
                image.create(MAX_X, MAX_Y, sf::Color::BACKGROUND_COLOR);
                
            }
            //teleport every ant to random location
            /*if (event.type == sf::Event::EventType::KeyPressed && event.key.code == sf::Keyboard::R)
            {
                window.setKeyRepeatEnabled(false);
                cout << "R";
                for(int i=0; i<NUMBER_OF_ANTS; i++)
                {
                    ameisen[i].setPosition(MAX_X*rand()/RAND_MAX,MAX_Y*rand()/RAND_MAX);
                }                 
            }*/
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
        //brush for TO Food marker
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
        {
            x = sf::Mouse::getPosition(window).x;
            y = sf::Mouse::getPosition(window).y;
            for (int i=-BRUSHSIZE; i<(BRUSHSIZE+1); i++)
            {
                for(int j=-BRUSHSIZE; j<(BRUSHSIZE+1); j++)
                {
                    color.TOFOOD_MARKER_COLOR = 200;
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
                    color.TOFOOD_MARKER_COLOR = 0;
                    image.setPixel(x+i, y+j, color); 
                }
            }
        }
        //brush for Food
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
        {
            x = sf::Mouse::getPosition(window).x;
            y = sf::Mouse::getPosition(window).y;
            for (int i=-BRUSHSIZE_DELETE; i<(BRUSHSIZE_DELETE+1); i++)
            {
                for(int j=-BRUSHSIZE_DELETE; j<(BRUSHSIZE_DELETE+1); j++)
                {
                    color.FOOD_COLOR = 250;
                    color.FOOD_COLOR = 250;
                    image.setPixel(x+i, y+j, color); 
                }
            }
        }
        
        //for schleife --> move ants and paint markers
        for(int i=0; i<NUMBER_OF_ANTS; i++)
        {
            //position of ant i
            x = ameisen[i].getPosition().x;
            y = ameisen[i].getPosition().y;
            
            //sense straight ahead
            int sense_x_s = x + SENSERANGE * SPEED * cos(dir[i]*RAD_PER_DEG);
            int sense_y_s = y + SENSERANGE * SPEED * sin(dir[i]*RAD_PER_DEG);
            sense_color_straight = image.getPixel(sense_x_s,sense_y_s);

            //sense left ahead
            int sense_x_l = x + SENSERANGE * SPEED * cos((dir[i]-ANGLE_OF_SENSORS)*RAD_PER_DEG);
            int sense_y_l = y + SENSERANGE * SPEED * sin((dir[i]-ANGLE_OF_SENSORS)*RAD_PER_DEG);
            sense_color_left = image.getPixel(sense_x_l,sense_y_l);

            //sense right ahead
            int sense_x_r = x + SENSERANGE * SPEED * cos((dir[i]+ANGLE_OF_SENSORS)*RAD_PER_DEG);
            int sense_y_r = y + SENSERANGE * SPEED * sin((dir[i]+ANGLE_OF_SENSORS)*RAD_PER_DEG);
            sense_color_right = image.getPixel(sense_x_r,sense_y_r);
            
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
            //ToHome marker detection
            if (status_ant[i] == SEEKING_FOOD)
            {
                int R = sense_color_right.TOFOOD_MARKER_COLOR;
                int S = sense_color_straight.TOFOOD_MARKER_COLOR;
                int L = sense_color_left.TOFOOD_MARKER_COLOR;

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
                if (sense_color_left.FOOD_COLOR > 0 || sense_color_right.FOOD_COLOR > 0 || sense_color_straight.FOOD_COLOR > 0)
                {
                    status_ant[i] = GOING_HOME;
                    dir[i] = dir[i]+180;
                    if (sense_color_left.FOOD_COLOR > 0)
                    {
                        color.b = 0;
                        image.setPixel(sense_x_l,sense_y_l,color);
                    }
                } 
            }
            else if (status_ant[i] == GOING_HOME)
            {
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
                x = ameisen[i].getPosition().x - (MAX_X/2);
                y = ameisen[i].getPosition().y - (MAX_Y/2);
                float l = sqrt(x*x+y*y);
                if (l <= COLONY_SIZE)
                {
                    status_ant[i] = SEEKING_FOOD;
                }
            }
            //move shapes after checking for pheromons
            dir[i] = dir[i] + round(((float)NEW_DIR*(float)rand()/(float)RAND_MAX)) -(NEW_DIR/2);
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
            
            //check pixel color density and paint (Tohome marker)  
            /*for (int i=-TRAILSIZE; i<(TRAILSIZE+1); i++)
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
            }*/
            //check for pixel color density and paint marker
            if (status_ant[i] == 1)
            {   
                for (int k=-TRAILSIZE; k<(TRAILSIZE+1); k++)
                {
                    for(int j=-TRAILSIZE; j<(TRAILSIZE+1); j++)
                    {
                        color = image.getPixel(x+k,y+j);
                        if(color.TOHOME_MARKER_COLOR < (250 - PHEROMONE_STEP))
                        {
                            color.TOHOME_MARKER_COLOR += PHEROMONE_STEP;
                            image.setPixel(x+k, y+j, color);
                        } 
                    }
                }
            }
            else if(status_ant[i] == 0)
            {
                for (int k=-TRAILSIZE; k<(TRAILSIZE+1); k++)
                {
                    for(int j=-TRAILSIZE; j<(TRAILSIZE+1); j++)
                    {
                        color = image.getPixel(x+k,y+j);
                        if(color.TOFOOD_MARKER_COLOR < (250 - PHEROMONE_STEP))
                        {
                            color.TOFOOD_MARKER_COLOR += PHEROMONE_STEP;
                            image.setPixel(x+k, y+j, color);
                        } 
                    }
                }
            }
        }
        /*decay_1 = decay_1 + 1;
        decay_2 = decay_2 + 1;
        decay_3 = decay_3 + 1;
        decay_4 = decay_4 + 1;
        decay_5 = decay_5 + 1;
        decay_6 = decay_6 + 1;
        decay_7 = decay_7 + 1;
        decay_8 = decay_8 + 1;
        decay_9 = decay_9 + 1;
        decay_0 = decay_0 + 1;
        cout << decay_0 << endl;*/
        //decay process
        /*int decay_step = MAX_Y/SPLIT_YCOMP;     // Number of lines per decay step
        for(int i=0;i<SPLIT_YCOMP;i++)
        {
            decay[i] = decay[i] + 1;
            if(decay[i]>=500)
            {
                decay[i]=0;
            }
            cout << decay[i] << " " << i << endl;
            if (decay[i] == decay_step*(i+1)) //decay[0]== 50*1 //decay[1]==50*2 //decay[2]==50*3
            {
                //cout << decay[i] << " " << i << endl;
                //delete 1000 x 50 field
                for (int k=0; k<(MAX_X); k++)
                {
                    int start_y = decay_step*i;
                    int end_y = decay_step*(i+1);
                    for(int j= start_y; j<end_y; j++)
                    {
                        color = image.getPixel(k, j);
                        if(color.TOHOME_MARKER_COLOR >PHEROMONE_STEP)
                        {
                            color.TOHOME_MARKER_COLOR -= PHEROMONE_STEP;
                        }
                        else
                        {
                            color.TOHOME_MARKER_COLOR =0;
                        }
                        if(color.TOFOOD_MARKER_COLOR >0)
                        {
                            color.TOFOOD_MARKER_COLOR -= PHEROMONE_STEP;;
                        }
                        else
                        {
                            color.TOFOOD_MARKER_COLOR =0;
                        }
                        image.setPixel(k, j, color);
                    }
                }
            }
        }*/
        for (int k=0; k<MAX_X; k++)
        {
            for(int j=0; j<MAX_Y; j++)
            {
                color = image.getPixel(k, j);
                if(color.TOHOME_MARKER_COLOR >PHEROMONE_DECAY)
                {
                    color.TOHOME_MARKER_COLOR -= PHEROMONE_DECAY;
                }
                else
                {
                    color.TOHOME_MARKER_COLOR =0;
                }
                if(color.TOFOOD_MARKER_COLOR >0)
                {
                    color.TOFOOD_MARKER_COLOR -= PHEROMONE_DECAY;;
                }
                else
                {
                    color.TOFOOD_MARKER_COLOR =0;
                }
                image.setPixel(k, j, color);
            }
        }
        decay_loop_cnt = decay_loop_cnt + 1;
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
//end of while(window is open)      
    return 0;
}