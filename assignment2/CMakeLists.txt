add_executable(assignment2
main.cpp
rasterizer.hpp
rasterizer.cpp
Triangle.hpp
Triangle.cpp
../include/global.hpp
../include/assignment.h
)
target_link_libraries(assignment2 ${OpenCV_LIBRARIES})
