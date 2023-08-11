// server端
//  参考：https://strawberrytree.top/blog/2020/09/17/%E4%BD%BF%E7%94%A8openssl%E4%BD%BF%E7%94%A8%E5%A4%96%E9%83%A8psk%E8%BF%9B%E8%A1%8C%E6%8F%A1%E6%89%8B%EF%BC%88tls1-3%EF%BC%89/
// g++ server-simple.cpp -o server-simple -I /usr/local/include -L
// /usr/local/lib64 -lssl -lcrypto

#include <arpa/inet.h>
#include <cstddef>
#include <openssl/err.h>
#include <openssl/ocsp.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PSK_KEY                                                                  \
    "12345678123456781234567812345678123456781234567812345678123456781234567812" \
    "3456781234567812345678"
#define PSK_UTF8_KEY      "xlidjg2w3-t*U)@xga154"
#define PSK_ID            "Client1"
#define AES256SHA384      1
#define AES128SHA256      2
#define CURR_CIPHERSUITES AES256SHA384
// #define CURR_CIPHERSUITES AES128SHA256

#define USEING_PSK

void report_error(const char *e_log) {
    perror(e_log);
    exit(EXIT_FAILURE);
}

int create_socket(int port) {
    int s;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(s, 1) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    return s;
}

static int psk_find_session_cb(SSL *ssl, const unsigned char *identity,
                               size_t identity_len, SSL_SESSION **sess) {
    SSL_SESSION *tmpsess = NULL;
    // unsigned char *key;
    unsigned char *key_utf8;
    long key_len;

    printf("xxxxxxxxx psk_find_session_cb\n");
    if (strlen(PSK_ID) != identity_len || memcmp(PSK_ID, identity, identity_len) != 0) {
        printf("PSK ID len %ld", strlen(PSK_ID));
        *sess = NULL;
        return 1;
    }

    // key = OPENSSL_hexstr2buf(PSK_KEY, &key_len);
    // if (key == NULL) {
    //     report_error("Could not convert PSK key to buffer\n");
    //     return 0;
    // }
    key_utf8 = (unsigned char *)PSK_UTF8_KEY;
    key_len  = strlen(PSK_UTF8_KEY);

    const SSL_CIPHER *cipher                       = NULL;
    const unsigned char tls13_aes128gcmsha256_id[] = {0x13, 0x01};
    const unsigned char tls13_aes256gcmsha384_id[] = {0x13, 0x02};

    cipher = SSL_CIPHER_find(ssl, tls13_aes256gcmsha384_id);//384是自协商成功
    // cipher = SSL_CIPHER_find(ssl, tls13_aes128gcmsha256_id); // 256是openssl默认的hash算法
    if (cipher == NULL) {
        report_error("Error finding suitable ciphersuite\n");
        // OPENSSL_free(key);
        return 0;
    }

    tmpsess = SSL_SESSION_new();
    if (tmpsess == NULL || !SSL_SESSION_set1_master_key(tmpsess, key_utf8, key_len) || !SSL_SESSION_set_cipher(tmpsess, cipher) || !SSL_SESSION_set_protocol_version(tmpsess, SSL_version(ssl))) {
        report_error("Session set error\n");
        // OPENSSL_free(key);
        return 0;
    }
    // OPENSSL_free(key);
    *sess = tmpsess;

    return 1;
}

void SslInit() {
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
    /* No explicit initialization is required. */
#elif defined(OPENSSL_IS_BORINGSSL)
    CRYPTO_library_init();
#else  /* !(OPENSSL_VERSION_NUMBER >= 0x1010000fL) && \
          !defined(OPENSSL_IS_BORINGSSL) */
    OPENSSL_config(NULL);
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
#endif /* !(OPENSSL_VERSION_NUMBER >= 0x1010000fL) && \
          !defined(OPENSSL_IS_BORINGSSL) */
    return;
}

SSL_CTX *create_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx) {
    SSL_CTX_set_options(
        ctx,
        SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_DTLSv1 | // 用1.3
                                                                                                //  SSL_OP_NO_TLSv1_1 |SSL_OP_NO_TLSv1_3| SSL_OP_NO_DTLSv1 | //用1.2
            SSL_OP_NO_COMPRESSION | SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);
    // if (SSL_CTX_set_cipher_list(
    //         ctx, "PSK-AES128-GCM-SHA256")
    //     != 1) {
    //     // ctx, default_cipher_tlsv1_2_list) != 1) {
    //     report_error("Set cipher list failed");
    //     return;
    // }

    // TLS1_3_VERSION
    // if (SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION) == 0) {
    //     report_error("Set ssl min version failed");
    //     return;
    // }
    // if (SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION) == 0) {
    //     report_error("Set ssl max version failed");
    //     return;
    // }
}

void showCtxCiphers(SSL_CTX *ctx) {
    STACK_OF(SSL_CIPHER) * ciphers;
    int i   = 0;
    ciphers = SSL_CTX_get_ciphers(ctx);
    printf("ssl ctx cipher lists:\n");
    for (i = 0; i < sk_SSL_CIPHER_num(ciphers); i++) {
        const SSL_CIPHER *cipher = sk_SSL_CIPHER_value(ciphers, i);
        const char *cipher_name  = SSL_CIPHER_get_name(cipher);

        printf("%s:", cipher_name);
    }
    printf("\n");
}

void showCiphers(SSL *ctx) {
    STACK_OF(SSL_CIPHER) * ciphers;
    int i   = 0;
    ciphers = SSL_get_ciphers(ctx);
    printf("ssl ctx cipher lists:\n");
    for (i = 0; i < sk_SSL_CIPHER_num(ciphers); i++) {
        const SSL_CIPHER *cipher = sk_SSL_CIPHER_value(ciphers, i);
        const char *cipher_name  = SSL_CIPHER_get_name(cipher);

        printf("%s:", cipher_name);
    }
    printf("\n");
}

void showSSLCiphers(SSL *ssl) {
    STACK_OF(SSL_CIPHER) * ciphers;
    int i   = 0;
    ciphers = SSL_get1_supported_ciphers(ssl);
    printf("ssl enable cipher lists:\n");
    for (i = 0; i < sk_SSL_CIPHER_num(ciphers); i++) {
        const SSL_CIPHER *cipher = sk_SSL_CIPHER_value(ciphers, i);
        const char *cipher_name  = SSL_CIPHER_get_name(cipher);

        printf("%s:", cipher_name);
    }
    printf("\n");
}

void showGetSharedCiphers(SSL *ssl) {
    STACK_OF(SSL_CIPHER) * ciphers;
    char buf[1024];
    int i = 0;
    SSL_get_shared_ciphers(ssl, buf, sizeof(buf));
    printf("Shared ciphers are: %s\n", buf);
}

void showClientCiphers(SSL *ssl) {
    STACK_OF(SSL_CIPHER) * ciphers;
    int i   = 0;
    ciphers = SSL_get_client_ciphers(ssl);
    printf("ssl client cipher lists:\n");
    for (i = 0; i < sk_SSL_CIPHER_num(ciphers); i++) {
        const SSL_CIPHER *cipher = sk_SSL_CIPHER_value(ciphers, i);
        const char *cipher_name  = SSL_CIPHER_get_name(cipher);

        printf("%s:", cipher_name);
    }
    printf("\n");
}

static unsigned int psk_server_cb(SSL *ssl, const char *identity,
                                  unsigned char *psk,
                                  unsigned int max_psk_len) {
    long key_len = 0;
    // unsigned char *key;

    if (identity == NULL) {
        printf("Error: client did not send PSK identity\n");
        return 0;
    }

    /* here we could lookup the given identity e.g. from a database */
    if (strcmp(identity, PSK_ID) != 0) {
        printf("PSK warning: client identity not what we expected"
               " (got '%s' expected '%s')\n",
               identity, PSK_ID);
    }

    /* convert the PSK key to binary */
    unsigned char *key_utf8 = (unsigned char *)PSK_UTF8_KEY;
    key_len                 = strlen(PSK_UTF8_KEY);
    if (key_len > (int)max_psk_len) {
        printf("psk buffer of callback is too small (%d) for key (%ld)\n",
               max_psk_len, key_len);
        return 0;
    }

    memcpy(psk, key_utf8, key_len);

    return key_len;
}

static const char *get_sigtype(int nid) {
    switch (nid) {
    case EVP_PKEY_RSA:
        return "RSA";

    case EVP_PKEY_RSA_PSS:
        return "RSA-PSS";

    case EVP_PKEY_DSA:
        return "DSA";

    case EVP_PKEY_EC:
        return "ECDSA";

    case NID_ED25519:
        return "Ed25519";

    case NID_ED448:
        return "Ed448";

    case NID_id_GostR3410_2001:
        return "gost2001";

    case NID_id_GostR3410_2012_256:
        return "gost2012_256";

    case NID_id_GostR3410_2012_512:
        return "gost2012_512";

    default:
        return NULL;
    }
}

static int do_print_sigalgs(SSL *s, int shared) {
    int i, nsig, client;

    client = SSL_is_server(s) ? 0 : 1;
    if (shared)
        nsig = SSL_get_shared_sigalgs(s, 0, NULL, NULL, NULL, NULL, NULL);
    else
        nsig = SSL_get_sigalgs(s, -1, NULL, NULL, NULL, NULL, NULL);
    if (nsig == 0)
        return 1;

    if (shared)
        printf("Shared ");

    if (client)
        printf("Requested ");
    printf("Signature Algorithms: ");
    for (i = 0; i < nsig; i++) {
        int hash_nid, sign_nid;
        unsigned char rhash, rsign;
        const char *sstr = NULL;
        if (shared)
            SSL_get_shared_sigalgs(s, i, &sign_nid, &hash_nid, NULL,
                                   &rsign, &rhash);
        else
            SSL_get_sigalgs(s, i, &sign_nid, &hash_nid, NULL, &rsign, &rhash);
        if (i)
            printf(":");
        sstr = get_sigtype(sign_nid);
        if (sstr)
            printf("%s", sstr);
        else
            printf("0x%02X", (int)rsign);
        if (hash_nid != NID_undef)
            printf("+%s", OBJ_nid2sn(hash_nid));
        else if (sstr == NULL)
            printf("+0x%02X", (int)rhash);
    }
    printf("\n");
    return 1;
}

int main(int argc, char **argv) {
    int sock;
    SSL_CTX *ctx;
    SslInit();

    ctx = create_context();
    configure_context(ctx);

    sock = create_socket(4433);
    printf("Server starting up 4433...\n");

#ifdef USEING_PSK
    printf("Using PSK\n");
    // 必须调用此函数，协商的报文才会携带PSK相关套件
    SSL_CTX_set_psk_server_callback(ctx, psk_server_cb);
    SSL_CTX_set_psk_find_session_callback(ctx, psk_find_session_cb);
    // SSL_CTX_set_tlsext_servername_callback(ctx, serverNameCallback);//SNI
#endif

    showCtxCiphers(ctx);

    printf("================================\n");
    int count = 0;

    SSL_CIPHER *cipher;
    /* Handle connections */
    while (1) {
        count += 1;
        const char reply[] = "ggggggggg ssl test\n";

        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int client    = accept(sock, (struct sockaddr *)&addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        SSL *ssl;
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        BIO *accept_bio = BIO_new_socket(client, BIO_CLOSE);
        SSL_set_bio(ssl, accept_bio, accept_bio);

        BIO *bio = BIO_pop(accept_bio);

        showCiphers(ssl);
        // showSSLCiphers(ssl);
        printf("SSL version(v1.2:%d, v1.3:%d):%d\n", TLS1_2_VERSION, TLS1_3_VERSION, SSL_version(ssl));

        if (SSL_accept(ssl) <= 0) {
            int nid;
            printf("accept error\n");
            showClientCiphers(ssl);
            showGetSharedCiphers(ssl);

            ERR_print_errors_fp(stderr);
        } else {
            showGetSharedCiphers(ssl);
            do_print_sigalgs(ssl, 1);
            printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
            SSL_write(ssl, reply, strlen(reply));
            printf("receive a connection: %d\n", count);
        }

        SSL_shutdown(ssl);
        printf("connection closed.\n");
        // SSL_free(ssl);
        printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    }
}