#include <iostream>
#include <list>

#include "str.hpp"
#include "token.hpp"
#include "tokenizer.hpp"

using namespace std;

ostream& operator<<(ostream& os,Str& inp) {
	for (int i=0;i<inp.len;i++) {
		switch (inp.str[i]) {
			case '\n':os << "\\n";
			case '\\':os << "\\\\";
			default:os << inp.str[i];
		}
	}
	return os;
}

ostream& operator<<(ostream& os,Token T) //AST pretty printer
{
	const string norm = "255;255;255";
	const string table[10] = 
	{
		"255;127;50",
		"255;255;50",
		"127;255;50",
		"50;255;50",
		"50;255;127",
		"50;255;255",
		"200;127;255",
		"100;100;255",
		"127;50;255",
		"255;50;255",	
	};
	const string tree = "150;150;150";
	static list<bool> depth;
	os << "\x1b[38;2;" << norm << 'm' << (int)T.Type << ":\"";
	os << "\x1b[38;2;" << table[T.Type] << "m" << T.Value;
	os << "\x1b[38;2;" << norm << "m\"";
	if (T.ChildBegin) depth.push_back(1);
	if (T.PreExec)
	{
		int i = T.Value.len+6+(T.ChildBegin==0);
		for (int k=0;k<i;k++) depth.push_back(false);
		os << "\x1b[38;2;" << tree << "m - " << *T.PreExec;
		while (i--) depth.erase(--depth.end());
	}
	else os << '\n';
	os << "\x1b[38;2;" << tree << 'm';
	if (T.ChildBegin)
	{
		Token* P = T.ChildBegin;
		while (P)
		{
			for (auto i:depth) os << ((i)?"│":" ");
			os << "\x1b[D";
			if (P==T.ChildEnd)
			{
				*(--depth.end()) = 0;
				os << "└";
			}
			else os << "├";
			os << *P;
			P = P->Next;
		}
		depth.pop_back();
	}
	return os;
}


int main()
{
	string s;
	getline(std::cin,s);
	auto Res = Tokenizer()(Str(const_cast<char*>(s.c_str()),s.size()));
	cout << *Res;
	return 0;
}