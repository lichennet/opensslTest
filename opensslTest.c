#include <string.h>
#include <openssl/rsa.h>
#include <openssl/aes.h>

// See blog: ����OpenSSL���Socket������а�ȫ����(RSA+AES)

int main()
{
        // 1. ����RSA��Կ��
        // ����512�ֽڹ�Կָ��ΪRSA_F4����Կ�ԣ���Կָ����RSA_F4��RSA_3����
        // �Ҳ�������ǵ����𣬾����ѡ��RSA_F4
        // ����ʹ��RSA_print_fp()����RSA����Ķ���
        RSA *ClientRsa = RSA_generate_key(512, RSA_F4, NULL, NULL);
        
        // ---------
        // 2. ��RSA�ṹ����ȡ��Կ��BUFF���Ա㽫��������Է�
        // 512λ��RSA�乫Կ�������������74�ֽڣ���˽Կ��ȡ�����г���300�ֽ�
        // Ϊ������������������Ԥ��һ��512�ֽڵĿռ�
        unsigned char PublicKey[512];
        unsigned char *PKey = PublicKey; // ע�����ָ�벻�Ƕ��࣬������Ҫ�������ģ�
        int PublicKeyLen = i2d_RSAPublicKey(ClientRsa, &PKey);
        
        // ���ܲ�������ķ�������Ϊi2d_RSAPublicKey()���޸�PublicKey��ֵ
        // ����Ҫ����PKey��������Ϊ������
        // unsigned char *PublicKey = (unsigned char *)malloc(512);
        // int PublicKeyLen = i2d_RSAPublicKey(ClientRsa, &PublicKey);
        
        // ����ֽڴ�ӡPublicKey��Ϣ
        printf("PublicKeyBuff, Len=%d\n", PublicKeyLen);
        
        // ---------
        // 3. ������������Ĺ�Կ��ϢPublicKey����һ����RSA��Կ(�����Կ�ṹֻ�й�Կ��Ϣ)
        PKey = PublicKey;
        RSA *EncryptRsa = d2i_RSAPublicKey(NULL, (const unsigned char**)&PKey, PublicKeyLen);
        
        // ---------
        // 4. ʹ��EncryptRsa�������ݣ���ʹ��ClientRsa��������
        // ע��, RSA����/���ܵ����ݳ����������ƣ�����512λ��RSA��ֻ������ܼ��ܽ���64�ֽڵ�����
        // �������RSA_NO_PADDING���ܷ�ʽ��512λ��RSA��ֻ�ܼ��ܳ��ȵ���64������
        // ������ȿ���ʹ��RSA_size()�����
        unsigned char InBuff[64], OutBuff[64];
        
        strcpy((char *)InBuff, "1234567890abcdefghiklmnopqrstuvwxyz.");
        RSA_public_encrypt(64, (const unsigned char*)InBuff, OutBuff, EncryptRsa, RSA_NO_PADDING); // ����
        
        memset(InBuff, 0, sizeof(InBuff));
        RSA_private_decrypt(64, (const unsigned char*)OutBuff, InBuff, ClientRsa, RSA_NO_PADDING); // ����
        printf("RSADecrypt OK: %s \n", InBuff);
        
        
        // ----------
        // 5. �������32�ֽ�Seed������256λ��AES��Կ��
        unsigned char Seed[32]; // ���Բ���Rand()�ȷ��������������Ϣ
        AES_KEY AESEncryptKey, AESDecryptKey;
        AES_set_encrypt_key(Seed, 256, &AESEncryptKey);
        AES_set_decrypt_key(Seed, 256, &AESDecryptKey);
        
        // ----------
        // 6. ʹ��AES��Կ��������/��������
        // ע�⣬256λ��AES��Կֻ�ܼ���/����16�ֽڳ�������
        strcpy((char *)InBuff, "a1b2c3d4e5f6g7h8?");
        AES_encrypt(InBuff, OutBuff, &AESEncryptKey);
        
        memset(InBuff, 0, sizeof(InBuff));
        AES_decrypt(OutBuff, InBuff, &AESDecryptKey);
        printf("AESDecrypt OK: %s \n", InBuff);
        
        
        // ----------
        // 7. ����Ҫ�ͷ�RSA�ṹ
        RSA_free(ClientRsa);
        RSA_free(EncryptRsa);
        
        return(0);
}
