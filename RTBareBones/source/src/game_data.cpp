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
	a.x = x_pos;
	a.y = y_pos;
	a.is_queen = true;
	ants.push_back(a);

	ant a2;
	a2.x = 47;
	a2.y = 11;
	ants.push_back(a2);

	ant a3;
	a3.x = 0;
	a3.y = 11;
	ants.push_back(a3);

}

void game_data::render_map()
{
    // display the map with the route

	for (int x_pos = 0; x_pos < 48; x_pos++)
	{
		for (int y_pos = 0; y_pos < 36; y_pos++)
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
 			 if(g_data.tiles_map[x_pos][y_pos].test(ITEM_EGG) == true) //Breed
			 {
				m_egg.Blit( (x_pos*24),(y_pos*24));
			 }
			 
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
				m_ant.BlitScaled( (ants[i].x*24)+12,(ants[i].y*24)+ 12, CL_Vec2f (1.5,1.5));
			}
			if (ants[i].holding_item == 1)
			{
				m_egg.Blit( (ants[i].x*24),(ants[i].y*24));
			}
		}

	}



}

//one step on the game
void game_data::step()
{
	if (paused)
	{
		return;
	}
	//we make the ants walk
	for (int i = 0; i < ants.size(); i++)
	{
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
						g_data.tiles_map[ants[i].x][ants[i].y][HAS_ITEM] = 1;
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
		else if (ants[i].is_queen == false)//is idle
		{
			int mod_x = 0;
			int mod_y = 0;
			//look for a job
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
				for (int x_pos = 0; x_pos < 48; x_pos++)
				{
					for (int y_pos = 0; y_pos < 36; y_pos++)
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
				//we set the best job
				if (best_mov[0] != 'a')
				{
					ants[i].path.movement_ = best_mov;
					g_data.tiles_map[work_pos_x][work_pos_y][WORKING_ON_ME] = 1;

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
		/*else
		{
			//we generate a path for them
			if (ants[i].is_queen == false)
			{
				ants[i].path.pathFind(ants[i].x, ants[i].y, 0, 11);
				if (ants[i].x == 0 && ants[i].y == 11)
				{
					ants[i].path.pathFind(ants[i].x, ants[i].y, 47, 11);
				}
			}
		}*/
	}
}

void game_data::draw_menu()
{
	//diggin
	if (draw_menu_ == 1)
	{
		m_btn_dig.Blit(80, 787); //only draw the dig menu

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
		m_btn_dig.Blit(150, 787); //only draw the dig menu

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
	m_btn_dig.Blit(10, 787);
	m_btn_dig.Blit(80, 787);
	m_btn_dig.Blit(150, 787);
	m_btn_dig.Blit(220, 787);
	m_btn_dig.Blit(290, 787);
	m_btn_dig.Blit(360, 787);
	m_btn_dig.Blit(430, 787);
	m_btn_dig.Blit(500, 787);
	m_btn_dig.Blit(570, 787);
}

void game_data::move_mouse(float x, float y)
{
	mouse_x = x;
	mouse_y = y;
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

}

void game_data::init()
{
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


}

game_data::game_data()
{
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

