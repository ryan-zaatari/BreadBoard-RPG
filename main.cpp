// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"
#include "mbed.h"
#include "SDFileSystem.h"
// Functions in this file
int get_action (GameInputs inputs);
int update_game (int action);
void draw_game (int init);
void init_main_map ();
int main ();
GameInputs input;
int action;
int update;
int staff = 0;
int map_num = 0;
int omni = false;
int npc_num = 0;
bool key = false;
bool fire_got = false;
bool lightning_got = false;
bool water_got = false;
bool menu = false;
int menu_num=0;
bool treasure_got = false;
int lives = 3;
int move = 6;
int move_direction = 0;
/**
 * The main game state. Must include Player locations and previous locations for
 * drawing to work properly. Other items can be added as needed.
 */
struct {
    int x,y;    // Current locations
    int px, py; // Previous locations
    int has_key;
    // You can add other properties for the player here
} Player;

/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possbile return values are defined below.
 */
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define OMNI_BUTTON 2
#define MENU_BUTTON 8
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_UP 5
#define GO_DOWN 6
#define END 7
#define RUN_LEFT 10
#define RUN_RIGHT 11
#define RUN_UP 12
#define RUN_DOWN 13

char *pickup_staff1 = "Got staff,";
char *pickup_staff2 = "no spells yet..";
char *pickup_lightning = "Got lightning";
char *pickup_spell = "spell";
char *pickup_fire = "Got fire";
char *pickup_water = "Got water";
char *use_lightning = "Used lightning";
char *use_fire = "Used fire";
char *use_water = "Used water";
char *nothing = "nothing happened";

const char *meet_npc[6] = {"You are alive!", "Find your staff", "and the elements", "to defeat the", "dragon. He likes", "fire"};
char *meet_npc2 = "Get on your way!";
const char *meet_npc3[2] = {"Amazing", "have this key"};

const char *dragon_dead[4] = {"Good job!", "Dragon is dead!", "collect reward", "from npc"};
char *gate_locked = "Gate locked";
int get_action(GameInputs inputs)
{
    if(treasure_got)
        return END;
    if (inputs.ay > 0.55) 
        return RUN_UP;
    if (inputs.ay < -0.55) 
        return RUN_DOWN;
    if (inputs.ax < -0.55)  
        return RUN_LEFT;
    if (inputs.ax > 0.55) 
        return RUN_RIGHT;
    if (inputs.ay > 0.3) 
        return GO_UP;
    if (inputs.ay < -0.3) 
        return GO_DOWN;
    if (inputs.ax < -0.3)  
        return GO_LEFT;
    if (inputs.ax > 0.3) 
        return GO_RIGHT;
    if (!inputs.b1) 
        return ACTION_BUTTON;
    if (!inputs.b2) 
        return OMNI_BUTTON;
    if(!inputs.b3)
        return MENU_BUTTON;
    return NO_ACTION;
}

int go_up(int x, int y){
    if(get_north(x, y)->walkable || omni) {
        Player.y--;
        return 1;
    }
    return 0;
}

int go_left(int x, int y){
    if(get_west(x, y)->walkable || omni) {
        Player.x--;
        return 1;
    }
    return 0;
}
int go_down(int x, int y){
    if(get_south(x, y)->walkable || omni) {
        Player.y++;
        return 1;
    }
    return 0;
}

int go_right(int x, int y){
    if(get_east(x, y)->walkable || omni) {
        Player.x++;
        return 1;
    }
    return 0;
}
int run_down(int x, int y){
    if((get_south(x, y)->walkable && get_south(x, y+1)->walkable)|| omni) {
        Player.y+=2;
        return 1;
    }
    return 0;
}

int run_right(int x, int y){
    if((get_east(x, y)->walkable && get_east(x+1, y)->walkable) || omni) {
        Player.x+=2;
        return 1;
    }
    return 0;
}
int run_up(int x, int y){
    if((get_north(x, y)->walkable && get_north(x, y-1)->walkable) || omni) {
        Player.y-=2;
        return 1;
    }
    return 0;
}
int run_left(int x, int y){
    if((get_west(x, y)->walkable && get_west(x-1, y)->walkable) || omni) {
        Player.x-=2;
        return 1;
    }
    return 0;
}

int interact(int x, int y)
{
    MapItem *item1 = get_north(x, y); 
    MapItem *item2 = get_south(x,y);
    MapItem *item3 = get_east(x, y); 
    MapItem *item4 = get_west(x,y);
    if(item1->type == STAFF) {
        staff = 1;
        Player.has_key = 2;
        map_erase(x,y-1);
        speech(pickup_staff1, pickup_staff2); }
    else if(item2->type == STAFF) {
        staff = 1;
        Player.has_key = 2;
        map_erase(x,y+1);
        speech(pickup_staff1, pickup_staff2); }
    else if(item3->type == STAFF) {
        staff = 1;
        Player.has_key = 2;
        map_erase(x +1,y);
        speech(pickup_staff1, pickup_staff2); }
    else if(item4->type == STAFF) {
        staff = 1;
        Player.has_key = 2;
        map_erase(x-1,y);
        speech(pickup_staff1, pickup_staff2); } 
    else if(item1->type == PORTAL || item2->type == PORTAL || item3->type == PORTAL || item4->type == PORTAL) {
        map_num++;
        set_active_map(map_num%2);
        }
    else if((item1->type == NPC || item2->type == NPC || item3->type == NPC || item4->type == NPC) && !npc_num) {
        long_speech(meet_npc,6);
        add_portal(1,1);
        npc_num++; }
    else if((item1->type == NPC || item2->type == NPC || item3->type == NPC || item4->type == NPC)&& npc_num==1) {
        speech(meet_npc2, NULL);}
    else if((item1->type == NPC || item2->type == NPC || item3->type == NPC || item4->type == NPC)&& npc_num==2) {
        long_speech(meet_npc3,2);
        key = true;
        npc_num=1;}
    else if(item1->type == CHEST || item2->type == CHEST|| item3->type == CHEST || item4->type == CHEST){
        treasure_got = true;      
    }
    else if(item1->type == GATE && !key) {
        speech(gate_locked, NULL); }
    else if(item1->type == GATE && key && menu_num == 0) {
        map_erase(12,5);
        map_erase(13,5);
        map_erase(14,5);
        map_erase(15,5);
        map_erase(16,5);} 
    else if(item1->type == BAD ){
        if(Player.has_key ==5){
            map_erase(x,y-1);
        } else{
            lives--;}}
    else if(item4->type == BAD){
        if(Player.has_key ==5){
            map_erase(x-1,y);
        } else{
            lives--;}}
    if(staff){
        if(item1->type == LIGHTNING) {
            Player.has_key = 3;
            map_erase(x,y-1);
            menu_num = 3;
            lightning_got = true;
            speech(pickup_lightning, pickup_spell); }
        else if(item2->type == LIGHTNING) {
            Player.has_key = 3;
            map_erase(x,y+1);
            menu_num = 3;
            lightning_got = true;
            speech(pickup_lightning, pickup_spell); }
        else if(item3->type == LIGHTNING) {
            Player.has_key = 3;
            map_erase(x +1,y);
            menu_num = 3;
            lightning_got = true;
            speech(pickup_lightning, pickup_spell); }
        else if(item4->type == LIGHTNING) {
            Player.has_key = 3;
            map_erase(x-1,y);
            lightning_got = true;
            menu_num = 3;
            speech(pickup_lightning, pickup_spell); }
        else if(item1->type == WATER) {
            Player.has_key = 4;
            map_erase(x,y-1);
            menu_num = 2;
            water_got = true;
            speech(pickup_water, pickup_spell); }
        else if(item2->type == WATER) {
            Player.has_key = 4;
            map_erase(x,y+1);
            menu_num = 2;
            water_got = true;
            speech(pickup_water, pickup_spell); }
        else if(item3->type == WATER) {
            Player.has_key = 4;
            map_erase(x +1,y);
            menu_num = 2;
            water_got = true;
            speech(pickup_water, pickup_spell); }
        else if(item4->type == WATER) {
            Player.has_key = 4;
            water_got = true;
            map_erase(x-1,y);
            menu_num = 2;
            speech(pickup_water, pickup_spell); }
        else if(item1->type == FIRE) {
            Player.has_key = 5;
            fire_got = true;
            map_erase(x,y-1);
            menu_num = 1;
            speech(pickup_fire, pickup_spell); }
        else if(item2->type == FIRE) {
            Player.has_key = 5;
            map_erase(x,y+1);
            menu_num = 1;
            fire_got = true;
            speech(pickup_fire, pickup_spell); }
        else if(item3->type == FIRE) {
            Player.has_key = 5;
            map_erase(x +1,y);
            fire_got = true;
            menu_num = 1;
            speech(pickup_fire, pickup_spell); }
        else if(item4->type == FIRE) {
            Player.has_key = 5;
            fire_got = true;
            menu_num = 1;
            map_erase(x-1,y);
            speech(pickup_fire, pickup_spell); }
        else if(item1->type == HEAD ||item2->type == HEAD||item3->type == HEAD||item4->type == HEAD){
                lives--;}
        else if(Player.has_key != 5 && Player.has_key != 2){
            if(item1->type == DRAGON ||item2->type == DRAGON||item3->type == DRAGON||item4->type == DRAGON) {
                map_erase(5,5);
                map_erase(6,5);
                map_erase(7,5);
                map_erase(7,6);
                map_erase(8,6);
                map_erase(9,7);
                map_erase(10,7);
                map_erase(8,6);
                map_erase(4,5);
                map_erase(8,7);
                switch(Player.has_key){
                    case 3: speech(use_lightning,NULL); break;
                    case 4: speech(use_water,NULL); break;}
                long_speech(dragon_dead,4);
                npc_num=2;
            }
        }
        else if(Player.has_key == 5){
            if(item1->type == DRAGON ||item2->type == DRAGON||item3->type == DRAGON||item4->type == DRAGON) {
                speech(use_fire,NULL); 
                speech(nothing, NULL);
            }
        }
    
}
    return 1;
}
/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 * 
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT 0
#define GAME_OVER 1
#define FULL_DRAW 2
int update_game(int action)
{
    // Save player previous location before updating
    Player.px = Player.x;
    Player.py = Player.y;
    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    switch(action)
    {
        case END:
            return END;
        case RUN_UP:   
         if (run_up(Player.px,Player.py))
        { 
            return FULL_DRAW;
        }
        break;
        case RUN_LEFT:
        if (run_left(Player.px,Player.py))
        { 
            return FULL_DRAW;
        }        break;            
        case RUN_DOWN:
        if (run_down(Player.px,Player.py))
        { 
            return FULL_DRAW;
        }            break;
        case RUN_RIGHT:
        if (run_right(Player.px,Player.py))
        { 
            return FULL_DRAW;
        }        break;
        case GO_UP:   
         if (go_up(Player.px,Player.py))
        { 
            return FULL_DRAW;
        }
        break;
        case GO_LEFT:
        if (go_left(Player.px,Player.py))
        { 
            return FULL_DRAW;
        }        break;            
        case GO_DOWN:
        if (go_down(Player.px,Player.py))
        { 
            return FULL_DRAW;
        }            break;
        case GO_RIGHT:
        if (go_right(Player.px,Player.py))
        { 
            return FULL_DRAW;
        }        break;
        case ACTION_BUTTON: 
        if (interact(Player.px,Player.py))
            return FULL_DRAW;
        break;
        case OMNI_BUTTON:
            omni = !omni;
         break;
         case MENU_BUTTON:
            menu = true;
            if(menu_num==3)
                menu_num =0;
            else menu_num++;
            wait_ms(150);
            return FULL_DRAW;
        default:   return FULL_DRAW;  
    }
    return NO_RESULT;
}

void playSound(char * wav){
    // open wav file
    FILE *wave_file;
    wave_file=fopen(wav,"r");

    // play wav file
    waver.play(wave_file);

    // close wav file
    fclose(wave_file);
}
int turn = 0;
/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status 
 * bars. Unless init is nonzero, this function will optimize drawing by only 
 * drawing tiles that have changed from the previous frame.
 */
void draw_game(int init)
{
    if (init == END) {
        uLCD.filled_rectangle(0,0,255,255,BLACK);
        uLCD.locate(2,6);
        uLCD.textbackground_color(BLACK);
        uLCD.text_width(2);
        uLCD.text_height(2);
        uLCD.color(0xFFFFFF);
        uLCD.printf("YOU WIN");
        playSound("/sd/win.wav");

        wait(10000000000000000);
        }
    if(lives==0){
        uLCD.filled_rectangle(0,0,255,255,BLACK);
        uLCD.locate(1,6);
        uLCD.textbackground_color(BLACK);
        uLCD.text_width(2);
        uLCD.text_height(2);
        uLCD.color(0xFF0000);
        uLCD.printf("YOU DIED");
        playSound("/sd/dead.wav");
        wait(10000000000000000);    
        }
    // Draw game border first
    if(init) draw_border();
    if(get_active_map() == get_map(1)){
    if(move_direction == 0)
    {
        add_npc(move+1, 5);
        map_erase(move, 5);
        move++;
    }
    if(move_direction == 1)
    {
        add_npc(move-1, 5);
        map_erase(move, 5);
        move--;
    }
     if(move == 6)
        move_direction = 0;
    if(move == 9)
        move_direction = 1;  
    }
    if(menu_num == 1 && fire_got) Player.has_key = 5;
    else if(menu_num==2 && water_got) Player.has_key = 4;
    else if(menu_num==3 && lightning_got) Player.has_key = 3;
    // Iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // Iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // Iterate over one column of tiles
        {
            // Here, we have a given (i,j)
            
            // Compute the current map (x,y) of this tile
            int x = i + Player.x;
            int y = j + Player.y;
            
            // Compute the previous map (px, py) of this tile
            int px = i + Player.px;
            int py = j + Player.py;
                        
            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;
            
            // Figure out what to draw
            DrawFunc draw = NULL;
            if (init && i == 0 && j == 0) // Only draw the player on init
            {
                draw_player(u, v, Player.has_key);
                continue;
            }
            else if (x >= 0 && y >= 0 && x < map_width() && y < map_height()) // Current (i,j) in the map
            {
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // Only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        draw = curr_item->draw;
                    }
                    else // There used to be something, but now there isn't
                    {
                        draw = draw_nothing;
                    }
                }
            }
            else if (init) // If doing a full draw, but we're out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            // Actually draw the tile
            if (draw) draw(u, v);
        }
    }

    // Draw status bars    
    draw_upper_status(Player.x, Player.y, ((map_num %2)+1));
    draw_lower_status(key, menu, fire_got, water_got, lightning_got, menu_num, lives);
    
    menu = false;
}


/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion. Note: using the similar
 * procedure you can init the secondary map(s).
 */
void init_main_map()
{
    // "Random" plants
    Map* map = set_active_map(0);
    for(int i = map_width() + 3; i < map_area(); i += 39)
    {
        add_plant(i % map_width(), i / map_width());
    }
    add_staff(7, 7);
    add_npc(6,5);
    pc.printf("plants\r\n");
    add_plant(1,3);
    add_bad(2,3);
    add_plant(2,4);
    add_plant(3,1);
    add_plant(4,1);
    add_plant(4,2);
    add_plant(4,3);
    add_plant(4,4);
    add_fire(5,7);
    add_bad(1,2);
    add_bad(2,2);
    add_plant(3,2);
    add_bad(3,3);
    add_bad(3,4); 
    add_bad(1,5);    
    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    pc.printf("Walls done!\r\n");
    add_wall(11,              1, VERTICAL,5);
    add_wall(17,              1, VERTICAL,5);
    add_gate(12,5);
    add_gate(13,5);
    add_gate(14,5);
    add_gate(15,5);
    add_gate(16,5);
    print_map();
    add_chest(14,1);
}
void init_second_map()
{
    // "Random" plants
    Map* map = set_active_map(1);
    
    
    add_portal(1,1);
    add_head(4,5);
    add_dragon(5,5);
    add_dragon(6,5);
    add_dragon(7,5);
    add_dragon(7,6);
    add_dragon(8,6);
    add_dragon(8,7);
    add_dragon(9,7);
    add_dragon(10,7);
    pc.printf("plants\r\n");
    add_lightning(3,4);
    add_water(3,3);

    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    pc.printf("Walls done!\r\n");
    

    print_map();
}

/**
 * Program entry point! This is where it all begins.
 * This function orchestrates all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */


int main()
{
    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");
    
    //speaker.period(1/1000);
    //speaker=0.3; 
    //wait(.5);
    //speaker=0.0;
    uLCD.textbackground_color(0x000000);
    //uLCD.filled_rectangle(0,0,255,255,0x000000);
    uLCD.locate(3.5,2);
    uLCD.textbackground_color(0x000000);
    uLCD.text_width(1);
    uLCD.text_height(1);
    uLCD.color(0xFF0000);
    uLCD.printf("Lost Wizard");
    uLCD.color(0xFFFFFF);
    uLCD.locate(1,3);
    uLCD.printf("B1-Interact");
    uLCD.locate(1,5);
    uLCD.printf("\n B2-Omni-Potent");
    uLCD.locate(1,7);
    uLCD.printf("\n B3-Menu Button");
    uLCD.color(0xFF0000);
    uLCD.locate(1,13);
    uLCD.printf("By: Ryan Zaatari");
    uLCD.color(0xFFFFFF);
    uLCD.locate(1,10);
    uLCD.printf("\n Press B1");
    do{
    input = read_inputs();
    }while(input.b1);
    
    // Initialize the maps
    maps_init();
    init_main_map();
    init_second_map();
    
    // Initialize game state
    set_active_map(0);
    Player.x = Player.y = 5;
    Player.has_key = 1;
    // Initial drawing

    draw_game(true);
    draw_lower_status(key, true, fire_got, water_got, lightning_got, menu_num, lives);

    // Main game loop
    while(1)
    {
        // Timer to measure game update speed
        Timer t; t.start();
        // Actually do the game update:
        // 1. Read inputs        
                input = read_inputs();   

        // 2. Determine action (get_action)   
                action = get_action(input);   
     
        // 3. Update game (update_game)
                update = update_game(action);

        // 3b. Check for game over
        // 4. Draw frame (draw_game)
        draw_game(update);
        
        // 5. Frame delay
        t.stop();
        int dt = t.read_ms();
        
        if (dt < 100) wait_ms(100 - dt);
    }
}
