// STL
#include <iostream>
#include <list>

// Base type
#include "error.hpp"
#include "object_operator.hpp"
#include "str.hpp"
#include "token.hpp"

// Parser
#include "parser.hpp"

// Executer
#include "executer.hpp"
#include "object.hpp"
#include "object_function.hpp"

// Core library

using namespace std;

ostream &operator<<(ostream &os, Str &inp) {
	for (int i = 0; i < inp.len; i++) {
		switch (inp.str[i]) {
		case '\n':
			os << "\\n";
		case '\\':
			os << "\\\\";
		default:
			os << inp.str[i];
		}
	}
	return os;
}

Error *Err = 0;

ostream &operator<<(ostream &os, Token &T) // AST pretty printer
{
	const string norm = "255;255;255";
	const string table[11] = {
		"255;150;50",
		"255;100;50",
		"255;255;50",
		"127;255;50",
		"50;255;50",
		"50;255;127",
		"50;255;255",
		"200;127;255",
		"100;100;255",
		"127;50;255",
		"255;255;255",
	};
	const string tree = "150;150;150";
	static list<bool> depth;
	bool Error = false;
	if (Err)
		if (&T == Err->Where)
			Error = true;
	int ErrLen = 0;
	if (Error) {
		os << "\x1b[38;2;255;50;50m" << (int)T.Type << ":\"" << T.Value << "\""
		   << "\x1b[38;2;255;100;100m // " << Err->What;
		ErrLen = strlen(Err->What) + 4;
		Err = Err->Next;
	} else {
		os << "\x1b[38;2;" << norm << 'm' << (int)T.Type << ":\"";
		os << "\x1b[38;2;" << table[T.Type] << "m" << T.Value;
		os << "\x1b[38;2;" << norm << "m\"";
	}
	if (T.ChildBegin)
		depth.push_back(1);
	if (T.PreExec) {
		int i = T.Value.len + 6 + (T.ChildBegin == 0) + ErrLen;
		for (int k = 0; k < i; k++)
			depth.push_back(false);
		os << "\x1b[38;2;" << tree << "m - " << *T.PreExec;
		while (i--)
			depth.erase(--depth.end());
	} else
		os << '\n';
	os << "\x1b[38;2;" << tree << 'm';
	if (T.ChildBegin) {
		Token *P = T.ChildBegin;
		while (P) {
			for (auto i : depth)
				os << ((i) ? "│" : " ");
			os << "\x1b[D";
			if (P == T.ChildEnd) {
				*(--depth.end()) = 0;
				os << "└";
			} else
				os << "├";
			os << *P;
			P = P->Next;
		}
		depth.pop_back();
	}
	return os;
}

#include "corelib_core.hpp"
#include "corelib_env.hpp"

inline void Register_All(Object &Shared) {
	corelib::Register_Core(Shared);
	corelib::Register_Env(Shared);
}

void Print_Thrown(Object *Thrown) {
	if (dynamic_cast<Object_Int *>(Thrown)) {
		cout << "(int) " << static_cast<Object_Int *>(Thrown)->Value;
		return;
	}
	if (dynamic_cast<Object_Decimal *>(Thrown)) {
		cout << "(decimal) " << static_cast<Object_Decimal *>(Thrown)->Value;
		return;
	}
	if (dynamic_cast<Object_String *>(Thrown)) {
		cout << "(string) \"" << static_cast<Object_String *>(Thrown)->Value << "\"";
		return;
	}
	if (dynamic_cast<Object_Array *>(Thrown)) {
		cout << "(array) [";
		for (auto &i : static_cast<Object_Array *>(Thrown)->Value) {
			Print_Thrown(i);
			cout << ", ";
		}
		cout << "]";
		return;
	}
	cout << "(unknown)";
}

int main() {
	string s;
	getline(std::cin, s);
	Token *Res;
	Parser tokenizer;
	Str inp(const_cast<char *>(s.c_str()), s.size());
	Res = tokenizer(inp);
	Err = tokenizer.Err.First;
	if (Err) {
		cout << *Res;
		return 1;
	}
	// Env setup
	Object_Env Env;
	Env.Deletable = false;
	Object Shared;
	Env.Shared = &Shared;
	Shared.Deletable = false;
	Object_Array Args;
	Args.Deletable = false;
	Env.Args = &Args;
	Register_Operators(Shared);
	Register_All(Shared);
	// Execution
	Object *EntryPoint = Execute(Res, Env);
	EntryPoint->Deletable = false;
	Env.Self = EntryPoint;
	try {
		Object *Result = EntryPoint->OnCall(Args, Env);
		if (Result)
			if (Result->Deletable)
				delete Result;
		delete EntryPoint;
	} catch (Error &e) {
		Err = &e;
		delete EntryPoint;
		cout << *Res;
	} catch (Object *e) {
		delete EntryPoint;
		cout << "Terminated after throwing: ";
		Print_Thrown(e);
		cout << '\n';
		delete e;
	}
	return 0;
}
