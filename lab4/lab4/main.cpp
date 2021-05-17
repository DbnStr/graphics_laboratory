#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <math.h>

using namespace std;

int startScreenWidth = 1280;
bool isScreenChanged = false;
int screenWidth = 1280, screenHeight = 720;

struct Point {
    int x, y;
};

struct Pixel {
    float r, g, b, alpha;
};

Pixel *buffer = NULL;
vector<Point> vertices;
vector<vector<Point>> lines;

void clearBuffer() {
    Pixel clearColor{0, 0, 0, 1};
    for (int i = 0; i < screenWidth * screenHeight; i++) {
        buffer[i] = clearColor;
    }
}

Pixel contourColor{1, 1, 1, 0.99};
Pixel vertexColor{0, 0, 1, 0.99};
Pixel fillColor{1, 1, 1, 1};
bool inputMode = true, fillMode = false, postfiltrationMode = false;



Pixel *getPixel(Point point) {
    return &buffer[point.y*screenWidth + point.x];
}

void setPixel(Point point, Pixel pixel) {
    Pixel *p = getPixel(point);
    *p = pixel;
}

void makeEdges(Point p1, Point p2) {
    vector<Point> line;
    if (p1.y <= p2.y) {
        for (int y = p1.y; y < p2.y; y++) {
            int x = p1.x + double((y - p1.y) * (p2.x - p1.x)) / double(p2.y - p1.y);
            line.push_back(Point{x, y});
        }
    } else {
        for (int y = p2.y; y < p1.y; y++) {
            int x = p1.x + double((y - p1.y) * (p2.x - p1.x)) / double(p2.y - p1.y);
            line.push_back(Point{x, y});
        }
    }
    lines.push_back(line);
    
    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);
    if (dy >= dx) {
        if (p1.y <= p2.y) {
            for (int y = p1.y; y < p2.y; y++) {
                int x = p1.x + double((y - p1.y) * (p2.x - p1.x)) / double(p2.y - p1.y);
                setPixel(Point{x, y}, contourColor);
            }
        } else {
            for (int y = p2.y; y < p1.y; y++) {
                int x = p1.x + double((y - p1.y) * (p2.x - p1.x)) / double(p2.y - p1.y);
                setPixel(Point{x, y}, contourColor);
            }
        }
    } else {
        if (p1.x <= p2.x) {
            for (int x = p1.x; x < p2.x; x++) {
                int y = p1.y + double((x - p1.x) * (p2.y - p1.y)) / double(p2.x - p1.x);
                setPixel(Point{x, y}, contourColor);
            }
        } else {
            for (int x = p2.x; x < p1.x; x++) {
                int y = p1.y + double((x - p1.x) * (p2.y - p1.y)) / double(p2.x - p1.x);
                setPixel(Point{x, y}, contourColor);
            }
        }
    }
}

void makeContour() {
    for (int i = 0; i < vertices.size() - 1; i++) {
        makeEdges(vertices[i], vertices[i + 1]);
    }
}

vector<int> insert(vector<int> xlist, int x) {
    int size = xlist.size();
    vector<int> result;
    for(int i = 0; i < size; i++) {
        if (x < xlist[i]) {
            result.push_back(x);
            for(int j = i; i < size; i++) {
                result.push_back(xlist[j]);
            }
            return result;
        }
        result.push_back(xlist[i]);
    }
    result.push_back(x);
    return result;
}

void fillContour() {
    std::cout << lines.size();
    int maxY = INT_MIN;
    int minY = INT_MAX;
    for (int i = 0; i < lines.size(); i++) {
        vector<Point> line = lines[i];
        int size = line.size();
        for (int j = 0; j < size; j++) {
            if (line[j].y > maxY) {
                maxY = line[j].y;
            }
            if (line[j].y < minY) {
                minY = line[j].y;
            }
        }
    }
    vector<vector<int>> xlists;
    
    for(int y = minY; y < maxY; y++) {
        vector<int> xlist;
        for (int i = 0; i < lines.size(); i++) {
            vector<Point> line = lines[i];
            int size = line.size();
            for (int j = 0; j < size; j++) {
                if (line[j].y == y) {
                    xlist = insert(xlist, line[j].x);
                }
            }
        }
        xlists.push_back(xlist);
    }
    int y = minY;
    for(int i = 0; i < xlists.size(); i++) {
        vector<int> xlist = xlists[i];
        int size = xlist.size();
        for(int j = 0; j < size; j = j + 2) {
            int x1 = xlist[j];
            int x2 = xlist[j + 1];
            for(int x = x1; x < x2; x++) {
                Point p = Point{x , y};
                setPixel(p, fillColor);
            }
        }
        y++;
    }
}
    


void onResize(GLFWwindow *window, int width, int height) {
    screenWidth = width;
    screenHeight = height;
    clearBuffer();
    vertices.clear();
    inputMode = true;
    fillMode = false;
    postfiltrationMode = false;
    lines.clear();
}

void closingContour() {
    vertices.push_back(vertices[0]);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if ( action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ENTER:
                if (inputMode && vertices.size() > 2) {
                    inputMode = false;
                    closingContour();
                }
                break;
            case GLFW_KEY_BACKSPACE:
                clearBuffer();
                vertices.clear();
                inputMode = true;
                fillMode = false;
                postfiltrationMode = false;
                lines.clear();
                break;
            case GLFW_KEY_SPACE:
                if (!fillMode && !inputMode) {
                    fillMode = true;
                    clearBuffer();
                    makeContour();
                }
                break;
            case GLFW_KEY_F:
                postfiltrationMode = !postfiltrationMode;
                break;
        }
    }
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (inputMode) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            if (isScreenChanged) {
                vertices.push_back(Point{int(round(2*xpos)), int(round(float(screenHeight) - 2*ypos))});
            } else {
                vertices.push_back(Point{int(round(xpos)), int(round(float(screenHeight) - ypos))});
            }
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (fillMode) {
            fillContour();
        }
    }
}

void drawVerices() {
    if (vertices.size() == 1) {
        glBegin(GL_POINTS);
        glColor4f(fillColor.r, fillColor.g, fillColor.b, fillColor.alpha);
        Point point = vertices[0];
        std::cout << "Age: " << point.x << std::endl;
        std::cout << "Ae: " << point.y << std::endl;
        glVertex2f(float(point.x), float(point.y));
        glEnd();
    } else if (vertices.size() > 1) {
        glBegin(GL_LINE_STRIP);
        glColor4f(fillColor.r, fillColor.g, fillColor.b, fillColor.alpha);
        for (int i = 0; i < vertices.size(); i++) {
            Point point = vertices[i];
            glVertex2f(float(point.x), float(point.y));
        }
        glEnd();
    }
}

void makePostfiltration() {
    glDrawBuffer(GL_BACK);
    glClear(GL_ACCUM_BUFFER_BIT);
    int rasterMask[9][2]{{0, 2}, {1, 2}, {2, 2},
        {0, 1}, {1, 1}, {2, 1},
        {0, 0}, {1, 0}, {2, 0}};
    float intensityMask[9]{1, 2, 1,
        2, 4, 2,
        1, 2, 1};
    
    for (int i = 0; i < 9; i++) {
        glRasterPos2i(rasterMask[i][0], rasterMask[i][1]);
        glDrawPixels(screenWidth, screenHeight, GL_RGBA, GL_FLOAT, &buffer[0]);
        glAccum(GL_ACCUM, intensityMask[i] / 8);
    }
    
    glAccum(GL_RETURN, 1.0);
}

int main(int argc, const char * argv[]) {
    GLFWwindow *window;
    
    if (!glfwInit()) {
        return -1;
    }
    
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, 4);
    
    window = glfwCreateWindow(screenWidth, screenHeight, "Lab4", NULL, NULL);
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
    
    buffer = new Pixel[screenWidth * screenHeight];
    clearBuffer();
    
    while (!glfwWindowShouldClose(window)) {
        glRasterPos2i(0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        if (!fillMode) {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, screenWidth, 0, screenHeight, -screenWidth, screenWidth);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            drawVerices();
        } else {
            glDrawBuffer(GL_BACK);
            glDrawPixels(screenWidth, screenHeight, GL_RGBA, GL_FLOAT, &buffer[0]);
            if (postfiltrationMode) {
                makePostfiltration();
            }
        }
        
        glfwSwapBuffers(window);
        
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    return 0;
}
