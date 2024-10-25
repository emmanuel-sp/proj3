#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

//Places non-matching bytes from file1 and file2 into a new file
void step1(int file1, int file2);
//Places non-matching bytes from file2 and file1 into a new file
void step2(int file1, int file2, size_t size1, size_t size2);

int main(int argc, char * argv[]) {
	if (argc != 3) {
		printf("Usage: proj3.out <file1> <file2>\n");
		exit(0);
	}

	// Opens both files
	int file1 = open(argv[1], O_RDWR);
	int file2 = open(argv[2], O_RDWR);

	if (file1 != -1 && file2 != -1) {
		struct timeval tv;

		//Step 1
		gettimeofday(&tv, NULL);
		double start = tv.tv_sec * 1000 + tv.tv_usec * .001;
		step1(file1, file2);
		gettimeofday(&tv, NULL);
		double end = tv.tv_sec * 1000 + tv.tv_usec * .001;
		printf("Step 1 took %lf milliseconds\n", end - start);

		//Step 2
		struct stat s1;
		struct stat s2;
		stat(argv[1], &s1);
		stat(argv[2], &s2);
		gettimeofday(&tv, NULL);
		start = tv.tv_usec * .001;
		step2(file1, file2, s1.st_size, s2.st_size);
		gettimeofday(&tv, NULL);
		end = tv.tv_usec * .001;
		printf("Step 2 took %lf milliseconds\n", end - start);
	} else {
		printf("There was an error reading a file.\n");
	}
	close(file1);
	close(file2);
	return 0;
}

void step1(int file1, int file2) {
	char buffer1[2] = {0, 0};
	char buffer2[2] = {0, 0};

	int d1 = open("differencesFoundInFile1.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
	if (d1 == -1) {
		printf("There was an error creating differencesFoundInFile1.txt\n");
		exit(0);
	}
	int i = 0;
	int end1 = lseek(file1, 0, SEEK_END); // End of file 1
	int end2 = lseek(file2, 0, SEEK_END); // End of file 2
	lseek(file1, 0, SEEK_SET); // Goes back to beginning
	lseek(file2, 0, SEEK_SET); // Goes back to beginning
	while (end1 > i) {
		read(file1, buffer1, 1); // Reads the first byte from file 1 and places into buffer 1
		if (end2 > i) {
			read(file2, buffer2, 1); // Reads first byte from file 2 and places into buffer 2
		} else {
			buffer2[0] = '\0'; // If file 2 reaches its end then it places null char in the buffer
		}
		if (buffer1[0] != buffer2[0]) {
			write(d1, buffer1, 1);
		}
		i++;
}
	lseek(file1, 0, SEEK_SET); // Goes back to beginning
	lseek(file2, 0, SEEK_SET); // Goes back to beginning
	close(d1); // Closes file
}

void step2(int file1, int file2, size_t size1, size_t size2) {
	int d2 = open("differencesFoundInFile2.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
	if (d2 == -1) {
		printf("There was an error creating differencesFoundInFile2.txt\n");
		exit(0);
	}
	char * bytes1 = (char *)malloc(sizeof(char) * size1);
	char * bytes2 = (char *)malloc(sizeof(char) * size2);
	char * bytesd = (char *)malloc(sizeof(char) * size2);
	read(file1, bytes1, size1);
	read(file2, bytes2, size2);
	int i = 0;
	int j = 0;
	while(i < size2) {
		if (i > size1 || *(bytes2 + i) != *(bytes1 + i)) {
			*(bytesd + j++) = *(bytes2 + i);
		}
		i++;
	}

	int wr = write(d2, bytesd, j);
	if (wr == -1) {
		printf("There was an error writing to a file.\n");
	}
	close(d2);
	free(bytes1);
	free(bytes2);
	free(bytesd);
}
