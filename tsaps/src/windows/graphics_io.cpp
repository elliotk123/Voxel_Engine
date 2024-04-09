#include <graphics_io.h>
#include <iostream>
#include <limits>
#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/gtc/type_ptr.hpp>

// Define window size
const int WIDTH = 800;
const int HEIGHT = 800;

// voxels
const int Voxels_X = 32;
const int Voxels_Y = 32;
const int Voxels_Z = 32;
const int Voxel_Tot = 32768;

//graphics 
const __int16 int16_max = std::numeric_limits<int16_t>::max();
const __int16 int16_min = std::numeric_limits<int16_t>::min();
const int int16_values = 65536; //2**16

//voxel step size  == Floor(int16_values - 1 / Voxels_*)
const __int16 Step_X = 2047;
const __int16 Step_Y = 2047;
const __int16 Step_Z = 2047;

const glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)WIDTH / (float)HEIGHT, 1.0f, 3.0f);


// Define vertex shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection*vec4(aPos.x, aPos.y, aPos.z -2.0f, 1.0);\n"
"}\0";

// Define fragment shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

Graphics_Io::~Graphics_Io() {
    // Deallocate resources and terminate glfw
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
}

Graphics_Io::Graphics_Io() {
    // Initialize glfw and create a window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Triangle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

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
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for shader program linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete the shader objects after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Generate a vertex buffer object and a vertex array object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind the vertex array object and the vertex buffer object
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copy the triangle vertices to the buffer
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_SHORT, GL_TRUE, 3 * sizeof(__int16), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind the vertex array object and the vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    vertices = new __int16[(Voxel_Tot * 3 * 2 * 5 * 3)]();
    projectionLoc = glGetUniformLocation(shaderProgram, "projection");
}

void Graphics_Io::Update_Screen() {
    // Process input
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Clear the color buffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the shader program and draw the triangle
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copy the triangle vertices to the buffer
    int size = Voxel_Count * 90 * sizeof(__int16);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, Voxel_Count*30);

    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();

}

void Graphics_Io::Send_Voxels(__int8* Voxels_Out) {
    Voxel_Count = 0;
    for (int i = 0; i < Voxels_X; i++) {
        for (int j = 0; j < Voxels_Y; j++) {
            for (int k = 0; k < Voxels_Z; k++) {
                if (Voxels_Out[i * Voxels_Y * Voxels_Z + j * Voxels_Z + k] > 0) {
                    Voxel_Count++;
                }

            }
        }
    }
    int Vertex_Counter = 0;
    for (int i = 0; i < Voxels_X; i++) {
        for (int j = 0; j < Voxels_Y; j++) {
            for (int k = 0; k < Voxels_Z; k++) {
                if (Voxels_Out[i * Voxels_Y * Voxels_Z + j * Voxels_Z + k] > 0) {
                    Vertex_Counter++;
                    //LEFT
                    //1
                    vertices[(90 * (Vertex_Counter - 1))] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+1] = int16_min + Step_Y * (j+1);
                    vertices[(90 * (Vertex_Counter - 1))+2] = int16_min + Step_Z * (k+1);
                    //2
                    vertices[(90 * (Vertex_Counter - 1))+3] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+4] = int16_min + Step_Y * (j + 1);
                    vertices[(90 * (Vertex_Counter - 1))+5] = int16_min + Step_Z * (k);
                    //3
                    vertices[(90 * (Vertex_Counter - 1))+6] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+7] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+8] = int16_min + Step_Z * k;
                    //3
                    vertices[(90 * (Vertex_Counter - 1))+9] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+10] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+11] = int16_min + Step_Z * k;
                    //4
                    vertices[(90 * (Vertex_Counter - 1))+12] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+13] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+14] = int16_min + Step_Z * (k+1);
                    //1
                    vertices[(90 * (Vertex_Counter - 1))+15] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+16] = int16_min + Step_Y * (j + 1);
                    vertices[(90 * (Vertex_Counter - 1))+17] = int16_min + Step_Z * (k + 1);
                    //FRONT
                    //1
                    vertices[(90 * (Vertex_Counter - 1))+18] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+19] = int16_min + Step_Y * (j + 1);
                    vertices[(90 * (Vertex_Counter - 1))+20] = int16_min + Step_Z * k;
                    //2
                    vertices[(90 * (Vertex_Counter - 1))+21] = int16_min + Step_X * (i+1);
                    vertices[(90 * (Vertex_Counter - 1))+22] = int16_min + Step_Y * (j + 1);
                    vertices[(90 * (Vertex_Counter - 1))+23] = int16_min + Step_Z * k;
                    //3
                    vertices[(90 * (Vertex_Counter - 1))+24] = int16_min + Step_X * (i + 1);
                    vertices[(90 * (Vertex_Counter - 1))+25] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+26] = int16_min + Step_Z * k;
                    //3
                    vertices[(90 * (Vertex_Counter - 1))+27] = int16_min + Step_X * (i + 1);
                    vertices[(90 * (Vertex_Counter - 1))+28] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+29] = int16_min + Step_Z * k;
                    //4
                    vertices[(90 * (Vertex_Counter - 1))+30] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+31] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+32] = int16_min + Step_Z * k;
                    //1
                    vertices[(90 * (Vertex_Counter - 1))+33] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+34] = int16_min + Step_Y * (j + 1);
                    vertices[(90 * (Vertex_Counter - 1))+35] = int16_min + Step_Z * k;
                    //RIGHT
                    //1
                    vertices[(90 * (Vertex_Counter - 1))+36] = int16_min + Step_X * (i+1);
                    vertices[(90 * (Vertex_Counter - 1))+37] = int16_min + Step_Y * (j + 1);
                    vertices[(90 * (Vertex_Counter - 1))+38] = int16_min + Step_Z * k;
                    //2
                    vertices[(90 * (Vertex_Counter - 1))+39] = int16_min + Step_X * (i + 1);
                    vertices[(90 * (Vertex_Counter - 1))+40] = int16_min + Step_Y * (j + 1);
                    vertices[(90 * (Vertex_Counter - 1))+41] = int16_min + Step_Z * (k + 1);
                    //3
                    vertices[(90 * (Vertex_Counter - 1))+42] = int16_min + Step_X * (i + 1);
                    vertices[(90 * (Vertex_Counter - 1))+43] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+44] = int16_min + Step_Z * (k+1);
                    //3
                    vertices[(90 * (Vertex_Counter - 1))+45] = int16_min + Step_X * (i + 1);
                    vertices[(90 * (Vertex_Counter - 1))+46] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+47] = int16_min + Step_Z * (k+1);
                    //4
                    vertices[(90 * (Vertex_Counter - 1))+48] = int16_min + Step_X * (i+1);
                    vertices[(90 * (Vertex_Counter - 1))+49] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+50] = int16_min + Step_Z * k;
                    //1
                    vertices[(90 * (Vertex_Counter - 1))+51] = int16_min + Step_X * (i+1);
                    vertices[(90 * (Vertex_Counter - 1))+52] = int16_min + Step_Y * (j + 1);
                    vertices[(90 * (Vertex_Counter - 1))+53] = int16_min + Step_Z * k;
                    //TOP
                    //1
                    vertices[(90 * (Vertex_Counter - 1))+54] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+55] = int16_min + Step_Y * (j + 1);
                    vertices[(90 * (Vertex_Counter - 1))+56] = int16_min + Step_Z * (k+1);
                    //2
                    vertices[(90 * (Vertex_Counter - 1))+57] = int16_min + Step_X * (i + 1);
                    vertices[(90 * (Vertex_Counter - 1))+58] = int16_min + Step_Y * (j + 1);
                    vertices[(90 * (Vertex_Counter - 1))+59] = int16_min + Step_Z * (k + 1);
                    //3
                    vertices[(90 * (Vertex_Counter - 1))+60] = int16_min + Step_X * (i + 1);
                    vertices[(90 * (Vertex_Counter - 1))+61] = int16_min + Step_Y * (j+1);
                    vertices[(90 * (Vertex_Counter - 1))+62] = int16_min + Step_Z * k;
                    //3
                    vertices[(90 * (Vertex_Counter - 1))+63] = int16_min + Step_X * (i + 1);
                    vertices[(90 * (Vertex_Counter - 1))+64] = int16_min + Step_Y * (j+1);
                    vertices[(90 * (Vertex_Counter - 1))+65] = int16_min + Step_Z * k;
                    //4
                    vertices[(90 * (Vertex_Counter - 1))+66] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+67] = int16_min + Step_Y * (j+1);
                    vertices[(90 * (Vertex_Counter - 1))+68] = int16_min + Step_Z * k;
                    //1
                    vertices[(90 * (Vertex_Counter - 1))+69] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+70] = int16_min + Step_Y * (j + 1);
                    vertices[(90 * (Vertex_Counter - 1))+71] = int16_min + Step_Z * (k+1);
                    //BOTTOM
                    //1
                    vertices[(90 * (Vertex_Counter - 1))+72] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+73] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+74] = int16_min + Step_Z * k;
                    //2
                    vertices[(90 * (Vertex_Counter - 1))+75] = int16_min + Step_X * (i + 1);
                    vertices[(90 * (Vertex_Counter - 1))+76] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+77] = int16_min + Step_Z * k;
                    //3
                    vertices[(90 * (Vertex_Counter - 1))+78] = int16_min + Step_X * (i + 1);
                    vertices[(90 * (Vertex_Counter - 1))+79] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+80] = int16_min + Step_Z * (k+1);
                    //3
                    vertices[(90 * (Vertex_Counter - 1))+81] = int16_min + Step_X * (i + 1);
                    vertices[(90 * (Vertex_Counter - 1))+82] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+83] = int16_min + Step_Z * (k+1);
                    //4
                    vertices[(90 * (Vertex_Counter - 1))+84] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+85] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+86] = int16_min + Step_Z * (k+1);
                    //1
                    vertices[(90 * (Vertex_Counter - 1))+87] = int16_min + Step_X * i;
                    vertices[(90 * (Vertex_Counter - 1))+88] = int16_min + Step_Y * j;
                    vertices[(90 * (Vertex_Counter - 1))+89] = int16_min + Step_Z * k;
                }
            }
        }
    }
}

bool Graphics_Io::Get_Window_Should_Close() {
    return glfwWindowShouldClose(window);
}

