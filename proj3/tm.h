/* Adrian Gerbaud and Samantha Rack
 * CSE 30151 - Theory of Computing
 * Project 3
 * tm.h
 */
 
#ifndef TM_H
#define TM_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <cerrno>

#define MAX_STEPS 1000

using namespace std;

struct transition {
	bool valid;
	string nextState;
	string tapeWrite;
	string direction;

	// constructor for default of the transition
	transition() {
		valid = false;
	}
};

class turingMachine {
	public:
		turingMachine();	// done
		int loadFromFile(string);	// done
		int reset(vector<string>);	// done
		int status();	// done
		int exceededSteps();	//done
		void advance();
		void printTrace();		// done
	
	private:
		//set of symbols in the machine's alphabet	
		vector<string> inputAlph;
		vector<string> tapeAlph;

		// current tape
		vector<string> tape;
		// index into tape vector, current location of the tm's head	
		int tHead;

		//set of states in the machine		
		vector<string> states;		
		//start state of the machine
		string startState;			
		//accept and reject states (exactly one of each allowed)
		string acceptState;
		string rejectState;
		// tracks where the machine currently is
		string currentState;

		//transition function is a list of each of the states that contains
		//	a list of transitions from it
		vector<vector <struct transition> > transitionF;

		// when this exceeds 1000, the machine is assumed to not halt on the given input
		int stepCount;
};


#endif
