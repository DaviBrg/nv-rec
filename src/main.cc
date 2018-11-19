#include <iostream>
#include <openssl/rsa.h>

using std::cout;

extern "C" {
  int RSA_set0_key(RSA *r, BIGNUM *n, BIGNUM *e, BIGNUM *d) {
    return 0;
  }
}

int main(int argc, char **argv) {
  cout << " Hello World!\n";
  return 0;
}
