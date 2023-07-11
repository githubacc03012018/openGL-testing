#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
 
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW did not init!" << "\n";
        return 1;
    }

    std::cout << glGetString(GL_VERSION) << "\n";

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    float bufferData[6] = {
        -0.5f, -0.5f,
        0.f,    0.5f,
        0.5f,   -0.5f
    };

    glBufferData(buffer, 6 * sizeof(float), bufferData, GL_STATIC_DRAW);


    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_TRIANGLES);

        glColor3f(0, 255, 0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        //glDrawElements(GL_TRIANGLES, 3, );
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
        glEnableVertexAttribArray(0);

        glEnd();
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}