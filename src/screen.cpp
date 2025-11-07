/** See a brief introduction (right-hand button) */
#include "screen.h"
/* Private include -----------------------------------------------------------*/
#include <assert.h>
#include <string.h>
/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define bitSet(value, bit)  ((value) |= (1UL << (bit)))
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */

Screen::Screen(size_t y_unit, size_t x_unit)
    : _range{.h = y_unit * _UNIT_PRE_BIT / _BIT_PRE_PART,
             .w = x_unit * _UNIT_PRE_BIT / _BIT_PRE_PART}
    , _buffer{new uint32_t[_range.h * _range.w / 32]{0}}
{}

Screen::~Screen() { delete[] _buffer; }

void Screen::clear()
{
  memset(_buffer, 0, sizeof(uint32_t) * _range.h * _range.w / 32);
}

void Screen::fill_triangle(const point_t *ps, size_t count)
{
  assert(count == 3);
  if(count != 3) {
    return;
  }

  const int origX = 0;
  const int origY = 0;

  int maxX = _range.w;
  int minX = 0;
  int maxY = _range.h;
  int minY = 0;

  int16_t nodeX[count];

  for(int pixelY = minY; pixelY <= maxY; ++pixelY) {
    int nodes = 0;
    int j     = count - 1;
    for(int i = 0; i < count; ++i) {
      int piy = ps[i].y + origY;
      int pjy = ps[j].y + origY;
      if((piy <= pixelY && pjy > pixelY) || (pjy <= pixelY && piy > pixelY)) {
        int pjx        = ps[j].x + origX;
        int pix        = ps[i].x + origX;
        int a          = (pixelY - piy) * (pjx - pix);
        int b          = (pjy - piy);
        nodeX[nodes++] = pix + a / b + (((a < 0) ^ (b > 0)) && (a % b));
      }
      j = i;
    }

    int i = 0;
    while(i < nodes - 1) {
      if(nodeX[i] > nodeX[i + 1]) {
        std::swap(nodeX[i], nodeX[i + 1]);
        if(i) {
          --i;
        }
      } else {
        ++i;
      }
    }

    for(int i = 0; i < nodes; i += 2) {
      if(nodeX[i] > maxX) {
        break;
      }

      if(nodeX[i + 1] > minX) {
        if(nodeX[i] < minX) {
          nodeX[i] = minX;
        }
        if(nodeX[i + 1] > maxX) {
          nodeX[i + 1] = maxX;
        }

        fill_row(pixelY, nodeX[i], nodeX[i + 1] - 1);
      }
    }
  }
}

void Screen::fill_row(int y, int x1, int x2)
{
  while(x1 <= x2) {
    fill_pix({x1, y});
    x1 += 1;
  }
}

void Screen::fill_pix(point_t p)
{
  if(p.x > _range.w | p.y > _range.h) {
    return;
  }

  auto index = p.y * _range.w / 32 + p.x / 32;
  bitSet(_buffer[index], p.x % 32);
}

void Screen::print()
{
  for(int y = 0; y < _range.h; y++) {
    for(int x = 0; x < _range.w; x++) {
      if(x % 32 == 0) {
        putc(' ', stdout);
      }
      auto index = y * _range.w / 32 + x / 32;
      putc(bitRead(_buffer[index], x % 32) == true ? '-' : '.', stdout);
    }
    putc('\n', stdout);
  }
}
