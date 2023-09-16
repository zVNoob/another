#pragma once

#include "error.hpp"
#include "object.hpp"
#include "str.hpp"

#include <iostream>
#include <typeinfo>
using namespace std;

namespace corelib {

static Object *If(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() < 3)
		throw "Missing argument";
	if (typeid(Args.Value[1]) != typeid(Args.Value[2]))
		throw "\"If\" case and \"Else\" case must have the same type";
	Object *Cond = Env.Register_AutoFree(Args.Value[0]->OnCall(Args, Env));
	if (Cond)
		if (dynamic_cast<Object_Int *>(Cond))
			if (static_cast<Object_Int *>(Cond)->Value)
				return Args.Value[1]->clone();
	return Args.Value[2]->clone();
}

static Object *While(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() < 1)
		throw "Missing argument";
	Object *Result = 0;
	while (1) {
		if (Result)
			if (Result->Deletable)
				delete Result;
		Result = Args.Value[0]->OnCall(Args, Env);
		if (Result) {
			if (dynamic_cast<Object_Int *>(Result))
				if (static_cast<Object_Int *>(Result)->Value == 0)
					break;
		} else
			break;
	}
	if (Result)
		if (Result->Deletable)
			delete Result;
	return 0;
}

static Object *Throw(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() > 0)
		throw Args.Value[0]->clone();
	throw "Missing argument";
}

static Object *Try(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() < 2)
		throw "Missing argument";
	Object *Dump = 0;
	try {
		Dump = Args.Value[0]->OnCall(Args, Env);
	} catch (Object *E) {
		Object_Array *Result = new Object_Array();
		Result->AddValue(E);
		try {
			Dump = Args.Value[1]->OnCall(*Result, Env);
			delete Result;

		} catch (...) {
			delete Result;
			throw;
		}
	}
	if (Dump)
		if (Dump->Deletable)
			delete Dump;

	return 0;
}

static Object *Catch(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() < 1)
		throw "Missing argument";
	try {
		Object *temp = Args.Value[0]->OnCall(Args, Env);
		if (temp)
			if (temp->Deletable)
				delete temp;
	} catch (Error &e) {
		return new Object_String(Str(e.What));
	}
	return new Object_String(Str((char *)0, 0));
}

inline void Register_Core(Object &Shared) {
	Shared.AddChild("if", new Object_Internal_Function(If));
	Shared.AddChild("while", new Object_Internal_Function(While));
	Shared.AddChild("try", new Object_Internal_Function(Try));
	Shared.AddChild("catch", new Object_Internal_Function(Catch));
	Shared.AddChild("throw", new Object_Internal_Function(Throw));
}

} // namespace corelib
