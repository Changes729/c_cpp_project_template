/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SCREEN_H
#define _SCREEN_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <iostream>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
class Screen
{
private:
  const size_t _METER_PRE_UNIT = 2;  /** pre Meter */
  const size_t _UNIT_PRE_BIT   = 64; /** pre Unit */
  const size_t _BIT_PRE_PART   = 8;

public:
  using point_t = struct
  {
    int x;
    int y;
  };
  using range_t = struct
  {
    size_t h;
    size_t w;
  };

public:
  Screen(size_t y_unit, size_t x_unit);
  ~Screen();

  void clear();
  void fill_triangle(const point_t *ps, size_t count);
  void print();

  range_t get_range() { return _range; }

protected:
  void fill_row(int y, int x1, int x2);
  void fill_pix(point_t p);

private:
  range_t   _range;
  uint32_t *_buffer;
};

#endif /* _SCREEN_H */
