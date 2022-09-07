add_executable(assignment1 
main.cpp 
rasterizer.cpp 
Triangle.hpp
Triangle.cpp
rasterizer.hpp
../include/assignment.h
../include/global.hpp
)

target_link_libraries(assignment1 ${OpenCV_LIBRARIES})