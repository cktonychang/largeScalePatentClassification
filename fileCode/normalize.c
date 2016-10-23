//
//  change.c
//  Change the sample label to +1/-1
//
//
//

#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[])
{
    char buffer[100000];
    char class;
    int num=0;
    char *tmp;
    char name[1000];
    double l;
    
    FILE *fpr=fopen(argv[1],"r");
    sprintf(name,"reg%s",argv[1]);
	printf("%s",name);
    FILE *fpw=fopen(name,"w");

    
    while (fgets(buffer,100000,fpr)) {
        class=buffer[0];
        l=-1;
        if (class=='A') l=1;
        
        strtok(buffer," ");
        num++;

        fprintf(fpw,"%g",l);
         while (tmp=strtok(NULL," ")) {
            fprintf(fpw," %s",tmp);
        }
    }
    fclose(fpw);
    fclose(fpr);
    return 0;
}
