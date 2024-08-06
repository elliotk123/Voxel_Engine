#include <graphics_io_base.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h> 
#include <basetsd.h>
#include <vertex_array.h>

class Graphics_Io : public Graphics_Io_Base {
public:
	Graphics_Io();
	~Graphics_Io();
	void Update_Screen();
	void Send_Voxels(__int8* Voxels);
	bool Get_Window_Should_Close();
private:
	GLFWwindow* mp_window;
	unsigned int m_shaderProgram, m_VAO, m_VBO;
	VertexArray* mp_vertexArray;
	int m_Voxel_Count;

};