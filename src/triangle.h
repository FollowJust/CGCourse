#pragma once

#include "hittable.h"
#include "interval.h"
#include "material.h"
#include "ray.h"
#include "vec3.h"
#include <memory>

class Triangle : public hittable
{
public:
	Triangle(const point3 & v1, const point3 & v2, const point3 & v3, const std::shared_ptr<material> material)
		: v1_(v1)
		, v2_(v2)
		, v3_(v3)
		, material_(material)
	{
		const vec3 & edge1 = v1_ - v3_;
		const vec3 & edge2 = v2_ - v3_;

		const vec3 & n = cross(edge1, edge2);
		normal_ = n / n.length();
		std::cout << "normal: " << normal_.x() << " " << normal_.y() << " " << normal_.z() << std::endl;
	}

	Triangle(const point3 & v1, const point3 & v2, const point3 & v3, const vec3 & normal, const std::shared_ptr<material> material)
		: v1_(v1)
		, v2_(v2)
		, v3_(v3)
		, normal_(normal)
		, material_(material)
	{}

	bool hit(const ray & r, interval ray_t, hit_record & rec) const override
	{
		constexpr float epsilon = std::numeric_limits<float>::epsilon();

		vec3 edge1 = v2_ - v1_;
		vec3 edge2 = v3_ - v1_;
		vec3 ray_cross_e2 = cross(r.direction(), edge2);
		float det = dot(edge1, ray_cross_e2);

		if (det > -epsilon && det < epsilon)
		{
			return false;// This ray is parallel to this triangle.
		}

		float inv_det = 1.0 / det;
		vec3 s = r.origin() - v1_;
		float u = inv_det * dot(s, ray_cross_e2);

		if ((u < 0 && abs(u) > epsilon) || (u > 1 && abs(u - 1) > epsilon))
		{
			return false;
		}

		vec3 s_cross_e1 = cross(s, edge1);
		float v = inv_det * dot(r.direction(), s_cross_e1);

		if ((v < 0 && abs(v) > epsilon) || (u + v > 1 && abs(u + v - 1) > epsilon))
		{
			return false;
		}

		// At this stage we can compute t to find out where the intersection point is on the line.
		float t = inv_det * dot(edge2, s_cross_e1);

		if (ray_t.surrounds(t))// ray intersection
		{
			rec.t = t;
			rec.p = r.at(rec.t);
			rec.set_face_normal(r, normal_);
			rec.mat = material_;

			return true;
		}

		return false;
	}

private:
	point3 v1_, v2_, v3_;
	vec3 normal_;
	std::shared_ptr<material> material_;
};