#define _POSIX_C_SOURCE 200809L

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>
#include <stdlib.h>

long pos_threshold = 10200547327;
long neg_threshold = 536870911;
uint32_t MU = 536870912;

int error = 0;
int count = 0;

int get_error_if_positive(LweSample *sample, int start, int end, TFheGateBootstrappingSecretKeySet* key, int ans) {
    
    long mid = (start + end)/2;

    int ci;
    long distance_from_threshold;
    
    if(start == end-1) {//base case for recursion
        return error;
    }
    
    distance_from_threshold = pos_threshold - mid;
    
    sample->b += distance_from_threshold; 
    ci = bootsSymDecrypt(sample, key);
    count = count + 1;
    sample->b -= distance_from_threshold;
    
    if(ci == ans) {//if decrypted correctly
        error = mid;
    	get_error_if_positive(sample, start, mid, key, ans);
    }
    else {
    	get_error_if_positive(sample, mid, end, key, ans);
    }
}

int get_error_if_negative(LweSample *sample, int start, int end, TFheGateBootstrappingSecretKeySet* key, int ans) {
    
    long mid = (start + end)/2;

    int ci;
    long distance_from_threshold;
    
    if(start == end-1) {//base case for recursion
        return error;
    }
    
    distance_from_threshold = neg_threshold - mid;
    
    sample->b -= distance_from_threshold; 
    ci = bootsSymDecrypt(sample, key);
    count = count + 1;
    sample->b += distance_from_threshold;
    
    if(ci == ans) {//if decrypted correctly
        error = mid;
    	get_error_if_negative(sample, start, mid, key, ans);
    }
    else {
    	get_error_if_negative(sample, mid, end, key, ans);
    }
}

void get_sign(LweSample *sample, int pos_min, int pos_max, int neg_min, int neg_max, TFheGateBootstrappingSecretKeySet* key, int ans) {
    long distance_from_threshold = pos_threshold - pos_min;
    FILE* samples;
    sample->b += distance_from_threshold;
    int ai = bootsSymDecrypt(sample, key);
    count = count + 1;
    sample->b -= distance_from_threshold;

    int error_1 = 0;
    
    if(ans != ai) {

    	error_1 = get_error_if_positive(sample, pos_min, pos_max, key, ans);

    	if(error_1 != 0) {//If the recovered error is zero, the sample will be rejected as it is highly unlikely that a resultant ciphertext of a homomorphic gate computation will be error-free
    		FILE* plaintext_ciphertext = fopen("gaussian_elimination.txt","a");
    		fprintf(plaintext_ciphertext, "recovered_error: %d m: %u b: %u a: ", error_1, ans == 1 ? MU : -MU, sample->b);
    		for (int32_t i = 0; i < key->params->in_out_params->n; ++i)
    		{
    		        fprintf(plaintext_ciphertext, "%u,", sample->a[i]);
    		}
    		fprintf(plaintext_ciphertext, "\n");
    		fclose(plaintext_ciphertext);

    	}

    }
    else {
    
    	error_1 = get_error_if_negative(sample, neg_min, neg_max, key, ans);

    	if(error_1 != 0) {//If the recovered error is zero, the sample will be rejected as it is highly unlikely that a resultant ciphertext of a homomorphic gate computation will be error-free
    		FILE* plaintext_ciphertext = fopen("gaussian_elimination.txt","a");
    		fprintf(plaintext_ciphertext, "recovered_error: %d m: %u b: %u a: ", -error_1, ans == 1 ? MU : -MU, sample->b);
    		for (int32_t i = 0; i < key->params->in_out_params->n; ++i)
    		{
    		        fprintf(plaintext_ciphertext, "%u,", sample->a[i]);
    		}
    		fprintf(plaintext_ciphertext, "\n");
    		fclose(plaintext_ciphertext);

    	}

    }

}

int main( int argc, char *argv[] ) {

    //reads the cloud key from file
    FILE* secret_key = fopen("secret.key","rb");
    TFheGateBootstrappingSecretKeySet* key = new_tfheGateBootstrappingSecretKeySet_fromFile(secret_key);
    fclose(secret_key);

    //if necessary, the params are inside the key
    const TFheGateBootstrappingParameterSet* params = key->params;

    //read the 1 ciphertext of the result
    LweSample* answer = new_gate_bootstrapping_ciphertext_array(1, params);

    //import the 32 ciphertexts from the answer file
    FILE* answer_data = fopen("answer.data","rb");

    for (int i=0; i<1; i++) 
        import_gate_bootstrapping_ciphertext_fromFile(answer_data, &answer[i], params);
    fclose(answer_data);

    long pos_min = 0;
    long pos_max = 10200547327;
    
    long neg_min = 0;
    long neg_max = 536870911;
 
    int ans = bootsSymDecrypt(answer, key);
    count = count + 1;

    if(ans == 1) {
    	get_sign(answer, pos_min, pos_max, neg_min, neg_max, key, ans);
    }

    FILE* count_of_oracle_accesses = fopen("Number_of_decryptions.csv","a");
    fprintf(count_of_oracle_accesses, "%d\n", count);
    fclose(count_of_oracle_accesses);

    //clean up all pointers
    delete_gate_bootstrapping_ciphertext_array(1, answer);
    delete_gate_bootstrapping_secret_keyset(key);
}

