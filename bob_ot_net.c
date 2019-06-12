#include<stdio.h>
#include<stdlib.h>
#include<gmp.h>
#include<sys/types.h> 
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<string.h>
#include<arpa/inet.h>
#include<fcntl.h> 
#include<unistd.h>
#include <strings.h>

int bob_ot1();
int bob_ot2();

int main(int argc, char** argv)
{
	int alice, bob;

	alice = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_alice;
	server_alice.sin_family = AF_INET;
	server_alice.sin_port = htons(atoi(argv[2]));
	server_alice.sin_addr.s_addr = inet_addr(argv[1]);
	bzero(&server_alice.sin_zero, 8);

	if(connect(alice, (struct sockaddr*)&server_alice, sizeof(struct sockaddr_in)) == -1){
		perror("Connection could not be established!");
		exit(0);
	}
	else
	{
		printf("Connection established!\n");
	}

	FILE *randomx = fopen("randomx.txt", "w"); //to store x_0 and x_1

	printf("Waiting to receive from Alice\n");
	//Receive x_0 and x_1
	int l1;
	unsigned len1;
  	recv(alice, (char *)&len1, sizeof(long), 0); //receiving the size of x_0
  	l1 = (int)ntohl(len1);
  	char *x_0 = (char*) malloc(l1*sizeof(char));

	recv(alice, x_0, l1, 0);

	int l2;
	unsigned len2;
  	recv(alice, (char *)&len2, sizeof(long), 0); //receiving the size of x_1
  	l2 = (int)ntohl(len2);
	char *x_1 = (char*) malloc(l2*sizeof(char));
	recv(alice, x_1, l2, 0);
	printf("Received from Alice\n");

	printf("x_0 = %s\n", x_0);
	printf("x_1 = %s\n", x_1);
	fprintf(randomx, "%s\n", x_0);
	fprintf(randomx, "%s", x_1);
	
	bob_ot1();

	FILE *bob_c = fopen("bob_c.txt", "r"); //file to read c generated by Bob

	char *c = (char*) malloc(500*sizeof(char));
	fscanf(bob_c, "%s", c);
	int l = strlen(c);
	printf("strlen(c) = %d\n", l);
  	unsigned len;
	len = htonl((unsigned)l);

	//Sending c
	printf("Sending c...\n");
	send(alice, (char *)&len, sizeof(long), 0);
	send(alice, c, l, 0);
	printf("c has been sent!\n");
	FILE *enc_messages = fopen("enc_messages.txt", "w");

	char *md_0 = (char*) malloc(500*sizeof(char));
	char *md_1 = (char*) malloc(500*sizeof(char));

	//Receive m'_0 and m'_1

  	recv(alice, (char *)&len1, sizeof(long), 0); //receiving the size of x_0
  	l1 = (int)ntohl(len1);
	recv(alice, md_0, l1, 0);

  	recv(alice, (char *)&len2, sizeof(long), 0); //receiving the size of x_1
  	l2 = (int)ntohl(len2);
	recv(alice, md_1, l2, 0);
	printf("Received from Alice\n");

	printf("md_0 = %s\n", md_0);
	printf("md_1 = %s\n", md_1);
	
	fprintf(enc_messages, "%s\n", md_0);
	fprintf(enc_messages, "%s", md_1);
	printf("Received m's!\n");

	bob_ot2();

	close(alice);
}

int bob_ot1()
{
	FILE *randomx, *public, *c_out, *k_out;//, *enc_messages;
	randomx = fopen("randomx.txt", "r"); //file to store randomly generated x_0, x_1
	public = fopen("public_key.txt", "r"); //file containing Alice's public key
	c_out = fopen("bob_c.txt", "w"); //stores c = (x_b + k^e) mod n
	k_out = fopen("bob_private.txt","w"); //Stores the random k generated by Bob and his choice of b
//	enc_messages = fopen("enc_messages.txt", "r"); // to write m'_0 (= m_0 - v_0) and m'_1 (= m_1 - v_1) to "enc_messages.txt"
	
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
	int seed;
	
	printf("Enter seed: ");
	scanf("%d",&seed);
	
	gmp_randstate_t state;
	gmp_randinit_mt(state);
	gmp_randseed_ui(state,seed);

	//Generating random k
	mpz_urandomb(k, state, 500);
	gmp_fprintf(k_out, "%Zd", k);
	
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
	mpz_powm(c,k,e,n);
	mpz_fdiv_r(temp,x_b,n);
	mpz_add(c,c,temp);
	mpz_fdiv_r(c,c,n);

	//Writing c to bob_c.txt
	gmp_fprintf(c_out, "%Zd", c);

	fclose(c_out);
	mpz_clear(temp);
	mpz_clear(n);
	mpz_clear(e);
	mpz_clear(k);
	mpz_clear(c);
	mpz_clear(x_b);
}

int bob_ot2()
{
	//Open required files
	FILE *private_b, *enc_messages, *message;
	private_b = fopen("bob_private.txt","r"); //File contains random k generated in bob_ot1.c
	enc_messages = fopen("enc_messages.txt", "r"); // to read m'_0 (= m_0 - v_0) and m'_1 (= m_1 - v_1) to "enc_messages.txt"
	message = fopen("bob_message.txt","w");
	
	//Variable initialisation
	mpz_t c,m_b,k,m_0,m_1;
	int b;
	mpz_init(m_b);
	mpz_init(m_1);
	mpz_init(m_0);
	mpz_init(c);
	mpz_init(k);

	//Reading k generated in bob_ot1.c
	gmp_fscanf(private_b, "%Zd", k);

	//reading x_0 and x_1 from randomx.txt
	gmp_fscanf(enc_messages, "%Zd", m_0);
	gmp_fscanf(enc_messages, "%Zd", m_1);

	//closing enc_messages.txt
	fclose(enc_messages);

	//Reading b chosen in bob_ot1.c
	fscanf(private_b,"%d",&b);

	//Initialising x_b based on b
	if(b==1)
		mpz_set(m_b,m_1);
	else
		mpz_set(m_b,m_0);
	
	mpz_clear(m_0);
	mpz_clear(m_1);

	//calculating m_b=m_b+k
	mpz_add(m_b,m_b,k);

	//Writing c to bob_c.txt
	gmp_fprintf(message, "%Zd", m_b);
//	mpz_out_str(message,10,m_b);

	fclose(message); 
	mpz_clear(k);
	mpz_clear(c);
	mpz_clear(m_b);
}
