/*
* breakout.c
*
* Robert Hill
* Computer Science 50
* Problem Set 4
* 3/3/2014
*/

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// width and height of paddle in pixels
#define P_WIDTH 60
#define P_HEIGHT 10

// constant for set y-axis value of the paddle
#define Y_PADDLE 500

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // variables for psuedo random generated velocity of ball
    double x_velocity = drand48() + 1;
    double y_velocity = drand48() + 3; 
           
    // label to instruct user to click to start game
    GLabel start = newGLabel("CLICK TO BEGIN");
    setFont(start, "SansSerif-18");
    
    // set location of start label
    double start_x = (WIDTH - getWidth(start)) / 2;
    double start_y = (HEIGHT + getFontAscent(start)) / 2 + 100;
    setLocation(start, start_x, start_y);
    
    // add start label to window
    add(window, start);
    
    // wait for user click to begin game
    waitForClick();
    
    // remove the start label
    removeGWindow(window, start);
    
    // initialize the scoreboard to display 0
    updateScoreboard(window, label, 0);

    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // TODO
        // check for mouse event
        GEvent mouse_move = getNextEvent(MOUSE_EVENT);

        // if mouse event detected
        if (mouse_move != NULL)
        {
            // if the event was movement move paddle
            if (getEventType(mouse_move) == MOUSE_MOVED)
            {
                double x_paddle = getX(mouse_move) - getWidth(paddle) / 2;
                double y_paddle = Y_PADDLE;
                setLocation(paddle, x_paddle, y_paddle);
            }
         }
         
         // move ball on window
         move(ball, x_velocity, y_velocity);

        // bounce off right, left, and top of game window
        if (getX(ball) + getWidth(ball) >= WIDTH || getX(ball) <= 0)
        {
            x_velocity = -x_velocity;
        }
        else if (getY(ball) <= 0)
        {
            y_velocity = -y_velocity;
        }
        else if (getY(ball) + getWidth(ball) >= HEIGHT)
        {  
            // if ball contacts bottom wall decrement            
            lives--;
            
            // if lives remain, reset ball and paddle
            if (lives)
            {
                double x_ball = (WIDTH - getWidth(ball)) / 2;
                double y_ball = (HEIGHT - getHeight(ball)) / 2;
                double x_paddle = (WIDTH - P_WIDTH) / 2 + 5;
                double y_paddle = ((HEIGHT - P_HEIGHT) / 2) + 205;
                setLocation(ball, x_ball, y_ball);
                setLocation(paddle, x_paddle, y_paddle);
                setVisible(ball, true);
                setVisible(paddle, true);
                waitForClick();
             }
             // if no lives, generate "GAME OVER" label
             else if (lives == 0)
             {
                setVisible(label, false);
                setVisible(ball, false);
                setVisible(paddle, false);
                GLabel over = newGLabel("GAME OVER");
                setFont(over, "SansSerif-18");
                double over_x = (WIDTH - getWidth(over)) / 2; 
                double over_y = (HEIGHT + getFontAscent(over)) / 2;
                setLocation(over, over_x, over_y);
                add(window, over);
                waitForClick();
             }
              
           } 
                  
        // linger before moving again
        pause(10);
        
        // call detectCollision and store return value in collision
        // which is of a GObject type
        GObject collision = detectCollision(window, ball);
        
        // if collision with brick, remove brick, reverse velocity 
        // and increment point variable, else if collision with paddle, 
        // reverse y_velocity, else if collision with scoreboard, ignore
        if (collision != NULL && collision != paddle && collision != label)
        {
            removeGWindow(window, collision);
            y_velocity = -y_velocity;
            points++;
            bricks--;
            updateScoreboard(window, label, points);
        }
        // if y velocity is greater than zero reverse velocity to ensure the ball
        // does not stick or roll on paddle
        else if (collision == paddle && y_velocity > 0)
        {  
             y_velocity = -y_velocity; 
        }
     }
     
     // if all bricks have been removed set paddle and ball to not visible
     if (bricks == 0)
     {
        setVisible(ball, false);
        setVisible(paddle, false);
     
        // create a label to let user know they have won
        GLabel win = newGLabel("YOU WON!!!");
        setFont(win, "SansSerif-18");
     
        // place the label in the center of the window       
        double win_x = (WIDTH - getWidth(win)) / 2; 
        double win_y = (HEIGHT + getFontAscent(win)) / 2;
        setLocation(win, win_x, win_y);
        
        // add win label to window
        add(window, win);
    
        waitForClick();
     }
     
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // set variables for brick width and height
    int b_width = (WIDTH / 10) - 6;
    int b_height = 10;
    int space = 5;
    int y = 70;
    
    // loop through rows 
    for (int i = 0; i < ROWS; i++)
    {   
        // loop through columns
        int x = 7;
        for (int j = 0; j < COLS; j++)
        { 
            // instantiates a brick and sets fill to true             
            GRect brick = newGRect(x, y, b_width, b_height);
            setFilled(brick, true);
            
            // sets brick row color            
            switch(i)
            {
                case 0: setColor(brick, "RED"); break;
                case 1: setColor(brick, "BLUE"); break;
                case 2: setColor(brick, "MAGENTA"); break;
                case 3: setColor(brick, "GREEN"); break;
                case 4: setColor(brick, "ORANGE"); break;
            }
            
            add(window, brick);
            // sets brick column location to brick width + space
            x += b_width + space; 
        }
        
        // sets brick row location to brick height + space
        y += b_height + space;
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // creates ball, sets color to black and location to center of window
    GOval ball = newGOval(0, 0, RADIUS * 2, RADIUS * 2);
   
    // sets ball location to center of window
    double x_ball = (WIDTH - getWidth(ball)) / 2;
    double y_ball = (HEIGHT - getHeight(ball)) / 2;
    setLocation(ball, x_ball, y_ball);
    
    // set color of ball to black
    setFilled(ball, true);
    setColor(ball, "BLACK");
    
    // add ball to window
    add(window, ball);
    
    // return ball of type GOval
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{   
    // create paddle     
    GRect paddle = newGRect(0, 0, P_WIDTH, P_HEIGHT);
    
    // sets paddle location to center bottom of the window
    double x_paddle = (WIDTH - P_WIDTH) / 2 + 5;
    double y_paddle = ((HEIGHT - P_HEIGHT) / 2) + 200;
    setLocation(paddle, x_paddle, y_paddle);
    
    // set color of paddle to black
    setFilled(paddle, true);
    setColor(paddle, "BLACK");
    
    // add paddle to window
    add(window, paddle);
    
    // return paddle
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // create label to display the score
    GLabel label = newGLabel(" ");
    
    // set font and color of label 
    setFont(label, "SansSerif-52");
    setColor(label, "LIGHT_GRAY");
    
    // add label to window
    add(window, label);
    
    // return a label of type GLabel
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
