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
	i = scan();
	sum = 20;

	arr[0] = scan();
	i = arr[1];
	arr[3] = arr[4];

	while (scan() >= scan()) {
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

	while (arr[i] < len) {
		if (i <= threshold)
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

void error() {
//	main() = 4;
//	a[m] = 42;
//	a[a] = 42;
//	scan(2);
//	print();
//	func(a,a,1);
}

int main() {
	global = 1;
	a[1] = 5;
	a[2] = func(a, 5 + 5, 5);

	print(scan());

	exit(0);

	if(!(4 < 5))
		global = 4;
	if( 4 < 5 && 5 < 6)
		global = 2;
	if( 4 < 5 || 5 < 6)
		global = 3;
}
