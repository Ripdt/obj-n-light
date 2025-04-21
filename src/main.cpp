#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <tuple>
#include <numbers>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>

#define LARGURA 512
#define ALTURA 512

class Vector3D {
public:
  float x, y, z;
  Vector3D(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
  Vector3D(const Vector3D& other) : x(other.x), y(other.y), z(other.z) {}

  Vector3D operator+(const Vector3D& other) const {
    return Vector3D(x + other.x, y + other.y, z + other.z);
  }
  Vector3D operator-(const Vector3D& other) const {
    return Vector3D(x - other.x, y - other.y, z - other.z);
  }
  Vector3D operator/(const float scalar) const {
    return Vector3D(x / scalar, y / scalar, z / scalar);
  }
  void operator=(const Vector3D& other) {
    x = other.x;
    y = other.y;
    z = other.z;
  }
};

struct Vector2D {
  float x, y;
};

struct VerticeFace {
  int indiceV = -1, indiceVN = -1, indiceVT = -1;
};

struct Face {
  std::vector<VerticeFace> v;
};

using ListaFaces = std::vector<Face>;

struct Poligono {
  Vector3D posicao = { 0, 0, 0 };
  Vector3D rotacao = { 0 ,0 ,0 };
  Vector3D escala  = { 1, 1, 1 };

  std::vector<Vector3D> vertices, normais;
  std::vector<Vector2D> texturas;
  ListaFaces faces;

  int callList = -1;
};

using Posicao = std::pair<int, int>;

struct Mouse {
  Posicao posicao = { -1, -1 };
  int botao = -1;
  float sense = .4f;
};

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
  glClearColor(1.f, 1.f, 1.f, 1.f);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-100, 100, -100, 100, -200, 200);
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_DEPTH_TEST);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  obj = carregar_obj("../res/cubo.obj");
  definir_desenho(obj);

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMotionFunc(mouse_move);
  glutMouseFunc(mouse_click);

  glutTimerFunc(delay, redraw, 0);

  glutMainLoop();
  return 0;
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

void movimentar(Poligono& cubo, float dx, float dy, float dz) {
  cubo.posicao.x += dx;
  cubo.posicao.y += dy;
  cubo.posicao.z += dz;
}

void rotacionar(Poligono& cubo, float anguloX, float anguloY, float anguloZ) {
  cubo.rotacao.x += anguloX * 180.f / (float)std::numbers::pi;
  cubo.rotacao.y += anguloY * 180.f / (float)std::numbers::pi;
  cubo.rotacao.z += anguloZ * 180.f / (float)std::numbers::pi;
}

void escalar(Poligono& cubo, float sx, float sy, float sz) {
  cubo.escala.x *= sx;
  cubo.escala.y *= sy;
  cubo.escala.z *= sz;
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case '+': escalar(obj, 1.1f, 1.1f, 1.1f); break;
  case '-': escalar(obj, 0.9f, 0.9f, 0.9f); break;
  case 'x': escalar(obj, 1.1f, 1.f, 1.f); break;
  case 'y': escalar(obj, 1.f, 1.1f, 1.f); break;
  case 'z': escalar(obj, 1.f, 1.f, 1.1f); break;
    // SHIFT
  case 'X': escalar(obj, .9f, 1.f, 1.f); break;
  case 'Y': escalar(obj, 1.f, .9f, 1.f); break;
  case 'Z': escalar(obj, 1.f, 1.f, .9f); break;
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
    const float offsetX = (x - mouse.posicao.first) * mouse.sense;
    const float offsetY = (mouse.posicao.second - y) * mouse.sense;

    movimentar(obj, offsetX, offsetY, 0);
  }
  else if (mouse.botao == GLUT_RIGHT_BUTTON) {
    const int offsetX = x - mouse.posicao.first;
    const int offsetY = y - mouse.posicao.second;
    const float grauX = offsetX / 180.f * -1;
    const float grauY = offsetY / 180.f * -1;

    rotacionar(obj, grauY, grauX, 0);
  }

  mouse.posicao = { x, y };
}

Vector3D normal_triangulo(const Face& face, size_t idx) {
  size_t idxA = idx, idxB = -1, idxC = -1;
  switch (idx) {
  case 0:
    idxB = 1; idxC = 2;
    break;
  case 1:
    idxB = 0; idxC = 2;
    break;
  case 2:
    idxB = 1; idxC = 0;
    break;
  }

  const Vector3D& a = obj.vertices[face.v[idxA].indiceV];
  const Vector3D& b = obj.vertices[face.v[idxB].indiceV];
  const Vector3D& c = obj.vertices[face.v[idxC].indiceV];

  const Vector3D ab = b - a;
  const Vector3D ac = c - a;

  const Vector3D normal = {
    ab.y * ac.z - ab.z * ac.y,
    ab.z * ac.x - ab.x * ac.z,
    ab.x * ac.y - ab.y * ac.x
  };

  float comprimento = std::sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
  return normal / comprimento;
}

void desenhar_triangulo(const Face& face) {
  glBegin(GL_TRIANGLES);

  for (size_t i = 0; i < face.v.size(); i++) {
    const VerticeFace& vf = face.v[i];
    if (vf.indiceVN > -1) {
      const Vector3D& normalOBJ = obj.normais[vf.indiceVN];
      glNormal3f(normalOBJ.x, normalOBJ.y, normalOBJ.z);
    }
    else {
      const Vector3D normal = normal_triangulo(face, i);
      glNormal3f(normal.x, normal.y, normal.z);
    }

    if (vf.indiceVT > -1) {
      const Vector2D& textura = obj.texturas[vf.indiceVT];
      glTexCoord2f(textura.x, textura.y);
    }
    
    const Vector3D& vertice = obj.vertices[vf.indiceV];
    glVertex3f(vertice.x, vertice.y, vertice.z);
  }
  glEnd();
}

void desenhar_quadrado(const Face& face) {
  glBegin(GL_QUADS);
  for (const VerticeFace& vf : face.v) {
    if (vf.indiceVN > -1) {
      const Vector3D& normal = obj.normais[vf.indiceVN];
      glNormal3f(normal.x, normal.y, normal.z);
    }

    if (vf.indiceVT > -1) {
      const Vector2D& textura = obj.texturas[vf.indiceVT];
      glTexCoord2f(textura.x, textura.y);
    }

    const Vector3D& vertice = obj.vertices[vf.indiceV];
    glVertex3f(vertice.x, vertice.y, vertice.z);
  }
  glEnd();
}

void desenhar_poligono(const Face& face) {
  glBegin(GL_POLYGON);
  for (const VerticeFace& vf : face.v) {
    if (vf.indiceVN > -1) {
      const Vector3D& normal = obj.normais[vf.indiceVN];
      glNormal3f(normal.x, normal.y, normal.z);
    }

    if (vf.indiceVT > -1) {
      const Vector2D& textura = obj.texturas[vf.indiceVT];
      glTexCoord2f(textura.x, textura.y);
    }

    const Vector3D& vertice = obj.vertices[vf.indiceV];
    glVertex3f(vertice.x, vertice.y, vertice.z);
  }
  glEnd();
}

void definir_desenho(Poligono& obj) {
  obj.callList = glGenLists(1);
  glNewList(obj.callList, GL_COMPILE);
  {
    glColor3d(0, 1, 0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    for (const Face& face : obj.faces) {
      if (face.v.size() == 3)
        desenhar_triangulo(face);
      else if (face.v.size() == 4)
        desenhar_quadrado(face);
      else
        desenhar_poligono(face);
    }
  }
  glEndList();
}

Poligono carregar_obj(std::string fname)
{
  std::ifstream arquivo(fname);
  Poligono obj;
  if (!arquivo.is_open()) {
    std::cout << "arquivo nao encontrado";
    exit(1);
  }
  else {
    std::string linhaArquivo;
    while (std::getline(arquivo, linhaArquivo))
    {
      std::istringstream linha(linhaArquivo);
      std::string tipo;
      linha >> tipo;
      if (tipo == "v") {
        float x, y, z;
        linha >> x >> y >> z;
        Vector3D vertice(x, y, z);
        obj.vertices.push_back(vertice);
      }
      else if (tipo == "vn") {
        float nx, ny, nz;
        linha >> nx >> ny >> nz;
        Vector3D normal(nx, ny, nz);
        obj.normais.push_back(normal);
      }
      else if (tipo == "vt") {
        float u, v;
        linha >> u >> v;
        Vector2D textura(u, v);
        obj.texturas.push_back(textura);
      }
      else if (tipo == "f") {
        Face face;
        std::string vert;
        int vIdx = 0;
        while (linha >> vert) {
          std::istringstream vss(vert);
          std::string idx;
          VerticeFace fv;

          std::getline(vss, idx, '/'); 
          fv.indiceV = std::stoi(idx) - 1;

          if (std::getline(vss, idx, '/') && idx != "")
            fv.indiceVT = std::stoi(idx) - 1;

          if (std::getline(vss, idx, '/') && idx != "")
            fv.indiceVN = std::stoi(idx) - 1;

          face.v.push_back(fv);
        }
        obj.faces.push_back(face);
      }
    }
  }

  const int totalVertices = static_cast<int>(obj.vertices.size());
  const int totalNormais = static_cast<int>(obj.normais.size());
  const int totalTexturas = static_cast<int>(obj.texturas.size());
  for (Face& face : obj.faces) {
    for (VerticeFace& vf : face.v) {
      vf.indiceV = vf.indiceV < 0 ? totalVertices + vf.indiceV : vf.indiceV;
      vf.indiceVN = vf.indiceVN < 0 ? totalNormais + vf.indiceVN : vf.indiceVN;
      vf.indiceVT = vf.indiceVT < 0 ? totalTexturas + vf.indiceVT : vf.indiceVT;
    }
  }

  arquivo.close();
  return obj;
}