#pragma once

#include <stack>

#include "str.hpp"
#include "token.hpp"
#include "error.hpp"

struct Tokenizer {
	unsigned char Get_Priorty(char Op) {
		switch(Op) {
			case '*':case '/':case '%': 		return 5;
			case '+':case '-': 					return 4;
			case '<':case '>':case '~': 		return 3;
			case '!':case '|':case '&':case '^':return 2;
			case ':':case '=': 					return 1;
			default: 							return 0;
		}
	}
	Token* _Identifier(Str& Input,Token*& Identifier) {
		if (Identifier == 0) return 0;
		Token* Result = Identifier;
		Identifier = 0;
		Result->Value.len = Input.str - Result->Value.str;
		while (Result->Value.str[0] == ' ' ||
			   Result->Value.str[0] == '\n' ||
			   Result->Value.str[0] == '\t') {
			if (Result->Value.len==0) break;
			Result->Value >> 1;
		}
		while (Result->Value.str[Result->Value.len-1] == ' ' ||
			   Result->Value.str[Result->Value.len-1] == '\n' ||
			   Result->Value.str[Result->Value.len-1] == '\t') {
			Result->Value << 1;
			if (Result->Value.len==0) break;
		}
		
		if (Result->Value.len == 0) {
			delete Result;
			return 0;
		}
		return Result;
	}

	Token* _String(Str& Input) {
		Input >> 1;
		Token* Result = new Token(Input,5);
		while (Input.len) {
			if (Input.str[0] == '"') break;
			if (Input.str[0] == '\\') Input >> 1;
			Input >> 1;
		}
		Result->Value.len = Input.str - Result->Value.str;
		return Result;
	}

	Token* _Expression(Str& Input) {
		Token* Result = new Token(Input,1); // a.k.a Node stack
		Token* Identifier = 0; // make it a int first
		std::stack<Token*> OpStack;
		while (Input.len) {
			switch(Input.str[0]) {
				case ';':case ',':
				Input >> 1;
				return Result;

				case '}':case ']':case ')':
				return Result;
				
				case '(':
				*Result << _Identifier(Input,Identifier);
				if (Result->ChildEnd)
					if (Result->ChildEnd->Type!=6) {
						*Result << _In_Bracket(Input, ')',1);
						break;
					}
				*Result < _Expression(Input);
				break;

				case '[':
				*Result << _Identifier(Input,Identifier);
				if (Result->ChildEnd)
					if (Result->ChildEnd->Type!=6) {
						*Result << _In_Bracket(Input, ']',4);
						break;
					}
				*Result < _In_Bracket(Input, ']', 3);
				break;
				
				case '{':
				*Result << _In_Bracket(Input,'}',0);
				break;

				case '"':
				*Result << _String(Input);
				break;

				case '.':
				*Result << _Identifier(Input,Identifier);
				break;

				case '*':case '/':case '%':
				case '+':case '-':
				case '<':case '>':case '~':
				case '!':case '|':case '&':case '^':
				case ':':case '=':
				while (!OpStack.empty()) {
						if (Get_Priorty(OpStack.top()->Value.str[0])
						  < Get_Priorty(Input.str[0])) break;
						if (Result->ChildEnd == 0) break;
						*OpStack.top() <
							Result->ChildEnd->detach();
						if (Result->ChildEnd)
							*OpStack.top() <
								Result->ChildEnd->detach();
						*OpStack.top() <
							OpStack.top()->ChildBegin->detach(); //Swap
						*Result << OpStack.top()->detach();
						OpStack.pop();
					}
				OpStack.push(new Token(Str(Input.str,1),6));
				*Result << _Identifier(Input,Identifier);
				*Result << OpStack.top(); //Insert placeholder
				break;

				case '0':case '1':case '2':case '3':case '4':
				case '5':case '6':case '7':case '8':case '9':
				if (Identifier == 0) 
					Identifier = new Token(Input,8); //Integer
				break;
				
				default:
				if (Identifier == 0)
					Identifier = new Token(Input,7); //Identifier
				else Identifier->Type = 7;
				break;
			}
			Input >> 1;
		}
		*Result << _Identifier(Input,Identifier);
		//Insert remaining operator
		while (!OpStack.empty())
   		{
   			if (Result->ChildEnd == 0) break;
   			*OpStack.top() < 
   				Result->ChildEnd->detach();
   			if (Result->ChildEnd->Prev)
   				*OpStack.top() <
   					Result->ChildEnd->Prev->detach();
   			*OpStack.top() <
   				OpStack.top()->ChildBegin->detach();
   			*Result << OpStack.top()->detach();
   			OpStack.pop();
		}
		return Result;
	}
	Token* _In_Bracket(Str& Input,
					   unsigned char EndChar,
					   unsigned char Type) {
		Token* Result = new Token(Input,Type);
		char* OldPos = Input.str;
		while (Input.len) {
			*Result < _Expression(Input);
			if (Input.str[0] == EndChar) break;
			if (Input.str == OldPos) break; //corrupted input
			OldPos = Input.str;
		}
		Result->Value.len = Input.str - Result->Value.str;
		return Result;
	}
	Token* operator()(Str Input) {
		return _In_Bracket(Input,'}',0);
	}
};
