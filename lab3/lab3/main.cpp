#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "iostream"

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
 c/v - уменьшить/увеличить число разбиений пирамиды
 */
struct Plane {
    int size;
    std::vector<glm::vec3> vertexs;
    glm::vec3 centerCircle;
};

std::vector<Plane> planes;

glm::vec3 pyramidOffset =  glm::vec3(0.5f, 0.0f, 0.1f);
float angle = 0;
float pyramidBaseEdge = 0.2f;
float difPyramidBaseEdge = 0.01f;
float pyramidHeight = 2.0f;
float difPyramideHeight = 0.05f;
float lambda = 1.0f;
float difLambda = 0.03f;
int numberOfSplits = 100;
int difNumberOFSplits = 1;
int numberOfVertexs = 4;

bool characteristicsChanged = true;
bool carcassMode = false;

float smallCubeDimension = 0.05f;
glm::vec3 offsetSmallCube = glm::vec3(0.f, 0.f, 0.5f);

glm::mat4 projectionMatrix = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    -0.5*(glm::cos(glm::pi<float>() / 6)), -0.5*(glm::sin(glm::pi<float>() / 6)), -1, 0,
    0, 0, 0, 1
};

float getCoordinate(float fCoordinate, float sCoordinate, float lambda) {
    return (fCoordinate + sCoordinate * lambda) / (1 + lambda);
}

glm::vec3 getVector(glm::vec3 fPoint, glm::vec3 sPoint, float lambda) {
    return glm::vec3(getCoordinate(fPoint.x, sPoint.x, lambda),
                     getCoordinate(fPoint.y, sPoint.y, lambda),
                     getCoordinate(fPoint.z, sPoint.z, lambda));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, width);
}

void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        carcassMode = !carcassMode;
    if (key == GLFW_KEY_UP) {
        numberOfVertexs += 1;
        characteristicsChanged = true;
    }
    if (key == GLFW_KEY_DOWN) {
        numberOfVertexs -= 1;
        characteristicsChanged = true;
    }
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
        characteristicsChanged = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) && (lambda > difLambda)) {
        lambda -= difLambda;
        characteristicsChanged = true;
    }
    if (glfwGetKey(window, GLFW_KEY_W)) {
        pyramidHeight += difPyramideHeight;
        characteristicsChanged = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) && (pyramidHeight > difPyramideHeight)) {
        pyramidHeight -= difPyramideHeight;
        characteristicsChanged = true;
    }
    if (glfwGetKey(window, GLFW_KEY_X)) {
        pyramidBaseEdge += difPyramidBaseEdge;
        characteristicsChanged = true;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) && (pyramidBaseEdge > difPyramidBaseEdge)) {
        pyramidBaseEdge -= difPyramidBaseEdge;
        characteristicsChanged = true;
    }
    if (glfwGetKey(window, GLFW_KEY_C) && (numberOfSplits > 1)) {
        numberOfSplits -= difNumberOFSplits;
        characteristicsChanged = true;
    }
    if (glfwGetKey(window, GLFW_KEY_V)) {
        numberOfSplits += difNumberOFSplits;
        characteristicsChanged = true;
    }
}

void drawAxes()
{
//    glBegin(GL_LINE_STRIP);
//    glColor3f(1.0f, 0.0f, 0.0f); //x - red
//    glVertex3f(0.0f, 0.0f, 0.0f);
//    glVertex3f(10.0f, 0.0f, 0.0f);
//    glEnd();
//    
//    glBegin(GL_LINE_STRIP);
//    glColor3f(0.0f, 1.0f, 0.0f); //y - green
//    glVertex3f(0.0f, 0.0f, 0.0f);
//    glVertex3f(0.0f, 10.0f, 0.0f);
//    glEnd();
//    
//    glBegin(GL_LINE_STRIP);
//    glColor3f(0.0f, 0.0f, 1.0f); //z - blue
//    glVertex3f(0.0f, 0.0f, 0.0f);
//    glVertex3f(0.0f, 0.0f, 30.0f);
//    glEnd();
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

void drawTruncatedPyramid(Plane *bottomPlane, Plane *topPlane)
{
    glColor3f(1.0f, 0.0f, 0.0f); //нижнее основание
    for(int i = 0; i < bottomPlane->size; i++) {
        glBegin(GL_TRIANGLES); //нижнее основание
        glVertex3fv(glm::value_ptr(bottomPlane->vertexs[i]));
        glVertex3fv(glm::value_ptr(bottomPlane->centerCircle));
        glVertex3fv(glm::value_ptr(bottomPlane->vertexs[(i+1) % bottomPlane->size]));
        glEnd();
    }

    glColor3f(1.0f, 0.0f, 0.0f); //верхнее основание
    for(int i = 0; i < bottomPlane->size; i++) {
        glBegin(GL_TRIANGLES);
        glVertex3fv(glm::value_ptr(topPlane->vertexs[i]));
        glVertex3fv(glm::value_ptr(topPlane->centerCircle));
        glVertex3fv(glm::value_ptr(topPlane->vertexs[(i+1) % topPlane->size]));
        glEnd();
    }
    
    int size = bottomPlane->size;
    glBegin(GL_QUADS);
    for(int i = 0; i < size; i++) {
        switch (i % 3) {
            case 0:
                glColor3f(1.f, 1.f, 1.f);
                break;
            case 1:
                glColor3f(0.0f, 0.0f, 1.f);
                break;
            case 2:
                glColor3f(1.f, 0.f, 0.f);
                break;
            default:
                break;
        }
        glVertex3fv(glm::value_ptr(topPlane->vertexs[i]));
        glVertex3fv(glm::value_ptr(topPlane->vertexs[(i+1) % size]));
        glVertex3fv(glm::value_ptr(bottomPlane->vertexs[(i+1) % size]));
        glVertex3fv(glm::value_ptr(bottomPlane->vertexs[i]));
        glClearColor(0, 0, 0, 0);
    }
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
    
    if (characteristicsChanged) {
        planes.clear();
        glm::vec3 origin = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 upVertex = glm::vec3(0.f, height, 0.f);
        
        Plane lowBotPlane;
        lowBotPlane.size = numberOfVertexs;
        for (int j = 0; j < numberOfVertexs; j++) {
            lowBotPlane.vertexs.push_back(glm::vec3(baseEdge * cos(j * 2 * M_PI / numberOfVertexs), 0, baseEdge * sin(j * 2 * M_PI / numberOfVertexs)));
        }
        
        planes.push_back(lowBotPlane);
        
        for (int i = 1; i <= numberOfSplits; i++) {
            float botLambda = (i - 1) * lambda / numberOfSplits;
            float topLambda = i * lambda / numberOfSplits;
            Plane botPlane;
            botPlane.size = numberOfVertexs;
            botPlane.centerCircle = getVector(origin, upVertex, botLambda);
            for(int j = 0; j < botPlane.size; j++) {
                botPlane.vertexs.push_back(getVector(lowBotPlane.vertexs[j], upVertex, botLambda));
            }
            
            Plane topPlane;
            topPlane.size = numberOfVertexs;
            topPlane.centerCircle = getVector(origin, upVertex, topLambda);
            for(int j = 0; j < topPlane.size; j++) {
                topPlane.vertexs.push_back(getVector(lowBotPlane.vertexs[j], upVertex, topLambda));
            }
            
            planes.push_back(topPlane);
            
            glPushMatrix();
            glMultMatrixf(glm::value_ptr(modelMatrix)); //почему не можем вынести поп матрикс за for
            drawTruncatedPyramid(&botPlane, &topPlane);
            glPopMatrix();
        }
        characteristicsChanged = false;
    }
    else {
        for(int i = 0; i < planes.size() - 1; i++) {
            glPushMatrix();
            glMultMatrixf(glm::value_ptr(modelMatrix));
            drawTruncatedPyramid(&planes[i], &planes[i+1]);
            glPopMatrix();
        }
    }
}

void display(GLFWwindow* window)
{
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    
    keyPressed(window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (carcassMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(glm::value_ptr(projectionMatrix));
    
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
    
    GLFWwindow* window = glfwCreateWindow(800, 800, "Simple example", NULL, NULL);
    
    glfwSetKeyCallback(window, key);
    
    if (!window)
    {
        glfwTerminate();
        exit(1);
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE); glDepthFunc(GL_LEQUAL);
    glDepthRange (0.0f, 1.0f);
    
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

