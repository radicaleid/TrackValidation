#ifndef TrackValidation_PixelClusterAnalysis_H
#define TrackValidation_PixelClusterAnalysis_H


#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include <EventLoop/Algorithm.h>
#include <TH1.h>
#include <TH2.h>
#include <fstream>
#include <stdio.h>
#include<iostream>


#include "TrackValidation/TrackEfficiencyHists.h"
#include "TrackValidation/TrackQualityHists.h"

using namespace std;


class PixelClusterAnalysis : public EL::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
public:
  // float cutValue;

   //ouput pixels
   // FILE *  outfile;

  // ofstream myfile;

  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)
public:
  // Tree *myTree; //!

   // pixels
   TH2 *myXY; //!
   TH2 *myXZ; //!
   TH1 *myZ; //!
   TH1 *myX; //!
   TH1 *myC; //!
   TH1 *myTOT; //!
   TH1 *myLayer; //!

   // strips
   TH2 *stXY; //!
   TH2 *stXZ; //!
   TH1 *stZ; //!
   TH1 *stX; //!
   TH1 *stC; //!
   TH1 *stTOT; //!
   TH1 *stLayer; //!

   TH1 *myPt; //!
   TH1 *myEta; //!
   TH1 *myPhi; //!

   TrackQualityHists*    m_QualAllTracks;    //!


  // this is a standard constructor
  PixelClusterAnalysis ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();


  xAOD::TEvent *m_event;          //!
  unsigned int  m_eventCounter;   //!
 

  // this is needed to distribute the algorithm to the workers
  ClassDef(PixelClusterAnalysis, 1);
};

#endif
