#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <random>

#include "ant.h"
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

    std::vector<ant> ants;
    candy candies[NUMBER_OF_CANDIES];

public:
    void init()
    {
        restart();
        for(int i = 0; i < NUMBER_OF_ANTS; i++)
        {
            ant a;
            a.init();
            ants.push_back(a);
        }

    }

    void restart()
    {
        display_set = false;
        game_end = false;
        for(size_t i = 0; i < ants.size(); i++)
            ants[i].set();
        for(int i = 0; i < NUMBER_OF_CANDIES; i++)
            candies[i].set();
    }

    void next_level()
    {
        for(int i = 0; i < NUMBER_OF_CANDIES; i++)
            candies[i].set();
        std::vector<ant> temp;
        for(size_t i = 0; i < ants.size(); i++)
        {
            int x = ants[i].how_many_kids();
            for(int j = 0; j < x; j++)
            {
                ant a;
                a.copy(ants[i]);
                temp.push_back(a);
            }
        }
        ants = temp;
    }

    void draw()
    {
        al_clear_to_color(al_map_rgb(0, 0, 0));

        for(size_t i = 0; i < ants.size(); i++)
            ants[i].draw();
        for(int i = 0; i < NUMBER_OF_CANDIES; i++)
            candies[i].draw();

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
        for(size_t i = 0; i < ants.size(); i++)
        {
            ants[i].find_target(NUMBER_OF_CANDIES, candies);
            ants[i].move();
            ants[i].try_to_eat(NUMBER_OF_CANDIES, candies);
        }
        int candies_on_board = 0;
        for(int i = 0; i < NUMBER_OF_CANDIES; i++)
            if(!candies[i].has_been_eaten())
                candies_on_board++;
        if(candies_on_board == 0)
            next_level();
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
    ALLEGRO_DISPLAY* disp = al_create_display(WIDTH, HEIGHT);
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
