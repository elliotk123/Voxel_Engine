#include <graphics_io.h>
#include <iostream>
#include <limits>
#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/gtc/type_ptr.hpp>

// Define window size
const int WIDTH = 1600;
const int HEIGHT = 800;

// voxels
const __int16 Voxels_X = 128;
const __int16 Voxels_Y = 64;
const __int16 Voxels_Z = 64;

//graphics 
const int int16_max = std::numeric_limits<int16_t>::max();
const int int16_mid = 0;
const int int16_min = std::numeric_limits<int16_t>::min();

// derivative constants 
const int Voxel_Max = Voxels_X * Voxels_Y * Voxels_Z;
const int int16_values = int16_max - int16_min + 1;
const __int16 Base_Step_X = 511; //2047; //std::floor(int16_values - 1 / Voxels_X));
const __int16 Base_Step_Y = 1023; //2047; //std::floor(int16_values - 1 / Voxels_X));

// Define vertex shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);\n"
"}\0";

// Define fragment shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 0.2f, 1.0f);\n"
"}\n\0";

Graphics_Io::~Graphics_Io() {
    // Deallocate resources and terminate glfw
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteProgram(m_shaderProgram);
    glfwTerminate();
}

Graphics_Io::Graphics_Io() {
    // Initialize glfw and create a window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mp_window = glfwCreateWindow(WIDTH, HEIGHT, "Triangle", NULL, NULL);
    if (mp_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(mp_window);

    // Load OpenGL function pointers using glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    // Compile vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for vertex shader compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for fragment shader compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link vertex and fragment shaders into a shader program
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    // Check for shader program linking errors
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete the shader objects after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Generate a vertex buffer object and a vertex array object
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    // Bind the vertex array object and the vertex buffer object
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // Copy the triangle vertices to the buffer
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 2, GL_SHORT, GL_TRUE, 2*sizeof(__int16), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind the vertex array object and the vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // num voxels * faces per voxel * 2 traingles per face * 3 vertices per triangle
    unsigned int maxNumVertices = (Voxels_X*Voxels_Y* 3 - Voxels_X * 2 - Voxels_Y * 2 - 4)*Voxels_Z * 2 * 3;
    mp_vertexArray = new VertexArray(maxNumVertices/32);
}

void Graphics_Io::Update_Screen() {
    // Process input
    if (glfwGetKey(mp_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(mp_window, true);

    // Clear the color buffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the shader program and draw the triangle
    glUseProgram(m_shaderProgram);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    // Copy the triangle vertices to the buffer
    __int16*  data = mp_vertexArray->getData();
    int size = (mp_vertexArray->getDataLength()) * sizeof(__int16);
    
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, mp_vertexArray->getDataLength()/2);

    // Swap buffers and poll events
    glfwSwapBuffers(mp_window);
    glfwPollEvents();


}

void Graphics_Io::Send_Voxels(__int8* Voxels_Out) {
    mp_vertexArray->resetBuffer();
    int Vertex_Counter = 0;
    __int16 X = 0;
    __int16 Y = 0;
    __int16 Z = 0;
    __int16 Step_X = Base_Step_X;
    __int16 Step_X_Behind = Base_Step_X;
    __int16 Step_Y = Base_Step_Y;
    __int16 Step_Y_Behind = Base_Step_Y;
    for (int i = 0; i < Voxel_Max; i++) {
        if (Voxels_Out[i] > 0) {
            X = i % Voxels_X; // between 0 and voxels_x - 1 
            Y = (i / Voxels_X) % Voxels_Y;
            Z = i / int(Voxels_X * Voxels_Y);
            Step_X = Base_Step_X * (Voxels_X) / (2 * Z + Voxels_X);
            Step_X_Behind = Base_Step_X * (Voxels_X) / (2 * (Z+1) + Voxels_X);
            Step_Y = Base_Step_Y * (Voxels_Y) / (2 * Z + Voxels_Y);
            Step_Y_Behind = Base_Step_Y * (Voxels_Y) / (2 * (Z+1) + Voxels_Y);

            // Front face -- always required 
            mp_vertexArray->addValues(int16_mid + Step_X * (X - Voxels_X / 2),     int16_mid + Step_Y * (Y + 1 - Voxels_Y / 2)); // top left 
            mp_vertexArray->addValues(int16_mid + Step_X * (X + 1 - Voxels_X / 2), int16_mid + Step_Y * (Y + 1 - Voxels_Y / 2)); // top right
            mp_vertexArray->addValues(int16_mid + Step_X * (X + 1 - Voxels_X / 2), int16_mid + Step_Y * (Y  - Voxels_Y / 2)); // bottom right
            mp_vertexArray->addValues(int16_mid + Step_X * (X + 1 - Voxels_X / 2), int16_mid + Step_Y * (Y - Voxels_Y / 2)); // bottom right
            mp_vertexArray->addValues(int16_mid + Step_X * (X - Voxels_X / 2),     int16_mid + Step_Y * (Y  - Voxels_Y / 2)); // bottom left
            mp_vertexArray->addValues(int16_mid + Step_X * (X - Voxels_X / 2), int16_mid + Step_Y * (Y + 1 - Voxels_Y / 2)); // top left 

            // left face -- for voxels in right half 
            if (X > Voxels_X / 2) {
                mp_vertexArray->addValues(int16_mid + Step_X_Behind * (X - Voxels_X / 2), int16_mid + Step_Y_Behind * (Y + 1 - Voxels_Y / 2)); // top left 
                mp_vertexArray->addValues(int16_mid + Step_X * (X - Voxels_X / 2), int16_mid + Step_Y * (Y + 1 - Voxels_Y / 2)); // top right
                mp_vertexArray->addValues(int16_mid + Step_X * (X - Voxels_X / 2), int16_mid + Step_Y * (Y - Voxels_Y / 2)); // bottom right
                mp_vertexArray->addValues(int16_mid + Step_X * (X - Voxels_X / 2), int16_mid + Step_Y * (Y - Voxels_Y / 2)); // bottom right
                mp_vertexArray->addValues(int16_mid + Step_X_Behind * (X - Voxels_X / 2), int16_mid + Step_Y_Behind * (Y - Voxels_Y / 2)); // bottom left
                mp_vertexArray->addValues(int16_mid + Step_X_Behind * (X - Voxels_X / 2), int16_mid + Step_Y_Behind * (Y + 1 - Voxels_Y / 2)); // top left 
            }

            // right face -- for voxels in left half 
            if (X < Voxels_X / 2 - 1) {
                mp_vertexArray->addValues(int16_mid + Step_X * (X + 1 - Voxels_X / 2), int16_mid + Step_Y * (Y + 1 - Voxels_Y / 2));// top left 
                mp_vertexArray->addValues(int16_mid + Step_X_Behind * (X + 1 - Voxels_X / 2), int16_mid + Step_Y_Behind * (Y + 1 - Voxels_Y / 2)); // top right
                mp_vertexArray->addValues(int16_mid + Step_X_Behind * (X + 1 - Voxels_X / 2), int16_mid + Step_Y_Behind * (Y - Voxels_Y / 2)); // bottom right
                mp_vertexArray->addValues(int16_mid + Step_X_Behind * (X + 1 - Voxels_X / 2), int16_mid + Step_Y_Behind * (Y - Voxels_Y / 2)); // bottom right
                mp_vertexArray->addValues(int16_mid + Step_X * (X + 1 - Voxels_X / 2), int16_mid + Step_Y * (Y - Voxels_Y / 2)); // bottom left
                mp_vertexArray->addValues(int16_mid + Step_X * (X + 1 - Voxels_X / 2), int16_mid + Step_Y * (Y + 1 - Voxels_Y / 2)); // top left 
            }

            // Top face -- for voxels in bottom half
            if (Y < Voxels_Y / 2 - 1) {
                mp_vertexArray->addValues(int16_mid + Step_X_Behind * (X - Voxels_X / 2), int16_mid + Step_Y_Behind * (Y + 1 - Voxels_Y / 2)); // top left
                mp_vertexArray->addValues(int16_mid + Step_X_Behind * (X + 1 - Voxels_X / 2), int16_mid + Step_Y_Behind * (Y + 1 - Voxels_Y / 2)); // top right
                mp_vertexArray->addValues(int16_mid + Step_X * (X + 1 - Voxels_X / 2), int16_mid + Step_Y * (Y + 1 - Voxels_Y / 2)); // bottom right
                mp_vertexArray->addValues(int16_mid + Step_X * (X + 1 - Voxels_X / 2), int16_mid + Step_Y * (Y + 1 - Voxels_Y / 2)); // bottom right
                mp_vertexArray->addValues(int16_mid + Step_X * (X - Voxels_X / 2), int16_mid + Step_Y * (Y + 1 - Voxels_Y / 2)); // bottom left 
                mp_vertexArray->addValues(int16_mid + Step_X_Behind * (X - Voxels_X / 2), int16_mid + Step_Y_Behind * (Y + 1 - Voxels_Y / 2)); // top left
            }

            // Bottom face -- for voxels in Top half
            if (Y > Voxels_Y / 2) {
                mp_vertexArray->addValues(int16_mid + Step_X * (X - Voxels_X / 2), int16_mid + Step_Y * (Y - Voxels_Y / 2)); // top left
                mp_vertexArray->addValues(int16_mid + Step_X * (X + 1 - Voxels_X / 2), int16_mid + Step_Y * (Y - Voxels_Y / 2)); // top right
                mp_vertexArray->addValues(int16_mid + Step_X_Behind * (X + 1 - Voxels_X / 2), int16_mid + Step_Y_Behind * (Y - Voxels_Y / 2)); // bottom right
                mp_vertexArray->addValues(int16_mid + Step_X_Behind * (X + 1 - Voxels_X / 2), int16_mid + Step_Y_Behind * (Y - Voxels_Y / 2)); // bottom right
                mp_vertexArray->addValues(int16_mid + Step_X_Behind * (X - Voxels_X / 2), int16_mid + Step_Y_Behind * (Y - Voxels_Y / 2)); // bottom left 
                mp_vertexArray->addValues(int16_mid + Step_X * (X - Voxels_X / 2), int16_mid + Step_Y * (Y - Voxels_Y / 2)); // top left
            }
        }
    }
}

bool Graphics_Io::Get_Window_Should_Close() {
    return glfwWindowShouldClose(mp_window);
}

