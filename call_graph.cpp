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
#include <stack>
using std::cerr;
using std::endl;
using std::string;
using std::stack;

/* ===================================================================== 
*/
/* Global Variables */
/* ===================================================================== 
*/

// COS375 TIP: Add global variables here 
string routineName;
bool foundMain = false;
FILE *outFile;
stack<string> callStack;

/* ===================================================================== 
*/
/* Function to do indentation correctly then print rountine - CHECK OH 
only first arg??? */
/* ===================================================================== 
*/
void Print(const string& routineName, ADDRINT firstArg) // do space based 
on pops but stack is lifo, need queue
{
    for (size_t i = 0; i < callStack.size(); ++i) // need to start with 
one
    {
        fprintf(outFile, " ");
    }
    fprintf(outFile, "%s(%lu,...)\n", routineName.c_str(), firstArg); // 
only first arg
}

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
        "This tool makes a function call graph\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

/* ===================================================================== 
*/

void executeAfterRoutine()
{
    if (foundMain){
       if (!callStack.empty()) {
        callStack.pop();
    }
    }
}

/* ===================================================================== 
*/
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

    //COS375: Add your code here
    callStack.push(routineName);

    // Print the current function call
    Print(routineName, firstArg);

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
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)executeBeforeRoutine, 
IARG_INST_PTR, IARG_FUNCARG_ENTRYPOINT_VALUE, 0, IARG_END);

    // pop after
    RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)executeAfterRoutine, 
IARG_END);
    
    RTN_Close(rtn);
}

/* ===================================================================== 
*/
// Function executed after instrumentation
VOID Fini(INT32 code, VOID *v)
{
    //COS375: Add your code here to dump instrumentation data that is 
collected.
    fprintf(outFile,"COS375 pin tool call graph\n");
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
    

    outFile = fopen("call_graph.out","w");
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

