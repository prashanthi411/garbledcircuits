#include<stdio.h> 
#include<stdlib.h>
#include<gmp.h>
#include "rsa.h"
int main(){
	FILE *fp1, *fp2, *fp3, *fp4, *fp5;
	fp1 = fopen("private_key.txt", "r"); //private key d is extracted from "private_key.txt"
	fp2 = fopen("bob_c.txt", "r"); //c, generated by Bob is extracted from "bob_c.txt" --> c = (x_b + k^e) mod n
	fp3 = fopen("blabel.txt", "r"); //the two strings are read from "blabel.txt"
	fp4 = fopen("enc_messages.txt", "w"); // to write m'_0 (= m_0 - v_0) and m'_1 (= m_1 - v_1) to "enc_messages.txt"
	fp5 = fopen("randomx.txt", "r"); //to read x_0 and x_1 from "randomx.txt"
	mpz_t x_0, x_1, v_0, v_1, n, d, c, m_0, m_1, md_0, md_1;
	mpz_init(x_0);
	mpz_init(x_1);
	mpz_init(v_0);
	mpz_init(v_1);
	mpz_init(n);
	mpz_init(d);
	mpz_init(c);
	mpz_init(m_0);
	mpz_init(m_1);
	mpz_init(md_0);
	mpz_init(md_1);
	gmp_fscanf(fp1, "%Zd", n); //extracting the value of n generated by keygen.c in "private_key.txt"
	gmp_fscanf(fp1, "%Zd", d); //extracting the value of d generated by keygen.c in "private_key.txt"
 	gmp_fscanf(fp2, "%Zd", c); //extracting the value of c from "bobc.txt"


 	mpz_fdiv_r(c, c, n);
 	gmp_fscanf(fp5, "%Zd", x_0); //extracting the value of x_0 (first string) from "randomx.txt"
 	mpz_fdiv_r(x_0, x_0, n);
	gmp_fscanf(fp5, "%Zd", x_1); //extracting the value of x_1 (second string) from "randomx.txt"
	mpz_fdiv_r(x_1, x_1, n);
	mpz_sub(v_0, c, x_0); // v_0 = c - x_0
	mpz_fdiv_r(v_0, v_0, n);
	mpz_sub(v_1, c, x_1); //v_1 = c - x_1
	mpz_fdiv_r(v_1, v_1, n);
	mpz_powm(v_0, v_0, d, n); //v_0 = (c - x_0)^d mod n
	mpz_powm(v_1, v_1, d, n); //v_1 = (c - x_1)^d mod n
	gmp_fscanf(fp3, "%Zd", m_0); //extracting the value of m_0 (first string) from "strings.txt"
	gmp_fscanf(fp3, "%Zd", m_1); //extracting the value of m_1 (second string) from "strings.txt"
	mpz_sub(md_0, m_0, v_0); //m'_0 = m_0 - v_0
	mpz_sub(md_1, m_1, v_1); //m'_1 = m_1 - v_1
	gmp_fprintf(fp4, "%Zd\n", md_0); //writing m'_0 to file "enc_messages.txt"
	gmp_fprintf(fp4, "%Zd", md_1); //writing m'_1 to file "enc_messages.txt"
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	fclose(fp4);
	fclose(fp5);
	mpz_clear(x_0);
	mpz_clear(x_1);
	mpz_clear(v_0);
	mpz_clear(v_1);
	mpz_clear(n);
	mpz_clear(d);
	mpz_clear(c);
	mpz_clear(m_0);
	mpz_clear(m_1);
	mpz_clear(md_0);
	mpz_clear(md_1);
}

