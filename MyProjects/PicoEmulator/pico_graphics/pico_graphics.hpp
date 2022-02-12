#pragma once

#include <string>
#include <cstdint>
#include <algorithm>
#include <vector>
#include "font6_data.hpp"

// a tiny little graphics library for our Pico products
// supports only 16-bit (565) RGB framebuffers
namespace pimoroni {

  typedef uint16_t Pen;

  struct Rect;

  struct Point {
    int32_t x = 0, y = 0;

    Point() = default;
    Point(int32_t x, int32_t y) : x(x), y(y) {}

    inline Point& operator-= (const Point &a) { x -= a.x; y -= a.y; return *this; }
    inline Point& operator+= (const Point &a) { x += a.x; y += a.y; return *this; }

    Point clamp(const Rect &r) const;
  };

  struct Rect {
    int32_t x = 0, y = 0, w = 0, h = 0;

    Rect() = default;
    Rect(int32_t x, int32_t y, int32_t w, int32_t h) : x(x), y(y), w(w), h(h) {}
    Rect(const Point &tl, const Point &br) : x(tl.x), y(tl.y), w(br.x - tl.x), h(br.y - tl.y) {}

    bool empty() const;
    bool contains(const Point &p) const;
    bool contains(const Rect &p) const;
    bool intersects(const Rect &r) const;
    Rect intersection(const Rect &r) const;

    void inflate(int32_t v);
    void deflate(int32_t v);
  };

  class PicoGraphics {
  public:
    uint16_t *frame_buffer;

    Rect      bounds;
    Rect      clip;

    Pen       pen;

    const Font *font;

  public:
    PicoGraphics(uint16_t width, uint16_t height, uint16_t *frame_buffer);
    void set_font(const Font *font);
    void set_pen(uint8_t r, uint8_t g, uint8_t b);
    void set_pen(uint32_t rgb);
    void set_pen(Pen p);

    constexpr Pen create_pen(uint8_t r, uint8_t g, uint8_t b) {
      uint16_t p = ((r & 0b11111000) << 8) |
                  ((g & 0b11111100) << 3) |
                  ((b & 0b11111000) >> 3);

      return p; // __builtin_bswap16(p);
    };

    void set_clip(const Rect &r);
    void remove_clip();

    Pen* ptr(const Point &p);
    Pen* ptr(const Rect &r);
    Pen* ptr(int32_t x, int32_t y);

    void clear();
    void pixel(const Point &p);
    void pixel_span(const Point &p, int32_t l);
    void rectangle(const Rect &r);
    void circle(const Point &p, int32_t r);
    void character(const char c, const Point &p, uint8_t scale = 2);
    void text(const std::string &t, const Point &p, int32_t wrap, uint8_t scale = 2);
    void polygon(const std::vector<Point> &points);
    void triangle(Point p1, Point p2, Point p3);
    void line(Point p1, Point p2);
    void fill(const Point &p);
    void triangleTexture(Point p1, Point p2, Point p3, 
        uint16_t* texture, int32_t tw, int32_t th,
        Point tp1, Point tp2, Point tp3);
        
  private:
  
    void triTexSection(int32_t &xa, int32_t &xb, int32_t dx, int32_t dx2, int32_t ya, int32_t yb,
    uint16_t* texture, int32_t tw,
    int32_t &txa, int32_t &tya, int32_t &txb, int32_t &tyb,
    int32_t tdxa, int32_t tdya, int32_t tdxb, int32_t tdyb,
    int32_t sx, int32_t bx);
  };

}
