#pragma once

#include "error.hpp"
#include "object.hpp"
#include <thread>

namespace corelib {

class Object_Thread : public Object {
	void Internal_Thread_Function() {
		try {
			Result = Callee->OnCall(*Args, Env);
		} catch (Object *E) {
			Thrown = E;
		} catch (Error &E) {
			SemanticError = &E;
		} catch (const char *Msg) {
			SemanticErrorMsg = Msg;
		}
		delete Args;
		delete Callee;
	}
	Object *Callee;
	Object_Array *Args;
	Object_Env &Env;

	bool Running = true;
	std::thread Thread;

	Error *SemanticError = 0;
	const char *SemanticErrorMsg = 0;
	Object *Result = 0;
	Object *Thrown = 0;

  public:
	Object_Thread(Object *Callee, Object_Array *Args, Object_Env &Env) : Args(Args), Callee(Callee), Env(Env), Thread(&Object_Thread::Internal_Thread_Function, this) {
		Deletable = false; // make sure it is not deleted before done
	};
	Object *OnCall(Object_Array &Args, Object_Env &Env) override {
		if (Running)
			Thread.join();
		Running = false;
		if (SemanticErrorMsg)
			throw SemanticErrorMsg;
		if (SemanticError)
			throw *SemanticError;
		if (Thrown)
			throw Thrown;
		return Result;
	}
	Object *clone() override {
		if (Running)
			Thread.join();
		Running = false;
		if (SemanticError)
			delete SemanticError;
		if (Thrown)
			delete Thrown;
		return Result;
	}
	~Object_Thread() {
		delete clone();
	}
};

static Object *Thread(Object_Array &Args, Object_Env &Env) {
	if (Args.Value.size() == 0)
		throw "Missing argument";
	// Seperate callee and args
	Object *Callee = Args.Value[0]->clone();
	Object_Array *Args2 = new Object_Array();
	for (int i = 1; i < Args.Value.size(); i++)
		Args2->AddValue(Args.Value[i]);
	// Create thread
	Object_Thread *Thread = new Object_Thread(Callee, Args2, Env);
	return Thread;
}

inline void Register_Thread(Object &Shared) {
	Shared.AddChild("thread", new Object_Internal_Function(Thread));
}

} // namespace corelib
