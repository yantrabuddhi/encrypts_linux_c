#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Program by Mandeep Singh Bhatia
//slow for large files, then use ramdisk
//This version is linux only as it call rm and tar
//11 Oct 2014
//license: new BSD license
char *passWord;
char *sourceName;
char *targetName;
char tempFile[1024];//="Temporary.hb";
char tempTarGZ[1024];//="Temporary.tar.gz";
FILE *f1;
FILE *f2;
int crypt;

int File2Hex(void);
int Hex2File(void);
int Scramble(void);
int DeScramble(void);
int pack(void);
int unpack(void);

int main(int argc,char *argv[])
{
	if (argc<4 || argc>5){
		printf("\nUsage (Encryption):\n");
		printf("encrypts e password Source-file-name target-file-name");
		printf("\nUsage (Decryption):\n");
		printf("encrypts d password Source-file-name\n");
		return -1;
	}
	
	strcpy(tempFile,tmpnam(NULL));
	strcpy(tempTarGZ,tmpnam(NULL));
	
	crypt=( (*argv[1]=='e')||(*argv[1]=='E') );
	passWord=argv[2];
	sourceName=argv[3];
	
	if (crypt)
	{
		if (argc!=5){
		printf("\nUsage (Encryption):\n");
		printf("encrypts e password Source-file-name target-file-name\n");
		return -1;
		}
		targetName=argv[4]; 
		if (!pack()){
			printf("\nCould not pack\n");return -1;
			}
			else{
				sourceName=tempTarGZ;
			}
	}
	else targetName=tempTarGZ;

	if (File2Hex()){
		printf("\nconverted to hex..");
		if (crypt){
			printf("\nScrambling...");
			if (Scramble()){
				printf("\nConverting to binary...");
				if (Hex2File())
					printf("\nEncryption Successful\n");
				else {
					printf("\nFailure\n");
					return -1;
				}
			}
			else {
				printf("\nFailure\n");
				return -1;
			}
		}
		else {
			printf("\nDeScrambling...");
			if (DeScramble()){
				printf("\nConverting to binary...");
				if (Hex2File())					
					printf("\nDecryption Successful\n");
				else {
					printf("\nFailure\n");
					return -1;
				}
			}
			else {
				printf("\nFailure\n");
				return -1;
			}
		}
	}
	else {
		printf("\nFailure\n");
		return -1;
	}
	if (!crypt) if (!unpack()){printf("\nCould not unpack\n");return -1;}
	char fln[1024];
	strcpy(fln,"rm ");
	strcat(fln,tempFile);
	system(fln);
	strcpy(fln,"rm ");
	strcat(fln,tempTarGZ);
	system(fln);
	return 0;
}
int pack(void)
{
	char st[1024];
	strcpy(st,"tar -cvzf ");
	strcat(st,tempTarGZ);
	strcat(st," ");
	strcat(st,sourceName);
	return !(system(st));
}
int unpack(void)
{
	char st[1024];
	strcpy(st,"tar -xvzf ");
	strcat(st,targetName);
	return !(system(st));
}

int File2Hex(void)
{
	int c;
	char s[5];

	f1=fopen(sourceName,"rb");
	if (f1==NULL) return 0;
	f2=fopen(tempFile,"wb");
	if (f2==NULL) return 0;
	while ((c=getc(f1))!=EOF)
	{
		sprintf(s,"%x",c);
		c=strlen(s);
		if (c==1){
			s[1]=s[0];
			s[0]='0';
			s[2]='\0';
		}
		putc((int)s[0],f2);
		putc((int)s[1],f2);
	}
	fclose(f1);
	fclose(f2);
	return 1;
}

int isHex(int h)
{
	if ( ((h>='0')&&(h<='9')) || ((h>='A')&&(h<='F')) || ((h>='a')&&(h<='f')) )
		return 1;
	else
		return 0;
}

int H2D(int h)
{
	if ((h>='0')&&(h<='9')) return h-'0';
	if ((h>='A')&&(h<='F')) return h-'A'+10;
	if ((h>='a')&&(h<='f')) return h-'a'+10;
	return -1;
}

int toCh(int *i,int i1,int i2)
{
	//char c;
	if ( (!isHex(i1))||(!isHex(i2)) ) return 0;
	*i= H2D(i1)*0x10+H2D(i2);//read from file msb,lsb
	return 1;
}

int Hex2File(void)
{
	int c,c1,c2;
	int tc;//do not place toCh in if condition with c2==eof.. gives prob as it may not evaluate it

	f1=fopen(tempFile,"rb");
	if (f1==NULL) return 0;
	f2=fopen(targetName,"wb");
	if (f2==NULL) return 0;
	while ((c1=getc(f1))!=EOF){
		c2=getc(f1);
		tc=toCh(&c,c1,c2);
			if ( (c2==EOF)||(!tc) ) {
				fclose(f1);
				fclose(f2);
				return 0;
			}
			putc(c,f2);
	}
	fclose(f1);
	fclose(f2);
	return 1;
}

long Fsize(FILE *f)
{
	long sz=0;
//	long cr=ftell(f);
	rewind(f);
	while (getc(f)!=EOF)
		sz++;
//	fseek(f,cr,0);
	return sz;
}

int Scramble(void)
{
	long l,fs,c,ps;
	int ch1,ch2;
	f1=fopen(tempFile,"r+b");
	if (f1==NULL) return 0;
	
	fs=Fsize(f1);
	ps=strlen(passWord);

//	for (c=1;c<=fs;c++)
	for (c=0;c<fs;c++)
	{
		fseek(f1,c,0);
		ch1=getc(f1);
//		l=(c+passWord[(c-1)%ps])%(fs+1);
//		l=((c+passWord[(c-1)%ps])%fs) + 1;
		l=((c+(passWord[c%ps]))%fs);
		fseek(f1,l,0);
		ch2=getc(f1);
		fseek(f1,c,0);
		putc(ch2,f1);
		fseek(f1,l,0);
		putc(ch1,f1);
	}
	fclose(f1);
	return 1;
}//scramble

int DeScramble(void)
{
	long l,fs,c,ps;
	int ch1,ch2;
	f1=fopen(tempFile,"r+b");
	if (f1==NULL) return 0;
	
	fs=Fsize(f1);
	ps=strlen(passWord);

	for (c=fs-1;c>=0;c--)
	{
		fseek(f1,c,0);
		ch1=getc(f1);
//		l=(c+passWord[(c-1)%ps])%(fs+1);
		l=((c+(passWord[c%ps]))%fs);
		fseek(f1,l,0);
		ch2=getc(f1);
		fseek(f1,c,0);
		putc(ch2,f1);
		fseek(f1,l,0);
		putc(ch1,f1);
	}
	fclose(f1);
	return 1;
}//Descramble
