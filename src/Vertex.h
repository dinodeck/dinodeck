#ifndef VERTEX_H
#define VERTEX_H

#include "Vector.h"

struct Vertex
{
    float x, y, z;
    float r, b, g, a;
    float u, v;

    Vertex()
        : x(0), y(0), z(0),
          r(1), b(1), g(1), a(1),
          u(0), v(0) {}

    Vertex(float x, float y, float z)
        : x(x), y(y), z(z),
          r(1), b(1), g(1), a(1),
          u(0), v(0) {}

    Vertex(float x, float y, float z,
           float r, float g, float b, float a)
        : x(x), y(y), z(z),
          r(r), b(b), g(g), a(a),
          u(0), v(0) {}

    Vertex(float x, float y, float z,
           float r, float g, float b, float a,
           float u, float v)
        : x(x), y(y), z(z),
          r(r), b(b), g(g), a(a),
          u(u), v(v) {}

    Vertex(const Vector& pos)
        : x(pos.x), y(pos.y), z(pos.z),
          r(1), b(1), g(1), a(1),
          u(0), v(0) {}

    Vertex(const Vector& pos,
           const Vector& col)
        : x(pos.x), y(pos.y), z(pos.z),
          r(col.x), b(col.y), g(col.z), a(col.w),
          u(0), v(0) {}

    Vertex(const Vector& pos,
           const Vector& col,
           float u, float v)
        : x(pos.x), y(pos.y), z(pos.z),
          r(col.x), b(col.y), g(col.z), a(col.w),
          u(u), v(v) {}

    Vertex(const Vector& pos,
           float u, float v)
        : x(pos.x), y(pos.y), z(pos.z),
          r(1), b(1), g(1), a(1),
          u(u), v(v) {}

    Vertex(float x, float y, float z,
           const Vector& col)
        : x(x), y(y), z(z),
          r(col.x), b(col.y), g(col.z), a(col.w),
          u(0), v(0) {}

    Vertex(float x, float y, float z,
           const Vector& col,
           float u, float v)
        : x(x), y(y), z(z),
          r(col.x), b(col.y), g(col.z), a(col.w),
          u(u), v(v) {}

      void Set(const Vector& pos,
               const Vector& col,
               float u_, float v_)
      {
          x = pos.x;
          y = pos.y;
          z = pos.z;

          r = col.x;
          g = col.y;
          b = col.z;
          a = col.w;

          u = u_;
          v = v_;
      }
};
#endif
