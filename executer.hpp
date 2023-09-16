#pragma once

#include "object.hpp"
#include "token.hpp"

#include "error.hpp"

static Object *Execute(Token *Input, Object_Env &Env);

#include "object_function.hpp"

static Object_Function *_0(Token *Input, Object_Env &Env) {
	return new Object_Function(Input);
}

static Object *_10(Token *Input, Object_Env &Env) {
	return Env.Shared;
}

static Object *_7(Token *Input, Object_Env &Env) {
	Object *NewEnv = &Env;
	if (Input->PreExec) {
		NewEnv = Env.Register_AutoFree(Execute(Input->PreExec, Env));
	}
	if (NewEnv == 0)
		throw Error(Input, "Cannot access member of null");
	Object *Result = NewEnv->Child[Input->Value];
	if (Result == 0)
		throw Error(Input, "Undefined member");
	Env.This = NewEnv;
	return Result;
}

static Object_Array *_3(Token *Input, Object_Env &Env) {
	Object_Array *Result = new Object_Array();
	Token *Iterator = Input->ChildBegin;
	while (Iterator) {
		try {
			Object *Temp = Execute(Iterator, Env);
			Result->AddValue(Temp);
		} catch (...) {
			delete Result;
			throw;
		}
		Iterator = Iterator->Next;
	}
	return Result;
}

static Object *_2(Token *Input, Object_Env &Env) {
	Object_Array *Args = _3(Input, Env);
	Args->Deletable = false;

	Object *Callee = 0;
	try {
		Callee = Env.Register_AutoFree(Execute(Input->PreExec, Env));
	} catch (...) {
		delete Args;
		throw;
	}
	if (Callee == 0) {
		delete Args;
		throw Error(Input, "Cannot call null");
	}
	try {
		Object *Result = Callee->OnCall(*Args, Env);
		delete Args;
		return Result;
	} catch (const char *Msg) {
		delete Args;
		throw Error(Input, Msg);
	} catch (...) {
		delete Args;
		throw;
	}
}

static Object *_4(Token *Input, Object_Env &Env) {
	Object_Array *Args = _3(Input, Env);
	Args->Deletable = false;

	Object *Callee = 0;
	try {
		Callee = Env.Register_AutoFree(Execute(Input->PreExec, Env));
	} catch (...) {
		delete Args;
		throw;
	}
	if (Callee == 0) {
		delete Args;
		throw Error(Input, "Cannot call null");
	}
	try {
		Object *Result = Callee->OnIndex(*Args, Env);
		delete Args;
		return Result;
	} catch (const char *Msg) {
		delete Args;
		throw Error(Input, Msg);
	} catch (...) {
		delete Args;
		throw;
	}
}

static Object *_6(Token *Input, Object_Env &Env) {
	if (Input->Value.str[0] == ':') {
		// Varible declaration
		if (Input->ChildBegin == Input->ChildEnd)
			throw Error(Input, "Missing varible name");
		if (Input->ChildBegin->PreExec)
			throw Error(Input, "Varible mustn't be child of other one");
		Object *Result = Execute(Input->ChildEnd, Env);
		Env.AddChild(Input->ChildBegin->Value, Result);
		return 0;
	}
	if (Input->Value.str[0] == '=') {
		// Assignment
		if (Input->ChildBegin == Input->ChildEnd)
			throw Error(Input, "Missing varible name");
		Object *Assigner = Execute(Input->ChildBegin, Env);
		if (Assigner == 0)
			throw Error(Input, "Cannot assign to null");
		if (Assigner->Deletable) {
			Env.Register_AutoFree(Assigner);
			throw Error(Input, "Cannot assign to a intermediate object");
		}
		Object *Assignee = Env.Register_AutoFree(Execute(Input->ChildEnd, Env));
		if (Assignee == 0)
			throw Error(Input, "Cannot assign null");
		Assigner->OnAssign(*Assignee, Env);
		return 0;
	}
	// Other operator
	Object_Array *Args = _3(Input, Env);
	Object *Caller = Env.Shared->Child[Input->Value];
	if (Caller == 0) {
		delete Args;
		throw Error(Input, "Operator not supported");
	}
	try {
		Object *Result = Caller->OnCall(*Args, Env);
		delete Args;
		return Result;
	} catch (const char *Msg) {
		delete Args;
		throw Error(Input, Msg);
	} catch (...) {
		delete Args;
		throw;
	}
}

#include <string>

static Object *_8_9(Token *Input, Object_Env &Env) {
	std::string Intermediate(Input->Value.str, Input->Value.len);
	if (Input->Type == 8)
		return new Object_Int(stoll(Intermediate));
	return new Object_Decimal(stold(Intermediate));
}

static Object *_5(Token *Input, Object_Env &Env) {
	char *Temp = new char[Input->Value.len];
	int len = 0;
	for (int i = 0; i < Input->Value.len; i++) {
		if (Input->Value.str[i] == '\\') {
			i++;
			if (Input->Value.str[i] == 'n')
				Temp[len++] = '\n';
			else if (Input->Value.str[i] == 't')
				Temp[len++] = '\t';
			else if (Input->Value.str[i] == '\\')
				Temp[len++] = '\\';
			else if (Input->Value.str[i] == '"')
				Temp[len++] = '\"';
			else
				Temp[len++] = Input->Value.str[i];
		} else
			Temp[len++] = Input->Value.str[i];
	}
	Object *Result = new Object_String(Str(Temp, len));
	delete[] Temp;
	return Result;
}

Object *Execute(Token *Input, Object_Env &Env) {
	switch (Input->Type) {
	case 0:
		return _0(Input, Env);
	case 10:
		return _10(Input, Env);
	case 7:
		return _7(Input, Env);
	case 3:
		return _3(Input, Env);
	case 2:
		return _2(Input, Env);
	case 4:
		return _4(Input, Env);
	case 5:
		return _5(Input, Env);
	case 8:
	case 9:
		return _8_9(Input, Env);
	case 6:
		return _6(Input, Env);
	default:
		throw Error(Input, "Unknown token");
	}
}
