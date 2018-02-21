#include "helper.h"

static GLFWwindow* win = NULL;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
// GLuint idMVPMatrix;
GLuint mvMatrixLoc;
GLuint mvpMatrixLoc;
GLuint cameraPositionLoc;
GLuint heightFactorLoc;
GLuint widthTextureLoc;
GLuint heightTextureLoc;

void sendToShader();
void createModelViewMatrix();
void toggleFullscreen(GLFWwindow*);

int windowed_xpos, windowed_ypos, windowed_width, windowed_height;

int widthTexture, heightTexture;
float heightFactor = 10.0f;
float speed = 0.0f;

Camera camera;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 mvp;
glm::mat4 mv;
glm::mat4 cameraMatrix;
glm::mat4 projectionMatrix;

static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch(key) {
      case(GLFW_KEY_F):
        toggleFullscreen(window);
        break;
      case(GLFW_KEY_U):
        speed += 0.1;
        break;
      case(GLFW_KEY_J):
        speed -= 0.1;
        break;
    }
  }

  switch(key) {
    case(GLFW_KEY_ESCAPE):
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;
    case(GLFW_KEY_O):
      heightFactor += 0.5;
      break;
    case(GLFW_KEY_L):
      heightFactor -= 0.5;
      break;
    case(GLFW_KEY_A):
      camera.rotateAroundUp(1);
      break;
    case(GLFW_KEY_D):
      camera.rotateAroundUp(-1);
      break;
    case(GLFW_KEY_W):
      camera.rotateAroundRight(1);
      break;
    case(GLFW_KEY_S):
      camera.rotateAroundRight(-1);
      break;
  }

  sendToShader();

}

void toggleFullscreen(GLFWwindow* window) {
  if (glfwGetWindowMonitor(window)) {
    glfwSetWindowMonitor(window, NULL,
     windowed_xpos, windowed_ypos,
     windowed_width, windowed_height, 0);
  }
  else {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor) {
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);
      glfwGetWindowPos(window, &windowed_xpos, &windowed_ypos);
      glfwGetWindowSize(window, &windowed_width, &windowed_height);
      glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
  }
}

void cameraInit() {
  camera.position = glm::vec3(
    widthTexture / 2,
    widthTexture / 10,
    (-1 * widthTexture) / 4
  );
  camera.gaze = glm::vec3(0.0, 0.0, 1.0);
  camera.up = glm::vec3(0.0, 1.0, 0.0);

}

void createModelViewMatrix() {
  cameraMatrix = glm::lookAt(
    camera.position,
    camera.position + camera.gaze,
    camera.up
  );

  projectionMatrix = glm::perspective(45.0f, 1.0f, 0.1f, 1000.0f);
  mvp = projectionMatrix * cameraMatrix * model;
  mv = cameraMatrix * model;
}

void sendToShader() {
  createModelViewMatrix();
  glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvp));
  glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mv));
  glUniform1f(heightFactorLoc, heightFactor);
  glUniform3fv(cameraPositionLoc, 1, glm::value_ptr(camera.position));
}

void draw()
{
  static bool firstTime = true;

  static int vertexPosDataSizeInBytes;
  static int vertexColDataSizeInBytes;
  static int numOfFaces;

  if (firstTime)
  {
    firstTime = false;

    glEnableClientState(GL_VERTEX_ARRAY);

    numOfFaces = 2 * widthTexture * heightTexture;
    GLuint* indices = new GLuint[numOfFaces * 3];
    int i = 0;
    for (int h = 0; h < heightTexture; h++) {
      for (int w = 0; w < widthTexture; w++, i += 2) {
        int tempWidth = h * (widthTexture+1) + w;

        indices[3 * i] = tempWidth;
        indices[3 * i + 1 ] = tempWidth + widthTexture + 1;
        indices[3 * i + 2 ] = tempWidth + 1;

        indices[3 * (i + 1)] = tempWidth + 1;
        indices[3 * (i + 1) + 1] = tempWidth + widthTexture + 1;
        indices[3 * (i + 1) + 2] = tempWidth + widthTexture + 2;

      }
    }

    int numOfVertices = (widthTexture+1)*(heightTexture+1);
    GLfloat* vertexPos = new GLfloat[numOfVertices * 3];
    int vertexNum = 0;
    for (int h = 0; h <= heightTexture; h++) {
      for (int w = 0; w <= widthTexture; w++) {
        vertexPos[3 * vertexNum] = w;
        vertexPos[3 * vertexNum + 1] = 0;
        vertexPos[3 * vertexNum + 2] = h;

        vertexNum++;
      }
    }

    GLuint vertexAttribBuffer, indexBuffer;

    glGenBuffers(1, &vertexAttribBuffer);
    glGenBuffers(1, &indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    vertexPosDataSizeInBytes =  numOfVertices * 3 * sizeof(GLfloat);
    int indexDataSizeInBytes = numOfFaces * 3 * sizeof(GLuint);

    glBufferData(GL_ARRAY_BUFFER, vertexPosDataSizeInBytes, vertexPos, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indices, GL_STATIC_DRAW);

  }
  glVertexPointer(3, GL_FLOAT, 0, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDrawElements(GL_TRIANGLES, numOfFaces * 3, GL_UNSIGNED_INT, 0);
}

void render() {

  glClearColor(0, 0, 0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  draw();
}

void setLocations() {
  mvMatrixLoc = glGetUniformLocation(idProgramShader, "MV");
  mvpMatrixLoc = glGetUniformLocation(idProgramShader, "MVP");
  cameraPositionLoc = glGetUniformLocation(idProgramShader, "cameraPosition");
  heightFactorLoc = glGetUniformLocation(idProgramShader, "heightFactor");
  widthTextureLoc = glGetUniformLocation(idProgramShader, "widthTexture");
  heightTextureLoc = glGetUniformLocation(idProgramShader, "heightTexture");
}

void init(char* textureFile) {
  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  win = glfwCreateWindow(1500, 1500, "OpenGL Heightmap", NULL, NULL);

  if (!win) {
      glfwTerminate();
      exit(-1);
  }
  glfwMakeContextCurrent(win);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

      glfwTerminate();
      exit(-1);
  }

  glfwSetKeyCallback(win, keyCallback);

  glfwSetWindowSizeCallback(win, windowSizeCallback);

  initShaders();
  glUseProgram(idProgramShader);
  initTexture(textureFile, &widthTexture, &heightTexture);

  setLocations();

  glUniform1i(widthTextureLoc, widthTexture);
  glUniform1i(heightTextureLoc, heightTexture);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glShadeModel(GL_SMOOTH);

  cameraInit();
  sendToShader();
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Please provide only a texture image\n");
    exit(-1);
  }

  init(argv[1]);

  while(!glfwWindowShouldClose(win)) {
    render();
    glfwSwapBuffers(win);
    glfwPollEvents();

    camera.position = camera.position + camera.gaze * speed;
    sendToShader();
  }


  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}
