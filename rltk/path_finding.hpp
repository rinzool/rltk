#pragma once

/* RLTK (RogueLike Tool Kit) 1.00
 * Copyright (c) 2016-Present, Bracket Productions.
 * Licensed under the LGPL - see LICENSE file.
 *
 * Path finding - interface to the A-Star system
 */

#include "astar.hpp"
#include <memory>
#include <deque>

namespace rltk {

// Template class used to forward to specialize the algorithm to the user's map format and
// and behaviors defined in navigator_t. This avoids the library mandating what your map
// looks like.
template<class location_t, class navigator_t>
class map_search_node {
public:
	location_t pos;

	map_search_node() {}
	map_search_node(location_t loc) : pos(loc) {}

	float GoalDistanceEstimate(map_search_node<location_t, navigator_t> &goal) {
		float result = navigator_t::get_distance_estimate(pos, goal.pos);
		//std::cout << "GoalDistanceEstimate called (" << result << ").\n";
		return result;
	}

	bool IsGoal(map_search_node<location_t, navigator_t> &node_goal) {
		bool result = navigator_t::is_goal(pos, node_goal.pos);
		//std::cout << "IsGoal called (" << result << ").\n";
		return result;
	}

	bool GetSuccessors(AStarSearch<map_search_node<location_t, navigator_t>> * a_star_search, map_search_node<location_t, navigator_t> * parent_node) {
		//std::cout << "GetSuccessors called.\n";
		std::vector<location_t> successors;

		if (parent_node != nullptr) {		
			navigator_t::get_successors(parent_node->pos, successors);			
		} else {
			location_t start = navigator_t::get_start();
			navigator_t::get_successors(start, successors);
		}
		for (location_t loc : successors) {
			map_search_node<location_t, navigator_t> tmp(loc);
			//std::cout << " --> " << loc.x << "/" << loc.y << "\n";
			a_star_search->AddSuccessor( tmp );
		}
		return true;
	}


	float GetCost(map_search_node<location_t, navigator_t> &successor) {
		float result = navigator_t::get_cost(pos, successor.pos);
		//std::cout << "GetCost called (" << result << ").\n";
		return result;
	}

	bool IsSameState(map_search_node<location_t, navigator_t> &rhs) {
		bool result = navigator_t::is_same_state(pos, rhs.pos);
		//std::cout << "IsSameState called (" << result << ").\n";
		return result;
	}
};

// Template class used to define what a navigation path looks like
template<class location_t>
struct navigation_path {
	bool success = false;
	location_t destination;
	std::deque<location_t> steps;
};

template<class location_t, class navigator_t>
std::shared_ptr<navigation_path<location_t>> find_path(location_t start, location_t end) 
{
	AStarSearch<map_search_node<location_t, navigator_t>> a_star_search;
	map_search_node<location_t, navigator_t> a_start(start);
	map_search_node<location_t, navigator_t> a_end(end);

	a_star_search.SetStartAndGoalStates(a_start, a_end);
	unsigned int search_state;
	unsigned int search_steps = 0;

	do {
		search_state = a_star_search.SearchStep();
		++search_steps;
	} while (search_state == AStarSearch<map_search_node<navigator_t, location_t>>::SEARCH_STATE_SEARCHING);

	if (search_state == AStarSearch<map_search_node<navigator_t, location_t>>::SEARCH_STATE_SUCCEEDED) {
		std::shared_ptr<navigation_path<location_t>> result = std::shared_ptr<navigation_path<location_t>>(new navigation_path<location_t>());
		result->destination = end;
		map_search_node<location_t, navigator_t> * node = a_star_search.GetSolutionStart();
		for (;;) {
			node = a_star_search.GetSolutionNext();
			if (!node) break;
			result->steps.push_back(node->pos);
		}
		a_star_search.FreeSolutionNodes();
		a_star_search.EnsureMemoryFreed();
		result->success = true;
		return result;
	}

	std::shared_ptr<navigation_path<location_t>> result = std::make_shared<navigation_path<location_t>>();
	a_star_search.EnsureMemoryFreed();
	return result;
}

}
