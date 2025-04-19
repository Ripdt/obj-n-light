#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <tuple>
#include <numbers>
#include <fstream>
#include <string>

struct Vetor3D {
  double x, y, z;
};

typedef Vetor3D Vertice;

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

Poligono criar_cubo(double x, double y, double z, double tamanho);
Poligono load_obj(std::string fname);
void setup_drawing(Poligono& obj);
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

  setup_drawing(obj);

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

void setup_drawing(Poligono& obj) {
  obj.callList = glGenLists(1);
  glNewList(obj.callList, GL_COMPILE);
  {
    std::cout << "Vertices: " << obj.vertices.size() << std::endl;
    std::cout << "Faces: " << obj.faces.size() << std::endl;
    glPointSize(2.0);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);

    for (const Face& face : obj.faces) {
      for (const Aresta& aresta : face.arestas) {
        const Vertice& v1 = obj.vertices[aresta.v1];
        const Vertice& v2 = obj.vertices[aresta.v2];
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
      }
    }

    glEnd();
  }
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

  arquivo.close();
  return obj;
}