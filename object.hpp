#pragma once

#include "str.hpp"
#include "token.hpp"
#include <map>
#include <string>
#include <vector>

class Object_Env;
class Object_Array;

class Object { // Type = 0, unusable
  public:
	std::map<Str, Object *, Str::STD_Str_LESS> Child;
	// GC
	bool Deletable = true;
	Object *_Next = 0;
	Object *_Prev = 0;
	virtual Object *clone() {
		throw (char *)"Undefined implementation";
	};
	virtual Object *OnCall(Object_Array &Args, Object_Env &Env) {
		throw (char *)"Undefined implementation";
	}
	virtual Object *OnIndex(Object_Array &Args, Object_Env &Env) {
		throw (char *)"Undefined implementation";
	}
	virtual void OnAssign(Object &Args, Object_Env &Env) { // Only do if args is casted already
		throw (char *)"Undefined implementation";
	}
	virtual ~Object() {
		for (auto &i : Child)
			if (i.second)
				delete i.second;
	}

	inline void AddChild(Str Key, Object *Value) {
		if (Value) {
			auto Temp = Child.find(Key);
			if (Temp != Child.end())
				delete Temp->second;
			if (Value->_Prev)
				Value->_Prev->_Next = Value->_Next;
			if (Value->_Next)
				Value->_Next->_Prev = Value->_Prev;
			Value->_Prev = 0;
			Value->_Next = 0;
			Value->Deletable = false;
			Child[Key] = Value;
		}
	}
};

class Object_Int : public Object { // Type = 2
  public:
	long long Value;
	Object_Int(long long Value) : Value(Value) {}
	Object *clone() override {
		return new Object_Int(Value);
	}
	void OnAssign(Object &Args, Object_Env &Env) override {
		if (dynamic_cast<Object_Int *>(&Args) == 0)
			throw (char *)"Type mismatch";
		Value = static_cast<Object_Int *>(&Args)->Value;
	}
};

class Object_Decimal : public Object { // Type = 1
  public:
	long double Value;
	Object_Decimal(long double Value) : Value(Value) {}
	Object *clone() override {
		return new Object_Decimal(Value);
	}
	void OnAssign(Object &Args, Object_Env &Env) override {
		if (dynamic_cast<Object_Decimal *>(&Args) == 0)
			throw (char *)"Type mismatch";
		Value = static_cast<Object_Decimal *>(&Args)->Value;
	}
};

class Object_Internal_Function : public Object {
  public:
	Object *(*Function)(Object_Array &Args, Object_Env &Env);
	Object_Internal_Function(Object *(*Function)(Object_Array &Args, Object_Env &Env)) {
		this->Function = Function;
	}
	Object *OnCall(Object_Array &Args, Object_Env &Env) override {
		return Function(Args, Env);
	}
	Object *clone() override {
		return new Object_Internal_Function(Function);
	}
};

class Object_Array : public Object { // Type = 3
  public:
	std::vector<Object *> Value;
	Object_Array() {
	}
	Object *clone() override {
		Object_Array *Result = new Object_Array();
		for (auto &i : Value) {
			Object *Temp = i->clone();
			Result->Value.push_back(Temp);
		}
		return Result;
	}
	void OnAssign(Object &Args, Object_Env &Env) override {
		if (dynamic_cast<Object_Array *>(&Args) == 0)
			throw (char *)"Type mismatch";
		Object_Array *T = static_cast<Object_Array *>(&Args);
		for (auto &i : T->Value) {
			Value.push_back(i->clone());
		}
	}
	Object *OnIndex(Object_Array &Args, Object_Env &Env) override {
		Object_Int *Ind = 0;
		if (Args.Value.size() == 0)
			throw (char *)"Missing argument";
		Ind = dynamic_cast<Object_Int *>(Args.Value[0]);
		if (Ind == 0)
			throw (char *)"Type mismatch";
		if (Ind->Value >= Value.size() || Ind->Value < 0)
			throw (char *)"Index out of range";
		return Value[Ind->Value];
	}
	~Object_Array() {
		for (auto &i : Value)
			delete i;
	}
	void AddValue(Object *Value) {
		if (Value) {
			if (Value->_Prev)
				Value->_Prev->_Next = Value->_Next;
			if (Value->_Next)
				Value->_Next->_Prev = Value->_Prev;
			if (Value->Deletable == 0)
				Value = Value->clone();
			Value->_Prev = 0;
			Value->_Next = 0;
			Value->Deletable = false;
			this->Value.push_back(Value);
		}
	}
};

class Object_String : public Object { // Type = 4
  public:
	Str Value;
	Object_String(Str Value) : Value(Value.clone()) {}
	~Object_String() {
		delete[] Value.str;
	}
	Object *clone() override {
		return new Object_String(Value);
	}
	void OnAssign(Object &Args, Object_Env &Env) override {
		if (dynamic_cast<Object_String *>(&Args) == 0)
			throw (char *)"Type mismatch";
		Object_String *T = static_cast<Object_String *>(&Args);
		delete[] Value.str;
		Value = T->Value.clone();
	}
	Object *OnIndex(Object_Array &Args, Object_Env &Env) override {
		Object_Int *Ind = 0;
		if (Args.Value.size() == 0)
			throw (char *)"Missing argument";
		Ind = dynamic_cast<Object_Int *>(Args.Value[0]);
		if (Ind == 0)
			throw (char *)"Type mismatch";
		if (Ind->Value >= Value.len || Ind->Value < 0)
			throw (char *)"Index out of range";
		return new Object_Int(Value.str[Ind->Value]);
	}
};

class Object_Env : public Object { // Type = 5
  public:
	// Internal function helper
	Object_Env *Prev;
	Object_Env *Global;
	Object *Shared;
	// Context
	Object *This;		// OOP object
	Object_Array *Args; // Function arguments
	Object *Self;		// Self callee (for recursive lambda)
	// GC-related
	Object *Floating_Object = 0;
	inline Object *Register_AutoFree(Object *Object) {
		if (Object)
			if (Object->Deletable) {
				Object->_Next = Floating_Object;
				if (Floating_Object)
					Floating_Object->_Prev = Object;
				Floating_Object = Object;
				return Object;
			}
		return Object;
	}
	inline void AutoFree() {
		while (Floating_Object) {
			Object *Temp = Floating_Object;
			Floating_Object = Floating_Object->_Next;
			delete Temp;
		}
	}
	Object_Env() {
		Args = 0;
		This = 0;
		Prev = this;
		Global = 0;
		Shared = 0;
		Self = 0;
		Floating_Object = 0;
		Deletable = false;
	}
	~Object_Env() {
		AutoFree();
	}
	Object_Env(Object_Env *Prev) : Object_Env() {
		this->Prev = Prev;
		if (Prev->Global)
			Global = Prev->Global;
		else
			Global = this;
		Shared = Prev->Shared;
		This = Prev->This;
	}
};
