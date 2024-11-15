#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 16

// 배열을 무작위로 섞는 함수
void shuffle(int array[], int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1); // 0부터 i 사이의 랜덤 인덱스 생성
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// S-box와 역 S-box를 생성하는 함수
void generateSBoxAndInverse(int sbox[SIZE][SIZE], int inverse[SIZE][SIZE]) {
    int flatArray[SIZE * SIZE]; // 1차원 배열로 S-box 값 저장

    // flatArray에 0부터 255까지의 값 저장
    for (int i = 0; i < SIZE * SIZE; i++) {
        flatArray[i] = i;
    }

    // flatArray를 무작위로 섞음
    shuffle(flatArray, SIZE * SIZE);

    // S-box와 역 S-box 생성
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int value = flatArray[i * SIZE + j];
            sbox[i][j] = value;                               // S-box에 값 저장
            inverse[value / SIZE][value % SIZE] = i * SIZE + j; // 역 S-box에 대응되는 위치 저장
        }
    }
}

// 2차원 배열을 출력하는 함수
void printArray(int array[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%3d ", array[i][j]); // 각 값을 3칸 간격으로 출력
        }
        printf("\n");
    }
}

//int main() {
//    int sbox[SIZE][SIZE];        // S-box를 저장할 배열
//    int inverse[SIZE][SIZE] = { 0 }; // 역 S-box를 저장할 배열
//
//    // 난수 생성을 위한 초기화
//    srand(time(NULL));
//
//    // S-box와 역 S-box 생성
//    generateSBoxAndInverse(sbox, inverse);
//
//    // S-box 출력
//    printf("S-box:\n");
//    printArray(sbox);
//
//    // 역 S-box 출력
//    printf("\nInverse S-box:\n");
//    printArray(inverse);
//
//    return 0;
//}
//