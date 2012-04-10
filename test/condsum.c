int global, a[42];
int test;
int scan ();
void print (int i);
void exit (int i);
//int func(int arr[10], int len, int threshold);

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

int test(int a, int b) {
	int a;
	a = 0;
	return a;
}

int main () {
  global = 1;
  return func(a,5+5,5);

  print(scan());

  exit(0);
}
