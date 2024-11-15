#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 16

// �迭�� �������� ���� �Լ�
void shuffle(int array[], int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1); // 0���� i ������ ���� �ε��� ����
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// S-box�� �� S-box�� �����ϴ� �Լ�
void generateSBoxAndInverse(int sbox[SIZE][SIZE], int inverse[SIZE][SIZE]) {
    int flatArray[SIZE * SIZE]; // 1���� �迭�� S-box �� ����

    // flatArray�� 0���� 255������ �� ����
    for (int i = 0; i < SIZE * SIZE; i++) {
        flatArray[i] = i;
    }

    // flatArray�� �������� ����
    shuffle(flatArray, SIZE * SIZE);

    // S-box�� �� S-box ����
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int value = flatArray[i * SIZE + j];
            sbox[i][j] = value;                               // S-box�� �� ����
            inverse[value / SIZE][value % SIZE] = i * SIZE + j; // �� S-box�� �����Ǵ� ��ġ ����
        }
    }
}

// 2���� �迭�� ����ϴ� �Լ�
void printArray(int array[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%3d ", array[i][j]); // �� ���� 3ĭ �������� ���
        }
        printf("\n");
    }
}

//int main() {
//    int sbox[SIZE][SIZE];        // S-box�� ������ �迭
//    int inverse[SIZE][SIZE] = { 0 }; // �� S-box�� ������ �迭
//
//    // ���� ������ ���� �ʱ�ȭ
//    srand(time(NULL));
//
//    // S-box�� �� S-box ����
//    generateSBoxAndInverse(sbox, inverse);
//
//    // S-box ���
//    printf("S-box:\n");
//    printArray(sbox);
//
//    // �� S-box ���
//    printf("\nInverse S-box:\n");
//    printArray(inverse);
//
//    return 0;
//}
//