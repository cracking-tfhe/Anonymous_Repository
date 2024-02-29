#define _POSIX_C_SOURCE 200809L

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>

int main() {

    //reads the cloud key from file
    FILE* cloud_key = fopen("cloud.key","rb");
    TFheGateBootstrappingCloudKeySet* bk = new_tfheGateBootstrappingCloudKeySet_fromFile(cloud_key);
    fclose(cloud_key);
 
    //if necessary, the params are inside the key
    const TFheGateBootstrappingParameterSet* params = bk->params;

    //read the 2x16 ciphertexts
    LweSample* ciphertext1 = new_gate_bootstrapping_ciphertext_array(1, params);
    LweSample* ciphertext2 = new_gate_bootstrapping_ciphertext_array(1, params);

    //reads the 2x16 ciphertexts from the cloud file
    FILE* cloud_data = fopen("cloud.data","rb");
    for (int i=0; i<1; i++) import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext1[i], params);
    for (int i=0; i<1; i++) import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext2[i], params);
    fclose(cloud_data);

    LweSample* result = new_gate_bootstrapping_ciphertext_array(1, params);

    bootsNAND(result, &ciphertext1[0], &ciphertext2[0], bk);
    
    uint32_t MU = 536870912;
    int n = 630;
    int key[n];

    char line[] = "1 1 1 0 1 1 0 1 0 1 1 0 0 1 1 1 0 0 1 0 0 0 0 0 0 0 0 1 1 1 0 1 1 1 0 1 1 1 0 0 1 0 0 0 1 1 1 1 0 0 1 1 0 0 0 1 0 0 0 1 0 0 0 1 1 0 1 1 0 0 0 1 1 0 0 1 0 0 1 0 1 0 1 1 1 1 1 0 1 1 0 1 1 0 1 1 1 0 1 0 1 1 1 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 1 1 1 0 1 0 0 1 0 0 1 1 0 0 1 1 1 1 0 1 0 1 0 0 0 0 1 1 0 0 0 0 0 0 1 0 0 1 0 1 0 1 1 1 0 1 1 1 0 1 0 0 1 0 0 1 0 1 0 0 1 0 0 0 0 1 1 0 1 0 0 0 1 0 0 1 1 1 0 1 0 0 0 0 1 0 1 1 0 0 1 1 1 1 1 0 0 1 0 1 0 0 0 1 0 0 1 0 1 0 0 0 0 1 0 0 0 0 1 0 1 0 1 1 0 1 0 1 0 0 0 1 0 1 1 1 0 0 0 1 1 1 0 1 0 1 0 0 1 0 0 1 0 1 0 1 1 0 0 0 1 0 0 1 0 0 1 0 0 1 1 1 1 1 0 1 1 0 1 0 1 0 0 1 0 0 1 1 1 0 0 1 1 0 0 0 1 0 1 1 0 1 0 1 0 0 1 0 0 1 0 1 1 1 0 1 0 0 1 1 0 1 1 1 0 1 0 0 1 0 0 1 0 1 0 0 0 1 0 0 0 1 1 1 1 0 0 1 0 1 1 1 0 0 0 0 1 1 0 0 0 0 1 1 1 1 1 0 1 0 1 0 1 0 0 0 0 0 0 1 1 1 1 0 0 1 1 1 1 1 0 0 1 0 0 1 0 1 0 1 0 0 1 1 1 1 0 1 1 0 0 0 0 1 1 0 1 1 1 0 0 0 0 1 1 0 1 1 1 1 1 1 1 1 0 1 1 1 0 1 0 0 1 1 0 1 1 0 0 1 0 1 1 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 1 1 0 1 0 1 1 0 0 1 1 1 1 1 1 1 1 1 0 1 0 0 1 0 1 1 1 1 0 0 1 1 0 1 0 1 1 0 1 1 0 1 1 0 1 0 1 1 0 0 0 1 1 0 1 0 0 1 0 0 1 0 1 0 0 0 1 0 1 0 0 1 0 0 0 1 0 1 1 0 0 1 1 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 0 1 0 1 1 1 0 1 1 1 0 1 0 0 0 1 1 0 1 0 0 1 0 0 0 1 0 0 1 0 0 1 0 1 0 1 1 1 0 1 1 0 1 0";
    int i = 0;

    char* tok = strtok(line, " ");

    while (tok != NULL)
    {
	key[i++] = atoi(tok);
	tok = strtok(NULL, " ");
    }

    uint32_t b = result->b;

    for (int32_t i = 0; i < n; ++i)
    {
        b -= result->a[i]*key[i];
    } 
    
    uint32_t temp = b - MU;
    uint32_t error = b - MU;
    int count = 0;
    while(temp != 0)
    {
	count = count + (temp%2);
	temp = temp/2;
    }

    FILE* plaintext_ciphertext = fopen("plaintext_ciphertext.txt","a");
    fprintf(plaintext_ciphertext, "For HomNAND: message: %u error: %u hamming_weight: %d b: %u a: ", MU, error, count, result->b);
    for (int32_t i = 0; i < n; ++i)
    {
    	fprintf(plaintext_ciphertext, "%u,", result->a[i]);
    }
    fprintf(plaintext_ciphertext, "\n");
    fclose(plaintext_ciphertext);

    //export the 32 ciphertexts to a file (for the cloud)
    FILE* answer_data = fopen("answer.data","wb");
    for (int i=0; i<1; i++) export_gate_bootstrapping_ciphertext_toFile(answer_data, &result[i], params);
    fclose(answer_data);

    //clean up all pointers
    delete_gate_bootstrapping_ciphertext_array(1, result);
    delete_gate_bootstrapping_ciphertext_array(1, ciphertext2);
    delete_gate_bootstrapping_ciphertext_array(1, ciphertext1);
    delete_gate_bootstrapping_cloud_keyset(bk);

}

