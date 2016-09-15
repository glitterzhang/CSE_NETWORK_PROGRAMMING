#include <stdio.h>
#include <string.h>
/*Funciton filesize is to calculate the size of file*/

void filesize(char *input, char *output)
{
    FILE *fp,*fp1;
    int fsize;
    fp=fopen(input,"rb");
    /*input - the name of input file*/
    fp1 = fopen(output,"w");
    /*output - the name of output file*/
  if (fp==NULL){
  	printf("No input filename");
  }
  else{
    fseek(fp, 0L, SEEK_END);
    fsize = ftell(fp);  
    printf("Size of file is %d\n",fsize);
    fprintf(fp1, "Size of file is %d\n",fsize);
    fclose(fp);
    fclose(fp1);
}
}
/*Funciton filesize is to calculate the number of match string*/
void Search(char *input, char *string, char *output)
{
    
    int num = 0;
    FILE *fp, *fp1;
    char str[1000];
    fp = fopen(input, "r");
    fp1 = fopen(output, "a+");
    if((fp== NULL)||(fp1 == NULL))
    {
        return;
    }
/* count the number of match string*/

    while(fgets(str, 1000, fp) != NULL) 
    {
        const char* temp = str;
        while(temp = strstr(temp, string)) 
        {
            num++;
            temp++;
        }
    }
    printf("The number of matches is %d\n", num);
    fprintf(fp1, "The number of matches is %d\n", num);
    fclose(fp);
    fclose(fp1);
}

int main(int argc, char *argv[])
{
  if (argc !=4)
  {
    printf("Usage:count <input> <string> <output>" );
  }
  else
  {
    filesize(argv[1],argv[3]);
    Search(argv[1],argv[2],argv[3]);
  }
  
  return 0;
}