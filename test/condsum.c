int global, c[42], b[13];
int test;
int scan ();
void print (int i);
void exit (int i, int y[2]);

/**
 *
 * @param arr
 * @param len
 * @param threshold
 * @return
 */
int func (int arr[10], int len, int threshold) {
  int i, sum;
  i = 0;
  sum = 0;
  while (i < len) {
    if (arr[i] <= threshold) sum = sum + 1;
    i = i + 1;
    print(i); 
  }
  return sum;
}

int main () {
  global = 1;
  return func(a,5+5,5);

  print(scan());

  exit(0);
}
