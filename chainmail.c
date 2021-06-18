#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>//have to -lm when compiling to actually use it
#include <getopt.h>
#include <unistd.h>//used for getopt, makes this linux-only

const double SWG_table[51]={0.324,0.300,0.276,0.252,0.232,0.212,0.192,0.176,0.160,0.144,0.128,0.116,0.104,0.092,0.080,0.72,0.064,0.056,0.048,0.040,0.036,0.032,0.028,0.024,0.022,0.020,0.018,0.0164,0.0148,0.0136,0.0124,0.0116,0.0108,0.0100,0.0092,0.0084,0.0076,0.0068,0.0060,0.0052,0.0048,0.0044,0.004,0.0036,0.0032,0.0028,0.0024,0.002,0.0016,0.0012,0.0010};//there is no formula for SWG. It is defined in inches
const char* argOptions="w:d:o:ih";//see documentation for option description
struct option longopts[]={
	{"inner-diameter",1,NULL,'d'},
	{"interactive",0,NULL,'i'},
	{"output",1,NULL,'o'},
	{"wire-diameter",1,NULL,'w'},
	{"help",0,NULL,'h'},
	{NULL, 0, NULL, 0}
};

double AWG_to_diameter(int AWG,int isInches);
double SWG_to_diameter(int SWG,int isInches);
int diameter_to_AWG(double diameter,int isInches);
int diameter_to_SWG(double diameter,int isInches);
int setDiamMeasures(char* unit,char* input);
int AWG=-1,SWG=-1;//these are global because used in setMeasures
double mmDiameter=-1,inDiameter=-1,inID=-1,mmID=-1,AR=-1;//mmDiameter is diameter of the metal, mmID is the inner space, AR is diameter/ID

//todo:put the above lines in a header file
int main(int argc, char* argv[]){
	if(argc < 2){
		printf("arguments needed\n");
		return 1;
	}
	char currArg;
	char* outputType;//use this later to figure out what to print out
	while((currArg = getopt_long(argc, argv, argOptions, longopts, NULL)) != -1){
		switch(currArg){
			case 'o':
				outputType = optarg;//optarg is set by getopt
				break;
			case 'w':
				if(argc>optind){//optind increases by 2 when taking an arg
					setDiamMeasures(argv[optind],argv[optind-1]);
					printf("Wire diameter %s %s given. ",argv[optind-1],argv[optind]);
				}else{
					printf("Not enough args for -w\n");
					exit(1);
				}
				optind++;//getopt only expect me to take 1 arg, not 2
				break;
			case 'd':
				if(argc>optind){
					printf("ID %s %s given. ",argv[optind-1],argv[optind]);
					if(strcmp(argv[optind],"in")==0){//didnt make function because semi-simple,maybe do later
						inID=strtod(argv[optind-1],NULL);
						if(inID==0&&(strcmp(argv[optind-1],"0")!=0||strcmp(argv[optind-1],"0.0")!=0)){
							printf("Incorrect input\n");
							exit(1);
						}
						mmID=round(1000*25.4*inID)/1000.0;
					}else if(strcmp(argv[optind],"mm")==0){
						mmID=strtod(argv[optind-1],NULL);
						if(mmID==0&&(strcmp(argv[optind-1],"0")!=0||strcmp(argv[optind-1],"0.0")!=0)){
							printf("Incorrect input\n");
							exit(1);
						}
						inID=round(1000*mmID/25.4)/1000.0;
					}else{
						printf("Unknown unit: %s\n",argv[optind]);
						exit(1);
					}
				}else{
					printf("Not enough args for -d\n");
					exit(1);
				}
				optind++;
				break;
			case 'h':
				printf("TODO: add help\n");
				break;
			case 'i':
				printf("TODO: add interactive mode\n");
				break;
			default:
				printf("Mistyped argument\n");
				break;
		}
	}
	if(mmID>0&&mmDiameter>0){//can calc AR
		AR=mmID/mmDiameter;
	}
	printf("\n");//newline after any printing of input
	if(outputType!=NULL){
		if(strcmp(outputType,"AWG")==0){
			if(AWG==-1){
				printf("Not enough info to convert to AWG\n");
				exit(1);
			}
			printf("That is equivilent to %d AWG.\n",AWG);
		}else if(strcmp(outputType,"SWG")==0){
			if(SWG==-1){
				printf("Not enough info to convert to SWG\n");
				exit(1);
			}
			printf("That is equivilent to %d SWG.\n",SWG);
		}else if(strcmp(outputType,"in")==0){
			if(inDiameter<0){
				if(inID<0){
					printf("Not enough info to convert to in\n");
					exit(1);
				}else{
				printf("That is equivilent to an ID of %.3lfin.\n",inID);
				}
			}else if(inID<0){
				printf("That is equivilent to a wire diameter of %.3lfin.\n",inDiameter);
			}else{
				printf("That is equivilent to an ID of %.3lfin, and a wire diameter of %.3lfin.\n",inID,inDiameter);
			}
		}else if(strcmp(outputType,"mm")==0){
			if(mmDiameter<0){
				if(mmID<0){
					printf("Not enough info to convert to mm\n");
					exit(1);
				}else{
				printf("That is equivilent to an ID of %.3lfmm.\n",mmID);
				}
			}else if(mmID<0){
				printf("That is equivilent to a wire diameter of %.3lfmm.\n",mmDiameter);
			}else{
				printf("That is equivilent to an ID of %.3lfmm, and a wire diameter of %.3lfmm.\n",mmID,mmDiameter);
			}
		}
		if(AR>0){//also might add an output suppress variable
			printf("This has an AR of %.2lf.\n",AR);//I've never seen an AR more precise than 2 digits
		}
	}
}

double AWG_to_diameter(int AWG,int isInches){//returns rounded to 4 decimal places, per the AWG standard
	if(isInches){
		return round(1000*0.005*(pow(92.0,(36-AWG)/39.0)))/1000.0;
	}else{
		return round(1000*0.127*(pow(92.0,(36-AWG)/39.0)))/1000.0;
	}
}
double SWG_to_diameter(int SWG,int isInches){
	if(SWG<=36&&SWG>=0){
		if(isInches){
			return SWG_table[SWG];
		}else{
			return SWG_table[SWG]*25.4;
	}
	printf("SWG not defined\n");
	return -1.0;
	}
}
int diameter_to_AWG(double diameter,int isInches){
	if(isInches){
		return round(-39*(log(diameter/0.005))/(log(92))+36);
	}else{
		return round(-39*(log(diameter/0.127))/(log(92))+36);
	}
}
int diameter_to_SWG(double diameter,int isInches){
	if(!isInches){//SWG is defined in inches, should really only pass inches to this
		diameter=diameter/2.54;//usually round, but here its a temp variable so slightly more precise not to
	}
	for(int i=1;i<51;i++){//51 is SWG table size. Starts at 1 bc upper bound, so comp 0 and 1
		if(SWG_table[i]<diameter){
			if(diameter-SWG_table[i]<SWG_table[i-1]-diameter){//which is closer
				return i;
			}else{
				return i-1;
			}
		}
	}
	printf("SWG not defined\n");
	return -1;
}
int setDiamMeasures(char* unit,char* input){
	if(strcmp(unit,"AWG")==0){
		AWG=strtol(input,NULL,10);//returns 0 if not convertible
		if(AWG==0&&strcmp(input,"0")!=0){
			printf("Incorrect input\n");
			exit(1);
		}
		mmDiameter=AWG_to_diameter(AWG,0);
		inDiameter=AWG_to_diameter(AWG,1);//not dividing mm to avoid rounding errors
		SWG=diameter_to_SWG(inDiameter,1);
		return 0;
	}else if(strcmp(unit,"SWG")==0){
		SWG=strtol(input,NULL,10);
		if(SWG==0&&strcmp(input,"0")!=0){
			printf("Incorrect input\n");
			exit(1);
		}
		mmDiameter=SWG_to_diameter(SWG,0);
		inDiameter=SWG_to_diameter(SWG,1);
		AWG=diameter_to_AWG(inDiameter,1);
		return 0;
	}else if(strcmp(unit,"in")==0){
		inDiameter=strtod(input,NULL);
		if(inDiameter==0&&(strcmp(input,"0")!=0||strcmp(input,"0.0")!=0)){
			printf("Incorrect input\n");
			exit(1);
		}
		mmDiameter=round(1000*25.4*inDiameter)/1000.0;
		inDiameter=round(1000*inDiameter)/1000.0;
		AWG=diameter_to_AWG(inDiameter,1);
		SWG=diameter_to_SWG(inDiameter,1);
		return 0;
	}else if(strcmp(unit,"mm")==0){
		mmDiameter=strtod(input,NULL);
		if(mmDiameter==0&&(strcmp(input,"0")!=0||strcmp(input,"0.0")!=0)){
			printf("Incorrect input\n");
			exit(1);
		}
		inDiameter=round(1000*mmDiameter/25.4)/1000.0;
		mmDiameter=round(1000*mmDiameter)/1000.0;
		AWG=diameter_to_AWG(inDiameter,1);
		SWG=diameter_to_SWG(inDiameter,1);
		return 0;
	}
	printf("Unknown unit: %s\n",unit);
	exit(1);
}
