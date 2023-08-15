#pragma once

// core type
#include "object.hpp"

namespace corelib {
// Environment access
static Object *Args(Object_Array &Args, Object_Env &Env) {
	return Env.Args;
}
static Object *This(Object_Array &Args, Object_Env &Env) {
	return Env.This;
}
static Object *Self(Object_Array &Args, Object_Env &Env) {
	return Env.Self;
}
static Object *Prev(Object_Array &Args, Object_Env &Env) {
	return Env.Prev;
}
static Object *Global(Object_Array &Args, Object_Env &Env) {
	return Env.Global;
}

#include "object_function.hpp"
static void Register_Env(Object &Shared) {
	Shared.AddChild(Str("args"), new Object_Internal_Function(Args));
	Shared.AddChild(Str("this"), new Object_Internal_Function(This));
	Shared.AddChild(Str("self"), new Object_Internal_Function(Self));
	Shared.AddChild(Str("prev"), new Object_Internal_Function(Prev));
	Shared.AddChild(Str("global"), new Object_Internal_Function(Global));
}
} // namespace corelib
