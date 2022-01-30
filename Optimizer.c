#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

int criticalArr[100];
int criticalArrCounter = 0;
int writeArr[100];
int writeArrCounter = 0;
int readArr[100];
int readArrCounter = 0;
int instrToRemove[100];
int instrToRemoveCounter = 0;

void criticalReg(Instruction*, int);
void removeDuplicate();
void writeStoreReg(Instruction*, int);
void removeInstr(Instruction*, int);
void checkStoreInstr(Instruction*, int);

int main()
{
	Instruction *head;

	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}
	/* YOUR CODE GOES HERE */
	Instruction *copyList1 = head;
	int totalNumInstr = 0;

	//finding WRITE instructions: cruical parts
	while(copyList1){
		if(copyList1->opcode == READ){
			readArr[readArrCounter] = copyList1->field1;
			//criticalArr[criticalArrCounter] = copyList1->field1;
			readArrCounter++;
			//criticalArrCounter++;
		}
		if(copyList1->opcode == WRITE){
			writeArr[writeArrCounter] = copyList1->field1;
			writeArrCounter++;
			//criticalArr[criticalArrCounter] = copyList1->field1;
			//criticalArrCounter++;
		}
		totalNumInstr++;
		copyList1 = copyList1->next;
	}	

	/*for(int i = 0; i < readArrCounter; i++){
		printf("%d\n", readArr[i]);
	}
	for(int i = 0; i < writeArrCounter; i++){
		printf("%d\n", writeArr[i]);
	}*/
	
	//Finding all STORE instructions for Crucial WRITE instructions: marking them are critcal
	for(int i = 0; i < writeArrCounter; i++){
		writeStoreReg(head, i);
	}
	
	//finding all critical register
	for(int i = 0; i < criticalArrCounter; i++){
		criticalReg(head, i);
	}
		
	/*for(int i = 0; i < criticalArrCounter; i++){
		printf("%d\n", criticalArr[i]);
	}*/
	
	//trying to figure out which instructions to remove}
	for(int i = 0; i < readArrCounter; i++){
		instrToRemove[i] = 1;
	}
	for(int i = totalNumInstr - writeArrCounter; i < totalNumInstr; i++){
		instrToRemove[i] = 1;
	}
	instrToRemoveCounter = readArrCounter;
	for(int i = 0; i < totalNumInstr; i++){
		removeInstr(head, i);
		instrToRemoveCounter++;
	}
	
	int numChanges = 0;
	for(int i = 0; i < totalNumInstr; i++){
		if(instrToRemove[i] == 1){
			numChanges++;
		}
		//printf("%d\n", instrToRemove[i]);
	}
	
	//removing unecessary instructions
	if(numChanges != totalNumInstr){

		int removingCounter = 0;
		while(head){
			if(head->opcode == READ){
				PrintInstruction(stdout, head);
				head = head->next;
			}
			else if(instrToRemove[removingCounter] == 1){
				PrintInstruction(stdout, head);
				//removingCounter++;
				head = head->next;
			}
			else if(head->opcode == WRITE){
				PrintInstruction(stdout, head);
				head = head->next;
			}
			else{
				Instruction *removing;
				removing = head;
				head = head->next;
				free(removing);
			}

			removingCounter++;
		}
			
	}

	//Printing out the instructions
	if (head) {
		PrintInstructionList(stdout, head);
		DestroyInstructionList(head);
	}
	return EXIT_SUCCESS;
}

//function to remove duplicate regisiters from the critical registers
void removeDuplicate(){
	int counter = 0;
	int tempArr[100];
	for(int i = 0; i < criticalArrCounter; i++){
		int j;
		for(j = 0; j < counter; j++){
			if(criticalArr[i] == tempArr[j])
				break;
		}
		if(j == counter){
			tempArr[counter] = criticalArr[i];
			counter++;
		}
	}
	for(int i = 0; i < counter; i++){
		criticalArr[i] = tempArr[i];
	}
	criticalArrCounter = counter;
}

//Function to help find STORE instructions and marking them as critical
void writeStoreReg(Instruction *head, int i){
	Instruction *copyList2 = head;
	int j = 0;
	while(copyList2){
		if(copyList2->opcode == STORE){
			if(writeArr[i] == copyList2->field1){
				criticalArr[criticalArrCounter] = copyList2->field2;
				criticalArrCounter++;
				instrToRemove[j] = 1;
			}
		}
		copyList2 = copyList2->next;
		j++;
	}
}

void checkStoreInstr(Instruction *head, int field_2){
	Instruction *listCopy = head;
	while(listCopy){
		if(listCopy->opcode == STORE){
			if(listCopy->field1 == field_2){
				criticalArr[criticalArrCounter] = listCopy->field2;
				criticalArrCounter++;
			}
		}
		listCopy = listCopy->next;
	}
}


//Function to help find critical registers
void criticalReg(Instruction *head, int i){
	Instruction *copyList3 = head;

	while(copyList3){
		//printf("%d %d %d: %d\n", copyList3->field1, copyList3->field2, copyList3->field3, criticalArr[i]);
		if(criticalArr[i] == copyList3->field1 || criticalArr[i] == copyList3->field2 || criticalArr[i] == copyList3->field3){
			if(copyList3->opcode == ADD || copyList3->opcode == SUB || copyList3->opcode == MUL || copyList3->opcode == OR || copyList3->opcode == XOR){
				criticalArr[criticalArrCounter] = copyList3->field2;
				criticalArrCounter++;
				removeDuplicate();
				criticalArr[criticalArrCounter] = copyList3->field3;
				criticalArrCounter++;
				removeDuplicate();
			}
			if(copyList3->opcode == LOADI){
				criticalArr[criticalArrCounter] = copyList3->field1;
				criticalArrCounter++;
				removeDuplicate();
			}
			if(copyList3->opcode == LOAD){
				criticalArr[criticalArrCounter] = copyList3->field1;
				criticalArrCounter++;
				checkStoreInstr(head, copyList3->field2);
				removeDuplicate();
			}
		}
		copyList3 = copyList3->next;
	}
}

void removeInstr(Instruction *head, int i){
	Instruction *copyList4 = head;
	//printf("%d", instrToRemoveCounter);
	while(copyList4){
	/*	if(copyList4->opcode == READ){
			instrToRemove[instrToRemoveCounter] = 1;
		}*/
		if(copyList4->opcode == LOADI || copyList4->opcode == LOAD){
			if(criticalArr[i] == copyList4->field1){
				//instrToRemove[i] = 1;
				instrToRemove[instrToRemoveCounter] = 1;
			}
		}
		if(copyList4->opcode == ADD || copyList4->opcode == SUB || copyList4->opcode == MUL || copyList4->opcode == OR || copyList4->opcode == XOR){
			if(criticalArr[i] == copyList4->field1 || criticalArr[i] == copyList4->field2 || criticalArr[i] == copyList4->field3){
				//instrToRemove[i] = 1;
				instrToRemove[instrToRemoveCounter] = 1;
			}
		}
		if(copyList4->opcode == STORE){
			if(criticalArr[i] == copyList4->field2)
				instrToRemove[instrToRemoveCounter] = 1;
		}

		copyList4 = copyList4->next;
	}
	//instrToRemoveCounter++;
}
