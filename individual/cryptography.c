#include <stdio.h>
#include <string.h>
const char *action, *type; 
//动作：-e(encrypt)/-d(decrypt)；
//类型：-t(text)/-b(binary)
const char *input, *output; //输入输出文件名
unsigned char key; //密钥
void text_encrypt(FILE *fpin, FILE *fpout)
{
    char ch;
    while((ch = fgetc(fpin)) != EOF){
        ch = (ch + key) % 128;
        fputc(ch, fpout);
    }
}
void text_decrypt(FILE *fpin, FILE *fpout)
{
    char ch; 
    while((ch = fgetc(fpin)) != EOF){
        ch = (ch - key + 128) % 128;
        fputc(ch, fpout);
    }
}
void binary_encrypt(FILE *fpin, FILE *fpout)
{
    char ch; 
    while(fread(&ch, 1, 1, fpin) == 1){
        ch = ch ^ key;
        fwrite(&ch, 1, 1, fpout);
    }
}
void binary_decrypt(FILE *fpin, FILE *fpout)
{
    binary_encrypt(fpin, fpout);
}
int main(int argc, char *argv[])
{
    FILE *fpin, *fpout;
    if (argc != 6)
    {
        printf("Error\n");
        exit(-1);
    }
    action = argv[1];
    type = argv[2];
    input = argv[3];
    output = argv[4];
    key = atoi(argv[5]);
    fpin = fopen(input, "rb");
    fpout = fopen(output, "wb+");
    if (strcmp(type, "-t") == 0)
    {
        if (strcmp(action, "-e") == 0)
            text_encrypt(fpin, fpout);
        else if (strcmp(action, "-d") == 0)
            text_decrypt(fpin, fpout);
    }
    else if (strcmp(type, "-b") == 0)
    {
        if (strcmp(action, "-e") == 0)
            binary_encrypt(fpin, fpout);
        else if (strcmp(action, "-d") == 0)
            binary_decrypt(fpin, fpout);
    }
    fclose(fpin);
    fclose(fpout);
    return 0;
}