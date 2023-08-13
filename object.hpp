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
	unsigned long Type;
	std::map<Str, Object *, Str::STD_Str_LESS> Child;
	bool Deletable = true; // false if managed by other objects
	virtual Object *clone() {
		throw (char *)"Undefined implementation";
	};
	virtual Object *OnCast(unsigned long Type, Object_Env &Env) {
		throw (char *)"Undefined implementation";
	}
	virtual Object *OnCall(Object_Array &Args, Object_Env &Env) {
		return clone();
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
				if (!i.second->Deletable)
					delete i.second;
	}
	void AddChild(Str Key, Object *Value) {
		if (Value) {
			if (Value->Deletable)
				Value->Deletable = false;
			Child[Key] = Value;
		}
	}
};

class Object_Decimal : public Object { // Type = 1
  public:
	long double Value;
	Object_Decimal(long double Value) : Value(Value) { Type = 1; }
	Object *clone() override {
		return new Object_Decimal(Value);
	}
	Object *OnCast(unsigned long Type, Object_Env &Env) override {
		if (Type == 1)
			return this;
		throw (char *)"Type mismatch";
	}
	void OnAssign(Object &Args, Object_Env &Env) override {
		Object_Decimal *T = static_cast<Object_Decimal *>(&Args);
		Value = T->Value;
	}
};

class Object_Int : public Object { // Type = 2
  public:
	long long Value;
	Object_Int(long long Value) : Value(Value) { Type = 2; }
	Object *clone() override {
		return new Object_Int(Value);
	}
	Object *OnCast(unsigned long Type, Object_Env &Env) override {
		if (Type == 2)
			return this;
		throw (char *)"Type mismatch";
	}
	void OnAssign(Object &Args, Object_Env &Env) override {
		Object_Int *T = static_cast<Object_Int *>(&Args);
		Value = T->Value;
	}
};

class Object_Array : public Object { // Type = 3
  public:
	std::vector<Object *> Value;
	Object_Array() { Type = 3; }
	Object *clone() override {
		Object_Array *Result = new Object_Array();
		for (auto &i : Value) {
			Object *Temp = i->clone();
			Temp->Deletable = false;
			Result->Value.push_back(Temp);
		}
		return Result;
	}
	Object *OnCast(unsigned long Type, Object_Env &Env) override {
		if (Type == 3)
			return this;
		throw (char *)"Type mismatch";
	}
	void OnAssign(Object &Args, Object_Env &Env) override {
		Object_Array *T = static_cast<Object_Array *>(&Args);
		for (auto &i : T->Value) {
			Object *Temp = i;
			if (Temp) {
				Temp = Temp->clone();
				Temp->Deletable = false;
			}
			Value.push_back(Temp);
		}
	}
	Object *OnIndex(Object_Array &Args, Object_Env &Env) override {
		Object_Int *Ind = 0;
		try {
			Ind = static_cast<Object_Int *>(Args.Value[0]->OnCast(2, Env));
		} catch (char err) {
			throw (char *)"Type mismatch";
		}
		if (Ind->Value >= Value.size() || Ind->Value < 0)
			throw (char *)"Index out of range";
		return Value[Ind->Value];
	}
	~Object_Array() {
		for (auto &i : Value)
			delete i;
	}
	void AddValue(Object *Value) {
		if (Value)
			Value->Deletable = false;
		this->Value.push_back(Value);
	}
};

class Object_String : public Object { // Type = 4
  public:
	Str Value;
	Object_String(Str Value) : Value(Value.clone()) { Type = 4; }
	~Object_String() {
		delete[] Value.str;
	}
	Object *clone() override {
		return new Object_String(Value);
	}
	Object *OnCast(unsigned long Type, Object_Env &Env) override {
		if (Type == 4)
			return this;
		throw (char *)"Type mismatch";
	}
	void OnAssign(Object &Args, Object_Env &Env) override {
		Object_String *T = static_cast<Object_String *>(&Args);
		delete[] Value.str;
		Value = T->Value.clone();
	}
	Object *OnIndex(Object_Array &Args, Object_Env &Env) override {
		Object_Int *Ind = 0;
		Ind = static_cast<Object_Int *>(Args.Value[0]->OnCast(2, Env));
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
	Object *This;
	Object_Array *Args;
	Object_Env() {
		Type = 5;
		Deletable = false;
		Args = 0;
		This = 0;
		Prev = this;
		Global = 0;
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
