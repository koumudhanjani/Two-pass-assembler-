/* emuc
 * Author: Anjani koumudh
 * User ID: [Your User ID]
 * Declaration of Authorship: This is my own work, written for CS2102.
 */
#include <stdio.h>
#include <stdlib.h>

#define MAXMEM 10000//max words of memory for emulator

int mem[MAXMEM];
int A=0,B=0,PC=0,SP=0;

int main(int argc,char* argv[]){
    if(argc<2){//to ensure object file is provided
        printf("Usage: %s <object_file.o>\n",argv[0]);
        return 1;
    }
    FILE* fin=fopen(argv[1],"rb");
    if(!fin){//open object file
        printf("Error: Could not open file %s\n",argv[1]);
        return 1;
    }

    int cnt=0;//load 32 bit integers
    while(fread(&mem[cnt],sizeof(int),1,fin)==1){
        cnt++;
        if(cnt>=MAXMEM) break;
    }
    fclose(fin);
    //produce memory dump
    printf("=== Memory Dump ===\n");
    for(int i=0; i<cnt; i++) printf("%04X  %08X\n",i,mem[i]);
    printf("===================\n\n");
    printf("Executing program...\n");

    //execute loop
    while(1){
        if(PC<0||PC>=MAXMEM){
            printf("RUNTIME ERROR: Program Counter out of bounds (%d)\n",PC);
            return 1;
        }
        int inst=mem[PC];
        int op=inst&0xFF;          //bottom 8 bits for op code
        int arg=(inst>>8)&0xFFFFFF; //upper 24 bits for operand
       //sign extension for operand
        if(arg&0x800000) arg|=0xFF000000;
        PC++;
        //execute the operation
        switch(op){
            case 0: B=A; A=arg; break;//ldc
            case 1: A=A+arg; break;//adc
            case 2: 
                if(SP+arg<0||SP+arg>=MAXMEM){
                    printf("RUNTIME ERROR: Memory access out of bounds at SP+%d\n",arg);
                    return 1;}
                B=A; A=mem[SP+arg]; break;//ldl
            case 3: 
                if(SP+arg<0||SP+arg>=MAXMEM){
                    printf("RUNTIME ERROR: Memory access out of bounds at SP+%d\n",arg);
                    return 1;}
                mem[SP+arg]=A; A=B; break;//stl
            case 4: 
                if(A+arg<0||A+arg>=MAXMEM){
                    printf("RUNTIME ERROR: Memory access out of bounds at A+%d\n",arg);
                    return 1;}
                A=mem[A+arg]; break;//ldnl
            case 5: 
                if(A+arg<0||A+arg>=MAXMEM){
                    printf("RUNTIME ERROR: Memory access out of bounds at A+%d\n",arg);
                    return 1;}
                mem[A+arg]=B; break;//stnl
            case 6: A=B+A; break;
            case 7: A=B-A; break;
            case 8: A=B<<A; break;
            case 9: A=B>>A; break;
            case 10: SP=SP+arg; break;//adj
            case 11: SP=A; A=B; break;//a2sp
            case 12: B=A; A=SP; break;//sp2a
            case 13: B=A; A=PC; PC=PC+arg; break;//call
            case 14: PC=A; A=B; break;//return
            case 15: if(A==0) PC=PC+arg; break;//brz
            case 16: if(A<0) PC=PC+arg; break;//brlz
            case 17: PC=PC+arg; break;//br
            case 18: //halt
                printf("Program execution halted successfully.\n");
                printf("Final State: A = %d, B = %d, PC = %d, SP = %d\n",A,B,PC,SP);
                return 0;
            case 19: A=B*A; break;
            case 20: 
                if(A==0){
                    printf("RUNTIME ERROR: Division by zero\n");
                    return 1;}
                A=B/A;
                break;
            default:
                printf("RUNTIME ERROR: Unknown opcode %d at PC %d\n",op,PC-1);
                return 1;
        }
    }
    return 0;
}