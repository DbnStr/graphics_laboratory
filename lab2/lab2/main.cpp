#include <stdlib.h>
#include <stdio.h>

#include <GLFW/glfw3.h>
#include <OPENGL/gl.h>
#include <OPengl/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float angle = 0;
float mainCubeDimension = 0.1f;
float smallCubeDimension = 0.05f;
float difCubeDimension = 0.01f;
float zTranslated = 0.1f;
glm::vec3 offsetMainCube = glm::vec3(0.05f, 0.05f, 0.05f);
glm::vec3 offsetSmallCube = glm::vec3(0.f, 1.f, 0.f);

void error(int error, const char* description)
{
    fputs(description, stderr);
}

void keyPressed(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ENTER)) {
        angle += 2;
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
        mainCubeDimension += difCubeDimension;
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        mainCubeDimension -= difCubeDimension;
    }
    if (glfwGetKey(window, GLFW_KEY_G)) {
        zTranslated += 0.005f;
    }
    
}

void drawCube(float a)
{
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f);              //нижняя грань
    glVertex3f(0, 0, 0);
    glVertex3f(0, a, 0.0f);
    glVertex3f(a, a, 0.0f);
    glVertex3f(a, 0, 0.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glColor3f(0.0f, 1.0f, 0.0f);              // верхняя грань
    glVertex3f(0, 0, a);
    glVertex3f(0, a, a);
    glVertex3f(a, a, a);
    glVertex3f(a, 0, a);
    glEnd();
    
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 1.0f);              // правая
    glVertex3f(0, a, a);
    glVertex3f(0, a, 0.0f);
    glVertex3f(a, a, 0.0f);
    glVertex3f(a, a, a);
    glEnd();
    
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 0.0f);              // левая
    glVertex3f(0, 0, a);
    glVertex3f(a, 0, a);
    glVertex3f(a, 0, 0.0f);
    glVertex3f(0, 0, 0.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glColor3f(0.0f, 1.0f, 1.0f);              // задняя
    glVertex3f(0, 0, a);
    glVertex3f(0, a, a);
    glVertex3f(0, a, 0.0f);
    glVertex3f(0, 0, 0.0f);
    glEnd();
    
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 1.0f);              // передняя
    glVertex3f(a, 0, a);
    glVertex3f(a, a, a);
    glVertex3f(a, a, 0.0f);
    glVertex3f(a, 0, 0.0f);
    glEnd();
}

void drawAxes()
{
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0f, 0.0f, 0.0f); //x - red
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(10.0f, 0.0f, 0.0f);
    glEnd();
    
    glBegin(GL_LINE_STRIP);
    glColor3f(0.0f, 1.0f, 0.0f); //y - green
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 10.0f, 0.0f);
    glEnd();
    
    glBegin(GL_LINE_STRIP);
    glColor3f(0.0f, 0.0f, 1.0f); //z - blue
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 10.0f);
    glEnd();
}

void drawSmallCube()
{
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, offsetSmallCube);
    
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(modelMatrix));
    
    drawCube(smallCubeDimension);
    
    glPopMatrix();
}

void drawMainCube() {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, offsetMainCube);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(1.f, 0.f, 0.f));
    
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(modelMatrix));
    
    drawCube(mainCubeDimension);
    
    glPopMatrix();
}

void display(GLFWwindow* window)
{
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::rotate(viewMatrix, glm::radians(-135.0f), glm::vec3(1.f,0.f,0.f));
    viewMatrix = glm::rotate(viewMatrix, glm::radians(45.0f), glm::vec3(0.f,0.f,1.f));
    
    glm::mat4 projectionMatrix = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        -0.5*(glm::cos(glm::pi<float>() / 6)), -0.5*(glm::sin(glm::pi<float>() / 6)), -1, 0,
        0, 0, 0, 1
    };
    
    int width, height;
    
    keyPressed(window);
    
    glfwGetFramebufferSize(window, &width, &height);
    
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(glm::value_ptr(projectionMatrix));
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(glm::value_ptr(viewMatrix));
    
    drawAxes();
    drawSmallCube();
    drawMainCube();
}

int main()
{
    glfwSetErrorCallback(error);
    
    if (!glfwInit())
        exit(1);
    
    GLFWwindow* window = glfwCreateWindow(1280, 1920, "Simple example", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        exit(1);
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    
    while (!glfwWindowShouldClose(window))
    {
        display(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
