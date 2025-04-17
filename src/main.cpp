#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <tuple>
#include <numbers>
#include <fstream>
#include <string>

struct Vertice {
  double x, y, z;
};

struct Aresta {
  int v1, v2;
};

using ListaVertices = std::vector<Vertice>;
using ListaArestas = std::vector<Aresta>;

struct Face {
  ListaArestas arestas;
};

using ListaFaces = std::vector<Face>;

using Posicao = std::pair<int, int>;

struct Poligono {
  Vertice posicao;
  ListaVertices vertices;
  ListaFaces faces;
  float rotacao = 0;
  int callList = -1;
};

struct Mouse {
  Posicao posicao = { -1, -1 };
  int botao = -1;
  float sense = .4;
};

Poligono criar_cubo(double x, double y, double z, double tamanho);
Poligono load_obj(std::string fname);
void desenhar(Poligono& cubo);
void display();
void keyboard(unsigned char key, int x, int y);
void mouse_move(int x, int y);
void mouse_click(int button, int state, int x, int y);
void redraw(int value);

Poligono obj;
int delay = 10;
Mouse mouse;

int main(int argc, char** argv) {
  glutInitWindowSize(512, 512);

  //obj = criar_cubo(0, 0, 0, 50);
  obj = load_obj("..\\..\\res\\elepham.obj");

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutCreateWindow("Wireframe");
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glOrtho(-100, 100, -100, 100, -100, 100);

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMotionFunc(mouse_move);
  glutMouseFunc(mouse_click);

  glutTimerFunc(delay, redraw, 0);

  glutMainLoop();
  return 0;
}

Poligono criar_cubo(double x, double y, double z, double tamanho) {
  Poligono novo_cubo;
  novo_cubo.posicao = { x, y, z };

  double t = tamanho / 2.0;
  novo_cubo.vertices = {
      {-t, -t, -t}, { t, -t, -t}, { t,  t, -t}, {-t,  t, -t},
      {-t, -t,  t}, { t, -t,  t}, { t,  t,  t}, {-t,  t,  t}
  };

  ListaArestas face1 = { {0, 1}, {1, 2}, {2, 3}, {3, 0} };
  ListaArestas face2 = { {4, 5}, {5, 6}, {6, 7}, {7, 4} };
  ListaArestas face3 = { {0, 4}, {1, 5}, {2, 6}, {3, 7} };

  novo_cubo.faces = {
    { face1 },
    { face2 },
    { face3 }
  };

  return novo_cubo;
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  desenhar(obj);
  glFlush();
}

void redraw(int value) {
  glutPostRedisplay();
  glutTimerFunc(delay, redraw, 0);
}

void desenhar(Poligono& obj) {
  glPushMatrix();
  //glTranslatef(0, -40.00, -105);
  //glScalef(0.4, 0.4, 0.4);
  //glRotatef(obj.rotacao, 0, 1, 0);
  glCallList(obj.callList);
  glPopMatrix();
}

void movimentar(Poligono& cubo, double dx, double dy, double dz) {
  cubo.posicao.x += dx;
  cubo.posicao.y += dy;
  cubo.posicao.z += dz;
  for (auto& v : cubo.vertices) {
    v.x += dx;
    v.y += dy;
    v.z += dz;
  }
}

void escalar(Poligono& cubo, double sx, double sy, double sz) {
  for (auto& v : cubo.vertices) {
    v.x *= sx;
    v.y *= sy;
    v.z *= sz;
  }
}

void rotacionar(Poligono& cubo, double anguloX, double anguloY, double anguloZ) {
  const double radX = anguloX * std::numbers::pi / 180.0;
  const double radY = anguloY * std::numbers::pi / 180.0;
  const double radZ = anguloZ * std::numbers::pi / 180.0;

  const double centerX = cubo.posicao.x;
  const double centerY = cubo.posicao.y;
  const double centerZ = cubo.posicao.z;

  for (Vertice& vertice : cubo.vertices) {
    double x = vertice.x - centerX;
    double y = vertice.y - centerY;
    double z = vertice.z - centerZ;

    // Rotação em X
    double novoY = y * cos(anguloX) - z * sin(anguloX);
    double novoZ = y * sin(anguloX) + z * cos(anguloX);
    y = novoY;
    z = novoZ;

    // Rotação em Y
    double novoX = x * cos(anguloY) + z * sin(anguloY);
    z = -x * sin(anguloY) + z * cos(anguloY);
    x = novoX;

    // Rotação em Z
    double novoX2 = x * cos(anguloZ) - y * sin(anguloZ);
    double novoY2 = x * sin(anguloZ) + y * cos(anguloZ);
    x = novoX2;
    y = novoY2;

    // Translação de volta (somando as coordenadas do centro)
    x += centerX;
    y += centerY;
    z += centerZ;

    // Atualiza as coordenadas
    vertice.x = x;
    vertice.y = y;
    vertice.z = z;
  }
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case '+': escalar(obj, 1.1, 1.1, 1.1); break;
  case '-': escalar(obj, 0.9, 0.9, 0.9); break;
  case 'r': rotacionar(obj, 0, 0, .1); break;
  case ' ': exit(0); break;
  }
}

void mouse_click(int button, int state, int x, int y) {
  mouse.botao = button;
  mouse.posicao = { x, y };
}

void mouse_move(int x, int y) {
  if (mouse.posicao.first < 0 || mouse.posicao.second < 0)
    mouse.posicao = { x, y };

  if (mouse.botao == GLUT_LEFT_BUTTON) {
    const double offsetX = (x - mouse.posicao.first) * mouse.sense;
    const double offsetY = (mouse.posicao.second - y) * mouse.sense;

    movimentar(obj, offsetX, offsetY, 0);
  }
  else if (mouse.botao == GLUT_RIGHT_BUTTON) {
    const double offsetX = x - mouse.posicao.first;
    const double offsetY = y - mouse.posicao.second;
    const double grauX = offsetX / 180 * -1;
    const double grauY = offsetY / 180 * -1;

    rotacionar(obj, grauY, grauX, 0);
  }

  mouse.posicao = { x, y };
}

void setup_drawing(Poligono& obj) {
  obj.callList = glGenLists(1);
  glPointSize(2.0);
  glNewList(obj.callList, GL_COMPILE);
  {
    glPushMatrix();
    glBegin(GL_LINES);

    for (const Face& face : obj.faces)
    {
      for (const Aresta& Aresta : face.arestas) {
        const Vertice& v1 = obj.vertices[Aresta.v1];
        const Vertice& v2 = obj.vertices[Aresta.v2];
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
      }
    }
    glEnd();

  }
  glPopMatrix();
  glEndList();
}

Poligono load_obj(std::string fname)
{
  int read;
  float x, y, z;
  std::ifstream arquivo(fname);
  Poligono obj;
  if (!arquivo.is_open()) {
    std::cout << "arquivo nao encontrado";
    exit(1);
  }
  else {
    std::string tipo;
    while (arquivo >> tipo)
    {

      if (tipo == "v")
      {
        float x, y, z;
        arquivo >> x >> y >> z;
        Vertice vertice = { x, y, z };
        obj.vertices.push_back(vertice);
      }

      if (tipo == "f")
      {
        std::string x, y, z;
        arquivo >> x >> y >> z;
        int fp = stoi(x.substr(0, x.find("/"))) - 1;
        int fs = stoi(y.substr(0, y.find("/"))) - 1;
        int ft = stoi(z.substr(0, z.find("/"))) - 1;
        Face face;
        face.arestas = { {fp, fs}, {fs, ft}, {ft, fp} };
        obj.faces.push_back(face);
      }
    }
  }

  setup_drawing(obj);

  arquivo.close();
  return obj;
}