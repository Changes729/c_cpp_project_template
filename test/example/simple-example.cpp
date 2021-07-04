#include "test-entry.h"

TEST(TestLink, Delete)
{
  int* a = new int(0);

  ASSERT_FALSE(*a != 0);
  ASSERT_TRUE(0 == 0);
}
