#include <stdio.h>

#include "robot.h"

main ()
{

  int m = 7;
  int i, ii;
  int min = 100;
  int min1 = 100;

  if (InitRobot ()) {
    printf ("Cannot init");
    return 1;
  }
  const int N = 5000;
  int v[N];
  int v1[N];
  int i1 = 0;
  int y;

  i = 0;
  RotateRobot (90, 100);
  while (1) {
    v1[i1++] = GetDistance ();
    if (i1 == N)
      break;
    if (GetSpeed (0) == 0)
      break;
  }
  for (y = 0; y < i1; ++y) {
    if (v1[y] <= min1) {
      min1 = v1[y];
    }
  }

  RotateRobot (-90, 100);
  printf ("min1=%d\n", min1);

  while (1) {
    if (GetSpeed (0) == 0)
      break;
  }
  sleep (5);

  RotateRobot (90, 100);
  while (1) {
    v[i++] = GetDistance ();
    if (i == N)
      break;
    if (GetSpeed (0) == 0)
      break;
  }
  printf ("i=%d\n", i);
  int a;
  for (a = 0; a < i; ++a) {
    if (v[a] <= min) {
      min = v[a];
      ii = a;
    }
  }
  int r = 10;
  printf ("min=%d\n", min);
  if (r <= min1 - min) {
    RotateRobot (-(90 - (90 * ii) / i + m), 100);
  }
  else {
    printf ("not found\n");
    RotateRobot (-90, 100);
  }
  // какие-то наши действия

  ShutRobot ();
}
