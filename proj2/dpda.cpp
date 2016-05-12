#include "dpda.h"

#define DEBUG 0 


/* Function:	dpda constructor
 * Arguments:	none
 */
dpda::dpda() {
	// nothing done in this constructor
	// user should call loadFromFile in order to set up the NFA
}


/* Function:	loadFromFile
 * Arguments:	filename - txt file containing the NFA description to be loaded
 * Returns:	0 on success, 1 with any error with the description file
 * Purpose:	populates the NFA object by reading the NFA description file
 */
int dpda::loadFromFile(string filename) {
	// pass in the filename of the dpda description
	// returns 0 on success, 1 on any failure
	
	vector<string> tempTs;		// hold transition lines of the file until all states read
	
	// open the file for reading dpda description
	FILE *fDescr = fopen(filename.c_str(), "r");
	if (fDescr == NULL) {
		cout << "fopen: " << strerror(errno) << endl;
		return 1;
	}	
	
	// descrLine[0] => what part of the dpda the line is describing
	// descrLine[2] => beginning of the set/description
	char descrLine[64];	
	
	///////////////////////
	// populate the DPDA //
	///////////////////////
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

			// set of stack alphabet symbols
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
						stackAlph.push_back(temp);
					}
					// otherwise, concatenate the next char to build the current symbol 
					else {
						stackAlph[stackAlph.size() - 1].push_back(descrLine[i]);
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

			//transition rule
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

			//set of accept states
			case 'F':
				for (i = 2; descrLine[i] != '\n' && descrLine[i] != '\0'; ++i) {
					// on a comma the previous symbol has been defined
					if (descrLine[i] == ',') {
						next = 1;
					}
					// after a comma, a new symbol is added to the accept states vector
					else if (next) {
						next = 0;
						string temp;
						temp.push_back(descrLine[i]);
						acceptStates.push_back(temp);
					}
					// otherwise, concatenate the next char to build the current symbol 
					else {
						acceptStates[acceptStates.size() - 1].push_back(descrLine[i]);
					}
				}
				break;

			//error in the description file, inform the user and exit
			default:
				cout << "Unknown line in the nfa description file: " << descrLine << endl;
				return 1;
				break;
		}		
	}
	

	///////////////////////////////////////
	// check for the validity of the DPDA //
	///////////////////////////////////////
	int valid, i, j;
	
	// is Q empty?
	if (states.size() == 0) {
		cout << "Invalid DPDA description: Set of states is empty.\n";
		return 1;
	}
	
	// is either alphabet empty?
	if (inputAlph.size() == 0 || stackAlph.size() == 0) {
		cout << "Invalid DPDA description: Alphabet is empty.\n";
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
		cout << "Invalid DPDA description: Start state not in set of states.\n";
		return 1;
	}

	// are all of the accept states members of Q?
	for (i = 0; i < acceptStates.size(); ++i) {
		valid = 0;
		for (j = 0; j < states.size(); ++j) {
			if (states[j] == acceptStates[i]) {
				valid = 1;
				break;
			}
		}
		if (!valid) {
			cout << "Invalid NFA description: One or more accept states not in set of states.\n";
			return 1;
		}
	}

	
	/////////////////////////////////////////////////////
	// fill in the transitions function data structure //
	/////////////////////////////////////////////////////
	int k;

	/// transitionF[currentState] = a vector that:
	///  (1) is empty, no transition out of currentState on that input
	///  (2) contains a struct oneTransition with input, stackPush, stackPop, and nextState

	// initilize transition matrix of states and input and stack alphabet symbols with empty string lists
	for (i = 0;i < states.size(); ++i) {
		vector<struct oneTransition> tStateTemp; 
		transitionF.push_back(tStateTemp);
	}	


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
		if (!valid) {
			cout << "Invalid DPDA description: One or more transitions involves an invalid state: " << tempTs[i] << endl;
			return 1;
		}	

		/// extract the input for the transition ///
		string tInput;
		++j;	//move past the ','
		while (tempTs[i][j] != ',') {
			tInput.push_back(tempTs[i][j]);
			++j;
		}
		//check if the tInput is in the alphabet
		valid = 0;
		for (k = 0; k < inputAlph.size(); ++k) {
			if (tInput == inputAlph[k]) {
				valid = 1;
				break;
			}
		}
		if (!valid) {
			//check if it is the empty string
			if (tInput != "e") {
				cout << "Invalid DPDA description: One or more transitions involves an invalid input symbol: " << tempTs[i] << endl; 
				return 1;
			}
		}

		/// eact the stack symbol to be popped ///
		string tStackPop;
		++j;
		while (tempTs[i][j] != ',') {
			tStackPop.push_back(tempTs[i][j]);
			++j;
		}
		// check if tStackPop is in the stack alphabet
		valid = 0;
		for (k = 0; k < stackAlph.size(); ++k) {
			if (tStackPop == stackAlph[k]) {
				valid = 1;
				break;
			}
		}
		if (!valid) {
			if (tStackPop != "e") {
				cout << "Invalid DPDA description: One or more transitions involves an invalid stack symbol: " << tempTs[i] << endl;
				return 1;
			}
		}


		/// extract the end state for the transition ///
		string tEnd;
		++j;	//move past the ','
		while (tempTs[i][j] != ',') {
			tEnd.push_back(tempTs[i][j]);
			++j;
		}
		// check if the tEnd state is valid
		valid = 0;
		for (k = 0; k < states.size(); ++k) {
			if (tEnd == states[k]) {
				valid = 1;
				break;
			}
		}
		if (!valid) {
			cout << "Invalid DPDA description: One or more transitions involves an invalid state: " << tempTs[i] << endl;
			return 1;
		}	


		/// extract the stack symbol to be pushed ///
		string tStackPush;
		++j;
		while (tempTs[i][j] != '\n' && tempTs[i][j] != '\0') {
			tStackPush.push_back(tempTs[i][j]);
			++j;
		}
		// check if the tStackPush character is valid
		valid = 0;
		for (k = 0; k < stackAlph.size(); ++k) {
			if (tStackPush == stackAlph[k]) {
				valid = 1;
				break;
			}
		}
		if (!valid) {
			if (tStackPush != "e") {
				cout << "Invalid DPDA description: One or more transitions involves an invalid stack symbol: " << tempTs[i] << endl;
				return 1;
			}
		}

		struct oneTransition t;
		t.input = tInput;
		t.stackPop = tStackPop;
		t.stackPush = tStackPush;
		t.nextState = tEnd;

		for (j = 0; j < states.size(); ++j) {
			if (tStart == states[j]) {
				transitionF[j].push_back(t);
				break;
			}
		}				
	}

	////////////////////////////////////
	// DEBUG -- PRINT THE TRANSITIONS //
	////////////////////////////////////
	#if DEBUG
	for (i = 0; i < states.size(); ++i) {
		cout << endl << states[i] << endl;
		for (j = 0; j < transitionF[i].size(); ++j) {
			cout << "\t" << transitionF[i][j].input << "," << transitionF[i][j].stackPop << "," << transitionF[i][j].stackPush << "," << transitionF[i][j].nextState << endl;
		}		

		cout << endl;
	}
	#endif

	if (!checkDeterministic() ) {
		cout << "PDA is not deterministic.\n";
		return 1;
	}

	return 0;	//success
}

/* FUnction:	checkDeterministic
 * Arguments:	none
 * Returns:		1 or 0
 * Purpose: 	Checks if the transitions out of each state define deterministic behavior for
 * 		the push down automata
 */
int dpda::checkDeterministic() {
	int i, j, k;
	for (i = 0; i < states.size(); ++i) {
		// check for e,e transition with existance of another transition
		for (j = 0; j < transitionF[i].size(); ++j) {
			if (transitionF[i][j].input == "e" && transitionF[i][j].stackPop == "e" && transitionF[i].size() > 1) return 0;
		}

		// check for rest of invalid combinations
		for (j = 0; j < transitionF[i].size(); ++j) {
			for (k = j+1; k < transitionF[i].size(); ++k) {
				if (transitionF[i][j].input == transitionF[i][k].input) {
					// input and stack pop are equivalent
					if (transitionF[i][j].stackPop == transitionF[i][k].stackPop) return 0;
					// input equivalent and at least 1 empty stackPop
					else if (transitionF[i][j].stackPop == "e" || transitionF[i][j].stackPop == "e") return 0;
				}
				else if (transitionF[i][j].stackPop == transitionF[i][k].stackPop) {
					// stack pop equivalent and at least 1 input empty string
					if (transitionF[i][j].input == "e" || transitionF[i][k].input == "e") return 0;
				}
				else if (transitionF[i][j].input == "e" && transitionF[i][k].stackPop == "e") return 0;
				else if (transitionF[i][j].stackPop == "e" && transitionF[i][k].input == "e") return 0;
			}
		}
	}

	return 1;
}

/* Function:	reset
 * Arguments:	none
 * Returns:	void
 * Purpose:	Resets the machine to the beginning, with only the states in
 * 		E(start state) and the stack empty.
 */
void dpda::reset() {
	int i;
	while (!mStack.empty()) {
		mStack.pop();
	}

	currentState = startState;

	// move to any state accessible via the empty string and the current stack
	advanceEmpty();
}


/* Function:	isAccepting
 * Arguments:	none
 * Returns:	1 for true, 0 for false
 * Purpose:	Returns true if the nfa is currently in an accepting state
 */
int dpda::isAccepting() {
	int i, j;
	for (i = 0; i < acceptStates.size(); ++i) {
		if (currentState == acceptStates[i]) return 1;
	}
	return 0;
}


/* Function:	advance
 * Arguments:	input - symbol to input into the machine
 * Returns:	void
 * Purpose:	Updates the currentState to reflect advancing the 
 * 		machine on the given input (and any empty strings)
 */
void dpda::advance(string input) {
	if (currentState == "") return;

	int change = 0;

	int stateIndex;
	for (stateIndex = 0; stateIndex < states.size(); ++stateIndex) {
		if (currentState == states[stateIndex]) break;
	}

	int i;
	// check each of the transitions for a match
	for (i = 0; i < transitionF[stateIndex].size(); ++i) {
		// if the input matches, then check the top of the stack
		if (transitionF[stateIndex][i].input == input) {
			// if the transition is without reading the stack, then complete the 
			// 	transition
			if (transitionF[stateIndex][i].stackPop == "e") {
				string prevState = currentState;
				currentState = transitionF[stateIndex][i].nextState;
				if (transitionF[stateIndex][i].stackPush != "e") {
					mStack.push(transitionF[stateIndex][i].stackPush);
				}
				change = 1;

				printTransition(prevState, input, "e", currentState);

				break;
			// if the character to read matches the top of the stack, pop the stack
			// 	and complete the transition
			} else if (!mStack.empty() && transitionF[stateIndex][i].stackPop == mStack.top()) {
				mStack.pop();
				string prevState = currentState;
				currentState = transitionF[stateIndex][i].nextState;
				if (transitionF[stateIndex][i].stackPush != "e") {
					mStack.push(transitionF[stateIndex][i].stackPush);
				}
				change = 1;

				printTransition(prevState, input, transitionF[stateIndex][i].stackPop, currentState);

				break;
			}
		}
	}

	if (!change) {
		// indicates that this input failed on the machine
		currentState = "";
	}

	// after checking for transition on an input character, check for any advance
	// 	that can be done on the empty string (with or without reading the stack)
	advanceEmpty();
}



/* Function: 	advanceEmpty
 * Arguments:	none
 * Returns:	void
 * Purpose:	Updates currentStates vector to include any states that can be
 * 		reached via the empty string.
 */
void dpda::advanceEmpty() {
	if (currentState == "") return;

	int change = 1;
		
	while (change) {
		change = 0;
		
		int stateIndex;
		for (stateIndex = 0; stateIndex < states.size(); ++stateIndex) {
			if (currentState == states[stateIndex]) break;
		}

		int i;
		// check each of the transitions for a match
		for (i = 0; i < transitionF[stateIndex].size(); ++i) {
			// if the input matches, then check the top of the stack
			if (transitionF[stateIndex][i].input == "e") {
				// if the transition is without reading the stack, then complete the 
				// 	transition
				if (transitionF[stateIndex][i].stackPop == "e") {
					string prevState = currentState;
					currentState = transitionF[stateIndex][i].nextState;
					if (transitionF[stateIndex][i].stackPush != "e") {
						mStack.push(transitionF[stateIndex][i].stackPush);
					}
					change = 1;

					printTransition(prevState, "e", "e", currentState);			

					break;
				// if the character to read matches the top of the stack, pop the stack
				// 	and complete the transition
				} else if (!mStack.empty() && transitionF[stateIndex][i].stackPop == mStack.top()) {
					mStack.pop();
					string prevState = currentState;
					currentState = transitionF[stateIndex][i].nextState;
					if (transitionF[stateIndex][i].stackPush != "e") {
						mStack.push(transitionF[stateIndex][i].stackPush);
					}
					change = 1;

					printTransition(prevState, "e", transitionF[stateIndex][i].stackPop, currentState);

					break;
				}
			}
		}
	}

}

/* Function:	printCurrentStates
 * Arguments:	none
 * Returns:	void
 * Purpose:	Print to stdin the current states separated by a comma
 */
void dpda::printTransition(string startState, string input, string stackRead, string endState) {
	cout << startState << "; " << input << "; " << stackRead << "; " << endState << "; ";
	int i;

	stack<string> temp;

	int first = 1;
	while(!mStack.empty()) {
		if (first) {
			first = 0;
			cout << mStack.top();
		} else {
			cout << "," << mStack.top();
		}

		temp.push(mStack.top());
		mStack.pop();
	}
	while (!temp.empty()) {
		mStack.push(temp.top());
		temp.pop();
	}
	cout << endl;

}
