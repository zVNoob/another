#pragma once

#include "object.hpp"

namespace corelib {

static Object *Prev(Object_Array &Args, Object_Env &Env) {
	return Env.Prev;
}

static Object *Global(Object_Array &Args, Object_Env &Env) {
	return Env.Global;
}

static Object *Args(Object_Array &Args, Object_Env &Env) {
	return Env.Args;
}

static Object *Self(Object_Array &Args, Object_Env &Env) {
	return Env.Self;
}

static Object *This(Object_Array &Args, Object_Env &Env) {
	return Env.This;
}

inline void Register_Env(Object &Shared) {
	Shared.AddChild("prev", new Object_Internal_Function(Prev));
	Shared.AddChild("global", new Object_Internal_Function(Global));
	Shared.AddChild("args", new Object_Internal_Function(Args));
	Shared.AddChild("self", new Object_Internal_Function(Self));
	Shared.AddChild("this", new Object_Internal_Function(This));
}
} // namespace corelib
