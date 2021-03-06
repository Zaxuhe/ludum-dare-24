#include "PlatformPrecomp.h"
#include "../include/game_data.h"

game_data g_data;

void game_data::generate_map()
{
    srand(time(NULL));
    // create empty map
    for(int y=0;y<m;y++)
    {
        for(int x=0;x<n;x++) g_data.tiles_map[x][y]=0;
    }

   //fill (generate) the map (we just let walk on border between the ground and the sky)
	for (int x_pos = 0; x_pos < 48; x_pos++)
	{
		for (int y_pos = 0; y_pos < 36; y_pos++)
		{
			if (y_pos < 11)
			{
				//m_sky.Bind();
				g_data.tiles_map[x_pos][y_pos][BLOCK] = 1;
			}
			if (y_pos == 11)
			{
				//m_sky.Bind();
				g_data.tiles_map[x_pos][y_pos][BLOCK] = 0;
			}
			else
			{
				//m_sand.Bind();
				g_data.tiles_map[x_pos][y_pos][BLOCK] = 1;
			}
		}
	}

    //we generate a tree
    int tree_base_x = rand()%35 + 8;
    int tree_base_y = rand()%3 + 5;

    int tree_width = rand()%3 + 5;
    if (tree_width%2 != 0)
    {
        tree_width += 1;
    }
    //g_data.tiles_map[tree_base_x][tree_base_y]=0;
    //hacemos el tronco
    for (int tree_base_y_ = 11; tree_base_y_ > tree_base_y; tree_base_y_--)
    {
        g_data.tiles_map[tree_base_x ][tree_base_y_][TREE_BASE] = 1;
    }
    //ahora hacemos el contenido del arbol
	for (int x_pos_ = tree_base_x-tree_width/2; x_pos_ < tree_base_x+tree_width/2; x_pos_++)
	{
		for (int y_pos_ = tree_base_y; y_pos_ > tree_base_y-5; y_pos_--)
		{
			g_data.tiles_map[x_pos_][y_pos_][TREE_LEAF] = 1;
		}
	}


    //we search for the center of the queen zone
    //This is the starting queen pos
	int y_pos = rand()%10 + 7 + 11;
    int x_pos = rand()%35 + 5;
    //desde la posicion hasta arriba hacemos paredes
    for (int y_pos_ = y_pos; y_pos_> 11; y_pos_--)
    {
        g_data.tiles_map[x_pos][y_pos_][BLOCK] = 0;
    }
    //ahora agregamos alrededor
	for (int x_pos_ = x_pos-1; x_pos_ < x_pos+2; x_pos_++)
	{
		for (int y_pos_ = y_pos-1; y_pos_ < y_pos+2; y_pos_++)
		{
            g_data.tiles_map[x_pos_][y_pos_][BLOCK] = 0;
		}
	}

	//now we add the ants
	//on the vector [0] is always the queen
	ant a;
	a.x = 2;//x_pos;
	a.y = 11;//y_pos;
	a.is_queen = true;
	a.path.pathFind(2,11,x_pos,y_pos);
	ants.push_back(a);

	ant a2;
	a2.x = 1;
	a2.y = 11;
	a2.path.pathFind(1,11,x_pos+1,y_pos);
	ants.push_back(a2);

	ant a3;
	a3.x = 0;
	a3.y = 11;
	a3.path.pathFind(0,11,x_pos-1,y_pos);
	ants.push_back(a3);

	//we also add the food
	g_data.tiles_map[x_pos+1][y_pos-1][ITEM_FOOD_5] = 1;
	g_data.tiles_map[x_pos+1][y_pos-1][HAS_ITEM] = 1;
	g_data.tiles_map[x_pos-1][y_pos-1][ITEM_FOOD_5] = 1;
	g_data.tiles_map[x_pos-1][y_pos-1][HAS_ITEM] = 1;
	g_data.tiles_map[x_pos][y_pos-1][ITEM_FOOD_5] = 1;
	g_data.tiles_map[x_pos][y_pos-1][HAS_ITEM] = 1;
	//the water
	g_data.tiles_map[x_pos+1][y_pos+1][ITEM_WATER_5] = 1;
	g_data.tiles_map[x_pos+1][y_pos+1][HAS_ITEM] = 1;
	g_data.tiles_map[x_pos-1][y_pos+1][ITEM_WATER_5] = 1;
	g_data.tiles_map[x_pos-1][y_pos+1][HAS_ITEM] = 1;
	g_data.tiles_map[x_pos][y_pos+1][ITEM_WATER_5] = 1;
	g_data.tiles_map[x_pos][y_pos+1][HAS_ITEM] = 1;

}

void game_data::render_map()
{
    // display the map with the route

	for (int x_pos = 0; x_pos < 48; x_pos++)
	{
		for (int y_pos =35; y_pos >= 0; y_pos--)
		{
			 //we always draw the sky
	 		 if (y_pos < 12)
			 {
				m_sky.BlitScaled( (x_pos*24)+12,(y_pos*24)+ 12, CL_Vec2f (0.75,0.75));
			 }

			 if(g_data.tiles_map[x_pos][y_pos].test(BLOCK) == true) //nothing
			 {
				 if (y_pos > 11)
				 {
					m_sand_digged.BlitScaled( (x_pos*24)+12,(y_pos*24)+ 12, CL_Vec2f (0.75,0.75));
				 }
			 }
 			 else //if(g_data.tiles_map[x_pos][y_pos].test(BLOCK) == false) //block
			 {
				 if (y_pos > 11)
				 {
					m_sand.BlitScaled( (x_pos*24)+12,(y_pos*24)+ 12, CL_Vec2f (0.75,0.75));
				 }
			 }
  			 if(g_data.tiles_map[x_pos][y_pos].test(TREE_BASE) == true) //wood
			 {
				m_wood.BlitScaled( (x_pos*24)+12,(y_pos*24)+ 12, CL_Vec2f (0.75,0.75));
			 }
   			 if(g_data.tiles_map[x_pos][y_pos].test(TREE_LEAF) == true) //leaf
			 {
				m_leaf.BlitScaled( (x_pos*24)+12,(y_pos*24)+ 12, CL_Vec2f (0.75,0.75));
			 }
			 if(g_data.tiles_map[x_pos][y_pos].test(DIG) == true) //Dig
			 {
				m_hover.Blit( (x_pos*24),(y_pos*24),MAKE_RGBA(255,255,255,255*0.5));
			 }
 			 if(g_data.tiles_map[x_pos][y_pos].test(BREED) == true) //Breed
			 {
				m_breed.Blit( (x_pos*24),(y_pos*24),MAKE_RGBA(255,255,255,255*0.5));
			 }
 			 if(g_data.tiles_map[x_pos][y_pos].test(FOOD) == true) //Breed
			 {
				m_food_zone.Blit( (x_pos*24),(y_pos*24),MAKE_RGBA(255,255,255,255*0.5));
			 }
 			 if(g_data.tiles_map[x_pos][y_pos].test(DRINK) == true) //Breed
			 {
				m_water_zone.Blit( (x_pos*24),(y_pos*24),MAKE_RGBA(255,255,255,255*0.5));
			 }
			 if(g_data.tiles_map[x_pos][y_pos].test(ITEM_FOOD_1) == true ||
				 g_data.tiles_map[x_pos][y_pos].test(ITEM_FOOD_2) == true ||
				 g_data.tiles_map[x_pos][y_pos].test(ITEM_FOOD_3) == true || 
				 g_data.tiles_map[x_pos][y_pos].test(ITEM_FOOD_4) == true ||
				 g_data.tiles_map[x_pos][y_pos].test(ITEM_FOOD_5) == true) //Breed
			 {
				m_food_1.Blit( (x_pos*24),(y_pos*24));
			 }
  			 if(g_data.tiles_map[x_pos][y_pos].test(ITEM_WATER_1) == true ||
				 g_data.tiles_map[x_pos][y_pos].test(ITEM_WATER_2) == true ||
				 g_data.tiles_map[x_pos][y_pos].test(ITEM_WATER_3) == true ||
				 g_data.tiles_map[x_pos][y_pos].test(ITEM_WATER_4) == true ||
				 g_data.tiles_map[x_pos][y_pos].test(ITEM_WATER_5) == true) //Breed
			 {
				m_water_1.Blit( (x_pos*24),(y_pos*24));
			 }
			 if (g_data.tiles_map[x_pos][y_pos].test(RAIN) == true)
			 {

				 m_water_1.Blit( (x_pos*24),(y_pos*24));
				 if (y_pos != 11 && rain_time % 25)
				 {
					g_data.tiles_map[x_pos][y_pos][RAIN] = 0;
					g_data.tiles_map[x_pos][y_pos+1][RAIN] = 1;
				 }
				 if (y_pos == 11)
				 {
					 //convert it to water
					 g_data.tiles_map[x_pos][y_pos][RAIN] = 0;
					 g_data.tiles_map[x_pos][y_pos][ITEM_WATER_5] = 1;
				 }
			 }
			 if(g_data.tiles_map[x_pos][y_pos].test(HP_LOSS_1) == true ||
				 g_data.tiles_map[x_pos][y_pos].test(HP_LOSS_2) == true ||
				 g_data.tiles_map[x_pos][y_pos].test(HP_LOSS_3) == true ||
				 g_data.tiles_map[x_pos][y_pos].test(HP_LOSS_4) == true ||
				 g_data.tiles_map[x_pos][y_pos].test(HP_LOSS_5) == true) //HP_LOSS
			 {
				 m_hp_loss.Blit( (x_pos*24),(y_pos*24));
				 if (g_data.tiles_map[x_pos][y_pos][HP_LOSS_1] == 1)
				 {
					 g_data.tiles_map[x_pos][y_pos][HP_LOSS_1] = 0;
				 }
				 else if (g_data.tiles_map[x_pos][y_pos][HP_LOSS_2] == 1)
				 {
					 g_data.tiles_map[x_pos][y_pos][HP_LOSS_2] = 0;
					 g_data.tiles_map[x_pos][y_pos][HP_LOSS_1] = 1;
				 }
				 else if (g_data.tiles_map[x_pos][y_pos][HP_LOSS_3] == 1)
				 {
					 g_data.tiles_map[x_pos][y_pos][HP_LOSS_3] = 0;
					 g_data.tiles_map[x_pos][y_pos][HP_LOSS_2] = 1;
				 }
				 else if (g_data.tiles_map[x_pos][y_pos][HP_LOSS_4] == 1)
				 {
					 g_data.tiles_map[x_pos][y_pos][HP_LOSS_4] = 0;
					 g_data.tiles_map[x_pos][y_pos][HP_LOSS_3] = 1;
				 }
				 else if (g_data.tiles_map[x_pos][y_pos][HP_LOSS_5] == 1)
				 {
					 g_data.tiles_map[x_pos][y_pos][HP_LOSS_5] = 0;
					 g_data.tiles_map[x_pos][y_pos][HP_LOSS_4] = 1;
				 }
				 
			 }
			 
		}
	}

	for (int i = 0; i < enemies_.size(); i++)
	{
		if (enemies_[i].type == 0)
		{
			m_bug.BlitScaled( (enemies_[i].x*24)+12,(enemies_[i].y*24)+ 12, CL_Vec2f (1.5,1.5));
		}
		if (enemies_[i].type == 1)
		{
			m_bug2.BlitScaled( (enemies_[i].x*24)+12,(enemies_[i].y*24)+ 12, CL_Vec2f (1.5,1.5));
		}
		if (enemies_[i].is_under_atack == true)
		{
			m_atack.Blit( (enemies_[i].x*24),(enemies_[i].y*24),MAKE_RGBA(255,255,255,255*0.5));
		}
	}

	for (int i = 0; i < ants.size(); i++)
	{
		if (ants[i].is_queen == true)
		{
			m_queen.BlitScaled( (ants[i].x*24)+12,(ants[i].y*24)+ 12, CL_Vec2f (1.5,1.5));
			if (ants[i].has_egg)
			{
				m_egg.Blit( (ants[i].x*24),(ants[i].y*24));
			}
		}
		else
		{
			if (ants[i].current_job == BE_A_BABY)
			{
				m_egg.Blit( (ants[i].x*24),(ants[i].y*24));
			}
			else
			{
				if (ants[i].sleeping == true)
				{
					m_ant_sleep.BlitScaled( (ants[i].x*24)+12,(ants[i].y*24)+ 12, CL_Vec2f (1.5,1.5));
				}
				else
				{
					m_ant.BlitScaled( (ants[i].x*24)+12,(ants[i].y*24)+ 12, CL_Vec2f (1.5,1.5));
				}
			}
			if (ants[i].holding_item == 1)
			{
				m_egg.Blit( (ants[i].x*24),(ants[i].y*24));
			}
			else if (ants[i].holding_item == 2 || ants[i].holding_item == 3 || ants[i].holding_item == 4 || ants[i].holding_item == 5 || ants[i].holding_item == 6)
			{
				m_food_1.Blit( (ants[i].x*24),(ants[i].y*24));
			}
			else if (ants[i].holding_item == 7 || ants[i].holding_item == 8 || ants[i].holding_item == 9 || ants[i].holding_item == 10 || ants[i].holding_item == 11)
			{
				m_water_1.Blit( (ants[i].x*24),(ants[i].y*24));
			}
		}

	}



}

void ant::cancel_job()
{
	if (current_job == DIG || current_job == BREED || current_job == PICK_EGG)
	{
		g_data.tiles_map[next_job_x][next_job_y][WORKING_ON_ME] = 0;

	}
	else if (current_job == PICK_FOOD || current_job == PICK_DRINK)
	{
		g_data.tiles_map[next_job_x2][next_job_y2][WORKING_ON_ME] = 0;
	}
	current_job = 0;
	
}

void game_data::enemy_step()
{
	for (int i = 0; i < enemies_.size(); i++)
	{
		if (enemies_[i].hp == 0)
		{
			g_data.tiles_map[enemies_[i].x][enemies_[i].y][ITEM_FOOD_5] = 1;
			g_data.tiles_map[enemies_[i].x][enemies_[i].y][HAS_ITEM] = 1;
			enemies_.erase(enemies_.begin()+i);
			enemies_killed++;
			i--;
			continue;
		}

		if (enemies_[i].path.movement_.length() == 0 || enemies_[i].path.current_postion_>=enemies_[i].path.movement_.length())
		{
			enemies_[i].path.movement_ = "";
			enemies_[i].path.current_postion_ = 0;
			//we have reached the position
		}
		else
		{
			if (enemies_[i].speed_curr == 0)
			{
				int j; char c;
				c =enemies_[i].path.movement_.at(enemies_[i].path.current_postion_);
				j=atoi(&c);
				enemies_[i].x=enemies_[i].x+g_data.dx[j];
				enemies_[i].y=enemies_[i].y+g_data.dy[j];
				enemies_[i].path.current_postion_++;
				enemies_[i].speed_curr = enemies_[i].speed;
			}
			else
			{
				enemies_[i].speed_curr--;
			}

		}

		//we search for a ant player to atack
		std::string mov;
		std::string best_mov;
		for (int u = 0; u < 100; u++)
		{
			best_mov += "a";
		}
		bool atacked = false;
		for (int z = 0; z < ants.size(); z++)
		{
			if (atacked == true)
			{
				continue;
			}
			if ((ants[z].x == enemies_[i].x-1 && ants[z].y == enemies_[i].y) ||
				(ants[z].x == enemies_[i].x+1  && ants[z].y == enemies_[i].y) ||
				(ants[z].x == enemies_[i].y-1 && ants[z].x == enemies_[i].x) || 
				(ants[z].x == enemies_[i].y+1 && ants[z].x == enemies_[i].x) ||
				(ants[z].x == enemies_[i].x && ants[z].y == enemies_[i].y)
				)
			{
				//we hit the ant
				if (enemies_[i].atack_speed_curr <= 0)
				{
					g_data.tiles_map[ants[z].x][ants[z].y+1][HP_LOSS_5] = 1;
					ants[z].hp-= enemies_[i].atack;
					if (ants[z].hp == 0)
					{
						g_data.tiles_map[ants[z].x][ants[z].y][ITEM_FOOD_5] = 1;
						g_data.tiles_map[ants[z].x][ants[z].y][HAS_ITEM] = 1;
						ants.erase(ants.begin()+z);
						z--;
					}
					enemies_[i].atack_speed_curr = enemies_[i].atack_speed;
				}
				else
				{
					enemies_[i].atack_speed_curr--;
				}
				enemies_[i].path.movement_ = "";
				best_mov = "a";
				break;
			}
			else if (enemies_[i].type == 0)//only on surface atack
			{
				if (ants[z].y == 11)
				{
					mov = enemies_[i].path.pathFind(enemies_[i].x, enemies_[i].y, ants[z].x+1, ants[z].y);
					if (mov.length() < best_mov.length() && mov.length() > 0)
					{
						best_mov = mov;
					}
					mov = enemies_[i].path.pathFind(enemies_[i].x, enemies_[i].y, ants[z].x-1, ants[z].y);
					if (mov.length() < best_mov.length() && mov.length() > 0)
					{
						best_mov = mov;
					}

				}
			}
			else if (enemies_[i].type == 1)//anyware
			{
				mov = enemies_[i].path.pathFind(enemies_[i].x, enemies_[i].y, ants[z].x+1, ants[z].y);
				if (mov.length() < best_mov.length() && mov.length() > 0)
				{
					best_mov = mov;
				}
				mov = enemies_[i].path.pathFind(enemies_[i].x, enemies_[i].y, ants[z].x-1, ants[z].y);
				if (mov.length() < best_mov.length() && mov.length() > 0)
				{
					best_mov = mov;
				}
				mov = enemies_[i].path.pathFind(enemies_[i].x, enemies_[i].y, ants[z].x, ants[z].y+1);
				if (mov.length() < best_mov.length() && mov.length() > 0)
				{
					best_mov = mov;
				}
				mov = enemies_[i].path.pathFind(enemies_[i].x, enemies_[i].y, ants[z].x, ants[z].y-1);
				if (mov.length() < best_mov.length() && mov.length() > 0)
				{
					best_mov = mov;
				}
			}
		}
		if (best_mov.length() > 0 && best_mov[0] != 'a')
		{
			enemies_[i].path.movement_ = best_mov;
		}

		if (enemies_[i].path.movement_.length() == 0)
		{
			//we move on a random way
			if (enemies_[i].type == 0) //on x axis
			{
				int move_way = rand()%(2);
				if (move_way == 0 && enemies_[i].x > 0)
				{
					mov = enemies_[i].path.pathFind(enemies_[i].x, enemies_[i].y, enemies_[i].x-1, enemies_[i].y);
				}
				else if (move_way == 1 && enemies_[i].x < 46)
				{
					mov = enemies_[i].path.pathFind(enemies_[i].x, enemies_[i].y, enemies_[i].x+1, enemies_[i].y);
				}
			}
			//the other dosent need rand because ti
		}
		//if no player found we move randomly
	}
}

//one step on the game
void game_data::step()
{
	if (paused)
	{
		return;
	}
	//Add enemy
	int time_to_add = (2000-enemies_killed*10);
	if (time_to_add < 600)
	{
		time_to_add = 600;
	}
	if (enemy_time % time_to_add == 0 && enemy_time != 0)
	{
		add_enemy_floor();
	}
	enemy_time ++;
	//Rain calculation
	if (rain_time == 0)
	{
		//we start to rain, 4 to 12 drops
		int rain_drops = rand()%(16-4) + 4;
		std::vector<int> drops;
		
		while (rain_drops > 0)
		{
			int next_drop =rand()%45 + 1; // 1 to 46
			bool found_drop = false;
			for (int i = 0;i < drops.size(); i++)
			{
				if (drops[i] == next_drop)
				{
					found_drop = true;
				}
			}
			if (found_drop == false)
			{
				drops.push_back(next_drop);
				rain_drops--;
			}
		}
		for (int i = 0;i < drops.size(); i++)
		{
			g_data.tiles_map[drops[i]][0][RAIN] = 1;
		}

		rain_time = 6000;

	}
	else
	{
		rain_time--;
	}

	//we make the bugs walk
	enemy_step();


	//we make the ants walk
	for (int i = 0; i < ants.size(); i++)
	{
		//just for the atack
		bool is_atacking = false;
		if (ants[i].current_job == 0 && ants[i].is_queen == false)
		{
			//we look for something to atack
			for (int z = 0; z < enemies_.size(); z++)
			{
				if (enemies_[z].is_under_atack)
				{
					//LogMsg("Aqui");
					if ((ants[i].x == enemies_[z].x-1 && ants[i].y == enemies_[z].y) ||
					(ants[i].x == enemies_[z].x+1  && ants[i].y == enemies_[z].y) ||
					(ants[i].x == enemies_[z].y-1 && ants[i].x == enemies_[z].x) || 
					(ants[i].x == enemies_[z].y+1 && ants[i].x == enemies_[z].x) ||
					(ants[i].x == enemies_[z].x && ants[i].y == enemies_[z].y)
					)
					{
						//LogMsg("HIT");
						if (ants[i].satack_curr == 0)
						{
							enemies_[z].hp -= ants[i].atack;
							ants[i].satack_curr = ants[i].satack;
						}
						else
						{
							ants[i].satack_curr --;
						}
						ants[i].path.movement_ = "";
						is_atacking = true;
						break;
					}
					is_atacking = true;
					//we atack
					std::string best_mov;
					for (int u = 0; u < 10000; u++)
					{
						best_mov += "a";
					}
					std::string mov;
					//vamos al huevo y lo agarramos
					if (enemies_[z].x-1 > 0)
					{
						mov = ants[i].path.pathFind(ants[i].x, ants[i].y, enemies_[z].x-1, enemies_[z].y);
						if (mov.length() < best_mov.length() && mov.length() > 0)
						{
							best_mov = mov;
							ants[i].cancel_job();
						}
					}
					mov = ants[i].path.pathFind(ants[i].x, ants[i].y, enemies_[z].x+1, enemies_[z].y);
					if (mov.length() < best_mov.length() && mov.length() > 0)
					{
						best_mov = mov;
						ants[i].cancel_job();
					}
					mov = ants[i].path.pathFind(ants[i].x, ants[i].y, enemies_[z].x, enemies_[z].y-1);
					if (mov.length() < best_mov.length() && mov.length() > 0)
					{
						best_mov = mov;
						ants[i].cancel_job();
					}
					mov = ants[i].path.pathFind(ants[i].x, ants[i].y, enemies_[z].x, enemies_[z].y+1);
					if (mov.length() < best_mov.length() && mov.length() > 0)
					{
						best_mov = mov;
						ants[i].cancel_job();
					}
					if (best_mov[0] != 'a')
					{
						ants[i].path.movement_ = best_mov;
					}
					ants[i].current_job = 0;
					break;
				}
			}
		}
			//if its thirst or hunger, it will stop every action and will go to eat or drink
			//if its sleeping it will sleep
			if (ants[i].sleepness < 10 && ants[i].sleeping == false)
			{
				ants[i].sleeping = true;
				continue;
			}
			else if (ants[i].sleeping == true)
			{
				ants[i].sleepness++;
				ants[i].hp++;
				if (ants[i].hp > 100)
				{
					ants[i].hp = 0;
				}
				if (ants[i].sleepness == 1500)
				{
					ants[i].sleepness = 400;
					ants[i].sleeping = false;
				}
				continue;
			}
			//WEE NEED TO FEEd
			if (ants[i].hunger < 100 && ants[i].current_job != SEARCH_FOOD && ants[i].current_job != SEARCH_DRINK && ants[i].current_job != DROP_FOOD && ants[i].current_job != DROP_DRINK)
			{
				//I HAVE TO FIND SOMETHING TO EATTT
				std::string best_mov;
				for (int u = 0; u < 10000; u++)
				{
					best_mov += "a";
				}
				std::string mov;
				for (int x_pos = 0; x_pos < 48; x_pos++)
				{
					for (int y_pos = 12; y_pos < 36; y_pos++)
					{
						if (g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_1] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_2] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_3] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_4] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_5] == 1
							)//we have found foood!
						{
							//LogMsg("Econtre comidaaa :)");
							//vamos a la comida la agarramos
							mov = ants[i].path.pathFind(ants[i].x, ants[i].y, x_pos, y_pos);
							if (mov.length() < best_mov.length() && mov.length() > 0)
							{
								best_mov = mov;
								ants[i].cancel_job();
								ants[i].current_job = SEARCH_FOOD;
								//work_pos_x = ants[0].x;
								//work_pos_y = ants[0].y;
								ants[i].next_job_x = ants[0].x;
								ants[i].next_job_y = ants[0].y;
								ants[i].job_time = 30*ants[i].job_speed;
							}
						}
					}
				}

				if (best_mov.length() > 0 && best_mov[0] != 'a')
				{
					ants[i].path.movement_ = best_mov;
				}
			}
			//WEE NEED TO DRINK
			if (ants[i].thirst < 100 && ants[i].current_job != SEARCH_DRINK && ants[i].current_job != SEARCH_FOOD && ants[i].current_job != DROP_FOOD && ants[i].current_job != DROP_DRINK)
			{
				//I HAVE TO FIND SOMETHING TO EATTT
				std::string best_mov;
				for (int u = 0; u < 10000; u++)
				{
					best_mov += "a";
				}
				std::string mov;
				for (int x_pos = 0; x_pos < 48; x_pos++)
				{
					for (int y_pos = 12; y_pos < 36; y_pos++)
					{
						if (g_data.tiles_map[x_pos][y_pos][ITEM_WATER_1] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_2] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_3] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_4] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_5] == 1
							)//we have found foood!
						{
							//we need to cancel the current job
							//LogMsg("Econtre comidaaa :)");
							//vamos a la comida la agarramos
							mov = ants[i].path.pathFind(ants[i].x, ants[i].y, x_pos, y_pos);
							if (mov.length() < best_mov.length() && mov.length() > 0)
							{
								best_mov = mov;
								ants[i].cancel_job();
								ants[i].current_job = SEARCH_DRINK;
								//work_pos_x = ants[0].x;
								//work_pos_y = ants[0].y;
								ants[i].next_job_x = ants[0].x;
								ants[i].next_job_y = ants[0].y;
								ants[i].job_time = 30*ants[i].job_speed;
							}
						}
					}
				}

				if (best_mov.length() > 0 && best_mov[0] != 'a')
				{
					ants[i].path.movement_ = best_mov;
				}
			}
		//its moving that means is going to do something
	    if(ants[i].path.movement_.length()>0 || ants[i].current_job != 0)
		{
			if (ants[i].path.movement_.length() == 0 || ants[i].path.current_postion_>=ants[i].path.movement_.length())
			{
				ants[i].path.movement_ = "";
				ants[i].path.current_postion_ = 0;
				//we have reached to te position :) if we have a job perform it!
				if (ants[i].current_job == DIG)
				{
					if (ants[i].job_time == 0)
					{
						g_data.tiles_map[ants[i].next_job_x][ants[i].next_job_y][DIG] = 0;
						g_data.tiles_map[ants[i].next_job_x][ants[i].next_job_y][BLOCK] = 0;
						g_data.tiles_map[ants[i].next_job_x][ants[i].next_job_y][WORKING_ON_ME] = 0;
						ants[i].current_job = 0;
						ants[i].next_job_x = 0;
						ants[i].next_job_y = 0;
					}
					else
					{
						ants[i].job_time--;
					}

				}
				else if (ants[i].current_job == BE_A_BABY)
				{
					if (ants[i].job_time == 0)
					{
						g_data.tiles_map[ants[i].x][ants[i].y][WORKING_ON_ME] = 0;
						//o mejor hacemos una nueva hormiga
						g_data.tiles_map[ants[i].x][ants[i].y][HAS_ITEM] = 0;
						//g_data.tiles_map[ants[i].x][ants[i].y][ITEM] = 1;
						ants[i].current_job = 0;
					}
					else
					{
						ants[i].job_time--;
					}
				}
				else if (ants[i].current_job == BREED)
				{
					if (ants[i].job_time == 0)
					{
						g_data.tiles_map[ants[i].next_job_x][ants[i].next_job_y][WORKING_ON_ME] = 0;
						//o mejor hacemos una nueva hormiga
						g_data.tiles_map[ants[i].next_job_x][ants[i].next_job_y][HAS_ITEM] = 1;
						//g_data.tiles_map[ants[i].next_job_x][ants[i].next_job_y][ITEM_EGG] = 1;
						ant baby(true);
						baby.x = ants[i].next_job_x;
						baby.y = ants[i].next_job_y;
						ants.push_back(baby);
						ants[i].holding_item = 0;
						ants[i].current_job = 0;
						ants[i].next_job_x = 0;
						ants[i].next_job_y = 0;
					}
					else
					{
						ants[i].job_time--;
					}
				}
				else if (ants[i].current_job == SEARCH_FOOD)
				{
					if (ants[i].job_time == 0)
					{
						if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_1] ||
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_2] ||
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_3] ||
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_4] ||
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_5]
							)
						{
							if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_1])
							{
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_1] = 0;
							}
							else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_2])
							{
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_2] = 0;
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_1] = 1;
							}
							else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_3])
							{
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_3] = 0;
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_2] = 1;
							}
							else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_4])
							{
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_4] = 0;
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_3] = 1;
							}
							else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_5])
							{
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_5] = 0;
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_4] = 1;
							}
							ants[i].hunger = 400; //max
						}
						ants[i].current_job = 0;
						ants[i].next_job_x = 0;
						ants[i].next_job_y = 0;
					}
					else
					{
						ants[i].job_time--;
					}
				}
				else if (ants[i].current_job == PICK_DRINK)
				{
					if (ants[i].job_time == 0)
					{
						//there is no longer food
						g_data.tiles_map[ants[i].x][ants[i].y][WORKING_ON_ME] = 0;
						if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_1])
						{
							ants[i].holding_item = 7;
						}
						else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_2])
						{
							ants[i].holding_item = 8;
						}
						else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_3])
						{
							ants[i].holding_item = 9;
						}
						else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_4])
						{
							ants[i].holding_item = 10;
						}
						else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_5])
						{
							ants[i].holding_item = 11;
						}
						g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_1] = 0;
						g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_2] = 0;
						g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_3] = 0;
						g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_4] = 0;
						g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_5] = 0;
						g_data.tiles_map[ants[i].x][ants[i].y][HAS_ITEM] = 0;
						ants[i].path.pathFind(ants[i].x, ants[i].y, ants[i].next_job_x, ants[i].next_job_y);
						ants[i].current_job = DROP_DRINK;
						ants[i].job_time = 30 * ants[i].job_speed;
					}
					else
					{
						ants[i].job_time--;
					}
				}
				else if (ants[i].current_job == PICK_FOOD)
				{
					if (ants[i].job_time == 0)
					{
						//there is no longer food
						g_data.tiles_map[ants[i].x][ants[i].y][WORKING_ON_ME] = 0;
						if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_1])
						{
							ants[i].holding_item = 2;
						}
						else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_2])
						{
							ants[i].holding_item = 3;
						}
						else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_3])
						{
							ants[i].holding_item = 4;
						}
						else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_4])
						{
							ants[i].holding_item = 5;
						}
						else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_5])
						{
							ants[i].holding_item = 6;
						}
						g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_1] = 0;
						g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_2] = 0;
						g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_3] = 0;
						g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_4] = 0;
						g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_5] = 0;
						g_data.tiles_map[ants[i].x][ants[i].y][HAS_ITEM] = 0;
						ants[i].path.pathFind(ants[i].x, ants[i].y, ants[i].next_job_x, ants[i].next_job_y);
						ants[i].current_job = DROP_FOOD;
						ants[i].job_time = 30 * ants[i].job_speed;
					}
					else
					{
						ants[i].job_time--;
					}
				}
				else if (ants[i].current_job == DROP_DRINK)
				{
					if (ants[i].job_time == 0)
					{
						if (ants[i].holding_item == 7)
						{
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_1] = 1;
						}
						else if (ants[i].holding_item == 8)
						{
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_2] = 1;
						}
						else if (ants[i].holding_item == 9)
						{
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_3] = 1;
						}
						else if (ants[i].holding_item == 10)
						{
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_4] = 1;
						}
						else if (ants[i].holding_item == 11)
						{
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_5] = 1;
						}
						g_data.tiles_map[ants[i].x][ants[i].y][WORKING_ON_ME] = 0;
						g_data.tiles_map[ants[i].x][ants[i].y][HAS_ITEM] = 1;
						ants[i].holding_item = 0;
						ants[i].current_job = 0;
						ants[i].next_job_x = 0;
						ants[i].next_job_y = 0;
					}
					else
					{
						ants[i].job_time--;
					}
				}
				else if (ants[i].current_job == DROP_FOOD)
				{
					if (ants[i].job_time == 0)
					{
						if (ants[i].holding_item == 2)
						{
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_1] = 1;
						}
						else if (ants[i].holding_item == 3)
						{
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_2] = 1;
						}
						else if (ants[i].holding_item == 4)
						{
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_3] = 1;
						}
						else if (ants[i].holding_item == 5)
						{
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_4] = 1;
						}
						else if (ants[i].holding_item == 6)
						{
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_5] = 1;
						}
						g_data.tiles_map[ants[i].x][ants[i].y][WORKING_ON_ME] = 0;
						g_data.tiles_map[ants[i].x][ants[i].y][HAS_ITEM] = 1;
						ants[i].holding_item = 0;
						ants[i].current_job = 0;
						ants[i].next_job_x = 0;
						ants[i].next_job_y = 0;
					}
					else
					{
						ants[i].job_time--;
					}
				}
				else if (ants[i].current_job == SEARCH_DRINK)
				{
					if (ants[i].job_time == 0)
					{
						if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_1] ||
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_2] ||
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_3] ||
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_4] ||
							g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_5]
							)
						{
							if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_1])
							{
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_1] = 0;
							}
							else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_2])
							{
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_2] = 0;
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_1] = 1;
							}
							else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_3])
							{
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_3] = 0;
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_2] = 1;
							}
							else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_4])
							{
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_4] = 0;
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_3] = 1;
							}
							else if (g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_5])
							{
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_5] = 0;
								g_data.tiles_map[ants[i].x][ants[i].y][ITEM_WATER_4] = 1;
							}
							ants[i].thirst =300; //max
						}
						ants[i].current_job = 0;
						ants[i].next_job_x = 0;
						ants[i].next_job_y = 0;
					}
					else
					{
						ants[i].job_time--;
					}
				}
				else if (ants[i].current_job ==PICK_EGG)
				{
					if (ants[i].job_time == 0)
					{
						g_data.tiles_map[ants[i].next_job_x][ants[i].next_job_y][WORKING_ON_ME] = 0;
						ants[0].has_egg = false;
						ants[i].holding_item = 1;
						ants[i].current_job = 0;
						ants[i].next_job_x = 0;
						ants[i].next_job_y = 0;
					}
					else
					{
						ants[i].job_time--;
					}

				}
			}
			else
			{
				if (ants[i].speed_curr == 0)
				{
					int j; char c;
					c =ants[i].path.movement_.at(ants[i].path.current_postion_);
					j=atoi(&c);
					ants[i].x=ants[i].x+g_data.dx[j];
					ants[i].y=ants[i].y+g_data.dy[j];
					ants[i].path.current_postion_++;
					ants[i].speed_curr = ants[i].speed;
					ants[i].hunger--;
					ants[i].thirst--;
					ants[i].sleepness--;
					if (ants[i].hunger == 0 || ants[i].thirst == 0)
					{
						g_data.tiles_map[ants[i].x][ants[i].y][ITEM_FOOD_5] = 1;
						g_data.tiles_map[ants[i].x][ants[i].y][HAS_ITEM] = 1;
						ants.erase(ants.begin()+i);
						i--;
						continue;
					}
				}
				else
				{
					ants[i].speed_curr--;
				}

			}
		}
		else if (ants[i].is_queen == true)//eggs making
		{
			if (ants[i].job_time == 0)
			{
				//we create a egg
				ants[i].job_time = 900;
				if (ants[i].has_egg == false)
				{
					ants[i].has_egg = true;
				}
			}
			else
			{
				ants[i].job_time--;
			}
		}
		else if (ants[i].is_queen == false && is_atacking == false)//is idle
		{
			int mod_x = 0;
			int mod_y = 0;
			if (ants[i].current_job == 0)
			{
				
				//now we see if a ant can reach the path, if its possible it will start working on it
				//this is the best thing I can do :)
				std::string best_mov;
				for (int u = 0; u < 10000; u++)
				{
					best_mov += "a";
				}
				std::string mov;
				int work_pos_x = 0;
				int work_pos_y = 0;

				//the queen egg
				//we check if we have an item already
				if (ants[0].has_egg && ants[i].holding_item == 0 && g_data.tiles_map[ants[0].x][ants[0].y][WORKING_ON_ME] == 0)
				{
					//vamos al huevo y lo agarramos
					mov = ants[i].path.pathFind(ants[i].x, ants[i].y, ants[0].x, ants[0].y);
					if (mov.length() < best_mov.length() && mov.length() > 0)
					{
						best_mov = mov;
						ants[i].current_job = PICK_EGG;
						work_pos_x = ants[0].x;
						work_pos_y = ants[0].y;
						ants[i].next_job_x = ants[0].x;
						ants[i].next_job_y = ants[0].y;
						ants[i].job_time = 30*ants[i].job_speed;
					}
				}
				bool need_food = false;
				bool need_water = false;
				int need_food_x = -1;
				int need_food_y = -1;
				//for the food we need to make a pre loop
				for (int x_pos = 0; x_pos < 48; x_pos++)
				{
					for (int y_pos = 12; y_pos < 36; y_pos++)
					{
						if (g_data.tiles_map[x_pos][y_pos][HAS_ITEM] == 0 && g_data.tiles_map[x_pos][y_pos][FOOD] == 1 && g_data.tiles_map[x_pos][y_pos][WORKING_ON_ME] == 0)
						{
							//we search for food to fill the zone
							need_food = true;
							need_food_x = x_pos;
							need_food_y = y_pos;
						}
						if (g_data.tiles_map[x_pos][y_pos][HAS_ITEM] == 0 && g_data.tiles_map[x_pos][y_pos][DRINK] == 1 && g_data.tiles_map[x_pos][y_pos][WORKING_ON_ME] == 0)
						{
							//we search for food to fill the zone
							need_water = true;
							need_food_x = x_pos;
							need_food_y = y_pos;
						}
					}
				}
				for (int x_pos = 0; x_pos < 48; x_pos++)
				{
					for (int y_pos = 12; y_pos < 36; y_pos++)
					{
						//Breed zone
						if (ants[i].holding_item == 1 && g_data.tiles_map[x_pos][y_pos][HAS_ITEM] == 0 && g_data.tiles_map[x_pos][y_pos][BREED] == 1 && g_data.tiles_map[x_pos][y_pos][WORKING_ON_ME] == 0)
						{
							//tratamos de mover a la hormiga a esa posicion para dejar el item
							mov = ants[i].path.pathFind(ants[i].x, ants[i].y, x_pos, y_pos);
							if (mov.length() < best_mov.length() && mov.length() > 0)
							{
								best_mov = mov;
								ants[i].current_job = BREED;
								work_pos_x = x_pos;
								work_pos_y = y_pos;
								ants[i].next_job_x = x_pos;
								ants[i].next_job_y = y_pos;
								ants[i].job_time = 30*ants[i].job_speed;
							}
						}
						//Food on zone
						if (need_food && ants[i].holding_item == 0 &&
							g_data.tiles_map[x_pos][y_pos][FOOD] == 0 && (
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_1] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_2] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_3] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_4] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_5] == 1) &&
							g_data.tiles_map[x_pos][y_pos][WORKING_ON_ME] == 0
							)//we have found untiled food!
						{
							
							//vamos a la comida la agarramos
							mov = ants[i].path.pathFind(ants[i].x, ants[i].y, x_pos, y_pos);
							if (mov.length() < best_mov.length() && mov.length() > 0)
							{
								//g_data.tiles_map[x_pos][y_pos][WORKING_ON_ME] = 1;
								//g_data.tiles_map[need_food_x][need_food_y][WORKING_ON_ME] = 1;
								best_mov = mov;
								ants[i].current_job = PICK_FOOD;
								work_pos_x = x_pos;
								work_pos_y = y_pos;
								ants[i].next_job_x = need_food_x;
								ants[i].next_job_y = need_food_y;
								ants[i].next_job_x2 = x_pos;
								ants[i].next_job_y2 = y_pos;
								ants[i].job_time = 30*ants[i].job_speed;
							}
						}
						//Water on zone
						if (need_water && ants[i].holding_item == 0 &&
							g_data.tiles_map[x_pos][y_pos][DRINK] == 0 && (
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_1] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_2] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_3] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_4] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_5] == 1) &&
							g_data.tiles_map[x_pos][y_pos][WORKING_ON_ME] == 0
							)//we have found untiled food!
						{
							
							//vamos a la comida la agarramos
							mov = ants[i].path.pathFind(ants[i].x, ants[i].y, x_pos, y_pos);
							if (mov.length() < best_mov.length() && mov.length() > 0)
							{
								//g_data.tiles_map[x_pos][y_pos][WORKING_ON_ME] = 1;
								//g_data.tiles_map[need_food_x][need_food_y][WORKING_ON_ME] = 1;
								best_mov = mov;
								ants[i].current_job = PICK_DRINK;
								work_pos_x = x_pos;
								work_pos_y = y_pos;
								ants[i].next_job_x = need_food_x;
								ants[i].next_job_y = need_food_y;
								ants[i].next_job_x2 = x_pos;
								ants[i].next_job_y2 = y_pos;
								ants[i].job_time = 30*ants[i].job_speed;
							}
						}
						//Dig zone
						else if (g_data.tiles_map[x_pos][y_pos][DIG] == 1 && g_data.tiles_map[x_pos][y_pos][WORKING_ON_ME] == 0)
						{
							//if its a wall we can dig it
							//left, right, up down, we search for the best one
							if (g_data.tiles_map[x_pos-1][y_pos][BLOCK] == 0) //there is an opening
							{
								//itentamos con todas las hormigas en los cuatro lados
								mov = ants[i].path.pathFind(ants[i].x, ants[i].y, x_pos-1, y_pos);
								if (mov.length() < best_mov.length() && mov.length() > 0)
								{
									best_mov = mov;
									ants[i].current_job = DIG;
									work_pos_x = x_pos;
									work_pos_y = y_pos;
									ants[i].next_job_x = x_pos;
									ants[i].next_job_y = y_pos;
									ants[i].job_time = 75*ants[i].job_speed;
								}
							}
							if (g_data.tiles_map[x_pos+1][y_pos][BLOCK] == 0) //there is an opening
							{
								//itentamos con todas las hormigas en los cuatro lados
								mov = ants[i].path.pathFind(ants[i].x, ants[i].y, x_pos+1, y_pos);
								if (mov.length() < best_mov.length() && mov.length() > 0)
								{
									best_mov = mov;
									ants[i].current_job = DIG;
									work_pos_x = x_pos;
									work_pos_y = y_pos;
									ants[i].next_job_x = x_pos;
									ants[i].next_job_y = y_pos;
									ants[i].job_time = 75*ants[i].job_speed;
								}
							}
							if (g_data.tiles_map[x_pos][y_pos-1][BLOCK] == 0) //there is an opening
							{
								//itentamos con todas las hormigas en los cuatro lados
								mov = ants[i].path.pathFind(ants[i].x, ants[i].y, x_pos, y_pos-1);
								if (mov.length() < best_mov.length() && mov.length() > 0)
								{
									best_mov = mov;
									ants[i].current_job = DIG;
									work_pos_x = x_pos;
									work_pos_y = y_pos;
									ants[i].next_job_x = x_pos;
									ants[i].next_job_y = y_pos;
									ants[i].job_time = 75*ants[i].job_speed;
								}
							}
							if (g_data.tiles_map[x_pos][y_pos+1][BLOCK] == 0) //there is an opening
							{
								//itentamos con todas las hormigas en los cuatro lados
								mov = ants[i].path.pathFind(ants[i].x, ants[i].y, x_pos, y_pos+1);
								if (mov.length() < best_mov.length() && mov.length() > 0)
								{
									best_mov = mov;
									ants[i].current_job = DIG;
									work_pos_x = x_pos;
									work_pos_y = y_pos;
									ants[i].next_job_x = x_pos;
									ants[i].next_job_y = y_pos;
									ants[i].job_time = 75*ants[i].job_speed;
								}
							}
						}
					}
				}
				//outside actions
				for (int x_pos = 0; x_pos < 48; x_pos++)
				{
					for (int y_pos = 0; y_pos < 12; y_pos++)
					{
						//Food outside
						if (need_food && ants[i].holding_item == 0 &&
							g_data.tiles_map[x_pos][y_pos][FOOD] == 0 && (
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_1] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_2] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_3] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_4] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_FOOD_5] == 1) &&
							g_data.tiles_map[x_pos][y_pos][WORKING_ON_ME] == 0
							)//we have found untiled food!
						{
							
							//vamos a la comida la agarramos
							mov = ants[i].path.pathFind(ants[i].x, ants[i].y, x_pos, y_pos);
							if (mov.length() < best_mov.length() && mov.length() > 0)
							{
								//g_data.tiles_map[x_pos][y_pos][WORKING_ON_ME] = 1;
								//g_data.tiles_map[need_food_x][need_food_y][WORKING_ON_ME] = 1;
								best_mov = mov;
								ants[i].current_job = PICK_FOOD;
								work_pos_x = x_pos;
								work_pos_y = y_pos;
								ants[i].next_job_x = need_food_x;
								ants[i].next_job_y = need_food_y;
								ants[i].next_job_x2 = x_pos;
								ants[i].next_job_y2 = y_pos;
								ants[i].job_time = 30*ants[i].job_speed;
							}
						}
						//Water outside
						if (need_water && ants[i].holding_item == 0 &&
							g_data.tiles_map[x_pos][y_pos][DRINK] == 0 && (
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_1] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_2] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_3] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_4] == 1 ||
							g_data.tiles_map[x_pos][y_pos][ITEM_WATER_5] == 1) &&
							g_data.tiles_map[x_pos][y_pos][WORKING_ON_ME] == 0
							)//we have found untiled food!
						{
							
							//vamos a la comida la agarramos
							mov = ants[i].path.pathFind(ants[i].x, ants[i].y, x_pos, y_pos);
							if (mov.length() < best_mov.length() && mov.length() > 0)
							{
								//g_data.tiles_map[x_pos][y_pos][WORKING_ON_ME] = 1;
								//g_data.tiles_map[need_food_x][need_food_y][WORKING_ON_ME] = 1;
								best_mov = mov;
								ants[i].current_job = PICK_DRINK;
								work_pos_x = x_pos;
								work_pos_y = y_pos;
								ants[i].next_job_x = need_food_x;
								ants[i].next_job_y = need_food_y;
								ants[i].next_job_x2 = x_pos;
								ants[i].next_job_y2 = y_pos;
								ants[i].job_time = 30*ants[i].job_speed;
							}
						}
					}
				}
				//we set the best job
				if (best_mov[0] != 'a')
				{
					ants[i].path.movement_ = best_mov;
					g_data.tiles_map[work_pos_x][work_pos_y][WORKING_ON_ME] = 1;
					if (ants[i].current_job == PICK_FOOD)
					{
						g_data.tiles_map[need_food_x][need_food_y][WORKING_ON_ME] = 1;
					}
					if (ants[i].current_job == PICK_DRINK)
					{
						g_data.tiles_map[need_food_x][need_food_y][WORKING_ON_ME] = 1;
					}
				}
			}
			if (ants[i].current_job == 0 && ants[i].path.movement_.length() == 0)
			{
				//move random if not found
				//if we are idle and we are outside get inside before moving random
				if (ants[i].y <= 11)
				{
					//we search for the nearest opening
					std::string mov;
					std::string best_mov;
					for (int x_pos = 0; x_pos < 48; x_pos++)
					{
						if (g_data.tiles_map[x_pos][12] == 0)
						{
							mov = ants[i].path.pathFind(ants[i].x, ants[i].y, x_pos, 12);
							if (mov.length() > best_mov.length())
							{
								best_mov = mov;
							}
						}
						ants[i].path.movement_ = best_mov;
					}
					
				}
				else
				{
					int mover_rand = rand()%4;
					if (mover_rand == 0)
					{	
						mod_x += 1;
					}
					else if (mover_rand == 1)
					{
						mod_y += 1;
					}
					else if (mover_rand == 2)
					{
						mod_x -= 1;
					}
					else if (mover_rand == 3)
					{
						if (ants[i].y > 12)
						{
							mod_y -= 1;
						}
						
					}
					ants[i].path.pathFind(ants[i].x, ants[i].y, ants[i].x + mod_x, ants[i].y + mod_y);
				}
			}

		}
	}
}

void game_data::draw_menu()
{
	//diggin
	if (draw_menu_ == 1)
	{
		menu_cancel.Blit(80, 787); //only draw the dig menu

		if (mouse_y<744 && mouse_y>12*24) //le dimos move en otra area que no es el menu
		{

			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);	
			m_hover.Blit(f_x * 24, f_y * 24,MAKE_RGBA(255,255,255,255*0.5));
		}
		//we make the rest
		if (selecting_first == true) //ya pusimos uno
		{
			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);	
			//m_hover.Blit(selection_1.x * 24, selection_1.y * 24);
			//because we have 2, we can draw the square
			int from_x;
			int from_y;
			int to_x;
			int to_y;
			if (selection_1.x > f_x)
			{
				from_x = f_x;
				to_x = selection_1.x;
			}
			else
			{
				from_x = selection_1.x;
				to_x = f_x;
			}
			if (selection_1.y > f_y)
			{
				from_y = f_y;
				to_y = selection_1.y;
			}
			else
			{
				to_y = f_y;
				from_y = selection_1.y;
			}

			for (int i = from_x; i < to_x+1; i++)
			{
				for (int z = from_y; z < to_y+1; z++)
				{
					m_hover.Blit(i * 24, z * 24,MAKE_RGBA(255,255,255,255*0.5));
				}
			}
			
		}
		return;
	}
	else if (draw_menu_ == 2)//breed zone
	{
		menu_cancel.Blit(150, 787); //only draw the dig menu

		if (mouse_y<744 && mouse_y>12*24) //le dimos move en otra area que no es el menu
		{

			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);	
			m_breed.Blit(f_x * 24, f_y * 24,MAKE_RGBA(255,255,255,255*0.5));
		}
		//we make the rest
		if (selecting_first == true) //ya pusimos uno
		{
			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);	
			//m_hover.Blit(selection_1.x * 24, selection_1.y * 24);
			//because we have 2, we can draw the square
			int from_x;
			int from_y;
			int to_x;
			int to_y;
			if (selection_1.x > f_x)
			{
				from_x = f_x;
				to_x = selection_1.x;
			}
			else
			{
				from_x = selection_1.x;
				to_x = f_x;
			}
			if (selection_1.y > f_y)
			{
				from_y = f_y;
				to_y = selection_1.y;
			}
			else
			{
				to_y = f_y;
				from_y = selection_1.y;
			}

			for (int i = from_x; i < to_x+1; i++)
			{
				for (int z = from_y; z < to_y+1; z++)
				{
					m_breed.Blit(i * 24, z * 24,MAKE_RGBA(255,255,255,255*0.5));
				}
			}
			
		}
		return;
	}
	else if (draw_menu_ == 3)//breed zone
	{
		menu_cancel.Blit(220, 787); //only draw the dig menu

		if (mouse_y<744 && mouse_y>12*24) //le dimos move en otra area que no es el menu
		{

			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);	
			m_food_zone.Blit(f_x * 24, f_y * 24,MAKE_RGBA(255,255,255,255*0.5));
		}
		//we make the rest
		if (selecting_first == true) //ya pusimos uno
		{
			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);	
			//m_hover.Blit(selection_1.x * 24, selection_1.y * 24);
			//because we have 2, we can draw the square
			int from_x;
			int from_y;
			int to_x;
			int to_y;
			if (selection_1.x > f_x)
			{
				from_x = f_x;
				to_x = selection_1.x;
			}
			else
			{
				from_x = selection_1.x;
				to_x = f_x;
			}
			if (selection_1.y > f_y)
			{
				from_y = f_y;
				to_y = selection_1.y;
			}
			else
			{
				to_y = f_y;
				from_y = selection_1.y;
			}

			for (int i = from_x; i < to_x+1; i++)
			{
				for (int z = from_y; z < to_y+1; z++)
				{
					m_food_zone.Blit(i * 24, z * 24,MAKE_RGBA(255,255,255,255*0.5));
				}
			}
			
		}
		return;
	}
	else if (draw_menu_ == 4)//breed zone
	{
		menu_cancel.Blit(290, 787); //only draw the dig menu

		if (mouse_y<744 && mouse_y>12*24) //le dimos move en otra area que no es el menu
		{

			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);	
			m_water_zone.Blit(f_x * 24, f_y * 24,MAKE_RGBA(255,255,255,255*0.5));
		}
		//we make the rest
		if (selecting_first == true) //ya pusimos uno
		{
			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);	
			//m_hover.Blit(selection_1.x * 24, selection_1.y * 24);
			//because we have 2, we can draw the square
			int from_x;
			int from_y;
			int to_x;
			int to_y;
			if (selection_1.x > f_x)
			{
				from_x = f_x;
				to_x = selection_1.x;
			}
			else
			{
				from_x = selection_1.x;
				to_x = f_x;
			}
			if (selection_1.y > f_y)
			{
				from_y = f_y;
				to_y = selection_1.y;
			}
			else
			{
				to_y = f_y;
				from_y = selection_1.y;
			}

			for (int i = from_x; i < to_x+1; i++)
			{
				for (int z = from_y; z < to_y+1; z++)
				{
					m_water_zone.Blit(i * 24, z * 24,MAKE_RGBA(255,255,255,255*0.5));
				}
			}
			
		}
		return;
	}
	else if (draw_menu_ == 5)//breed zone
	{
		menu_cancel.Blit(360, 787); //only draw the dig menu
		return;
	}
	menu_pause.Blit(10, 787);
	menu_dig.Blit(80, 787);
	menu_breed.Blit(150, 787);
	menu_food.Blit(220, 787);
	menu_water.Blit(290, 787);
	menu_atack.Blit(360, 787);
}

void game_data::move_mouse(float x, float y)
{
	mouse_x = x;
	mouse_y = y;
}

void game_data::add_enemy_floor()
{
	//we add an enemy on the right or left position
	enemies e;
	int time_to_add = (2000-enemies_killed*10);
	if (time_to_add < 600)
	{
		time_to_add = 600;
	}
	if (enemy_time != 0 && enemy_time % (time_to_add * 10) == 0 && enemies_killed != 0)
	{
		e.type = 1;
	}
	enemies_.push_back(e);
}


void game_data::click(float x, float y)
{
	if (x > 10 && x < 10+64 && y > 787 && y < 787+64 && (draw_menu_ == 0)) //click pause
	{
		if (paused)
		{
			paused = false;
		}
		else
		{
			paused = true;
		}
	}
	else if (x > 80 && x < 80+64 && y > 787 && y < 787+64 && (draw_menu_ == 0 || draw_menu_ == 1)) //click dig
	{
		//LogMsg("Disable DIG");
		if (draw_menu_ != 1)
		{
			draw_menu_ = 1;
			paused = true;
			selecting_first = false;
		}
		else //we disable diging
		{
			//LogMsg("Disable DIG");
			paused = false;
			draw_menu_ = 0;
			selecting_first = false;
		}
	}
	else if (x > 150 && x < 150+64 && y > 787 && y < 787+64 && (draw_menu_ == 0 || draw_menu_ == 2)) //breed zones
	{
		if (draw_menu_ != 2)
		{
			draw_menu_ = 2;
			paused = true;
			selecting_first = false;
		}
		else //we disable diging
		{
			paused = false;
			draw_menu_ = 0;
		}
	}
	else if (x > 220 && x < 220+64 && y > 787 && y < 787+64 && (draw_menu_ == 0 || draw_menu_ == 3)) //food zone
	{
		if (draw_menu_ != 3)
		{
			draw_menu_ = 3;
			paused = true;
			selecting_first = false;
		}
		else //we disable diging
		{
			paused = false;
			draw_menu_ = 0;
		}
	}
	else if (x > 290 && x < 290+64 && y > 787 && y < 787+64 && (draw_menu_ == 0 || draw_menu_ == 4)) //water zone
	{
		if (draw_menu_ != 4)
		{
			draw_menu_ = 4;
			paused = true;
			selecting_first = false;
		}
		else //we disable diging
		{
			paused = false;
			draw_menu_ = 0;
		}
	}
	else if (x > 360 && x < 360+64 && y > 787 && y < 787+64 && (draw_menu_ == 0 || draw_menu_ == 5)) //water zone
	{
		if (draw_menu_ !=5)
		{
			draw_menu_ = 5;
			paused = true;
			selecting_first = false;
		}
		else //we disable diging
		{
			for (int i = 0; i < enemies_.size(); i++)
			{
				enemies_[i].is_under_atack = false;
			}
			paused = false;
			draw_menu_ = 0;
		}
	}
	/*else if (x > 430 && x < 430+64 && y > 787 && y < 787+64 && (draw_menu_ == 0 || draw_menu_ == 6)) //water zone
	{
		if (draw_menu_ !=6)
		{
			draw_menu_ = 6;
			paused = true;
			selecting_first = false;
		}
		else //we disable diging
		{
			paused = false;
			draw_menu_ = 6;
		}
	}*/

	//diggin
	//we have made a click so we can save the position
	if (draw_menu_ == 1) //dig
	{
		if (mouse_y<744 && mouse_y>12*24) //le dimos move en otra area que no es el menu
		{
			
			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);
			if (selecting_first == false)
			{
				selecting_first = true;
				selection_1 = point2(f_x,f_y);
			}
			else//we are seleting the second one
			{
				selection_2 = point2(f_x,f_y);
				draw_menu_ = 0;
				paused = false;
				//now we make add all the tiles to dig
				int from_x;
				int from_y;
				int to_x;
				int to_y;
				if (selection_1.x > f_x)
				{
					from_x = f_x;
					to_x = selection_1.x;
				}
				else
				{
					from_x = selection_1.x;
					to_x = f_x;
				}
				if (selection_1.y > f_y)
				{
					from_y = f_y;
					to_y = selection_1.y;
				}
				else
				{
					to_y = f_y;
					from_y = selection_1.y;
				}

				for (int i = from_x; i < to_x+1; i++)
				{
					for (int z = from_y; z < to_y+1; z++)
					{
						//if its a wall we can dig it
						if (g_data.tiles_map[i][z][BLOCK] == 1)
						{
							g_data.tiles_map[i][z][DIG] = 1;
						}		
					}
				}

			}
		}
	}
	else if (draw_menu_ == 2) //Breed zone
	{
		if (mouse_y<744 && mouse_y>12*24) //le dimos move en otra area que no es el menu
		{
			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);
			if (selecting_first == false)
			{
				selecting_first = true;
				selection_1 = point2(f_x,f_y);
			}
			else//we are seleting the second one
			{
				selection_2 = point2(f_x,f_y);
				draw_menu_ = 0;
				paused = false;
				//now we make add all the tiles to dig
				int from_x;
				int from_y;
				int to_x;
				int to_y;
				if (selection_1.x > f_x)
				{
					from_x = f_x;
					to_x = selection_1.x;
				}
				else
				{
					from_x = selection_1.x;
					to_x = f_x;
				}
				if (selection_1.y > f_y)
				{
					from_y = f_y;
					to_y = selection_1.y;
				}
				else
				{
					to_y = f_y;
					from_y = selection_1.y;
				}

				for (int i = from_x; i < to_x+1; i++)
				{
					for (int z = from_y; z < to_y+1; z++)
					{
						//if its not a wall it can be a breed zone
						if (g_data.tiles_map[i][z][BLOCK] == 0)
						{
							g_data.tiles_map[i][z][BREED] = 1;
						}		
					}
				}

			}
		}
	}
	else if (draw_menu_ == 3) //Food zone
	{
		if (mouse_y<744 && mouse_y>12*24) //le dimos move en otra area que no es el menu
		{
			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);
			if (selecting_first == false)
			{
				selecting_first = true;
				selection_1 = point2(f_x,f_y);
			}
			else//we are seleting the second one
			{
				selection_2 = point2(f_x,f_y);
				draw_menu_ = 0;
				paused = false;
				//now we make add all the tiles to dig
				int from_x;
				int from_y;
				int to_x;
				int to_y;
				if (selection_1.x > f_x)
				{
					from_x = f_x;
					to_x = selection_1.x;
				}
				else
				{
					from_x = selection_1.x;
					to_x = f_x;
				}
				if (selection_1.y > f_y)
				{
					from_y = f_y;
					to_y = selection_1.y;
				}
				else
				{
					to_y = f_y;
					from_y = selection_1.y;
				}

				for (int i = from_x; i < to_x+1; i++)
				{
					for (int z = from_y; z < to_y+1; z++)
					{
						//if its not a wall it can be a breed zone
						if (g_data.tiles_map[i][z][BLOCK] == 0)
						{
							g_data.tiles_map[i][z][FOOD] = 1;
						}		
					}
				}

			}
		}
	}
	else if (draw_menu_ == 4) //Food zone
	{
		if (mouse_y<744 && mouse_y>12*24) //le dimos move en otra area que no es el menu
		{
			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);
			if (selecting_first == false)
			{
				selecting_first = true;
				selection_1 = point2(f_x,f_y);
			}
			else//we are seleting the second one
			{
				selection_2 = point2(f_x,f_y);
				draw_menu_ = 0;
				paused = false;
				//now we make add all the tiles to dig
				int from_x;
				int from_y;
				int to_x;
				int to_y;
				if (selection_1.x > f_x)
				{
					from_x = f_x;
					to_x = selection_1.x;
				}
				else
				{
					from_x = selection_1.x;
					to_x = f_x;
				}
				if (selection_1.y > f_y)
				{
					from_y = f_y;
					to_y = selection_1.y;
				}
				else
				{
					to_y = f_y;
					from_y = selection_1.y;
				}

				for (int i = from_x; i < to_x+1; i++)
				{
					for (int z = from_y; z < to_y+1; z++)
					{
						//if its not a wall it can be a breed zone
						if (g_data.tiles_map[i][z][BLOCK] == 0)
						{
							g_data.tiles_map[i][z][DRINK] = 1;
						}		
					}
				}

			}
		}
	}
	else if (draw_menu_ == 6) //Food zone
	{
		if (mouse_y<744 && mouse_y>12*24) //le dimos move en otra area que no es el menu
		{
			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);
			if (selecting_first == false)
			{
				selecting_first = true;
				selection_1 = point2(f_x,f_y);
			}
			else//we are seleting the second one
			{
				selection_2 = point2(f_x,f_y);
				draw_menu_ = 0;
				paused = false;
				//now we make add all the tiles to dig
				int from_x;
				int from_y;
				int to_x;
				int to_y;
				if (selection_1.x > f_x)
				{
					from_x = f_x;
					to_x = selection_1.x;
				}
				else
				{
					from_x = selection_1.x;
					to_x = f_x;
				}
				if (selection_1.y > f_y)
				{
					from_y = f_y;
					to_y = selection_1.y;
				}
				else
				{
					to_y = f_y;
					from_y = selection_1.y;
				}

				for (int i = from_x; i < to_x+1; i++)
				{
					for (int z = from_y; z < to_y+1; z++)
					{
						//if its not a wall it can be a breed zone
							g_data.tiles_map[i][z][DRINK] = 0;
							g_data.tiles_map[i][z][FOOD] = 0;
							g_data.tiles_map[i][z][BREED] = 0;	
							g_data.tiles_map[i][z][DIG] = 0;	
					}
				}

			}
		}
	}
	else if (draw_menu_ == 5) //Attack
	{

		if (mouse_y<744) //le dimos move en otra area que no es el menu
		{
			int f_x = floorf(mouse_x/24);
			int f_y = floorf(mouse_y/24);
			bool atacked = false;
			for (int i = 0; i < enemies_.size(); i++)
			{
				//LogMsg("click: %d, %d", f_x, f_y);
				//LogMsg("enemy: %d, %d", enemies_[i].x, enemies_[i].y);
				if (f_x == enemies_[i].x && f_y == enemies_[i].y)
				{
					enemies_[i].is_under_atack = true;
					atacked = true;
				}
				else
				{
					enemies_[i].is_under_atack = false;
				}
			}
			if (atacked == true)
			{
				draw_menu_ = 0;
				paused = false;
			}
		}
	}
}

void game_data::init()
{
	enemies_killed = 0;
	draw_menu_ = 0; //0 draws
	paused = false;
	if (!m_sand.IsLoaded())
	{
		m_sand.LoadFile("interface/sand.rttex");
	}
	if (!m_sand_digged.IsLoaded())
	{
		m_sand_digged.LoadFile("interface/sand_digged.rttex");
	}
	if (!m_sky.IsLoaded())
	{
		m_sky.LoadFile("interface/sky.rttex");
	}
	if (!m_ant.IsLoaded())
	{
		m_ant.LoadFile("interface/ant.rttex");
	}
	if (!m_ant_sleep.IsLoaded())
	{
		m_ant_sleep.LoadFile("interface/ant_sleep.rttex");
	}
	if (!m_queen.IsLoaded())
	{
		m_queen.LoadFile("interface/queen.rttex");
	}
	if (!m_wood.IsLoaded())
	{
		m_wood.LoadFile("interface/wood.rttex");
	}
	if (!m_leaf.IsLoaded())
	{
		m_leaf.LoadFile("interface/leaf.rttex");
	}
	if (!m_btn_dig.IsLoaded())
	{
		m_btn_dig.LoadFile("interface/btn.rttex");
	}
	if (!m_hover.IsLoaded())
	{
		m_hover.LoadFile("interface/hover.rttex");
	}
	if (!m_breed.IsLoaded())
	{
		m_breed.LoadFile("interface/breed.rttex");
	}
	if (!m_egg.IsLoaded())
	{
		m_egg.LoadFile("interface/larva.rttex");
	}
	if (!m_food_1.IsLoaded())
	{
		m_food_1.LoadFile("interface/food_1.rttex");
	}
	if (!m_water_1.IsLoaded())
	{
		m_water_1.LoadFile("interface/water_1.rttex");
	}
	if (!m_food_zone.IsLoaded())
	{
		m_food_zone.LoadFile("interface/food_zone.rttex");
	}
	if (!m_water_zone.IsLoaded())
	{
		m_water_zone.LoadFile("interface/w_zone.rttex");
	}
	if (!m_bug.IsLoaded())
	{
		m_bug.LoadFile("interface/bug_1.rttex");
	}
	if (!m_bug2.IsLoaded())
	{
		m_bug2.LoadFile("interface/bug_2.rttex");
	}
	if (!m_hp_loss.IsLoaded())
	{
		m_hp_loss.LoadFile("interface/loss_hp.rttex");
	}
	if (!m_atack.IsLoaded())
	{
		m_atack.LoadFile("interface/atack.rttex");
	}

	if (!menu_pause.IsLoaded())
	{
		menu_pause.LoadFile("interface/m_pause.rttex");
	}
	if (!menu_dig.IsLoaded())
	{
		menu_dig.LoadFile("interface/m_dig.rttex");
	}
	if (!menu_atack.IsLoaded())
	{
		menu_atack.LoadFile("interface/m_attack.rttex");
	}
	if (!menu_breed.IsLoaded())
	{
		menu_breed.LoadFile("interface/m_breed.rttex");
	}
	if (!menu_water.IsLoaded())
	{
		menu_water.LoadFile("interface/m_water.rttex");
	}
	if (!menu_food.IsLoaded())
	{
		menu_food.LoadFile("interface/m_food.rttex");
	}
	if (!menu_cancel.IsLoaded())
	{
		menu_cancel.LoadFile("interface/m_cancel.rttex");
	}

}

game_data::game_data()
{
	enemy_time = 0;
	rain_time = 6000;
    //int dx2[4]={1, 0, -1, 0};
    //int dy2[4]={0, 1, 0, -1};
    //we add all the ways you can move
    dx[0] = 1;
    dx[1] = 0;
    dx[2] = -1;
    dx[3] = 0;
    dy[0] = 0;
    dy[1] = 1;
    dy[2] = 0;
    dy[3] = -1;
}

