/*
 * GameMaster.cc
 *
 *  Created on: June 7, 2014
 *      Author: Andreea
 */

#include <iostream>
#include <fstream>
#include <map>

#include <src/gamemaster/GameMaster.hpp>
#include <src/Globals.hpp>
//#include <algorithm> //std::transform

using std::deque;
using std::map;
//using std::unordered_map;
using std::multimap;
using std::pair;
using std::cout;
using std::endl;

//TODO: finish this.  It depends on what you want to print out...  hahaha ahaaha heeheehee.  Ha haaha ..
//are you just typing what I'm saying?  Hahahahaha. ... OK you can stop now... HEeeeheheehee... hahaa..
//I'm curious how long you'll keep going ... hahahaha... haaahaaaahaaheeeheeheeheheeeheeeheeheeheehaa
//OK stop!  Seriou....hahahahahaahahah heheeheheeheeeeheeeheeheeheh   <silent laughing>  haahaahahaaahaaa
//Alright...ha  hahahahaa  <leaves room> ... <comes back>  haaahaaahahaa hee heehee are you serious?  Garrett,
//stop...heeeheehee haaahaaahaahaaahaahaahaahaha...ahhh....Alright, I'll ignore you now.hah aahaahhaahahaha hee
//heeeheehee ... <silently chuckling>  ... heeheeheeeheehaahahahahahahahahahahahahahehehaahahahahahahahahahaahah
//<leaves room> hmmhmmhmmmhmmmhmmm <silent chuckling> hmmhmmmh mmmmhmmm hmmm hmmhmmhmmhmm hmmhmm haahaahaa
void output::PrintGenerationInfo(const deque<Brain> brains) {
	std::ofstream out("gamemaster_output.txt");
	if (out.good() == false) {
		std::cerr << "ERROR: problem opening temporary file!";
	}

	//std::cout << "Print some info\n";
	out << "Printing brain fitness scores: \n";
	for (auto brain_it = brains.begin(); brain_it != brains.end(); brain_it++) {
		out << brain_it->fitness_score_ << " ";
	}
	out << endl;
}

//For now, main() will not be using this constructor
/*
GameMaster::GameMaster(const size_t num_brains,
					   const size_t num_neurons, const size_t num_input_neurons, const size_t num_output_neurons,
					   const int max_decisions,
					   const int input_duration, const int input_output_delay, const int output_duration,
					   const string maze_map_file, const int maze_random_start,
					   const int num_generations, const size_t num_mutated_neurons, const size_t num_mutated_synapses,
						 const float prob_asexual) :
						 num_brains_(num_brains), max_decisions_(max_decisions), input_duration_(input_duration),
						 output_duration_(output_duration), maze_random_start_(maze_random_start),
						 num_generations_(num_generations), num_mutated_neurons_(num_mutated_neurons),
						 num_mutated_synapses_(num_mutated_synapses), maze_map_file_(maze_map_file) {

	//construct brains
	for (size_t ii = 0; ii < num_brains_; ii++) {
		Brain new_brain(num_neurons, num_input_neurons, num_output_neurons);
		brains_.push_back(new_brain);
	}

	//construct evolution
	Evolution evolution_(prob_asexual);
}
*/

GameMaster::GameMaster(const size_t num_brains,
					const size_t num_neurons, const size_t num_input_neurons, const size_t num_output_neurons,
					const float av_active_threshold, const float st_dev_active_threshold,
					const float av_start_activation, const float st_dev_start_activation,
					const float av_decay_rate, const float st_dev_decay_rate,
					const int av_num_syn, const int st_dev_num_syn,
					const float av_syn_strength, const float st_dev_syn_strength,
					const int max_decisions,
					const int input_duration, const int input_output_delay, const int output_duration,
					const string maze_map_file, const int maze_random_start,
					const int num_generations, const size_t num_mutated_neurons, const size_t num_mutated_synapses,
					const float prob_asexual) :
					num_brains_(num_brains), max_decisions_(max_decisions),
					input_duration_(input_duration), input_output_delay_(input_output_delay),
					output_duration_(output_duration), maze_map_file_(maze_map_file),
					maze_random_start_(maze_random_start), num_generations_(num_generations),
					num_mutated_neurons_(num_mutated_neurons), num_mutated_synapses_(num_mutated_synapses)  {
	//construct brains
	for (size_t ii = 0; ii < num_brains_; ii++) {
		Brain new_brain(num_neurons, num_input_neurons, num_output_neurons,
						av_active_threshold, st_dev_active_threshold,
						av_start_activation, st_dev_start_activation,
						av_decay_rate, st_dev_decay_rate,
						av_num_syn, st_dev_num_syn,
						av_syn_strength, st_dev_syn_strength);
		brains_.push_back(new_brain);
	}

	//construct evolution
	Evolution evolution_(prob_asexual);
}

int GameMaster::ObtainBrainFitnesses() {
	//Loop through each brain
	for (auto brain_it = brains_.begin(); brain_it != brains_.end(); brain_it++) {
		//Default to worst outcome. If the brain doesn't finish the maze in max_decisions_ time then it receives the worst score.
		int num_decisions = max_decisions_;

		//TODO: after profiling code, determine if this is creating this object each time costing us too much?
		//if so, we could make a single maze_task and implement a "reset" function that puts the player back to start.
		MazeTask maze_task(maze_map_file_, maze_random_start_);

		//Loop through every decision the brain must make
		for (int decision = 0; decision < max_decisions_; decision++) {
			//Advance position through the maze (either from the start or from the previous loop's decision)
			if (!maze_task.AdvancePosition()) {
				cout << "Brain tried to enter a wall; it will receive the worst fitness score" << endl;
				break;
			}

			//Check if brain has finished the maze
			if (maze_task.IsFinished()) {
				num_decisions = decision;
				break;
			}

			//Get brain input (which are the possible directions from this position)
			deque<bool> brain_input = maze_task.GetBrainInput();
								//			cout << "\nBrain input: ";   //TODO: delete this output, just debugging
								//			for (auto br_input_it = brain_input.begin(); br_input_it != brain_input.end(); br_input_it++) {
								//				cout << *br_input_it << " ";
								//			}
								//			cout << endl;

			//Let the brain decide on an action
			//The first element in the map is an output of the brain, while the second is counting how many times
			//the brain has given this output
			//TODO: this doesn't need to be an ordered map, but if I try to make it unordered_map I get errors later. Don't fully understand why.
			map<deque<bool>, int> brain_output_frequencies;
			//Loop through brain cycles, looping enough times to collect all the input and all the output for one decision
			for (int cycle = 0; cycle < (input_output_delay_ + output_duration_); cycle++) {
				//if we're still in the input period, give input to brain at each cycle
				if (cycle < input_duration_) {
					brain_it->give_input(brain_input);
				}

				brain_it->Cycle();

				//if we're in the output period, store the output for each cycle into brain_output
				if (cycle >= input_output_delay_) {
					deque<bool> brain_output = brain_it->get_output();
										//					cout << "\nBrain output: "; //TODO: get rid of this, it's for debugging.
										//					for (auto br_output_it = brain_output.begin(); br_output_it != brain_output.end(); br_output_it++) {
										//						cout << *br_output_it << " ";
										//					}
										//					cout << endl;

					//check if this brain_output is already in the map and if so add 1 to its current frequency,
					//otherwise add it to the map
					if (brain_output_frequencies.count(brain_output)) {
						brain_output_frequencies[brain_output]++;
					} else {
						brain_output_frequencies[brain_output] = 1;
					}
				}
			}

			//Test the brain_outputs in order of the frequency, and try to find the most common valid output
			//(that doesn't send the brain into a wall)
			//flip the map of <brain_output, frequency> to a multimap <frequency, brain_output>
			//(which gets automatically sorted by its frequency)
			multimap<int, deque<bool>> brain_output_sorted;
			for (auto map_it = brain_output_frequencies.begin(); map_it != brain_output_frequencies.end(); map_it++) {
				brain_output_sorted.insert( pair<int, deque<bool> >(map_it->second, map_it->first) );
			}
			bool found_valid_decision = false;
			deque<bool> brain_decision; //final (most common and valid) brain output will correspond to the decision
			//loop through all outputs, checking if brain has found a valid decision,
			//and turn the brain in that direction if it has.
			//Note: the output is order from lowest frequency output to highest, so we loop in reverse order
			for (auto output_it = brain_output_sorted.rbegin(); output_it != brain_output_sorted.rend(); output_it++) {
				if (maze_task.ActOnDecision(output_it->second)) {
							//del  //					cout << "Found valid decision! It is: " << output_it->second[0] << output_it->second[1] << endl;
					found_valid_decision = true;
					brain_decision = output_it->second;
					break;
				}
			}

			//If no valid decision has been found, the brain decided to go into a wall, so it gets the worst fitness score
			if (!found_valid_decision) {
				break;
			}

		} //end for loop through decisions

		//Set fitness score to 1/num_decisions
		//Test if the brain solved the maze in zero decisions
		if (num_decisions == 0) {
			std::cerr << "The maze used allows brains to solve it with zero decisions!" << endl;
			return -1;
		}
		brain_it->set_fitness_score(1.0/num_decisions);
												//del     cout << brain_it->get_fitness_score() << " ";
	} //end for loop through every brain
	cout << endl;
	return 0;
}


int GameMaster::MasterControl() {
	for (int gener = 0; gener < num_generations_; gener++) {
		std::cout << "Generation " << gener << std::endl;
		int test = ObtainBrainFitnesses();
		if (test < 0) {
			return -1;
		}

		cout << "Exitted ObtainBrainFitnesses" << endl;
		output::PrintGenerationInfo(brains_);

		//find the list of most fit brains
		test = evolution_.ChooseMostFitBrains(brains_);
		if (test < 0) {
			return -1;
		}

		//Obtain the next generation of brains
		brains_ = evolution_.GetNextGeneration(brains_, num_mutated_neurons_, num_mutated_synapses_);
	}

	return 0;
}



