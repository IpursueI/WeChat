#ifndef CHECK_SIGNATURE_H_
#define CHECK_SIGNATURE_H_

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <cstring>
#include <openssl/sha.h>

using namespace std;

bool checkSignature(const string &signature, const string &timestamp, const string &nonce);

#endif

