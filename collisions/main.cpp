#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <random>

#include "cell.h"
#include <vector>

class simulation{
private:
    ALLEGRO_DISPLAY * display;
    std::mutex mutex_display;
    std::condition_variable cv_display;
    bool display_set = false;

    std::mutex mutex_end;
    std::condition_variable cv_end;
    bool game_end = false;

    std::vector<cell> cells;
    ALLEGRO_COLOR chart_colors[STATUS_NUMBER];
    int chart_status[STATUS_NUMBER];
    int chart_single_width;
    int chart_single_height;


    void chart_init()
    {
        chart_colors[STATUS_NORMAL] = COLOR_NORMAL;
        chart_colors[STATUS_SICK] = COLOR_SICK;
        chart_colors[STATUS_IMMUNE] = COLOR_IMMUNE;
        chart_colors[STATUS_DEAD] = COLOR_DEAD;
        chart_single_width = CHART_WIDTH / STATUS_NUMBER;
        chart_single_height = HEIGHT / NUMBER_OF_CELLS;

    }

public:
    void init()
    {
        chart_init();
        restart();
        for(size_t i = 0; i < NUMBER_OF_CELLS; i++)
        {
            cell a;
            a.init();
            bool is_ok = true;
            for(size_t j = 0; j < i && is_ok; j++)
            {
                if(a.collide(cells[j]))
                {
                    is_ok = false;
                }
            }
            if(is_ok)
                cells.push_back(a);
            else
                i--;
        }

    }

    void restart()
    {
        display_set = false;
        game_end = false;
        for(size_t i = 0; i < cells.size(); i++)
        {
            cells[i].set();
            for(size_t j = 0; j < i; j++)
            {
                if(cells[i].collide(cells[j]))
                {
                    i--;
                    break;
                }
            }
        }
    }


    void draw()
    {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_filled_rectangle(WIDTH, 0, WIDTH + CHART_WIDTH, HEIGHT, al_map_rgb(100, 100, 100));
        for(int i = 0; i < STATUS_NUMBER; i++)
            chart_status[i] = 0;
        for(size_t i = 0; i < cells.size(); i++)
            chart_status[cells[i].get_status()]++;

        for(int i = 0; i < STATUS_NUMBER; i++)
        {
            int x1 = WIDTH + i * chart_single_width;
            int x2 = x1 + chart_single_width;
            int y2 = HEIGHT;
            int y1 = HEIGHT - chart_status[i] * chart_single_height;
            al_draw_filled_rectangle(x1, y1, x2, y2, chart_colors[i]);
        }

        for(size_t i = 0; i < cells.size(); i++)
            cells[i].draw();

        al_flip_display();

    }

    void set_display(ALLEGRO_DISPLAY * d)
    {
        std::unique_lock<std::mutex> lock(mutex_display);
        display = d;
        display_set = true;
        cv_display.notify_all();
    }

    ALLEGRO_DISPLAY * get_display()
    {
        std::unique_lock<std::mutex> lock(mutex_display);
        cv_display.wait(lock, [this]{return display_set;});
        return display;
    }


    void wait_for_end()
    {
        std::unique_lock<std::mutex> lock(mutex_end);
        cv_end.wait(lock, [this]{return game_end;});
    }

    void end()
    {
        std::unique_lock<std::mutex> lock(mutex_end);
        game_end = true;
        cv_end.notify_all();
    }

    bool is_ended()
    {
        std::unique_lock<std::mutex> lock(mutex_end);
        return game_end;
    }

    void move()
    {
        for(size_t i = 0; i < cells.size(); i++)
        {
            cells[i].move();
        }
        for(size_t i = 0; i < cells.size(); i++)
        {
            for(size_t j = 0; j < i; j++)
            {
                if(cells[i].collide(cells[j]))
                {
                    cells[i].meet(&cells[j]);
                }
            }
        }
    }
};

void input_manager(simulation &sim)
{

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(sim.get_display()));
    ALLEGRO_EVENT event;
    bool run = true;
    while(run)
    {
        al_wait_for_event(queue, &event);
        switch(event.type)
        {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
            case ALLEGRO_EVENT_KEY_DOWN:
                sim.end();
                run = false;
                break;
        }
    }
    al_destroy_event_queue(queue);
}

void create_display(simulation & sim)
{
    al_set_new_display_flags(ALLEGRO_WINDOWED);
    ALLEGRO_DISPLAY* disp = al_create_display(WIDTH + CHART_WIDTH, HEIGHT);
    al_init_primitives_addon();
    sim.set_display(disp);
    while(!sim.is_ended())
    {
        sim.draw();
        sim.move();
        usleep(1000);
    }

    sim.wait_for_end();
}

void init(simulation & game)
{
    al_init();
    al_install_keyboard();
    al_install_mouse();
    game.init();
}

int main()
{
    simulation sim;
    init(sim);
    std::thread display([&sim]{create_display(sim);});
    std::thread input([&sim]{input_manager(sim);});
    input.join();
    display.join();
    return 0;
}
