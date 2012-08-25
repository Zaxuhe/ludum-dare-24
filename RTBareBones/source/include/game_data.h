#ifndef GAME_DATA_H
#define GAME_DATA_H
extern const int n; // horizontal size of the map
extern const int m; // vertical size size of the map
#include "a_star.h"
#include <vector>
#include <bitset>

//jobs:
// 0:Idle

//Settable jobs
// 1:Dig
// 2:Breeding
// 3:Battle
// 4:Mushrooms
// 5:Food
// 6:Attend Queen
// 7:Pick Rain
// 8:Build wall

// 9:Walking - just walking to some place
// 10:Sleeping - mientras duerme se cura la vida, duerme hasta que hp > 50
// 11:Eating
// 12:Drinking

//player and enemies come from here

typedef enum Type {
BLOCK = 0,
TREE_BASE = 1, 
TREE_LEAF = 2,
BREED = 3,
FOOD = 4,
DRINK = 5,
FUNGI = 6,
DIG = 7,
WALL = 8,
WORKING_ON_ME = 9, //if something is woring on it (example dig)
//items
HAS_ITEM = 10,
ITEM_FOOD_1 = 11,
ITEM_FOOD_2 = 12,
ITEM_FOOD_3 = 13,
ITEM_FOOD_4 = 14,
ITEM_FOOD_5 = 15,
ITEM_WATER_1 = 16,
ITEM_WATER_2 = 17,
ITEM_WATER_3 = 18,
ITEM_WATER_4 = 19,
ITEM_WATER_5 = 20,

//jobs
PICK_EGG = 100,
BE_A_BABY = 101, //its a job because you have to be a baby in order to grow :)
SEARCH_FOOD = 102,
SEARCH_DRINK = 103
};
class ant
{
    public:
    ant(bool baby = false)
    {
        current_job = 0; //start as idle
        is_queen = false;
        hp = 100;
        sleepness = 200;
        //hunger = 500;
        //thirst = 400;
        hunger = 600;
        thirst = 500;
        
		holding_item = 0;
        age = 0;
        atack = 1;
        defense = 1;
        x = 0;
        y = 0;
        speed = 7;
		speed_curr = 6;
		job_speed = 1;
		job_time = 900;
		has_egg = false;
		if (baby)
		{
			job_time = 1800;
			current_job = BE_A_BABY;
		}
		sleeping = false;
    }

	void cancel_job();


    //algoritmo de actividades de mis hormigas
    //lista de trabajos
    //busca entre los trabajos que puede hacer, algo que hacer
    //si no encuentra nada que hacer se mueve de manera random
    //(si esta en la superficie solo se mete al hormiguero)
    //si encuentra algo que hacer lo hace (busca el mas cercano)
    //cuando termina ese trabajo PRIMERO intenta hacer ese trabajo otravez
    //si no existe entonces vuelve a empezar el algoritmo

    int hp; //starting at 100, you die at zero
    int sleepness; //starting at 100, go to sleep at 20, sleep anyware at 0, wake up at 100
	bool sleeping;
    int hunger; //100 is full, at 50 it goes for more food, at 0 you die
    int thirst; //100 is full, at 50 it goes for more food, at 0 you die
    int atack;
    int defense;
	int job_time;
	float job_speed; //1 = max time; less then that will make jobs faster

	bool has_egg; //only the queen

    int x;
    int y;
	int next_job_x;
	int next_job_y;

    int holding_item; //0 not holding anything, else holding the id
	//items
	//item = 1 egg
    int age; //from 0 to 10, you are a baby(no jobs and no move), on 100 or 200 you die

    int current_job;
    std::vector<int> jobs_list; //la lista de los trabajos que puedo hacer :)
    a_star path;
	void draw_menu();
    bool is_queen; //es reina? //si es reina solo se mueve si tu le asignas un nuevo queen zone
    int speed; //speed = how many steps it will take to advance one square
	int speed_curr; //the time to the next walk

};


class point2
{
	public:
	point2(int x_, int y_)
	{
		x = x_;
		y = y_;
	};
	point2()
	{
	}
	int x;
	int y;
};

class game_data
{
    public:

    game_data();

    int dx[4];
    int dy[4];
    bitset<32> tiles_map[48][36];
    //map tiles
    //0 = nothing
    //1 = blocking
    //2 = tree tronco
    //4 = tree leaves
    //8 = Breed zone
    //16 = Food Stock
    //32 = Drink Stock
    //64 = Fungi
	//128 = Dig Tile (about to be digged)
	//256 = Create wall (someday we will create a wall)

	bool selecting_first;
	point2 selection_1;
	point2 selection_2;

	
    void generate_map();
	void render_map();
	void init();
	void step();
	void draw_menu();
	void click(float x, float y);
	void move_mouse(float x, float y);

	float mouse_x;
	float mouse_y;

	bool paused;
	int draw_menu_;




    std::vector<ant> ants;

	//Textures
	Surface m_sand; //for testing
	Surface m_sand_digged; //for testing
	Surface m_sky; //for testing
	Surface m_ant; //for testing
	Surface m_ant_sleep; //for testing
	Surface m_wood; //for testing
	Surface m_leaf; //for testing
	Surface m_queen; //for testing
	Surface m_hover; //for testing
	Surface m_breed; //for testing
	Surface m_egg; //for testing
	Surface m_food_1; //for testing
	Surface m_water_1; //for testing

	Surface m_btn_dig; //for testing

};

//singletn
extern game_data g_data;

#endif // GAME_DATA_H
