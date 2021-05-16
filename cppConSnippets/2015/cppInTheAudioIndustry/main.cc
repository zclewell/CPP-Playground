#include <unistd.h>

#include <iostream>

#include "no_block.hh"

using namespace std;

static int counter = 0;

void incrCounter(int i) {
	counter += i;
}

int main() {

	NoBlock<int, 0, incrCounter> nb;
	nb.start();

	for (int i = 1; i < 10; ++i) {
		nb.send(i);
	}

	nb.stop();

	cout << counter << endl;

	return 0;
}
