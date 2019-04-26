/*
 * Declarations for Deme class to evolve a genetic algorithm for the
 * travelling-salesperson problem.  A deme is a population of individuals.
 */

#include "chromosome.hh"
#include "deme.hh"
#include <cassert>
#include <chrono> // For unique generator_ seeds

// Generate a Deme of the specified size with all-random chromosomes.
// Also receives a mutation rate in the range [0-1].
Deme::Deme(const Cities* cities_ptr, unsigned pop_size, double mut_rate) 
	: mut_rate_(mut_rate),
	  pop_size_(pop_size),
	  generator_(std::chrono::system_clock::now().time_since_epoch().count()) {
	
	while(pop_size) {
		// Uses clone() to fill pop_ since chromo doesn't last outside of the constructor, but clone()
		// creates a reference and allocates memory to the Chromosome
		auto chromo = Chromosome(cities_ptr);
		pop_.push_back(chromo.clone());
		pop_size--;
	}
}

// Clean up as necessary
Deme::~Deme() {
	// Deletes every Chromosome* in pop_, freeing the memory allocated by clone()
	while(!pop_.empty()) {
		pop_.erase(pop_.begin());
	}
}

// Evolve a single generation of new chromosomes, as follows:
// We select pop_size/2 pairs of chromosomes (using the select() method below).
// Each chromosome in the pair can be randomly selected for mutation, with
// probability mut_rate, in which case it calls the chromosome mutate() method.
// Then, the pair is recombined once (using the recombine() method) to generate
// a new pair of chromosomes, which are stored in the Deme.
// After we've generated pop_size new chromosomes, we delete all the old ones.
void Deme::compute_next_generation() {
	assert(!(pop_size_ % 2)); // Ensures there are two parents to create and be replaced by two children
	auto next_gen = pop_;
	for(unsigned i = 0; i < pop_size_; ++i) {
		// Random number generator to find mutation chance
		std::uniform_real_distribution<double> distribution(0., 1.);
		
		auto parent1 = select_parent();
		auto mut = distribution(generator_);
		if(mut < mut_rate_) { parent1->mutate(); }
		
		auto parent2 = select_parent();
		// Ensures parent1 and parent2 aren't the same
		while(parent1 == parent2) {
			parent2 = select_parent();
		}
		mut = distribution(generator_);
		if(mut < mut_rate_) { parent2->mutate(); }
		
		auto kids = parent1->recombine(parent2);
		next_gen[i] = kids.first;
		++i;
		next_gen[i] = kids.second;
	}
	pop_ = next_gen;
}

// Return a copy of the chromosome with the highest fitness.
const Chromosome* Deme::get_best() const {
	double best = 0;
	auto fittest = *pop_.begin();
	for(auto chromo : pop_) {
		auto fit = chromo->get_fitness();
		if(fit > best) {
			best = fit;
			fittest = chromo;
		}
	}
	return fittest;
		
}

// Randomly select a chromosome in the population based on fitness and
// return a pointer to that chromosome.
Chromosome* Deme::select_parent() {
	auto fitness_sum = 0.;
	auto partial_sum = 0.;
	// Calculates the sum of the fitnesses
	for(auto chromo : pop_) {
		fitness_sum += chromo->get_fitness();
	}
	
	// Generates a random number and finds the chromosome where the partial fitness is greater than it
	std::uniform_real_distribution<double> distribution(0., fitness_sum);
	auto fixed = distribution(generator_);
	auto index = -1; // Initialized to -1 so the while loop that increments index will start at index = 0
	while(partial_sum < fixed) {
		++index;
		partial_sum += pop_[index]->get_fitness();
	}
	return pop_[index];
}
