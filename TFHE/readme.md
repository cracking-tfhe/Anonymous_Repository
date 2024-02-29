Below is a demo of how the server can introduce perturbations in the resulting ciphertext of a homomorphic computation by choosing a bound of the error present in the resultant ciphertexts. For this POC, we assume wlog that the client has asked the server to compute a single NAND gate and has allowed the server the ability to decrypt the result of this homomorphic NAND computation. To carry out the attack, the server sets (pos_min, pos_max) as (0, 10200547327) and (neg_min, neg_max) as (0, 536870911). Given that 10200547327 and 536870911 is the positive and negative threshold of the NAND gate, respectively, (pos_min, pos_max) encompasses all possible positive errors while (neg_min, neg_max) encompasses all possible negative errors.

Before proceeding with the demo code, please run the following command to install the prerequisite libraries:

1. sudo bash setup.sh		#setup.sh is a bash script containing all the commands to install required tools. In case the script does not run for some reason, please run the commands individually to install the tools.

These commands are used to compile our scripts:

1. gcc user_testing.c -o user_testing -ltfhe-spqlios-fma 		#user_testing.c contains the code to automate the entire end-to-end process.

2. gcc cloud_testing.c -o cloud_testing -ltfhe-spqlios-fma		#cloud_testing.c contains the code that is run on the server to perform a homomorphic NAND operation on the obtained ciphertexts.

3. gcc verify_testing.c -o verify_testing -ltfhe-spqlios-fma		#verify_testing.c contains the code run by the server to perform the binary-search-based error recovery attack.

4. ./user_testing		#This command automates the end-to-end process of generating random plaintext pairs, encrypting them, sending them to the server where homomorphic operations are performed on the same and the tampered result is decrypted to check if the decryption leads to a correct or incorrect result.

The entire attack takes around 15 minutes to complete. In case the above command gets terminated before completion or the experiment needs to be repeated, please run the fifth command to clear out any temporary files created before running the fourth command again.

5. python clean_temporary_files.py

If the fourth command runs successfully, please run the sixth command to solve the system of equations created earlier and to obtain the secret key. The command automatically checks whether the obtained key is correct or not.

6. sage -python equation_solver.py

The above script uses SageMath to perform Gaussian Elimination method of solving the system of equations.
