#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "linear.h"

int print_null(const char *s,...) {return 0;}

static int (*info)(const char *fmt,...) = &printf;

struct feature_node *x;
int max_nr_attr = 64;

struct model* model_;
int flag_predict_probability=0;

int min_op;
int max_op;

FILE *roc;//

struct model * * predModel;

double *min;

void exit_input_error(int line_num)
{
	fprintf(stderr,"Wrong input format at line %d\n", line_num);
	exit(1);
}

static char *line = NULL;
static int max_line_len;

static char* readline(FILE *input)
{
	int len;

	if(fgets(line,max_line_len,input) == NULL)
		return NULL;

	while(strrchr(line,'\n') == NULL)
	{
		max_line_len *= 2;
		line = (char *) realloc(line,max_line_len);
		len = (int) strlen(line);
		if(fgets(line+len,max_line_len-len,input) == NULL)
			break;
	}
	return line;
}

double min_max_predict(struct feature_node *tot)
{
	int i,j;
	double max=-214748364;

	min=(double *)malloc(sizeof(double)*max_op);

	for(i=0;i<max_op;i++) //3
		min[i]=214748364.0;

	for(i=0;i<max_op;i++)
	{
		for(j=0;j<min_op;j++)
		{
			int cal=i*min_op+j;
			double res=predict(predModel[cal],x);
			min[i]=(res<min[i])?res:min[i];
		}
		max=min[i]>max?min[i]:max;
	}
	return max;
}

void do_predict(FILE *input, FILE *output)
{
	int correct = 0;
	int total = 0;
    //int TP=0,TN=0,FP=0,FN=0;
    double threshold[11]={-8,-4,-2,-1,-0.5,0,0.5,1,2,4,8};
    double predict_label[11];//edit
    int TP[11],TN[11],FP[11],FN[11]; //edit
	double error = 0;
	double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;
    //double TPR,FPR,p,F1;
    double TPR[11],FPR[11];
    double p,F1;
    //int idx;
    int r_i;
    
    for (r_i=0; r_i<11; r_i++) {
        TP[r_i]=FP[r_i]=TN[r_i]=FN[r_i]=0;
    }

	int nr_class=get_nr_class(*predModel);
	double *prob_estimates=NULL;
	int j, n;
	int nr_feature=get_nr_feature(*predModel);

	if((*predModel)->bias>=0)
		n=nr_feature+1;
	else
		n=nr_feature;

    //edit
	if(flag_predict_probability)
	{
		int *labels;

		if(!check_probability_model(model_))
		{
			fprintf(stderr, "probability output is only supported for logistic regression\n");
			exit(1);
		}

		labels=(int *) malloc(nr_class*sizeof(int));
		get_labels(model_,labels);
		prob_estimates = (double *) malloc(nr_class*sizeof(double));
		fprintf(output,"labels");
		for(j=0;j<nr_class;j++)
			fprintf(output," %d",labels[j]);
		fprintf(output,"\n");
		free(labels);
	}
    //edit end
    
	max_line_len = 1024;
	line = (char *)malloc(max_line_len*sizeof(char));
	while(readline(input) != NULL)
	{
		int i = 0;
		//double target_label, predict_label;
        double target_label;
        
        double tmp;
		char *idx, *val, *label, *endptr;
		int inst_max_index = 0; // strtol gives 0 if wrong format

		label = strtok(line," \t\n");
		if(label == NULL) // empty line
			exit_input_error(total+1);

		target_label = strtod(label,&endptr);
		if(endptr == label || *endptr != '\0')
			exit_input_error(total+1);

		while(1)
		{
			if(i>=max_nr_attr-2)	// need one more for index = -1
			{
				max_nr_attr *= 2;
				x = (struct feature_node *) realloc(x,max_nr_attr*sizeof(struct feature_node));
			}

			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;
			errno = 0;
			x[i].index = (int) strtol(idx,&endptr,10);
			if(endptr == idx || errno != 0 || *endptr != '\0' || x[i].index <= inst_max_index)
				exit_input_error(total+1);
			else
				inst_max_index = x[i].index;

			errno = 0;
			x[i].value = strtod(val,&endptr);
			if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				exit_input_error(total+1);

			// feature indices larger than those in training are not used
			if(x[i].index <= nr_feature)
				++i;
		}

		if((*predModel)->bias>=0)
		{
			x[i].index = n;
			x[i].value = model_->bias;
			i++;
		}
		x[i].index = -1;

        //edit
		if(flag_predict_probability)
		{
			int j;
			predict_label[5] = predict_probability(model_,x,prob_estimates);
			fprintf(output,"%g",predict_label);
			for(j=0;j<model_->nr_class;j++)
				fprintf(output," %g",prob_estimates[j]);
			fprintf(output,"\n");
		}
		else
		{
        //edit end
            
		//predict_label = min_max_predict(x);
        tmp=min_max_predict(x);
        fprintf(output,"%g\t",tmp);//edit
        
        for (r_i=0; r_i<11; r_i++) {
            predict_label[r_i]=(tmp>threshold[r_i])?(*predModel)->label[0]:(*predModel)->label[1];
        }//edit
        //predict_label=(predict_label>0)?(*predModel)->label[0]:(*predModel)->label[1];
		//////////////////
		/*--------------*/
		fprintf(output,"%g\t%g\n",predict_label[5],target_label);
		
        //edit by c
        
        for (r_i=0; r_i<11; r_i++) {
            if (r_i==5 && predict_label[5] == target_label)
            //if(predict_label[r_i] == target_label)
                correct++;
            if (predict_label[r_i]>0&&target_label>0) TP[r_i]++;
            else if (predict_label[r_i]>0&&target_label<0) FP[r_i]++;
            else if(predict_label[r_i]<0&&target_label<0) TN[r_i]++;
            else FN[r_i]++;
        }
		
        //edit end
        
		error += (predict_label[5]-target_label)*(predict_label[5]-target_label);
		sump += predict_label[5];
		sumt += target_label;
		sumpp += predict_label[5]*predict_label[5];
		sumtt += target_label*target_label;
		sumpt += predict_label[5]*target_label;
		++total;
        }
	}
	if(check_regression_model(*predModel))
	{
		info("Mean squared error = %g (regression)\n",error/total);
		info("Squared correlation coefficient = %g (regression)\n",
			((total*sumpt-sump*sumt)*(total*sumpt-sump*sumt))/
			((total*sumpp-sump*sump)*(total*sumtt-sumt*sumt))
			);
	}
    else {
        info("Accuracy = %g%% (%d/%d)\n",(double) correct/total*100,correct,total);
        //edit by c
        
        for (r_i=0; r_i<11; r_i++) {
            TPR[r_i]=(double)TP[r_i]/(TP[r_i]+FN[r_i]);
            FPR[r_i]=(double)FP[r_i]/(FP[r_i]+TN[r_i]);
        }
//        TPR=(double)TP/(TP+FN);
//        FPR=(double)FP/(FP+TN);
//        p=(double)TP/(TP+FP);
//        F1=2*TPR*p/(p+TPR);
        
        p=(double)TP[5]/(TP[5]+FP[5]);
        F1=2*TPR[5]*p/(p+TPR[5]);
        
        info("TP = %d (/%d)\n",TP[5],total);
        info("FP = %d (/%d)\n",FP[5],total);
        info("TN = %d (/%d)\n",TN[5],total);
        info("FN = %d (/%d)\n",FN[5],total);
        
        info("TPR = %g (%d/(%d+%d))\n",TPR[5],TP[5],TP[5],FN[5]);
        info("FPR = %g (%d/(%d+%d))\n",FPR[5],FP[5],FP[5],TN[5]);
        info("p = %g (%d/(%d+%d))\n",p,TP[5],TP[5],FP[5]);
        info("F1 = %g\n",F1);
        
        for (r_i=0; r_i<11; r_i++) {
            fprintf(roc,"%lf:\t%lf\t%lf\n",threshold[r_i],FPR[r_i],TPR[r_i]);
        }
        //edit end
        //info("Accuracy = %g%% (%d/%d)\n",(double) correct/total*100,correct,total);
    }
    
    if(flag_predict_probability)
		free(prob_estimates);
}

void exit_with_help()
{
	printf(
	"Usage: predict [options] test_file model_file output_file\n"
	"options:\n"
	"-b probability_estimates: whether to output probability estimates, 0 or 1 (default 0); currently for logistic regression only\n"
	"-q : quiet mode (no outputs)\n"
	);
	exit(1);
}

int main(int argc, char **argv)
{
	FILE *input, *output;
    //FILE *roc;//edit
	int i;
	char modelName[1024]={0};

	//MPIpredict posNum negNum testFile modelFile resultFile

	min_op=atoi(argv[2]);
	max_op=atoi(argv[1]);
	predModel=(struct model **)malloc(min_op*max_op*sizeof(struct model*));



	input = fopen(argv[3],"r");
	if(input == NULL)
	{
		fprintf(stderr,"can't open input file %s\n",argv[3]);
		exit(1);
	}

	output = fopen(argv[5],"w");
	if(output == NULL)
	{
		fprintf(stderr,"can't open output file %s\n",argv[5]);
		exit(1);
	}
    
    char name[100];
    sprintf(name,"%s.roc",argv[5]);
    roc=fopen(name,"w");

	int sum=min_op*max_op;


	for(i=0;i<sum;i++)
	{
		sprintf(modelName,"%s%d.mod",argv[4],i);
        //sprintf(modelName,"%s.model%d",argv[4],i); //edit

		if((predModel[i]=load_model(modelName))==0)
		{
			fprintf(stderr,"can't open model file %s\n",modelName);
			exit(1);
		}
	}

	x = (struct feature_node *) malloc(max_nr_attr*sizeof(struct feature_node));
	do_predict(input, output);

	for(i=0;i<sum;i++)
	free_model_content(predModel[i]);
	free(predModel);
	free(line);
	free(min);
	free(x);
	fclose(input);
	fclose(output);
    fclose(roc);
	return 0;
}

