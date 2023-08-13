#pragma once

#include "token.hpp"

struct Error {
	Token *Where;
	char *What;
	Error *Prev = 0;
	Error *Next = 0;
	Error(Token *Where, const char *What = 0) : Where(Where), What(const_cast<char *>(What)) {}
};

struct ErrorList {
	Error *First = 0;
	Error *Last = 0;
	void Add(Error *Inp) {
		if (First == 0) {
			First = Inp;
			Last = Inp;
		} else if (Inp->Where == Last->Where) {
			Last->What = Inp->What;
			delete Inp;
		} else {
			Inp->Prev = Last;
			Last->Next = Inp;
			Last = Inp;
		}
	};
	~ErrorList() {
		while (First) {
			Error *temp = First;
			First = First->Next;
			delete temp;
		}
	}
};
