/*
 * Copyright 2002-2020 Intel Corporation.
 * 
 * This software is provided to you as Sample Source Code as defined in the accompanying
 * End User License Agreement for the Intel(R) Software Development Products ("Agreement")
 * section 1.L.
 * 
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.
 */

/*! @file
 *  This file contains an ISA-portable PIN tool for counting dynamic instructions
 */
/* Code structure provided by COS375 */

#include "pin.H"
#include <iostream>
#include <string.h>
#include <stack>
using std::cerr;
using std::endl;
using std::string;
using std::stack;

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

string routineName;
bool foundMain = false;
FILE *outFile;
stack<string> callStack;

/* ===================================================================== */

void Print(const string& routineName, ADDRINT firstArg)
{
    for (size_t i = 0; i < callStack.size(); ++i)
    {
        fprintf(outFile, " ");
    }
    fprintf(outFile, "%s(%lu,...)\n", routineName.c_str(), firstArg);
}

/* ===================================================================== */
/* Print Help Message */
/* ===================================================================== */

INT32 Usage()
{
    cerr <<
        "This tool makes a function call graph\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

/* ===================================================================== */

void executeAfterRoutine()
{
    if (foundMain){
       if (!callStack.empty()) {
        callStack.pop();
       }
    }
}

/* ===================================================================== */
// A callback function executed at runtime before executing first
// instruction in a function
void executeBeforeRoutine(ADDRINT ip, ADDRINT firstArg)
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

    callStack.push(routineName);

    // Print the current function call
    Print(routineName, firstArg);

    // Check if exit function is called
    if(routineName.compare("exit") == 0){
        foundMain=false;
    }
}

/* ===================================================================== */
// Function executed everytime a new routine is found
VOID Routine(RTN rtn, VOID *v)
{
    RTN_Open(rtn);
    //Insert callback to function executeBeforeRoutine which will be 
    //executed just before executing first instruction in the routine
    //at runtime
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)executeBeforeRoutine, 
        IARG_INST_PTR, IARG_FUNCARG_ENTRYPOINT_VALUE, 0, IARG_END);

    RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)executeAfterRoutine, 
        IARG_END);
    
    RTN_Close(rtn);
}

/* ===================================================================== */
// Function executed after instrumentation
VOID Fini(INT32 code, VOID *v)
{
    fprintf(outFile,"COS375 pin tool call graph\n");
    fclose(outFile);
}

/* ===================================================================== */
/* Main */
/* ===================================================================== */

int main(int argc, char *argv[])
{
    PIN_InitSymbols();
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    

    outFile = fopen("call_graph.out","w");
    RTN_AddInstrumentFunction(Routine, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */

