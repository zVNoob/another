#pragma once

#include "error.hpp"
#include "object.hpp"
#include <string>

static Object *Execute(Token *Input, Object_Env &Env);

#include "object_function.hpp"

static Object_Function *_0(Token *Input, Object_Env &Env) {
	return new Object_Function(Input);
}

static Object *_7(Token *Input, Object_Env &Env) {
	Object *CurEnv = &Env;
	if (Input->PreExec) {
		CurEnv = Execute(Input->PreExec, Env);
		Env.This = CurEnv;
	}
	if (CurEnv == 0)
		throw Error(Input, (char *)"Cannot access null member");
	Object *Result = CurEnv->Child[Input->Value];
	if (Result == 0) {
		if (CurEnv->Deletable)
			delete CurEnv;
		throw Error(Input, (char *)"Undefined identifier");
	}
	if (CurEnv->Deletable) {
		Result = Result->clone();
		delete CurEnv;
	}

	return Result;
}

static Object *_10(Token *Input, Object_Env &Env) {
	return Env.Shared;
}

static Object_Array *_3(Token *Input, Object_Env &Env) {
	Object_Array *Result = new Object_Array();
	Token *Iterator = Input->ChildBegin;
	while (Iterator) {
		try {
			Object *Child = Execute(Iterator, Env);
			if (Child) {
				if (Child->Deletable == 0)
					Child = Child->clone();
				Child->Deletable = false;
				Result->Value.push_back(Child);
			}
		} catch (...) {
			delete Result;
			throw;
		}
		Iterator = Iterator->Next;
	}
	return Result;
}

static Object *_2(Token *Input, Object_Env &Env) {
	Object *Callee = Execute(Input->PreExec, Env);
	if (Callee == 0)
		throw Error(Input, (char *)"Cannot call null");
	Object_Array *Args = 0;
	try {
		Args = _3(Input, Env);
	} catch (...) {
		if (Callee->Deletable)
			delete Callee;
		throw;
	}
	Args->Deletable = false;
	try {
		Object *Result = Callee->OnCall(*Args, Env);
		delete Args;
		if (Callee->Deletable) {
			if (Result)
				if (Result->Deletable == 0)
					Result = Result->clone();
			delete Callee;
		}
		return Result;
	} catch (const char *s) {
		delete Args;
		if (Callee->Deletable)
			delete Callee;
		throw Error(Input, s);
	} catch (...) {
		delete Args;
		if (Callee->Deletable)
			delete Callee;
		throw;
	}
}

static Object *_4(Token *Input, Object_Env &Env) {
	Object *Callee = Execute(Input->PreExec, Env);
	if (Callee == 0)
		throw Error(Input, (char *)"Cannot call null");
	Object_Array *Args = 0;
	try {
		Args = _3(Input, Env);
	} catch (...) {
		if (Callee->Deletable)
			delete Callee;
		throw;
	}
	Args->Deletable = false;
	try {
		Object *Result = Callee->OnIndex(*Args, Env);
		delete Args;
		if (Callee->Deletable) {
			if (Result)
				if (Result->Deletable == 0)
					Result = Result->clone();
			delete Callee;
		}
		return Result;
	} catch (const char *s) {
		delete Args;
		if (Callee->Deletable)
			delete Callee;
		throw Error(Input, s);
	} catch (...) {
		delete Args;
		if (Callee->Deletable)
			delete Callee;
		throw;
	}
}

static Object *_6(Token *Input, Object_Env &Env) {
	if (Input->Value.str[0] == ':') {
		if (Input->ChildBegin == Input->ChildEnd)
			throw Error(Input, (char *)"Missing variable name");
		Env.AddChild(Input->ChildBegin->Value, Execute(Input->ChildEnd, Env));
		return 0;
	}
	if (Input->Value.str[0] == '=') {
		Object *Assigner = Execute(Input->ChildBegin, Env);
		if (Assigner == 0)
			throw Error(Input, (char *)"Cannot assign to null");
		if (Assigner->Deletable) {
			throw Assigner;
			throw Error(Input, (char *)"Cannot assign to constant");
		}
		Object *Result = 0;
		try {
			Result = Execute(Input->ChildEnd, Env);
			if (Result == 0)
				throw Error(Input, (char *)"Cannot assign null");
			Assigner->OnAssign(*Result, Env);
		} catch (...) {
			if (Result)
				if (Result->Deletable)
					delete Result;
			throw;
		}
		return 0;
	}
	Object *Callee = Env.Shared->Child[Input->Value];
	if (Callee == 0)
		throw Error(Input, (char *)"Undefined operator");
	Object_Array *Args = _3(Input, Env);
	try {
		Object *Result = Callee->OnCall(*Args, Env);
		delete Args;
		return Result;
	} catch (const char *e) {
		delete Args;
		throw Error(Input, e);
	} catch (...) {
		delete Args;
		throw;
	}
}

static Object *_8_9(Token *Input, Object_Env &Env) {
	std::string s(Input->Value.str, Input->Value.len);
	if (Input->Type == 8) // int
		return new Object_Int(std::stoll(s));
	return new Object_Decimal(std::stold(s));
}

static Object *_5(Token *Input, Object_Env &Env) {
	return new Object_String(Input->Value);
}

Object *Execute(Token *Input, Object_Env &Env) {
	switch (Input->Type) {
	case 0:
		return _0(Input, Env);
	case 7:
		return _7(Input, Env);
	case 10:
		return _10(Input, Env);
	case 3:
		return _3(Input, Env);
	case 2:
		return _2(Input, Env);
	case 6:
		return _6(Input, Env);
	case 4:
		return _4(Input, Env);
	case 8:
	case 9:
		return _8_9(Input, Env);
	case 5:
		return _5(Input, Env);
	default:
		throw Error(Input, (char *)"Unknown token");
	}
}
