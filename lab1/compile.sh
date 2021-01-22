
echo '#include <string>' > /tmp/rg_vertex_shader.cpp
echo 'std::string VERTEX_SHADER = R"=(' >> /tmp/rg_vertex_shader.cpp
cat src/shaders/vertexshader.glsl >> /tmp/rg_vertex_shader.cpp
echo ')=";' >> /tmp/rg_vertex_shader.cpp

echo '#include <string>' > /tmp/rg_fragment_shader.cpp
echo 'std::string FRAGMENT_SHADER = R"=(' >> /tmp/rg_fragment_shader.cpp
cat src/shaders/fragmentshader.glsl >> /tmp/rg_fragment_shader.cpp
echo ')=";' >> /tmp/rg_fragment_shader.cpp

echo '#include <string>' > /tmp/rg_linestrip_vertex_shader.cpp
echo 'std::string LINESTRIP_VERTEX_SHADER = R"=(' >> /tmp/rg_linestrip_vertex_shader.cpp
cat src/shaders/linestrip_vertexshader.glsl >> /tmp/rg_linestrip_vertex_shader.cpp
echo ')=";' >> /tmp/rg_linestrip_vertex_shader.cpp

echo '#include <string>' > /tmp/rg_linestrip_fragment_shader.cpp
echo 'std::string LINESTRIP_FRAGMENT_SHADER = R"=(' >> /tmp/rg_linestrip_fragment_shader.cpp
cat src/shaders/linestrip_fragmentshader.glsl >> /tmp/rg_linestrip_fragment_shader.cpp
echo ')=";' >> /tmp/rg_linestrip_fragment_shader.cpp

rm main
clang++ -std=c++11 -lglfw -lglew -lassimp -framework OpenGL \
    /tmp/rg_vertex_shader.cpp /tmp/rg_fragment_shader.cpp \
    /tmp/rg_linestrip_vertex_shader.cpp \
    /tmp/rg_linestrip_fragment_shader.cpp \
    src/bspline.cpp src/drawutil.cpp src/shader.cpp src/mesh.cpp src/main.cpp \
    -o main

rm /tmp/rg_vertex_shader.cpp /tmp/rg_fragment_shader.cpp
