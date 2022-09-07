add_executable(assignment3
main.cpp
rasterizer.hpp
rasterizer.cpp
../include/global.hpp
../include/assignment.h
Triangle.hpp
Triangle.cpp
Texture.hpp
Texture.cpp
Shader.hpp
OBJ_Loader.h
)
target_link_libraries(assignment3 ${OpenCV_LIBRARIES})