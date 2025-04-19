#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <tuple>
#include <numbers>
#include <fstream>
#include <string>
#include <math.h>

#define LARGURA 512
#define ALTURA 512

class Vector3D {
public:

  double x, y, z;
  Vector3D(double x = 0, double y = 0 , double z = 0) : x(x), y(y), z(z) {}
  Vector3D(const Vector3D& other) : x(other.x), y(other.y), z(other.z) {}

  Vector3D operator+(const Vector3D& other) const {
    return Vector3D(x + other.x, y + other.y, z + other.z);
  }
  Vector3D operator-(const Vector3D& other) const {
    return Vector3D(x - other.x, y - other.y, z - other.z);
  }
  Vector3D operator/(double scalar) const {
    return Vector3D(x / scalar, y / scalar, z / scalar);
  }
  void operator=(const Vector3D& other) {
    x = other.x;
    y = other.y;
    z = other.z;
  }
};

typedef Vector3D Vetor3D;
typedef Vetor3D Vertice;

class Linha {
public:
  Vetor3D v1, v2;
  Linha(const Vetor3D& _v1, const Vetor3D& _v2) : v1(_v1), v2(_v2) {}
};

struct Aresta {
  int v1, v2;
};

using ListaVertices = std::vector<Vertice>;
using ListaArestas = std::vector<Aresta>;

struct Face {
  int v1, v2, v3;
};

using ListaFaces = std::vector<Face>;

using Posicao = std::pair<int, int>;

struct Poligono {
  Vetor3D posicao = { 0, 0, 0 };
  Vetor3D rotacao = { 0 ,0 ,0 };
  Vetor3D escala  = { 1, 1, 1 };

  ListaVertices vertices;
  ListaFaces faces;
  int callList = -1;
};

struct Mouse {
  Posicao posicao = { -1, -1 };
  int botao = -1;
  float sense = .4;
};

Vetor3D pCamera ;

Poligono criar_cubo(double x, double y, double z, double tamanho);
Poligono carregar_obj(std::string fname);
void definir_desenho(Poligono& obj);
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
  glutInitWindowSize(LARGURA, ALTURA);

  glutInit(&argc, argv);
  glutCreateWindow("Wireframe");
  glClearColor(1.0, 1.0, 1.0, 1.0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-100, 100, -100, 100, -200, 200);
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_DEPTH_TEST);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  obj = criar_cubo(0, 0, 0, 50);
  //obj = load_obj("");

  definir_desenho(obj);

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
    { 0, 1, 2 },
    { 0, 2, 3 },
    { 4, 5, 6 },
    { 4, 6, 7 },
    { 0, 1, 5 },
    { 0, 5, 4 },
    { 1, 2, 6 },
    { 1, 6, 5 },
    { 2, 3, 7 },
    { 2, 7, 6 },
    { 3, 0, 4 },
    { 3, 4, 7 }
  };

  return novo_cubo;
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  desenhar(obj);

  glutSwapBuffers();
}

void redraw(int value) {
  glutPostRedisplay();
  glutTimerFunc(delay, redraw, 0);
}

void desenhar(Poligono& obj) {

  glPushMatrix();
  glTranslatef(obj.posicao.x, obj.posicao.y, obj.posicao.z);
  glRotatef(obj.rotacao.x, 1, 0, 0);
  glRotatef(obj.rotacao.y, 0, 1, 0);
  glRotatef(obj.rotacao.z, 0, 0, 1);
  glScalef(obj.escala.x, obj.escala.y, obj.escala.z);
  glCallList(obj.callList);
  glPopMatrix();
}

void movimentar(Poligono& cubo, double dx, double dy, double dz) {
  cubo.posicao.x += dx;
  cubo.posicao.y += dy;
  cubo.posicao.z += dz;
}

void rotacionar(Poligono& cubo, double anguloX, double anguloY, double anguloZ) {
  cubo.rotacao.x += anguloX * 180 / std::numbers::pi;
  cubo.rotacao.y += anguloY * 180 / std::numbers::pi;
  cubo.rotacao.z += anguloZ * 180 / std::numbers::pi;
}

void escalar(Poligono& cubo, double sx, double sy, double sz) {
  cubo.escala.x *= sx;
  cubo.escala.y *= sy;
  cubo.escala.z *= sz;
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case '+': escalar(obj, 1.1, 1.1, 1.1); break;
  case '-': escalar(obj, 0.9, 0.9, 0.9); break;
  case 'x': escalar(obj, 1.1, 1, 1); break;
  case 'y': escalar(obj, 1, 1.1, 1); break;
  case 'z': escalar(obj, 1, 1, 1.1); break;
    // SHIFT
  case 'X': escalar(obj, .9, 1, 1); break;
  case 'Y': escalar(obj, 1, .9, 1); break;
  case 'Z': escalar(obj, 1, 1, .9); break;
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

void definir_desenho(Poligono& obj) {
  obj.callList = glGenLists(1);
  glNewList(obj.callList, GL_COMPILE);
  {
    glColor3d(0, 1, 0);
    glBegin(GL_TRIANGLES);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    for (const Face& face : obj.faces) {

      const Vertice& a = obj.vertices[face.v1];
      const Vertice& b = obj.vertices[face.v2];
      const Vertice& c = obj.vertices[face.v3];

      Vector3D ab = b - a;
      Vector3D ac = c - a;

      Vector3D normal(
        ab.y * ac.z - ab.z * ac.y,
        ab.z * ac.x - ab.x * ac.z,
        ab.x * ac.y - ab.y * ac.x
      );

      double comprimento = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
      Vector3D normalUnit = normal / comprimento;

      glNormal3f(normalUnit.x, normalUnit.y, normalUnit.z);

      glVertex3f(a.x, a.y, a.z);
      glVertex3f(b.x, b.y, b.z);
      glVertex3f(c.x, c.y, c.z);
    }

    glEnd();
  }
  glEndList();
}

Poligono carregar_obj(std::string fname)
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
        Face face = {fp, fs, ft};
        obj.faces.push_back(face);
      }
    }
  }

  arquivo.close();
  return obj;
}