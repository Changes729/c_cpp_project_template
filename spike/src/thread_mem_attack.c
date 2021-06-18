#include <stdio.h>
#include <stdlib.h>

#define __USE_GNU
// #define __USE_XOPEN2K
#include <pthread.h>
#include <unistd.h>

static void* Func_First(void* _)
{
  pthread_attr_t attrTmp;
  size_t         stackSize;
  void*          stackAddr = NULL;
  int            tag;
  pthread_attr_init(&attrTmp);
  pthread_getattr_np(pthread_self(), &attrTmp);
  pthread_attr_getstack(&attrTmp, &stackAddr, &stackSize);
  printf("1-%p--:%p\n", stackAddr, &tag);

  tag = 1;
  sleep(2);

  if(1 != tag) {
    printf("'tag' has been changed!\n");
  }
  printf("Tag is:%d\n", tag);

  return 0;
}

static void* Func_Attack(void* _)
{
  pthread_attr_t attrTmp;
  size_t         stackSize;
  void*          stackAddr = NULL;
  int*           pAttk     = NULL;
  pthread_attr_init(&attrTmp);
  pthread_getattr_np(pthread_self(), &attrTmp);
  pthread_attr_getstack(&attrTmp, &stackAddr, &stackSize);

  //pAttk = stackAddr - 0x1000 - sizeof(int) - 0x160;
  pAttk = stackAddr - 0x1000 - sizeof(int) - 0x160;
  printf("2-%p--:%p\n", stackAddr, pAttk);
  sleep(1);
  *pAttk = 2;

  return 0;
}

int main(void)
{
  pthread_t      tid1, tid2;
  pthread_attr_t attr1, attr2;

  pthread_attr_init(&attr1);
  pthread_attr_init(&attr2);

  printf("ret:%d %d\n",
         pthread_attr_setstacksize(&attr1, (size_t)8 * 1024 * 2),
         pthread_attr_setguardsize(&attr1, (size_t)0));
  printf("ret:%d %d\n",
         pthread_attr_setstacksize(&attr2, (size_t)8 * 1024 * 2),
         pthread_attr_setguardsize(&attr2, (size_t)0));

  pthread_create(&tid2, &attr2, Func_Attack, NULL);
  pthread_create(&tid1, &attr1, Func_First, NULL);

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  pthread_exit(NULL);
}
