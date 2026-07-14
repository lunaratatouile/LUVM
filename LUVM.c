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
    vm.MEM[0] = 0b00001100;
    vm.MEM[1] = 1;
    vm.MEM[2] = 0x68;
    vm.MEM[3] = 0b00001100;
    vm.MEM[4] = 2;
    vm.MEM[5] = 0x01;
    vm.MEM[6] = 0b00001101;
    vm.MEM[7] = 2;
    vm.MEM[8] = 1;
    vm.MEM[9] = 0b00001100;
    vm.MEM[10] = 1;
    vm.MEM[11] = 0x65;
    vm.MEM[12] = 0b00001100;
    vm.MEM[13] = 2;
    vm.MEM[14] = 0x01;
    vm.MEM[15] = 0b00001101;
    vm.MEM[16] = 2;
    vm.MEM[17] = 1;
    vm.MEM[18] = 0b00001100;
    vm.MEM[19] = 1;
    vm.MEM[20] = 0x6C;
    vm.MEM[21] = 0b00001100;
    vm.MEM[22] = 2;
    vm.MEM[23] = 0x01;
    vm.MEM[24] = 0b00001101;
    vm.MEM[25] = 2;
    vm.MEM[26] = 1;
    vm.MEM[27] = 0b00001100;
    vm.MEM[28] = 1;
    vm.MEM[29] = 0x6C;
    vm.MEM[30] = 0b00001100;
    vm.MEM[31] = 2;
    vm.MEM[32] = 0x01;
    vm.MEM[33] = 0b00001101;
    vm.MEM[34] = 2;
    vm.MEM[35] = 1;
    vm.MEM[36] = 0b00001100;
    vm.MEM[37] = 1;
    vm.MEM[38] = 0x6F;
    vm.MEM[39] = 0b00001100;
    vm.MEM[40] = 2;
    vm.MEM[41] = 0x01;
    vm.MEM[42] = 0b00001101;
    vm.MEM[43] = 2;
    vm.MEM[44] = 1;
    vm.MEM[45] = 0b00001100;
    vm.MEM[46] = 1;
    vm.MEM[47] = 0x20;
    vm.MEM[48] = 0b00001100;
    vm.MEM[49] = 2;
    vm.MEM[50] = 0x01;
    vm.MEM[51] = 0b00001101;
    vm.MEM[52] = 2;
    vm.MEM[53] = 1;
    vm.MEM[54] = 0b00001100;
    vm.MEM[55] = 1;
    vm.MEM[56] = 0x77;
    vm.MEM[57] = 0b00001100;
    vm.MEM[58] = 2;
    vm.MEM[59] = 0x01;
    vm.MEM[60] = 0b00001101;
    vm.MEM[61] = 2;
    vm.MEM[62] = 1;
    vm.MEM[63] = 0b00001100;
    vm.MEM[64] = 1;
    vm.MEM[65] = 0x6F;
    vm.MEM[66] = 0b00001100;
    vm.MEM[67] = 2;
    vm.MEM[68] = 0x01;
    vm.MEM[69] = 0b00001101;
    vm.MEM[70] = 2;
    vm.MEM[71] = 1;
    vm.MEM[72] = 0b00001100;
    vm.MEM[73] = 1;
    vm.MEM[74] = 0x72;
    vm.MEM[75] = 0b00001100;
    vm.MEM[76] = 2;
    vm.MEM[77] = 0x01;
    vm.MEM[78] = 0b00001101;
    vm.MEM[79] = 2;
    vm.MEM[80] = 1;
    vm.MEM[81] = 0b00001100;
    vm.MEM[82] = 1;
    vm.MEM[83] = 0x6C;
    vm.MEM[84] = 0b00001100;
    vm.MEM[85] = 2;
    vm.MEM[86] = 0x01;
    vm.MEM[87] = 0b00001101;
    vm.MEM[88] = 2;
    vm.MEM[89] = 1;
    vm.MEM[90] = 0b00001100;
    vm.MEM[91] = 1;
    vm.MEM[92] = 0x64;
    vm.MEM[93] = 0b00001100;
    vm.MEM[94] = 2;
    vm.MEM[95] = 0x01;
    vm.MEM[96] = 0b00001101;
    vm.MEM[97] = 2;
    vm.MEM[98] = 1;


    //vm.MEM[99] = 0b00000001;

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
