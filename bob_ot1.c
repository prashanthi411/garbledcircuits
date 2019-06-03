#include <stdio.h> 
#include <stdlib.h>
#include <gmp.h>
#include "rsa.h"

int main(){
	//Open required files
	FILE *randomx, *public, *c_out, *k_out, *enc_messages;
	randomx = fopen("randomx.txt", "r"); //file to store randomly generated x_0, x_1
	public = fopen("public_key.txt", "r"); //file containing Alice's public key
	c_out = fopen("bob_c.txt", "w"); //stores c = (x_b + k^e) mod n
	k_out = fopen("bob_private.txt","w"); //Stores the random k generated by Bob and his choice of b
	enc_messages = fopen("enc_messages.txt", "r"); // to write m'_0 (= m_0 - v_0) and m'_1 (= m_1 - v_1) to "enc_messages.txt"
	
	//Variable initialisation
	mpz_t c,x_b,k,e,n,x_0,x_1,temp;
	int b;
	mpz_init(x_b);
	mpz_init(x_1);
	mpz_init(x_0);
	mpz_init(c);
	mpz_init(k);
	mpz_init(e);
	mpz_init(n);
	mpz_init(temp);
	gmp_randstate_t state;
	gmp_randinit_mt(state);

	//Generating random k
	mpz_urandomb(k, state, 500);
	mpz_out_str(k_out,10,k);
	
	//Reading n and e from public_key.txt
	gmp_fscanf(public, "%Zd", n);
	gmp_fscanf(public, "%Zd", e);

	//reading x_0 and x_1 from randomx.txt
	gmp_fscanf(randomx, "%Zd", x_0);
	gmp_fscanf(randomx, "%Zd", x_1);

	//closing public_key.txt and randomx.txt
	fclose(public);
	fclose(randomx);

	//Bob chooses b
	printf("Enter b: ");
	scanf("%d",&b);
	fprintf(k_out,"\n%d",b);
	fclose(k_out);

	//Initialising x_b based on b
	if(b==1)
		mpz_set(x_b,x_1);
	else
		mpz_set(x_b,x_0);
	
	mpz_clear(x_0);
	mpz_clear(x_1);

	//calculating c=(x_b + (k^e)) mod n
	pmod(c,k,e,n);
	mpz_fdiv_r(temp,x_b,n);
	mpz_add(c,k,temp);
	mpz_fdiv_r(c,c,n);

	//Writing c to bob_c.txt
	mpz_out_str(c_out,10,c);

	fclose(c_out);
	mpz_clear(temp);
	mpz_clear(n);
	mpz_clear(e);
	mpz_clear(k);
	mpz_clear(c);
	mpz_clear(x_b);
}
