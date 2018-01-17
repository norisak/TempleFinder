#include <stdio.h>
#include <time.h>

// Constants used by the random number generator
#define MULTIPLIER 0x5DEECE66DLL
#define ADDEND 11LL
#define MASK 0xFFFFFFFFFFFFLL

// Report seeds with at least this many mending books.
#define MENDING_THRESHOLD 3


// The seed limit when using 48 bits
const long long seed_modulus = 281474976710656LL; // 2⁴⁸


// An implementation of Java's random number generator used by Minecraft.
static int next(long long *seed, int bits){
	*seed = (*seed * MULTIPLIER + ADDEND) & MASK;
	return *seed >> (48 - bits);
}

static void setSeed(long long *seed, long long newSeed){
	*seed = newSeed ^ MULTIPLIER;
}

static void advance(long long *seed){
	*seed = (*seed * MULTIPLIER + ADDEND) & MASK;
}

static int nextInt(long long *seed, int n){
	return next(seed, 31) % n;
}

static int nextIntPure(long long *seed){
	return next(seed, 31);
}

static long long nextLong(long long *seed){
	return ((long long)(next(seed, 32)) << 32) + next(seed, 32);
}

static int getInt(long long *seed, int min, int max){
	return nextInt(seed, max-min+1) + min;
}


static int check_loottableseed(long long loottableseed, int *mending_count){
	long long initial_seed = loottableseed;
	loottableseed = loottableseed ^ MULTIPLIER;
	long long *seed = &loottableseed;

	int num_rolls = getInt(seed, 2, 4);  // Number of items to get from the first loot pool


	for (int i = 0; i < num_rolls; ++i){
		int roll = nextInt(seed, 232);  // Roll which items we want to choose. 232 is the sum of weights

		if (roll < 125){  // Contains useless items that also use the RNG to roll a random count
			advance(seed);
			continue;
		}
		if (roll >= 175 && roll < 195){  // We got an enchanted book
			// Roll which enchantment to put on the book. There are 30 possible enchantments.
			int enchantment = nextInt(seed, 30);
			int level = 1;

			// Roll for enchantment level, if the enchantment can have levels
			if (enchantment != 6  && enchantment != 10 && enchantment != 19 && enchantment != 24 &&
				enchantment != 25 && enchantment < 28){
					level = nextIntPure(seed);  // The actual enchantment level will be 1 + (level % max_level)
				}

			if (enchantment == 28){  // Mending.
				/**
				 *  Note that these are not the internal enchantment IDs, but rather the enchantment's
				 *  position in the enchantment registery.
				 *  See enchantment_ids.txt for which enchantments correspond to which IDs.
				 * 
				**/
				*mending_count += 1;
			}

		}
	}

	return 0;
	
}


static void check_temple(long long world_seed, int x, int z){
	// Temples can generate once per 512 * 512 block area. x and z specify which such area we're at.

	// Some code found in minecrafts source code to "randomize" the RNG state.
	long long random_seed = (long long)x * 341873128712LL + (long long)z * 132897987541LL + world_seed + 14357617LL;
	
	// When using java.util.Random setSeed method, it xors the seed you give it with the MULTIPLIER.
	random_seed ^= MULTIPLIER;

	long long *seed = &random_seed;

	// Convert the coordinates into chunk coordinates. This is the position where a temple can generate.
	x = x*32 + nextInt(seed, 24);
	z = z*32 + nextInt(seed, 24);

	// Also found in minecraft source code. This computes the seed used when populating a chunk.
	setSeed(seed, world_seed);
	long long a = nextLong(seed) / 2LL * 2LL + 1LL;
	long long b = nextLong(seed) / 2LL * 2LL + 1LL;

	long long chunk_populate_seed = (long long)x * a + (long long)z * b ^ world_seed;
	
	setSeed(seed, chunk_populate_seed);

	// The number of mending books found at this temple
	int mending = 0;
	int *mending_ptr = &mending;

	// Generate the 4 chests and check their loot for mending books
	check_loottableseed(nextLong(seed), mending_ptr);
	check_loottableseed(nextLong(seed), mending_ptr);
	check_loottableseed(nextLong(seed), mending_ptr);
	check_loottableseed(nextLong(seed), mending_ptr);


	
	if (mending >= MENDING_THRESHOLD){
		printf("%d mending at %lld\n", mending, world_seed);
	}
}



void seed_search(){
	clock_t cs = clock();
	printf("\nInitializing seed search\n\n");
	
    
    for (long long seed = 0; seed < 100000000; ++seed){
        check_temple(seed, 0, 0);
    }


	clock_t deltatime = clock()-cs;
	double timetaken = ((double)deltatime)/((double)CLOCKS_PER_SEC);

	printf("Search complete, took %f sec\n",timetaken);
}


int main(int argc, char **argv) {

    seed_search();

	return 0;
}

