/*
 * Copyright (C) Vamsi.  2017-18 All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License version 2.
 */
#include "mex.h"
#include "mexNs3State.h"

using namespace ns3;
static int MEXIsLoaded = 0;

void cleanup(void)
{
    MEXIsLoaded = 0;
}

void *getElem(MemMngmt *obj)
{
    return obj->vptr;
}

typedef enum methodType
{
    MATLABWIFIPHYHELPER,
    SETCHANNEL,
    DEFAULT,
    DELETE,
    TOTAL_METHODS
}methodType_t;

static char methodNames[TOTAL_METHODS][CMD_BUFFER_SIZE] = {"new", "SetChannel", "Default", "delete"};

methodType_t identifyMethod(const char* cmd)
{
    for (int i = 0; i < TOTAL_METHODS; i++)
    {
        if (!strcmp(methodNames[i], cmd))
            return methodType_t(i);
    }
    return TOTAL_METHODS;
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
    if (!MEXIsLoaded)
    {
        mexAtExit(cleanup);
        MEXIsLoaded = 1;
    }
    uint16_t indx = 0;
    char cmd[CMD_BUFFER_SIZE];
    if (nrhs < 1 || mxGetString(prhs[INPUT_Command_String_Index], cmd, sizeof(cmd)))
    {
        mexErrMsgTxt("Check your inputs");
    }
    switch(identifyMethod(cmd))
    {
        case MATLABWIFIPHYHELPER:
        {
            if ((nlhs != 1) || (nrhs > 1))
            {
                mexErrMsgTxt("enter correct number of input and output args");
            }
            MatlabWifiPhyHelper *obj = new MatlabWifiPhyHelper();
            MemMngmt *tst = bindMngrNewElem();
            tst->vptr = reinterpret_cast<void*>(obj);
            tst->objType = MATLABPHYHELPOBJ;
            plhs[0] = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
            *((uint64_t *)mxGetData(plhs[0])) = reinterpret_cast<uint64_t>((MemMngmt*)tst);
            break;
        }
        case SETCHANNEL:
        {
            if ((nlhs != 0) || (nrhs > 3))
            {
                mexErrMsgTxt("enter correct number of input and output args");
            }
            switch(nrhs)
            {
                case 3:
                {
                    MemMngmt *tst2=  bindMngrHasListElem ((MemMngmt *)(*((uint64_t *)mxGetData(prhs[2]))));
                    Ptr<MatlabWifiChannel> arg2 = *(reinterpret_cast<Ptr<MatlabWifiChannel>*>(tst2->vptr));
                    void *obj =  getElem(bindMngrHasListElem ((MemMngmt *)(*((uint64_t *)mxGetData(prhs[1])))));
                    (reinterpret_cast<MatlabWifiPhyHelper*>(obj))->SetChannel(arg2);
                    break;
                }
            }
            break;
        }
        case DELETE:
        {
            MemMngmt *obj =  bindMngrHasListElem ((MemMngmt *)(*((uint64_t *)mxGetData(prhs[1]))));
            if(obj)
            {
                if(obj->objType == MATLABPHYHELPOBJ)
                {
                    delete reinterpret_cast<MatlabWifiPhyHelper*>(obj->vptr);
                }
                bindMngrDelSingleElem(obj);
            }
            break;
        }
        case DEFAULT:
        {
            if ((nlhs != 1) || (nrhs > 1))
            {
                mexErrMsgTxt("enter correct number of input and output args");
            }
            MatlabWifiPhyHelper *tmp = new MatlabWifiPhyHelper(MatlabWifiPhyHelper::Default());
            MemMngmt *tst = bindMngrNewElem();
            tst->vptr = reinterpret_cast<void*>(tmp);
            tst->objType = MATLABPHYHELPOBJ;
            plhs[0] = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
            *((uint64_t *)mxGetData(plhs[0])) = reinterpret_cast<uint64_t>((MemMngmt*)tst);
            break;
        }
        default:
            break;
    }
}
