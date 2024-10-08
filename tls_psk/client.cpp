// client 端
// 参考：https://strawberrytree.top/blog/2020/09/17/%E4%BD%BF%E7%94%A8openssl%E4%BD%BF%E7%94%A8%E5%A4%96%E9%83%A8psk%E8%BF%9B%E8%A1%8C%E6%8F%A1%E6%89%8B%EF%BC%88tls1-3%EF%BC%89/
// g++ client-simple.cpp -o client-simple -I /usr/local/include -L /usr/local/lib64 -lssl -lcrypto
//s_server -psk 786c69646a673277332d742a552940786761313534 -port 4433 -nocert -psk_identity Client1 -psk_identity Client1 -tls1_3

#include <openssl/ossl_typ.h>
#include <openssl/tls1.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#define HOST              "0.0.0.0:4433"
#define PSK_KEY           "786c69646a673277332d742a552940786761313534"
#define PSK_UTF8_KEY      "xlidjg2w3-t*U)@xga154"
#define PSK_ID            "Client1"
#define AES256SHA384      1
#define AES128SHA256      2
#define CURR_CIPHERSUITES AES256SHA384
// #define CURR_CIPHERSUITES AES128SHA256
#define SERVER_NAME "ssl.test.com"

// #define USEING_PSK

// #define CURR_CIPHERSUITES AES128SHA256

void report_error(const char *e_log) {
    perror(e_log);
    exit(EXIT_FAILURE);
}

int create_socket(int port) {
    int s;
    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    return s;
}

static int ocsp_resp_cb(SSL *s, void *arg) {
    const unsigned char *p;
    int len;
    OCSP_RESPONSE *rsp;
    len = SSL_get_tlsext_status_ocsp_resp(s, &p);
    if (p == NULL) {
        printf("no response sent\n");
        return 1;
    }
    printf("OCSP response success\n");

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

    method = TLS_client_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

static int psk_use_session_utf8_cb(SSL *ssl, const EVP_MD *md,
                                   const unsigned char **id,
                                   size_t *idlen,
                                   SSL_SESSION **sess) {
    SSL_SESSION *usesess = SSL_SESSION_new();
    printf("xxxxxxxxxxxxxxxx begin psk_use_session_utf8_cb \n");
    long key_len;
    // unsigned char *key = OPENSSL_hexstr2buf(PSK_KEY, &key_len);
    // if (key == NULL)
    // {
    //     report_error("Could not convert PSK key  to buffer\n");
    //     return 0;
    // }
    unsigned char *key_utf8 = (unsigned char *)PSK_UTF8_KEY;
    key_len                 = strlen(PSK_UTF8_KEY);

    const SSL_CIPHER *cipher                          = NULL;
    const unsigned char tls13_aes128gcmsha256_id[]    = {0x13, 0x01};
    const unsigned char tls13_aes256gcmsha384_id[]    = {0x13, 0x02};
    const unsigned char tls12_pskaes256gcmsha384_id[] = {0x00, 0xA9};

    // cipher = SSL_CIPHER_find(ssl, tls12_pskaes256gcmsha384_id);//tlsv1.2
    cipher = SSL_CIPHER_find(ssl, tls13_aes256gcmsha384_id); // 384是自协商成功
    // cipher = SSL_CIPHER_find(ssl, tls13_aes128gcmsha256_id);//256是openssl默认的hash算法
    if (cipher == NULL) {
        report_error("Error finding suitable ciphersuite\n");
        // OPENSSL_free(key);
        goto err;
    }

    if (usesess == NULL
        || !SSL_SESSION_set1_master_key(usesess, key_utf8, key_len)
        || !SSL_SESSION_set_cipher(usesess, cipher)
        || !SSL_SESSION_set_protocol_version(usesess, TLS1_3_VERSION)) {
        report_error("Session set error\n");
        // OPENSSL_free(key);
        goto err;
    }

    cipher = SSL_SESSION_get0_cipher(usesess);
    if (cipher == NULL)
        goto err;

    if (md != NULL && SSL_CIPHER_get_handshake_digest(cipher) != md) {
        goto err;
    } else {
        *sess  = usesess;
        *id    = (unsigned char *)PSK_ID;
        *idlen = strlen(PSK_ID);
    }
    return 1;
err:
    SSL_SESSION_free(usesess);
    report_error("error happens\n");
    return 0;
}

static int psk_use_session_cb(SSL *ssl, const EVP_MD *md,
                              const unsigned char **id,
                              size_t *idlen,
                              SSL_SESSION **sess) {
    SSL_SESSION *usesess = SSL_SESSION_new();
    printf("xxxxxxxxxxxxxxxx begin psk_use_session_cb \n");
    long key_len;
    unsigned char *key = OPENSSL_hexstr2buf(PSK_KEY, &key_len);
    if (key == NULL) {
        report_error("Could not convert PSK key  to buffer\n");
        return 0;
    }

    const SSL_CIPHER *cipher                             = NULL;
    const unsigned char tls13_aes128gcmsha256_id[]       = {0x13, 0x01};
    const unsigned char tls13_aes256gcmsha384_id[]       = {0x13, 0x02};
    const unsigned char tls12_dhepskaes256gcmsha384_id[] = {0x00, 0xAB};
    const unsigned char xxxxxx[]                         = {0x00, 0xB2};

    // if (CURR_CIPHERSUITES == AES256SHA384) {
    //     cipher = SSL_CIPHER_find(ssl, tls13_aes256gcmsha384_id);
    // }
    // else if (CURR_CIPHERSUITES == AES128SHA256) {
    //     cipher = SSL_CIPHER_find(ssl, tls13_aes128gcmsha256_id);
    // }
    // else {
    //     report_error("Error ciphersuites define setting\n");
    // }
    cipher = SSL_CIPHER_find(ssl, tls13_aes128gcmsha256_id);
    // cipher = SSL_CIPHER_find(ssl, tls12_dhepskaes256gcmsha384_id);
    if (cipher == NULL) {
        report_error("Error finding suitable ciphersuite\n");
        OPENSSL_free(key);
        goto err;
    }

    if (usesess == NULL
        || !SSL_SESSION_set1_master_key(usesess, key, key_len)
        || !SSL_SESSION_set_cipher(usesess, cipher)
        || !SSL_SESSION_set_protocol_version(usesess, TLS1_3_VERSION)) {
        report_error("Session set error\n");
        OPENSSL_free(key);
        goto err;
    }

    cipher = SSL_SESSION_get0_cipher(usesess);
    if (cipher == NULL)
        goto err;

    if (md != NULL && SSL_CIPHER_get_handshake_digest(cipher) != md) {
        goto err;
    } else {
        *sess  = usesess;
        *id    = (unsigned char *)PSK_ID;
        *idlen = strlen(PSK_ID);
    }
    return 1;
err:
    SSL_SESSION_free(usesess);
    report_error("error happens\n");
    return 0;
}

void showCiphers(SSL_CTX *ctx) {
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

// tls1.3 default cipher list is TLS_DEFAULT_CIPHERSUITES, define in openssl:
/*
#  define TLS_DEFAULT_CIPHERSUITES "TLS_AES_256_GCM_SHA384:" \
                                   "TLS_CHACHA20_POLY1305_SHA256:" \
                                   "TLS_AES_128_GCM_SHA256"
*/

const char default_cipher_tlsv1_2_list[] =
    // 下面全是按照3GPP TS 33.210第6.2节建议添加
    "ECDHE-ECDSA-AES128-GCM-SHA256:"
    "DHE-RSA-AES128-GCM-SHA256:"
    "ECDHE-ECDSA-AES256-GCM-SHA384:"
    "ECDHE-RSA-AES256-GCM-SHA384:"
    "DHE-PSK-AES128-GCM-SHA256:"
    "DHE-PSK-AES256-GCM-SHA384:"

    // 默认配置下符合的3GPP TS 33.210第6.2节要求
    "DHE-RSA-AES256-GCM-SHA384:"
    "ECDHE-ECDSA-CHACHA20-POLY1305-SHA256:"
    "ECDHE-RSA-CHACHA20-POLY1305-SHA256:"
    "DHE-RSA-CHACHA20-POLY1305-SHA256:"
    "ECDHE-RSA-AES128-GCM-SHA256:"

    // psk-系列的套件，是按照5gc对接iwf需求增加的
    "PSK-AES128-GCM-SHA256:"
    "PSK-AES256-GCM-SHA384:"
    "PSK-CHACHA20-POLY1305:"
    // PSK-AES128-CBC-SHA256属于TLSv1的版本，目前禁用的tlsv1，如果服务端支持此套件，还是能协商成功
    "PSK-AES128-CBC-SHA256:"

    // ECDHE-PSK-AESxxx-GCM-SHAxxx(最后两个)在openssl 1.1.1f里没有
    "ECDHE-PSK-AES128-GCM-SHA256:" 
    "ECDHE-PSK-AES256-GCM-SHA384"; 

void configure_context(SSL_CTX *ctx) {
    SSL_CTX_set_options(
        ctx,
        SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_DTLSv1 |
            // 用1.3
            // SSL_OP_NO_TLSv1_1 |
            // 用1.2
              SSL_OP_NO_TLSv1_1 |SSL_OP_NO_TLSv1_3 |
            SSL_OP_NO_COMPRESSION | SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);
    if (SSL_CTX_set_cipher_list(
            // ctx, "ALL:!COMPLEMENTOFDEFAULT:!eNULL:!SHA")        != 1) {
        ctx, default_cipher_tlsv1_2_list) != 1) {
        report_error("Set cipher list failed");
        return;
    }
    // if (SSL_CTX_set_ciphersuites(
    //         // ctx, "ALL:!COMPLEMENTOFDEFAULT:!eNULL:!SHA") != 1) {
    //         ctx, "TLS_AES_128_GCM_SHA256") != 1) {
    //      report_error("Set cipher list failed");
    //     return ;
    // }
    // SSL_CTX_set_tlsext_status_cb(ctx, cert_status_cb);

    // TLS1_3_VERSION
    // if (SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION) == 0) {
    //     report_error("Set ssl min version failed");
    //     return;
    // }
    // if (SSL_CTX_set_max_proto_version(ctx, TLS1_2_VERSION) == 0) {
    //     report_error("Set ssl max version failed");
    //     return;
    // }
#ifndef USEING_PSK
    // if (SSL_CTX_use_PrivateKey_file(ctx, "./iwf.key", SSL_FILETYPE_PEM) != 1){
    if (SSL_CTX_use_PrivateKey_file(ctx, "./5gc.key", SSL_FILETYPE_PEM) != 1) {
    // if (SSL_CTX_use_PrivateKey_file(ctx, "/root/tmp/client.key", SSL_FILETYPE_PEM) != 1) {
        report_error("Could not read private key file");
        return;
    }
    // if (SSL_CTX_use_certificate_file(ctx, "./iwf.crt", SSL_FILETYPE_PEM) != 1){
    if (SSL_CTX_use_certificate_file(ctx, "./5gc.crt", SSL_FILETYPE_PEM) != 1) {
    // if (SSL_CTX_use_certificate_file(ctx, "/root/tmp/client.crt", SSL_FILETYPE_PEM) != 1) {
        // if (SSL_CTX_use_certificate_chain_file(ssl_ctx, cert_file) != 1) {
        report_error("Could not read certificate file");
        return;
    }
    /* 检查用户私钥是否正确 */
    if (!SSL_CTX_check_private_key(ctx)) {
        report_error("Check private key failed");
        return;
    }
    printf("Read client cert and key success.\n");
#else
    // determine ciphersuite to use
    // if (CURR_CIPHERSUITES == AES256SHA384) {
    //     SSL_CTX_set_ciphersuites(ctx, "TLS_AES_256_GCM_SHA384");
    // }
    // else if (CURR_CIPHERSUITES == AES128SHA256) {
    //     SSL_CTX_set_ciphersuites(ctx, "TLS_AES_128_GCM_SHA256");
    // }
    // else {
    //     report_error("Error ciphersuites define setting\n");
    // }
#endif
}

static unsigned int psk_client_cb(SSL *ssl, const char *hint, char *identity,
                                  unsigned int max_identity_len,
                                  unsigned char *psk,
                                  unsigned int max_psk_len) {
    int ret;
    long key_len;
    // unsigned char *key;

    /*
     * lookup PSK identity and PSK key based on the given identity hint here
     */
    ret = BIO_snprintf(identity, max_identity_len, "%s", PSK_ID);
    if (ret < 0 || (unsigned int)ret > max_identity_len) {
        printf("psk id is error\n");
        return 0;
    }

    /* convert the PSK key to binary */
    unsigned char *key_utf8 = (unsigned char *)PSK_UTF8_KEY;
    key_len                 = strlen(PSK_UTF8_KEY);
    if (max_psk_len > INT_MAX || key_len > (long)max_psk_len) {
        printf("psk buffer of callback is too small (%d) for key (%ld)\n",
               max_psk_len, key_len);
        return 0;
    }

    memcpy(psk, key_utf8, key_len);

    return key_len;
}

void ShowCerts(SSL *ssl) {
    X509 *cert;
    char *line;

    // 获取证书并返回X509操作句柄
    cert = SSL_get_peer_certificate(ssl);
    if (cert != NULL) {
        printf("server数字证书信息:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("证书: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("颁发者: %s\n\n", line);
        free(line);
        X509_free(cert);
        printf("对server证书验证通过!!!\n");
    } else {
        printf("无证书信息,对client证书验证失败!!!\n");
    }
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
    SSL_CTX *ctx;
    SslInit();
    ctx = create_context();

    configure_context(ctx);

#ifdef USEING_PSK
    printf("Using PSK\n");
    // 必须调用此函数，协商的报文才会携带PSK相关函数，并且兼容tls1.3，和SSL_CTX_set_psk_use_session_callback二选一
    SSL_CTX_set_psk_client_callback(ctx, psk_client_cb);
    // 主要是tls1.3后的psk相关函数，如果只支持tls1.3，则只用此函数
    //  SSL_CTX_set_psk_use_session_callback(ctx, psk_use_session_cb);
    //  SSL_CTX_set_psk_use_session_callback(ctx, psk_use_session_utf8_cb);
#else
    printf("Using CERT\n");
    SSL_CTX_set_tlsext_status_cb(ctx, ocsp_resp_cb); // OCSP
    // SSL_VERIFY_NONE
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL); // 证书校验
    // if (SSL_CTX_load_verify_locations(ctx, "5gc_ca.crt", NULL) <= 0) {
        if (SSL_CTX_load_verify_locations(ctx, "iwf_ca.crt",NULL)<=0){
        printf("unable to load verify crt.\n");
        return -2;
    }
    // SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL); //证书校验
#endif

    showCiphers(ctx);

    BIO *bio = BIO_new_ssl_connect(ctx);
    SSL *ssl;
    BIO_get_ssl(bio, &ssl);
    BIO_set_ssl_mode(bio, 1);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
#ifndef USEING_PSK
    SSL_set_tlsext_host_name(ssl, SERVER_NAME);              // SNI
    SSL_set_tlsext_status_type(ssl, TLSEXT_STATUSTYPE_ocsp); // OCSP
#endif
    // SSL_set_tlsext_host_name(ssl, SERVER_NAME); // SNI
    BIO_set_conn_hostname(bio, HOST);
    // if (SSL_set_cipher_list(            ssl, "!SHA") != 1) {
    //      report_error("Set cipher list failed");
    //     return 0;
    // }

    showSSLCiphers(ssl);

    do_print_sigalgs(ssl, 0);
    if (BIO_do_connect(bio) <= 0) {
        BIO_free_all(bio);
        printf("errored; unable to connect.\n");
        ERR_print_errors_fp(stderr);
        return -2;
    }
#ifndef USEING_PSK
    // 获取校验结果，实际上在握手阶段就会执行校验
    long verify_result = SSL_get_verify_result(ssl);
    printf("Verify return code: %ld (%s)\n", verify_result,
           X509_verify_cert_error_string(verify_result));
    ShowCerts(ssl);
#endif
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    char tmpbuf[1024 + 1];
    for (;;) {
        int len = BIO_read(bio, tmpbuf, 1024);
        if (len == 0) {
            break;
        } else if (len < 0) {
            if (!BIO_should_retry(bio)) {
                printf("errored; read failed.\n");
                ERR_print_errors_fp(stderr);
                break;
            }
        } else {
            tmpbuf[len] = 0;
            printf("%s", tmpbuf);
        }
    }

    SSL_shutdown(ssl);
    // SSL_free(ssl);

    return 0;
}