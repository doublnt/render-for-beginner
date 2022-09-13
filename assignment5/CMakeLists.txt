add_executable(assignment5
main.cpp
../include/global.hpp
../include/assignment.h
Light.hpp
Object.hpp
Renderer.cpp
Renderer.hpp
Scene.hpp
Scene.cpp
Sphere.hpp
Triangle.hpp
Vector.hpp
)
target_link_libraries(assignment5 ${OpenCV_LIBRARIES})

