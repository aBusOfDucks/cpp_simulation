#ifndef ANTS_CONST_H
#define ANTS_CONST_H

#define WIDTH 1500
#define HEIGHT 800
#define CHART_WIDTH 200

#define CELL_SIZE 10
#define NUMBER_OF_CELLS 100
#define MAX_CELL_SPEED 10

#define CHANCE_TO_DIE 500
#define CHANCE_TO_HEAL 300
#define CHANCE_TO_BECOME_IMMUNE 3

#define STATUS_NUMBER 4

#define STATUS_DEAD 0
#define STATUS_SICK 1
#define STATUS_NORMAL 2
#define STATUS_IMMUNE 3

#define COLOR_DEAD al_map_rgb(255, 255, 255)
#define COLOR_SICK al_map_rgb(255, 0, 0)
#define COLOR_NORMAL al_map_rgb(0, 255, 0)
#define COLOR_IMMUNE al_map_rgb(0, 0, 255)
#endif //ANTS_CONST_H
