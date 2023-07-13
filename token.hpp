#pragma once
#include "str.hpp"

//Type definition
//0: Code
//1: Expression
//2: Function
//3: Array
//4: Index
//5: String
//6: Operator
//7: Identifier
//8: Integer
//9: Decimal

struct Token {
	unsigned char Type;
	Str Value;
	Token* PreExec;
	Token(Str Value,unsigned char Type,Token* PreExec=0)
		:Value(Value),Type(Type),PreExec(PreExec){};

	Token* Next = 0;
	Token* Prev = 0;

	Token* ChildBegin = 0;
	Token* ChildEnd = 0;

	Token* Parent = 0;

	inline Token& operator<(Token* inp) {
		if (inp) {
			if (ChildEnd) ChildEnd->Next = inp;
			else ChildBegin = inp;
			inp->Parent = this;
			inp->Prev = ChildEnd;
			ChildEnd = inp;
		}
		return *this;
	}
	
	inline Token* detach() {
		if (Next) Next->Prev = Prev;
		if (Prev) Prev->Next = Next;
		if (Parent) {
			if (Parent->ChildBegin == this) 
				Parent->ChildBegin = Next;
			if (Parent->ChildEnd == this)
				Parent->ChildEnd = Prev;
		}
		Next = Prev = Parent = 0;
		return this;
	}

	inline Token& operator<<(Token* inp) {
		if (inp) {
			if (this->ChildEnd) {
				if (this->ChildEnd->Type==8) 
					if (inp->Type==8) {
						this->ChildEnd->Value.len =
							inp->Value.str - 
							this->ChildEnd->Value.str + 
							inp->Value.len;
						this->ChildEnd->Type=9;
						return *this;
					}
				if (this->ChildEnd->Type!=6&&inp->Type!=6)
					if (!(this->ChildEnd->Type==1&&inp->Type==1))
						inp->PreExec = this->ChildEnd->detach();
			}
		}
		return (*this)<inp;
	}
};
