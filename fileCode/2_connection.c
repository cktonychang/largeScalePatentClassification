//
//  sub2.c
//  样本分割方式2：A5:B7:C4:D1
//
//
//

#include <stdio.h>
#include <string.h>

int main()
{
    char buffer[100000];
    char *tmp;
    char class;
    int num;
    int l;
    int count[17];
    char *cname[17] = {"A1","A21-A24","A41-A47","A61","A62-A63","B1-B9","B21-B24","B25-B32","B41-B44","B60","B61-B64","B65-B82","C1-C7","C8","C9-C14","C21-C30","D"};
    
    FILE *fpc=fopen("count2.txt","w");
    FILE *fpr=fopen("train.txt","r");
    FILE *fpa1=fopen("sam0.pos","w");
    FILE *fpa2=fopen("sam1.pos","w");
    FILE *fpa3=fopen("sam2.pos","w");
    FILE *fpa4=fopen("sam3.pos","w");
    FILE *fpa5=fopen("sam4.pos","w");
    
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
    
    int i,s;
    for (i=0;i<17;i++) count[i]=0;
    
    FILE *fp;
    
    while (fgets(buffer,100001,fpr)) {
        sscanf(buffer,"%c%d%*s",&class,&num);
        l=-1;
        switch (class) {
            case 'A':
                if (num==1) {fp=fpa1;s=0;}
                else if (num>=21 && num<=24) {fp=fpa2;s=1;}
                else if (num>=41&&num<=47) {fp=fpa3;s=2;}
                else if (num==61) {fp=fpa4;s=3;}
                else {fp=fpa5;s=4;}
                l=1;
                break;
                
            case 'B':
                if (num>=1 && num<=9) {fp=fpna1;s=5;}
                else if(num>=21&&num<=24) {fp=fpna2;s=6;}
                else if(num>=25&&num<=32) {fp=fpna3;s=7;}
                else if(num>=41&&num<=44) {fp=fpna4;s=8;}
                else if (num==60) {fp=fpna5;s=9;}
                else if(num>=61&&num<=64) {fp=fpna6;s=10;}
                else {fp=fpna7;s=11;}
                break;
                
            case 'C':
                if (num>=1&&num<=7) {fp=fpna8;s=12;}
                else if(num==8) {fp=fpna9;s=13;}
                else if(num>=9&&num<=14) {fp=fpna10;s=14;}
                else {s=15;fp=fpna11;}
                break;
                
            default:
                s=16;
                fp=fpna12;
                break;
        }
        
        strtok(buffer," ");
        fprintf(fp,"%d",l);
        count[s]++;
        
        while (tmp=strtok(NULL," ")) {
            fprintf(fp," %s",tmp);
        }
    }
    
    //for (i=0;i<17;i++) fprintf(fpc,"%d:\t%d\n",i,count[i]);
    for (i=0;i<17;i++) fprintf(fpc,"%s:\t%d\n",cname[i],count[i]);
    
    fclose(fpc);
    fclose(fpr);
    fclose(fpa1);
    fclose(fpa2);
    fclose(fpa3);
    fclose(fpa4);
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
    
    return 0;
    
}
