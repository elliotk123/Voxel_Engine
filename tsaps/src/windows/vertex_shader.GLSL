#version 330 core
layout (location = 0) in vec3 aPos;
const mat4 Projection = mat4()
void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
};