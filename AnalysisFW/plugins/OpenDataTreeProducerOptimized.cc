
// Forked from SMPJ Analysis Framework
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/SMPJAnalysisFW
// https://github.com/cms-smpj/SMPJ/tree/v1.0


#include <typeinfo>
#include <iostream>
#include <sstream>
#include <istream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <functional>
#include "TTree.h"
#include <vector>
#include <cassert>
#include <TLorentzVector.h>
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "json/json.h"

#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "OpenDataTreeProducerOptimized.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"

#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"//for muon namespace

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/JetExtendedAssociation.h"
#include "DataFormats/JetReco/interface/JetID.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"

#include "RecoJets/JetAssociationProducers/src/JetTracksAssociatorAtVertex.h"


using pat::Muon;
using pat::MuonCollection;
using namespace std;
using namespace reco;
using namespace edm;
using namespace pat;
OpenDataTreeProducerOptimized::OpenDataTreeProducerOptimized(edm::ParameterSet const &cfg) {
	mMinPFPt           = cfg.getParameter<double>                    ("minPFPt");
	mMinJJMass         = cfg.getParameter<double>                    ("minJJMass");
	mMaxY              = cfg.getParameter<double>                    ("maxY");
	mMinNPFJets        = cfg.getParameter<int>                       ("minNPFJets");
	mPFak5JetsName     = cfg.getParameter<edm::InputTag>             ("pfak5jets");
	mOfflineVertices   = cfg.getParameter<edm::InputTag>             ("offlineVertices");
	mGoodVtxNdof       = cfg.getParameter<double>                    ("goodVtxNdof");
	mGoodVtxZ          = cfg.getParameter<double>                    ("goodVtxZ");
	mSrcPFRho          = cfg.getParameter<edm::InputTag>             ("srcPFRho");
	mPFMET             = cfg.getParameter<edm::InputTag>             ("pfmet");
	mGenJetsName       = cfg.getUntrackedParameter<edm::InputTag>    ("genjets",edm::InputTag(""));
	mPrintTriggerMenu  = cfg.getUntrackedParameter<bool>             ("printTriggerMenu",false);
	mIsMCarlo          = cfg.getUntrackedParameter<bool>             ("isMCarlo",false);
	mUseGenInfo        = cfg.getUntrackedParameter<bool>             ("useGenInfo",false);
	mMinGenPt          = cfg.getUntrackedParameter<double>           ("minGenPt",30);
	processName_       = cfg.getParameter<std::string>               ("processName");
	triggerNames_      = cfg.getParameter<std::vector<std::string> > ("triggerNames");
	triggerResultsTag_ = cfg.getParameter<edm::InputTag>             ("triggerResults");
	mJetCorr_ak5       = cfg.getParameter<std::string>               ("jetCorr_ak5");
	muoncollection = cfg.getParameter<edm::InputTag> ("recomuons");
	electroncollection=cfg.getParameter<edm::InputTag>("recoelectrons");
	beamspotcollection=cfg.getParameter<edm::InputTag>("recobeamspot");
	conversioncollection=cfg.getParameter<edm::InputTag> ("recoconversion");
  jsonname=cfg.getParameter<std::string>("jsoninput"); 
}

void OpenDataTreeProducerOptimized::beginJob() {
	mTree = fs->make< TTree >("OpenDataTree", "OpenDataTree");
	file_id.open("/home/cms-opendata/WorkingArea/CMSSW_5_3_32/src/json-cms/AnalysisFW/python/outputjsons/"+jsonname,std::ios_base::app | std::ios_base::out);
 cout << "File name " << "/home/cms-opendata/WorkingArea/CMSSW_5_3_32/src/json-cms/AnalysisFW/python/outputjsons/"+jsonname << endl;

	// Variables of the flat tuple
	mTree->Branch("njet", &njet, "njet/i");
	mTree->Branch("jet_pt", jet_pt, "jet_pt[njet]/F");
	mTree->Branch("jet_CSV", jet_CSV, "jet_CSV[njet]/F");
	mTree->Branch("jet_eta", jet_eta, "jet_eta[njet]/F");
	mTree->Branch("jet_phi", jet_phi, "jet_phi[njet]/F");
	mTree->Branch("jet_E", jet_E, "jet_E[njet]/F");   
	mTree->Branch("jet_tightID", jet_tightID, "jet_tightID[njet]/O");
	mTree->Branch("jet_area", jet_area, "jet_area[njet]/F");
	mTree->Branch("jet_jes", jet_jes, "jet_jes[njet]/F");
	mTree->Branch("jet_igen", jet_igen, "jet_igen[njet]/I");


	mTree->Branch("ngen", &ngen, "ngen/i");
	mTree->Branch("gen_pt", gen_pt, "gen_pt[ngen]/F");
	mTree->Branch("gen_eta", gen_eta, "gen_eta[ngen]/F");
	mTree->Branch("gen_phi", gen_phi, "gen_phi[ngen]/F");
	mTree->Branch("gen_E", gen_E, "gen_E[ngen]/F");

	mTree->Branch("run", &run, "run/i");
	mTree->Branch("lumi", &lumi, "lumi/i");
	mTree->Branch("event", &event, "event/l");
	mTree->Branch("ntrg", &ntrg, "ntrg/i");
	mTree->Branch("triggers", triggers, "triggers[ntrg]/O");
	mTree->Branch("triggernames", &triggernames);
	mTree->Branch("prescales", prescales, "prescales[ntrg]/i");
	mTree->Branch("met", &met, "met/F");
	mTree->Branch("met_phi", &met_phi, "met_phi/F");
	mTree->Branch("sumet", &sumet, "sumet/F");
	mTree->Branch("rho", &rho, "rho/F");
	mTree->Branch("pthat", &pthat, "pthat/F");
	mTree->Branch("mcweight", &mcweight, "mcweight/F");

	mTree->Branch("chf", chf, "chf[njet]/F");   
	mTree->Branch("nhf", nhf, "nhf[njet]/F");   
	mTree->Branch("phf", phf, "phf[njet]/F");   
	mTree->Branch("elf", elf, "elf[njet]/F");   
	mTree->Branch("muf", muf, "muf[njet]/F");   
	mTree->Branch("hf_hf", hf_hf, "hf_hf[njet]/F");   
	mTree->Branch("hf_phf", hf_phf, "hf_phf[njet]/F");   
	mTree->Branch("hf_hm", hf_hm, "hf_hm[njet]/i");    
	mTree->Branch("hf_phm", hf_phm, "hf_phm[njet]/i");
	mTree->Branch("chm", chm, "chm[njet]/i");   
	mTree->Branch("nhm", nhm, "nhm[njet]/i");   
	mTree->Branch("phm", phm, "phm[njet]/i");   
	mTree->Branch("elm", elm, "elm[njet]/i");   
	mTree->Branch("mum", mum, "mum[njet]/i");
	mTree->Branch("beta", beta, "beta[njet]/F");   
	mTree->Branch("bstar", bstar, "bstar[njet]/F");

	mTree->Branch("nmu", &nmu, "nmu/i");
	mTree->Branch("muon_pt", muon_pt, "muon_pt[nmu]/F") ;
	mTree->Branch("muon_px", muon_px, "muon_px[nmu]/F") ;
	mTree->Branch("muon_py", muon_py, "muon_py[nmu]/F") ;
	mTree->Branch("muon_pz", muon_pz, "muon_pz[nmu]/F") ;
	mTree->Branch("muon_eta", muon_eta, "muon_eta[nmu]/F") ;
	mTree->Branch("muon_phi", muon_phi, "muon_phi[nmu]/F") ;
	mTree->Branch("muon_emEt", muon_emEt, "muon_emEt[nmu]/F") ;
	mTree->Branch("muon_sumPt", muon_sumPt ,"muon_sumPt[nmu]/F") ;
	mTree->Branch("muon_hadEt", muon_hadEt, "muon_hadEt[nmu]/F") ;
	mTree->Branch("muon_isPFMuon", muon_isPFMuon, "muon_isPFMuon[nmu]/i") ;
	mTree->Branch("muon_tightID", muon_tightID, "muon_tightID[nmu]/i") ;
	mTree->Branch("muon_tightIso", muon_tightIso, "muon_tightIso[nmu]/i") ;
	mTree->Branch("muon_isGlobalMuon", muon_isGlobalMuon, "muon_isGlobalMuon[nmu]/i") ;
	mTree->Branch("muon_isStandAloneMuon", muon_isStandAloneMuon, "muon_isStandAloneMuon[nmu]/i") ;
	mTree->Branch("muon_isTrackerMuon", muon_isTrackerMuon, "muon_isTrackerMuon[nmu]/i") ;
	mTree->Branch("muon_normChi2", muon_normChi2, "muon_normChi2[nmu]/F") ;
	mTree->Branch("muon_numberOfValidHits", muon_numberOfValidHits, "muon_numberOfValidHits[nmu]/i") ;
	mTree->Branch("muon_dxy", muon_dxy, "muon_dxy[nmu]/F") ;
	mTree->Branch("muon_dz", muon_dz, "muon_dz[nmu]/F") ;
	mTree->Branch("muon_numberOfValidPixelHits", muon_numberOfValidPixelHits, "muon_numberOfValidPixelHits[nmu]/i") ;
	mTree->Branch("muon_numberOfMatchedStations", muon_numberOfMatchedStations, "muon_numberOfMatchedStations[nmu]/i") ;
	mTree->Branch("muon_trackerWithMeasurement", muon_trackerWithMeasurement, "muon_trackerWithMeasurement[nmu]/i") ;
	mTree->Branch("muon_charge", muon_charge, "muon_charge[nmu]/i") ;


	mTree->Branch("nel", &nel, "nel/i");
	mTree->Branch("electron_pt", electron_pt, "electron_pt[nel]/F");
	mTree->Branch("electron_px", electron_px, "electron_px[nel]/F") ;
	mTree->Branch("electron_py", electron_py, "electron_py[nel]/F") ;
	mTree->Branch("electron_pz", electron_pz, "electron_pz[nel]/F") ;
	mTree->Branch("electron_eta", electron_eta, "electron_eta[nel]/F") ;
	mTree->Branch("electron_phi", electron_phi, "electron_phi[nel]/F") ;
	mTree->Branch("electron_E", electron_E, "electron_E[nel]/F") ;
	mTree->Branch("electron_charge", electron_charge, "electron_charge[nel]/i") ;
	mTree->Branch("electron_tightID", electron_tightID, "electron_tightID[nel]/i") ;
	mTree->Branch("electron_tightIso", electron_tightIso, "electron_tightIso[nel]/i") ;
	mTree->Branch("electron_hadronicOverEm", electron_hadronicOverEm , "electron_hadronicOverEm[nel]/F");             
	mTree->Branch("electron_sigmaIetaIeta", electron_sigmaIetaIeta, "electron_sigmaIetaIeta[nel]/F");               
	mTree->Branch("electron_ecalEnergy",  electron_ecalEnergy, "electron_ecalEnergy[nel]/F");
	mTree->Branch("electron_trackMomentum",  electron_trackMomentum, "electron_trackMomentum[nel]/F");
	mTree->Branch("electron_dPhiIn",   electron_dPhiIn, "electron_dPhiIn[nel]/F");                     
	mTree->Branch("electron_dxy",    electron_dxy, "electron_dxy[nel]/F");                       
	mTree->Branch("electron_dz",   electron_dz, "electron_dz[nel]/F");
	mTree->Branch("electron_numberOfHits",   electron_numberOfHits, "electron_numberOfHits[nel]/F");               
	mTree->Branch("electron_dEtaIn",   electron_dEtaIn, "electron_dEtaIn[nel]/F");                     
	mTree->Branch("electron_superClusterEta",   electron_superClusterEta, "electron_superClusterEta[nel]/F");                     
	mTree->Branch("electron_passconversionveto", electron_passconversionveto, "electron_passconversionveto[nel]/i");                 
	mTree->Branch("electron_isoSumPt", electron_isoSumPt, "electron_isoSumPt[nel]/F");
	mTree->Branch("electron_isoEcalSumEt",electron_isoEcalSumEt , "electron_isoEcalSumEt[nel]/F");
	mTree->Branch("electron_isoHcalSumEt",electron_isoHcalSumEt, "electron_isoHcalSumEt[nel]/F");
 
 

}

void OpenDataTreeProducerOptimized::endJob() {
	cout << " ENTRA AQUI " << endl;
	file_id << styledWriter.write(theevent);

	file_id.close();


}


void OpenDataTreeProducerOptimized::beginRun(edm::Run const &iRun,
		edm::EventSetup const &iSetup) {
   
	// Mapping trigger indices 
	bool changed(true);
	if (hltConfig_.init(iRun, iSetup, processName_, changed) && changed) {

		// List of trigger names and indices 
		// are not emptied between events, must be done here
		triggerIndex_.clear();
		triggernames.clear();

		// Iterate over all active triggers of the AOD file
		auto name_list = hltConfig_.triggerNames();
		for (std::string name_to_search: triggerNames_) {

			// Find the version of jet trigger that is active in this run 
			for (std::string name_candidate: name_list) {

				// Match the prefix to the full name (eg. HLT_Jet30 to HLT_Jet30_v10)
				if ( name_candidate.find(name_to_search + "_v") != std::string::npos ) {
					// Save index corresponding to the trigger
					triggerIndex_.push_back(hltConfig_.triggerIndex(name_candidate));

					// Save the trigger name
					triggernames.push_back("jt" + name_to_search.substr(7, string::npos));
					break;            
				}
			}
		}
	}

	// Retrieve cross section of the simulated process
	mcweight = 0;
	if (mIsMCarlo) {

		edm::Handle<GenRunInfoProduct> genRunInfo;
		iRun.getByLabel("generator", genRunInfo );

		// Save only the cross section, since the total number of 
		// generated events is not available in this context (!!)
		mcweight = genRunInfo->crossSection();
		std::cout << "Cross section: " <<  mcweight << std::endl;
	}



}


void OpenDataTreeProducerOptimized::analyze(edm::Event const &event_obj,
		edm::EventSetup const &iSetup) {


	// Event info
	run = event_obj.id().run();
	lumi = event_obj.luminosityBlock();
	event = event_obj.id().event();

	// Json file





	string eventstring=std::to_string(event);
	string lumistring=std::to_string(lumi);
	string runstring=std::to_string(run);



	// Triggers
	edm::Handle<edm::TriggerResults>   triggerResultsHandle_;
	event_obj.getByLabel(triggerResultsTag_, triggerResultsHandle_);

	// Sanity checks
	assert(triggerResultsHandle_.isValid() && "Error in getting TriggerResults from Event!");
	assert(triggerResultsHandle_->size() == hltConfig_.size() && "Size mismatch between triggerResultsHandle_ and hltConfig_");

	// Number of triggers to be saved
	ntrg = triggerIndex_.size();

	// Iterate only over the selected jet triggers
	for (unsigned itrig = 0; itrig < ntrg; itrig++) {

		// Trigger bit
		Bool_t isAccepted = triggerResultsHandle_->accept(triggerIndex_[itrig]);
		triggers[itrig] = isAccepted;

		// Trigger prescales are retrieved using the trigger name
		std::string trgName = hltConfig_.triggerName(triggerIndex_[itrig]);
		const std::pair< int, int > prescalePair(hltConfig_.prescaleValues(event_obj, iSetup, trgName));

		// Total prescale: PreL1*PreHLT 
		prescales[itrig] = prescalePair.first*prescalePair.second;   
	}    

	// Rho
	Handle< double > rho_handle;
	event_obj.getByLabel(mSrcPFRho, rho_handle);
	rho = *rho_handle;


	// Generator Info

	// Retrieve pthat and mcweight (only MC)
	pthat = 0;
	if (mIsMCarlo && mUseGenInfo) {
		Handle< GenEventInfoProduct > hEventInfo;
		event_obj.getByLabel("generator", hEventInfo);

		// Monte Carlo weight (NOT AVAILABLE FOR 2011 MC!!)
		//mcweight = hEventInfo->weight();

		// Pthat 
		if (hEventInfo->hasBinningValues()) {
			pthat = hEventInfo->binningValues()[0];
		}
	}

	// Generator-level jets
	ngen = 0;
	if (mIsMCarlo) {

		Handle< GenJetCollection > genjets;
		event_obj.getByLabel(mGenJetsName, genjets);

		// Index of the simulated jet
		int gen_index = 0; 

		for (GenJetCollection::const_iterator i_gen = genjets->begin(); i_gen != genjets->end(); i_gen++)  {

			// pT and rapidity selection
			if (i_gen->pt() > mMinGenPt && fabs(i_gen->y()) < mMaxY) {
				gen_pt[gen_index] = i_gen->pt();
				gen_eta[gen_index] = i_gen->eta();
				gen_phi[gen_index] = i_gen->phi();
				gen_E[gen_index] = i_gen->energy();
				gen_index++;
			}
		}

		// Number of generated jets in this event
		ngen = gen_index;
	}

	// Vertex Info
	Handle<reco::VertexCollection> recVtxs;
	event_obj.getByLabel(mOfflineVertices, recVtxs);


	//Muons

	edm::Handle<reco::MuonCollection> muonsHandle;
	event_obj.getByLabel(muoncollection, muonsHandle);
	reco::Vertex goodvertex;
	int totalmuons=0;

	//Electrons

	edm::Handle<reco::GsfElectronCollection> electronsHandle;
	event_obj.getByLabel(electroncollection, electronsHandle);
	int totalelectrons=0;



	bool istheregoodvertex=false;
	for (unsigned ivtx = 0; ivtx < recVtxs->size(); ivtx++) {
		reco::Vertex vertex = (*recVtxs)[ivtx];

		// Loop over tracks associated with the vertex
		if (!(vertex.isFake()) && 
				vertex.ndof() >= mGoodVtxNdof && 

				fabs(vertex.z()) <= mGoodVtxZ) {
			goodvertex=vertex;
			istheregoodvertex=true;
			break;
		}}                                                                 

  
   //if ((muonsHandle->size() + electronsHandle->size())  >=1) onelepton=true;
   
	for (unsigned i = 0; i < muonsHandle->size(); ++i)
	{ 

		if (!(*muonsHandle)[i].globalTrack().isNull()){



			muon_pt[totalmuons]=(*muonsHandle)[i].pt() ;
			muon_px[totalmuons]=(*muonsHandle)[i].px();
			muon_py[totalmuons]= (*muonsHandle)[i].py() ;
			muon_pz[totalmuons]= (*muonsHandle)[i].pz();
			muon_eta[totalmuons]= (*muonsHandle)[i].eta();
			muon_phi[totalmuons]=(*muonsHandle)[i].phi();
			muon_E[totalmuons]= (*muonsHandle)[i].energy();
			muon_sumPt[totalmuons]= (*muonsHandle)[i].isolationR03().sumPt ;
			muon_emEt[totalmuons]= (*muonsHandle)[i].isolationR03().emEt ;
			muon_hadEt[totalmuons]= (*muonsHandle)[i].isolationR03().hadEt ;
			muon_isGlobalMuon[totalmuons]= (*muonsHandle)[i].isGlobalMuon() ;
			muon_isStandAloneMuon[totalmuons]= (*muonsHandle)[i].isStandAloneMuon() ;
			muon_isTrackerMuon[totalmuons]= (*muonsHandle)[i].isTrackerMuon() ;
			muon_charge[totalmuons]= (*muonsHandle)[i].charge();
			muon_normChi2[totalmuons]= (*muonsHandle)[i].globalTrack()->normalizedChi2()  ;
			muon_numberOfValidHits[totalmuons]= (*muonsHandle)[i].globalTrack()->hitPattern().numberOfValidMuonHits();
			muon_numberOfMatchedStations[totalmuons]=  (*muonsHandle)[i].numberOfMatchedStations() ;
			muon_dz[totalmuons]= (*muonsHandle)[i].muonBestTrack()->dz(goodvertex.position()); //->dz(goodvertex.position());
			muon_dxy[totalmuons]= (*muonsHandle)[i].muonBestTrack()->dxy(goodvertex.position());//(goodvertex.position());
			muon_trackerWithMeasurement[totalmuons]= (*muonsHandle)[i].innerTrack()->hitPattern().trackerLayersWithMeasurement() ;
			muon_numberOfValidPixelHits[totalmuons]= (*muonsHandle)[i].innerTrack()->hitPattern().numberOfValidPixelHits() ;
			muon_isPFMuon[totalmuons]= (*muonsHandle)[i].isPFMuon();

			//https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Tight_Muon
			muon_tightID[i]=0;
			if (muon_isGlobalMuon[i] && muon_isPFMuon[i] &&   muon_normChi2[i] < 10 &&  muon_numberOfValidHits[i] > 0 &&  muon_numberOfMatchedStations[i] > 1 && fabs(muon_dxy[i])  < 0.2 && fabs(muon_dz[i])  < 0.5 && muon_numberOfValidPixelHits[i] > 0 &&   muon_trackerWithMeasurement[i] > 5){
				muon_tightID[totalmuons]=1;
			}

			muon_tightIso[i]=0;
			if ( ((muon_emEt[i] + muon_sumPt[i] + muon_hadEt[i])/ muon_pt[i])  < 0.10){
				muon_tightIso[totalmuons]=1;

			}

			// fill json

			std::string muonstring = std::to_string(totalmuons);
			theevent["Event"][eventstring]["muon"][muonstring]["pt"]=muon_pt[totalmuons] ;
			theevent["Event"][eventstring]["muon"][muonstring]["px"]=muon_px[totalmuons] ;
			theevent["Event"][eventstring]["muon"][muonstring]["py"]=muon_py[totalmuons] ;
			theevent["Event"][eventstring]["muon"][muonstring]["pz"]=muon_pz[totalmuons] ;
			theevent["Event"][eventstring]["muon"][muonstring]["phi"]=muon_phi[totalmuons] ;
			theevent["Event"][eventstring]["muon"][muonstring]["eta"]=muon_eta[totalmuons] ;
			theevent["Event"][eventstring]["muon"][muonstring]["tightID"]=muon_tightID[totalmuons] ;
			theevent["Event"][eventstring]["muon"][muonstring]["tightIso"]=muon_tightIso[totalmuons] ;
			
			
			totalmuons++;
			
			



		}



	}

	nmu=totalmuons;


	edm::Handle<reco::ConversionCollection> hConversions;
	event_obj.getByLabel(conversioncollection, hConversions);

	edm::Handle<reco::BeamSpot> bsHandle;
	event_obj.getByLabel(beamspotcollection, bsHandle);
	const reco::BeamSpot &thebs = *bsHandle.product();




	for (unsigned i = 0; i < electronsHandle->size(); ++i)
	{ 
		if (!(*electronsHandle)[i].gsfTrack().isNull()){     

			electron_pt[totalelectrons]=(*electronsHandle)[i].pt() ;
			electron_px[totalelectrons]=(*electronsHandle)[i].px();
			electron_py[totalelectrons]=(*electronsHandle)[i].py();
			electron_pz[totalelectrons]=(*electronsHandle)[i].pz();
			electron_eta[totalelectrons]= (*electronsHandle)[i].eta();
			electron_phi[totalelectrons]= (*electronsHandle)[i].phi();
			electron_E[totalelectrons]= (*electronsHandle)[i].energy();
			electron_charge[totalelectrons]= (*electronsHandle)[i].charge();
			electron_hadronicOverEm[totalelectrons]= (*electronsHandle)[i].hadronicOverEm() ;
			electron_sigmaIetaIeta[totalelectrons]= (*electronsHandle)[i].sigmaIetaIeta() ;
			electron_ecalEnergy[totalelectrons]= (*electronsHandle)[i].ecalEnergy() ;
			electron_trackMomentum[totalelectrons]= (*electronsHandle)[i].ecalEnergy() /  (*electronsHandle)[i].eSuperClusterOverP();
			electron_dPhiIn[totalelectrons]= (*electronsHandle)[i].deltaPhiSuperClusterTrackAtVtx() ;
			electron_dxy[totalelectrons]= (*electronsHandle)[i].gsfTrack()->dxy(goodvertex.position());
			electron_dz[totalelectrons]= (*electronsHandle)[i].gsfTrack()->dz(goodvertex.position()) ;
			electron_numberOfHits[totalelectrons]= (*electronsHandle)[i].gsfTrack()->trackerExpectedHitsInner().numberOfHits() ;
			electron_dEtaIn[totalelectrons]  =(*electronsHandle)[i].deltaEtaSuperClusterTrackAtVtx();
			electron_passconversionveto[i]=!ConversionTools::hasMatchedConversion((*electronsHandle)[i],hConversions, thebs.position(), true, 2.0, 1e-6, 0) ;
			electron_superClusterEta[totalelectrons]=(*electronsHandle)[i].superCluster()->eta() ;
			electron_isoSumPt[totalelectrons]=(*electronsHandle)[i].dr03TkSumPt();
			electron_isoEcalSumEt[totalelectrons]=(*electronsHandle)[i].dr03EcalRecHitSumEt();
			electron_isoHcalSumEt[totalelectrons]=(*electronsHandle)[i].dr03HcalTowerSumEt();


			electron_tightID[totalelectrons]=0;
			electron_tightIso[totalelectrons]=0;


			//barrel
			if (fabs( electron_superClusterEta[i])<= 1.479){

				if (fabs(electron_dEtaIn[i]) < 0.004 &&  fabs(electron_dPhiIn[i]) < 0.03  && electron_sigmaIetaIeta[i] < 0.01  &&  fabs(electron_dxy[i]) < 0.02 && fabs(electron_dz[i]) < 0.1 && electron_hadronicOverEm[i] < 0.12 &&  fabs(1/electron_ecalEnergy[i] - 1/electron_trackMomentum[i]) < 0.05  && electron_numberOfHits[i] <=0 && electron_passconversionveto[i]){

					electron_tightID[totalelectrons]=1;

				}
			}     

			//endcap
			if (fabs( electron_superClusterEta[i])> 1.479 || fabs( electron_superClusterEta[i]) < 2.5){

				if (fabs(electron_dEtaIn[i]) < 0.005 &&  fabs(electron_dPhiIn[i]) < 0.02  && electron_sigmaIetaIeta[i] < 0.03  &&  fabs(electron_dxy[i]) < 0.02 && fabs(electron_dz[i]) < 0.1 && electron_hadronicOverEm[i] < 0.10 &&  fabs(1/electron_ecalEnergy[i] - 1/electron_trackMomentum[i]) < 0.05  && electron_numberOfHits[i] <=0 && electron_passconversionveto[i]){

					electron_tightID[totalelectrons]=1;
				}
			}     

			if ((electron_isoSumPt[i] + electron_isoEcalSumEt[i] + electron_isoHcalSumEt[i]) / electron_pt[i] < 0.10){

				electron_tightIso[totalelectrons]=1;

			}



			std::string electronstring = std::to_string(totalelectrons);
			theevent["Event"][eventstring]["electron"][electronstring]["pt"]=electron_pt[totalelectrons];
			theevent["Event"][eventstring]["electron"][electronstring]["px"]=electron_px[totalelectrons];
			theevent["Event"][eventstring]["electron"][electronstring]["py"]=electron_py[totalelectrons];
			theevent["Event"][eventstring]["electron"][electronstring]["pz"]=electron_pz[totalelectrons];
			theevent["Event"][eventstring]["electron"][electronstring]["phi"]=electron_phi[totalelectrons];
			theevent["Event"][eventstring]["electron"][electronstring]["eta"]=electron_eta[totalelectrons];
			theevent["Event"][eventstring]["electron"][electronstring]["tightID"]=electron_tightID[totalelectrons];
			theevent["Event"][eventstring]["electron"][electronstring]["tightIso"]=electron_tightIso[totalelectrons] ;


       
       			totalelectrons++;



		}
	}




	nel=totalelectrons;



	// PF AK5 Jets

	edm::Handle<reco::PFJetCollection> ak5_handle;
	event_obj.getByLabel(mPFak5JetsName, ak5_handle);
	const JetCorrector* corrector_ak5 = JetCorrector::getJetCorrector(mJetCorr_ak5, iSetup);

	// Jet Track Association (JTA)
	edm::Handle <reco::TrackCollection> tracks_h;
	event_obj.getByLabel ("generalTracks", tracks_h);
	std::auto_ptr<reco::JetTracksAssociation::Container> tracksInJets (new reco::JetTracksAssociation::Container (reco::JetRefBaseProd(ak5_handle)));
	// format inputs
	std::vector <edm::RefToBase<reco::Jet> > allJets;
	allJets.reserve (ak5_handle->size());
	for (unsigned i=0; i < ak5_handle->size(); ++i)
	{
		edm::RefToBase<reco::Jet> jetRef(edm::Ref<reco::PFJetCollection>(ak5_handle, i));
		allJets.push_back(jetRef);
	}
	std::vector <reco::TrackRef> allTracks;
	allTracks.reserve(tracks_h->size());
	for (unsigned i = 0; i < tracks_h->size(); ++i) 
		allTracks.push_back (reco::TrackRef(tracks_h, i));
	// run JTA algorithm
	JetTracksAssociationDRVertex mAssociator(0.5); // passed argument: 0.5 cone size
	mAssociator.produce (&*tracksInJets, allJets, allTracks);

	// Index of the selected jet 
	int ak5_index = 0;

	// Jet energy correction factor
	double jec = -1.0;

	// Jets will be unsorted in pT after applying JEC,  
	// therefore store corrected jets in a new collection (map): key (double) is pT * -1 (key), 
	// value (std::pair<PFJet*, double>) is pair of original jet iterator and corresponding JEC factor
	std::map<double, std::pair<reco::PFJetCollection::const_iterator, double> > sortedJets;
	for (auto i_ak5jet_orig = ak5_handle->begin(); i_ak5jet_orig != ak5_handle->end(); ++i_ak5jet_orig) {
		// take jet energy correction and get corrected pT
		jec = corrector_ak5->correction(*i_ak5jet_orig, event_obj, iSetup);
		// Multiply pT by -1 in order to have largest pT jet first (sorted in ascending order by default)
		sortedJets.insert(std::pair<double, std::pair<reco::PFJetCollection::const_iterator, double> >(-1 * i_ak5jet_orig->pt() * jec, std::pair<reco::PFJetCollection::const_iterator, double>(i_ak5jet_orig, jec)));
	}

	// Iterate over the jets (sorted in pT) of the event
	for (auto i_ak5jet_orig = sortedJets.begin(); i_ak5jet_orig != sortedJets.end(); ++i_ak5jet_orig) {

		// Apply jet energy correction "on the fly":
		// copy original (uncorrected) jet;
		PFJet corjet = *((i_ak5jet_orig->second).first);
		// take stored JEC factor
		jec = (i_ak5jet_orig->second).second;
		// apply JEC
		corjet.scaleEnergy(jec);
		// pointer for further use
		const PFJet* i_ak5jet = &corjet;

		// Skip the current iteration if jet is not selected
		if (fabs(i_ak5jet->y()) > mMaxY || 
				(i_ak5jet->pt()) < mMinPFPt) {
			continue;
		}

		// Computing beta and beta*

		// Get tracks
		reco::TrackRefVector tracks = reco::JetTracksAssociation::getValue(*tracksInJets, *((i_ak5jet_orig->second).first));

		float sumTrkPt(0.0), sumTrkPtBeta(0.0),sumTrkPtBetaStar(0.0);
		beta[ak5_index] = 0.0;
		bstar[ak5_index] = 0.0;

		// Loop over tracks of the jet
		for(auto i_trk = tracks.begin(); i_trk != tracks.end(); i_trk++) {

			if (recVtxs->size() == 0) break;

			// Sum pT
			sumTrkPt += (*i_trk)->pt();

			// Loop over vertices
			for (unsigned ivtx = 0; ivtx < recVtxs->size(); ivtx++) {
				reco::Vertex vertex = (*recVtxs)[ivtx];

				// Loop over tracks associated with the vertex
				bool flagBreak = false;
				if (!(vertex.isFake()) && 
						vertex.ndof() >= mGoodVtxNdof && 
						fabs(vertex.z()) <= mGoodVtxZ) {

					for(auto i_vtxTrk = vertex.tracks_begin(); i_vtxTrk != vertex.tracks_end(); ++i_vtxTrk) {

						// Match the jet track to the track from the vertex
						reco::TrackRef trkRef(i_vtxTrk->castTo<reco::TrackRef>());

						// Check for matching vertices
						if (trkRef == (*i_trk)) {
							if (ivtx == 0) {
								sumTrkPtBeta += (*i_trk)->pt();
							}
							else {
								sumTrkPtBetaStar += (*i_trk)->pt();
							}
							flagBreak = true;
							break;
						} 
					} 
					if(flagBreak)
						break;
				} 
			} 
		}
		if (sumTrkPt > 0) {
			beta[ak5_index]   = sumTrkPtBeta/sumTrkPt;
			bstar[ak5_index]  = sumTrkPtBetaStar/sumTrkPt;
		} 


		// Jet composition
		// (all energy fractions have to be multiplied by the JEC factor)
		chf[ak5_index]     = i_ak5jet->chargedHadronEnergyFraction() * jec;
		nhf[ak5_index]     = (i_ak5jet->neutralHadronEnergyFraction() + i_ak5jet->HFHadronEnergyFraction()) * jec;
		phf[ak5_index]     = i_ak5jet->photonEnergyFraction() * jec;
		elf[ak5_index]     = i_ak5jet->electronEnergyFraction() * jec;
		muf[ak5_index]     = i_ak5jet->muonEnergyFraction() * jec;
		hf_hf[ak5_index]   = i_ak5jet->HFHadronEnergyFraction() * jec;
		hf_phf[ak5_index]  = i_ak5jet->HFEMEnergyFraction() * jec;
		hf_hm[ak5_index]   = i_ak5jet->HFHadronMultiplicity();
		hf_phm[ak5_index]  = i_ak5jet->HFEMMultiplicity();
		chm[ak5_index]     = i_ak5jet->chargedHadronMultiplicity();
		nhm[ak5_index]     = i_ak5jet->neutralHadronMultiplicity();
		phm[ak5_index]     = i_ak5jet->photonMultiplicity();
		elm[ak5_index]     = i_ak5jet->electronMultiplicity();
		mum[ak5_index]     = i_ak5jet->muonMultiplicity();
		int npr      = i_ak5jet->chargedMultiplicity() + i_ak5jet->neutralMultiplicity();

		bool isHighEta = fabs(i_ak5jet->eta()) > 2.4;
		bool isLowEta = fabs(i_ak5jet->eta()) <= 2.4 && 
			nhf[ak5_index] < 0.9 &&
			phf[ak5_index] < 0.9 && 
			elf[ak5_index] < 0.99 && 
			chf[ak5_index] > 0 && 
			chm[ak5_index] > 0;
		bool tightID =  npr > 1 && 
			phf[ak5_index] < 0.99 && 
			nhf[ak5_index] < 0.99 &&
			(isLowEta || isHighEta);


		// Variables of the tuple
		jet_tightID[ak5_index] = tightID;
		jet_area[ak5_index] = i_ak5jet->jetArea();
		jet_jes[ak5_index] = jec; // JEC factor

		// p4 is already corrected!
		auto p4 = i_ak5jet->p4();
		jet_pt[ak5_index]   = p4.Pt();
		jet_eta[ak5_index]  = p4.Eta();
		jet_phi[ak5_index]  = p4.Phi();
		jet_E[ak5_index]    = p4.E(); 

		// Matching a GenJet to this PFjet
		jet_igen[ak5_index] = 0;
		if (mIsMCarlo && ngen > 0) {

			// Index of the generated jet matching this PFjet
			jet_igen[ak5_index] = -1; // is -1 if no matching jet

			// Search generated jet with minimum distance to this PFjet   
			float r2min(999);
			for (unsigned int gen_index = 0; gen_index != ngen; gen_index++) {
				double deltaR2 = reco::deltaR2( jet_eta[ak5_index], 
						jet_phi[ak5_index],
						gen_eta[gen_index], 
						gen_phi[gen_index]);
				if (deltaR2 < r2min) {
					r2min = deltaR2;
					jet_igen[ak5_index] = gen_index;
				}
			}
		}


		std::string jetstring = std::to_string(ak5_index);
		theevent["Event"][eventstring]["jet"][jetstring]["pt"]=jet_pt[ak5_index];
		theevent["Event"][eventstring]["jet"][jetstring]["phi"]=jet_phi[ak5_index];
//		theevent["Event"][eventstring]["jet"][jetstring]["CSV"]=jet_CSV[ak5_index];
		theevent["Event"][eventstring]["jet"][jetstring]["eta"]=jet_eta[ak5_index];
		theevent["Event"][eventstring]["jet"][jetstring]["tightID"]=jet_tightID[ak5_index];
		ak5_index++;

	}  
	// Number of selected jets in the event
	njet = ak5_index;    

	edm::Handle<reco::JetTagCollection> bTagHandle;
	event_obj.getByLabel("combinedSecondaryVertexBJetTags", bTagHandle);
	const reco::JetTagCollection & bTags = *(bTagHandle.product());

	// Loop over jets and study b tag info.
	for (int j=0; j!= (int) njet; ++j){
		TLorentzVector thejet;
		thejet.SetPtEtaPhiE(jet_pt[j], jet_eta[j], jet_phi[j], jet_E[j]);
		float btagdisc=-100;

		for (int i = 0; i != (int)bTags.size(); ++i) {

			TLorentzVector btag;
			btag.SetPtEtaPhiM(bTags[i].first->pt(),bTags[i].first->eta(),bTags[i].first->phi(),bTags[i].first->mass());
			if (btag.DeltaR(thejet) < 0.01) {
				btagdisc=bTags[i].second;
				break;
			}  

		}

		jet_CSV[j]=btagdisc;
		    std::string jstring = std::to_string(j);
		                    theevent["Event"][eventstring]["jet"][jstring]["CSV"]=jet_CSV[j];
		                            
		                            

	}

	// MET
	Handle< PFMETCollection > met_handle;
	event_obj.getByLabel("pfMet", met_handle);

	met = (*met_handle)[0].et();
	met_phi = (*met_handle)[0].phi();
	sumet = (*met_handle)[0].sumEt();



	//fill json with "per event" variables

	theevent["Event"][eventstring]["met"]=met ;
	theevent["Event"][eventstring]["met_phi"]=met_phi ;
	theevent["Event"][eventstring]["sumet"]=sumet;
	theevent["Event"][eventstring]["nummu"]=nmu;
	theevent["Event"][eventstring]["numel"]=nel;
	theevent["Event"][eventstring]["njet"]=njet;
  theevent["Event"][eventstring]["xsec"]=mcweight;
  theevent["Event"][eventstring]["isMC"]=(int)mIsMCarlo;
  theevent["Event"][eventstring]["lumi"]=lumi;
  theevent["Event"][eventstring]["run"]=run;




	// Finally, fill the tree
	if (njet >= (unsigned)mMinNPFJets && istheregoodvertex  ) {            
		istheregoodvertex=true;
		//mTree->Fill();
	}


}


void OpenDataTreeProducerOptimized::endRun(edm::Run const &iRun, edm::EventSetup const &iSetup) {
	//write json file

}

OpenDataTreeProducerOptimized::~OpenDataTreeProducerOptimized() {


}


DEFINE_FWK_MODULE(OpenDataTreeProducerOptimized);
