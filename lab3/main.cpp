#include <stdlib.h>
#include <stdio.h>

#include <GLFW/glfw3.h>
#include <OPENGL/gl.h>
#include <OPengl/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
/*
    a/d - уменьшить/увеличить lambda(коэффициент для определения вершин усеченных относительно верхней вершины обычной пирамиды)
    w/s - увеличить/уменьшить высоту пирамиды
    z/x - уменьшить/увеличить измерение нижней грани
    enter - увеличить угол вращения
 */
glm::vec3 pyramidOffset =  glm::vec3(0.1f, 0.1f, 0.f);

struct BottomPlane {
    glm::vec3 topLeft;
    glm::vec3 topRight;
    glm::vec3 botRight;
    glm::vec3 botLeft;
}
float angle = 0;
float pyramidBaseEdge = 0.1f;
float difPyramidBaseEdge = 0.01f;
float pyramidHeight = 0.2f;
float difPyramideHeight = 0.05f;
float lambda = 1.0f;
float difLambda = 0.03f;

float smallCubeDimension = 0.05f;
glm::vec3 offsetSmallCube = glm::vec3(0.f, 1.f, 0.f);

float getCoordinate(float fCoordinate, float sCoordinate, float lambda) {
    return (fCoordinate + sCoordinate * lambda) / (1 + lambda);
}

glm::vec3 getVector(glm::vec3 fPoint, glm::vec3 sPoint, float lambda) {
    return glm::vec3(getCoordinate(fPoint.x, sPoint.x, lambda),
                     getCoordinate(fPoint.y, sPoint.y, lambda),
                     getCoordinate(fPoint.z, sPoint.z, lambda));
}

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
        lambda += difLambda;
    }
    if (glfwGetKey(window, GLFW_KEY_A) && lambda > difLambda) {
        lambda -= difLambda;
    }
    if (glfwGetKey(window, GLFW_KEY_W)) {
        pyramidHeight += difPyramideHeight;
    }
    if (glfwGetKey(window, GLFW_KEY_S) && pyramidHeight > difPyramideHeight) {
        pyramidHeight -= difPyramideHeight;
    }
    if (glfwGetKey(window, GLFW_KEY_X)) {
        pyramidBaseEdge += difPyramidBaseEdge;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) && pyramidBaseEdge > difPyramidBaseEdge) {
        pyramidBaseEdge -= difPyramidBaseEdge;
    }
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

void drawPiramid(float baseEdge, float height)
{
    float SQRT_TWO = glm::sqrt(2.0f);
    float centerPyramidX = baseEdge / SQRT_TWO;
    float centerPyramidY = centerPyramidX;
    glm::vec3 upVertex = glm::vec3(centerPyramidX, centerPyramidY, height);
    
    glBegin(GL_QUADS); //основание
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, baseEdge, 0.0f);
    glVertex3f(baseEdge, baseEdge, 0.0f);
    glVertex3f(baseEdge, 0.0f, 0.0f);
    glEnd();
    
    glBegin(GL_TRIANGLE_STRIP); //задняя грань
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3fv(glm::value_ptr(upVertex));
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, baseEdge, 0.0f);
    glEnd();
    
    glBegin(GL_TRIANGLE_STRIP); //правая грань
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3fv(glm::value_ptr(upVertex));
    glVertex3f(0.0f, baseEdge, 0.0f);
    glVertex3f(baseEdge, baseEdge, 0.0f);
    glEnd();
    
    glBegin(GL_TRIANGLE_STRIP); //передняя грань
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3fv(glm::value_ptr(upVertex));
    glVertex3f(baseEdge, 0.0f, 0.0f);
    glVertex3f(baseEdge, baseEdge, 0.0f);
    glEnd();
    
    glBegin(GL_TRIANGLE_STRIP); //левая грань
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3fv(glm::value_ptr(upVertex));
    glVertex3f(baseEdge, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();
}

void drawTruncatedPyramid(float baseEdge, float height, float botLambda, float topLambda)
{
    float SQRT_TWO = glm::sqrt(2.0f);
    float centerPyramidX = baseEdge / SQRT_TWO;
    float centerPyramidY = centerPyramidX;
    glm::vec3 upVertex = glm::vec3(centerPyramidX, centerPyramidY, height);
    //вершины основания пирамиды
    glm::vec3 topLeft = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 topRight = glm::vec3(0.0f, baseEdge, 0.0f);
    glm::vec3 botLeft = glm::vec3(baseEdge, 0.0f, 0.0f);
    glm::vec3 botRight = glm::vec3(baseEdge, baseEdge, 0.0f);
    
    glBegin(GL_QUADS); //основание
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3fv(glm::value_ptr(topLeft));
    glVertex3fv(glm::value_ptr(topRight));
    glVertex3fv(glm::value_ptr(botRight));
    glVertex3fv(glm::value_ptr(botLeft));
    glEnd();
    
    glBegin(GL_QUADS); //задняя грань
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3fv(glm::value_ptr(getVector(topRight, upVertex, lambda)));
    glVertex3fv(glm::value_ptr(getVector(topLeft, upVertex, lambda)));
    glVertex3fv(glm::value_ptr(topLeft));
    glVertex3fv(glm::value_ptr(topRight));
    glEnd();
    
    glBegin(GL_QUADS); //правая грань
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3fv(glm::value_ptr(getVector(botRight, upVertex, lambda)));
    glVertex3fv(glm::value_ptr(getVector(topRight, upVertex, lambda)));
    glVertex3fv(glm::value_ptr(topRight));
    glVertex3fv(glm::value_ptr(botRight));
    glEnd();
    
    glBegin(GL_QUADS); //передняя грань
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3fv(glm::value_ptr(getVector(botRight, upVertex, lambda)));
    glVertex3fv(glm::value_ptr(getVector(botLeft, upVertex, lambda)));
    glVertex3fv(glm::value_ptr(botLeft));
    glVertex3fv(glm::value_ptr(botRight));
    glEnd();
    
    glBegin(GL_QUADS); //левая грань
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3fv(glm::value_ptr(getVector(botLeft, upVertex, lambda)));
    glVertex3fv(glm::value_ptr(getVector(topLeft, upVertex, lambda)));
    glVertex3fv(glm::value_ptr(topLeft));
    glVertex3fv(glm::value_ptr(botLeft));
    glEnd();
    
    glBegin(GL_QUADS); //верхняя грань
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3fv(glm::value_ptr(getVector(topRight, upVertex, lambda)));
    glVertex3fv(glm::value_ptr(getVector(topLeft, upVertex, lambda)));
    glVertex3fv(glm::value_ptr(getVector(botLeft, upVertex, lambda)));
    glVertex3fv(glm::value_ptr(getVector(botRight, upVertex, lambda)));
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

void drawOurPiramid(float baseEdge, float height) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, pyramidOffset);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(1.f, 0.f, 0.f));
    
    int k = 3;
    for (int i = 1; i < k; i++) {
        float botLambda = (i - 1) * lambda / k;
        float topLambda = i * lambda / k;
    }
    
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(modelMatrix));
    
    drawTruncatedPyramid(baseEdge, height);
    
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
    
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glMultMatrixf(glm::value_ptr(projectionMatrix));
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(glm::value_ptr(viewMatrix));
    
    drawAxes();
    drawSmallCube();
    drawOurPiramid(pyramidBaseEdge, pyramidHeight);
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

