#include "stdio.h"
#include "string.h"
#include "math.h"
#include "stdlib.h"
#include "stdbool.h"

int lineNumber = 1;
int count = 0;
int row = 0;
char reg1[20];
char reg2[20];
char reg3[20];
char lineBuffer[110];
char pc2[10];
int pc;
int arraypc[20];
int arraypc2[20];
int org = 1;
int org1 = 0;
int org2 = 0;
int one = 1;


struct symTabRow {
    char label[100];
    char tokenType[10];
    char value[20];
};
struct symTabRow symbolTable[2000];

struct codeRow {
    int opcode;
    int offset;
    int as;
    int ad;
    int r1;
    int r2;
    int bw;
    int source;
    int dst;
};
struct codeRow codeTable[2000];

bool isComment();
bool readOneLine();
bool getLabel();
bool isEmpty();
bool isOpcode();
bool addrmode(char a[]);
bool addrmode2(char a[]);
int writeOneLine();
int findvalue();
int getpc();
int orgpc();
int findpcvalue();
int getcode();
int getr1value();
void getregister(char a[]);
void getr3value();
void modereg();
void getas();
void getreg4();
void getregister2(char a[]);
void modereg2();
void getad();
void checkerror();


FILE* rdFile;
FILE* outFile;
FILE* object;

int main()
{
    char a[] = "";
    printf("Please enter filename\n");
    scanf("%s",a);
    rdFile=fopen(a,"r");
    if(rdFile==NULL)
    {
        printf("Unable to open the file!Please make sure the filename is correct\n");
        exit(1);
    }
    else
    {
        printf("open file succeed!\n");
    }
    outFile=fopen("outlist.lst","w");
    object=fopen("object.txt","w");
    fprintf (object, "YuanchaoLi00");
    while(readOneLine()){                                                        //first pass, determine symbol
        getLabel();
        if(!isComment()){
            if (strstr(lineBuffer, " ORG") != NULL){
                arraypc2[org1++] = pc - arraypc[org2++];
            }
        }
        orgpc();
        getpc();

    }
    fseek(rdFile, 0, SEEK_SET);                                            //second pass, determine code
    while(readOneLine()){
        writeOneLine();
        orgpc();
        getcode();
        if(isComment()||isEmpty()||strstr(lineBuffer," EQU") != NULL)
            fprintf (outFile, "%3d:  %s",lineNumber++, lineBuffer);
        else if(isOpcode()&&codeTable[row - 1].r1!=-1&&codeTable[row - 1].r2!=-1)
            fprintf (outFile, "%3d:  %04X  %X%X%X%X%04X%04X %s",lineNumber++,pc,codeTable[row - 1].ad+codeTable[row - 1].bw+codeTable[row - 1].as,codeTable[row - 1].dst,codeTable[row - 1].opcode,codeTable[row - 1].source,(codeTable[row - 1].r1<<8|codeTable[row - 1].r1>>8)&0xFFFF,(codeTable[row - 1].r2<<8|codeTable[row - 1].r2>>8)&0xFFFF,lineBuffer);
        else if(isOpcode()&&codeTable[row - 1].r1==-1&&codeTable[row - 1].r2!=-1)
            fprintf (outFile, "%3d:  %04X  %X%X%X%X%04X     %s",lineNumber++,pc,codeTable[row - 1].ad+codeTable[row - 1].bw+codeTable[row - 1].as,codeTable[row - 1].dst,codeTable[row - 1].opcode,codeTable[row - 1].source,(codeTable[row - 1].r2<<8|codeTable[row - 1].r2>>8)&0xFFFF,lineBuffer);
        else if(isOpcode()&&codeTable[row - 1].r1!=-1&&codeTable[row - 1].r2==-1)
            fprintf (outFile, "%3d:  %04X  %X%X%X%X%04X     %s",lineNumber++,pc,codeTable[row - 1].ad+codeTable[row - 1].bw+codeTable[row - 1].as,codeTable[row - 1].dst,codeTable[row - 1].opcode,codeTable[row - 1].source,(codeTable[row - 1].r1<<8|codeTable[row - 1].r1>>8)&0xFFFF,lineBuffer);
        else if(isOpcode()&&codeTable[row - 1].r1==-1&&codeTable[row - 1].r2==-1)
            fprintf (outFile, "%3d:  %04X  %X%X%X%X         %s",lineNumber++,pc,codeTable[row - 1].ad+codeTable[row - 1].bw+codeTable[row - 1].as,codeTable[row - 1].dst,codeTable[row - 1].opcode,codeTable[row - 1].source,lineBuffer);
        else
            fprintf (outFile, "%3d:  %04X                   %s",lineNumber++,pc,lineBuffer);
        getpc();
        if(!isComment()){
            if (strstr(lineBuffer, " ORG") != NULL){
                fprintf (object, "FF00AA55%04X%04d",pc,arraypc2[one++]);
            }
            if(isOpcode()&&codeTable[row - 1].r1!=-1&&codeTable[row - 1].r2!=-1)
                fprintf (object, "%X%X%X%X%04X%04X",codeTable[row - 1].ad+codeTable[row - 1].bw+codeTable[row - 1].as,codeTable[row - 1].dst,codeTable[row - 1].opcode,codeTable[row - 1].source,(codeTable[row - 1].r1<<8|codeTable[row - 1].r1>>8)&0xFFFF,(codeTable[row - 1].r2<<8|codeTable[row - 1].r2>>8)&0xFFFF);
            else if(isOpcode()&&codeTable[row - 1].r1==-1&&codeTable[row - 1].r2!=-1)
                fprintf (object, "%X%X%X%X%04X",codeTable[row - 1].ad+codeTable[row - 1].bw+codeTable[row - 1].as,codeTable[row - 1].dst,codeTable[row - 1].opcode,codeTable[row - 1].source,(codeTable[row - 1].r2<<8|codeTable[row - 1].r2>>8)&0xFFFF);
            else if(isOpcode()&&codeTable[row - 1].r1!=-1&&codeTable[row - 1].r2==-1)
                fprintf (object, "%X%X%X%X%04X",codeTable[row - 1].ad+codeTable[row - 1].bw+codeTable[row - 1].as,codeTable[row - 1].dst,codeTable[row - 1].opcode,codeTable[row - 1].source,(codeTable[row - 1].r1<<8|codeTable[row - 1].r1>>8)&0xFFFF);
            else if(isOpcode()&&codeTable[row - 1].r1==-1&&codeTable[row - 1].r2==-1)
                fprintf (object, "%X%X%X%X",codeTable[row - 1].ad+codeTable[row - 1].bw+codeTable[row - 1].as,codeTable[row - 1].dst,codeTable[row - 1].opcode,codeTable[row - 1].source);
        }
        checkerror();
    }
    fprintf (object, "FF00AA55");
    fprintf (outFile, "\n");
    fprintf (outFile, "                               LIST OF SYMBOYS\n");
    fprintf (outFile, "============================================================================\n");
    fprintf (outFile, "SYMBOL                               TYPE                   VALUE\n");
    fprintf (outFile, "----------------------------------------------------------------------------\n");
    count = 0;
    for(int i = 0; i < 100; i++) {
        fprintf(outFile, "%-25s            %-10s             %s\n", symbolTable[count].label,symbolTable[count].tokenType,symbolTable[count].value);
        count++;
    }
    fclose(rdFile);
    return 0;
}

bool readOneLine() {

    int i;
    // clear line buffer before we start
    for(i = 0;i < 100;i++)
        lineBuffer[i] = 0;

    if(fgets(lineBuffer,100,rdFile) != NULL) {
        return true;
    }
    else
        return false; //error condition
}
int writeOneLine() {

    printf("%d %s\n",lineNumber,lineBuffer);


    return 0;
}
bool isComment(){
    int i = 0;
    while(lineBuffer[i] == ' '){
        i++;
    }
    if(lineBuffer[i] == ';'){
        return true;
    }
    else{
        return false;
    }
}
bool isEmpty(){
    int i = 0;
    while(lineBuffer[i] == ' '){
        i++;
    }
    if(lineBuffer[i] == '\n'){
        printf("IS Empty\n");
        return true;
    }
    else{
        printf("not empty\n");
        return false;
    }
}
bool getLabel() {
    if ((lineBuffer[0] >= 'A') && (lineBuffer[0] <= 'Z') || (lineBuffer[0] >= 'a') && (lineBuffer[0] <= 'z')) {
        int i = 0;
        while(lineBuffer[i] != ' '&& lineBuffer[i] != '\n'&& lineBuffer[i] != ':'){
            i++;
        }
        strncpy(symbolTable[count].label,lineBuffer,i);
        if(strstr(lineBuffer," EQU") != NULL){
            strcpy(symbolTable[count].tokenType, "Constant");
            findvalue();
        }else
        if(strstr(lineBuffer," DB") != NULL){
            strcpy(symbolTable[count].tokenType, "DB");
            findvalue();
        }else
        if(strstr(lineBuffer," DS") != NULL){
            strcpy(symbolTable[count].tokenType, "DS");
            findvalue();
        }else{
            strcpy(symbolTable[count].tokenType, "Address");
            sprintf(symbolTable[count].value,"%X",pc); // print hex
        }
        for(int k =0; k < count; k++){
            if(strstr(symbolTable[count].value, symbolTable[k].label) != NULL){
                strncpy(symbolTable[count].value,symbolTable[k].value,20);
            }
        }
        count++;
    }
    return 0;
}
int findvalue(){
    int k = 0;
    int m = 0;
    while(lineBuffer[m] != ' ')
        m++;
    while(lineBuffer[m] == ' ')
        m++;
    while(lineBuffer[m] != ' ')
        m++;
    while(lineBuffer[m] == ' ')
        m++;
    while(lineBuffer[m] != '\n'&& lineBuffer[m] != ';'&& lineBuffer[m] != ' '){
        if(lineBuffer[m] == '$')
            m+=4;
        symbolTable[count].value[k] = lineBuffer[m];
        m++;
        k++;
    }
return 1;
}
int getr1value(){
    int k = 0;
    int m = 0;
    for(int i = 0; i < 20;i++){
        reg1[i] = 0;
    }
    while(lineBuffer[m] != ' ')
        m++;
    while(lineBuffer[m] == ' ')
        m++;
    while(lineBuffer[m] != ' ')
        m++;
    while(lineBuffer[m] == ' ')
        m++;
    while(lineBuffer[m] != '\n'&& lineBuffer[m] != ';'&& lineBuffer[m] != ' '&& lineBuffer[m] != ','){
        reg1[k] = lineBuffer[m];
        m++;
        k++;
    }
    return 1;
}

int orgpc(){
    if(!isComment()){
        if (strstr(lineBuffer, " ORG") != NULL) {
            findpcvalue();
            pc = strtol(pc2, NULL, 16); //char to hex value
            arraypc[org++] = pc;
        }
    }
    return 1;
}
int getpc() {                                                           //calculate pc

    if (!isComment()) {
        if (strstr(lineBuffer, " DB") != NULL) {
            pc += 1;
        }
        if (strstr(lineBuffer, " DW") != NULL) {
            pc += 2;
        }
        if (strstr(lineBuffer, " #") != NULL) {
            pc += 2;
        }
        if (strstr(lineBuffer, "&") != NULL) {
            pc += 2;
        }

        if (strstr(lineBuffer, "(R") != NULL) {
            pc += 2;
        }
        if (strstr(lineBuffer, " DS") != NULL) {
            pc += atoi(symbolTable[count - 1].value);
        }
        if(isOpcode()) {
            pc += 2;
        }
        printf("pc is %x\n", pc);
        return pc;
    }
}
int findpcvalue() {
    int m = 0;
    while (lineBuffer[m] == ' ')
        m++;
    while (lineBuffer[m] != ' ')
        m++;
    while (lineBuffer[m] == ' ')
        m++;
    for(int k = 0; k < 6; k++) {
        pc2[k] = lineBuffer[m];
m++;
    }
}
int getcode(){                                              //determine code table based on different opcodes
    if(isOpcode()) {
        if(strstr(lineBuffer, ".b") != NULL)
            codeTable[row].bw = 4;
        else
            codeTable[row].bw = 0;
        if (strstr(lineBuffer, " add ") != NULL) {
            codeTable[row].opcode = 5;
            getr1value();
            getas();
            addrmode("#");
            addrmode("&");
            modereg();
            getreg4();
            getad();
            addrmode2("&");
            modereg2();

        }
        if (strstr(lineBuffer, " cmp ") != NULL) {
            codeTable[row].opcode = 9;
            getr1value();
            getas();
            addrmode("#");
            addrmode("&");
            modereg();
            getreg4();
            getad();
            addrmode2("&");
            modereg2();

        }
        if (strstr(lineBuffer, " and") != NULL) {
            codeTable[row].opcode = 15;
            getr1value();
            getas();
            addrmode("#");
            addrmode("&");
            modereg();
            getreg4();
            getad();
            addrmode2("&");
            modereg2();

        }
        if (strstr(lineBuffer, " bic") != NULL) {
            codeTable[row].opcode = 12;
            getr1value();
            getas();
            addrmode("#");
            addrmode("&");
            modereg();
            getreg4();
            getad();
            addrmode2("&");
            modereg2();

        }
        if (strstr(lineBuffer, " bis") != NULL) {
            codeTable[row].opcode = 13;
            getr1value();
            getas();
            addrmode("#");
            addrmode("&");
            modereg();
            getreg4();
            getad();
            addrmode2("&");
            modereg2();

        }
        if (strstr(lineBuffer, " bit") != NULL) {
            codeTable[row].opcode = 11;
            getr1value();
            getas();
            addrmode("#");
            addrmode("&");
            modereg();
            getreg4();
            getad();
            addrmode2("&");
            modereg2();

        }
        if (strstr(lineBuffer, " call") != NULL) {
            codeTable[row].opcode = 1;
            codeTable[row].dst = 1;
            codeTable[row].r2 = -1;
            getr1value();
            addrmode("#");
            getas();
            codeTable[row].ad = 0;
            codeTable[row].source = 2;

        }
        if (strstr(lineBuffer, " dec") != NULL) {
            codeTable[row].opcode = 4;
            codeTable[row].source = 3;
            codeTable[row].r1 = -1;
            codeTable[row].as = 0;
            getr1value();
            getad();
            modereg2();
        }
        if (strstr(lineBuffer, " inc") != NULL) {
            codeTable[row].opcode = 4;
            codeTable[row].source = 3;
            codeTable[row].r1 = -1;
            codeTable[row].as = 0;
            getr1value();
            getad();
            modereg2();
        }
        if (strstr(lineBuffer, " jz") != NULL) {
            codeTable[row].opcode = 2;
            codeTable[row].r1 = -1;
            codeTable[row].r2 = -1;
            getr1value();
            for (int k = 0; k < count; k++) {
                if (strstr(reg1, symbolTable[k].label) != NULL) {
                    strncpy(reg1, symbolTable[k].value, 20);
                    codeTable[row].offset = strtol(reg1, NULL, 16);
                    codeTable[row].offset = codeTable[row].offset - 2 - pc;
                    codeTable[row].source = 4 + (codeTable[row].offset>>8 & 3);
                    codeTable[row].ad = 8*(codeTable[row].offset>>7 & 1);
                    codeTable[row].bw = 4*(codeTable[row].offset>>6 & 1);
                    codeTable[row].as = codeTable[row].offset>>4 & 3;
                    codeTable[row].dst = codeTable[row].offset & 15;
                }
            }

        }
        if (strstr(lineBuffer, " jnz") != NULL) {
            codeTable[row].opcode = 2;
            codeTable[row].r1 = -1;
            codeTable[row].r2 = -1;
            getr1value();
            for (int k = 0; k < count; k++) {
                if (strstr(reg1, symbolTable[k].label) != NULL) {
                    strncpy(reg1, symbolTable[k].value, 20);
                    codeTable[row].offset = strtol(reg1, NULL, 16);
                    codeTable[row].offset = codeTable[row].offset - 2 - pc;
                    codeTable[row].source = codeTable[row].offset>>8 & 3;
                    codeTable[row].ad = 8*(codeTable[row].offset>>7 & 1);
                    codeTable[row].bw = 4*(codeTable[row].offset>>6 & 1);
                    codeTable[row].as = codeTable[row].offset>>4 & 3;
                    codeTable[row].dst = codeTable[row].offset & 15;
                }
            }
        }
        if (strstr(lineBuffer, " mov") != NULL) {
            codeTable[row].opcode = 4;
            getr1value();
            getas();
            addrmode("#");
            addrmode("&");
            modereg();
            getreg4();
            getad();
            addrmode2("&");
            modereg2();

        }
        if (strstr(lineBuffer, " ret") != NULL) {
            codeTable[row].opcode = 1;
            codeTable[row].source = 3;
            codeTable[row].r1 = -1;
            codeTable[row].as = 0;
            codeTable[row].ad = 0;
            codeTable[row].r2 = -1;
            codeTable[row].dst = 0;
        }
        if (strstr(lineBuffer, " push") != NULL) {
            codeTable[row].opcode = 1;
            codeTable[row].source = 2;
            codeTable[row].r1 = -1;
            getr1value();
            getas();
            modereg2();
            codeTable[row].r2 = -1;
        }
        if (strstr(lineBuffer, " pop") != NULL) {
            codeTable[row].opcode = 4;
            codeTable[row].source = 1;
            codeTable[row].r1 = -1;
            codeTable[row].as = 3;
            getr1value();
            getad();
            addrmode2("&");
            modereg2();
        }
        if (strstr(lineBuffer, " swpb") != NULL) {
            codeTable[row].opcode = 1;
            codeTable[row].source = 0;
            codeTable[row].r1 = -1;
            getr1value();
            getas();
            modereg2();
            codeTable[row].r2 = -1;
        }
        row++;
    }
}
bool isOpcode(){
    if(!isComment()){
    if (strstr(lineBuffer, " mov") != NULL || strstr(lineBuffer, " inc") != NULL || \
            strstr(lineBuffer, " dec") != NULL || strstr(lineBuffer, " jnz") != NULL || \
strstr(lineBuffer, " ret") != NULL || strstr(lineBuffer, " add") != NULL || strstr(lineBuffer, " and") != NULL || \
            strstr(lineBuffer, " jz") != NULL\
 || strstr(lineBuffer, " inc") != NULL || strstr(lineBuffer, " bic") != NULL || strstr(lineBuffer, " bis") != NULL || \
            strstr(lineBuffer, " bit") != NULL \
 || strstr(lineBuffer, " call") != NULL || strstr(lineBuffer, " cmp") != NULL || strstr(lineBuffer, " push") != NULL || \
            strstr(lineBuffer, " pop") != NULL \
 || strstr(lineBuffer, " swpb") != NULL){
        return true;
    }}
      return false;
}
bool addrmode(char a[]){
    int no = 1;
    if (strstr(reg1, a) != NULL) {
        int m = 0;
        for(int i = 0; i < 20;i++){
            reg2[i] = 0;
        }
        if (strstr(reg1, a) != NULL)
            m++;
        for(int k = 0; m < 20;k++) {
            reg2[k] = reg1[m];
            m++;
        }
        for (int k = 0; k < count; k++) {
            char temp[20] = "";
            if (strstr(reg2, symbolTable[k].label) != NULL) {
                strncpy(temp, symbolTable[k].value, 20);
                if(strstr(lineBuffer,"+")!=NULL) {
                    codeTable[row].r1 += strtol(temp, NULL, 16);
                    codeTable[row].source = 0;
                }
                else {
                    codeTable[row].r1 = strtol(temp, NULL, 16);
                    codeTable[row].source = 0;
                }
                no = 0;
            }
        }
        if (no == 1) {
            codeTable[row].r1 = strtol(reg2, NULL, 16);
            codeTable[row].source = 0;
        }
    }
    return true;
}
void getregister(char a[]){                              //determine register
    if (strstr(a, "R0") != NULL)
        codeTable[row].source = 0;
    if (strstr(a, "R1") != NULL)
        codeTable[row].source = 1;
    if (strstr(a, "R2") != NULL)
        codeTable[row].source = 2;
    if (strstr(a, "R3") != NULL)
        codeTable[row].source = 3;
    if (strstr(a, "R4") != NULL)
        codeTable[row].source = 4;
    if (strstr(a, "R5") != NULL)
        codeTable[row].source = 5;
    if (strstr(a, "R6") != NULL)
        codeTable[row].source = 6;
    if (strstr(a, "R7") != NULL)
        codeTable[row].source = 7;
    if (strstr(a, "R8") != NULL)
        codeTable[row].source = 8;
    if (strstr(a, "R9") != NULL)
        codeTable[row].source = 9;
    if (strstr(a, "R10") != NULL)
        codeTable[row].source = 10;
    if (strstr(a, "R11") != NULL)
        codeTable[row].source = 11;
    if (strstr(a, "R12") != NULL)
        codeTable[row].source = 12;
    if (strstr(a, "R13") != NULL)
        codeTable[row].source = 13;
    if (strstr(a, "R14") != NULL)
        codeTable[row].source = 14;
    if (strstr(a, "R15") != NULL)
        codeTable[row].source = 15;
}
void getr3value(){
    int i = 0,j = 0;
    while(reg1[i] != '('){
        reg3[i] = reg1[i];
        i++;
    }
}
void modereg(){
    if (strstr(reg1, "@") != NULL){
            int m = 0;
            for(int i = 0; i < 20;i++){
                reg2[i] = 0;
            }
        if (strstr(reg1, "@") != NULL)
                m++;
            for(int k = 0; m < 20;k++) {
                reg2[k] = reg1[m];
                m++;
            }
        getregister(reg2);
        codeTable[row].r1 = -1;
    }
    if (strstr(reg1, "@") == NULL&&strstr(reg1, "#") == NULL&&strstr(reg1, "&") == NULL&&strstr(reg1, "(R") == NULL){
        getregister(reg1);
        codeTable[row].r1 = -1;
    }
    if (strstr(reg1, "(R") != NULL){
        getr3value();
        codeTable[row].r1 = strtol(reg3, NULL, 16);
        getregister(reg1);
    }
}
void getas(){                                         //determine AS bits based on different address mode
    if(strstr(reg1, "(R") != NULL)
        codeTable[row].as = 1;
    else
    if(strstr(reg1, "@") != NULL)
        codeTable[row].as = 2;
    else
    if(strstr(reg1, "#") != NULL)
        codeTable[row].as = 3;
    else
    if(strstr(reg1, "&") != NULL)
        codeTable[row].as = 1;
    else
        codeTable[row].as = 0;
}
void getreg4(){
    int k = 0;
    int m = 0;
    for(int i = 0; i < 20;i++){
        reg1[i] = 0;
    }
    while(lineBuffer[m] != ' ')
        m++;
    while(lineBuffer[m] == ' ')
        m++;
    while(lineBuffer[m] != ' ')
        m++;
    while(lineBuffer[m] == ' ')
        m++;
    while(lineBuffer[m] != ',')
        m++;
    m+=1;
    while(lineBuffer[m] == ' ')
        m++;
    while(lineBuffer[m] != '\n'&& lineBuffer[m] != ';'&& lineBuffer[m] != ' '&& lineBuffer[m] != ','){
        reg1[k] = lineBuffer[m];
        m++;
        k++;
    }
}
bool addrmode2(char a[]){
    int no = 1;
    if (strstr(reg1, a) != NULL) {
        int m = 0;
        for(int i = 0; i < 20;i++){
            reg2[i] = 0;
        }
        if (strstr(reg1, a) != NULL)
            m++;
        for(int k = 0; m < 20;k++) {
            reg2[k] = reg1[m];
            m++;
        }
        for (int k = 0; k < count; k++) {
            char temp[20] = "";
            if (strstr(reg2, symbolTable[k].label) != NULL) {
                strncpy(temp, symbolTable[k].value, 20);
                if(strstr(lineBuffer,"+")!=NULL) {
                    codeTable[row].r2 += strtol(temp, NULL, 16);
                    codeTable[row].dst = 0;
                }
                else {
                    codeTable[row].r2 = strtol(temp, NULL, 16);
                    codeTable[row].dst = 0;
                }
                no = 0;
            }
        }
        if (no == 1) {
            codeTable[row].r2 = strtol(reg2, NULL, 16);
            codeTable[row].dst = 0;
        }
    }
    return true;
}
void getregister2(char a[]){
    if (strstr(a, "R0") != NULL)
        codeTable[row].dst = 0;
    if (strstr(a, "R1") != NULL||strstr(a, "SP") != NULL)
        codeTable[row].dst = 1;
    if (strstr(a, "R2") != NULL)
        codeTable[row].dst = 2;
    if (strstr(a, "R3") != NULL)
        codeTable[row].dst = 3;
    if (strstr(a, "R4") != NULL)
        codeTable[row].dst = 4;
    if (strstr(a, "R5") != NULL)
        codeTable[row].dst = 5;
    if (strstr(a, "R6") != NULL)
        codeTable[row].dst = 6;
    if (strstr(a, "R7") != NULL)
        codeTable[row].dst = 7;
    if (strstr(a, "R8") != NULL)
        codeTable[row].dst = 8;
    if (strstr(a, "R9") != NULL)
        codeTable[row].dst = 9;
    if (strstr(a, "R10") != NULL)
        codeTable[row].dst = 10;
    if (strstr(a, "R11") != NULL)
        codeTable[row].dst = 11;
    if (strstr(a, "R12") != NULL)
        codeTable[row].dst = 12;
    if (strstr(a, "R13") != NULL)
        codeTable[row].dst = 13;
    if (strstr(a, "R14") != NULL)
        codeTable[row].dst = 14;
    if (strstr(a, "R15") != NULL)
        codeTable[row].dst = 15;
}
void modereg2(){
    if (strstr(reg1, "@") == NULL&&strstr(reg1, "#") == NULL&&strstr(reg1, "&") == NULL&&strstr(reg1, "(R") == NULL){
        getregister2(reg1);
        codeTable[row].r2 = -1;
    }
    if (strstr(reg1, "(R") != NULL){
        getr3value();
        codeTable[row].r2 = strtol(reg3, NULL, 16);
        getregister2(reg1);
    }
}
void getad(){
    if(strstr(reg1, "(R") != NULL)
        codeTable[row].ad = 8;
    else
    if(strstr(reg1, "&") != NULL)
        codeTable[row].ad = 8;
    else
        codeTable[row].ad = 0;
}
void checkerror(){
    if(strstr(lineBuffer, "ORG") != NULL)
        if(strstr(lineBuffer, "0x") == NULL)
            fprintf (outFile, "ERROR - expected Assembler Directive parameter\n");
    if(strstr(lineBuffer, " dec ") != NULL||strstr(lineBuffer, " inc ") != NULL) {
        if (strstr(lineBuffer, ",") != NULL)
            fprintf(outFile, "ERROR - opcode expects one operand, found two operands\n");
        if(strstr(lineBuffer, "R") == NULL)
            fprintf(outFile, "ERROR - expected OPERAND\n");
    }
    if(lineBuffer[0] == ' ')
        if(lineBuffer[1]!=' ')
            fprintf(outFile, "ERROR - label doesn't start in column 1\n");
    if(strstr(lineBuffer, "bis.b") != NULL)
        if(strstr(lineBuffer, "&") == NULL)
            fprintf(outFile, "ERROR - expected OPERAND\n");
    if(strstr(lineBuffer, "_?") != NULL)
        fprintf(outFile, "ERROR - label not valid\n");
    if(strstr(lineBuffer, "dec.c") != NULL)
        fprintf(outFile, "ERROR - opcode not valid\n");
}

