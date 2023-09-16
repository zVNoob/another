#pragma once

#include "object.hpp"

static Object *Execute(Token *Input, Object_Env &Env);

class Object_Function : public Object {
  public:
	Token *Entry;
	Object_Function(Token *Entry) {
		this->Entry = Entry;
	}
	Object *clone() override {
		return new Object_Function(Entry);
	}
	Object *OnCall(Object_Array &Args, Object_Env &Env) override {
		Object_Env *NewEnv = new Object_Env(&Env);
		NewEnv->Args = &Args;
		NewEnv->Self = this;
		Token *Iterator = Entry->ChildBegin;
		Object *Result = 0;

		while (Iterator) {
			if (Result)
				if (Result->Deletable)
					delete Result;
			Result = 0;
			NewEnv->AutoFree();
			NewEnv->This = Env.This;
			try {
				Result = Execute(Iterator, *NewEnv);
			} catch (...) {
				delete NewEnv;
				throw;
			}
			Iterator = Iterator->Next;
		}
		if (Result) {
			if (Result->Deletable) {
				if (Result->_Prev)
					Result->_Prev->_Next = Result->_Next;
				if (Result->_Next)
					Result->_Next->_Prev = Result->_Prev;
				Result->_Prev = 0;
				Result->_Next = 0;
			} else {
				Result = Result->clone();
			}
		}
		delete NewEnv;
		return Result;
	}
};
