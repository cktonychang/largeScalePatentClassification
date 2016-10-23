#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include "linear.h"
#include "mpi.h"
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
#define INF HUGE_VAL

void print_null(const char *s) {}
int MPItrain(char *posFile, char *negFile, char *modFile,int type);//edit


void exit_with_help()
{
	printf(
	"Usage: train [options] training_set_file [model_file]\n"
	"options:\n"
	"-s type : set type of solver (default 1)\n"
	"  for multi-class classification\n"
	"	 0 -- L2-regularized logistic regression (primal)\n"
	"	 1 -- L2-regularized L2-loss support vector classification (dual)\n"
	"	 2 -- L2-regularized L2-loss support vector classification (primal)\n"
	"	 3 -- L2-regularized L1-loss support vector classification (dual)\n"
	"	 4 -- support vector classification by Crammer and Singer\n"
	"	 5 -- L1-regularized L2-loss support vector classification\n"
	"	 6 -- L1-regularized logistic regression\n"
	"	 7 -- L2-regularized logistic regression (dual)\n"
	"  for regression\n"
	"	11 -- L2-regularized L2-loss support vector regression (primal)\n"
	"	12 -- L2-regularized L2-loss support vector regression (dual)\n"
	"	13 -- L2-regularized L1-loss support vector regression (dual)\n"
	"-c cost : set the parameter C (default 1)\n"
	"-p epsilon : set the epsilon in loss function of SVR (default 0.1)\n"
	"-e epsilon : set tolerance of termination criterion\n"
	"	-s 0 and 2\n"
	"		|f'(w)|_2 <= eps*min(pos,neg)/l*|f'(w0)|_2,\n"
	"		where f is the primal function and pos/neg are # of\n"
	"		positive/negative data (default 0.01)\n"
	"	-s 11\n"
	"		|f'(w)|_2 <= eps*|f'(w0)|_2 (default 0.001)\n"
	"	-s 1, 3, 4, and 7\n"
	"		Dual maximal violation <= eps; similar to libsvm (default 0.1)\n"
	"	-s 5 and 6\n"
	"		|f'(w)|_1 <= eps*min(pos,neg)/l*|f'(w0)|_1,\n"
	"		where f is the primal function (default 0.01)\n"
	"	-s 12 and 13\n"
	"		|f'(alpha)|_1 <= eps |f'(alpha0)|,\n"
	"		where f is the dual function (default 0.1)\n"
	"-B bias : if bias >= 0, instance x becomes [x; bias]; if < 0, no bias term added (default -1)\n"
	"-wi weight: weights adjust the parameter C of different classes (see README for details)\n"
	"-v n: n-fold cross validation mode\n"
	"-q : quiet mode (no outputs)\n"
	);
	exit(1);
}

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

void parse_command_line(int argc, char **argv, char *input_file_name, char *model_file_name);
//void read_problem(const char *filename);
void do_cross_validation();
void read_problem(const char *posName, const char *negName);


struct feature_node *x_space;
struct parameter param;
struct problem prob;
struct model* model_;
int flag_cross_validation;
int nr_fold;
double bias;

int main(int argc, char **argv)
{
	//MPItrain <posNum> <negNum> <inputName> <modelName> <timeName>
	char posFile[1024];
	char negFile[1024];
	char modFile[1024];
	clock_t begin;
	clock_t end;
	double timeMess=0;
	double period=0;
	const char *error_msg;
	FILE *timeFile=fopen(argv[5],"w+");

	int posNum=atoi(argv[1]);
	int negNum=atoi(argv[2]);
	int myid, numprocs,src,type;
    
    //edit
    if (argc==7) type=atoi(argv[6]);
    else type=1;
    //edit end

	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	memset(posFile, 0, sizeof(char)*1024);
	sprintf(posFile,"%s%d.pos",argv[3],myid/negNum);

	memset(negFile, 0, sizeof(char)*1024);
	sprintf(negFile,"%s%d.neg",argv[3],myid%negNum);

	memset(modFile, 0, sizeof(char)*1024);
	sprintf(modFile,"%s%d.mod",argv[4],myid);
    //sprintf(modFile,"%s.model%d",argv[4],myid);//edit

	begin=clock();

	//MPItrain(posFile, negFile, modFile);
    MPItrain(posFile, negFile, modFile,type);//edit

	end=clock();

	period=(double)(end-begin)/CLOCKS_PER_SEC;

	printf("\nMODEL %d COMPLETED IN %lf SECS.\n",myid, period);

	if(myid!=0)
	{
		timeMess=period;
		MPI_Send(&timeMess,1,MPI_DOUBLE,0,99,MPI_COMM_WORLD);
	}
	else
	{
		fprintf(timeFile,"Process 0: %lf secs.\n",period);
		for(src=1;src<numprocs;src++)
		{
			MPI_Recv(&timeMess,1,MPI_DOUBLE,src,99,MPI_COMM_WORLD,&status);
			fprintf(timeFile,"Process %d: %lf secs.\n",src,timeMess);
		}
		fclose(timeFile);
	}

	MPI_Finalize();

	return 0;

}

//void set_para()
void set_para(int type)//edit
{
	param.solver_type = L2R_L2LOSS_SVC_DUAL;
	param.C = 1;
	param.eps = INF; // see setting below
	param.p = 0.1;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
    
	flag_cross_validation = 0;
	bias = -1;
    
    if (type==2) param.solver_type=PERCEPTRON;//edit
    else if(type==3) param.solver_type=LMS;
    
    //edit
    if(param.eps == INF)
    {
        switch(param.solver_type)
        {
            case L2R_LR:
            case L2R_L2LOSS_SVC:
                param.eps = 0.01;
                break;
            case L2R_L2LOSS_SVR:
                param.eps = 0.001;
                break;
            case L2R_L2LOSS_SVC_DUAL:
            case L2R_L1LOSS_SVC_DUAL:
            case MCSVM_CS:
            case L2R_LR_DUAL:
                param.eps = 0.1;
                break;
            case L1R_L2LOSS_SVC:
            case L1R_LR:
                param.eps = 0.01;
                break;
            case L2R_L1LOSS_SVR_DUAL:
            case L2R_L2LOSS_SVR_DUAL:
                param.eps = 0.1;
                break;
            //edit
            case PERCEPTRON:
            case LMS:
                param.eps=0.002;
                break;
            //edit end
        }
    }
    //edit end

}

//int MPItrain(char *posFile, char *negFile, char *modFile)
int MPItrain(char *posFile, char *negFile, char *modFile,int type)//edit
{
	//set_para();
    set_para(type);//edit
	read_problem(posFile,negFile);

	//error_msg = check_parameter(&prob,&param);

	//if(error_msg)
	//{
	//	fprintf(stderr,"ERROR: %s\n",error_msg);
	//	exit(1);
	//}

	//if(flag_cross_validation)
	//{
	//	do_cross_validation();
	//}
	//else
	//{
	model_=train(&prob, &param);
	if(save_model(modFile, model_))
	{
		fprintf(stderr,"can't save model to file %s\n",modFile);
		exit(1);
	}
	
	free_and_destroy_model(&model_);
	//}
	destroy_param(&param);
	free(prob.y);
	free(prob.x);
	free(x_space);
	free(line);

	return 0;
}


void do_cross_validation()
{
	int i;
	int total_correct = 0;
	double total_error = 0;
	double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
	double *target = Malloc(double, prob.l);

	cross_validation(&prob,&param,nr_fold,target);
	if(param.solver_type == L2R_L2LOSS_SVR ||
	   param.solver_type == L2R_L1LOSS_SVR_DUAL ||
	   param.solver_type == L2R_L2LOSS_SVR_DUAL)
	{
		for(i=0;i<prob.l;i++)
		{
			double y = prob.y[i];
			double v = target[i];
			total_error += (v-y)*(v-y);
			sumv += v;
			sumy += y;
			sumvv += v*v;
			sumyy += y*y;
			sumvy += v*y;
		}
		printf("Cross Validation Mean squared error = %g\n",total_error/prob.l);
		printf("Cross Validation Squared correlation coefficient = %g\n",
				((prob.l*sumvy-sumv*sumy)*(prob.l*sumvy-sumv*sumy))/
				((prob.l*sumvv-sumv*sumv)*(prob.l*sumyy-sumy*sumy))
			  );
	}
	else
	{
		for(i=0;i<prob.l;i++)
			if(target[i] == prob.y[i])
				++total_correct;
		printf("Cross Validation Accuracy = %g%%\n",100.0*total_correct/prob.l);
	}

	free(target);
}


// read in a problem (in libsvm format)
void read_problem(const char *posName, const char *negName)
{
	int max_index, inst_max_index, i;
	size_t elements, j;
	
	FILE *fp[2];

	fp[0] = fopen(posName,"r");
	fp[1] = fopen(negName,"r");

	char *endptr;
	char *idx, *val, *label;

	if(fp[0] == NULL)
	{
		fprintf(stderr,"can't open input file %s\n",posName);
		exit(1);
	}

	if(fp[1] == NULL)
	{
		fprintf(stderr,"can't open input file %s\n",negName);
		exit(1);
	}


	prob.l = 0;
	elements = 0;
	max_line_len = 1024;
	line = Malloc(char,max_line_len);

	int count=0;
	int lineNum[2]={0};

	for(count=0; count<2;count++)
	{
		lineNum[count]=0;

		while(readline(fp[count])!=NULL)
		{
			char *p = strtok(line," \t"); // label

			// features
			while(1)
			{
				p = strtok(NULL," \t");
				if(p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
					break;
				elements++;
			}
			elements++; // for bias term
			prob.l++;
			lineNum[count]++;
		}
	}

	rewind(fp[0]);
	rewind(fp[1]);

	prob.bias=bias;

	prob.y = Malloc(double,prob.l);
	prob.x = Malloc(struct feature_node *,prob.l);
	x_space = Malloc(struct feature_node,elements+prob.l);

	max_index = 0;
	j=0;
	for(i=0;i<prob.l;i++)
	{
		inst_max_index = 0; // strtol gives 0 if wrong format


		if(i<lineNum[0])
			readline(fp[0]);
		else
			readline(fp[1]);

		//readline(fp);
		
		prob.x[i] = &x_space[j];
		label = strtok(line," \t\n");
		if(label == NULL) // empty line
			exit_input_error(i+1);

		prob.y[i] = strtod(label,&endptr);
		if(endptr == label || *endptr != '\0')
			exit_input_error(i+1);

		while(1)
		{
			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;

			errno = 0;
			x_space[j].index = (int) strtol(idx,&endptr,10);
			if(endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
				exit_input_error(i+1);
			else
				inst_max_index = x_space[j].index;

			errno = 0;
			x_space[j].value = strtod(val,&endptr);
			if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				exit_input_error(i+1);

			++j;
		}

		if(inst_max_index > max_index)
			max_index = inst_max_index;

		if(prob.bias >= 0)
			x_space[j++].value = prob.bias;

		x_space[j++].index = -1;
	}

	if(prob.bias >= 0)
	{
		prob.n=max_index+1;
		for(i=1;i<prob.l;i++)
			(prob.x[i]-2)->index = prob.n;
		x_space[j-2].index = prob.n;
	}
	else
		prob.n=max_index;

	fclose(fp[0]);
	fclose(fp[1]);
}
