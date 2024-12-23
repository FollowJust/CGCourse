#ifndef COLOR_H
#define COLOR_H
//==============================================================================================
// Originally written in 2020 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "interval.h"
#include "vec3.h"

using color = vec3;


inline double linear_to_gamma(double linear_component)
{
	if (linear_component > 0)
		return std::sqrt(linear_component);

	return 0;
}


void color_to_unsigned_char(const color & pixel_color, unsigned char & r, unsigned char & g, unsigned char & b, unsigned char & a)
{
	// Translate the [0,1] component values to the byte range [0,255].
	static const interval intensity(0.000, 0.999);
	r = static_cast<unsigned char>(256 * intensity.clamp(linear_to_gamma(pixel_color.x())));
	g = static_cast<unsigned char>(256 * intensity.clamp(linear_to_gamma(pixel_color.y())));
	b = static_cast<unsigned char>(256 * intensity.clamp(linear_to_gamma(pixel_color.z())));
	a = 255;
}


#endif
