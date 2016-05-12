/* Adrian Gerbaud and Samantha Rack
 * CSE 30151 - Theory of Computing
 * Project 3
 * main.cpp
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <string>
#include <vector>

#include "tm.h"

using namespace std;

/// MAIN ///
int main(int argc, char **argv) {
	/* check user arguments */
	if (argc != 2) {
		cout << "Usage: " << argv[0] << " <filename>\n";
		return 1;	
	}

	/* load the tm from the provided file */
	turingMachine machine;
	if (machine.loadFromFile(argv[1]) != 0) {
		return 1;
	}

	/* read in set of input lines */
	char numInputLines[10];
	cin.getline(numInputLines, 10);
	int i, j, k;
	vector<string> inputLines;
	for (i = 0; i < atoi(numInputLines); ++i) {
		char linec[1000];
		cin.getline(linec, 1000);
		string line(linec);
		inputLines.push_back(line);
	}

	/* process each of the input lines */
	for (i = 0; i < inputLines.size(); ++i) {
		if (i > 0) cout << endl;

		// fill in a vector for the current input
		vector<string> inputTape;
		string currentInput = "";
		for (j = 0; inputLines[i][j] != '\0'; ++j) {
			if (inputLines[i][j] == ',') {
				inputTape.push_back(currentInput);
				currentInput = "";
			}
			else {
				currentInput += inputLines[i][j];
			}
		}
		if (currentInput != "") inputTape.push_back(currentInput);

		// reset the machine with the provided input
		if (machine.reset(inputTape) != 0) {
			 return 1;
		}
		// output the original configuration of the machine
		machine.printTrace();

		// let the machine run
		while (machine.status() == 0 && !machine.exceededSteps()) {
			// have machine take next step
			machine.advance();

			// output the state of the machine after that step
			machine.printTrace();
		}

		// print result of the machine
		if (machine.status() == 1)
			cout << "ACCEPT\n"; 
		else if (machine.status() == 2)
			cout << "REJECT\n";
		else 
			cout << "DID NOT HALT\n";
	}

}

