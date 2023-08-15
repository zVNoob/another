#pragma once

#include "error.hpp"
#include "object.hpp"

static Object *Execute(Token *Input, Object_Env &Env);

class Object_Function : public Object { // Type = 6
  public:
	Token *EntryPoint;
	Object_Function(Token *EntryPoint) : EntryPoint(EntryPoint) {
	}
	Object *clone() override {
		return new Object_Function(EntryPoint);
	}
	void OnAssign(Object &Args, Object_Env &Env) override {
		Object_Function *T = static_cast<Object_Function *>(&Args);
		EntryPoint = T->EntryPoint;
	}
	Object *OnCall(Object_Array &Args, Object_Env &Env) override {
		Token *Iterator = EntryPoint->ChildBegin;
		Object_Env *NewEnv = new Object_Env(&Env);
		NewEnv->Deletable = false;
		NewEnv->Args = &Args;
		NewEnv->Self = this;
		Args.Deletable = false;
		Object *Result = 0;
		Object *Save_This_Env = NewEnv->This;
		while (Iterator) {
			// Clean up old result
			if (Result) {
				if (Result->Deletable)
					delete Result;
				Result = 0;
			}
			try {
				Result = Execute(Iterator, *NewEnv);
				NewEnv->This = Save_This_Env;
			} catch (...) {
				delete NewEnv;
				throw;
			}
			Iterator = Iterator->Next;
		}
		if (Result)
			if (Result->Deletable == 0)
				try {
					Result = Result->clone();
				} catch (const char *e) {
					delete NewEnv;
					throw Error(EntryPoint, e);
				} catch (...) {
					delete NewEnv;
					throw;
				}
		delete NewEnv;
		return Result;
	}
};

class Object_Internal_Function : public Object { // Type = 7
  public:
	Object *(*Function)(Object_Array &Args, Object_Env &Env);
	Object_Internal_Function(Object *(*Function)(Object_Array &Args, Object_Env &Env)) : Function(Function) {
	}
	Object *clone() override {
		return new Object_Internal_Function(Function);
	}
	void OnAssign(Object &Args, Object_Env &Env) override {
		Object_Internal_Function *T = static_cast<Object_Internal_Function *>(&Args);
		Function = T->Function;
	}
	Object *OnCall(Object_Array &Args, Object_Env &Env) override {
		return Function(Args, Env);
	}
};
