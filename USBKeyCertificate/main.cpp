#include <iostream>
#include <windows.h>
#include <string>
#include <cstring>
#include <fstream>
//#include <openssl/rsa.h>
//#include <openssl/err.h>
//#include <openssl/pem.h>
using namespace std;

// #define DEBUG

static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";


static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const *bytes_to_encode, unsigned int in_len) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';

    }

    return ret;

}

int main(int argc, char const *argv[]) {

    ifstream fileIn;
    string userInputPassword(argv[1]), userPassword;
    fileIn.open(string(argv[2])+"password.txt");
    fileIn >> userPassword;
    fileIn.close();

    if (userInputPassword != userPassword) {
        return 0;
    }

    system(("C:\\WinCPS\\OpenSSL\\bin\\openssl.exe rsautl -decrypt -in C:\\WinCPS\\Temp\\usb-nonce.en -inkey "+ string(argv[2]) +"private.pem -out "+ string(argv[2]) +"nonce.txt").c_str());

#ifdef DEBUG

    ofstream fOut("E:\\log.txt");
    fOut<<"C:\\WinCPS\\OpenSSL\\bin\\openssl.exe rsautl -decrypt -in C:\\WinCPS\\Temp\\usb-nonce.en -inkey "+ string(argv[2]) +"private.pem -out "+ string(argv[2]) +"nonce.txt";

#endif

    fileIn.open(string(argv[2])+"nonce.txt");
    int nonce;
    fileIn >> nonce;
    fileIn.close();

    string userName;
    fileIn.open(string(argv[2])+"username.txt");
    fileIn >> userName;
    fileIn.close();

    string mix = to_string(nonce) + userName;
    string response = base64_encode(reinterpret_cast<const unsigned char *>(mix.c_str()), mix.length());

    ofstream fileOut("C:\\WinCPS\\Temp\\usb-response.txt");
    fileOut<<response;
    fileOut.close();

    system(("C:\\WinCPS\\OpenSSL\\bin\\openssl.exe pkeyutl -sign -in C:\\WinCPS\\Temp\\usb-response.txt -inkey "+ string(argv[2]) +"private.pem -out C:\\WinCPS\\Temp\\usb-response.sig").c_str());

#ifdef DEBUG
    fOut<<"C:\\WinCPS\\OpenSSL\\bin\\openssl.exe pkeyutl -sign -in C:\\WinCPS\\Temp\\usb-response.txt -inkey "+ string(argv[2]) +"private.pem -out C:\\WinCPS\\Temp\\usb-response.sig";
    fOut.close();
#endif

    return 0;
}