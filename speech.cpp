#include "speech.h"

#include "globals.h"
#include "hardware.h"
#include "graphics.h"
/**
 * Draw the speech bubble background.
 * Use this file to draw speech bubbles, look at the uLCD libraries for printing
 * text to the uLCD (see graphics.cpp note for link)
 */
static void draw_speech_bubble();

/**
 * Erase the speech bubble.
 */
static void erase_speech_bubble();

/**
 * Draw a single line of the speech bubble.
 * @param line The text to display
 * @param which If TOP, the first line; if BOTTOM, the second line.
 */
#define TOP    0
#define BOTTOM 1
static void draw_speech_line(const char* line, int which);

/**
 * Delay until it is time to scroll.
 */
static void speech_bubble_wait();

void draw_speech_bubble()
{
    uLCD.filled_rectangle(0, 113 , 128, 93, WHITE);
    uLCD.filled_rectangle(0,    93, 128,  94, 0x00FF00); 
    uLCD.filled_rectangle(0,   113, 128, 112, 0x00FF00); 
    uLCD.filled_rectangle(0,    113,   1, 93, 0x00FF00);
    uLCD.filled_rectangle(128,  113, 126, 93, 0x00FF00); 
}

void erase_speech_bubble()
{
    
}

void draw_speech_line(const char* line, int which)
{
    uLCD.color(BLACK);
    uLCD.textbackground_color(WHITE);
    //
        switch(which){
            case TOP:
                uLCD.locate(1,12);
                while(*line){
                uLCD.printf("%c", *line);
                line++;}
                break;
            case BOTTOM:    
                uLCD.locate(1,13);
                while(*line){
                uLCD.printf("%c", *line);
                line++;}
                speech_bubble_wait();
                break;
    }
}

void speech_bubble_wait()
{
    Timer t; t.start();
    GameInputs in; 
    int time=0;
    bool blink=false;
    while(in.b1)
    {
        in = read_inputs();
        time = t.read_ms();
        wait_ms(150);
        blink = !blink;
        if (blink) 
            uLCD.filled_circle(120,121,4,0x00FF00); //green blink
        else 
            uLCD.filled_circle(120,121,4,BLACK);
        in = read_inputs();
    }

    uLCD.filled_circle(120,121,4,BLACK); 
}

void speech(const char* line1, const char* line2)
{
    draw_speech_bubble();
    draw_speech_line(line1, TOP);
    draw_speech_line(line2, BOTTOM);
    speech_bubble_wait();
    draw_border();
}

void long_speech(const char* lines[], int n)
{
const char* line1;
const char* line2;
 for(int i = 0; i < n; i+=2){ 
 draw_speech_bubble();
 line1 = lines[i];
 line2 = lines[i+1];
    draw_speech_line(line1, TOP);
    draw_speech_line(line2, BOTTOM);
    speech_bubble_wait();
    }
    draw_border();   
}
