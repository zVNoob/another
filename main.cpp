#include "error.hpp"
#include "executer.hpp"
#include "object.hpp"
#include "object_function.hpp"
#include "parser.hpp"
#include "str.hpp"
#include "token.hpp"
#include <cstring>
#include <iostream>
#include <list>

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

Object *Internal_Print(Object_Array &Args, Object_Env &Env) {
	for (auto &i : Args.Value)
		switch (i->Type) {
		case 1:
			cout << static_cast<Object_Decimal *>(i)->Value;
			break;
		case 2:
			cout << static_cast<Object_Int *>(i)->Value;
			break;
		case 4:
			for (int in = 0; in < static_cast<Object_String *>(i)->Value.len; in++)
				cout << static_cast<Object_String *>(i)->Value.str[in];
			break;
		default:
			throw (char *)"Unwriteable";
		}
	return 0;
}

Object *Internal_Args(Object_Array &Args, Object_Env &Env) {
	return Env.Args;
}

Object *Internal_If(Object_Array &Args, Object_Env &Env) {
	if (Args.Value[1]->Type == 1)
		throw "Missing return value";
	Object *Cond = Args.Value[0];
	Object *Then = Args.Value[1];
	Object *Else = 0;
	if (Args.Value.size() > 2)
		Else = Args.Value[2];

	Object *RealCond = Cond->OnCast(2, Env);
	Object *Result;
	if (static_cast<Object_Int *>(RealCond)->Value)
		Result = Then;
	else
		Result = Else;
	if (RealCond->Deletable)
		delete RealCond;
	return Result->clone();
}

Object *Internal_Int(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 0)
		throw "Missing argument";
	Object *Input = Args.Value[0];
	if (Input->Type == 4) {
		Object_String *Temp = static_cast<Object_String *>(Input);
		string Str(Temp->Value.str, Temp->Value.len);
		return new Object_Int(stoi(Str));
	}
	Object *Result = Input->OnCast(2, Env);
	if (Result != Input)
		return Result;
	return Result->clone();
}

Object *Internal_Input(Object_Array &Args, Object_Env &Env) {
	std::string s;
	getline(std::cin, s);
	return new Object_String(Str(s.c_str(), s.size()));
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
	Object Shared;
	Env.Shared = &Shared;
	Shared.Deletable = false;
	Object_Array Args;
	Args.Deletable = false;
	Env.Args = &Args;
	// Shared setup
	Shared.AddChild(Str("print", 5), new Object_Internal_Function(Internal_Print));
	Shared.AddChild(Str("args", 4), new Object_Internal_Function(Internal_Args));
	Shared.AddChild(Str("if", 2), new Object_Internal_Function(Internal_If));
	Shared.AddChild(Str("int", 3), new Object_Internal_Function(Internal_Int));
	Shared.AddChild(Str("input", 5), new Object_Internal_Function(Internal_Input));
	// Execution
	Object *EntryPoint = Execute(Res, Env);
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
	}
	return 0;
}
