#pragma once

// Base type
#include "object.hpp"

// STL
#include <iostream>

namespace corelib {
// Basic programming blocks

static Object *If(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 1)
		throw "Missing return value";
	Object *Cond = Args.Value[0];
	Object *Then = Args.Value[1];
	Object *Else = 0;
	if (Args.Value.size() > 2)
		Else = Args.Value[2];
	Object_Int *RealCond = dynamic_cast<Object_Int *>(Cond);
	if (RealCond == 0)
		throw "Type mismatch";
	if (RealCond->Value) {
		return Then->clone();
	}
	return Else->clone();
}

static Object *While(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() < 2)
		throw "Missing argument";
	Object_Array *Output = new Object_Array();
	while (1) {
		Object_Array Temp;
		Object *Cond = Args.Value[0]->OnCall(Temp, Env);
		if (Cond == 0)
			break;
		Cond = dynamic_cast<Object_Int *>(Cond);
		if (Cond != 0)
			if (static_cast<Object_Int *>(Cond)->Value == 0)
				break;
		try {
			Output->AddValue(Args.Value[1]->OnCall(Temp, Env));
		} catch (...) {
			delete Output;
			throw;
		}
	}
	return Output;
}

static Object *Throw(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() < 1)
		throw "Missing argument";
	throw Args.Value[0]->clone();
}

static Object *Try(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() < 1)
		throw "Missing argument";
	Object_Array *Temp = new Object_Array();
	try {
		Object *Result = Args.Value[0]->OnCall(*Temp, Env);
		delete Temp;
		return Result;
	} catch (Object *) {
		if (Args.Value.size() > 1)
			try {
				return Args.Value[1]->OnCall(*Temp, Env);
			} catch (...) {
				delete Temp;
				throw;
			}
		delete Temp;
		return 0;
	} catch (...) {
		delete Temp;
		throw;
	}
}

#include "object_function.hpp"
static void Register_Core(Object &Shared) {
	Shared.AddChild(Str("if"), new Object_Internal_Function(If));
	Shared.AddChild(Str("while"), new Object_Internal_Function(While));
	Shared.AddChild(Str("throw"), new Object_Internal_Function(Throw));
	Shared.AddChild(Str("try"), new Object_Internal_Function(Try));
}

} // namespace corelib
