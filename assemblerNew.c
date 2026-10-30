#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX 100
int first=0;

struct InstructionInfo {
    char* opcode;
    char* func3;
    char* func7;
    char* rd;
    char* rs1;
    char* rs2;
}info;

typedef struct {
    char label[MAX];
    int lineNumber;
} LabelInfo;
LabelInfo labelInfo[12];

void immediate(int num);
void writeInstructionToFile(const char *outputFileChoice, const char *filename, int *instruction, int size);
void processFile(char **filename);
char checkType(char* instr);
char* registerMap(char* instr);

// Function to convert a binary array to a hexadecimal string
char * binaryToHex(int *binary, int size, char *hex) {
    unsigned int binaryval = 0;

    for (int i = 0; i < size; i++) {
        binaryval = (binaryval << 1) | binary[i];
    }

    sprintf(hex, "0x%08X", binaryval);
    // printf("Hexadecimal: %s\n", hex);
    return hex;
    // printf("%s\n");
}

int matchLabel(char * instr, int labelCount)
{
    
    for (int i = 0; i < labelCount; i++) {
        if (strcmp(labelInfo[i].label, instr) == 0) { // Use strcmp to compare strings
            int num = labelInfo[i].lineNumber;
            return num;
        }
    }
    return -1; // Return -1 if no match is found
}

int main(int argc, char **argv) {
    
    processFile(argv);
    return 0;
}


// Function to open, read, process the file, and tokenize each line
void processFile(char **filename) {
    const char *outputFile = filename[2];
    const char *outputFileChoice = filename[3]; //choice either hex or binary
    FILE* filePointer = fopen(filename[1], "r");
    if (filePointer == NULL) {
        perror("Error: ");
        return;
    }
    
    FILE* filePointerForLabel = fopen(filename[1], "r");
    if (filePointerForLabel == NULL) {
        perror("Error: ");
        return;
    }

    char buf[MAX];
    char delimiter[] = " ,:()";
    char *instr;
    char* copy;

    
    int labelCount = 0;
    int lineNumberC = 0;
    int firstline=0;

    // Read through the file line by line
    while (fgets(buf, MAX, filePointerForLabel) != NULL) {
        // Check if the line contains a label terminated by ':'
        if(firstline==0)
        {
            lineNumberC+=0;
            firstline++;
        }
        else {
            lineNumberC+=4;
        }
        char *colonPos = strchr(buf, ':');
        if (colonPos != NULL) {
            // Temporarily terminate the string at the colon
            *colonPos = '\0';
            // Save the label and line number
            strncpy(labelInfo[labelCount].label, buf, MAX);
            labelInfo[labelCount].lineNumber = lineNumberC;
            labelCount++;
            // Avoid overflow of the labelInfo array
            if (labelCount >= 12) {
                break;
            }
        } else {
            // Move to the next line if no colon is found
            continue;
        }
        
    }

    fclose(filePointerForLabel);


    int lineNumberLabel = 0;
    while (!feof(filePointer)) {
        int count=1;
        if (fgets(buf, MAX, filePointer) != NULL) {
            // printf(" %s", buf);
            instr = strtok(buf, delimiter);
            char typeOfInstr = checkType(instr);
            if(typeOfInstr=='N')
                printf(" %s", "Error(might be a label)");
            else if(typeOfInstr=='R')
                {
                    while (instr != NULL && count<=3) {
                        if (count == 1) {
                            instr = strtok(NULL, delimiter); // Move to the next token
                            info.rd = registerMap(instr); // Extracting rd
                        } else if (count == 2) {
                            instr = strtok(NULL, delimiter); // Move to the next token
                            info.rs1 = registerMap(instr); // Extracting rs1
                        } 
                        else if(count == 3) {
                            instr = strtok(NULL, delimiter); // Move to the next token
                            instr[strcspn(instr, "\n")] = '\0'; 
                            info.rs2 = registerMap(instr); // Extracting rs2
                        }
                        count++;
                        
                    }
                    
                    char* result = (char*)malloc(32 * sizeof(char));
                    // Initialize the result with an empty string
                    result[0] = '\0';
                    
                    // Concatenate the strings
                    strcat(result, info.func7);
                    strcat(result, info.rs2);
                    strcat(result, info.rs1);
                    strcat(result, info.func3);
                    strcat(result, info.rd);
                    strcat(result, info.opcode);
                    
                    
                    //conversion from ascii (string) to integer
                    int instruction[32];
                    for(int i=0; i<32; i++)
                    {
                        instruction[i]=result[i]-48;
                    }
                    writeInstructionToFile(outputFileChoice, outputFile, instruction, 32);
                   
                } 
            else if(typeOfInstr=='I')
                {
                    while (instr != NULL && count<=3) {
                        if (count == 1) {
                            instr = strtok(NULL, delimiter); // Move to the next token
                            info.rd = registerMap(instr); // Extracting rd
                        } else if (count == 2) {
                            if(info.opcode=="0000011")
                            {
                                instr = strtok(NULL, delimiter); // Move to the next token
                                int num = atoi(instr);
                                immediate(num); // Extracting immediate bits
                            }
                            else{
                                instr = strtok(NULL, delimiter); // Move to the next token
                                info.rs1 = registerMap(instr); // Extracting rs1
                            }
                        } 
                        else if(count == 3) {
                            instr = strtok(NULL, delimiter); // Move to the next token
                            instr[strcspn(instr, "\n")] = '\0'; 
                            if(info.opcode=="0000011")
                            {
                                info.rs1 = registerMap(instr); // Extracting rs1
                            }
                            else{
                                int num = atoi(instr);
                                immediate(num); // Extracting rs2
                            }
                            
                        }
                        count++;
                    }
                    
                    char* result = (char*)malloc(32 * sizeof(char));
                    // Initialize the result with an empty string
                    result[0] = '\0';
                    // Concatenate the strings
                    if((info.func3=="001" && info.opcode=="0010011") || (info.func3=="101" && info.opcode=="0010011"))
                    {
                        strcat(result, info.func7);
                        strcat(result, info.rs2);
                        strcat(result, info.rs1);
                        strcat(result, info.func3);
                        strcat(result, info.rd);
                        strcat(result, info.opcode);
                    }
                    else{
                        strcat(result, info.rs2);
                        strcat(result, info.rs1);
                        strcat(result, info.func3);
                        strcat(result, info.rd);
                        strcat(result, info.opcode);
                    }
                    //conversion from ascii (string) to integer
                    int instruction[32];
                    for(int i=0; i<32; i++)
                    {
                        instruction[i]=result[i]-48;
                    }
                    writeInstructionToFile(outputFileChoice, outputFile, instruction, 32);
                   
                } 
            else if(typeOfInstr=='S')
            {
                while (instr != NULL && count<=3) {
                    if (count == 1) {
                        instr = strtok(NULL, delimiter); // Move to the next token
                        info.rs2 = registerMap(instr); // Extracting rd
                    } else if (count == 2) {
                        instr = strtok(NULL, delimiter); // Move to the next token
                        int num = atoi(instr);
                        immediate(num); // Extracting immediate bits
                    } 
                    else if(count == 3) {
                        instr = strtok(NULL, delimiter); // Move to the next token
                        instr[strcspn(instr, "\n")] = '\0';
                        info.rs1 = registerMap(instr); // Extracting rs1
                        
                    }
                    count++;
                }
                
                char* result = (char*)malloc(32 * sizeof(char));
                // Initialize the result with an empty string
                result[0] = '\0';
                // Concatenate the strings
                // Extract imm[11:5] and imm[4:0]
                char imm_upper[8]; // 7 bits + 1 for null terminator
                char imm_lower[6]; // 5 bits + 1 for null terminator

                strncpy(imm_upper, info.rd, 7); // Copy the first 7 bits (imm[11:5])
                imm_upper[7] = '\0'; // Null terminate

                strncpy(imm_lower, info.rd + 7, 5); // Copy the next 5 bits (imm[4:0])
                imm_lower[5] = '\0'; // Null terminate
                // printf("%s\n", imm_lower);
                // printf("%s\n", imm_upper);
                // Concatenate the strings in the required order
                strcat(result, imm_upper);  // imm[11:5]
                strcat(result, info.rs2);   // rs2
                strcat(result, info.rs1);   // rs1
                strcat(result, info.func3); // func3
                strcat(result, imm_lower);  // imm[4:0]
                strcat(result, info.opcode);// opcode
            
                
                //conversion from ascii (string) to integer
                int instruction[32];
                for(int i=0; i<32; i++)
                {
                    instruction[i]=result[i]-48;
                }
                writeInstructionToFile(outputFileChoice, outputFile, instruction, 32);
                
            } 
            else if(typeOfInstr=='B')
            {
                while (instr != NULL && count<=3) {
                    if (count == 1) {
                        instr = strtok(NULL, delimiter); // Move to the next token
                        info.rs1 = registerMap(instr); // Extracting rd
                    } 
                    else if (count == 2) {        
                        instr = strtok(NULL, delimiter); // Move to the next token
                        info.rs2 = registerMap(instr); // Extracting rd
                        } 
                    else if(count == 3) {
                        instr = strtok(NULL, delimiter); // Move to the next token
                        instr[strcspn(instr, "\n")] = '\0';
                        int lineToJump = matchLabel(instr, labelCount);
                        lineToJump=lineToJump-lineNumberLabel;
                        immediate(lineToJump);

                    }
                    count++;
                    
                }
                
                int lineNumberLabel = 0;
                 char* result = (char*)malloc(32 * sizeof(char));
                // Initialize the result with an empty string
                result[0] = '\0';
                // Concatenate the strings
                // Extract imm[11:5] and imm[4:0]
                char imm_upper[6]; // 7 bits + 1 for null terminator
                char imm_lower[6]; // 5 bits + 1 for null terminator
                char imm_middle1[2]; // 1 bit + 1 for null terminator
                char imm_middle2[5]; // 4 bits + 1 for null terminator
                char imm_final[14];  // 13 bits + 1 for null terminator

                // Extract the immediate parts
                strncpy(imm_upper, info.rd + 1, 6); // Copy the first 7 bits (imm[10:5])
                imm_upper[6] = '\0'; 

                strncpy(imm_lower, info.rd + 8, 4); // Copy the next 4 bits (imm[4:1])
                imm_lower[4] = '\0'; 

                strncpy(imm_middle1, info.rd + 1, 1); // Copy the next bit (imm[11])
                imm_middle1[1] = '\0'; 

                strncpy(imm_middle2, info.rd, 1); // Copy the next bit (imm[12])
                imm_middle2[1] = '\0'; 

                // Concatenate the strings in the required order
                strcpy(result, imm_middle2); // imm[12]
                strcat(result, imm_upper);   // imm[10:5]
                strcat(result, info.rs2);    // rs2
                strcat(result, info.rs1);    // rs1
                strcat(result, info.func3);  // func3
                strcat(result, imm_lower);   // imm[4:1]
                strcat(result, imm_middle1); // imm[11]
                strcat(result, info.opcode); // opcode

                //conversion from ascii (string) to integer
                int instruction[32];
                for(int i=0; i<32; i++)
                {
                    instruction[i]=result[i]-48;
                }
                writeInstructionToFile(outputFileChoice, outputFile, instruction, 32);
                
            }
             else if(typeOfInstr=='U')
            {
                char imm_upper[20]; // 20 bits + 1 for null terminator
                imm_upper[0] = '\0'; // Initialize with null terminator
                while (instr != NULL && count<=2) {
                    if (count == 1) {
                        instr = strtok(NULL, delimiter); // Move to the next token
                        info.rd = registerMap(instr); // Extracting rd
                    } 
                    else if(count == 2) {
                        instr = strtok(NULL, delimiter); // Move to the next token
                        instr=instr+2; 
                        instr[strcspn(instr, "\n")] = '\0';
                        char abc;
                        for (int i = 0; instr[i] != '\0'; i++) {
                            abc=instr[i];
                            // int num = atoi(abc);
                            immediate(abc);
                            if(i==0)
                                strncpy(imm_upper, info.rs2, 4);
                            else
                            {
                                strcat(imm_upper, info.rs2);     
                            }
                        }
                        imm_upper[20] = '\0'; // Null terminate

                        // Extract the immediate parts (20 bits from imm starting at bit 12, which is imm[31:12])
                        

                    }
                    count++;
                    
                }
                
                char* result = (char*)malloc(32 * sizeof(char));
                // Initialize the result with an empty string
                result[0] = '\0';
                // Concatenate the strings
                // Extract imm[11:5] and imm[4:0]

                // Concatenate the strings in the required order
                strcpy(result, imm_upper);   // imm[31:12]
                strcat(result, info.rd);     // rd
                strcat(result, info.opcode); // opcode
                //conversion from ascii (string) to integer
                int instruction[32];
                for(int i=0; i<32; i++)
                {
                    instruction[i]=result[i]-48;
                }
                writeInstructionToFile(outputFileChoice, outputFile, instruction, 32);
                
            }
            
            else if(typeOfInstr=='J')
            {
                while (instr != NULL && count<=2) {
                    if (count == 1) {
                        instr = strtok(NULL, delimiter); // Move to the next token
                        info.rd = registerMap(instr); // Extracting rd
                    } 
                    else if (count == 2) {
                        instr = strtok(NULL, delimiter); // Move to the next token
                        instr[strcspn(instr, "\n")] = '\0';
                        int lineToJump = matchLabel(instr, labelCount);
                        if(lineToJump>0)
                        {
                            printf("%d\n",lineToJump);
                        }
                        else
                            printf("%s\n","error");
                        lineToJump=lineToJump-lineNumberLabel;
                        immediate(lineToJump);

                    }
                    count++;
                    
                }
                
                char* result = (char*)malloc(32 * sizeof(char));
                // Initialize the result with an empty string
                result[0] = '\0';
                // Concatenate the strings

                 // For immediate[20, 10:1, 11, 19:12]
                    char imm_upper[2];    // For immediate[20]
                    char imm_lower[10];     // For immediate[10:1]
                    char imm_middle1[2];   // For immediate[11]
                    char imm_middle2[7];   // For immediate[19:12]
                    
                    // Extract the immediate parts
                    strncpy(imm_upper, info.rs2+1, 1);      // immediate[20]
                    // imm_upper[1] = '\0';
                    
                    strncpy(imm_lower, info.rs2 + 9, 10);   // immediate[10:1]
                    // imm_lower[10] = '\0';
                    
                    strncpy(imm_middle1, info.rs2 + 8, 1);  // immediate[11]
                    // imm_middle1[1] = '\0';
                    
                    strncpy(imm_middle2, info.rs2 + 2, 7);  // immediate[19:12]
                    // imm_middle2[7] = '\0';
                    
                 // For immediate[20, 10:1, 11, 19:12]
                    // Concatenate the strings in the required order
                    // strcpy(result, imm_upper);
                    strcat(result, imm_lower);
                    strcat(result, imm_middle1);
                    strcat(result, imm_middle2);
                    strcat(result, info.rd);
                    strcat(result, info.opcode);

                //conversion from ascii (string) to integer
                int instruction[32];
                for(int i=0; i<32; i++)
                {
                    instruction[i]=result[i]-48;
                }
                writeInstructionToFile(outputFileChoice, outputFile, instruction, 32);
                
            }
            
            while (instr != NULL) {
                
                instr = strtok(NULL, delimiter);
            }
        }
        lineNumberLabel+=4; //PC+4 for eaach line
    }

    fclose(filePointer);
}

void writeInstructionToFile(const char *outputFileChoice, const char *filename, int *instruction, int size) {
    FILE *fptr;
    char binary[33];
    char * hexToOut;
    char hex[11]; // "0x" + 8 hex digits + null terminator

    // Convert the binary array to a string
    for (int i = 0; i < size; i++) {
        binary[i] = instruction[i] + '0'; // Convert int to char
    }
    binary[size] = '\0'; // Null terminate the string
     // Convert the binary array to a hexadecimal string
    hexToOut=binaryToHex(instruction, size, hex);
    // printf("HexadecimalReturned: %s\n", hexToOut);

    if (first == 0) {
        fptr = fopen(filename, "w");
        first++;
    } else {
        fptr = fopen(filename, "ab");
    }

    if (strcmp(outputFileChoice, "-b") == 0)
    {
        for (int i = 0; i < size; i++) {
            fprintf(fptr, "%d", instruction[i]);
        }
    }
    else if (strcmp(outputFileChoice, "-h") == 0)
    {
        
        fprintf(fptr, "%s", hexToOut);
        
    }
    fprintf(fptr, "\n");
    fclose(fptr);
}

void immediate(int num) {
    if((info.func3=="001" && info.opcode=="0010011")|| (info.func3=="101" && info.opcode=="0010011")) //i-type srai, srli, slli
    {
        char *binary = (char *)malloc(5 * sizeof(char)); // 12 bits + 1 for null terminator
        if (binary == NULL) {
            exit(EXIT_FAILURE);
        }

        // Convert the integer to a 12-bit binary string
        for (int i = 4; i >= 0; i--) {
            binary[i] = (num & 1) + '0';
            num >>= 1;
        }
        binary[5] = '\0'; // Null terminate the string

        // Allocate memory for rs2 in the info struct
        info.rs2 = (char *)malloc(5 * sizeof(char)); // 12 bits + 1 for null terminator
        if (info.rs2 == NULL) {
            free(binary);
            exit(EXIT_FAILURE);
        }

        // Copy the binary string to rs2
        strcpy(info.rs2, binary);

        // Free the temporary binary string memory
        free(binary);

    }
    else {
        
         if(info.opcode=="0100011" || info.opcode=="1100011") //S-type and B-type
        {
            char *binary = (char *)malloc(13 * sizeof(char)); // 12 bits + 1 for null terminator
            if (binary == NULL) {
                exit(EXIT_FAILURE);
            }

            // Convert the integer to a 12-bit binary string
            for (int i = 11; i >= 0; i--) {
                binary[i] = (num & 1) + '0';
                num >>= 1;
            }
            binary[12] = '\0'; // Null terminate the string
            info.rd = (char *)malloc(13 * sizeof(char)); // 12 bits + 1 for null terminator
            if (info.rd == NULL) {
                free(binary);
                exit(EXIT_FAILURE);
            }

            // Copy the binary string to rs2
            strcpy(info.rd, binary);
            free(binary);
        }
        
        else if( info.opcode=="1101111" ) // J-type
        {
            char *binary = (char *)malloc(20 * sizeof(char)); // 12 bits + 1 for null terminator
            if (binary == NULL) {
                exit(EXIT_FAILURE);
            }

            // Convert the integer to a 12-bit binary string
            for (int i = 19; i >= 0; i--) {
                binary[i] = (num & 1) + '0';
                num >>= 1;
            }
            binary[20] = '\0'; // Null terminate the string
            info.rs2 = (char *)malloc(20 * sizeof(char)); // 32 bits + 1 for null terminator
            if (info.rs2 == NULL) {
                free(binary);
                exit(EXIT_FAILURE);
            }

            // Copy the binary string to rs2
            strcpy(info.rs2, binary);
            printf("%s\n", binary);
            free(binary);
        }
        else if( info.opcode=="0110111" || info.opcode=="0010111" ) // U-type
        {
            char *binary = (char *)malloc(4 * sizeof(char)); // 5 bits + 1 for null terminator
            if (binary == NULL) {
                exit(EXIT_FAILURE);
            }

            // Convert the integer to a 12-bit binary string
            for (int i = 3; i >= 0; i--) {
                binary[i] = (num & 1) + '0';
                num >>= 1;
            }
            binary[4] = '\0'; // Null terminate the string
            info.rs2 = (char *)malloc(4 * sizeof(char)); // 32 bits + 1 for null terminator
            if (info.rs2 == NULL) {
                free(binary);
                exit(EXIT_FAILURE);
            }

            // Copy the binary string to rs2
            strcpy(info.rs2, binary);
            free(binary);
        }
        else {
            char *binary = (char *)malloc(13 * sizeof(char)); // 12 bits + 1 for null terminator
            if (binary == NULL) {
                exit(EXIT_FAILURE);
            }

            // Convert the integer to a 12-bit binary string
            for (int i = 11; i >= 0; i--) {
                binary[i] = (num & 1) + '0';
                num >>= 1;
            }
            binary[12] = '\0'; // Null terminate the string
        // Allocate memory for rs2 in the info struct
            info.rs2 = (char *)malloc(13 * sizeof(char)); // 12 bits + 1 for null terminator
            if (info.rs2 == NULL) {
                free(binary);
                exit(EXIT_FAILURE);
            }

            // Copy the binary string to rs2
            strcpy(info.rs2, binary);
            free(binary);
        }
        // Free the temporary binary string memory
        

    }
}


char* registerMap(char* instr)
{
    if((strcmp(instr, "x0")==0) || (strcmp(instr, "zero")==0))
    {
        return "00000";
    }
    else if((strcmp(instr, "x1")==0) || (strcmp(instr, "ra")==0))
    {
        return "00001";
    }
    else if((strcmp(instr, "x2")==0) || (strcmp(instr, "sp")==0))
    {
        return "00010";
    }
    else if((strcmp(instr, "x3")==0) || (strcmp(instr, "gp")==0))
    {
        return "00011";
    }
    else if((strcmp(instr, "x4")==0) || (strcmp(instr, "tp")==0))
    {
        return "00100";
    }
    else if((strcmp(instr, "x5")==0) || (strcmp(instr, "t0")==0))
    {
        return "00101";
    }
    else if((strcmp(instr, "x6")==0) || (strcmp(instr, "t1")==0))
    {
        return "00110";
    }
    else if((strcmp(instr, "x7")==0) || (strcmp(instr, "t2")==0))
    {
        return "00111";
    }
    else if((strcmp(instr, "x8")==0) || (strcmp(instr, "s0")==0))
    {
        return "01000";
    }
    else if((strcmp(instr, "x9")==0) || (strcmp(instr, "s1")==0))
    {
        return "01001";
    }
    else if((strcmp(instr, "x10")==0) || (strcmp(instr, "a0")==0))
    {
        return "01010";
    }
    else if((strcmp(instr, "x11")==0) || (strcmp(instr, "a1")==0))
    {
        return "01011";
    }
    else if((strcmp(instr, "x12")==0) || (strcmp(instr, "a2")==0))
    {
        return "01100";
    }
    else if((strcmp(instr, "x13")==0) || (strcmp(instr, "a3")==0))
    {
        return "01101";
    }
    else if((strcmp(instr, "x14")==0) || (strcmp(instr, "a4")==0))
    {
        return "01110";
    }
    else if((strcmp(instr, "x15")==0) || (strcmp(instr, "a5")==0))
    {
        return "01111";
    }
    else if((strcmp(instr, "x16")==0) || (strcmp(instr, "a6")==0))
    {
        return "10000";
    }
    else if((strcmp(instr, "x17")==0) || (strcmp(instr, "a7")==0))
    {
        return "10001";
    }
    else if((strcmp(instr, "x18")==0) || (strcmp(instr, "s2")==0))
    {
        return "10010";
    }
    else if((strcmp(instr, "x19")==0) || (strcmp(instr, "s3")==0))
    {
        return "10011";
    }
    else if((strcmp(instr, "x20")==0) || (strcmp(instr, "s4")==0))
    {
        return "10100";
    }
    else if((strcmp(instr, "x21")==0) || (strcmp(instr, "s5")==0))
    {
        return "10101";
    }
    else if((strcmp(instr, "x22")==0) || (strcmp(instr, "s6")==0))
    {
        return "10110";
    }
    else if((strcmp(instr, "x23")==0) || (strcmp(instr, "s7")==0))
    {
        return "10111";
    }
    else if((strcmp(instr, "x24")==0) || (strcmp(instr, "s8")==0))
    {
        return "11000";
    }
    else if((strcmp(instr, "x25")==0) || (strcmp(instr, "s9")==0))
    {
        return "11001";
    }
    else if((strcmp(instr, "x26")==0) || (strcmp(instr, "s10")==0))
    {
        return "11010";
    }
    else if((strcmp(instr, "x27")==0) || (strcmp(instr, "s11")==0))
    {
        return "11011";
    }
    else if((strcmp(instr, "x28")==0) || (strcmp(instr, "t3")==0))
    {
        return "11100";
    }
    else if((strcmp(instr, "x29")==0) || (strcmp(instr, "t4")==0))
    {
        return "11101";
    }
    else if((strcmp(instr, "x30")==0) || (strcmp(instr, "t5")==0))
    {
        return "11110";
    }
    else if((strcmp(instr, "x31")==0) || (strcmp(instr, "t6")==0))
    {
        return "11111";
    }
    else 
        return "xxxxx";
}

char checkType(char* instr)
{
    //R-type
    if(strcmp(instr, "add")==0)
        {
            info.opcode="0110011";
            info.func3="000";
            info.func7="0000000";
            return 'R';
        }
    else if(strcmp(instr, "sub")==0)
        {
            info.opcode="0110011";
            info.func3="000";
            info.func7="0100000";
            return 'R';
        }
    else if(strcmp(instr, "sll")==0)
        {
            info.opcode="0110011";
            info.func3="001";
            info.func7="0000000";
            return 'R';
        }
    else if(strcmp(instr, "slt")==0)
        {
            info.opcode="0110011";
            info.func3="010";
            info.func7="0000000";
            return 'R';
        }
    else if(strcmp(instr, "sltu")==0)
        {
            info.opcode="0110011";
            info.func3="011";
            info.func7="0000000";
            return 'R';
        }
    else if(strcmp(instr, "xor")==0)
        {
            info.opcode="0110011";
            info.func3="100";
            info.func7="0000000";
            return 'R';
        }
    else if(strcmp(instr, "srl")==0)
        {
            info.opcode="0110011";
            info.func3="101";
            info.func7="0000000";
            return 'R';
        }
    else if(strcmp(instr, "sra")==0)
        {
            info.opcode="0110011";
            info.func3="101";
            info.func7="0100000";
            return 'R';
        }
    else if(strcmp(instr, "or")==0)
        {
            info.opcode="0110011";
            info.func3="110";
            info.func7="0000000";
            return 'R';
        }
    else if(strcmp(instr, "and")==0)
        {
            info.opcode="0110011";
            info.func3="111";
            info.func7="0000000";
            return 'R';
        }
    //I-type
    else if(strcmp(instr, "lb")==0)
        {
            info.opcode="0000011";
            info.func3="000";
            return 'I';
        }
    else if(strcmp(instr, "lh")==0)
        {
            info.opcode="0000011";
            info.func3="001";
            return 'I';
        }
    else if(strcmp(instr, "lw")==0)
        {
            info.opcode="0000011";
            info.func3="010";
            return 'I';
        }
    else if(strcmp(instr, "lbu")==0)
        {
            info.opcode="0000011";
            info.func3="100";
            return 'I';
        }
    else if(strcmp(instr, "lhu")==0)
        {
            info.opcode="0000011";
            info.func3="101";
            return 'I';
        }
    else if(strcmp(instr, "addi")==0)
        {
            info.opcode="0010011";
            info.func3="000";
            return 'I';
        }
    else if(strcmp(instr, "slli")==0)
        {
            info.opcode="0010011";
            info.func7="0000000";
            info.func3="001";
            return 'I';
        }
    else if(strcmp(instr, "slti")==0)
        {
            info.opcode="0010011";
            info.func3="010";
            return 'I';
        }
    else if(strcmp(instr, "sltiu")==0)
        {
            info.opcode="0010011";
            info.func3="011";
            return 'I';
        }
    else if(strcmp(instr, "xori")==0)
        {
            info.opcode="0010011";
            info.func3="100";
            return 'I';
        }
    else if(strcmp(instr, "srli")==0)
        {
            info.opcode="0010011";
            info.func7="0000000";
            info.func3="101";
            return 'I';
        }
    else if(strcmp(instr, "srai")==0)
        {
            info.opcode="0010011";
            info.func7="0100000";
            info.func3="101";
            return 'I';
        }
    else if(strcmp(instr, "ori")==0)
        {
            info.opcode="0010011";
            info.func3="110";
            return 'I';
        }
    else if(strcmp(instr, "andi")==0)
        {
            info.opcode="0010011";
            info.func3="111";
            return 'I';
        }
    else if(strcmp(instr, "jalr")==0)
        {
            info.opcode="0010011";
            info.func3="000";
            return 'I';
        }
    
    //S-type
    else if(strcmp(instr, "sb")==0)
        {
            info.opcode="0100011";
            info.func3="000";
            return 'S';
        }
    else if(strcmp(instr, "sh")==0)
        {
            info.opcode="0100011";
            info.func3="001";
            return 'S';
        }
    else if(strcmp(instr, "sw")==0)
        {
            info.opcode="0100011";
            info.func3="010";
            return 'S';
        }
    
    //U-type
    else if(strcmp(instr, "auipc")==0)
        {
            info.opcode="0010111";
            return 'U';
        }
    else if(strcmp(instr, "lui")==0)
        {
            info.opcode="0110111";
            return 'U';
        }

    //B-type
    else if(strcmp(instr, "beq")==0)
        {
            info.opcode="1100011";
            info.func3="000";
            return 'B';
        }
    else if(strcmp(instr, "bne")==0)
        {
            info.opcode="1100011";
            info.func3="001";
            return 'B';
        }
    else if(strcmp(instr, "blt")==0)
        {
            info.opcode="1100011";
            info.func3="100";
            return 'B';
        }
    else if(strcmp(instr, "bge")==0)
        {
            info.opcode="1100011";
            info.func3="101";
            return 'B';
        }
    else if(strcmp(instr, "bltu")==0)
        {
            info.opcode="1100011";
            info.func3="110";
            return 'B';
        }
    else if(strcmp(instr, "bgeu")==0)
        {
            info.opcode="1100011";
            info.func3="111";
            return 'B';
        }

    //J-type
    else if(strcmp(instr, "jal")==0)
        {
            info.opcode="1101111";
            return 'J';
        }
    
    else 
        return 'N';
}