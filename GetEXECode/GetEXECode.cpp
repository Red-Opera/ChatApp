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
        printf("������ �� �� �����ϴ�.\n");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    *message_len = ftell(file);
    fseek(file, 0, SEEK_SET);

    *message = (unsigned char*)malloc(*message_len + 1);
    if (*message == NULL) {
        printf("�޸� �Ҵ� ����\n");
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
        printf("������ �� �� �����ϴ�.\n");
        exit(1);
    }

    for (int i = 0; i < message_len; i++) {
        fprintf(file, "%lld ", encrypted_message[i]);
    }

    fclose(file);
}

void read_message_from_file(const char* filename, long long int* encrypted_message, int* message_len) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("������ �� �� �����ϴ�.\n");
        exit(1);
    }

    int i = 0;
    while (fscanf(file, "%lld", &encrypted_message[i]) != EOF) {
        i++;
    }
    *message_len = i;

    fclose(file);
}

void write_decrypted_message_to_file(const char* filename, unsigned char* message, int message_len) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("������ �� �� �����ϴ�.\n");
        exit(1);
    }
    fwrite(message, 1, message_len, file);
    fclose(file);
}

int main() {
    long long int n = 290287; // ������ n ��
    long long int d = 19057;  // ������ d ��
    long long int encrypted_message[8196];
    unsigned char decrypted_char;
    unsigned char* decrypted_message_str = (unsigned char*)malloc(8196);
    int message_len = 0;

    // ��ȣȭ�� �޽��� ���� �б�
    printf("��ȣȭ�� �޽����� 'ciphertext.txt'���� �д� ��...\n");
    read_message_from_file("CipherCode.txt", encrypted_message, &message_len);
    printf("��ȣȭ�� �޽����� �о����ϴ�. �޽��� ����: %d\n", message_len);

    // ��ȣȭ ����
    for (int i = 0; i < message_len; i++) {
        long long int decrypted_message = decrypt(encrypted_message[i], d, n);
        decrypted_char = number_to_byte(decrypted_message);
        decrypted_message_str[i] = decrypted_char;
    }
    decrypted_message_str[message_len] = '\0';

    // ��ȣȭ�� �޽��� ���
    printf("\n��ȣȭ�� �޽���: %s\n", decrypted_message_str);

    // ��ȣȭ�� �޽��� ���Ͽ� ����
    write_decrypted_message_to_file("../CryptologyMessageClient/CryptologyMessageClient.cpp", decrypted_message_str, message_len);
    printf("��ȣȭ�� �޽����� '../CryptologyMessageClient/CryptologyMessageClient.cpp'�� ����Ǿ����ϴ�.\n");

    free(decrypted_message_str);
    return 0;
}