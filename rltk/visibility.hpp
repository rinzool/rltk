#pragma once
/* RLTK (RogueLike Tool Kit) 1.00
 * Copyright (c) 2016-Present, Bracket Productions.
 * Licensed under the LGPL - see LICENSE file.
 *
 * Provides a wrapper for bitmap fonts.
 */

#include <functional>
#include "geometry.hpp"

namespace rltk {

namespace visibility_private {

template<class location_t_>
void internal_2d_sweep(const location_t_ &position, const location_t_ &destination, const int &range, std::function<void(location_t_)> set_visible, 
	std::function<bool(location_t_)> is_opaque)
{
	bool blocked = false;
	line_func(position.x, position.y, destination.x, destination.y, [&blocked, &is_opaque, &set_visible, &range, &position] (int X, int Y) {
		float distance = distance2d(position.x, position.y, X, Y);
		if (distance <= range) {
			location_t_ pos;
			pos.x = X;
			pos.y = Y;
			if (!blocked) set_visible(pos);
			if (!is_opaque(pos)) blocked = true;
		}
	});	
}

}

/* Simple all-direction visibility sweep in 2 dimensions. This requires that your location_t utilize an x and y
 * component. Parameters:
 * position - where you are sweeping from.
 * range - the number of tiles you can traverse.
 * set_visible - a callback (such as bool set_visible(location_t & loc)) to say "this is visible"
 * is_opaque - a callback to ask your map if you can see through a tile.
 */
template<class location_t_>
void visibility_sweep_2d(const location_t_ &position, const int &range, std::function<void(location_t_)> set_visible, 
	std::function<bool(location_t_)> is_opaque)
{
	// You can always see yourself
	set_visible(position);

	// Box-sweep
	for (int i=0-range; i<range; ++i) {

		// Going up
		location_t_ destination;
		destination.x = position.x + i;
		destination.y = position.y - range;
		visibility_private::internal_2d_sweep(position, destination, range, set_visible, is_opaque);
			
		// Going down
		destination.x = position.x + i;
		destination.y = position.y + range;
		visibility_private::internal_2d_sweep(position, destination, range, set_visible, is_opaque);

		// Going left
		destination.x = position.x - range;
		destination.y = position.y + i;
		visibility_private::internal_2d_sweep(position, destination, range, set_visible, is_opaque);

		// Going right
		destination.x = position.x + range;
		destination.y = position.y + i;
		visibility_private::internal_2d_sweep(position, destination, range, set_visible, is_opaque);
	}
}

}
