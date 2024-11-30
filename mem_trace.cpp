/*
 * Copyright 2002-2020 Intel Corporation.
 * 
 * This software is provided to you as Sample Source Code as defined in 
the accompanying
 * End User License Agreement for the Intel(R) Software Development 
Products ("Agreement")
 * section 1.L.
 * 
 * This software and the related documents are provided as is, with no 
express or implied
 * warranties, other than those that are expressly stated in the License.
 */

/*! @file
 *  This file contains an ISA-portable PIN tool for counting dynamic 
instructions
 */

#include "pin.H"
#include <iostream>
#include <string.h>
using std::cerr;
using std::endl;
using std::string;

/* ===================================================================== 
*/
/* Global Variables */
/* ===================================================================== 
*/

// COS375 TIP: Add global variables here 
string routineName;
bool foundMain = false;
FILE *outFile;
int count;

/* ===================================================================== 
*/
/* Commandline Switches */
/* ===================================================================== 
*/


/* ===================================================================== 
*/
/* Print Help Message                                                    
*/
/* ===================================================================== 
*/

INT32 Usage()
{
    cerr <<
        "This tool is for tracing memory.\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

/* ===================================================================== 
*/

VOID PrintMemAccess(ADDRINT ip, ADDRINT memAddr, const char *opType)
{
    if (foundMain){
        fprintf(outFile, "0x%lx 0x%lx %s\n", ip, memAddr, opType);
        count++;
    }
}

/* ===================================================================== 
*/
// A callback function executed at runtime before executing first
// instruction in a function
void executeBeforeRoutine(ADDRINT ip)
{
    // Check if main function is called
    // If so then set foundMain to true
    routineName = (RTN_FindNameByAddress(ip));
    if (routineName.compare("main") == 0){
        foundMain=true;
    }    
    
    // Do nothing until main function is seen
    if (!foundMain){
        return;
    }

    //COS375: Add your code here
        
    // Check if exit function is called
    if(routineName.compare("exit") == 0){
        foundMain=false;
    }
}

/* ===================================================================== 
*/
// Function executed everytime a new routine is found
VOID Routine(RTN rtn, VOID *v)
{
    RTN_Open(rtn);
    //Insert callback to function executeBeforeRoutine which will be 
    //executed just before executing first instruction in the routine
    //at runtime
    INS_InsertCall(RTN_InsHead(rtn), IPOINT_BEFORE, 
        (AFUNPTR)executeBeforeRoutine, IARG_INST_PTR, IARG_END);

    //Iterate over all instructions of routne rtn
    for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins)){
        //COS375: used sample from assignment
        if (INS_IsMemoryRead(ins)) { //instr address, load add, op load or 
if statment
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)PrintMemAccess,
            IARG_INST_PTR, IARG_MEMORYREAD_EA, IARG_PTR, "L", IARG_END);
        }

    // Check if the instruction is a memory write
        if (INS_IsMemoryWrite(ins)) {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)PrintMemAccess, 
                IARG_INST_PTR, IARG_MEMORYWRITE_EA, IARG_PTR, "S", 
IARG_END);
        }
    }
        
    RTN_Close(rtn);
}

/* ===================================================================== 
*/
// Function executed after instrumentation
VOID Fini(INT32 code, VOID *v)
{
    //COS375: Add your code here to dump instrumentation data that is 
collected.
    fprintf(outFile,"Tool for tracking memory access.\n");
    fprintf(outFile, "Number or memory accesses: %i\n", count);
    fclose(outFile);
}


// DO NOT EDIT CODE AFTER THIS LINE
/* ===================================================================== 
*/
/* Main                                                                  
*/
/* ===================================================================== 
*/

int main(int argc, char *argv[])
{
    PIN_InitSymbols();
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    

    outFile = fopen("mem_trace.out","w");
    RTN_AddInstrumentFunction(Routine, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== 
*/
/* eof */
/* ===================================================================== 
*/

