/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* AliAnaysisTaskMyTask
 *
 * empty task which can serve as a starting point for building an analysis
 * as an example, one histogram is filled
 */

#include "TChain.h"
#include "TH1F.h"
#include "TList.h"
#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"
#include "AliAODEvent.h"
#include "AliAODInputHandler.h"
#include "AliAODMCParticle.h"
#include "AliMCEvent.h"
#include "AliAnalysisTaskMyTask.h"

class AliAnalysisTaskMyTask;    // your analysis class

using namespace std;            // std namespace: so you can do things like 'cout'

ClassImp(AliAnalysisTaskMyTask) // classimp: necessary for root

AliAnalysisTaskMyTask::AliAnalysisTaskMyTask() : AliAnalysisTaskSE(),
    fAOD(0), fOutputList(0), fHistPt(0), fMCEvent(0), fReadMC(kFALSE), fHistPDG(0), fOutputTree(0), fPt(0.)
{
    // default constructor, don't allocate memory here!
    // this is used by root for IO purposes, it needs to remain empty
}
//_____________________________________________________________________________
AliAnalysisTaskMyTask::AliAnalysisTaskMyTask(const char* name) : AliAnalysisTaskSE(name),
    fAOD(0), fOutputList(0), fHistPt(0), fMCEvent(0), fReadMC(kFALSE), fHistPDG(0), fOutputTree(0), fPt(0.)
{
    // constructor
    DefineInput(0, TChain::Class());    // define the input of the analysis: in this case we take a 'chain' of events
                                        // this chain is created by the analysis manager, so no need to worry about it,
                                        // it does its work automatically
    DefineOutput(1, TList::Class());    // define the ouptut of the analysis: in this case it's a list of histograms
                                        // you can add more output objects by calling DefineOutput(2, classname::Class())
                                        // if you add more output objects, make sure to call PostData for all of them, and to
                                        // make changes to your AddTask macro!
    DefineOutput(2, TTree::Class());    // define a second ouptut of the analysis: in this case it's a tree
}
//_____________________________________________________________________________
AliAnalysisTaskMyTask::~AliAnalysisTaskMyTask()
{
    // destructor
    if(fOutputList) {
        delete fOutputList;     // at the end of your task, it is deleted from memory by calling this function
    }
    if (fOutputTree) {          // the tree has to be explicitly deleted because it is not in the output list
        delete fOutputTree;
    }
}
//_____________________________________________________________________________
void AliAnalysisTaskMyTask::UserCreateOutputObjects()
{
    // create output objects
    //
    // this function is called ONCE at the start of your analysis (RUNTIME)
    // here you ceate the histograms that you want to use
    //
    // the histograms are in this case added to a tlist, this list is in the end saved
    // to an output file
    //
    fOutputList = new TList();          // this is a list which will contain all of your histograms
                                        // at the end of the analysis, the contents of this list are written
                                        // to the output file
    fOutputList->SetOwner(kTRUE);       // memory stuff: the list is owner of all objects it contains and will delete them
                                        // if requested (dont worry about this now)

    // example of a histogram
    fHistPt = new TH1F("fHistPt", "fHistPt", 100, 0, 10);       // create your histogra
    fOutputList->Add(fHistPt);          // don't forget to add it to the list! the list will be written to file, so if you want
                                        // your histogram in the output file, add it to the list!

    fOutputTree = new TTree("fOutputTree", "fOutputTree"); // create your tree
    fOutputTree->Branch("pT", &fPt);                       // add a branch to the tree
                                                           // the first argument is the name of the branch
                                                           // the second argument is the address of the item that you want to put in your branch
                                                           // you can add as many branches as you want, but be careful that the size of your tree can be very large!
                                                           // you don't have to add your tree to the output list, since it is a separated output!

    if(fReadMC)                                                   // if read MC information, create histogram with PDG codes
    {
        fHistPDG = new TH1F("fHistPDG", "fHistPDG", 500, 0, 500);
        fOutputList->Add(fHistPDG);
    }

    PostData(1, fOutputList);           // postdata will notify the analysis manager of changes / updates to the
                                        // fOutputList object. the manager will in the end take care of writing your output to file
                                        // so it needs to know what's in the output
    PostData(2, fOutputTree);           // postdata also the fOutputTree
}
//_____________________________________________________________________________
void AliAnalysisTaskMyTask::UserExec(Option_t *)
{
    // user exec
    // this function is called once for each event
    // the manager will take care of reading the events from file, and with the static function InputEvent() you
    // have access to the current event.
    // once you return from the UserExec function, the manager will retrieve the next event from the chain
    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());    // get an event (called fAOD) from the input file
                                                        // there's another event format (ESD) which works in a similar wya
                                                        // but is more cpu/memory unfriendly. for now, we'll stick with aod's
    if(!fAOD) return;                                   // if the pointer to the event is empty (getting it failed) skip this event
        // example part: i'll show how to loop over the tracks in an event
        // and extract some information from them which we'll store in a histogram
    Int_t iTracks(fAOD->GetNumberOfTracks());           // see how many tracks there are in the event
    for(Int_t i(0); i < iTracks; i++) {                 // loop ove rall these tracks
        AliAODTrack* track = static_cast<AliAODTrack*>(fAOD->GetTrack(i));         // get a track (type AliAODTrack) from the event
        if(!track || !track->TestFilterBit(1)) continue;                            // if we failed, skip this track
        fPt = track->Pt();
        fHistPt->Fill(fPt);                             // plot the pt value of the track in a histogram
        fOutputTree->Fill();                            // fill tree --> this has to be called only once for all the branches
    }

    if(fReadMC)                                         // if flag for MC information enabled, process MC particles
    {
        fMCEvent = MCEvent();                           // get MC event
        if(!fMCEvent)                                   // if the pointer to the event is empty (getting it failed) skip this event
            return;
        ProcessMCParticles();                           // process MC particles calling the method that loops over MC particles
    }
                                                        // continue until all the tracks are processed
    PostData(1, fOutputList);                           // stream the results the analysis of this event to
                                                        // the output manager which will take care of writing
                                                        // it to a file
    PostData(2, fOutputTree);
}
//_____________________________________________________________________________
void AliAnalysisTaskMyTask::Terminate(Option_t *)
{
    // terminate
    // called at the END of the analysis (when all events are processed)
}
//_____________________________________________________________________________
Bool_t AliAnalysisTaskMyTask::ProcessMCParticles()
{
    TClonesArray* AODMCTrackArray = dynamic_cast<TClonesArray*>(fInputEvent->FindListObject(AliAODMCParticle::StdBranchName()));
    if(!AODMCTrackArray)
        return kFALSE;

    for(Int_t iPart=0; iPart<AODMCTrackArray->GetEntriesFast(); iPart++)
    {
        AliAODMCParticle* part = static_cast<AliAODMCParticle*>(AODMCTrackArray->At(iPart));
        if(!part)
            continue;
        fHistPDG->Fill(part->GetPdgCode());
    }

    return kTRUE;
}