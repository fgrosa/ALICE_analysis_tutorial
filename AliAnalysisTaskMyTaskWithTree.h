/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. */
/* See cxx source for full Copyright notice */
/* $Id$ */

#ifndef AliAnalysisTaskMyTaskWithTree_H
#define AliAnalysisTaskMyTaskWithTree_H

#include <TTree.h>
#include <TH1F.h>
#include <TList.h>

#include "AliAnalysisTaskSE.h"
#include "AliAODEvent.h"

class AliAnalysisTaskMyTaskWithTree : public AliAnalysisTaskSE
{
    public:
                                AliAnalysisTaskMyTaskWithTree();
                                AliAnalysisTaskMyTaskWithTree(const char *name);
        virtual                 ~AliAnalysisTaskMyTaskWithTree();

        virtual void            UserCreateOutputObjects();
        virtual void            UserExec(Option_t* option);
        virtual void            Terminate(Option_t* option);

    private:
        AliAODEvent*            fAOD;           //! input event
        TList*                  fOutputList;    //! output list
        TH1F*                   fHistPt;        //! dummy histogram
        TTree*                  fOutputTree;    //! output tree
        Float_t                 fPt;            /// pt of the tracks to be saved in the tree

        AliAnalysisTaskMyTaskWithTree(const AliAnalysisTaskMyTaskWithTree&); // not implemented
        AliAnalysisTaskMyTaskWithTree& operator=(const AliAnalysisTaskMyTaskWithTree&); // not implemented

        ClassDef(AliAnalysisTaskMyTaskWithTree, 1);
};

#endif
