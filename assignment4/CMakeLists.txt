add_executable(assignment4
main.cpp
../include/global.hpp
../include/assignment.h
)
target_link_libraries(assignment4 ${OpenCV_LIBRARIES})
