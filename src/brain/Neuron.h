/*
 * Neuron.h
 *
 * Note: the synapses of a neuron can connect back to the same neuron.
 *
 *  Created on: May 15, 2014
 *      Author: Andreea
 */

#ifndef BRAIN__NEURON_H_
#define BRAIN__NEURON_H_

#include <unordered_map>
#include <random>

using std::unordered_map;


constexpr float MIN_STRENGTH = 0.0;
constexpr float MAX_STRENGTH = 100.0;		//TODO find better way to define this somewhere...
constexpr float MIN_ACTIVATION = 0.0;		//TODO find better way to define this somewhere...
constexpr float MAX_ACTIVATION = 100.0;		//TODO find better way to define this somewhere...
constexpr float TIME_STEP = 0.001;		//TODO find better way to define this somewhere...
constexpr float MIN_DECAY_RATE = 0.0;
constexpr float MAX_DECAY_RATE = 5.0;		//TODO find better way to define this somewhere...
//Not sure if this is good, but we'll occasionally want pseudo-random numbers instead of using random_device
//for example, with testing, pseudo-random is better.
namespace my_types {
	//Type of random generator used for all randomness in program
	//(including neuron creation, brain creation, evolution, etc.)
	typedef std::random_device gen_type;
//	typedef std::default_random_engine gen_type;
}


class Neuron {

public:

	//TODO: Brain::MutateNeurons(.) accesses Neuron::MutateSynapses, so either we keep this friendship or
	//make MutateSynapses public.  Another idea is to make the whole Neuron class private.  After all, only Brain accesses it right?
	friend class Brain;

	//generate neuron with random active_threshold and decay_rate and with synapses (with random strengths) coming from
	//random origin neurons.  It needs to know how many neurons are in the brain.  The number of synapses is given by
	//uniform distribution: [0,num_neurons], but if duplicate synapses are removed.
	Neuron(const int num_neurons);
	//Assigns the synapses to random originating neurons
	Neuron(const float start_activation, const float decay_rate, const float active_threshold,
					const int num_neurons, const int num_synapses);

	//Neuron(const Neuron &neur);		//copy constructor
	//Neuron& operator=(const Neuron &neur) { return *this; }

	void set_activation(const float &activation) { activation_ = activation; }
	float get_activation() const { return activation_; }
	void set_new_activation(const float &new_activation) { new_activation_ = new_activation; }
	float get_new_activation() const { return new_activation_; }
	void set_active_threshold(const float &active_threshold) { active_threshold_ = active_threshold; }
	float get_active_threshold() const { return active_threshold_; }
	void set_decay_rate(const float &decay_rate) { decay_rate_ = decay_rate; }
	float get_decay_rate() const { return decay_rate_; }

	//Updates the current activation to be the new activation at time t + delta t,
	//and sets the future new_activation to zero
	void update_activation() { activation_ = new_activation_; new_activation_ = 0; }
	//Decides if neuron is active enough for the neuron to fire.
	bool ActivationFunction() const;

public:
	//map of synapses where the first element is the origin neuron and the second element is the connection strength
	unordered_map<int, float> synapses_;

	float activation_;				//current activation of the neuron
	float new_activation_;
	float active_threshold_;
	float decay_rate_;				//strength of each neuron decays exponentially with a certain decay rate

	//Randomly mutates synapses. num_mutated_synapses is the number of synapses
	//that will undergo a mutation. If it is positive, new connections will be formed
	//or the strengths of existing ones will be changed. If it is negative,
	//random connections will be removed. num_neurons is the total number of neurons in the brain.
	void MutateSynapses(int num_mutated_synapses, const int num_neurons);
};


#endif /* BRAIN__NEURON_H_ */
