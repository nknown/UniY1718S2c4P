#include "AppServFunc.h"

//CentralServer Comamnds
	void set_ds(int x){
		sprintf(myBuffer,"SET_DS %i;%i;%s;%i\n",x,id,ip,upt);
		mysend(udp_fp,SC_addr);
	}
	void Ans_set_ds(){
		int n;
		myrecv(udp_fp,SC_addr);
		if (0!=sscanf(myBuffer,"OK %i;",&n) && n==id) {AppState.state=s_ds_ok;AppState.ds=true;}
	}
	void withdraw_ds(int x){
		sprintf(myBuffer,"WITHDRAW_DS %i;%i\n",x,id);
		mysend(udp_fp,SC_addr);
		myrecv(udp_fp,SC_addr);//o servidor Central responde! Existe erro
		AppState.ds=false;
	}
	void set_start (int x){
		sprintf(myBuffer,"SET_START %i;%i;%s;%i\n",x,id,ip,tpt);//verify tpt
		mysend(udp_fp,SC_addr);
		AppState.state=s_s;
	}
	void Ans_set_start(){
		int n;
		myrecv(udp_fp,SC_addr);
		if (0!=sscanf(myBuffer,"OK %i;",&n) && n==id) {AppState.state=s_s_ok;AppState.ss=true;}
	}
	void withdraw_start(int x){
		sprintf(myBuffer,"WITHDRAW_START %i;%i\n",x,id);
		mysend(udp_fp,SC_addr);
		myrecv(udp_fp,SC_addr);
		AppState.ss=false;
	}
	void get_start(int x){
		int addrlen =(int) sizeof(SC_addr);
		sprintf(myBuffer,"GET_START %i;%i\n",x,id);
		mysend(udp_fp,SC_addr);
		AppState.state=g_s;
		return;
	}
	int Ans_get_start(){
		int n;
		myrecv(udp_fp,SC_addr);
		if (0!=sscanf(myBuffer,"OK %i;%i;%s;%i\n",&n,&Oid,Oip,&Otpt) && n==id){
			fprintf(stderr,"AnsData:\n\tid: %i\n\tip %s\n\ttpt %i\n",Oid,Oip,Otpt);
			return 1;
		}
		fprintf(stderr,"Err WrongMsg\n");
		return 0;
	}
	void serv_start(){
		fprintf(stderr,">>Start Exists [%d@%s:%d]\n",Oid,Oip,Otpt);	
	}
//Exit & Eror

	void myerr(int code,char* msg){
		fprintf(stderr,"Err_%d",code);
		if (msg!=NULL) fprintf(stderr,":%s",msg);
		fprintf(stderr,"\n");
		myfpClose();
		exit(-code);
	}


	void myfpClose(){
		if (udp_fp>=0){ close(udp_fp);udp_fp=-1;}
		if (tcp_fd>=0){ close(tcp_fd);tcp_fd=-1;}
		return;
	}	
//Start SetUp
	void appConfg ( int argc,char **argv){
	//arg is a vector of string,
	//arg last pointer must be NULL
	int i=0;
	char *SName=DV_csName;//CentralServer Name
	cspt=DV_cspt;
	int argCheck = 0;
	while (argv[i]!=NULL){
		if (strcmp(argv[i],"-n")==0 && argc>i){
			i++;
			argCheck++;
			id=atoi(argv[i]);
		}else	if (strcmp(argv[i],"-j")==0 && argc>i){
			i++;
			argCheck++;
			ip=argv[i];
		}else   if (strcmp(argv[i],"-u")==0 && argc>i){
                        i++;
			argCheck++;
			upt=atoi(argv[i]);
                }else   if (strcmp(argv[i],"-t")==0 && argc>i){
                        i++;
			argCheck++;
			tpt=atoi(argv[i]);
                }
		if (strcmp(argv[i],"-i")==0 && argc>i){
			i++;
			SName=argv[i];
		}else	if (strcmp(argv[i],"-p")==0 && argc>i){
			i++;
			cspt=atoi(argv[i]);
		}
		i++;
	}
	//check arguments for service
	#ifdef app_service
	if(argCheck!=4)myerr(1,"missing arguments");
	#endif
	//get CentralServer IP
	csh = gethostbyname(SName);
	if (csh==NULL) myerr(2,"Fail to gethostbyname");
	csa=(struct in_addr*)csh->h_addr_list[0];
	//Show Info
	fprintf(stderr,"Service:\n\tid: %d\n\tip: %s\n\tupt: %d\n\ttpt: %d\n",id,ip,upt,tpt);
	fprintf(stderr,"\nCentralServer:\n\t<%s:%d>\n",SName,cspt);
	fprintf(stderr,"\tofficial host name: %s\n",csh->h_name);
	fprintf(stderr,"\tinternet address: %s (%08lX)\n",inet_ntoa(*csa),(long unsigned int)ntohl(csa->s_addr));
	return;
	}