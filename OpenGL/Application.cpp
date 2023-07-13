#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

unsigned int loadShader(const unsigned int type, const std::string& source) {
	unsigned int shaderId = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shaderId, 1, &src, nullptr);
	glCompileShader(shaderId);

	int result = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);

		char* message = (char*)_malloca(sizeof(char) * length);
		glGetShaderInfoLog(shaderId, length, &length, message);
		std::cout << printf("Error shader failed to compile, %s", message);

		glDeleteShader(shaderId);

		return 0;
	}

	return shaderId;
}

struct ShaderSource {
	std::string VertexShader;
	std::string FragmentShader;
};

ShaderSource getShaderFromFile(const std::string& filePath) {
	
	std::ifstream stream(filePath);
	std::string line;
	std::stringstream ss[2];

	enum ShaderType {
		None = -1, Vertex = 0, Fragment = 1
	};
	ShaderType type = ShaderType::None;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::Vertex;
			}
			else { // only vertex and fragment shaders for now
				type = ShaderType::Fragment;
			}
		}
		else {
			ss[(int)type] << line << "\n";
		}
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int loadShaders(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();

	unsigned int vs = loadShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(640, 480, "OpenGL Examples", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW did not init!" << "\n";
		return 1;
	}

	std::cout << glGetString(GL_VERSION) << "\n";
	 
	auto shaderSource = getShaderFromFile("res/Basic.shader");
	unsigned int shader = loadShaders(shaderSource.VertexShader, shaderSource.FragmentShader);

	float vertices[] = {
	   -0.5f, -0.5f,
	   0.5f,    0.5f,
	   0.5f,   -0.5f,
	   -0.5f,   0.5f
	};

	unsigned int indices[] = {
		0, 1, 2,
		0,3,1
	};

	// bind the vertices
	unsigned int buffer, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &buffer);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	// bind the indices
	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glUseProgram(shader);
	int location = glGetUniformLocation(shader, "u_Color");
	
	float r = 0.f;
	float incrementAmount = 0.005f;
	while (!glfwWindowShouldClose(window))
	{
		if (r > 1.f) {
			incrementAmount = -0.005f;
		}
		else if (r < 0.f) {
			incrementAmount = 0.005f;
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glUniform4f(location, r, 0.5f, 0.2f, 1.0f);

		r += incrementAmount;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &buffer);
	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}