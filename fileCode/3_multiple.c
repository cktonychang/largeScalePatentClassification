//
//  sub6.c
//  样本分割方式6：A6:B9:C4:D1-多标号
//
//
//

#include <stdio.h>
#include <string.h>

int main()
{
    char buffer[100000];
    char buf1[100000];
    char label[10000];
    int repeat[20];
    int count[20];
    char *tmp;
    char *tmp1;
    char class;
    int num;
    int l;
    int i;
    int s;
    int f;
    
    char *cname[20] = {"A1","A21-A24","A41-A46","A47","A61","A62-A63","B1-B9","B21-B24","B25-B32","B41","B42-B44","B60","B61-B64","B65","B66-B82","C1-C7","C8","C9-C14","C21-C30","D"};
    
    FILE *fpc=fopen("count3.txt","w");
    FILE *fpr=fopen("train.txt","r");
    FILE *fpa1=fopen("sam0.pos","w");
    FILE *fpa2=fopen("sam1.pos","w");
    FILE *fpa3=fopen("sam2.pos","w");
    FILE *fpa4=fopen("sam3.pos","w");
    FILE *fpa5=fopen("sam4.pos","w");
    FILE *fpa6=fopen("sam5.pos","w");
    
    FILE *fpna1=fopen("sam0.neg","w");
    FILE *fpna2=fopen("sam1.neg","w");
    FILE *fpna3=fopen("sam2.neg","w");
    FILE *fpna4=fopen("sam3.neg","w");
    FILE *fpna5=fopen("sam4.neg","w");
    FILE *fpna6=fopen("sam5.neg","w");
    FILE *fpna7=fopen("sam6.neg","w");
    FILE *fpna8=fopen("sam7.neg","w");
    FILE *fpna9=fopen("sam8.neg","w");
    FILE *fpna10=fopen("sam9.neg","w");
    FILE *fpna11=fopen("sam10.neg","w");
    FILE *fpna12=fopen("sam11.neg","w");
    FILE *fpna13=fopen("sam12.neg","w");
    FILE *fpna14=fopen("sam13.neg","w");
    
    FILE *fp;
    
    for (i=0; i<20; ++i) {
        count[i]=0;
    }
    
    while (fgets(buffer,100001,fpr)) {
        sscanf(buffer,"%s %*s",label);
        f=strlen(label);
        tmp1=strtok(label,",");
        for (i=0;i<20;++i) repeat[i]=0;
        while (tmp1!=NULL) {
            sscanf(tmp1,"%c%d%*s",&class,&num);
            l=-1;
            switch (class) {
                case 'A':
                    if (num==1) {fp=fpa1;s=0;}
                    else if (num>=21 && num<=24) {fp=fpa2;s=1;}
                    else if (num>=41&&num<=46) {fp=fpa3;s=2;}
                    else if (num==47) {fp=fpa4;s=3;}
                    else if (num==61) {fp=fpa5;s=4;}
                    else {fp=fpa6;s=5;}
                    l=1;
                    break;
                    
                case 'B':
                    if (num>=1&&num<=9) {fp=fpna1;s=6;}
                    else if(num>=21&&num<=24) {fp=fpna2;s=7;}
                    else if(num>=25&&num<=32) {fp=fpna3;s=8;}
                    else if(num==41) {fp=fpna4;s=9;}
                    else if(num>=42&&num<=44) {fp=fpna5;s=10;}
                    else if(num==60) {fp=fpna6;s=11;}
                    else if(num>=61&&num<=64) {fp=fpna7;s=12;}
                    else if (num==65) {fp=fpna8;s=13;}
                    else {fp=fpna9;s=14;}
                    break;
                    
                case 'C':
                    if (num>=1&&num<=7) {fp=fpna10;s=15;}
                    else if(num==8) {fp=fpna11;s=16;}
                    else if (num>=9&&num<=14) {fp=fpna12;s=17;}
                    else  {fp=fpna13;s=18;}
                    break;
                    
                default:
                    fp=fpna14;s=19;
                    break;
            }
            
            if (repeat[s]==0) {
                fprintf(fp,"%d %s",l,buffer+f+1);
                repeat[s]=1;
                count[s]++;
            }
            tmp1=strtok(NULL,",");
        }
    }
    
    for (i=0; i<20; ++i) {
        fprintf(fpc,"%s:\t%d\n",cname[i],count[i]);
    }
    
    fclose(fpc);
    fclose(fpr);
    fclose(fpa1);
    fclose(fpa2);
    fclose(fpa3);
    fclose(fpa4);
    fclose(fpa5);
    fclose(fpa6);
    fclose(fpna1);
    fclose(fpna2);
    fclose(fpna3);
    fclose(fpna4);
    fclose(fpna5);
    fclose(fpna6);
    fclose(fpna7);
    fclose(fpna8);
    fclose(fpna9);
    fclose(fpna10);
    fclose(fpna11);
    fclose(fpna12);
    fclose(fpna13);
    fclose(fpna14);
    
    return 0;
    
}
