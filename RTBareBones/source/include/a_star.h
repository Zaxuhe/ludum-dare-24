#ifndef A_STAR_H
#define A_STAR_H

/** {{{ http://code.activestate.com/recipes/577457/ (r4) */
// Astar.cpp
// http://en.wikipedia.org/wiki/A*
// Compiler: Dev-C++ 4.9.9.2
// FB - 201012256

#include <iostream>
#include <iomanip>
#include <queue>
#include <string>
#include <math.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>

//static const int n=48; // horizontal size of the map
//static const int m=36; // vertical size size of the map

class a_star
{
    public:
        a_star();
        virtual ~a_star();

        std::string movement_;
        int current_postion_;
		

        int closed_nodes_map[48][36]; // map of closed (tried-out) nodes
        int open_nodes_map[48][36]; // map of open (not-yet-tried) nodes
        int dir_map[48][36]; //map of directions

        std::string pathFind(const int & xStart, const int & yStart,
                                     const int & xFinish, const int & yFinish);
    protected:

    private:
};

class node
{
    // current position
    int xPos;
    int yPos;
    // total distance already travelled to reach the node
    int level;
    // priority=level+remaining distance estimate
    int priority;  // smaller: higher priority

    public:
        node(int xp, int yp, int d, int p)
            {xPos=xp; yPos=yp; level=d; priority=p;}

        int getxPos() const {return xPos;}
        int getyPos() const {return yPos;}
        int getLevel() const {return level;}
        int getPriority() const {return priority;}

        void updatePriority(const int & xDest, const int & yDest)
        {
             priority=level+estimate(xDest, yDest)*10; //A*
        }

        // give better priority to going strait instead of diagonally
        void nextLevel(const int & i) // i: direction
        {
             int dir=4; // number of possible directions to go at any position
             level+=(dir==8?(i%2==0?10:14):10);
        }

        // Estimation function for the remaining distance to the goal.
        const int & estimate(const int & xDest, const int & yDest) const
        {
            static int xd, yd, d;
            xd=xDest-xPos;
            yd=yDest-yPos;

            // Euclidian Distance
            d=static_cast<int>(sqrtf(xd*xd+yd*yd));

            // Manhattan distance
            //d=abs(xd)+abs(yd);

            // Chebyshev distance
            //d=max(abs(xd), abs(yd));

            return(d);
        }
};

inline bool operator<(const node & a, const node & b)
{
  return a.getPriority() > b.getPriority();
}
#endif // A_STAR_H
