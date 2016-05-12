/* Samantha Rack
 * CSE 30151 - Theory of Computing
 * Project 1
 * nfa.h
 */
 
#ifndef NFA_H
#define NFA_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <cerrno>

using namespace std;

class nfa {
	public:
		nfa();	
		int loadFromFile(string);
		void reset();
		int isAccepting();
		void advance(string);
		void advanceOne(string, string);
		void advanceEmpty();
		void printCurrentStates();
	
	private:
		//set of symbols in the machine's alphabet	
		vector<string> alphabet;	

		//set of states in the machine		
		vector<string> states;		

		//start state of the machine
		string startState;			

		//set of accept states of the machine
		vector<string> acceptStates;	

		//transition function for machine in form of a matrix of states
		// and alphabet inputs, each entry of the matrix is a set of 
		// states corresponding to the transition for a state, input pair
		vector<vector <vector <string> > > transitionF;

		vector<string> currentStates;
};


#endif
