#pragma once

#include "token.hpp"

struct Error {
	Token* Where;
	char* What;
	Error* Prev = 0;
	Error* Next = 0;
	Error(Token* Where, char* What = 0) :
	Where(Where),What(What) {}
};

struct ErrorList {
	Error* First=0;
	Error* Last=0;
	void Add(Error* Inp) {
		if (First == 0) {
			First = Inp;
			Last = Inp;
		}
		if (Inp->Where == Last->Where) 
			Last->What = Inp->What;
		else {
			Inp->Prev = Last;
			Last->Next = Inp;
			Last = Inp;
		}
	};
	~ErrorList() {
		while (Last) {
			Error* temp = Last;
			Last = Last->Prev;
			delete temp;
		}
	}
};
