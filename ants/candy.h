#ifndef ANTS_CANDY_H
#define ANTS_CANDY_H

#include "const.h"

class candy {
private:
    double poz_x, poz_y;
    bool exist;

    void relocate()
    {
        std::random_device dev;
        std::mt19937 rng;
        rng = std::mt19937(dev());
        std::uniform_int_distribution<std::mt19937::result_type> poz_x_generator;
        std::uniform_int_distribution<std::mt19937::result_type> poz_y_generator;

        poz_x_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, WIDTH);
        poz_y_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, HEIGHT);

        poz_x = poz_x_generator(rng);
        poz_y = poz_y_generator(rng);
    }

public:
    void draw()
    {
        if(!exist)
            return;
        al_draw_filled_circle(poz_x, poz_y, CANDY_SIZE, CANDY_COLOR);
    }
    void set()
    {
        relocate();
        exist = true;
    }

    void eat()
    {
        exist = false;
    }

    bool has_been_eaten()
    {
        return !exist;
    }

    double calculate_distance_square(double x, double y)
    {
        double dx = x - poz_x;
        double dy = y - poz_y;
        return dx * dx + dy * dy;
    }

    void calculate_vector(double x, double y, double * dx, double * dy)
    {
        (*dx) = poz_x - x;
        (*dy) = poz_y - y;
    }

    bool check_collision(double x, double y)
    {
        double dx = x - poz_x;
        double dy = y - poz_y;
        double dis = dx * dx + dy * dy;
        dis = sqrt(dis);
        if(dis <= CANDY_SIZE + ANT_SIZE)
        {
            eat();
            return true;
        }
        else
            return false;
    }
};

#endif //ANTS_CANDY_H
