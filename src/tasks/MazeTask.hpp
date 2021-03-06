/*
 *	An example maze textfile could look like:
 *
 *	0 0 0 0 0 0 0 0
 *	0 2 1 1 1 1 1 0
 *	0 0 0 1 0 0 0 0
 *	0 0 1 1 1 1 3 0
 *	0 0 0 0 0 0 0 0
 *
 *	Note that it should be padded with zeros.
 *
 *	Created on: May 4, 2014
 *	Author: Garrett R, Andreea G
 */

#ifndef TASKS_MAZETASK_H_
#define TASKS_MAZETASK_H_

#include <string>
#include <deque>

#include <src/tasks/TaskInterface.hpp>

using std::deque;

//directions player may face in
enum class Direction {LEFT, UP, DOWN, RIGHT};
//the different kinds of tile that make up map
enum class MazeTile {WALL, NORMAL, START, FINISH};


class MazeTask : public TaskInterface {

public:

	//Grant access for unit-testing
	friend class MazeTest;

	//the map file (see docs at top of file) and whether you want
	//a random starting position.  If random_start is false, then
	//player is started at place in map where value is 2.
	//To set the starting direction, it will loop through the 4 directions
	//stopping when it finds a valid one.
	explicit MazeTask(const std::string& map_file, const bool random_start = false);

	//Turns the brain in the new direction of motion
	//The argument decision should be a deque with 2 Booleans.
	//The first element shows if we're going straight, the second is right/left.
	// So, the possible inputs and their corresponding meanings are:
	// [true,true] and [true,false] -> Stay pointed straight
	// [false,false] -> turn left,  [false,true] -> turn right.
	//return false if decision is invalid (i.e. it would have the player facing the wall)
	bool ActOnDecision(const deque<bool>& decision);

	//player moves forward until they reach a decision point (not including dead-ends upon which the player reverses
		//direction and continues automatically, and corners which the player turns and continues)
	//return false if player was not able to move at all (i.e. they were facing a wall)
	//If the player ever steps on the FINISH, advance position will stop there.
	bool AdvancePosition();

	//return 3 booleans, the first for if a left-turn is possible, the second for straight,
	//and the third for right-turn.
	deque<bool> GetBrainInput() const;

	//Returns 1 for finished, 0 for not finished
	int IsFinished() const;

private:

	bool LoadMap(const std::string& map_file);

	//A matrix representing the map, where 0s are walls, 1s are valid paths,
	//2 is the suggested starting point, and 3 is the finish.
	deque<deque<MazeTile> > map_;

	//Current location of the player given by row and height
	int row_;
	int col_;

	//The width and height of the map (including the padding zeros)
	size_t width_;
	size_t height_;

	//Stored the direction which the player is pointing in our (2D) maze.
	Direction player_direction_;

	void TurnLeft(); //Changes player_direction_
	void TurnRight();
	void TurnAround(); //go in opposite direction

	//Get the tiles in front, to the left and to the right of player
	MazeTile GetTileLeft() const;
	MazeTile GetTileRight() const;
	MazeTile GetTileFront() const;

};


#endif /* TASKS_MAZETASK_H_ */
