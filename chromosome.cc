/*
 * Implementation for Chromosome class
 */

#include <algorithm>
#include <cassert>
#include <random>
#include <chrono> // For unique generator_ seeds

#include "chromosome.hh"

//////////////////////////////////////////////////////////////////////////////
// Generate a completely random permutation from a list of cities
Chromosome::Chromosome(const Cities* cities_ptr)
	: cities_ptr_(cities_ptr),
	  order_(random_permutation(cities_ptr->size())),
	  generator_(std::chrono::system_clock::now().time_since_epoch().count()) {
	
	assert(is_valid());
}

//////////////////////////////////////////////////////////////////////////////
// Clean up as necessary
Chromosome::~Chromosome() {
	assert(is_valid());
}

//////////////////////////////////////////////////////////////////////////////
// Perform a single mutation on this chromosome
void Chromosome::mutate() {
	assert(is_valid());
	
	// Generates two random integers, which are indexes in order_
	std::uniform_int_distribution<int> distribution(0, order_.size() - 1);
	auto pt1 = distribution(generator_);
	auto pt2 = distribution(generator_);
	while(pt1 == pt2) {
		pt2 = distribution(generator_);
	}

	// Swaps the two city indexes
	std::iter_swap(order_.begin() + pt1, order_.begin() + pt2);
	assert(is_valid());
}

//////////////////////////////////////////////////////////////////////////////
// Note: this method allocates memory for the new offsprings
std::pair<Chromosome*, Chromosome*> Chromosome::recombine(const Chromosome* other) {
	assert(is_valid());
	assert(other->is_valid());
	
	// Generates two random integers for the consecutive subset
	std::uniform_int_distribution<int> distribution(0, order_.size() - 1);
	unsigned b = distribution(generator_);
	unsigned e = distribution(generator_);
	while(e == 0) { e = distribution(generator_); }
	while(b >= e) { b = distribution(generator_); }
	
	// Creates the two children and ensures they aren't identical to their parents
	auto child1 = create_crossover_child(other, this, b, e);
	while(child1 == other || child1 == this) {
		child1 = create_crossover_child(other, this, b, e);
	}
	assert(child1->is_valid());

	auto child2 = other->create_crossover_child(this, other, b, e);
	while(child2 == this || child2 == other) {
		child2 = other->create_crossover_child(this, other, b, e);
	}
	assert(child2->is_valid());
	
	return std::make_pair(child1, child2);
}
//////////////////////////////////////////////////////////////////////////////
// For an ordered set of parents, return a child using the ordered crossover.
// The child will have the same values as p1 in the range [b,e),
// and all the other values in the same order as in p2.
Chromosome* Chromosome::create_crossover_child(const Chromosome* p1,
		const Chromosome* p2,
		unsigned b,
		unsigned e) const {
	Chromosome* child = p1->clone();
	
	// We iterate over both parents separately, copying from parent1 if the
	// value is within [b,e) and from parent2 otherwise
	unsigned i = 0, j = 0;
	
	for ( ; i < p1->order_.size() && j < p2->order_.size(); ++i) {
		if (i >= b and i < e) {
			child->order_[i] = p1->order_[i];
		} else { // Increment j as long as its value is in the [b,e) range of p1
			while (p1->is_in_range(p2->order_[j], b, e)) {
				++j;
			}
			assert(j < p2->order_.size());
			child->order_[i] = p2->order_[j];
			j++;
		}
	}
	
	assert(child->is_valid());
	return child;
}

// Return a positive fitness value, with higher numbers representing
// fitter solutions (shorter total-city traversal path).
double Chromosome::get_fitness() const {
	return 50 / calculate_total_distance();
}

// A chromosome is valid if it has no repeated values in its permutation,
// as well as no indices above the range (length) of the chromosome.
// We implement this check with a sort, which is a bit inefficient, but simple
bool Chromosome::is_valid() const {
	// Sorts the chromosome, which should result in a vector [0, 1, ...] up to the size of the Cities object
	// perm_values used because order_ is const from Cities
	auto perm_values = order_;
	std::sort(perm_values.begin(), perm_values.end());
	auto range = cities_ptr_->size();
	
	// Checks that every city in the permutation is represented once and only once; if not, returns false
	for(unsigned i = 0; i < perm_values.size(); ++i) {
		if(i > range) { return false; }
		if(i != perm_values[i]) { return false; }
	}
	return true;
}

// Find whether a certain value appears in a given range of the chromosome.
// Returns true if value is within the specified the range specified
// [begin, end) and false otherwise.
bool Chromosome::is_in_range(unsigned value, unsigned begin, unsigned end) const {
	assert(is_valid());
	
	// Finds the iterator pointing to the certain value, then calculates its index
	auto val = std::find(order_.begin(), order_.end(), value);
	auto val_pos = std::distance(order_.begin(), val);
	
	// Checks if the value's index is between [begin, end)
	if(val_pos >= begin && val_pos < end) { return true; }
	return false;
}
