#ifndef GRAPHICS_H
#define GRAPHICS_H


/**
 * Draws the player. This depends on the player state, so it is not a DrawFunc.
 */
void draw_player(int u, int v, int key);

/**
 * Takes a string image and draws it to the screen. The string is 121 characters
 * long, and represents an 11x11 tile in row-major ordering (across, then down,
 * like a regular multi-dimensional array). The available colors are:
 *      R = Red
 *      Y = Yellow
 *      G = Green
 *      D = Brown ("dirt")
 *      5 = Light grey (50%)
 *      3 = Dark grey (30%)
 *      Any other character is black
 * More colors can be easily added by following the pattern already given.
 */
void draw_img(int u, int v, const char* img);

/**
 * DrawFunc functions. 
 * These can be used as the MapItem draw functions.
 */
void draw_nothing(int u, int v);
void draw_wall(int u, int v);
void draw_portal(int u, int v);
void draw_lightning(int x, int y);
void draw_fire(int x, int y);
void draw_water(int x, int y);
void draw_staff(int x, int y);
void draw_npc(int x, int y);
void draw_plant(int x, int y);
void draw_dragon(int x, int y);
void draw_gate(int x, int y);
void draw_chest(int x, int y);
void draw_head(int x, int y);
void draw_bad(int x, int y);


/**
 * Draw the upper status bar.
 */
void draw_upper_status(int x, int y, int map_num);

/**
 * Draw the lower status bar.
 */ 
void draw_lower_status(bool key, bool menu, bool fire, bool water, bool lightning, int menu_num, int lives);

/**
 * Draw the border for the map.
 */
void draw_border();

#endif // GRAPHICS_H