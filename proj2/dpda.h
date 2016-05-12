/* Samantha Rack
 * CSE 30151 - Theory of Computing
 * Project 1
 * dpda.h
 */
 
#ifndef DPDA_H
#define DPDA_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <cerrno>

using namespace std;

struct oneTransition {
	string input;
	string stackPop;

	string stackPush;
	string nextState;
};

class dpda {
	public:
		dpda();	
		int loadFromFile(string);
		void reset();
		int isAccepting();
		void advance(string);
		void advanceEmpty();
		int checkDeterministic();
		void printTransition(string, string, string, string);
	
	private:
		//set of symbols in the machine's alphabet	
		vector<string> inputAlph;
		vector<string> stackAlph;

		// stack
		stack<string> mStack;

		//set of states in the machine		
		vector<string> states;		

		//start state of the machine
		string startState;			

		//set of accept states of the machine
		vector<string> acceptStates;	

		//transition function is a list of each of the states that contains
		//	a list of transitions from it
		vector<vector <struct oneTransition> > transitionF;

		string currentState;
};


#endif
