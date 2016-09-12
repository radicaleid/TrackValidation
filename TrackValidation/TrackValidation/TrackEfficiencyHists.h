#ifndef TrackEfficiencyHists_h
#define TrackEfficiencyHists_h

#include "TH1D.h"
#include "TH2D.h"
#include "TGraphAsymmErrors.h"
#include "TMath.h"
#include "TString.h"

#ifndef __MAKECINT__
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTracking/TrackParticle.h"
#endif // not __MAKECINT__

#include <EventLoop/Worker.h>
#include <vector>
#include <set>

struct TrackEfficiencyHists
{

  TString name;      //!

  TH1D*  h_eta;        //!
  TH1D*  h_phi;        //!
  TH1D*  h_pt;         //!
  TH1D*  h_ptHigh;     //!
  TH1D*  h_ptLog;      //!
  TH1D*  h_radius;     //!
  TH1D*  h_radiuswzcut;//!
  TH2D*  h_ptVsEta;    //!
  TH2D*  h_etaVsPhi;   //!
  TH2D*  h_radiusVsEta;//!
  TH2D*  h_radiusVsZ;  //!
  
  std::vector< TH1* > allHists;
  std::set<int>       usedBarcodes;
  std::set<int>       Barcodes;

  TrackEfficiencyHists(const TString& n)
  {
    name = n;


    const int nbinsPt=100;
    Double_t ptbins[nbinsPt+1];
    double dx = 3./nbinsPt;
    double l10 = TMath::Log(10);
    for (int i=0;i<=nbinsPt;i++) {
      ptbins[i] = TMath::Exp(l10*(i*dx -1.) );
    }


    h_eta        =  new TH1D( name + "TracksVsEta", "TracksVsEta; #eta; # Tracks", 20, -4.0, 4.0 );
    h_phi        =  new TH1D( name + "TracksVsPhi", "TracksVsPhi; #phi [rad]; # Tracks", 40, -TMath::Pi(), TMath::Pi() );
    h_pt         =  new TH1D( name + "TracksVsPt", "TracksVsPt; p_{T} [GeV]; # Tracks",  25,0,50.0  );
    h_ptHigh     =  new TH1D( name + "TracksVsPtHigh", "TracksVsPtHigh; p_{T} [GeV]; # Tracks", 50, 0, 500 );
    h_ptLog      =  new TH1D( name + "TracksVsPtLog", "TracksVsPt; p_{T} [GeV]; # Tracks",  nbinsPt, ptbins  );
    h_radius     =  new TH1D( name + "TracksVsR", "TracksVsR; Radius [mm]; # Tracks", 50, 0, 200 );
    h_radiuswzcut=  new TH1D( name + "TracksVsRwZcut", "TracksVsRwZcut; Radius [mm]; # Tracks", 50, 0, 200 );
    h_ptVsEta    =  new TH2D( name + "TracksVsPtEta", "TracksVsPtEta;  p_{T} [GeV]; #eta ; # Tracks", 25, 0, 50, 40, -4.0, 4.0 ); 
    h_etaVsPhi   =  new TH2D( name + "TracksVsEtaPhi", "TracksVsEtaPhi;  #eta ; #phi [rad]; # Tracks", 50, -4.0, 4.0,  50, -TMath::Pi(), TMath::Pi() ); 
    h_radiusVsEta=  new TH2D( name + "TracksVsRadiusEta", "TracksVsRadiusEta;  Radius [mm]; #eta; # Tracks", 50, 0, 200, 20, -4.0, 4.0 );
    h_radiusVsZ  =  new TH2D( name + "TracksVsRadiusZ", "TracksVsRadiusZ;  Radius [mm]; #eta; # Tracks", 50, 0, 200, 100, -800, 800 );
    
    allHists.push_back( h_eta );
    allHists.push_back( h_phi );
    allHists.push_back( h_pt );
    allHists.push_back( h_ptHigh );
    allHists.push_back( h_ptLog );
    allHists.push_back( h_radius );
    allHists.push_back( h_radiuswzcut );
    allHists.push_back( h_ptVsEta );
    allHists.push_back( h_etaVsPhi );
    allHists.push_back( h_radiusVsEta );
    allHists.push_back( h_radiusVsZ );
  

    for( std::vector<TH1*>::iterator hist = allHists.begin(); hist != allHists.end(); ++hist){
		  (*hist)->Sumw2();
    }
	
	}
  
  ~TrackEfficiencyHists()
  {
    for( std::vector<TH1*>::iterator hist = allHists.begin(); hist != allHists.end(); ++hist){
      delete *hist;
    }
  }

  //* Determine if the TruthParticle has been used before */
  bool hasBeenUsed( const xAOD::TruthParticle* tp)
	{

    // make sure unique barcode for pile-up. S.Chekanov. 
    int newbarcode=int(tp->barcode());//+int(1000000*tp->eta()*tp->eta())+int(1000000*tp->phi()*tp->phi());
/*  std::cout<<"barcode: "<<tp->barcode()<<" newbarcode: "<<newbarcode<<std::endl;
    if(!(Barcodes.find(int(tp->barcode())) == Barcodes.end()))
      std::cout<<"find duplicate barcode: "<<tp->barcode()<<" newbarcode: "<<newbarcode<<std::endl;
*/    if(usedBarcodes.find(newbarcode) == usedBarcodes.end()){
			return false;
		} else {    
//      std::cout<<"find duplicate new barcode: "<<tp->barcode()<<" newbarcode: "<<newbarcode<<std::endl;
			return true;
		}
	}

  /** Fill the histograms using truth information */
  void Fill( const xAOD::TruthParticle * tp )
  {
    usedBarcodes.insert( int(tp->barcode()));//+int(1000000*tp->eta()*tp->eta())+int(1000000*tp->phi()*tp->phi()) ); 
    Barcodes.insert( int(tp->barcode()) ); 
    h_eta->Fill( tp->eta() );
    h_phi->Fill( tp->phi() );
    h_pt->Fill( tp->pt() *1e-3 );
    h_ptHigh->Fill( tp->pt()*1e-3 );
    h_ptLog->Fill( tp->pt() *1e-3 );
   
    double r = tp->hasProdVtx() ? tp->prodVtx()->perp() : 0 ;
    double z = tp->hasProdVtx() ? tp->prodVtx()->z() : 0 ;
    h_radius->Fill( r ); 
    if (fabs(z) < 300.) h_radiuswzcut->Fill( r );

    h_ptVsEta->Fill(  tp->pt() *1e-3, tp->eta()  );
    h_etaVsPhi->Fill( tp->eta(), tp->phi()  );
    h_radiusVsEta->Fill( r, tp->eta() );
    h_radiusVsZ->Fill( r, z );
  }

  /** Fill the histograms using the recosntructed information*/ 
  void Fill( const xAOD::TrackParticle * tp )
  {
    h_eta->Fill( tp->eta() );
    h_phi->Fill( tp->phi() );
    h_pt->Fill( tp->pt() *1e-3 );
    h_ptHigh->Fill( tp->pt() *1e-3 );
    h_ptLog->Fill( tp->pt() *1e-3 );
   
    h_ptVsEta->Fill(  tp->pt() *1e-3, tp->eta()  );
    h_etaVsPhi->Fill( tp->eta(), tp->phi()  );
  }

  /** Add histograms to the output file */
  void Book( EL::Worker* wk )
  {
    for( std::vector<TH1*>::iterator hist = allHists.begin(); hist != allHists.end(); ++hist){
      wk->addOutput( *hist );
    }
  }

  /** Make the efficiency plots and add them to the output file:  
	 * Note that object that you call this from is the numerator  */
  void BuildAndBookEfficiency( const TString prefix, const TrackEfficiencyHists * denom,  EL::Worker* wk ) const
  {
    std::vector<TH1*>::const_iterator pass  =  allHists.cbegin();
    std::vector<TH1*>::const_iterator total =  denom->allHists.cbegin();
    int count(0);
    for( ; pass != allHists.end(); ++pass, ++total, ++count){
      if ( (*pass)->GetDimension() == 1  &&  (*pass)->GetEntries() <= (*total)->GetEntries() ){
        // Due to grid merging does not work for TGraphs we move to TH1 for eff.
        //TGraphAsymmErrors* eff = new TGraphAsymmErrors();
	//eff->Divide( (*pass), (*total), "cl=0.683 b(1,1) mode");
	TH1*  eff = (TH1*) (*pass)->Clone();
        eff->Divide( (*pass), (*total), 1., 1., "cl=0.683 b(1,1) mode");

        TString xname=(*pass)->GetName();
        xname=xname.ReplaceAll(TString("/"), TString("_") );
 
        eff->SetName( prefix + xname );
        eff->SetTitle( (*pass)->GetTitle() );
        eff->GetXaxis()->SetTitle( (*pass)->GetXaxis()->GetTitle() );
        eff->GetYaxis()->SetTitle("Reco/Truth");
        wk->addOutput( eff );
      } else {
	if( (*total)->GetEntries() <= 0 ) continue;


        TString xname=(*pass)->GetName();
        xname=xname.ReplaceAll(TString("/"), TString("_") ); 

        TH1*  clone = (TH1*) (*total)->Clone(  prefix + xname );
        if( (*total)->GetEntries() > 0 ) {
            clone->Divide( (*pass), (*total) );
	} else {
  	    clone->Reset();
        }
        wk->addOutput( clone );
      }
    }
  }
};





#endif 
