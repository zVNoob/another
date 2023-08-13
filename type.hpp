#pragma once

#include <forward_list>
#include <list>
#include <vector>

class TypeManager {
	std::vector<std::forward_list<unsigned long>> TypeTree;

  public:
	unsigned long GetNewID(std::forward_list<unsigned long> DerivedFrom) {
		TypeTree.push_back(DerivedFrom);
		return TypeTree.size();
	};
	bool Is(unsigned long Child, unsigned long Parent) {
		std::list<unsigned long> Available = {Child};
		while (Available.size()) {
			Child = *Available.begin();
			Available.pop_front();
			if (Child == Parent)
				return true;
			if (Child > Parent)
				for (auto &i : TypeTree[Child])
					Available.push_back(i);
		}
		return false;
	}
};
