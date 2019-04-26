These are all the files needed to implement a genetic algorithm used to solve the Traveling Salesman problem.

---
The Chromosome class is similar to the Cities class, but is usable in the context of the genetic algorithm.

The is_valid() function tests that a Chromosome's permutation (taken from a Cities object) does not skip or repeat
any elements. It does so by sorting a copy of the permutation and running through each element, checking that
every consecutive element corresponds with an incrementing index.

The calculate_fitness() method is relatively simple. To ensure that larger distances have lower fitness, but still
maintain practical values, the fitness is inversely proportional to the distance, multiplied by a factor of 50.

The Chromosome constructor uses the chrono library to access the current system time, which produces a unique seed
for the random generator_ for each Chromosome. The default destructor prevents any memory leaks.

The mutate() method uses a uniform_int_distribution object to select two random indexes from the Chromosome's
permutation and then swaps them.

The recombine() method also uses uniform_int_distribution to randomly choose a range of indexes, from which the
consecutive subset of cities will be taken from one parent to give to the child. Then, two children are created
from two parents, while ensuring that the two children aren't identical to their parents or each other. This also
uses the helper methods create_crossover_child() and is_in_range(). is_in_range() functions by finding the index
of the desired value, and returning true if the index is between the given subset indexes.

---
The Deme class creates a population of diverse Chromosomes, gradually producing a more fit population.

The constructor uses the same chrono library to initialize the random number generator_. Then, for every value of
a given pop_size, it creates a Chromosome. Since the created Chromosome only has a scope of the constructor, the
Deme creates a clone of the constructor to allocate space and make a reference to it, then adds it to the pop_
vector. The destructor then runs through the same pop_ vector, and, if there is a remaining Chromosome, deletes it.
This ensures that no memory leaks.

The get_best() method finds the fittest Chromosome in the Deme at a given time by checking through the pop_ vector
and returning the Chromosome with the highest get_fitness().

The select_parent() method finds a random Chromosome, with a preference towards more fit Chromosomes, using Fitness
Proportionate selection. It calculates and sums the fitness of every Chromosome in pop_, then generates a random
number between 0 and the sum of the fitnesses. Then, it adds Chromosomes' fitness to a partial sum and returns the
Chromosome whose fitness pushed the partial sum over the random number. Thus, Chromosomes with a higher fitness are
more likely to be selected.

The compute_next_generation() method replaces the population in Deme with a new one, built from the original
population as parents. Iterating through the population size, two parents are selected based on fitness, and a
random number is generated; if it is less than the rate of mutation, then the parent is mutated. From there, the two
parents are recombined to form children, and the children are added to a new pop_ vector. This is repeated until
the entire population has been replaced, and then the original pop_ vector is replaced with the new. As the old
Chromosomes fall out of scope, the destructor deallocates their memory.

---
As shown in "comparison.gif", the genetic algorithm did produce a more efficient path than the randomized, but not
to the extent desired.
