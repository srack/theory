#include "nfa.h"

#define DEBUG 0

/* Function:	nfa constructor
 * Arguments:	none
 */
nfa::nfa() {
	// nothing done in this constructor
	// user should call loadFromFile in order to set up the NFA
}


/* Function:	loadFromFile
 * Arguments:	filename - txt file containing the NFA description to be loaded
 * Returns:	0 on success, 1 with any error with the description file
 * Purpose:	populates the NFA object by reading the NFA description file
 */
int nfa::loadFromFile(string filename) {
	// pass in the filename of the nfa description
	// returns 0 on success, 1 on any failure
	
	vector<string> tempTs;		// hold transition lines of the file until all states read
	
	// open the file for reading nfa description
	FILE *fDescr = fopen(filename.c_str(), "r");
	//FILE *fDescr = fopen("testFiles/nfa1.txt", "r");
	if (fDescr == NULL) {
		cout << "fopen: " << strerror(errno) << endl;
		return 1;
	}	
	
	// descrLine[0] => what part of the nfa the line is describing
	// descrLine[2] => beginning of the set/description
	char descrLine[64];	
	
	//////////////////////
	// populate the NFA //
	//////////////////////
	while (fgets(descrLine, sizeof(descrLine), fDescr) != NULL){
		int i;
		int next = 1;
		switch(descrLine[0]) {
			//set of alphabet symbols
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
						alphabet.push_back(temp);
					}
					// otherwise, concatenate the next char to build the current symbol 
					else {
						alphabet[alphabet.size() - 1].push_back(descrLine[i]);
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
	// check for the validity of the NFA //
	///////////////////////////////////////
	int valid, i, j;
	
	// is Q empty?
	if (states.size() == 0) {
		cout << "Invalid NFA description: Set of states is empty.\n";
		return 1;
	}
	
	// is the alphabet empty?
	if (alphabet.size() == 0) {
		cout << "Invalid NFA description: Alphabet is empty.\n";
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
		cout << "Invalid NFA description: Start state not in set of states.\n";
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

	/// transitionF[currentState][input] = a vector that:
	///  (1) is empty, no transition out of currentState on that input
	///  (2) contains one or more states to which the machine transitions on the input

	// initilize transition matrix of states and alphabet symbols with empty string lists
	for (i = 0; i < states.size(); ++i) {
		vector<vector <string> > tStateTemp;
		for (j = 0; j < alphabet.size() + 1; ++j) {		// +1 for the empty string
			vector <string> tAlphaTemp;
			tStateTemp.push_back(tAlphaTemp);
		}
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
			cout << "Invalid NFA description: One or more transitions involves an invalid state: " << tempTs[i] << endl;
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
		for (k = 0; k < alphabet.size(); ++k) {
			if (tInput == alphabet[k]) {
				valid = 1;
				break;
			}
		}
		if (!valid) {
			//check if it is the empty string
			if (tInput != "e") {
				cout << "Invalid NFA description: One or more transitions involves an invalid input symbol: " << tempTs[i] << endl; 
				return 1;
			}
		}

		/// extract the end state for the transition ///
		string tEnd;
		++j;	//move past the ','
		while (tempTs[i][j] != '\n' && tempTs[i][j] != '\0') {
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
			cout << "Invalid NFA description: One or more transitions involves an invalid state: " << tempTs[i] << endl;
			return 1;
		}	

		// fill in the transition data structure
		for (j = 0; j < states.size(); ++j) {
			if (tStart == states[j]) {
				if (tInput == "e") {
					transitionF[j][alphabet.size()].push_back(tEnd);
				} else {
					for (k = 0; k < alphabet.size(); ++k) {
						if (tInput == alphabet[k]) {
							transitionF[j][k].push_back(tEnd);
							break;
						}			
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
		cout << endl << states[i] << endl;
		for (j = 0; j < alphabet.size(); ++j) {
			cout << "\t" << alphabet[j] << ":\t";
			for (k = 0; k < transitionF[i][j].size(); ++k) {
				cout << transitionF[i][j][k] << "\t";
			}
			cout << endl;
		}
		cout << "\te:\t";
		for (k = 0; k < transitionF[i][j].size(); ++k) {
			cout << transitionF[i][j][k] << "\t";
		}
		cout << endl;
	}
	#endif

	reset();
	
	return 0;	//success
}


/* Function:	reset
 * Arguments:	none
 * Returns:	void
 * Purpose:	Resets the machine to the beginning, with only the states in
 * 		E(start states).
 */
void nfa::reset() {
	currentStates.clear();
	currentStates.push_back(startState);

	// add any states accessible via the empty string
	advanceEmpty();
}


/* Function:	isAccepting
 * Arguments:	none
 * Returns:	1 for true, 0 for false
 * Purpose:	Returns true if the nfa is currently in an accepting state
 */
int nfa::isAccepting() {
	int i, j;
	for (i = 0; i < acceptStates.size(); ++i) {
		for (j = 0; j < currentStates.size(); ++j) {
			if (currentStates[j] == acceptStates[i]) return 1;
		}
	}
	return 0;
}


/* Function:	advance
 * Arguments:	input - symbol to input into the machine
 * Returns:	void
 * Purpose:	Updates the currentStates vector to reflect advancing the 
 * 		machine on the given input (and any empty strings)
 */
void nfa::advance(string input) {
	vector<string> oldStates(currentStates);
	currentStates.clear();

	int i;
	for (i = 0; i < oldStates.size(); ++i) {
		advanceOne(oldStates[i], input);		
	}
	
	advanceEmpty();
}


void nfa::advanceOne(string state, string input) {
	// determine indices for the state,input pair
	int stateIndex, inputIndex;
	for (stateIndex = 0; stateIndex < states.size(); ++stateIndex) {
		if (state == states[stateIndex]) break;
	}
	if (input == "e") inputIndex = alphabet.size();
	else {
		for (inputIndex = 0; inputIndex < alphabet.size(); ++inputIndex) {
			if (input == alphabet[inputIndex]) break;
		}
		if (inputIndex == alphabet.size()) {
			cout << "Invalid input character: skipping this character.\n";
			return;
		}
	}

	// add to the currentStates vector for each of the states for which there is a transition
	//   on the state, input pair
	int i, j;
	for (i = 0; i < transitionF[stateIndex][inputIndex].size(); ++i) {
		string stateToAdd = transitionF[stateIndex][inputIndex][i];
		// check if the state is already in currentStates
		int inCurrent = 0;
		for (j = 0; j < currentStates.size(); ++j) {
			if (stateToAdd == currentStates[j]) {
				inCurrent = 1;
				break;
			}
		}
		// if the state is not already in currentStates, add it
		if (!inCurrent) currentStates.push_back(stateToAdd);
	}
}


/* Function: 	advanceEmpty
 * Arguments:	none
 * Returns:	void
 * Purpose:	Updates currentStates vector to include any states that can be
 * 		reached via the empty string.
 */
void nfa::advanceEmpty() {
	int oldSize = -1;
	int newSize = 0;
	// once the oldSize == new size, there are no further meaningful transitions
	//   on the empty string
	while (newSize != oldSize) {
		oldSize = currentStates.size();
		int i;
		for (i = 0; i < currentStates.size(); ++i) {
			advanceOne(currentStates[i], "e");
		}
		newSize = currentStates.size();
	}
}

/* Function:	printCurrentStates
 * Arguments:	none
 * Returns:	void
 * Purpose:	Print to stdin the current states separated by a comma
 */
void nfa::printCurrentStates() {
	int i;
	for (i = 0; i < currentStates.size(); ++i) {
		cout << currentStates[i];
		if (i < currentStates.size() - 1) cout << ",";
	}
}
