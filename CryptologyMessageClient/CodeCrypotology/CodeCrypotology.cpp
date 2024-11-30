#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

long long int gcd(long long int a, long long int b) {
    while (b != 0) {
        long long int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}
long long int lcm(long long int a, long long int b) {
    return (a * b) / gcd(a, b);
}


long long int mod_inverse(long long int a, long long int m) {
    long long int m0 = m, t, q;
    long long int x0 = 0, x1 = 1;
    if (m == 1) {
        return 0;
    }
    while (a > 1) {
        q = a / m;
        t = m;
        m = a % m;
        a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }
    if (x1 < 0) {
        x1 += m0;
    }
    return x1;
}

int is_prime(long long int num) {
    if (num <= 1) return 0;
    for (long long int i = 2; i <= sqrt(num); i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

void generate_keys(long long int* n, long long int* e, long long int* d) {
    long long int p, q;
    do {
        p = rand() % 1000 + 50;
    } while (!is_prime(p));

    do {
        q = rand() % 1000 + 50;
    } while (!is_prime(q));

    *n = p * q;
    long long int phi = lcm(p - 1, q - 1);

    do {
        *e = rand() % (phi - 2) + 2;
    } while (gcd(*e, phi) != 1);

    *d = mod_inverse(*e, phi);
}

long long int mod_exp(long long int base, long long int exp, long long int mod) {
    long long int result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp = exp / 2;
    }
    return result;
}

long long int encrypt(long long int msg, long long int e, long long int n) {
    return mod_exp(msg, e, n);
}

long long int decrypt(long long int c, long long int d, long long int n) {
    return mod_exp(c, d, n);
}

void read_message_from_file(const char* filename, unsigned char** message, int* message_len) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("파일을 열 수 없습니다.\n");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    *message_len = ftell(file);
    fseek(file, 0, SEEK_SET);

    *message = (unsigned char*)malloc(*message_len + 1);
    if (*message == NULL) {
        printf("메모리 할당 오류\n");
        exit(1);
    }

    fread(*message, 1, *message_len, file);
    (*message)[*message_len] = '\0';

    fclose(file);
}

long long int byte_to_number(unsigned char byte) {
    return (long long int)byte;
}

unsigned char number_to_byte(long long int num) {
    return (unsigned char)num;
}

void write_encrypted_message_to_file(const char* filename, long long int* encrypted_message, int message_len) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("파일을 열 수 없습니다.\n");
        exit(1);
    }

    for (int i = 0; i < message_len; i++) {
        fprintf(file, "%lld ", encrypted_message[i]);
    }

    fclose(file);
}

void write_decrypted_message_to_file(const char* filename, unsigned char* message, int message_len) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("파일을 열 수 없습니다.\n");
        exit(1);
    }
    fwrite(message, 1, message_len, file);
    fclose(file);
}

int main() {
    long long int n, e, d;
    unsigned char* message = NULL;
    int message_len = 0;
    long long int encrypted_message[8196], decrypted_message;
    unsigned char decrypted_char;
    unsigned char* decrypted_message_str = (unsigned char*)malloc(8196);

    generate_keys(&n, &e, &d);
    printf("공개 키 (n, e): (%lld, %lld)\n", n, e);
    printf("개인 키 (n, d): (%lld, %lld)\n", n, d);

    read_message_from_file("message.txt", &message, &message_len);
    printf("원본 메시지: %s\n", message);

    printf("\n암호화된 메시지 (ciphertext)를 파일에 저장 중...\n");
    for (int i = 0; i < message_len; i++) {
        encrypted_message[i] = encrypt(byte_to_number(message[i]), e, n);
    }

    write_encrypted_message_to_file("ciphertext.txt", encrypted_message, message_len);
    printf("암호화된 메시지가 'ciphertext.txt'에 저장되었습니다.\n");

    for (int i = 0; i < message_len; i++) {
        decrypted_message = decrypt(encrypted_message[i], d, n);
        decrypted_char = number_to_byte(decrypted_message);
        decrypted_message_str[i] = decrypted_char;
    }
    decrypted_message_str[message_len] = '\0';

    printf("\n복호화된 메시지: %s\n", decrypted_message_str);

    write_decrypted_message_to_file("decrypted_message.txt", decrypted_message_str, message_len);

    free(message);
    free(decrypted_message_str);

    return 0;
}
