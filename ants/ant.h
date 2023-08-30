#ifndef ANTS_ANT_H
#define ANTS_ANT_H

#include "candy.h"
#include <iostream>
#include <math.h>

class ant {
private:
    double poz_x, poz_y;
    double dx, dy;

    int color;

    int speed;

    int candy_eaten;

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

    ALLEGRO_COLOR get_color()
    {
        return al_map_rgb(speed, 100, color);
    }

    double trim(double x, double a, double b)
    {
        if(x < a)
            return a;
        if(x > b)
            return b;
        return x;
    }

    void mutate()
    {
        std::random_device dev;
        std::mt19937 rng;
        rng = std::mt19937(dev());
        std::uniform_int_distribution<std::mt19937::result_type> poz_generator;
        std::uniform_int_distribution<std::mt19937::result_type> speed_generator;

        poz_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, 41);
        speed_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, 11);

     //   poz_x += (double) poz_generator(rng) - 20;
     //   poz_y += (double) poz_generator(rng) - 20;
        speed += speed_generator(rng) - 5;

     //   poz_x = trim(poz_x, 0, WIDTH - 1);
    //    poz_y = trim(poz_y, 0, HEIGHT - 1);
        speed = trim(speed, 0, 255);

    }

public:

    void init()
    {
        std::random_device dev;
        std::mt19937 rng;
        rng = std::mt19937(dev());
        std::uniform_int_distribution<std::mt19937::result_type> speed_generator;

        speed_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, 256);

        speed = speed_generator(rng);
        color = speed_generator(rng);
        set();
    }

    void draw()
    {
        al_draw_filled_rectangle(poz_x - ANT_SIZE, poz_y - ANT_SIZE, poz_x + ANT_SIZE, poz_y + ANT_SIZE, get_color());
    }

    void set()
    {
        relocate();
        candy_eaten = 0;
    }


    void find_target(int candy_number, candy * candies)
    {
        int closest_id = -1;
        double closest_dist;

        for(int i = 0; i < candy_number; i++)
        {
            if(!candies[i].has_been_eaten())
            {
                if(closest_id == -1)
                {
                    closest_id = i;
                    closest_dist = candies[i].calculate_distance_square(poz_x, poz_y);
                }
                else
                {
                    double dist = candies[i].calculate_distance_square(poz_x, poz_y);
                    if(dist < closest_dist)
                    {
                        closest_dist = dist;
                        closest_id = i;
                    }
                }
            }
        }

        if(closest_id == -1)
        {
            dx = 0;
            dy = 0;
            return;
        }

        candies[closest_id].calculate_vector(poz_x, poz_y, &dx, &dy);

        double distance = sqrt(closest_dist);

        if(distance >= (speed / 64.0))
        {
            if(distance > 0)
            {
                dx = (dx / distance) * (speed / 64.0);
                dy = (dy / distance) * (speed / 64.0);
            }
            else
            {
                dx = 0;
                dy = 0;
            }
        }
    }

    void move()
    {
        poz_x += dx;
        poz_y += dy;
    }

    void try_to_eat(int candy_number, candy * candies)
    {
        for(int i = 0; i < candy_number; i++)
        {
            if(!candies[i].has_been_eaten())
            {
                if(candies[i].check_collision(poz_x, poz_y))
                {
                    candy_eaten++;
                }
            }
        }
    }

    void copy(ant a)
    {
        speed = a.speed;
 //       poz_x = a.poz_x;
   //     poz_y = a.poz_y;
        relocate();
        candy_eaten = 0;
        color = a.color;
        mutate();
    }

    int how_many_eaten()
    {
        return candy_eaten;
    }
};

#endif //ANTS_ANT_H
