////////////////////////////////////////////////////////////////////////
// Class:       PointIdEffTest
// Module Type: analyzer
// File:        PointIdEffTest_module.cc
//
// Author: dorota.stefan@cern.ch
//
// Generated at Fri Apr 29 06:42:27 2016 by Dorota Stefan using artmod
// from cetpkgsupport v1_10_01.
////////////////////////////////////////////////////////////////////////

#include "lardataobj/Simulation/SimChannel.h"
#include "larsim/Simulation/LArG4Parameters.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/Shower.h"
#include "larreco/Calorimetry/CalorimetryAlg.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "larcoreobj/SimpleTypesAndConstants/PhysicalConstants.h"
#include "lardata/Utilities/DatabaseUtil.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Table.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "larreco/RecoAlg/ImagePatternAlgs/PointIdAlg/PointIdAlg.h"

#include "TH1.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TVector3.h"

#include <iostream>
#include <fstream>

#include <cmath>

namespace nnet
{
	typedef std::unordered_map< unsigned int, std::vector< std::vector< art::Ptr<recob::Hit> > > > view_clmap;
	typedef std::unordered_map< unsigned int, view_clmap > tpc_view_clmap;
	typedef std::unordered_map< unsigned int, tpc_view_clmap > cryo_tpc_view_clmap;

	class PointIdEffTest;
}

class nnet::PointIdEffTest : public art::EDAnalyzer {
public:
    enum EId { kShower = 0, kTrack = 1 };

	struct Config {
		using Name = fhicl::Name;
		using Comment = fhicl::Comment;

		fhicl::Table<nnet::PointIdAlg::Config> PointIdAlg {
			Name("PointIdAlg")
		};

		fhicl::Atom<art::InputTag> SimModuleLabel {
			Name("SimModuleLabel"),
			Comment("...")
		};

		fhicl::Atom<art::InputTag> WireLabel {
			Name("WireLabel"),
			Comment("...")
		};

		fhicl::Atom<art::InputTag> HitsModuleLabel {
			Name("HitsModuleLabel"),
			Comment("...")
		};

		fhicl::Atom<art::InputTag> ClusterModuleLabel {
			Name("ClusterModuleLabel"),
			Comment("...")
		};

		fhicl::Atom<bool> SaveHitsFile {
			Name("SaveHitsFile"),
			Comment("...")
		};

		fhicl::Atom<double> Threshold {
			Name("Threshold"),
			Comment("...")
		};

		fhicl::Atom<unsigned int> View {
			Name("View"),
			Comment("...")
		};
    };
    using Parameters = art::EDAnalyzer::Table<Config>;

    explicit PointIdEffTest(Parameters const& config);
    // The destructor generated by the compiler is fine for classes
    // without bare pointers or other resource use.

    // Plugins should not be copied or assigned.
    PointIdEffTest(PointIdEffTest const &) = delete;
    PointIdEffTest(PointIdEffTest &&) = delete;
    PointIdEffTest & operator = (PointIdEffTest const &) = delete;
    PointIdEffTest & operator = (PointIdEffTest &&) = delete;

  // Required functions.
	virtual void beginRun(const art::Run& run) override;

	virtual void beginJob() override;
	virtual void endJob() override;

    virtual void analyze(art::Event const & e) override;

private:
    void cleanup(void);

    void countTruthDep(
        const std::vector< sim::SimChannel > & channels,
        float & emLike, float & trackLike) const;

	int GetMCParticle(
	    const std::vector< sim::SimChannel > & channels,
        const std::vector< art::Ptr<recob::Hit> > & hits);

	void GetRecoParticle(std::vector< art::Ptr<recob::Hit> > const & hits, int mctype);

	int fRun, fEvent;
    float fMcDepEM, fMcDepTrack, fMcFractionEM;
    float fHitEM_0p5, fHitTrack_0p5, fHitFractionEM_0p5;
    float fHitEM_0p85, fHitTrack_0p85;
    float fTotHit, fCleanHit;

	int fMcPid;
	int fClSize;
	int fRecoPid;
	int fPure;
	double fPidValue;
	

	int fTrkOk;
	int fTrkB;
	int fShOk;
	int fShB;
	int fNone;
	int fTotal;

	double fElectronsToGeV;
	float fOutTrk;
	float fOutSh;

	TTree *fEventTree, *fClusterTree, *fHitTree;

	std::ofstream fHitsOutFile;

	nnet::PointIdAlg fPointIdAlg;
    double fThreshold;
	unsigned int fView;

	geo::GeometryCore const* fGeometry;

	std::unordered_map< int, const simb::MCParticle* > fParticleMap;

	cryo_tpc_view_clmap fClMap;	

	art::InputTag fSimulationProducerLabel;
	art::InputTag fWireProducerLabel;
	art::InputTag fHitsModuleLabel;
	art::InputTag fClusterModuleLabel;
	bool fSaveHitsFile;
};

nnet::PointIdEffTest::PointIdEffTest(nnet::PointIdEffTest::Parameters const& config) : art::EDAnalyzer(config),
	fMcPid(-1), fClSize(0), fRecoPid(-1),
	fTrkOk(0), fTrkB(0), fShOk(0),
	fShB(0), fNone(0), fTotal(0),

 	fPointIdAlg(config().PointIdAlg()),
	fThreshold(config().Threshold()),
	fView(config().View()),
	fSimulationProducerLabel(config().SimModuleLabel()),
	fWireProducerLabel(config().WireLabel()),
	fHitsModuleLabel(config().HitsModuleLabel()),
	fClusterModuleLabel(config().ClusterModuleLabel()),
	fSaveHitsFile(config().SaveHitsFile())
{
    fGeometry = &*(art::ServiceHandle<geo::Geometry>());
}

void nnet::PointIdEffTest::beginRun(const art::Run&)
{
	art::ServiceHandle< sim::LArG4Parameters > larParameters;
	fElectronsToGeV = 1./larParameters->GeVToElectrons();
}

void nnet::PointIdEffTest::beginJob()
{
	art::ServiceHandle<art::TFileService> tfs;

    fEventTree = tfs->make<TTree>("event","event info");
    fEventTree->Branch("fRun", &fRun, "fRun/I");
    fEventTree->Branch("fEvent", &fEvent, "fEvent/I");
    fEventTree->Branch("fMcDepEM", &fMcDepEM, "fMcDepEM/F");
    fEventTree->Branch("fMcDepTrack", &fMcDepTrack, "fMcDepTrack/F");
    fEventTree->Branch("fMcFractionEM", &fMcFractionEM, "fMcFractionEM/F");
    fEventTree->Branch("fHitEM_0p5", &fHitEM_0p5, "fHitEM_0p5/F");
    fEventTree->Branch("fHitTrack_0p5", &fHitTrack_0p5, "fHitTrack_0p5/F");
    fEventTree->Branch("fHitFractionEM_0p5", &fHitFractionEM_0p5, "fHitFractionEM_0p5/F");
    fEventTree->Branch("fHitEM_0p85", &fHitEM_0p85, "fHitEM_0p85/F");
    fEventTree->Branch("fHitTrack_0p85", &fHitTrack_0p85, "fHitTrack_0p85/F");
    fEventTree->Branch("fCleanHit", &fCleanHit, "fCleanHit/F");

	fHitTree = tfs->make<TTree>("hit","hits info");
	fHitTree->Branch("fOutSh", &fOutSh, "fOutSh/F");
	fHitTree->Branch("fOutTrk", &fOutTrk, "fOutTrk/F");

	fClusterTree = tfs->make<TTree>("cluster","clusters info");
	fClusterTree->Branch("fMcPid", &fMcPid, "fMcPid/I");
	fClusterTree->Branch("fClSize", &fClSize, "fClSize/I");
	fClusterTree->Branch("fRecoPid", &fRecoPid, "fRecoPid/I");
	fClusterTree->Branch("fPidValue", &fPidValue, "fPidValue/D");

	if (fSaveHitsFile) fHitsOutFile.open("hits_pid.prn");
}

void nnet::PointIdEffTest::endJob()
{
	if (fSaveHitsFile) fHitsOutFile.close();

	std::cout << " fShOk " << fShOk << " fTrkOk " << fTrkOk << std::endl;
	std::cout << " fShB " << fShB << " fTrkB " << fTrkB << std::endl;
	std::cout << " fNone " << fNone << " Total " << fTotal << std::endl;

	std::cout << " fShErr " << fShB / float(fShB + fShOk) << " fTrkErr " << fTrkB / float(fTrkB + fTrkOk) << std::endl;
}

void nnet::PointIdEffTest::cleanup(void)
{
    fClMap.clear();

	fOutSh = -1; fOutTrk = -1;

    fMcDepEM = 0; fMcDepTrack = 0; fMcFractionEM = 0;
    fHitEM_0p5 = 0; fHitTrack_0p5 = 0; fHitFractionEM_0p5 = 0;
    fHitEM_0p85 = 0; fHitTrack_0p85 = 0;
    fTotHit = 0; fCleanHit = 0;
}

void nnet::PointIdEffTest::analyze(art::Event const & e)
{
    cleanup(); // remove everything from member vectors and maps

	fRun = e.run();
	fEvent = e.id().event();

	// access to MC information
	
	// MC particles list
	auto particleHandle = e.getValidHandle< std::vector<simb::MCParticle> >(fSimulationProducerLabel);
	std::vector< art::Ptr<simb::MCParticle> > particleList;
	art::fill_ptr_vector(particleList, particleHandle);

	for (auto const& particle : *particleHandle)
	{
		fParticleMap[particle.TrackId()] = &particle;
	}

	// SimChannels
	auto simChannelHandle = e.getValidHandle< std::vector<sim::SimChannel> >(fSimulationProducerLabel);
	std::vector< art::Ptr<sim::SimChannel> > channelList;
	art::fill_ptr_vector(channelList, simChannelHandle);

    countTruthDep(*simChannelHandle, fMcDepEM, fMcDepTrack);

	// output from reconstruction

	// wires
	auto wireHandle = e.getValidHandle< std::vector<recob::Wire> >(fWireProducerLabel);

	// hits
	auto hitListHandle = e.getValidHandle< std::vector<recob::Hit> >(fHitsModuleLabel);
	std::vector< art::Ptr<recob::Hit> > hitList;
	art::fill_ptr_vector(hitList, hitListHandle);

	// clusters
	auto clusterListHandle = e.getValidHandle< std::vector<recob::Cluster> >(fClusterModuleLabel);
	std::vector< art::Ptr<recob::Cluster> > clusterList;
	art::fill_ptr_vector(clusterList, clusterListHandle);
 
	const art::FindManyP<recob::Hit> hitsFromClusters(clusterListHandle, e, fClusterModuleLabel);
	for (size_t clid = 0; clid != clusterListHandle->size(); ++clid)
	{
		auto const& hits = hitsFromClusters.at(clid);
		if (!hits.size()) continue;

		unsigned int cryo = hits.front()->WireID().Cryostat;
		unsigned int tpc = hits.front()->WireID().TPC;
		unsigned int view = hits.front()->WireID().Plane;

		fClMap[cryo][tpc][view].push_back(hits);
	}

    for (auto const & hi : hitList)
	{
		bool unclustered = true;
		for (size_t k = 0; k < hitsFromClusters.size(); ++k)
		{
			auto v = hitsFromClusters.at(k);
			for (auto const & hj : v)
			{
				if (hi.key() == hj.key()) { unclustered = false; break; }
			}
			if (!unclustered) break;
		}

		if (unclustered)
		{
			unsigned int cryo = hi->WireID().Cryostat;
		    unsigned int tpc = hi->WireID().TPC;
		    unsigned int view = hi->WireID().Plane;

            std::vector< art::Ptr<recob::Hit> > singlehit;
            singlehit.push_back(hi);

		    fClMap[cryo][tpc][view].push_back(singlehit);
		}
	}

	for (auto const& c : fClMap)
	{
		unsigned int cryo = c.first;
		for (auto const& t : c.second)
		{
			unsigned int tpc = t.first;
			for (auto const& v : t.second)
			{
				unsigned int view = v.first;
				if (view == fView)
				{
					fPointIdAlg.setWireDriftData(*wireHandle, view, tpc, cryo);

					for (auto const& h : v.second)
					{
						int mctype = GetMCParticle(*simChannelHandle, h); // mctype == -1 : cluster out of fiducial area
						if (mctype > -1)                                  //                or mixed track-em custer
						{
						    GetRecoParticle(h, mctype);
						}
					}
				}
			}
		}
	}

    if (fTotHit > 0) fCleanHit = fCleanHit / fTotHit;
    else fCleanHit = 0;

    double totEmTrk = fHitEM_0p5 + fHitTrack_0p5;
    if (totEmTrk > 0) fHitFractionEM_0p5 = fHitEM_0p5;
    else fHitFractionEM_0p5 = 0;

    double totMcDep = fMcDepEM + fMcDepTrack;
    if (totMcDep) fMcFractionEM = fMcDepEM / totMcDep;
    else fMcFractionEM = 0;

	fEventTree->Fill();

	cleanup(); // remove everything from member vectors and maps
}

/******************************************/

void nnet::PointIdEffTest::countTruthDep(
    const std::vector< sim::SimChannel > & channels,
    float & emLike, float & trackLike) const
{
    emLike = 0; trackLike = 0;
	for (auto const& channel : channels)
	{
		// for every time slice in this channel:
		auto const& timeSlices = channel.TDCIDEMap();
		for (auto const& timeSlice : timeSlices)
		{
			// loop over the energy deposits.
			auto const& energyDeposits = timeSlice.second;
			for (auto const& energyDeposit : energyDeposits)
			{
				int trackID = energyDeposit.trackID;

				double energy = energyDeposit.numElectrons * fElectronsToGeV * 1000;

				if (trackID < 0)
				{
					emLike += energy;
				}
				else if (trackID > 0)
				{
					auto search = fParticleMap.find(trackID);
					bool found = true;
					if (search == fParticleMap.end())
					{
						mf::LogWarning("TrainingDataAlg") << "PARTICLE NOT FOUND";
						found = false;
					}

					int pdg = 0;
					if (found)
					{
						const simb::MCParticle& particle = *((*search).second);
						if (!pdg) pdg = particle.PdgCode(); // not EM activity so read what PDG it is
					}

					if ((pdg == 11) || (pdg == -11) || (pdg == 22)) emLike += energy;
					else trackLike += energy;
				}
			}
		}
	}
}

/******************************************/

int nnet::PointIdEffTest::GetMCParticle(
    const std::vector< sim::SimChannel > & channels,
    const std::vector< art::Ptr<recob::Hit> > & hits)
{
	double totEnSh = 0, totEnTrk = 0;
	size_t insideFidArea = 0;

	for (auto const& hit: hits)
	{
		if (!fPointIdAlg.isInsideFiducialRegion(hit->WireID().Wire, hit->PeakTime())) continue;
		insideFidArea++;

		// the channel associated with this hit.
		auto hitChannelNumber = hit->Channel();

        double hitEnSh = 0, hitEnTrk = 0;
		for (auto const& channel : channels)
		{
			if (channel.Channel() != hitChannelNumber) continue;

			// for every time slice in this channel:
			auto const& timeSlices = channel.TDCIDEMap();
			for (auto const& timeSlice : timeSlices)
			{
				int time = timeSlice.first;
				if (std::abs(hit->TimeDistanceAsRMS(time)) < 1.0)
				{
					// loop over the energy deposits.
					auto const& energyDeposits = timeSlice.second;
		
					for (auto const& energyDeposit : energyDeposits)
					{
						int trackID = energyDeposit.trackID;

						double energy = energyDeposit.numElectrons * fElectronsToGeV * 1000;

						if (trackID < 0)
						{
							hitEnSh += energy;
						}
						else if (trackID > 0)
						{
							auto search = fParticleMap.find(trackID);
							bool found = true;
							if (search == fParticleMap.end())
							{
								mf::LogWarning("TrainingDataAlg") << "PARTICLE NOT FOUND";
								found = false;
							}
						
							int pdg = 0;
							if (found)
							{
								const simb::MCParticle& particle = *((*search).second);
								if (!pdg) pdg = particle.PdgCode(); // not EM activity so read what PDG it is
							}

							if ((pdg == 11) || (pdg == -11) || (pdg == 22)) hitEnSh += energy;
							else hitEnTrk += energy;
						}
					}
				}
			}
		}
		totEnSh += hitEnSh;
		totEnTrk += hitEnTrk;

        double hitAdc = hit->SummedADC() * fPointIdAlg.LifetimeCorrection(hit->PeakTime());
		fTotHit += hitAdc;

		if (hitEnSh > hitEnTrk) { fHitEM_0p5 += hitAdc; }
		else { fHitTrack_0p5 += hitAdc; }

        double hitDep = hitEnSh + hitEnTrk;
		if (hitEnSh > 0.85 * hitDep) { fHitEM_0p85 += hitAdc; fCleanHit += hitAdc; }
		else if (hitEnTrk > 0.85 * hitDep) { fHitTrack_0p85 += hitAdc; fCleanHit += hitAdc; }
	}

	int result = -1;
	if (insideFidArea > 2 * hits.size() / 3) // 2/3 of the cluster hits inside fiducial area
	{
		if (totEnSh > 1.5 * totEnTrk) // major energy deposit from EM activity
		{
			result = nnet::PointIdEffTest::kShower;
		}
		else if (totEnTrk > 1.5 * totEnSh)
		{
			result = nnet::PointIdEffTest::kTrack;
		}
	}

	return result;
}

/******************************************/

void nnet::PointIdEffTest::GetRecoParticle(std::vector< art::Ptr<recob::Hit> > const & hits, int mctype)
{
	fMcPid = mctype;
	fClSize = hits.size();

	fPidValue = 0;
	if (fPointIdAlg.NClasses() == 1)
	{
		fPidValue = fPointIdAlg.predictIdValue(hits);
	}
	else
	{
		auto vout = fPointIdAlg.predictIdVector(hits);
		double p_trk_or_sh = vout[0] + vout[1];
		if (p_trk_or_sh > 0) fPidValue = vout[0] / p_trk_or_sh;
	}

	if (fPidValue < fThreshold) fRecoPid = nnet::PointIdEffTest::kShower;
	else if (fPidValue > fThreshold) fRecoPid = nnet::PointIdEffTest::kTrack;
	else fRecoPid = -1;

	if ((fRecoPid == nnet::PointIdEffTest::kShower) && (mctype == nnet::PointIdEffTest::kShower))
	{
		fShOk++;
	}
	else if ((fRecoPid == nnet::PointIdEffTest::kTrack) && (mctype == nnet::PointIdEffTest::kTrack))
	{
		fTrkOk++;
	}	
	else if ((fRecoPid == nnet::PointIdEffTest::kShower) && (mctype == nnet::PointIdEffTest::kTrack))
	{
		fTrkB++;
	}
	else if ((fRecoPid == nnet::PointIdEffTest::kTrack) && (mctype == nnet::PointIdEffTest::kShower))
	{
		fShB++;
	}
	else
	{
		fNone++;
	}	

	fTotal++;

	if (fSaveHitsFile)
	{
		for (auto const h : hits)
		{
			fHitsOutFile << fRun << " " << fEvent << " "
				<< h->WireID().TPC  << " " << h->WireID().Wire << " " << h->PeakTime() << " "
				<< mctype << " " << fRecoPid << " " << fPidValue << std::endl;
		}
	}
	
	fClusterTree->Fill();
}

/******************************************/

DEFINE_ART_MODULE(nnet::PointIdEffTest)
