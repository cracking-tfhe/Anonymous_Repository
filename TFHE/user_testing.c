#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    srand(2022);

    //generate a keyset
    const int minimum_lambda = 110;
    TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(minimum_lambda);

    //generate a random key
    uint32_t seed[] = { 101, 103, 107 };
    tfhe_random_generator_setSeed(seed,3);
    TFheGateBootstrappingSecretKeySet* key = new_random_gate_bootstrapping_secret_keyset(params);

    //export the secret key to file for later use
    FILE* secret_key = fopen("secret.key","wb");
    export_tfheGateBootstrappingSecretKeySet_toFile(secret_key, key);
    fclose(secret_key);

    //export the cloud key to a file (for the cloud)
    FILE* cloud_key = fopen("cloud.key","wb");
    export_tfheGateBootstrappingCloudKeySet_toFile(cloud_key, &key->cloud);
    fclose(cloud_key);
    
    FILE* plaintext_ciphertext = fopen("secret_key.txt","a");
    for (int32_t i = 0; i < 630; ++i)
    {
    	fprintf(plaintext_ciphertext, "%u ", key->lwe_key->key[i]);
    }
    fclose(plaintext_ciphertext);

    int plaintext1;
    int plaintext2;

    FILE* data_points;

    for (int i = 1; i <= 1500; i++)
    {
	plaintext1 = rand()%2;
    	plaintext2 = rand()%2;

    	LweSample* ciphertext1 = new_gate_bootstrapping_ciphertext_array(1, params);
    	for (int j=0; j<1; j++) {
            bootsSymEncrypt(&ciphertext1[j], (plaintext1>>j)&1, key);
    	}

    	LweSample* ciphertext2 = new_gate_bootstrapping_ciphertext_array(1, params);
    	for (int k=0; k<1; k++) {
            bootsSymEncrypt(&ciphertext2[k], (plaintext2>>k)&1, key);
    	}

	printf("Running iteration: %d\n", i);

    	//export the 2x16 ciphertexts to a file (for the cloud)
    	FILE* cloud_data = fopen("cloud.data","wb");
    	for (int i=0; i<1; i++) 
            export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext1[i], params);
    	for (int i=0; i<1; i++) 
            export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext2[i], params);
    	fclose(cloud_data);

    	//clean up all pointers
    	delete_gate_bootstrapping_ciphertext_array(1, ciphertext1);
    	delete_gate_bootstrapping_ciphertext_array(1, ciphertext2);
    	
    	//Processing cloud
	system("./cloud_testing");

	//Running CVO to retrieve error
	system("./verify_testing");

    }

    //clean up all pointers
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

}

