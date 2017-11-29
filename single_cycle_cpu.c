/**
 * @author : Justin Benge
 * Date    : 9 November, 2017
 * Filename: proj_hw04.c
 * Description: Implement the various stages of a single cycle cpu 
 */
#include <stdint.h>
#include "proj_hw04.h"


typedef int WORD;


#define INST_WORDS 1024    // size of the instruction memory, in words
#define DATA_WORDS 16384   // size of the data        memory, in words



/*
 * Used to zero extend some 16 bit value to 32 bits 
 */
WORD zeroExtend16to32(WORD val)
{
	uint32_t zeroExt = (uint16_t)val;
	return (WORD)zeroExt;
}

/*
 *Used to get an instruction out of instruction memory using
 * the program counter as the index into memory
 */
WORD getInstruction(WORD curPC, WORD *instructionMemory)
{
	WORD retval;
	retval = instructionMemory[curPC/4];
	return retval;
}

/*
 * Used to fill the instruction fields given an instruction
 * (I think we are just "decoding" instructions and storing the
 * the binary into the struct InstructionFields
 */
void extract_instructionFields(WORD instruction, InstructionFields *fieldsOut)
{
	fieldsOut->opcode  = (instruction >> 26) & 0x3f;
	fieldsOut->rs      = (instruction >> 21) & 0x1f;
	fieldsOut->rt      = (instruction >> 16) & 0x1f;
	fieldsOut->imm16   = (instruction & 0xffff);
	fieldsOut->rd      = (instruction >> 11) & 0x1f;
	fieldsOut->shamt   = (instruction >> 6) & 0x1f;
	fieldsOut->funct   = (instruction & 0x3f);
	fieldsOut->imm32   = signExtend16to32(fieldsOut->imm16);
	fieldsOut->address = (instruction & 0x3ffffff);
}


/*
 * Fill the cpu control bits given a set of instruction fields
 * (do I need one giant switch case?
 */
int  fill_CPUControl(InstructionFields *fields, CPUControl *controlOut)
{
	/*
	 * R-Format instructions always have opcode 0
   	 * R-Format instructions for this project:
	 * 	(*)add, (*)addu, (*)sub, (*)subu, and, or, slt
	 * 
	 * (most of these should have the same/very similar control bits
	 *
	 * regDest = RegWrite = 1
	 * ALUSrc  = MemWrite = MemRead = MemToReg = branch = jump = 0
	 */
	if(fields->opcode == 0)
	{
		//first set all the common bits
		controlOut->regDst = controlOut->regWrite = 1;
		
		controlOut->ALUsrc = controlOut->memWrite = controlOut->memRead = controlOut->memToReg = controlOut->branch = controlOut->jump = 0;
		
		// now check the function code to see what the ALUop
		// and bNegate should be 
		
		//0 and
		//1 or
		//2 add
		//3 less

		//add
		if(fields->funct == 32)
		{
			controlOut->ALU.op      = 2;
			controlOut->ALU.bNegate = 0;
			return 1;
		}
		//addu op????
		if(fields->funct == 33)
		{
			controlOut->ALU.op      = 2;
			controlOut->ALU.bNegate = 0;
			return 1;
		}
		//sub
		if(fields->funct == 34)
		{
			controlOut->ALU.op      = 2;
			controlOut->ALU.bNegate = 1;
			return 1;
		}
		//subu op??
		if(fields->funct == 35)
		{
			controlOut->ALU.op      = 2;
			controlOut->ALU.bNegate = 1;
			return 1;
		}
		//and
		if(fields->funct == 36)
		{
			controlOut->ALU.op      = 0;
			controlOut->ALU.bNegate = 0;
			return 1;
		}
		//or
		if(fields->funct == 37)
		{
			controlOut->ALU.op      = 1;
			controlOut->ALU.bNegate = 0;
			return 1;
		}
		//slt
		if(fields->funct == 42)
		{
			controlOut->ALU.op      = 3;
			controlOut->ALU.bNegate = 1;
			return 1;
		}
		//if we don't find an instruction then 
		// set control bits to zero and return 0
		else
		{
			controlOut->regDst = controlOut->regWrite = 0;
			return 0;
		}
		
	}
	
	/*
	 * I-Format instructions get theyre own opcode
	 *
	 * (however should have similar control bits?)
	 *
	 * lw
	 * regDest = MemWrite = 0 
	 * RegWrite = ALUSrc = MemRead = MemToReg = 1
	 *
	 * sw
	 * RegDest = RegWrite = MemRead = MemToReg = 0
	 * ALUSrc = MemWrite = 1
	 */
	
	// opcode for j
	if(fields->opcode == 2)
	{
		controlOut->ALU.op       = 0;
		controlOut->ALU.bNegate  = 0;
		controlOut->regDst       = 0;
		controlOut->ALUsrc       = 0;
		controlOut->memToReg     = 0;
		controlOut->regWrite     = 0;
		controlOut->memRead      = 0;
		controlOut->memWrite     = 0;
		controlOut->branch       = 0;
		controlOut->jump         = 1;
		return 1;
	}
	// opcode for beq
	if(fields->opcode == 4)
	{	
		controlOut->ALU.op       = 2;
		controlOut->ALU.bNegate  = 1;
		controlOut->regDst       = 0;
		controlOut->ALUsrc       = 0;
		controlOut->memToReg     = 0;
		controlOut->regWrite     = 0;
		controlOut->memRead      = 0;
		controlOut->memWrite     = 0;
		controlOut->branch       = 1;
		controlOut->jump         = 0;
		return 1;
	}
	//bne
	if(fields->opcode == 5)
	{
		controlOut->ALU.op       = 2;
		controlOut->ALU.bNegate  = 1;
		controlOut->regDst       = 0;
		controlOut->ALUsrc       = 0;
		controlOut->memToReg     = 0;
		controlOut->regWrite     = 0;
		controlOut->memRead      = 0;
		controlOut->memWrite     = 0;
		controlOut->branch       = 1;
		controlOut->jump         = 0;
		return 1;
	}
	//opcdoe for addi
	if(fields->opcode == 8)
	{
		controlOut->ALU.op       = 2;
		controlOut->ALU.bNegate  = 0;
		controlOut->regDst       = 0;
		controlOut->ALUsrc       = 1;
		controlOut->memToReg     = 0;
		controlOut->regWrite     = 1;
		controlOut->memRead      = 0;
		controlOut->memWrite     = 0;
		controlOut->branch       = 0;
		controlOut->jump         = 0;
		return 1;
	}
	// opcode for addiu	
	// not sure what control wires should be
	if(fields->opcode == 9)
	{
		controlOut->ALU.op       = 2;
		controlOut->ALU.bNegate  = 0;
		controlOut->regDst       = 0;
		controlOut->ALUsrc       = 1;
		controlOut->memToReg     = 0;
		controlOut->regWrite     = 1;
		controlOut->memRead      = 0;
		controlOut->memWrite     = 0;
		controlOut->branch       = 0;
		controlOut->jump         = 0;
		return 1;
	}
	//opcode for slti
	//not sure of control vaues
	if(fields->opcode == 10)
	{
		controlOut->ALU.op       = 3;
		controlOut->ALU.bNegate  = 1;
		controlOut->regDst       = 0;
		controlOut->ALUsrc       = 1;
		controlOut->memToReg     = 0;
		controlOut->regWrite     = 1;
		controlOut->memRead      = 0;
		controlOut->memWrite     = 0;
		controlOut->branch       = 0;
		controlOut->jump         = 0;
		return 1;
	}
	//andi
	if(fields->opcode == 12)
	{
		controlOut->ALU.op       = 0;
		controlOut->ALU.bNegate  = 0;	
		controlOut->regDst       = 0;
		controlOut->ALUsrc       = 1;
		controlOut->memToReg     = 0;
		controlOut->regWrite     = 1;
		controlOut->memRead      = 0;
		controlOut->memWrite     = 0;
		controlOut->branch       = 0;
		controlOut->jump         = 0;
		return 1;
	}	
	//ori
	if(fields->opcode == 13)
	{
		controlOut->ALU.op       = 1;
		controlOut->ALU.bNegate  = 0;
		controlOut->regDst       = 0;
		controlOut->ALUsrc       = 1;
		controlOut->memToReg     = 0;
		controlOut->regWrite     = 1;
		controlOut->memRead      = 0;
		controlOut->memWrite     = 0;
		controlOut->branch       = 0;
		controlOut->jump         = 0;
		return 1;
	}
	//opcode for lw
	if(fields->opcode == 35)
	{
		controlOut->ALU.op       = 2;
		controlOut->ALU.bNegate  = 0;
		controlOut->regDst       = 0;
		controlOut->ALUsrc       = 1;
		controlOut->memToReg     = 1;
		controlOut->regWrite     = 1;
		controlOut->memRead      = 1;
		controlOut->memWrite     = 0;
		controlOut->branch       = 0;
		controlOut->jump         = 0;
		return 1;
	}
	//opcode for sw
	if(fields->opcode == 43)
	{
		controlOut->ALU.op       = 2;
		controlOut->ALU.bNegate  = 0;	
		controlOut->regDst       = 0;
		controlOut->ALUsrc       = 1;
		controlOut->memToReg     = 0;
		controlOut->regWrite     = 0;
		controlOut->memRead      = 0;
		controlOut->memWrite     = 1;
		controlOut->branch       = 0;
		controlOut->jump         = 0;
		return 1;
	}
	
	return 0;
}

/*
 *Get the first ALU input ie Read Data 1
 */
WORD getALUinput1(CPUControl *controlIn,
                  InstructionFields *fieldsIn,
                  WORD rsVal, WORD rtVal)
{
	return rsVal;
}

/*
 * Get the second ALU input so read Data 2 or the 16 bit immediate field
 * using a mux ( I may need to mux's one to choose read data 2, and one to 
 * to choose alu input)
 */
WORD getALUinput2(CPUControl *controlIn,
                  InstructionFields *fieldsIn,
                  WORD rsVal, WORD rtVal)
{
	// if ALUsrc = 1 then we don't care about what is in
	// read data 2 just choose imm32
	if(controlIn->ALUsrc == 1)
	{
		//alu input 2 for andi
		if(controlIn->ALU.op == 0)
			return zeroExtend16to32(fieldsIn->imm16);
		//alu input 2 for ori
		if(controlIn->ALU.op == 1)
			return zeroExtend16to32(fieldsIn->imm16);
		return fieldsIn->imm32;
	}
	
	return rtVal;
}

/*
 * Should I again just have one big switch case here to choose 
 * which instruction the ALU executes? 
 * 
 * Also should I be using controlIn.ALU.op to determine which instruction
 * to execute? (yes)
 * start by checking the aluop, if it is 0,1, or 3 then they should be 
 * farily easy, as I either do not have to check any extra wires, or 
 * minimal wires
 *
 * if it is 2 ie the add operation, then check all the control wires to 
 * see exactly what operaiton I should be performing
 *
 * Execute a single cycle of the ALU ie, and, or, add, less operations to 
 * perform the various instructiosn
 */
void execute_ALU(CPUControl *controlIn,
                 WORD input1, WORD input2,
                 ALUResult  *aluResultOut)
{
	//compute and(i) and jump
	if(controlIn->ALU.op == 0)
	{
		//andi 
		if(controlIn->ALUsrc)
			aluResultOut->result = input1 & input2;
		else
			aluResultOut->result = input1 & input2;
	}
	//compute or(i)
	else if(controlIn->ALU.op == 1)
	{
		if(controlIn->ALUsrc)
			aluResultOut->result = input1 | input2; 
		else	
			aluResultOut->result = input1 | input2;
	}
	//compute slt and slti
	else if(controlIn->ALU.op == 3)
	{
		//slti
		if(controlIn->ALUsrc)
		{
			aluResultOut->result = input1< input2;
		}
		//slt
		else
		{
			aluResultOut->result = input1<input2;
		}
	}
	
	/*
	 * Now we need another big switch case for 
	 * add, addu (*)
	 * sub, subu (*)
	 * addi, subi (not sure how to access immmediate field)
	 * lw (how to implement this)
	 * sw (how to implemtn this)
	 * beq (how to implement this)
	 * j (how to implement this)
	 */ 
	else
	{
		// add, addu, sub, subu
		if(controlIn->regDst)
		{
			// sbu/subu
			if(controlIn->ALU.bNegate)
				aluResultOut->result = input1 - input2;
			// add/addu
			else
				aluResultOut->result = input1 + input2;
		}
		if(controlIn->ALUsrc)
		{
			//subi
			if(controlIn->ALU.bNegate)
				aluResultOut->result = input1 - input2;
			//addi
			else
				aluResultOut->result = input1 + input2;
		}
		//lw
		if(controlIn->memRead)
		{
			aluResultOut->result = input2;
		}
		//sw (address comes from rsVal?
		if(controlIn->memWrite)
		{
			aluResultOut->result = input2;
		}
		//beq/bne
		if(controlIn->branch)
		{
			//bne
			if(controlIn->ALU.bNegate)
				aluResultOut->result = input1;
			else
				aluResultOut->result = input1;
		}
	}
	
	//Finally check to see if the aluresult was 0
	if(aluResultOut->result == 0)
		aluResultOut->zero = 1;
}

/*
 * I think I should be checking memRead and memWrite to determine
 * if I should be writing to memory, or reading from memory
 *
 * Update the memory if it needs to be updated, otherwise set
 * the 'out' bit to 0
 */
void execute_MEM(CPUControl *controlIn,
                 ALUResult  *aluResultIn,
                 WORD        rsVal, WORD rtVal,
                 WORD       *memory,
                 MemResult  *resultOut)
{
	//If we should write to memeory
	if(controlIn->memWrite)
	{
		memory[(aluResultIn->result)/4] = rtVal;
		resultOut->readVal = 0;
	}
	//if we should read from memory
	// this should be fine as memRead will never be set when 
	// memWrite is set and vice versa?
	if(controlIn->memRead)
	{
		resultOut->readVal = memory[(aluResultIn->result)/4];
	}
	else
		resultOut->readVal = 0;
}

/*
 * To implement this, I want to check the aluOp to see if it is
 * a branch or jump instruction. if so then increment by the address
 * in the instruction field, otherwise just add 4?
 *
 * Update the pc by 4 then check to see if we brahced or jumped
 * and update it accordingly
 */
WORD getNextPC(InstructionFields *fields, CPUControl *controlIn, int aluZero,
               WORD rsVal, WORD rtVal,
               WORD oldPC)
{
	// first add 4 to the oldPC
	WORD newPc = oldPC + 4;
	//now check to see if we branced
	if(controlIn->branch && aluZero)
	{
		int shift = (fields->imm32 << 2);
		newPc += shift;
		return newPc;
	}
	//now Check to see if we jumped
	if(controlIn->jump)
	{
		//if we did jump, we need to first get
		// the top 4 bits of the original
		// pc ie PC[31-28]
		WORD top4 = newPc & 0xf0000000;
		
		// now we need to or it with
		// the jump address shifted left two bits
		newPc = top4 | ((fields->address << 2) & 0xfffffff);
	}
	//now return the new Pc
	return newPc;
}

/*
 * The final stage of the cpu, write back to the registers if necessary
 * otherwise do nothgin
 */ 
void execute_updateRegs(InstructionFields *fields, CPUControl *controlIn,
                        ALUResult  *aluResultIn, MemResult *memResultIn,
                        WORD       *regs)
{
	//if we need to write to registers
	if(controlIn->regWrite)
	{
		//are we writing to the rd register?
		if(controlIn->regDst)
		{
			//are we writing from memory?
			if(controlIn->memToReg)
				regs[fields->rd] = memResultIn->readVal;
			//otherwise use aluresult
			else
				regs[fields->rd] = aluResultIn->result;
		}
		//otherwise write to the rt register
		else
		{
			if(controlIn->memToReg)
				regs[fields->rt] = memResultIn->readVal;
			
			else
				regs[fields->rt] = aluResultIn->result;
		}
	}
} // end file
