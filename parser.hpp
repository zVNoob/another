#pragma once

#include <stack>

#include "error.hpp"
#include "str.hpp"
#include "token.hpp"

class Parser {
  public:
	ErrorList Err;
	Token *Result = 0;

  private:
	unsigned char Get_Priorty(char Op) {
		// clang-format off
		switch (Op) {
		case '!':
			return 6; // Why??
		case '*':case '/':case '%':
			return 5;
		case '+':case '-':
			return 4;
		case '<':case '>':case '~':
			return 3;
		case '|':case '&':case '^':
			return 2;
		case ':':case '=':
			return 1;
		default:
			return 0;
		}
		// clang-format on
	}
	Token *_Identifier(Str &Input, Token *&Identifier) {
		if (Identifier == 0)
			return 0;
		Token *Result = Identifier;
		Identifier = 0;
		Result->Value.len = Input.str - Result->Value.str;
		while (Result->Value.str[Result->Value.len - 1] == ' ' ||
			   Result->Value.str[Result->Value.len - 1] == '\n' ||
			   Result->Value.str[Result->Value.len - 1] == '\t') {
			Result->Value << 1;
			if (Result->Value.len == 0)
				break;
		}

		if (Result->Value.len == 0) {
			delete Result;
			return 0;
		}
		return Result;
	}

	Token *_String(Str &Input) {
		Token *Result = new Token(Input, 5);
		while (Input.len) {
			if (Input.str[0] == '"')
				break;
			if (Input.str[0] == '\\')
				Input >> 1;
			Input >> 1;
		}
		Result->Value.len = Input.str - Result->Value.str;
		return Result;
	}

	Token *_Expression(Str &Input) {
		Token *Result = new Token(Input, 1); // a.k.a Node stack
		Token *Identifier = 0;				 // make it a int first
		std::stack<Token *> OpStack;
		bool HasError = false;
		while (Input.len) {
			switch (Input.str[0]) {
			// clang-format off
			case ';':case ',':case '}':case ']':case ')':
				// clang-format on
				goto _Expression_EndPoint;

			case '(':
				*Result << _Identifier(Input, Identifier);
				Input >> 1;
				if (Result->ChildEnd)
					if (Result->ChildEnd->Type != 6) {
						*Result << _In_Bracket(Input, ')', 2);
						break;
					}
				*Result < _Expression(Input);
				break;

			case '[':
				*Result << _Identifier(Input, Identifier);
				Input >> 1;
				if (Result->ChildEnd)
					if (Result->ChildEnd->Type != 6) {
						*Result << _In_Bracket(Input, ']', 4);
						break;
					}
				*Result < _In_Bracket(Input, ']', 3);
				break;

			case '{':
				Input >> 1;
				*Result << _In_Bracket(Input, '}', 0);
				break;

			case '"':
				Input >> 1;
				*Result << _String(Input);
				break;

			case '.':
				*Result << _Identifier(Input, Identifier);
				break;

			case '@':
				*Result << new Token(Str(Input.str, 1), 10);
				break;
			// clang-format off
			case '*':case '/':case '%':
			case '+':case '-':
			case '<':case '>':case '~':
			case '!':case '|':case '&':case '^':
			case ':':case '=':
				// clang-format on
				*Result << _Identifier(Input, Identifier);
				while (!OpStack.empty()) {
					if (Get_Priorty(OpStack.top()->Value.str[0]) <
						Get_Priorty(Input.str[0]))
						break;
					if (OpStack.top()->Next == 0)
						break;
					*OpStack.top() < OpStack.top()->Next->detach();
					if (OpStack.top()->Prev) {
						if (OpStack.top()->Prev->Type != 6)
							*OpStack.top() < OpStack.top()->Prev->detach();
						else if (OpStack.top()->Prev->ChildEnd)
							*OpStack.top() < OpStack.top()->Prev->detach();
					}
					*OpStack.top() <
						OpStack.top()->ChildBegin->detach(); // Swap
					*Result << OpStack.top()->detach();
					OpStack.pop();
				}
				OpStack.push(new Token(Str(Input.str, 1), 6));
				*Result << OpStack.top(); // Insert placeholder
				break;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':

				if (Identifier == 0)
					Identifier = new Token(Input, 8); // Integer
				break;
			case ' ':
			case '\n':
			case '\t':
				break;
			default:
				if (Identifier == 0)
					Identifier = new Token(Input, 7); // Identifier
				else
					Identifier->Type = 7;
				break;
			}
			Input >> 1;
		}
	_Expression_EndPoint:
		*Result << _Identifier(Input, Identifier); // Add final ID
		Result->Value.len = Input.str - Result->Value.str;
		if (Input.len)
			if (Input.str[0] == ';' || Input.str[0] == ',')
				Input >> 1; // Small hack
							// Insert remaining operator
		while (!OpStack.empty()) {
			if (OpStack.top()->Next == 0) {
				Err.Add(new Error(OpStack.top(), (char *)"Expected operand"));
				OpStack.pop();
				HasError = true;
				continue;
			}
			*OpStack.top() < OpStack.top()->Next->detach();
			if (OpStack.top()->Prev) {
				if (OpStack.top()->Prev->Type != 6)
					*OpStack.top() < OpStack.top()->Prev->detach();
				else if (OpStack.top()->Prev->ChildEnd)
					*OpStack.top() < OpStack.top()->Prev->detach();
			}
			*OpStack.top() < OpStack.top()->ChildBegin->detach(); // Swap
			*Result << OpStack.top()->detach();
			OpStack.pop();
		}
		// Optimise output if no error is found
		if (HasError == false) {
			Token *NewRes = Result->ChildBegin;
			if (NewRes)
				NewRes->detach();
			delete Result;
			Result = NewRes;
		}
		return Result;
	}
	Token *_In_Bracket(Str &Input, unsigned char EndChar, unsigned char Type) {
		Token *Result = new Token(Input, Type);
		char *OldPos = Input.str;
		while (Input.len) {
			*Result < _Expression(Input);
			if (Input.len) {
				if (Input.str[0] == EndChar)
					break;
				if (Input.str == OldPos) { // corrupted input
					Err.Add(
						new Error(Result->ChildEnd, (char *)"Undefined syntax"));
					Input >> 1; // force skip
				}
			}
			OldPos = Input.str;
		}
		Result->Value.len = Input.str - Result->Value.str;
		return Result;
	}
	Str Strip_Comment(Str Input) {
		char *Temp = new char[Input.len];
		bool Is_Comment = false;
		long len = 0;
		for (long i = 0; i < Input.len; i++) {
			if (Input.str[i] == '#')
				Is_Comment = true;
			if (Input.str[i] == '\n')
				Is_Comment = false;
			if (Is_Comment == false)
				Temp[len++] = Input.str[i];
		}
		char *Result = new char[len];
		for (long i = 0; i < Input.len; i++)
			Result[i] = Temp[i];
		delete[] Temp;
		return Str(Result, len);
	}

  public:
	Token *operator()(Str Input) {
		Result = _In_Bracket(Input, '}', 0);
		return Result;
	}
	~Parser() {
		delete Result;
	}
};
