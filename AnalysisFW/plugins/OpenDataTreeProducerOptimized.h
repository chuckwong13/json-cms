#ifndef OpenDataTreeProducerOptimized_h
#define OpenDataTreeProducerOptimized_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/JetCorrFactors.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include <iostream>
#include <fstream>

using namespace edm;
using namespace reco;
using namespace std;
using namespace trigger;

class OpenDataTreeProducerOptimized : public edm::EDAnalyzer 
{
  public:

    explicit OpenDataTreeProducerOptimized(edm::ParameterSet const& cfg);
    virtual void beginJob();
    virtual void beginRun(edm::Run const &, edm::EventSetup const& iSetup);
    virtual void analyze(edm::Event const& evt, edm::EventSetup const& iSetup);
    virtual void endRun(edm::Run const &, edm::EventSetup const& iSetup);
    virtual void endJob();
    virtual ~OpenDataTreeProducerOptimized();


  private:  

    // Function to help sort the jet wrt. pT
    static bool cmp_patjets(const pat::Jet &pj1, const pat::Jet &pj2) {
        return pj1.pt() > pj2.pt();
    }

    //---- Configurable parameters --------  
    bool            mIsMCarlo;
    bool            mUseGenInfo;
    bool            mPrintTriggerMenu;
    int             mMinNPFJets;
    double          mMinPFPt, mMinGenPt, mMaxY, mMinJJMass;
    int             mGoodVtxNdof;
    double          mGoodVtxZ; 
    edm::InputTag   mPFak5JetsName;

    
    // ---- PF Jet input tags ----- //
    edm::InputTag   mGenJetsName;
    edm::InputTag   mSrcPFRho;
    edm::InputTag   mPFMET;
    edm::InputTag   muoncollection;
    edm::InputTag electroncollection; 
    edm::InputTag   mOfflineVertices;
    edm::InputTag conversioncollection;
    edm::InputTag beamspotcollection;    
    //---- Trigger----------------------
    std::string                 processName_;
    std::vector<std::string>    triggerNames_;
    std::vector<unsigned int>   triggerIndex_;
    edm::InputTag               triggerResultsTag_;
    HLTConfigProvider           hltConfig_;
    
    // Output variables
    edm::Service<TFileService>  fs;
    TTree                       *mTree;

    
    //---- TTree variables --------
    
    static const UInt_t kMaxNjet = 64;
    static const UInt_t kMaxNtrg = 32;
    static const UInt_t kMaxNmuon=64;
    static const UInt_t kMaxNelectron=64;

    // PF jets
    UInt_t njet;
    Float_t jet_pt[kMaxNjet];
    Float_t jet_CSV[kMaxNjet];
    Float_t jet_eta[kMaxNjet];
    Float_t jet_phi[kMaxNjet];
    Float_t jet_E[kMaxNjet];
    Bool_t jet_tightID[kMaxNjet];
    Float_t jet_area[kMaxNjet];
    Float_t jet_jes[kMaxNjet];
    Int_t jet_igen[kMaxNjet];



                                                                   

    UInt_t nmu;
    Float_t muon_pt[kMaxNmuon];
    Float_t muon_px[kMaxNmuon];
    Float_t muon_py[kMaxNmuon];
    Float_t muon_pz[kMaxNmuon];
    Float_t muon_eta[kMaxNmuon];
    Float_t muon_phi[kMaxNmuon];
    Float_t muon_E[kMaxNmuon];
    Float_t muon_emEt[kMaxNmuon];
    Float_t muon_sumPt[kMaxNmuon];
    Float_t muon_hadEt[kMaxNmuon];
    Int_t muon_isGlobalMuon[kMaxNmuon];
    Int_t muon_isStandAloneMuon[kMaxNmuon];
    Int_t muon_isTrackerMuon[kMaxNmuon];
    Int_t muon_charge[kMaxNmuon];
    Int_t muon_isPFMuon[kMaxNmuon];
    Bool_t muon_tightID[kMaxNmuon];
    Bool_t muon_tightIso[kMaxNmuon];
    Float_t muon_normChi2[kMaxNmuon];
    Float_t muon_dxy[kMaxNmuon];
    Float_t muon_dz[kMaxNmuon];
    Int_t muon_numberOfValidHits[kMaxNmuon];
    Int_t  muon_numberOfValidPixelHits[kMaxNmuon];
    Int_t muon_numberOfMatchedStations[kMaxNmuon];
    Int_t muon_trackerWithMeasurement[kMaxNmuon];
    
    
     UInt_t nel;
    Float_t electron_isoSumPt[kMaxNelectron];
    Float_t electron_isoEcalSumEt[kMaxNelectron];
    Float_t electron_isoHcalSumEt[kMaxNelectron];
    Float_t electron_pt[kMaxNelectron];
    Float_t electron_px[kMaxNelectron];
    Float_t electron_py[kMaxNelectron];
    Float_t electron_pz[kMaxNelectron];
    Float_t electron_eta[kMaxNelectron];
    Float_t electron_phi[kMaxNelectron];
    Float_t electron_E[kMaxNelectron];
    Float_t electron_emEt[kMaxNelectron];
    Float_t electron_hadEt[kMaxNelectron];
    Bool_t electron_tightIso[kMaxNelectron];
    Bool_t electron_tightID[kMaxNelectron];
    Int_t electron_isStandAloneelectron[kMaxNelectron];
    Int_t electron_isTrackerelectron[kMaxNelectron];
    Int_t electron_charge[kMaxNelectron];                               
  Float_t electron_hadronicOverEm[kMaxNelectron];                
  Float_t electron_trackMomentum[kMaxNelectron];                
  Float_t electron_sigmaIetaIeta[kMaxNelectron];                        
 Float_t electron_ecalEnergy[kMaxNelectron];                            
 Float_t electron_dPhiIn[kMaxNelectron];                                
 Float_t electron_dxy[kMaxNelectron];                                  
 Float_t electron_dz[kMaxNelectron];                              	
 Float_t electron_numberOfHits[kMaxNelectron];                          
 Float_t electron_dEtaIn[kMaxNelectron];                                
 Float_t electron_superClusterEta[kMaxNelectron];                                
 Int_t electron_passconversionveto[kMaxNelectron];                                                   

    // Jet composition
    Float_t chf[kMaxNjet];
   	Float_t nhf[kMaxNjet];
   	Float_t phf[kMaxNjet];
   	Float_t elf[kMaxNjet];
   	Float_t muf[kMaxNjet];
   	Float_t hf_hf[kMaxNjet];
   	Float_t hf_phf[kMaxNjet];
   	Int_t hf_hm[kMaxNjet];
   	Int_t hf_phm[kMaxNjet];
   	Int_t chm[kMaxNjet];
   	Int_t nhm[kMaxNjet];
   	Int_t phm[kMaxNjet];
   	Int_t elm[kMaxNjet];
   	Int_t mum[kMaxNjet];   
    Float_t beta[kMaxNjet];   
    Float_t bstar[kMaxNjet];

    // Generated jets
    UInt_t ngen;
    Float_t gen_pt[kMaxNjet];
    Float_t gen_eta[kMaxNjet];
    Float_t gen_phi[kMaxNjet];
    Float_t gen_E[kMaxNjet];

    // Event identification
    UInt_t run;
    UInt_t lumi;
    ULong64_t event;

    // Triggers
    UInt_t ntrg;
    Bool_t triggers[kMaxNtrg];
    std::vector<std::string> triggernames;
    UInt_t prescales[kMaxNtrg];

    // MET, SuMET, rho, eventfilter
    Float_t met;
    Float_t met_phi;
    Float_t sumet;
    Float_t rho;

    // MC variables
    Float_t pthat;
    Float_t mcweight;

    // Jet correction labels
    std::string mJetCorr_ak5;
    std::string jsonname;


    std::ofstream file_id;
    Json::Value theevent;
   Json::StyledWriter styledWriter; 
};

#endif
