int global, a[42];
int test;
int scan();
void print(int i);
void exit(int i);
int func(int arr[10], int len, int threshold);

/**
 *
 * @param arr
 * @param len
 * @param threshold
 * @return
 */
int func(int arr[10], int len, int threshold) {
	int i, sum;
	i = 0;
	sum = 20;

	if (sum >= i) {
		i = 42;
		i = 3;
		i = 2;
	}

	if (i < sum) {
		i = 3;
		i = 2;
		i = 1;
	} else {
		i = 2;
	}
	print(i);

	while (i < len) {
		if (arr[i] <= threshold)
			sum = sum + 1;
		i = i + 1;
		print(i);
	}

	i = 0;
	do {
		print(i);
		i = i + 1;
	} while(i < 5);
	return sum;
}

int main() {
	global = 1;
	a = 5;
	a = func(a, 5 + 5, 5);

	print(scan());

	exit(0);
}
