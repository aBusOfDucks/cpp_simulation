#ifndef ANTS_ANT_H
#define ANTS_ANT_H

#include "const.h"
#include <iostream>
#include <math.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

class cell {
private:
    double poz_x, poz_y;
    double dx, dy;
    bool is_good;
    double speed;
    bool alive;
    bool immune;

    void relocate()
    {
        std::random_device dev;
        std::mt19937 rng;
        rng = std::mt19937(dev());
        std::uniform_int_distribution<std::mt19937::result_type> poz_x_generator;
        std::uniform_int_distribution<std::mt19937::result_type> poz_y_generator;

        poz_x_generator = std::uniform_int_distribution<std::mt19937::result_type>(CELL_SIZE, WIDTH - CELL_SIZE);
        poz_y_generator = std::uniform_int_distribution<std::mt19937::result_type>(CELL_SIZE, HEIGHT - CELL_SIZE);

        poz_x = poz_x_generator(rng);
        poz_y = poz_y_generator(rng);
    }

    void get_direction()
    {
        std::random_device dev;
        std::mt19937 rng;
        rng = std::mt19937(dev());
        std::uniform_int_distribution<std::mt19937::result_type> poz_x_generator;
        std::uniform_int_distribution<std::mt19937::result_type> poz_y_generator;
        std::uniform_int_distribution<std::mt19937::result_type> speed_generator;

        poz_x_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, WIDTH);
        poz_y_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, HEIGHT);
        speed_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, MAX_CELL_SPEED * 100);

        int x = poz_x_generator(rng);
        int y = poz_y_generator(rng);

        dx = x - poz_x;
        dy = y - poz_y;

        speed = speed_generator(rng) / 100.0;
        double dis = dx * dx + dy * dy;
        dis = sqrt(dis);
        if(dis == 0)
        {
            dx = 0;
            dy = 0;
        }
        else
        {
            dx = dx / dis * speed;
            dy = dy / dis * speed;
        }

    }

    ALLEGRO_COLOR get_color()
    {
        if(immune)
            return COLOR_IMMUNE;
        if(!alive)
            return COLOR_DEAD;
        if(is_good)
            return COLOR_NORMAL;
        else
            return COLOR_SICK;
    }

    void sickness_check() {
        if (is_good)
            return;
        std::random_device dev;
        std::mt19937 rng;
        rng = std::mt19937(dev());
        std::uniform_int_distribution<std::mt19937::result_type> death_generator;
        std::uniform_int_distribution<std::mt19937::result_type> heal_generator;
        std::uniform_int_distribution<std::mt19937::result_type> immune_generator;

        death_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, CHANCE_TO_DIE);
        heal_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, CHANCE_TO_HEAL);
        immune_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, CHANCE_TO_BECOME_IMMUNE);
        if (death_generator(rng) == 0)
            alive = false;
        if (heal_generator(rng) == 0)
        {
            is_good = true;
            if(immune_generator(rng) == 0)
                immune = true;
        }
    }

public:

    void init()
    {
        std::random_device dev;
        std::mt19937 rng;
        rng = std::mt19937(dev());
        std::uniform_int_distribution<std::mt19937::result_type> generator;

        generator = std::uniform_int_distribution<std::mt19937::result_type>(0, 1);

        is_good = (generator(rng) == 0);
        set();
        alive = true;
        immune = false;
    }

    int get_status()
    {
        if(!alive)
            return STATUS_DEAD;
        if(immune)
            return STATUS_IMMUNE;
        if(!is_good)
            return STATUS_SICK;
        return STATUS_NORMAL;
    }

    void draw()
    {
        al_draw_filled_circle(poz_x, poz_y, CELL_SIZE, get_color());
    }

    void set()
    {
        alive = true;
        immune = false;
        relocate();
        get_direction();
    }



    void move()
    {
        poz_x += dx;
        poz_y += dy;
        if(poz_x < CELL_SIZE || poz_x > WIDTH - CELL_SIZE)
        {
            poz_x -= dx * 2;
            dx *= -1;
        }
        if(poz_y < CELL_SIZE || poz_y > HEIGHT - CELL_SIZE)
        {
            poz_y -= dy * 2;
            dy *= -1;
        }
        sickness_check();
    }

    bool collide(cell a)
    {
        double dis_x = poz_x - a.poz_x;
        double dis_y = poz_y - a.poz_y;
        double dis = dis_x * dis_x + dis_y * dis_y;
        dis = sqrt(dis);
        if(dis <= 2 * CELL_SIZE)
            return true;
        return false;
    }

    void change(bool b)
    {
        is_good = b;
    }

    void bounce(cell * a)
    {
        // TODO: more realistic collisions
        double dis_x = poz_x - a->poz_x;
        double dis_y = poz_y - a->poz_y;
        double dis = dis_x * dis_x + dis_y * dis_y;
        dis = sqrt(dis);
        dx = dis_x;
        dy = dis_y;
        a->dx = -dx;
        a->dy = -dy;
        double new_speed = (speed + a->speed) / 2.0;
        speed = new_speed;
        a->speed = new_speed;
        if(dis > 0)
        {
            dx = dx / dis * speed;
            dy = dy / dis * speed;
            a->dx = a->dx / dis * speed;
            a->dy = a->dy / dis * speed;
        }
    }

    void meet(cell * a)
    {
        bounce(a);
        if(!alive || !a->alive)
            return;
        if(immune || a->immune)
            return;
        if(a->is_good && is_good)
        {
            return;
        }
        if((!a->is_good) &&  (!is_good))
        {
            return;
        }
        a->change(false);
        is_good = false;

    }
};

#endif //ANTS_ANT_H
