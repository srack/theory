/* Adrian Gerbaud and Samantha Rack
 * CSE 30151 - Theory of Computing
 * Project 3
 * tm.cpp
 */

#include "tm.h"

#define DEBUG 0


/* Function:	turingMachine constructor
 * Arguments:	none
 */
turingMachine::turingMachine() {
	// user should call loadFromFile in order to set up the TM
}


/* Function:	loadFromFile
 * Arguments:	filename - txt file containing the TM description to be loaded
 * Returns:		0 on success, 1 with any error with the description file
 * Purpose:		populates the TM object by reading the TM description file
 */
int turingMachine::loadFromFile(string filename) {
	// pass in the filename of the tm description
	// returns 0 on success, 1 on any failure
	
	vector<string> tempTs;		// hold transition lines of the file until all states read
	
	// open the file for reading tm description
	FILE *fDescr = fopen(filename.c_str(), "r");
	if (fDescr == NULL) {
		cout << "fopen: " << strerror(errno) << endl;
		return 1;
	}	
	
	// descrLine[0] => what part of the tm the line is describing
	// descrLine[2] => beginning of the set/description
	char descrLine[1024];	
	
	/////////////////////
	// populate the TM //
	/////////////////////
	while (fgets(descrLine, sizeof(descrLine), fDescr) != NULL){
		int i;
		int next = 1;
		switch(descrLine[0]) {
			// set of input alphabet symbols
			case 'A':
				for (i = 2; descrLine[i] != '\n' && descrLine[i] != '\0'; ++i) {
					// on a comma the previous symbol has been defined
					if (descrLine[i] == ',') {
						next = 1;
					}
					// after a comma, a new symbol is added to the alphabet vector
					else if (next) {
						next = 0;
						string temp;
						temp.push_back(descrLine[i]);
						inputAlph.push_back(temp);
					}
					// otherwise, concatenate the next char to build the current symbol 
					else {
						inputAlph[inputAlph.size() - 1].push_back(descrLine[i]);
					}
				}
				break;

			// set of tape alphabet symbols
			case 'Z':
				for (i = 2; descrLine[i] != '\n' && descrLine[i] != '\0'; ++i) {
					// on a comma the previous symbol has been defined
					if (descrLine[i] == ',') {
						next = 1;
					}
					// after a comma, a new symbol is added to the alphabet vector
					else if (next) {
						next = 0;
						string temp;
						temp.push_back(descrLine[i]);
						tapeAlph.push_back(temp);
					}
					// otherwise, concatenate the next char to build the current symbol 
					else {
						tapeAlph[tapeAlph.size() - 1].push_back(descrLine[i]);
					}
				}
				break;

			//set of states
			case 'Q':
				for (i = 2; descrLine[i] != '\n' && descrLine[i] != '\0'; ++i) {
					// on a comma the previous symbol has been defined
					if (descrLine[i] == ',') {
						next = 1;
					}
					// after a comma, a new symbol is added to the states vector
					else if (next) {
						next = 0;
						string temp;
						temp.push_back(descrLine[i]);
						states.push_back(temp);
					}
					// otherwise, concatenate the next char to build the current symbol 
					else {
						states[states.size() - 1].push_back(descrLine[i]);
					}
				}
				break;

			//transition rule -- temporary vector for this to deal with after the rest of the information
			case 'T':
				tempTs.push_back((string)descrLine);
				break;

			//start state
			case 'S':
				// build the string for the start state
				for (i = 2; descrLine[i] != '\n' && descrLine[i] != '\0'; ++i) {
					startState.push_back(descrLine[i]);
				}
				break;

			//accept state and reject state
			case 'F':
				next = 0;
				for (i = 2; descrLine[i] != '\n' && descrLine[i] != '\0'; ++i) {
					// on a comma the previous symbol has been defined
					if (descrLine[i] == ',') {
						if (next) {
							// error, more than two states specified
							cout << "Invalid TM description: More than one accept state or reject state.\n";
							return 1;
						}					
						next = 1;	// use next to indicate that already have gotten a comma (should only be one)
					}
					// after a comma, the reject state is the next thing seen
					else if (next) {
						rejectState.push_back(descrLine[i]);
					}
					// otherwise, still reading the accept state 
					else {
						acceptState.push_back(descrLine[i]);
					}
				}
				break;

			//error in the description file, inform the user and exit
			default:
				cout << "Unknown line in the TM description file: " << descrLine << endl;
				return 1;
				break;
		}		
	}


	//////////////////////////////////////
	// check for the validity of the TM //
	//////////////////////////////////////
	int valid, i, j;
	
	// is set of states empty?
	if (states.size() == 0) {
		cout << "Invalid TM description: Set of states is empty.\n";
		return 1;
	}
	
	// is either alphabet empty?
	if (inputAlph.size() == 0 || tapeAlph.size() == 0) {
		cout << "Invalid TM description: Alphabet is empty.\n";
		return 1;
	}

	// does the tape alphabet contain the input alphabet?
	// does the input alphabet contain " "? -- it should not
	for (i = 0; i < inputAlph.size(); ++i) {
		// check for blank character
		if (inputAlph[i] == " ") {
			cout << "Invalid TM description: Input alphabet cannot contain \" \".\n";
			return 1;
		}
		// check that the tape alphabet has each input character
		valid = 0;
		for (j = 0; j < tapeAlph.size(); ++j) {
			if (inputAlph[i] == tapeAlph[j]) {
				valid = 1;
				break;
			}
		}
		if (!valid) {
			cout << "Invalid TM description: Tape alphabet does not contain all characters of the input alphabet.\n";
			return 1;
		}
	} 

	// does the tape alphabet contain " "? -- it should
	valid = 0;
	for (i = 0; i < tapeAlph.size(); ++i) {
		if (tapeAlph[i] == " ") {
			valid = 1;
			break;
		}
	}
	if (!valid) {
		cout << "Invalid TM description: Tape alphabet does not contain \" \".\n";
		return 1;
	}

	// is the start state a member of Q?
	valid = 0;
	for (i = 0; i < states.size(); ++i) {
		if (states[i] == startState) {
			valid = 1;
			break;
		}
	}
	if (!valid) {
		cout << "Invalid TM description: Start state not in set of states.\n";
		return 1;
	}

	// are the accept and reject states in Q and different?
	valid = 0;
	for (i = 0; i < states.size(); ++i) {
		if (states[i] == acceptState) {
			++valid;
		} 
		// use an else if to ensure that they aren't the same state
		else if (states[i] == rejectState) {
			++valid;
		}
	}
	if (valid != 2) {
		cout << "Invalid TM description: Accept and/or reject state not in set of states or are the same state.\n";
		return 1;
	}
	
	/////////////////////////////////////////////////////
	// fill in the transitions function data structure //
	/////////////////////////////////////////////////////
	int k;

	/// transitionF[currentState][currentTape]:
	///  (1) is the empty string
	///  (2) contains a struct transition

	// initialize empty matrix with default structs
	for (i = 0; i < states.size(); ++i) {
		vector <struct transition> tempV;		
		for (j = 0; j < tapeAlph.size(); ++j) {
			struct transition tempT;
			tempV.push_back(tempT);			
		}
		transitionF.push_back(tempV);		
	}

	// for each transition, parse it and add it to the transition function
	for (i = 0; i < tempTs.size(); ++i) {
		j = 2;	//starting position for information from temp transitions

		/// extract the start state for the transition ///
		string tStart;
		while (tempTs[i][j] != ',') {
			tStart.push_back(tempTs[i][j]);
			++j;
		}
		// check if the tStart state is valid
		valid = 0;
		for (k = 0; k < states.size(); ++k) {
			if (tStart == states[k]) {
				valid = 1;
				break;
			}
		}
		if (tStart == acceptState || tStart == rejectState) valid = 0;
		if (!valid) {
			cout << "Invalid TM description: One or more transitions involves an invalid state: " << tempTs[i] << endl;
			return 1;
		}	



		/// extract the current tape symbol to be read for the transition ///
		string tRead;
		++j;	//move past the ','
		while (tempTs[i][j] != ',') {
			tRead.push_back(tempTs[i][j]);
			++j;
		}
		//check if the tRead is in the alphabet
		valid = 0;
		for (k = 0; k < tapeAlph.size(); ++k) {
			if (tRead == tapeAlph[k]) {
				valid = 1;
				break;
			}
		}
		if (!valid) {
			cout << "Invalid TM description: One or more transitions involves an invalid tape symbol: " << tempTs[i] << endl; 
			return 1;
		}

		/// extract the resulting state ///
		string tEnd;
		++j;
		while (tempTs[i][j] != ',') {
			tEnd.push_back(tempTs[i][j]);
			++j;
		}
		// check if tEnd is a valid state
		valid = 0;
		for (k = 0; k < states.size(); ++k) {
			if (tEnd == states[k]) {
				valid = 1;
				break;
			}
		}

		if (!valid) {
			cout << "Invalid TM description: One or more transitions involves an invalid state: " << tempTs[i] << endl;
			return 1;
		}


		/// extract the symbol to be written on the tape ///
		string tWrite;
		++j;	//move past the ','
		while (tempTs[i][j] != ',') {
			tWrite.push_back(tempTs[i][j]);
			++j;
		}
		// check if the tWrite symbol is valid
		valid = 0;
		for (k = 0; k < tapeAlph.size(); ++k) {
			if (tWrite == tapeAlph[k]) {
				valid = 1;
				break;
			}
		}
		if (!valid) {
			cout << "Invalid TM description: One or more transitions involves an invalid tape symbol: " << tempTs[i] << endl;
			return 1;
		}	


		/// extract the direction to the machine will move in ///
		string tDir;
		++j;
		while (tempTs[i][j] != '\n' && tempTs[i][j] != '\0') {
			tDir.push_back(tempTs[i][j]);
			++j;
		}
		// check if the tDir is R or L
		if (tDir != "R" && tDir != "L") {		
			cout << "Invalid TM description: One or more transitions involves an invalid direction: " << tempTs[i] << endl;
			return 1;
		}

		// for transitions, valid == false when there is no transition for that pair
		struct transition t;
		t.valid = true;
		t.nextState = tEnd;
		t.tapeWrite = tWrite;
		t.direction = tDir;

		// put it in the transition function matrix
		for (j = 0; j < states.size(); ++j) {
			if (tStart == states[j]) {
				for (k = 0; k < tapeAlph.size(); ++k) {
					if (tRead == tapeAlph[k]) {
						if (transitionF[j][k].valid == true) {
							cout << "Given TM description is nondeterministic. More than one transition for " << tStart << ", " << tRead << " pair.\n";
							return 1;
						}
						transitionF[j][k] = t;
						break;
					}
				}
				break;
			}
		}				
	}

	////////////////////////////////////
	// DEBUG -- PRINT THE TRANSITIONS //
	////////////////////////////////////
	#if DEBUG
	for (i = 0; i < states.size(); ++i) {
		for (j = 0; j < tapeAlph.size(); ++j) {
			if (transitionF[i][j].valid == true) {
				cout << states[i] << ", " << tapeAlph[j] << ", " << transitionF[i][j].nextState << ", " << transitionF[i][j].tapeWrite << ", " << transitionF[i][j].direction << endl;
			}
		}
	}
	#endif

	return 0;	//success
}


/* Function:	reset
 * Arguments:	none
 * Returns:	0 on success, 1 with an error in the input 
 * Purpose:	Resets the machine to the beginning, with only the states in
 * 		E(start state) and the stack empty.
 */
int turingMachine::reset(vector<string> startTape) {
	// verify that the input tape is valid
	int i, j;
	int valid;
	for (i = 0; i < startTape.size(); ++i) {
		valid = 0;
		for (j = 0; j < inputAlph.size(); ++j) {
			if (startTape[i] == inputAlph[j]) {
				valid = 1;
				break;
			}
		}
		if (!valid) {
			//okay if the user uses " " in the input
			if (startTape[i] != " ") {
				cout << "Invalid symbol on input tape: " << startTape[i] << endl;
				return 1;
			}
		}
	}

	tape = startTape;
	tHead = 0;

	stepCount = 0;

	currentState = startState;

	return 0;
}


/* Function:	status
 * Arguments:	none
 * Returns:		0 - not finished, 1 - accepting, 2 - rejecting
 */
int turingMachine::status() {
	if (currentState == acceptState) return 1;
	else if (currentState == rejectState) return 2;
	else return 0;
}

/* Function:	exceededSteps
 * Arguments:	none
 * Returns:		0 if have stepCount < MAX_STEPS, 1 otherwise
 */
int turingMachine::exceededSteps() {
	if (stepCount < MAX_STEPS) return 0;
	else return 1;
}


/* Function:	advance
 * Arguments:	none
 * Returns:		void
 * Purpose:		Transitions the machine forward one step based on the current state
 * 				and the current tape symbol being read.
 */
void turingMachine::advance() {
	if (currentState == "") return;

	int change = 0;

	int stateI, tapeI;
	for (stateI = 0; stateI < states.size(); ++stateI) {
		if (currentState == states[stateI]) break;
	}
	for (tapeI = 0; tapeI < tapeAlph.size(); ++tapeI) {
		if (tape[tHead] == tapeAlph[tapeI]) break;
	}

	// look in the transition function matrix for the transition structure
	if (transitionF[stateI][tapeI].valid) {
		currentState = transitionF[stateI][tapeI].nextState;
		tape[tHead] = transitionF[stateI][tapeI].tapeWrite;
		if (transitionF[stateI][tapeI].direction == "R") {
			tHead++;
		} else {
			if (tHead != 0) --tHead;	//stay where are if already furthest possible to left
		}
	} 
	else {	// no transition available
		currentState = rejectState;
		// the tape doesn't change
		++tHead;	// assume move right
	}

	// check if the tape vector representation has to be expanded
	while (tHead >= tape.size()) {
		// add more blanks to the tape
		tape.push_back(" ");
	}

	// increment the number of steps completed so far
	++stepCount;

}




/* Function:	printTrace
 * Arguments:	none
 * Returns:		void
 * Purpose:		Print to stdin the current state of the machine
				FORMAT -- (<symbols before head>)<currentState>(<symbols at and after the head>)
 */
void turingMachine::printTrace() {
	// print the symbols before the head
	cout << "(";
	int i;	
	int first = 1;
	for (i = 0; i < tHead; ++i) {
		if (i != 0) cout << ",";
		cout << tape[i];	
	}
	cout << ")";

	// print the current state in the middle
	cout << currentState;
	
	// print the symbols at and after the head until rightmost non-blank characters
	cout << "(";
	for (i = tHead; i < tape.size(); ++i) {
		if (tape[i] == " ") {
			int j;
			int valid = 0;
			// check if there are any non-blank characters after this one
			for (j = i; j < tape.size(); ++j) {
				if (tape[j] != " ") {
					valid = 1;
					break;
				}
			}
			if (!valid) break;
		}
		// if the character is non-blank or if there are non-blank characters after it, then print it
		if (i != tHead) cout << ",";
		cout << tape[i];		
		
	}
	cout << ")";

	cout << endl;

}
