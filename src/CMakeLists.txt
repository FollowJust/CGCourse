set(SRCS
    main.cc
    camera.h
    color.h
    hittable_list.h
    hittable.h
    interval.h
    material.h
    ray.h
    rtweekend.h
    sphere.h
    vec3.h
)

add_executable(raytracing ${SRCS})