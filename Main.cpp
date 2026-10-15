#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>

const unsigned int MAIN_WIDTH = 800;
const unsigned int MAIN_HEIGHT = 600;
const unsigned int WIN2_WIDTH = 400;
const unsigned int WIN2_HEIGHT = 400;

bool isAnimating = true;
float shapeColor[3] = { 0.0f, 1.0f, 0.0f };
float squareColorTint[3] = { 1.0f, 1.0f, 1.0f };

bool menuOpen = false;
double menuClickX = 0, menuClickY = 0;

struct ClickedCircle {
    float x, y;
    float r, g, b;
    float spawnTime;
};
std::vector<ClickedCircle> extraCircles;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_R: shapeColor[0] = 1.0f; shapeColor[1] = 0.0f; shapeColor[2] = 0.0f; break;
        case GLFW_KEY_G: shapeColor[0] = 0.0f; shapeColor[1] = 1.0f; shapeColor[2] = 0.0f; break;
        case GLFW_KEY_B: shapeColor[0] = 0.0f; shapeColor[1] = 0.0f; shapeColor[2] = 1.0f; break;
        case GLFW_KEY_Y: shapeColor[0] = 1.0f; shapeColor[1] = 1.0f; shapeColor[2] = 0.0f; break;
        case GLFW_KEY_O: shapeColor[0] = 1.0f; shapeColor[1] = 0.5f; shapeColor[2] = 0.0f; break;
        case GLFW_KEY_P: shapeColor[0] = 0.5f; shapeColor[1] = 0.0f; shapeColor[2] = 0.5f; break;
        case GLFW_KEY_W: shapeColor[0] = 1.0f; shapeColor[1] = 1.0f; shapeColor[2] = 1.0f; break;
        }
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            menuOpen = true;
            menuClickX = xpos;
            menuClickY = ypos;
        }
        else if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (menuOpen) {
                double relX = xpos - menuClickX;
                double relY = ypos - menuClickY;

                if (relX >= 0 && relX <= 150 && relY >= 0 && relY <= 120) {
                    if (relY < 30) { isAnimating = false; }
                    else if (relY < 60) { isAnimating = true; }
                    else if (relY < 80) {
                        squareColorTint[0] = 1.0f; squareColorTint[1] = 1.0f; squareColorTint[2] = 1.0f;
                    }
                    else if (relY < 100) {
                        squareColorTint[0] = 1.0f; squareColorTint[1] = 0.0f; squareColorTint[2] = 0.0f;
                    }
                    else {
                        squareColorTint[0] = 0.0f; squareColorTint[1] = 1.0f; squareColorTint[2] = 0.0f;
                    }
                }
                menuOpen = false;
            }
            else {
                float glX = (float)(xpos / MAIN_WIDTH) * 2.0f - 1.0f;
                float glY = 1.0f - (float)(ypos / MAIN_HEIGHT) * 2.0f;

                ClickedCircle newCircle;
                newCircle.x = glX;
                newCircle.y = glY;
                newCircle.r = (float)rand() / RAND_MAX;
                newCircle.g = (float)rand() / RAND_MAX;
                newCircle.b = (float)rand() / RAND_MAX;
                newCircle.spawnTime = (float)glfwGetTime();

                extraCircles.push_back(newCircle);
            }
        }
    }
}

void drawSquare(float angle) {
    glPushMatrix();
    glTranslatef(-0.25f, 0.0f, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.25f, 0.0f, 0.0f);

    glBegin(GL_QUADS);
    glColor3f(squareColorTint[0], squareColorTint[1], squareColorTint[2]);
    glVertex2f(-0.5f, 0.5f);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(0.0f, -0.5f);
    glVertex2f(-0.5f, -0.5f);

    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.0f, -0.5f);
    glEnd();
    glPopMatrix();
}

void drawEllipse() {
    glColor3f(0.8f, 0.2f, 0.2f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.14159f / 180.0f;
        glVertex2f(cos(theta) * 0.8f, sin(theta) * 0.4f);
    }
    glEnd();
}

void drawShapesWin2(float triangleAngle, float circleScale) {
    glColor3f(shapeColor[0], shapeColor[1], shapeColor[2]);

    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.0f);
    glRotatef(triangleAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, -0.05f, 0.0f);

    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f, -0.4f);
    glVertex2f(0.5f, -0.4f);
    glVertex2f(0.0f, 0.5f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.6f, 0.0f);
    glScalef(circleScale, circleScale, 1.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.14159f / 180.0f;
        glVertex2f(cos(theta) * 0.3f, sin(theta) * 0.3f);
    }
    glEnd();
    glPopMatrix();
}

int main() {
    srand((unsigned int)time(NULL));

    if (!glfwInit()) return -1;

    GLFWwindow* mainWindow = glfwCreateWindow(MAIN_WIDTH, MAIN_HEIGHT, "Main Window", NULL, NULL);
    if (!mainWindow) {
        glfwTerminate();
        return -1;
    }

    GLFWwindow* window2 = glfwCreateWindow(WIN2_WIDTH, WIN2_HEIGHT, "window 2", NULL, NULL);
    if (!window2) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(mainWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    glfwSetKeyCallback(window2, keyCallback);
    glfwSetMouseButtonCallback(mainWindow, mouseButtonCallback);

    float accumulatedAngle = 0.0f;
    float lastTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(mainWindow) && !glfwWindowShouldClose(window2)) {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (isAnimating) {
            accumulatedAngle += deltaTime * 50.0f;
        }

        float squareAngle = accumulatedAngle;
        float triangleAngle = -accumulatedAngle;
        float circleScale = 1.0f + 0.3f * sin(currentTime * 3.0f);

        glfwMakeContextCurrent(mainWindow);

        glViewport(0, 0, MAIN_WIDTH, MAIN_HEIGHT);
        glEnable(GL_SCISSOR_TEST);
        glScissor(0, 0, MAIN_WIDTH, MAIN_HEIGHT);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawSquare(squareAngle);

        for (const auto& c : extraCircles) {
            float cScale = 1.0f + 0.2f * sin((currentTime - c.spawnTime) * 3.0f);
            glPushMatrix();
            glTranslatef(c.x, c.y, 0.0f);
            glScalef(cScale, cScale, 1.0f);
            glColor3f(c.r, c.g, c.b);
            glBegin(GL_POLYGON);
            for (int i = 0; i < 360; i++) {
                float theta = i * 3.14159f / 180.0f;
                glVertex2f(cos(theta) * 0.15f, sin(theta) * 0.15f);
            }
            glEnd();
            glPopMatrix();
        }

        int subX = 50, subY = MAIN_HEIGHT - 250;
        int subW = 300, subH = 200;
        glViewport(subX, subY, subW, subH);
        glScissor(subX, subY, subW, subH);
        glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawEllipse();

        glDisable(GL_SCISSOR_TEST);

        if (menuOpen) {
            glViewport((int)menuClickX, MAIN_HEIGHT - (int)menuClickY - 120, 150, 120);
            glScissor((int)menuClickX, MAIN_HEIGHT - (int)menuClickY - 120, 150, 120);
            glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        glfwSwapBuffers(mainWindow);

        glfwMakeContextCurrent(window2);
        glViewport(0, 0, WIN2_WIDTH, WIN2_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawShapesWin2(triangleAngle, circleScale);

        glfwSwapBuffers(window2);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}