#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "document.h"
#include <istreamwrapper.h> 
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

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

struct ReturnStruct {
	float* vertices;
	unsigned int* indices;
	int vSize = 0;
	int iSize = 0;
};

ReturnStruct parseJson(const char* filePath) {
	std::ifstream ifs(filePath);
	rapidjson::IStreamWrapper isw(ifs);
	rapidjson::Document document;
	document.ParseStream(isw);

	if (document.HasParseError()) {
		std::cout << "Error : " << document.GetParseError() << '\n';
		std::cout << "Offset : " << document.GetErrorOffset() << '\n';
	}

	const auto& object = document.FindMember("geometry_object")->value;
	const auto& verts = object.FindMember("vertices")->value;
	const auto& indx = object.FindMember("triangles")->value;

	int vertsSize = verts.Size();
	int indeciesSize = indx.Size();

	float* vertices = new float[vertsSize];
	unsigned int* indices = new unsigned int[indeciesSize];

	int i = 0;
	for (; i < vertsSize; i++) {
		vertices[i] = verts.GetArray()[i].GetFloat();
	}
	i = 0;
	for (; i < indeciesSize; i++) {
		indices[i] = (unsigned int)indx.GetArray()[i].GetInt();
	}

	return { vertices , indices, vertsSize, indeciesSize };
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

	const char* filePath = "res/teapot.json";
	auto res = parseJson(filePath);

	//float vertices[] = {
	//   -0.5f, -0.5f,
	//   0.5f,    0.5f,
	//   0.5f,   -0.5f,
	//   -0.5f,   0.5f
	//};

	//unsigned int indices[] = {
	//	0, 1, 2,
	//	//0,3,1
	//};  
	
	// bind the vertices
	unsigned int buffer, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &buffer);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(res.vertices[0]) * res.vSize, res.vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	// bind the indices
	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(res.indices[0]) * res.iSize, res.indices, GL_STATIC_DRAW);

	glUseProgram(shader);
	int uniformColor = glGetUniformLocation(shader, "u_Color");

	//glm::mat4 proj = glm::ortho(-80.f, 80.f, -80.f, 80.f, -1.f, 1.f);

	glm::mat4 proj = glm::ortho(-2.f, 2.f, -2.f, 2.f, -1.f, 1.f);
	//glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(0,0));
	//glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(-10.f, 4.f, 0.f));


	int unifromProjection = glGetUniformLocation(shader, "u_MVP");
	glUniformMatrix4fv(unifromProjection, 1, false, glm::value_ptr(proj));

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
		glDrawElements(GL_TRIANGLES, res.iSize, GL_UNSIGNED_INT, nullptr);
		glGetError();

		glUniform4f(uniformColor, r, 0.5f, 0.2f, 1.0f);

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