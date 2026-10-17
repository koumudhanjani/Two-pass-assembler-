/* asmc
 * Author: [Your Name]
 * User ID: [Your User ID]
 * Declaration of Authorship: This is my own work, written for CS2102.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXL 2000
#define MAXSYM 1000
#define MAXLOG 500

typedef struct {
    char name[20];
    int code;
    int arg;
} OpDef;

OpDef optab[]={//instructins table
    {"ldc",0,1}, {"adc",1,1}, {"ldl",2,1}, {"stl",3,1},
    {"ldnl",4,1}, {"stnl",5,1}, {"add",6,0}, {"sub",7,0},
    {"shl",8,0}, {"shr",9,0}, {"adj",10,1}, {"a2sp",11,0},
    {"sp2a",12,0}, {"call",13,1}, {"return",14,0}, {"brz",15,1},
    {"brlz",16,1}, {"br",17,1}, {"halt",18,0}, {"HALT",18,0},
    {"mul",19,0}, {"div",20,0}, {"data",-1,1}, {"SET",-2,1}
};
int opcnt=24;

typedef struct {//for labels
    char name[50];
    int addr;
    int used;
    int defline;
} Symbol;

Symbol syms[MAXSYM];
int symcnt=0;

typedef struct {//for err and warn
    int line;
    char type[10];
    char text[256];
} Log;

Log logs[MAXLOG];
int logcnt=0;

void addlog(int l,const char* t,const char* msg){
    if(logcnt>=MAXLOG) return;
    logs[logcnt].line=l;
    strcpy(logs[logcnt].type,t);
    strcpy(logs[logcnt].text,msg);
    logcnt++;;//for unknown instructions
}

int getop(char* mnem){
    for(int i=0;i<opcnt;i++){
        if(strcmp(optab[i].name,mnem)==0) return optab[i].code;
    }
    return -99;
}

int getaddr(char* name, int* out){//uses pass by reference,returns 0or1 for existence
    for(int i=0;i<symcnt;i++){
        if(strcmp(syms[i].name,name)==0){
            if(out) *out=syms[i].addr;
            return 1;
        }
    }
    return 0;
}

void markused(char* name){
    for(int i=0;i<symcnt;i++){
        if(strcmp(syms[i].name,name)==0){
            syms[i].used=1;
            return;
        }
    }
}

int validid(char* name){//for label name
    if(!isalpha(name[0])) return 0;
    for(int i=1; name[i]!='\0'; i++){
        if(!isalnum(name[i])&&name[i]!='_') return 0;
    }
    return 1;
}

int isnum(char* s){//checks if string is a valid decimal/hex/octal integer
    if(*s=='-'||*s=='+') s++;
    if(*s=='\0') return 0;
    char* end;
    strtol(s,&end,0);
    return (*end=='\0');
}

int main(int argc,char* argv[]){
    if(argc<2) return 1;
    FILE* src=fopen(argv[1],"r");
    if(!src) return 1;//if file cant be opened

    char raw[MAXL][256];//reads all lines
    int lines=0;
    while(lines<MAXL && fgets(raw[lines],256,src)) lines++;
    fclose(src);
    int pcmap[MAXL];
    int pc=0,errs=0,lastset=0;
    /*===========
     *pass1
     *===========*/
    for(int i=0; i<lines; i++){
        char buf[256],lastlbl[50]="";
        strcpy(buf,raw[i]);
        char* cmt=strchr(buf,';');
        if(cmt) *cmt='\0';//to remove comments

        char* col=strchr(buf,':');
        char* ptr=buf;
        if(col){//for label
            *col='\0';
            char* tok=strtok(buf," \t\n");
            if(tok){
                if(!validid(tok)){
                    char e[100]; sprintf(e,"invalid label '%s'",tok);
                    addlog(i+1,"ERR",e); errs=1;
                }else if(getaddr(tok,NULL)){
                    char e[100]; sprintf(e,"duplicate label '%s'",tok);
                    addlog(i+1,"ERR",e); errs=1;
                }else if(symcnt>=MAXSYM){
                    addlog(i+1,"ERR","maximum symbol count exceeded"); errs=1;
                }else{
                    strcpy(syms[symcnt].name,tok);
                    syms[symcnt].addr=pc;
                    syms[symcnt].used=0;
                    syms[symcnt].defline=i+1;
                    strcpy(lastlbl,tok);
                    symcnt++;
                }
            }
            ptr=col+1;
        }

        pcmap[i]=pc;
        char* tok=strtok(ptr," \t\n");
        if(tok){
            if(strcmp(tok,"SET")==0){//for set directives
                char* vstr=strtok(NULL," \t\n");
                int valid=0;
                if(vstr&&isnum(vstr)){
                    lastset=(int)strtol(vstr,NULL,0);//converts into ineger
                    valid=1;
                }
                if(lastlbl[0]=='\0'){
                    addlog(i+1,"ERR","SET must follow a label"); errs=1;
                }else{
                    syms[symcnt-1].addr=lastset;
                    if(!valid){ addlog(i+1,"ERR","invalid SET value"); errs=1; }
                }
            }else pc++;//standard instruction
        }
    }

    char pfx[256],lst[256],obj[256],log[256];
    strncpy(pfx,argv[1],strcspn(argv[1],".")); pfx[strcspn(argv[1],".")]='\0';
    sprintf(lst,"%s.lst",pfx); sprintf(obj,"%s.o",pfx); sprintf(log,"%s.log",pfx);
    //p2type ,0=skip,1=instruction,2=err format
    int p2type[MAXL]={0},p2pc[MAXL]={0},p2mc[MAXL]={0};
    char p2str[MAXL][256]={{0}};
    /*=========
     *pass2*
     *=========*/
    for(int i=0; i<lines; i++){
        char buf[256],clean[256];
        strcpy(buf,raw[i]); buf[strcspn(buf,"\n")]='\0';
        strcpy(clean,raw[i]);
        
        char* cptr=strchr(clean,';'); if(cptr) *cptr='\0';
        char* colptr=strchr(clean,':');
        char* st=colptr?colptr+1:clean;
        while(*st==' '||*st=='\t') st++;
        char* en=st+strlen(st)-1;
        while(en>=st&&(*en==' '||*en=='\t'||*en=='\n'||*en=='\r')){ *en='\0'; en--; }
        strcpy(p2str[i],st);

        char* cmt=strchr(buf,';'); if(cmt) *cmt='\0';
        char* col=strchr(buf,':');
        char* ptr=buf;
        if(col){ *col='\0'; ptr=col+1; }

        char* tok=strtok(ptr," \t\n");
        if(!tok){ p2type[i]=0;continue; }//for empty line

        int op=getop(tok);
        int currpc=pcmap[i];
        p2pc[i]=currpc;

        if(op==-99){//for unknown operand
            char e[100]; sprintf(e,"unknown mnemonic '%s'",tok);
            addlog(i+1,"ERR",e); errs=1;
            p2type[i]=2; continue;
        }
        if(op==-2){ p2type[i]=0;continue; }//for set directives

        char* arg=strtok(NULL,"");
        if(arg){//extract operand
            while(*arg==' '||*arg=='\t') arg++;
            char* aen=arg+strlen(arg)-1;
            while(aen>=arg&&(*aen==' '||*aen=='\t'||*aen=='\n'||*aen=='\r')){ *aen='\0'; aen--; }
            if(*arg=='\0') arg=NULL;
        }

        int val=0;
        for(int j=0;j<opcnt;j++){
            if(optab[j].code==op){
                if(optab[j].arg&&!arg){ addlog(i+1,"ERR","missing operand"); errs=1; }
                else if(!optab[j].arg&&arg){ addlog(i+1,"ERR","unexpected operand"); errs=1; }
                break;
            }
        }

        if(arg){
            if(op==-1&&!isnum(arg)){
                addlog(i+1,"ERR","invalid DATA value"); errs=1;
            }else if(!isnum(arg)){//if operand is text,then label(resolve it)
                int addr;
                if(!getaddr(arg,&addr)){
                    char e[100]; sprintf(e,"undefined label '%s'",arg);
                    addlog(i+1,"ERR",e); errs=1;
                }else{
                    markused(arg);
                    if(op>=13&&op<=17){
                        val=addr-(currpc+1);
                        if(val==-1) addlog(i+1,"WARN","possible infinite loop (backward jump)");
                    }else val=addr;
                }
            }else val=(int)strtol(arg,NULL,0);
        }

        if(op!=-1&&op!=-2){//excepet for data and set
            if(val<-8388608||val>8388607){
                addlog(i+1,"ERR","operand out of 24-bit range"); errs=1;
            }
        }
        //unsigned for shifting negative numbers
        int mc=(((unsigned int)val)<<8)|(op&0xFF);
        if(op==-1) mc=val;
        p2mc[i]=mc; p2type[i]=1;
    }

    for(int i=0;i<symcnt;i++){
        if(!syms[i].used){
            char w[100]; sprintf(w,"Label '%s' defined but never used",syms[i].name);
            int tgt=syms[i].defline;
            for(int k=0; k<lines; k++){
                if(p2type[k]==1&&p2pc[k]==syms[i].addr){ tgt=k+1; break; }
            }
            addlog(tgt,"WARN",w);
        }
    }
    //file output generation
    FILE* flst=fopen(lst,"w");
    FILE* fobj=fopen(obj,"wb");
    if(!flst || !fobj) return 1;//if not created(disk full)
    
    for(int i=0;i<lines;i++){
        if(p2type[i]==0) continue;
        char msg[256]="";
        int punused=0; 

        for(int j=0; j<logcnt; j++){
            if(logs[j].line==i+1){
                char t[150];
                if(strcmp(logs[j].type,"WARN")==0){
                     if(strncmp(logs[j].text,"Label",5)==0){
                         if(punused) continue;
                         punused=1; sprintf(t,"  %s: %s",logs[j].type,logs[j].text);
                     }else sprintf(t,"  %s: Line %d: %s",logs[j].type,logs[j].line,logs[j].text);
                }else sprintf(t,"  %s: Line %d: %s",logs[j].type,logs[j].line,logs[j].text);
                strcat(msg,t);
            }
        }

        if(p2type[i]==2){//write to .lst file
            fprintf(flst,"%04x  %08X  %s%s\n",p2pc[i],0,p2str[i],msg);
        }else if(p2type[i]==1){
            fprintf(flst,"%04x  %08X  %s%s\n",p2pc[i],p2mc[i],p2str[i],msg);
            fwrite(&p2mc[i],sizeof(int),1,fobj);
        }
    }
    fclose(flst);
    fclose(fobj);
    //for .log file
    FILE* flog=fopen(log,"w");
    if(!flog) return 1;

    fprintf(flog,"=== Assemble output ===\n");
    if(errs){
        fprintf(flog,"ERRORS:\n");
        for(int i=0; i<logcnt; i++){
            if(strcmp(logs[i].type,"ERR")==0) fprintf(flog,"ERR: Line %d: %s\n",logs[i].line,logs[i].text);
        }
    }
    
    int warns=0;
    for(int i=0;i<logcnt;i++) if(strcmp(logs[i].type,"WARN")==0) warns=1;
    if(warns){
        fprintf(flog,"--- warnings ---\n");
        for(int i=0; i<logcnt; i++){
            if(strcmp(logs[i].type,"WARN")==0){
                if(strncmp(logs[i].text,"Label",5)==0) fprintf(flog,"WARN: %s\n",logs[i].text);
                else fprintf(flog,"WARN: Line %d: %s\n",logs[i].line,logs[i].text);
            }
        }
    }
    
    fprintf(flog,"--- labels ---\n");
    for(int i=0;i<symcnt;i++) fprintf(flog,"%s : %d\n",syms[i].name,syms[i].addr);
    if(!errs) fprintf(flog,"Program assembled.\n");
    fclose(flog);

    return 0;
}