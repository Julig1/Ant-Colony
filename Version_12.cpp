//TO_DO
/*
    - pheromon tracking wie foodtracking, bloß dass es die entferntesten pheromone ansteuert, die in der bewegungsrichtung liegen
    - randomfood spawner
    - essen im vorhinein zeichen und als image beim start importieren
        --> vorgerfertigte Formen als essen, sowie text
    
*/


#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include <math.h>
#include <bits/stdc++.h>
using namespace std;

//Global variables
#define RAD_PER_DEG (float)(0.01745329238F) //180/pi
#define MAX_X (1920 + FOOD_SCANNER_FIELD)
#define MAX_Y (1080 + FOOD_SCANNER_FIELD)

//Colony
#define COLONY_SIZE 30
#define NUMBER_OF_ANTS 500

//colors
#define FOOD_COLOR g
#define BACKGROUND_COLOR Black
#define AMEISEN_COLOR White
#define TOHOME_MARKER_COLOR b
#define TOFOOD_MARKER_COLOR r

//antstatus and antmovement
#define NEW_DIR 10 //cone infront of ant in which ant chooses random new direction
#define SPEED (float)(3.0F) //steplength of ant
#define SENSERANGE 4 //distance sensors from ant
#define GOING_HOME 0
#define SEEKING_FOOD 1
#define TURNANGLE 10 //angle of turn when ant senses pheromone
#define ANGLE_OF_SENSORS 30 //angle in which the sensors are apart of the ant
#define SENSOR_SIZE 1 //(n+2) x (n+2) Feld
#define FOOD_SCANNER_FIELD 50

//pheromones and pheromone decay
#define PHEROMONE_STEP 100 //discret steps of color. higher value is lighter color and more intense pheromone
#define MINIMUM_PHEROMONE 1
#define TRAILSIZE 1 //(n+2) x (n+2) Feld gemalt
#define PHEROMONE_DECAY 1 //every pixel's color value is decremented by this amount
#define MAX_PHEROMONE_INTENSITY 100 //after 100 steps ants will no longer spwan pheromones 
#define PHEROMONE_DECAY_TIMER 5

//brushes
#define BRUSHSIZE 2 //(n+2) x (n+2) Feld gemalt //brushsize for drawing
#define BRUSHSIZE_DELETE 10 //(n+2) x (n+2) Feld gemalt //brushsize for deleting pixels

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
    sf::Color sense_color_f_left;
    sf::Color sense_color_f_right;
    sf::Color food_scanner_field_color;
    sf::Color ameisen_color;
    sf::Color transparent(255,255,255,0);

    //create shapes
    sf::CircleShape ameisen[NUMBER_OF_ANTS];
    sf::CircleShape sensor_left[NUMBER_OF_ANTS];
    sf::CircleShape sensor_right[NUMBER_OF_ANTS];
    sf::CircleShape sensor_straight[NUMBER_OF_ANTS];
    sf::CircleShape sensor_far_left[NUMBER_OF_ANTS];
    sf::CircleShape sensor_far_right[NUMBER_OF_ANTS];
    sf::CircleShape colony;
    sf::Color colony_color(0,255,255);

    //Boarder
    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(MAX_X-2*FOOD_SCANNER_FIELD,MAX_Y-2*FOOD_SCANNER_FIELD));
    rectangle.setOutlineColor(sf::Color::Red);
    rectangle.setOutlineThickness(5);
    rectangle.setPosition(FOOD_SCANNER_FIELD,FOOD_SCANNER_FIELD);
    rectangle.setFillColor(transparent);

    //variables
    float dir[NUMBER_OF_ANTS];
    float previous_dir[NUMBER_OF_ANTS];
    float new_dir[NUMBER_OF_ANTS];
    float pheromone_intensity[NUMBER_OF_ANTS];
    int status_ant[NUMBER_OF_ANTS];
    float dx;
    float dy;
    int x,y;
    int decay_loop_cnt = 0;
    float food_scanner_field[FOOD_SCANNER_FIELD*2+1][FOOD_SCANNER_FIELD*2+1];
    int food_in_colony = 0;

    // Make the ameisen pixel specific color
    sf::Color color(0,0,0);

    //text to draw
    sf::Font Arial;
    Arial.loadFromFile("arial.ttf");

    //Keys
    cout << "F - Food brush" << endl << "D - Delete brush " << endl << "H - To home Marker brush " << endl << "T - To food Marker brush" << endl << "N - Delete all pixels " << endl << "R - Teleport Ants to  random Location (not implemented) " << endl;
    //for schleife --> create all ants and 1 circle for colony
    colony.setFillColor(colony_color);
    colony.setRadius(COLONY_SIZE);
    colony.setPosition(MAX_X/2,MAX_Y/2);
    colony.setOrigin(COLONY_SIZE,COLONY_SIZE); 
    for(int i=0; i<NUMBER_OF_ANTS; i++)
    {
        ameisen[i].setRadius(2.f);
        ameisen[i].setFillColor(sf::Color::AMEISEN_COLOR);
        ameisen[i].setPosition(MAX_X/2,MAX_Y/2);
        dir[i]=i*30; //ants start in circle moving outwards
        status_ant[i] = SEEKING_FOOD;
        pheromone_intensity[i] = MAX_PHEROMONE_INTENSITY;
    }
    for(int i=0; i<NUMBER_OF_ANTS; i++)
    {
        float radius_sensor = 1;
        sensor_left[i].setRadius(radius_sensor);
        sensor_left[i].setFillColor(sf::Color::Blue);
        sensor_right[i].setRadius(radius_sensor);
        sensor_right[i].setFillColor(sf::Color::Blue);
        sensor_straight[i].setRadius(radius_sensor);
        sensor_straight[i].setFillColor(sf::Color::Yellow);
        sensor_far_left[i].setRadius(radius_sensor);
        sensor_far_left[i].setFillColor(sf::Color::Red);
        sensor_far_right[i].setRadius(radius_sensor);
        sensor_far_right[i].setFillColor(sf::Color::Red);
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
            if (pheromone_intensity[i] >= MINIMUM_PHEROMONE)
            {
                pheromone_intensity[i] = pheromone_intensity[i] - 0.1; //can be float
            }
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

            //sense far left ahead
            int sense_x_fl = x + SENSERANGE * SPEED * cos((dir[i]-2*ANGLE_OF_SENSORS)*RAD_PER_DEG);
            int sense_y_fl = y + SENSERANGE * SPEED * sin((dir[i]-2*ANGLE_OF_SENSORS)*RAD_PER_DEG);
            sense_color_f_left = image.getPixel(sense_x_fl,sense_y_fl);

            //sense far right ahead
            int sense_x_fr = x + SENSERANGE * SPEED * cos((dir[i]+2*ANGLE_OF_SENSORS)*RAD_PER_DEG);
            int sense_y_fr = y + SENSERANGE * SPEED * sin((dir[i]+2*ANGLE_OF_SENSORS)*RAD_PER_DEG);
            sense_color_f_right = image.getPixel(sense_x_fr,sense_y_fr);

            //compare pheromone density of all sensors and turn accordingly
            if (status_ant[i] == SEEKING_FOOD)
            {
                int R = sense_color_right.TOFOOD_MARKER_COLOR;
                int S = sense_color_straight.TOFOOD_MARKER_COLOR;
                int L = sense_color_left.TOFOOD_MARKER_COLOR;
                int fL = sense_color_f_left.TOFOOD_MARKER_COLOR;
                int fR = sense_color_f_right.TOFOOD_MARKER_COLOR;

                if (S < R || S < L || S < fR || S < fL)    
                {   
                    if(L > R || fL > fR || fL > R || L > fR)
                    {
                        if(fL > L)
                        {
                            dir[i]=dir[i]-2*TURNANGLE; //-
                            //cout << "Ant turned left" << endl;
                        }
                        if(fL < L)
                        {
                            dir[i]=dir[i]-TURNANGLE; //+
                            //cout << "Ant turned right" << endl;
                        }
                    }
                    if(L < R || fL < fR || fL < R || L < fR)
                    {
                        if(R < fR)
                        {
                            dir[i]=dir[i]+2*TURNANGLE; //+
                            //cout << "Ant turned right" << endl;
                        }
                        if(R > fR)
                        {
                            dir[i]=dir[i]+TURNANGLE; //+
                            //cout << "Ant turned right" << endl;
                        }
                    }
                }
                float minimum = MAX_X;
                previous_dir[i] = dir[i];
                for (int j=-FOOD_SCANNER_FIELD; j<(FOOD_SCANNER_FIELD+1); j++)
                {
                    for(int k=-FOOD_SCANNER_FIELD; k<(FOOD_SCANNER_FIELD+1); k++)
                    {
                        /*
                        4.24(-3,-3) 3.61(-2,-3) 3.16(-1,-3) 3(0,-3) 3.16(1,-3) 3.61(2,-3) 4.24(3,-3) 
                        3.61(-3,-2) 2.83(-2,-2) 2.24(-1,-2) 2(0,-2) 2.24(1,-2) 2.83(2,-2) 3.61(3,-2) 
                        3.16(-3,-1) 2.24(-2,-1) 1.41(-1,-1) 1(0,-1) 1.41(1,-1) 2.24(2,-1) 3.16(3,-1) 
                        3(-3,0)     2(-2,0)     1(-1,0)     0(0,0)  1(1,0)     2(2,0)     3(3,0) 
                        3.16(-3,1)  2.24(-2,1)  1.41(-1,1)  1(0,1)  1.41(1,1)  2.24(2,1)  3.16(3,1) 
                        3.61(-3,2)  2.83(-2,2)  2.24(-1,2)  2(0,2)  2.24(1,2)  2.83(2,2)  3.61(3,2) 
                        4.24(-3,3)  3.61(-2,3)  3.16(-1,3)  3(0,3)  3.16(1,3)  3.61(2,3)  4.24(3,3)
                        */
                        food_scanner_field_color = image.getPixel(x+k,y+j);
                        int F = food_scanner_field_color.FOOD_COLOR;
                        //cout << F << endl;
                        float distance = sqrt(k*k+j*j);
                        //cout << setprecision(3) << distance << "(" << k << "," << j << ")" << " ";
                        if(F > 0)
                        {
                            if(distance < minimum)
                            {
                                minimum = distance;
                                if(k<0||k>0)
                                {
                                    float h = (float)j;
                                    float j_divide_k = h/k;
                                    //cout << minimum << ", new_dir="<< round(atan(j_divide_k)/RAD_PER_DEG) << ", j/k=" << j_divide_k << ", k=" << k << ", j=" << j << ", x=" << x << ", y=" << y << endl;
                                    new_dir[i] = atan(j_divide_k)/RAD_PER_DEG;
                                    if(j<TRAILSIZE && k<TRAILSIZE)
                                    {
                                        new_dir[i]= new_dir[i] + 180;
                                    }
                                    else if(j>TRAILSIZE && k>TRAILSIZE)
                                    {
                                        new_dir[i]= new_dir[i];
                                    }
                                    else if(j<TRAILSIZE && k>TRAILSIZE)
                                    {
                                        new_dir[i]= new_dir[i];
                                    }
                                    else if(j>TRAILSIZE && k<TRAILSIZE)
                                    {
                                        new_dir[i]= new_dir[i] + 180;
                                    }
                                    else if(j==0 && k>TRAILSIZE)
                                    {
                                        new_dir[i] = 0;
                                    }
                                    else if(j==0 && k<TRAILSIZE)
                                    {
                                        new_dir[i] = 180;
                                        //cout << "k kleiner 0" << endl;
                                    }
                                    else
                                    {
                                        new_dir[i]=previous_dir[i];
                                    }
                                }
                                else if(k==0)
                                {
                                    if(j < 0)
                                    {
                                        new_dir[i] = 270;
                                    }
                                    else if(j > 0)
                                    {
                                        new_dir[i] = 90;
                                    }                                    
                                    else if(j==0)
                                    {
                                        new_dir[i] = previous_dir[i];
                                    }
                                    //cout << minimum << ", k=" << k << ", j=" << j << ", x=" << x << ", y=" << y << endl;
                                }
                                //cout << dir[i] << ", ";
                                //cout << new_dir[i] << endl;
                                dir[i] = new_dir[i];
                                if(dir[i] < 0)
                                {
                                    dir[i] = 360 + dir[i];
                                }
                                //no angles above 360 allowed
                                else if(dir[i]>=360)
                                {
                                    dir[i] = dir[i] - 360;
                                }
                                //cout << dir[i] << ", ";
                                //cout << new_dir[i] << endl;
                            }
                            //cout << setprecision(3) << distance << "(" << k << "," << j << ")" << " ";
                        }
                        /*else if (F <= 0)
                        {
                            cout << "0 ";
                        }*/
                    }
                    //cout << endl;
                }
                //cout << endl;
                bool essen_gefunden = FALSE;
                int x = ameisen[i].getPosition().x;
                int y = ameisen[i].getPosition().y;
                for(int k = -TRAILSIZE; k < (TRAILSIZE+1); k++)
                {
                    for(int j = -TRAILSIZE; j < (TRAILSIZE+1); j++)
                    {    
                        ameisen_color = image.getPixel(x+k,y+j);
                        int FC = ameisen_color.FOOD_COLOR;
                        if(FC > 0)
                        {
                            if(essen_gefunden == FALSE)
                            {
                                dir[i] = dir[i]+180;
                                //cout << "turn 180 ant eat" << endl;
                            }
                            essen_gefunden = TRUE;
                            status_ant[i] = GOING_HOME;
                            pheromone_intensity[i] = MAX_PHEROMONE_INTENSITY;
                            ameisen_color.FOOD_COLOR = 0;
                            image.setPixel(x+k,y+j,ameisen_color);
                            //cout << "essen gefunden" << endl;
                            
                        }
                    }
                }
            }
            //ToHome marker detection
            else if (status_ant[i] == GOING_HOME)
            {
                int R = sense_color_right.TOHOME_MARKER_COLOR;
                int S = sense_color_straight.TOHOME_MARKER_COLOR;
                int L = sense_color_left.TOHOME_MARKER_COLOR;
                int fL = sense_color_f_left.TOHOME_MARKER_COLOR;
                int fR = sense_color_f_right.TOHOME_MARKER_COLOR;

                if (S < R || S < L || S < fR || S < fL)    
                {   
                    if(L > R || fL > fR || fL > R || L > fR)
                    {
                        if(fL > L)
                        {
                            dir[i]=dir[i]-2*TURNANGLE; //-
                            //cout << "Ant turned left" << endl;
                        }
                        if(fL < L)
                        {
                            dir[i]=dir[i]-TURNANGLE; //+
                            //cout << "Ant turned right" << endl;
                        }
                    }
                    if(L < R || fL < fR || fL < R || L < fR)
                    {
                        if(R < fR)
                        {
                            dir[i]=dir[i]+2*TURNANGLE; //+
                            //cout << "Ant turned right" << endl;
                        }
                        if(R > fR)
                        {
                            dir[i]=dir[i]+TURNANGLE; //+
                            //cout << "Ant turned right" << endl;
                        }
                    }
                }
            //distance to center
                x = ameisen[i].getPosition().x - (MAX_X/2);
                y = ameisen[i].getPosition().y - (MAX_Y/2);
                float l = sqrt(x*x+y*y);
            //switch ant to seeking food again
                if (l <= COLONY_SIZE)
                {
                    status_ant[i] = SEEKING_FOOD;
                    dir[i] = dir[i]+180;
                    pheromone_intensity[i] = MAX_PHEROMONE_INTENSITY;
                    food_in_colony = food_in_colony + 1;
                }
            }
            //move shapes after checking for pheromons
            dir[i] = dir[i] + round(((float)NEW_DIR*(float)rand()/(float)RAND_MAX)) -(NEW_DIR/2);
            dx = SPEED* cos(dir[i]*RAD_PER_DEG);
            dy = SPEED* sin(dir[i]*RAD_PER_DEG);
            ameisen[i].move(dx, dy);
            
            //border collision
            if ((ameisen[i].getPosition().x >= MAX_X-FOOD_SCANNER_FIELD) || (ameisen[i].getPosition().x <= 0+FOOD_SCANNER_FIELD))
            { 
                ameisen[i].move(-dx, -dy);
                dir[i]=180-dir[i];
            }
            if ((ameisen[i].getPosition().y >= MAX_Y-FOOD_SCANNER_FIELD) || (ameisen[i].getPosition().y <= 0+FOOD_SCANNER_FIELD))
            { 
                ameisen[i].move(-dx, -dy);
                dir[i]= -dir[i];
            }
            
        //draw pheromons 3x3 field
        //new position of ant i
            //set positions of sensors relativ to ant i
            x = ameisen[i].getPosition().x;
            y = ameisen[i].getPosition().y;
            sensor_left[i].setPosition( x + SENSERANGE * SPEED * cos((dir[i]-ANGLE_OF_SENSORS)*RAD_PER_DEG), \
                                        y + SENSERANGE * SPEED * sin((dir[i]-ANGLE_OF_SENSORS)*RAD_PER_DEG));
            sensor_right[i].setPosition(x + SENSERANGE * SPEED * cos((dir[i]+ANGLE_OF_SENSORS)*RAD_PER_DEG), \
                                        y + SENSERANGE * SPEED * sin((dir[i]+ANGLE_OF_SENSORS)*RAD_PER_DEG) );
            sensor_far_left[i].setPosition( x + SENSERANGE * SPEED * cos((dir[i]-2*ANGLE_OF_SENSORS)*RAD_PER_DEG), \
                                        y + SENSERANGE * SPEED * sin((dir[i]-2*ANGLE_OF_SENSORS)*RAD_PER_DEG));
            sensor_far_right[i].setPosition(x + SENSERANGE * SPEED * cos((dir[i]+2*ANGLE_OF_SENSORS)*RAD_PER_DEG), \
                                        y + SENSERANGE * SPEED * sin((dir[i]+2*ANGLE_OF_SENSORS)*RAD_PER_DEG) );
            sensor_straight[i].setPosition( x + SENSERANGE * SPEED * cos(dir[i]*RAD_PER_DEG), \
                                            y + SENSERANGE * SPEED * sin(dir[i]*RAD_PER_DEG) );
            //cout << " " << dir[i] << " " << x-(sensor_left[i].getPosition().x) << " " << y-(sensor_left[i].getPosition().y) << endl;
            //check for pixel color density and paint marker
            if (status_ant[i] == SEEKING_FOOD)
            {   
                for (int k=-TRAILSIZE; k<(TRAILSIZE+1); k++)
                {
                    for(int j=-TRAILSIZE; j<(TRAILSIZE+1); j++)
                    {
                        color = image.getPixel(x+k,y+j);
                        int delta_color = round(PHEROMONE_STEP*pheromone_intensity[i]/100);
                        if(color.TOHOME_MARKER_COLOR < (250 - delta_color))
                        {
                            color.TOHOME_MARKER_COLOR += delta_color;
                        }
                        else
                        {
                            color.TOHOME_MARKER_COLOR = 250;
                        } 
                        image.setPixel(x+k, y+j, color);
                    }
                }
            }
            else if(status_ant[i] == GOING_HOME)
            {
                for (int k=-TRAILSIZE; k<(TRAILSIZE+1); k++)
                {
                    for(int j=-TRAILSIZE; j<(TRAILSIZE+1); j++)
                    {
                        color = image.getPixel(x+k,y+j);
                        int delta_color = round(PHEROMONE_STEP*pheromone_intensity[i]/100);
                        if(color.TOFOOD_MARKER_COLOR < (250 - delta_color))
                        {
                            color.TOFOOD_MARKER_COLOR += delta_color;
                        }
                        else
                        {
                            color.TOFOOD_MARKER_COLOR = 250;
                        } 
                        image.setPixel(x+k, y+j, color);
                    }
                }
            }
        }
        //decay process
        decay_loop_cnt = decay_loop_cnt + 1;
        if(decay_loop_cnt>=PHEROMONE_DECAY_TIMER)
        {    
            decay_loop_cnt = 0;
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
        }
        
        sf::Text food_text(to_string(food_in_colony), Arial);
        food_text.setCharacterSize(30);
        food_text.setStyle(sf::Text::Bold);
        food_text.setFillColor(sf::Color::White);
        food_text.setPosition(MAX_X/2, FOOD_SCANNER_FIELD);

        //update image and texture of ant trail
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        window.draw(sprite);
        window.draw(colony);
        window.draw(food_text);
        window.draw(rectangle);

        for(int i=0;i < NUMBER_OF_ANTS; i++)
        {
        window.draw(ameisen[i]);
        window.draw(sensor_left[i]);
        window.draw(sensor_right[i]);
        window.draw(sensor_straight[i]);
        window.draw(sensor_far_right[i]);
        window.draw(sensor_far_left[i]);
        }
        //Sleep(0.01);
             
        // end the current frame
        window.display();
    }
//end of while(window is open)      
    return 0;
}