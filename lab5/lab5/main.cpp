//Кирус-Бек, двумерное внешнее отсчение

#define GL_SILENCE_DEPRECATION
#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>

using namespace std;

void clipping();

int startScreenWidth = 1280;
bool isScreenChanged = false;
int screenWidth = 1280, screenHeight = 720;

struct Point {
    double x, y;
};

vector<Point> cutter;
vector<Point> linesPoints;
vector<glm::vec2> innerNormals;
vector<vector<double>> pVectors;

bool drawCutterMode = true, drawLineMode = false, drawContourMode = false, clippingMode = false;

void onResize(GLFWwindow *window, int width, int height) {
    screenWidth = width;
    screenHeight = height;
    cutter.clear();
    linesPoints.clear();
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ENTER:
                drawCutterMode = false;
                drawLineMode = true;
                break;
            case GLFW_KEY_SPACE:
                drawContourMode = true;
                break;
            case GLFW_KEY_P:
                clippingMode = !clippingMode;
                break;
        }
    }
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (drawCutterMode) {
            if (isScreenChanged) {
                cutter.push_back(Point{2*xpos, float(screenHeight) - 2*ypos});
            } else {
                cutter.push_back(Point{xpos, float(screenHeight) - ypos});
            }
        } else {
            if (drawLineMode) {
                if (isScreenChanged) {
                    linesPoints.push_back(Point{2 * xpos, float(screenHeight) - 2*ypos});
                } else {
                    linesPoints.push_back(Point{xpos, float(screenHeight) - ypos});
                }
            }
        }
    }
}

void drawAll() {
    if (drawContourMode) {
        glColor3f(1.f, 0.f, 0.f);
        glBegin(GL_LINE_LOOP);
        for(int i = 0; i < cutter.size(); i++) {
            glVertex2f(float(cutter[i].x), float(cutter[i].y));
        }
        glEnd();
        if (!clippingMode) {
            glColor3f(1.f, 1.f, 1.f);
            glBegin(GL_LINES);
            for(int  i = 0; i < linesPoints.size(); i++) {
                glVertex2f(float(linesPoints[i].x), float(linesPoints[i].y));
            }
            glEnd();
        } else {
            clipping();
        }
    } else {
        glBegin(GL_POINTS);
        glColor3f(1.f, 1.f, 1.f);
        for(int i = 0; i < cutter.size(); i++) {
            glVertex2f(float(cutter[i].x), float(cutter[i].y));
        }
        for(int  i = 0; i < linesPoints.size(); i++) {
            glVertex2f(float(linesPoints[i].x), float(linesPoints[i].y));
        }
        glEnd();
    }
    
}

double getScalarProduct(glm::vec2 firstVector, glm::vec2 secondVector) {
    return firstVector.x * secondVector.x + firstVector.y * secondVector.y;
}

glm::vec2 getInnerNormal(Point firstV, Point secondV, Point thirdV) {
    glm::vec2 vector = glm::vec2{secondV.x - firstV.x, secondV.y - firstV.y};
    glm::vec2 innerNormal = glm::vec2{-1 * vector.y / vector.x , 1};
    glm::vec2 innerLine = glm::vec2{thirdV.x - firstV.x, thirdV.y - firstV.y};
    if (getScalarProduct(innerLine, innerNormal) > 0) {
        return innerNormal;
    }
    else {
        innerNormal.x *= -1;
        innerNormal.y *= -1;
        return innerNormal;
    }
}

void drawNormals(int countOfVertices) {
    glColor3f(1.f, 1.f, 1.f);
    glBegin(GL_LINES);
    for(int i = 0; i < countOfVertices; i++) {
        glVertex2f(cutter[i].x, cutter[i].y);
        glm::vec2 innerNormal = innerNormals[i];
        glVertex2f(cutter[i].x + innerNormal.x * 100, cutter[i].y + innerNormal.y * 100);
    }
    glEnd();
}

glm::vec2 getVectorOfTwoPoints(Point f, Point s) {
    return glm::vec2{s.x - f.x, s.y - f.y};
}

void setInnerNormals() {
    int countOfVertices = (int)cutter.size();
    for(int i = 0; i < countOfVertices; i++) {
        glm::vec2 innerNormal = getInnerNormal(cutter[i], cutter[(i+1) % countOfVertices], cutter[(i+2) % countOfVertices]);
        innerNormals.push_back(innerNormal);
    }
}

void cyrusBeck() {
    int countOfVertices = (int)cutter.size();
    glColor3f(1.f, 1.f, 1.f);
    glBegin(GL_LINES);
    for(int i = 0; i < linesPoints.size(); i += 2) {
        bool lineFullInner = false;
        bool lineFullOuter = false;
        double tOutput = 1;
        double tInput = 0;
        Point f = linesPoints[i];
        Point s = linesPoints[i + 1];
        for(int j = 0; j < countOfVertices; j++) {
            double Pi = getScalarProduct(innerNormals[j], getVectorOfTwoPoints(f, s));
            double Qi = getScalarProduct(getVectorOfTwoPoints(cutter[j], f), innerNormals[j]);
            double t = -1 * Qi / Pi;
            if (Pi == 0) {
                if (Qi < 0) {
                    lineFullOuter = true;
                } else {
                    lineFullInner = true;
                }
            }
            if (Pi > 0) {
                if (tInput < t && t <= 1) {
                    tInput = t;
                }
            }
            if (Pi < 0) {
                if (tOutput > t && t >=0) {
                    tOutput = t;
                }
            }
        }
        if (lineFullInner) {
            continue;
        }
        if (lineFullOuter) {
            glVertex2f(f.x, f.y);
            glVertex2f(s.x, s.y);
            continue;
        }
        glVertex2f(f.x, f.y);
        glVertex2f(f.x + (s.x - f.x) * tInput, f.y + (s.y - f.y) * tInput);
        glVertex2f(f.x + (s.x - f.x) * tOutput, f.y + (s.y - f.y) * tOutput);
        glVertex2f(s.x, s.y);
    }
    glEnd();
}
void clipping( ) {
    innerNormals.clear();
    pVectors.clear();
    setInnerNormals();
    cyrusBeck();
}

int main(int argc, const char * argv[]) {
    GLFWwindow *window;
    
    if (!glfwInit()) {
        return -1;
    }
    
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    window = glfwCreateWindow(screenWidth, screenHeight, "Lab5", NULL, NULL);
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    if (screenWidth != startScreenWidth) {
        isScreenChanged = true;
    }
    
    
    glfwSetFramebufferSizeCallback(window, onResize);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);
    
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    glEnable(GL_ALPHA_TEST);
    
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, screenWidth, 0, screenHeight, -screenWidth, screenWidth);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        drawAll();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    return 0;
}
