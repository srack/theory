/* Samantha Rack
 * CSE 30151 - Theory of Computing
 * Project 1
 * main.cpp
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <string>
#include <vector>

#include "nfa.h"

using namespace std;

/// MAIN ///
int main(int argc, char **argv) {
	/* check user arguments */
	if (argc != 2) {
		cout << "Usage: " << argv[0] << "<filename>\n";
		return 1;	
	}

	/* load the nfa from the provided file */
	nfa machine;
	if (machine.loadFromFile(argv[1]) != 0) {
		return 1;
	}

	/* read in set of input lines */
	string numInputLines;
	cin >> numInputLines;
	int i, j, k;
	vector<string> inputLines;
	for (i = 0; i < atoi(numInputLines.c_str()); ++i) {
		string line;
		cin >> line;
		inputLines.push_back(line);
	}

	/* process each of the input lines */
	for (i = 0; i < inputLines.size(); ++i) {
		if (i > 0) cout << endl;
		// reset the machine
		machine.reset();
		// print the status update
		cout << "; ";
		machine.printCurrentStates();
		cout << endl;

		// advance the machine for each input
		string currentInput = "";
		for (j = 0; inputLines[i][j] != '\0'; ++j) {
			if (inputLines[i][j] == ',') {
				machine.advance(currentInput);
				//print the status update
				cout << currentInput << "; ";
				machine.printCurrentStates();
				cout << endl;
				currentInput = "";
			}
			else {
				currentInput += inputLines[i][j];
			}
		}
		if (currentInput.size() > 0) {
			machine.advance(currentInput);
			//print the status update
			cout << currentInput << "; ";
			machine.printCurrentStates();
			cout << endl;
		}
		// check if machine is accepting and print result
		if (machine.isAccepting()) { 
			cout << "ACCEPT\n";
		} else {
			cout << "REJECT\n";
		}
	}


}

