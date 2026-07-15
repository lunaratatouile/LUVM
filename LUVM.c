#include <stdio.h>

/*
 *
 * !!!!!!!!!! UTILISE DES UINT BORDEL DE MERDE LOUP !!!!!!!!!!!
 * 00000000 nop
 * 00000001 halt
 * 00000010 mov wREG iREG
 * 00000011 add wREG 1REG 2REG
 * 00000100 sub wREG 1REG 2REG
 * 00000101 and wREG 1REG 2REG
 * 00000110 or wREG 1REG 2REG
 * 00000111 xor wREG 1REG 2REG
 * 00001000 jmp ADDR
 * 00001001 jz REG ADDR
 * 00001010 equ wREG 1REG 2REG
 * 00001011 shft wREG direction(0=down else up)
 * 00001100 ldi wREG imm
 * 00001101 deo deviceREG valueREG (device 0 is console and port 0 is write)
 * 00001110 call ADDR
 * 00001111 ret returns to the last address in the stack
 */

typedef unsigned char uint8;
typedef unsigned short uint16;
void exit(int status);
typedef struct{
   uint8 MEM[0x10000];
   uint8 DEV[0x100];
   uint8 REG[8];
   uint16 PC;
   uint16 STK[256];
   uint8 stkcount;
   int RUN;
}VM;

void pushstk(uint16 value,VM *vm){
    vm->STK[vm->stkcount]=value;
    vm->stkcount++;
}

uint16 popstk(VM *vm){
    uint16 res = vm->STK[vm->stkcount-1];
    vm->stkcount--;
    return res;
}

void errorREG(uint8 reg){
    if (reg>7){
        printf("\nERROR:(index out of range) You are accessing an inexistant register, there is only 8 register so from 0 to 7.");
        exit(1);
    };
}

void handle_deo(uint8 port,uint8 value){
    switch(port){
        case 0x01:
            fputc(value, stdout);
            return;
    };
}

void OpRead(char opcode,VM *vm){
    switch(opcode){
        case 0b00000000:
            break;
        case 0b00000001:
            vm->RUN=0;
            break;
        case 0b00000010:
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            uint8 wr=vm->MEM[vm->PC];
            vm->PC++;
            vm->REG[wr]=vm->REG[vm->MEM[vm->PC]];
            break;
        case 0b00000011:
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            wr = vm->MEM[vm->PC];
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            uint8 r1 = vm->MEM[vm->PC];
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            uint8 r2 = vm->MEM[vm->PC];
            vm->REG[wr]=vm->REG[r1]+vm->REG[r2];
            break;
        case 0b00000100:
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            wr = vm->MEM[vm->PC];
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            r1 = vm->MEM[vm->PC];
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            r2 = vm->MEM[vm->PC];
            vm->REG[wr]=vm->REG[r1]-vm->REG[r2];
            break;
        case 0b00000101:
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            wr = vm->MEM[vm->PC];
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            r1 = vm->MEM[vm->PC];
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            r2 = vm->MEM[vm->PC];
            vm->REG[wr]=vm->REG[r1]&vm->REG[r2];
            break;
        case 0b00000110:
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            wr = vm->MEM[vm->PC];
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            r1 = vm->MEM[vm->PC];
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            r2 = vm->MEM[vm->PC];
            vm->REG[wr]=vm->REG[r1]|vm->REG[r2];
            break;
        case 0b00000111:
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            wr = vm->MEM[vm->PC];
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            r1 = vm->MEM[vm->PC];
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            r2 = vm->MEM[vm->PC];
            vm->REG[wr]=vm->REG[r1]^vm->REG[r2];
            break;
        case 0b00001000:
            vm->PC++;
            uint16 addr=vm->MEM[vm->PC];
            vm->PC++;
            addr = (addr << 8) | vm->MEM[vm->PC];
            vm->PC=addr-1;
            break;
        case 0b00001001:
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            r1 = vm->REG[vm->MEM[vm->PC]];
            vm->PC++;
            if (r1==0){
                addr=vm->MEM[vm->PC];
                vm->PC++;
                addr = (addr << 8) | vm->MEM[vm->PC];
                vm->PC=addr-1;
            };
            break;
        case 0b00001010:
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            wr = vm->MEM[vm->PC];
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            r1 = vm->MEM[vm->PC];
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            r2 = vm->MEM[vm->PC];
            if (vm->REG[r1]==vm->REG[r2]){
                vm->REG[wr]=1;
            } else {
                vm->REG[wr]=0;
            }
            break;
        case 0b00001011:
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            wr = vm->MEM[vm->PC];
            vm->PC++;
            if (vm->MEM[vm->PC]==0){
                vm->REG[wr]=(uint8)(vm->REG[wr] >> 1);
            }
            else{
                vm->REG[wr]=(uint8)(vm->REG[wr] << 1);
            }
            break;
        case 0b00001100:
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            wr = vm->MEM[vm->PC];
            vm->PC++;
            vm->REG[wr]=vm->MEM[vm->PC];
            break;
        case 0b00001101:
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            r1 = vm->MEM[vm->PC];
            vm->PC++;
            errorREG(vm->MEM[vm->PC]);
            r2 = vm->MEM[vm->PC];
            vm->DEV[vm->REG[r1]]=vm->REG[r2];
            handle_deo(vm->REG[r1],vm->REG[r2]);
            break;
        case 0b00001110:
            pushstk(vm->PC+3,vm);
            vm->PC++;
            addr=vm->MEM[vm->PC];
            vm->PC++;
            addr = (addr << 8) | vm->MEM[vm->PC];
            vm->PC=addr-1;
            break;
        case 0b00001111:
            addr=popstk(vm);
            vm->PC=addr-1;
            break;
        }
}

int
main(int argc, char *argv[])
{

    VM vm;
    for(int i = 0; i < 0x10000; i++) {
            vm.MEM[i] = 0;
        }
    for(int i = 0; i < 8; i++) {
            vm.REG[i] = 0;
        }
    for(int i = 0; i < 256; i++) {
            vm.STK[i] = 0;
            vm.DEV[i] = 0;
        }
    vm.stkcount=0;

    FILE *f;
	if(argc < 2)
		return fprintf(stdout, "usage: %s file.lun\n", argv[0]);
	else if(!(f = fopen(argv[1], "rb")))
		return fprintf(stderr, "%s: %s not found.\n", argv[0], argv[1]);
	fread(&vm.MEM[0x00], 0xff00, 1, f), fclose(f);

    vm.RUN=1;
    vm.PC=0;
    while (vm.RUN){
        OpRead(vm.MEM[vm.PC],&vm);
        if (vm.PC!=0xffff){
            vm.PC++;
        }
        else{
            printf("\nERROR:program counter out of bound");
            exit(1);
        }
    }
}
