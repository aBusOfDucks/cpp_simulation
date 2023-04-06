#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <random>
#include <iostream>
#include <vector>

#define WIDTH 1200
#define HEIGHT 1200
#define INITIAL_NUMBER 100
using std::vector;


class ant{
private:
	double x, y;
	int speed;
	int size;
	int range;
	double target_x, target_y;
	double target_dis;
	bool has_eaten;
	bool alive;
public:
	void set(int poz_x, int poz_y, int sp, int si, int ra)
	{
		x = poz_x;
		y = poz_y;
		speed = sp;
		size = si;
		range = ra;
		target_dis = 1000000;
		has_eaten = false;
		alive = true;
	}
	
	void draw()
	{
		if(!alive)
			return;
		al_draw_filled_rectangle(x - size / 16, y - size / 16, x + size / 16, y + size / 16, al_map_rgb(speed, size, range));
	}
	
	void move(int tick)
	{
		if(!alive)
			return;
		if(tick % (17 - speed / 16) != 0)
			return;
		if(target_dis > 10000)
			return;
		
		if(target_dis < size / 32)
			return;
			
		if(target_x  > x)
			x++;
		else
			x--;
		if(target_y > y)
			y++;
		else
			y--;
		target_dis = 1000000;
	}
	
	void find_target(ant a)
	{
		if(!alive || size - a.size < 32 || a.is_dead())
			return;
		
		double dis = (x - a.x) * (x - a.x) + (y - a.y) * (y - a.y);
		dis = sqrt(dis);
		if(dis > range)
			return;
		if(dis < target_dis)
		{
			target_dis = dis;
			target_x = a.x;
			target_y = a.y;
		}
	}
	
	void try_eat(ant & a)
	{
		if(!alive || size - a.size < 32 || a.is_dead())
			return;
		
		double dis = (x - a.x) * (x - a.x) + (y - a.y) * (y - a.y);
		dis = sqrt(dis);	
		if(dis < size / 16)
		{
			has_eaten = true;
			a.die();
		}
	}
	
	void die()
	{
		alive = false;
	}
	
	bool is_dead()
	{
		return !alive;
	}
	
	void end_of_day()
	{
		if(!has_eaten)
			die();
		has_eaten = false;
	}
	
	ant mutate(int a, int b, int c)
	{
		ant temp;
		temp.speed = speed + a;
		temp.size = size + b;
		temp.range = range + c;
		if(temp.speed < 0)
			temp.speed = 0;
		if(temp.size < 0)
			temp.size = 0;
		if(temp.range < 0)
			temp.range = 0;
		while(temp.speed + temp.size + temp.range > 255)
		{
			temp.speed--;
			temp.size--;
			temp.range--;
		}
		temp.has_eaten = false;
		temp.alive = true;
		temp.target_dis = 1000000;
		return temp;
	}
	
	void set_poz(double poz_x, double poz_y)
	{
		x = poz_x;
		y = poz_y;
	}
};

vector <ant> ants;

void init()
{
	al_init();
	std::random_device dev;
	std::mt19937 rng;
	std::uniform_int_distribution<std::mt19937::result_type> poz_x_generator;
	std::uniform_int_distribution<std::mt19937::result_type> poz_y_generator;
	std::uniform_int_distribution<std::mt19937::result_type> char_generator;
	poz_x_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, WIDTH);
	poz_y_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, HEIGHT);
	char_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, 255);
	ant temp;
	for(size_t i = 0; i < INITIAL_NUMBER; i++)
		ants.push_back(temp);
	rng = std::mt19937(dev());
	for(size_t i = 0; i < ants.size(); i++)
	{
		double poz_x = poz_x_generator(rng);
		double poz_y = poz_y_generator(rng);
		int speed = char_generator(rng);
		int size = char_generator(rng);
		int range = char_generator(rng);
		while(speed + size + range > 255)
		{
			speed--;
			size--;
			range--;
		}
		ants[i].set(poz_x, poz_y, speed, size, range);
	}
}

void draw()
{
	al_clear_to_color(al_map_rgb(0, 0, 0));

	for(size_t i = 0; i < ants.size(); i++)
	{
		ants[i].draw();
	}

	al_flip_display();
}

int main()
{
	init();
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	ALLEGRO_DISPLAY* disp = al_create_display(WIDTH, HEIGHT);
	al_init_primitives_addon();
	draw();
	int tick = 0;
	std::random_device dev;
	std::mt19937 rng;
	std::uniform_int_distribution<std::mt19937::result_type> poz_x_generator;
	std::uniform_int_distribution<std::mt19937::result_type> poz_y_generator;
	std::uniform_int_distribution<std::mt19937::result_type> mutation_generator;
	poz_x_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, WIDTH);
	poz_y_generator = std::uniform_int_distribution<std::mt19937::result_type>(0, HEIGHT);
	mutation_generator = std::uniform_int_distribution<std::mt19937::result_type>(-5, 5);
	while(true)
	{
		int number_of_berries = ants.size();
		while(number_of_berries)
		{
			for(size_t i = 0; i < ants.size(); i++)
			{
				ants[i].move(tick);
			}
			for(size_t i = 0; i < ants.size(); i++)
			{
				for(size_t j = i + 1; j < ants.size(); j++)
				{
					ants[i].try_eat(ants[j]);
					ants[j].try_eat(ants[i]);
				}
			}
			tick++;
			tick %= 64;
			for(size_t i = 0; i < ants.size(); i++)
			{
				for(size_t j = i + 1; j < ants.size(); j++)
				{
					ants[i].find_target(ants[j]);
					ants[j].find_target(ants[i]);
				}
			}
			draw();
			usleep(5000);
		}
		vector <ant> temp;
		for(size_t i = 0; i < ants.size(); i++)
		{
			ants[i].end_of_day();
			if(!ants[i].is_dead())
			{
				for(int j = 0; j < 8; j++)
				{
					int a = mutation_generator(rng);
					int b = mutation_generator(rng);
					int c = mutation_generator(rng);
					temp.push_back(ants[i].mutate(a, b, c));
				}
			}	
		}
		std::cout << ants.size() << "\n";
		ants = temp;
		std::cout << ants.size() << "\n";
		for(size_t i = 0; i < ants.size(); i++)
		{
			double x = poz_x_generator(rng);
			double y = poz_y_generator(rng);
			ants[i].set_poz(x, y);
		}
	}
	al_destroy_display(disp);
	return 0;
}
