/**
 * copyright (C) 2011
 * Claudio Kopper <claudio.kopper@nikhef.nl>
 * $Id: ConvertToGST.cxx 126483 2014-12-02 23:04:01Z hoshina $
 *
 * @file ConvertToGST.h
 * @version $Revision$
 * @date $Date: 2014-12-02 17:04:01 -0600 (Tue, 02 Dec 2014) $
 * @author Claudio Kopper
 *
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */

#include "genie-icetray/ConvertToGST.h"

#include <vector>

#include "icetray/I3Logging.h"

#include "Conventions/GBuild.h"
#include "Conventions/Constants.h"
#include "Conventions/Units.h"
#include "Conventions/GVersion.h"
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepStatus.h"
#include "GHEP/GHepParticle.h"
#include "GHEP/GHepUtils.h"
#include "Ntuple/NtpMCFormat.h"
#include "Ntuple/NtpMCTreeHeader.h"
#include "Ntuple/NtpMCEventRecord.h"
#include "Numerical/RandomGen.h"
#include "Messenger/Messenger.h"
#include "PDG/PDGCodes.h"
#include "PDG/PDGUtils.h"
#include "PDG/PDGLibrary.h"
//#include "Utils/CmdLineArgParserUtils.h"
//#include "Utils/CmdLineArgParserException.h"
#include "Utils/SystemUtils.h"
#include "Utils/T2KEvGenMetaData.h"

using namespace genie;

bool I3GENIE_Helper::ConvertToGST(const EventRecord &event, int32_t iev, I3GENIEResultDict &gstMap)
{
    //-- some constants
    const double e_h = 1.3; // typical e/h ratio used for computing mean `calorimetric response'
    
    //-- define branch variables
    //
    int32_t    brIev         = 0;      // Event number 
    int32_t    brNeutrino    = 0;      // Neutrino pdg code
    int32_t    brTarget      = 0;      // Nuclear target pdg code (10LZZZAAAI)
    int32_t    brTargetZ     = 0;      // Nuclear target Z (extracted from pdg code above)
    int32_t    brTargetA     = 0;      // Nuclear target A (extracted from pdg code above)
    int32_t    brHitNuc      = 0;      // Hit nucleon pdg code      (not set for COH,IMD and NuEL events)
    int32_t    brHitQrk      = 0;      // Hit quark pdg code        (set for DIS events only)
    bool   brFromSea     = false;  // Hit quark is from sea     (set for DIS events only)
    bool   brResId       = 0;      // Produced baryon resonance (set for resonance events only)
    bool   brIsQel       = false;  // Is QEL?
    bool   brIsRes       = false;  // Is RES?
    bool   brIsDis       = false;  // Is DIS?
    bool   brIsCoh       = false;  // Is Coherent?
    bool   brIsDfr       = false;  // Is Diffractive?
    bool   brIsImd       = false;  // Is IMD?
    bool   brIsNuEL      = false;  // Is ve elastic?
    bool   brIsEM        = false;  // Is Electro-magnetic?
    bool   brIsCC        = false;  // Is CC?
    bool   brIsNC        = false;  // Is NC?
    bool   brIsCharmPro  = false;  // Produces charm?
    int32_t    brCodeNeut    = 0;      // The equivalent NEUT reaction code (if any)
    int32_t    brCodeNuance  = 0;      // The equivalent NUANCE reaction code (if any)
    double brWeight      = 0;      // Event weight
    double brKineXs      = 0;      // Bjorken x as was generated during kinematical selection; takes fermi momentum / off-shellness into account
    double brKineYs      = 0;      // Inelasticity y as was generated during kinematical selection; takes fermi momentum / off-shellness into account
    double brKineTs      = 0;      // Energy transfer to nucleus at COH events as was generated during kinematical selection
    double brKineQ2s     = 0;      // Momentum transfer Q^2 as was generated during kinematical selection; takes fermi momentum / off-shellness into account
    double brKineWs      = 0;      // Hadronic invariant mass W as was generated during kinematical selection; takes fermi momentum / off-shellness into account
    double brKineX       = 0;      // Experimental-like Bjorken x; neglects fermi momentum / off-shellness 
    double brKineY       = 0;      // Experimental-like inelasticity y; neglects fermi momentum / off-shellness 
    double brKineT       = 0;      // Experimental-like energy transfer to nucleus at COH events 
    double brKineQ2      = 0;      // Experimental-like momentum transfer Q^2; neglects fermi momentum / off-shellness
    double brKineW       = 0;      // Experimental-like hadronic invariant mass W; neglects fermi momentum / off-shellness 
    double brEv          = 0;      // Neutrino energy @ LAB
    double brPxv         = 0;      // Neutrino px @ LAB
    double brPyv         = 0;      // Neutrino py @ LAB
    double brPzv         = 0;      // Neutrino pz @ LAB
    double brEn          = 0;      // Initial state hit nucleon energy @ LAB
    double brPxn         = 0;      // Initial state hit nucleon px @ LAB
    double brPyn         = 0;      // Initial state hit nucleon py @ LAB
    double brPzn         = 0;      // Initial state hit nucleon pz @ LAB
    int brPdgl           = 0;      // Final state primary lepton Pdg code
    double brEl          = 0;      // Final state primary lepton energy @ LAB
    double brKEl         = 0;      // Final state primary lepton kinetic energy @ LAB
    double brPxl         = 0;      // Final state primary lepton px @ LAB
    double brPyl         = 0;      // Final state primary lepton py @ LAB
    double brPzl         = 0;      // Final state primary lepton pz  @ LAB
    int32_t    brNfP         = 0;      // Nu. of final state p's + \bar{p}'s (after intranuclear rescattering)
    int32_t    brNfN         = 0;      // Nu. of final state n's + \bar{n}'s
    int32_t    brNfPip       = 0;      // Nu. of final state pi+'s
    int32_t    brNfPim       = 0;      // Nu. of final state pi-'s
    int32_t    brNfPi0       = 0;      // Nu. of final state pi0's (
    int32_t    brNfKp        = 0;      // Nu. of final state K+'s
    int32_t    brNfKm        = 0;      // Nu. of final state K-'s
    int32_t    brNfK0        = 0;      // Nu. of final state K0's + \bar{K0}'s
    int32_t    brNfEM        = 0;      // Nu. of final state gammas and e-/e+ 
    int32_t    brNfOther     = 0;      // Nu. of heavier final state hadrons (D+/-,D0,Ds+/-,Lamda,Sigma,Lamda_c,Sigma_c,...)
    int32_t    brNiP         = 0;      // Nu. of `primary' (: before intranuclear rescattering) p's + \bar{p}'s  
    int32_t    brNiN         = 0;      // Nu. of `primary' n's + \bar{n}'s  
    int32_t    brNiPip       = 0;      // Nu. of `primary' pi+'s 
    int32_t    brNiPim       = 0;      // Nu. of `primary' pi-'s 
    int32_t    brNiPi0       = 0;      // Nu. of `primary' pi0's 
    int32_t    brNiKp        = 0;      // Nu. of `primary' K+'s  
    int32_t    brNiKm        = 0;      // Nu. of `primary' K-'s  
    int32_t    brNiK0        = 0;      // Nu. of `primary' K0's + \bar{K0}'s 
    int32_t    brNiEM        = 0;      // Nu. of `primary' gammas and e-/e+ 
    int32_t    brNiOther     = 0;      // Nu. of other `primary' hadron shower particles
    int32_t    brNf          = 0;      // Nu. of final state particles in hadronic system
    std::vector<int32_t>    brPdgf;    // Pdg code of k^th final state particle in hadronic system
    std::vector<double> brEf;      // Energy   of k^th final state particle in hadronic system @ LAB
    std::vector<double> brKEf;     // Kin. Energy   of k^th final state particle in hadronic system @ LAB
    std::vector<double> brPxf;     // Px       of k^th final state particle in hadronic system @ LAB
    std::vector<double> brPyf;     // Py       of k^th final state particle in hadronic system @ LAB
    std::vector<double> brPzf;     // Pz       of k^th final state particle in hadronic system @ LAB
    int32_t    brNi          = 0;      // Nu. of particles in 'primary' hadronic system (before intranuclear rescattering)
    std::vector<int32_t>    brPdgi;    // Pdg code of k^th particle in 'primary' hadronic system 
    std::vector<int32_t>    brResc;    // FSI code of k^th particle in 'primary' hadronic system 
    std::vector<double> brEi;      // Energy   of k^th particle in 'primary' hadronic system @ LAB
    std::vector<double> brPxi;     // Px       of k^th particle in 'primary' hadronic system @ LAB
    std::vector<double> brPyi;     // Py       of k^th particle in 'primary' hadronic system @ LAB
    std::vector<double> brPzi;     // Pz       of k^th particle in 'primary' hadronic system @ LAB
    double brVtxX;                 // Vertex x in detector coord system (SI)
    double brVtxY;                 // Vertex y in detector coord system (SI)
    double brVtxZ;                 // Vertex z in detector coord system (SI)
    double brVtxT;                 // Vertex t in detector coord system (SI)
    double brCalResp0;             // Approximate calorimetric response to the hadronic system computed as sum of
    //  - (kinetic energy) for pi+, pi-, p, n 
    //  - (energy + mass)  for antiproton, antineutron
    //  - ((e/h) * energy)   for pi0, gamma, e-, e+, where e/h is set to 1.3
    //  - (kinetic energy) for other particles
    
    TLorentzVector pdummy(0,0,0,0);

    // Computing event characteristics
    //
    
    //input particles
    GHepParticle * neutrino = event.Probe();
    assert(neutrino);
    GHepParticle * target = event.Particle(1);
    assert(target);
    GHepParticle * fsl = event.FinalStatePrimaryLepton();
    assert(fsl);
    GHepParticle * hitnucl = event.HitNucleon();
    
    int32_t tgtZ = 0;
    int32_t tgtA = 0;
    if(pdg::IsIon(target->Pdg())) {
        tgtZ = pdg::IonPdgCodeToZ(target->Pdg());
        tgtA = pdg::IonPdgCodeToA(target->Pdg());
    } 
    if(target->Pdg() == kPdgProton ) { tgtZ = 1; tgtA = 1; }    
    if(target->Pdg() == kPdgNeutron) { tgtZ = 0; tgtA = 1; }    
    
    //summary info
    const Interaction * interaction = event.Summary();
    const InitialState & init_state = interaction->InitState();
    const ProcessInfo &  proc_info  = interaction->ProcInfo();
    const Kinematics &   kine       = interaction->Kine();
    const XclsTag &      xcls       = interaction->ExclTag();
    const Target &       tgt        = init_state.Tgt();
    
    //vertex in detector coord system
    TLorentzVector * vtx = event.Vertex();
    
    //process id
    bool is_qel    = proc_info.IsQuasiElastic();
    bool is_res    = proc_info.IsResonant();
    bool is_dis    = proc_info.IsDeepInelastic();
    bool is_coh    = proc_info.IsCoherent();
    bool is_dfr    = proc_info.IsDiffractive();
    bool is_imd    = proc_info.IsInverseMuDecay();
    bool is_nuel   = proc_info.IsNuElectronElastic();
    bool is_em     = proc_info.IsEM();
    bool is_weakcc = proc_info.IsWeakCC();
    bool is_weaknc = proc_info.IsWeakNC();
    
    if(!hitnucl) { assert(is_coh || is_imd || is_nuel); }
    
    // hit quark 
    // set only for DIS events
    int  qrk  = (is_dis) ? tgt.HitQrkPdg() : 0;     
    bool seaq = (is_dis) ? tgt.HitSeaQrk() : false; 
    
    // resonance id ($GENIE/src/BaryonResonance/BaryonResonance.h)
    // set only for resonance neutrinoproduction
    int resid = (is_res) ? xcls.Resonance() : -99;
    
    // (qel or dis) charm production?
    bool charm = xcls.IsCharmEvent();
    
    // get neut and nuance equivalent reaction codes (if any)
    brCodeNeut    = utils::ghep::NeutReactionCode(&event);
    brCodeNuance  = utils::ghep::NuanceReactionCode(&event);
    
    // weight
    double weight = event.Weight();
    
    // cross section and probability
    double xsec = event.XSec() / (1e-38*units::cm2);
    double diffxsec = event.DiffXSec() / (1e-38*units::cm2);
    double prob = event.Probability();

    double tgtmass = PDGLibrary::Instance()->Find(target->Pdg())->Mass();
    
    // Access kinematical params _exactly_ as they were selected internally
    // (at the hit nucleon rest frame; 
    // for bound nucleons: taking into account fermi momentum and off-shell kinematics)
    //
    bool get_selected = true;
    double xs  = kine.x (get_selected);
    double ys  = kine.y (get_selected);
    double ts  = (is_coh || is_dfr) ? kine.t (get_selected) : -1;
    double Q2s = kine.Q2(get_selected);
    double Ws  = kine.W (get_selected);
    
    LOG("gntpc", pDEBUG) 
    << "[Select] Q2 = " << Q2s << ", W = " << Ws 
    << ", x = " << xs << ", y = " << ys << ", t = " << ts;
    
    // Calculate the same kinematical params but now as an experimentalist would 
    // measure them by neglecting the fermi momentum and off-shellness of bound nucleons
    //
    
    const TLorentzVector & k1 = *(neutrino->P4());                     // v 4-p (k1)
    const TLorentzVector & k2 = *(fsl->P4());                          // l 4-p (k2)
    const TLorentzVector & p1 = (hitnucl) ? *(hitnucl->P4()) : pdummy; // N 4-p (p1)      
    int pdgl = fsl->Pdg();
    
    double M  = constants::kNucleonMass; 
    TLorentzVector q  = k1-k2;                     // q=k1-k2, 4-p transfer
    double Q2 = -1 * q.M2();                       // momemtum transfer
    double v  = (hitnucl) ? q.Energy()       : -1; // v (E transfer to the nucleus)
    double x  = (hitnucl) ? 0.5*Q2/(M*v)     : -1; // Bjorken x
    double y  = (hitnucl) ? v/k1.Energy()    : -1; // Inelasticity, y = q*P1/k1*P1
    double W2 = (hitnucl) ? M*M + 2*M*v - Q2 : -1; // Hadronic Invariant mass ^ 2
    double W  = (hitnucl) ? TMath::Sqrt(W2)  : -1; 
    double t  = 0;
    
    LOG("gntpc", pDEBUG) 
    << "[Calc] Q2 = " << Q2 << ", W = " << W 
    << ", x = " << x << ", y = " << y << ", t = " << t;
    
    // Extract more info on the hadronic system
    // Only for QEL/RES/DIS/COH events
    //
    bool study_hadsyst = (is_qel || is_res || is_dis || is_coh);
    
    //
    TObjArrayIter piter(&event);
    GHepParticle * p = 0;
    int ip=-1;
    
    //
    // Extract the final state system originating from the hadronic vertex 
    // (after the intranuclear rescattering step)
    //
    
    LOG("gntpc", pDEBUG) << "Extracting final state hadronic system";
    
    vector<int> final_had_syst;
    while( (p = (GHepParticle *) piter.Next()) && study_hadsyst)
    {
        ip++;
        // don't count final state lepton as part hadronic system 
        //if(!is_coh && event.Particle(ip)->FirstMother()==0) continue;
        if(event.Particle(ip)->FirstMother()==0) continue;
        if(pdg::IsPseudoParticle(p->Pdg())) continue;
        int pdgc = p->Pdg();
        int ist  = p->Status();
        if(ist==kIStStableFinalState) {
            if (pdgc == kPdgGamma || pdgc == kPdgElectron || pdgc == kPdgPositron)  {
                int igmom = p->FirstMother();
                if(igmom!=-1) {
                    // only count e+'s e-'s or gammas not from decay of pi0
                    if(event.Particle(igmom)->Pdg() != kPdgPi0) { final_had_syst.push_back(ip); }
                }
            } else {
                final_had_syst.push_back(ip);
            }
        }
        // now add pi0's that were decayed as short lived particles
        else if(pdgc == kPdgPi0){
            int ifd = p->FirstDaughter();
            int fd_pdgc = event.Particle(ifd)->Pdg();
            // just require that first daughter is one of gamma, e+ or e-  
            if(fd_pdgc == kPdgGamma || fd_pdgc == kPdgElectron || fd_pdgc == kPdgPositron){
                final_had_syst.push_back(ip);
            }
        }
    }//particle-loop
    
    if( count(final_had_syst.begin(), final_had_syst.end(), -1) > 0) {
        return false;
    }
    
    //
    // Extract info on the primary hadronic system (before any intranuclear rescattering)
    // looking for particles with status_code == kIStHadronInTheNucleus 
    // An exception is the coherent production and scattering off free nucleon targets 
    // (no intranuclear rescattering) in which case primary hadronic system is set to be 
    // 'identical' with the final  state hadronic system
    //
    
    LOG("gntpc", pDEBUG) << "Extracting primary hadronic system";
    
    ip = -1;
    TObjArrayIter piter_prim(&event);
    
    vector<int> prim_had_syst;
    if(study_hadsyst) {
        // if coherent or free nucleon target set primary states equal to final states
        if(!pdg::IsIon(target->Pdg()) || (is_coh)) {
            vector<int>::const_iterator hiter = final_had_syst.begin();
            for( ; hiter != final_had_syst.end(); ++hiter) {
                prim_had_syst.push_back(*hiter);
            }
        } 
        // otherwise loop over all particles and store indices of those which are hadrons
        // created within the nucleus
        else {
            while( (p = (GHepParticle *) piter_prim.Next()) ){
                ip++;      
                int ist_comp  = p->Status();
                if(ist_comp==kIStHadronInTheNucleus) {
                    prim_had_syst.push_back(ip); 
                }
            }//particle-loop   
            //
            // also include gammas from nuclear de-excitations (appearing in the daughter list of the 
            // hit nucleus, earlier than the primary hadronic system extracted above)
            for(int i = target->FirstDaughter(); i <= target->LastDaughter(); i++) {
                if(i<0) continue;
                if(event.Particle(i)->Status()==kIStStableFinalState) { prim_had_syst.push_back(i); }
            }      
        }//freenuc?
    }//study_hadsystem?
    
    if( count(prim_had_syst.begin(), prim_had_syst.end(), -1) > 0) {
        return false;
    }
    
    //
    // Al information has been assembled -- Start filling up the tree branches
    //
    brIev        = (int32_t) iev;      
    brNeutrino   = neutrino->Pdg();      
    brTarget     = target->Pdg(); 
    brTargetZ    = tgtZ;
    brTargetA    = tgtA;   
    brHitNuc     = (hitnucl) ? hitnucl->Pdg() : 0;      
    brHitQrk     = qrk;     
    brFromSea    = seaq;  
    brResId      = resid;
    brIsQel      = is_qel;
    brIsRes      = is_res;
    brIsDis      = is_dis;  
    brIsCoh      = is_coh;  
    brIsDfr      = is_dfr;  
    brIsImd      = is_imd;  
    brIsNuEL     = is_nuel;  
    brIsEM       = is_em;  
    brIsCC       = is_weakcc;  
    brIsNC       = is_weaknc;  
    brIsCharmPro = charm;
    brWeight     = weight;      
    brKineXs     = xs;      
    brKineYs     = ys;      
    brKineTs     = ts;      
    brKineQ2s    = Q2s;            
    brKineWs     = Ws;      
    brKineX      = x;      
    brKineY      = y;      
    brKineT      = t;      
    brKineQ2     = Q2;      
    brKineW      = W;      
    brEv         = k1.Energy();      
    brPxv        = k1.Px();  
    brPyv        = k1.Py();  
    brPzv        = k1.Pz();  
    brEn         = (hitnucl) ? p1.Energy() : 0;      
    brPxn        = (hitnucl) ? p1.Px()     : 0;      
    brPyn        = (hitnucl) ? p1.Py()     : 0;      
    brPzn        = (hitnucl) ? p1.Pz()     : 0;            
    brEl         = k2.Energy();      
    brKEl        = fsl->KinE();      
    brPxl        = k2.Px();      
    brPyl        = k2.Py();      
    brPzl        = k2.Pz();      
    brPdgl       = pdgl;
    
    // prim had syst
    brNiP        = 0;
    brNiN        = 0;    
    brNiPip      = 0;    
    brNiPim      = 0;    
    brNiPi0      = 0;    
    brNiKp       = 0;  
    brNiKm       = 0;  
    brNiK0       = 0;  
    brNiEM       = 0;  
    brNiOther    = 0;  
    brNi = prim_had_syst.size();
    
    brPdgi.resize(brNi);
    brResc.resize(brNi);
    brEi.resize(brNi);
    brPxi.resize(brNi);
    brPyi.resize(brNi);
    brPzi.resize(brNi);
    
    for(int j=0; j<brNi; j++) {
        p = event.Particle(prim_had_syst[j]);
        assert(p);
        brPdgi[j] = p->Pdg();     
        brResc[j] = p->RescatterCode();     
        brEi  [j] = p->Energy();     
        brPxi [j] = p->Px();     
        brPyi [j] = p->Py();     
        brPzi [j] = p->Pz();     
        
        if      (p->Pdg() == kPdgProton  || p->Pdg() == kPdgAntiProton)   brNiP++;
        else if (p->Pdg() == kPdgNeutron || p->Pdg() == kPdgAntiNeutron)  brNiN++;
        else if (p->Pdg() == kPdgPiP) brNiPip++; 
        else if (p->Pdg() == kPdgPiM) brNiPim++; 
        else if (p->Pdg() == kPdgPi0) brNiPi0++; 
        else if (p->Pdg() == kPdgKP)  brNiKp++;  
        else if (p->Pdg() == kPdgKM)  brNiKm++;  
        else if (p->Pdg() == kPdgK0    || p->Pdg() == kPdgAntiK0)  brNiK0++; 
        else if (p->Pdg() == kPdgGamma || p->Pdg() == kPdgElectron || p->Pdg() == kPdgPositron) brNiEM++;
        else brNiOther++;
        
        LOG("gntpc", pINFO) 
        << "Counting in primary hadronic system: idx = " << prim_had_syst[j]
        << " -> " << p->Name();
    }
    
    LOG("gntpc", pINFO) 
    << "N(p):"             << brNiP
    << ", N(n):"           << brNiN
    << ", N(pi+):"         << brNiPip
    << ", N(pi-):"         << brNiPim
    << ", N(pi0):"         << brNiPi0
    << ", N(K+,K-,K0):"    << brNiKp+brNiKm+brNiK0
    << ", N(gamma,e-,e+):" << brNiEM
    << ", N(etc):"         << brNiOther << "\n";
    
    // f/s had syst
    brNfP        = 0;
    brNfN        = 0;    
    brNfPip      = 0;    
    brNfPim      = 0;    
    brNfPi0      = 0;    
    brNfKp       = 0;  
    brNfKm       = 0;  
    brNfK0       = 0;  
    brNfEM       = 0;  
    brNfOther    = 0;  
    
    brCalResp0   = 0;
    
    brNf = final_had_syst.size();
    
    brPdgf.resize(brNf);
    brEf.resize(brNf);
    brKEf.resize(brNf);
    brPxf.resize(brNf);
    brPyf.resize(brNf);
    brPzf.resize(brNf);
    
    for(int j=0; j<brNf; j++) {
        p = event.Particle(final_had_syst[j]);
        assert(p);
        
        int    hpdg = p->Pdg();     
        double hE   = p->Energy();     
        double hKE  = p->KinE();     
        double hpx  = p->Px();     
        double hpy  = p->Py();     
        double hpz  = p->Pz();     
        double hm   = p->Mass();     

        brPdgf[j] = hpdg;
        brEf  [j] = hE;
        brKEf [j] = hKE;
        brPxf [j] = hpx;
        brPyf [j] = hpy;
        brPzf [j] = hpz;
        
        if      ( hpdg == kPdgProton      )  { brNfP++;     brCalResp0 += hKE;        }
        else if ( hpdg == kPdgAntiProton  )  { brNfP++;     brCalResp0 += (hE + hm);  }
        else if ( hpdg == kPdgNeutron     )  { brNfN++;     brCalResp0 += hKE;        }
        else if ( hpdg == kPdgAntiNeutron )  { brNfN++;     brCalResp0 += (hE + hm);  }
        else if ( hpdg == kPdgPiP         )  { brNfPip++;   brCalResp0 += hKE;        }
        else if ( hpdg == kPdgPiM         )  { brNfPim++;   brCalResp0 += hKE;        }
        else if ( hpdg == kPdgPi0         )  { brNfPi0++;   brCalResp0 += (e_h * hE); }
        else if ( hpdg == kPdgKP          )  { brNfKp++;    brCalResp0 += hKE;        }
        else if ( hpdg == kPdgKM          )  { brNfKm++;    brCalResp0 += hKE;        }
        else if ( hpdg == kPdgK0          )  { brNfK0++;    brCalResp0 += hKE;        }
        else if ( hpdg == kPdgAntiK0      )  { brNfK0++;    brCalResp0 += hKE;        }
        else if ( hpdg == kPdgGamma       )  { brNfEM++;    brCalResp0 += (e_h * hE); }
        else if ( hpdg == kPdgElectron    )  { brNfEM++;    brCalResp0 += (e_h * hE); }
        else if ( hpdg == kPdgPositron    )  { brNfEM++;    brCalResp0 += (e_h * hE); }
        else                                 { brNfOther++; brCalResp0 += hKE;        }
        
        LOG("gntpc", pINFO) 
        << "Counting in f/s system from hadronic vtx: idx = " << final_had_syst[j]
        << " -> " << p->Name();
    }
    
    LOG("gntpc", pINFO) 
    << "N(p):"             << brNfP
    << ", N(n):"           << brNfN
    << ", N(pi+):"         << brNfPip
    << ", N(pi-):"         << brNfPim
    << ", N(pi0):"         << brNfPi0
    << ", N(K+,K-,K0):"    << brNfKp+brNfKm+brNfK0
    << ", N(gamma,e-,e+):" << brNfEM
    << ", N(etc):"         << brNfOther << "\n";
    
    brVtxX = vtx->X();   
    brVtxY = vtx->Y();   
    brVtxZ = vtx->Z();   
    brVtxT = vtx->T();
  
    
    // finally, fill the map:
    
    gstMap.clear();
    
    gstMap.insert(std::make_pair("iev", brIev));
    
    gstMap.insert(std::make_pair("iev",           brIev));
    gstMap.insert(std::make_pair("neu",	          brNeutrino));
    gstMap.insert(std::make_pair("tgt",           brTarget));
    gstMap.insert(std::make_pair("Z",             brTargetZ));
    gstMap.insert(std::make_pair("A",             brTargetA));
    gstMap.insert(std::make_pair("hitnuc",        brHitNuc));
    gstMap.insert(std::make_pair("hitqrk",        brHitQrk));
    gstMap.insert(std::make_pair("resid",         brResId));
    gstMap.insert(std::make_pair("sea",	          brFromSea));
    gstMap.insert(std::make_pair("qel",	          brIsQel));
    gstMap.insert(std::make_pair("res",	          brIsRes));
    gstMap.insert(std::make_pair("dis",	          brIsDis));
    gstMap.insert(std::make_pair("coh",           brIsCoh));
    gstMap.insert(std::make_pair("dfr",           brIsDfr));
    gstMap.insert(std::make_pair("imd",	          brIsImd));
    gstMap.insert(std::make_pair("nuel",          brIsNuEL));
    gstMap.insert(std::make_pair("em",	          brIsEM));
    gstMap.insert(std::make_pair("cc",	          brIsCC));
    gstMap.insert(std::make_pair("nc",	          brIsNC));
    gstMap.insert(std::make_pair("charm",         brIsCharmPro));
    gstMap.insert(std::make_pair("neut_code",     brCodeNeut));
    gstMap.insert(std::make_pair("nuance_code",   brCodeNuance));
    gstMap.insert(std::make_pair("wght",          brWeight));
    gstMap.insert(std::make_pair("xs",	          brKineXs));
    gstMap.insert(std::make_pair("ys",	          brKineYs));
    gstMap.insert(std::make_pair("ts",	          brKineTs));
    gstMap.insert(std::make_pair("Q2s",	          brKineQ2s));
    gstMap.insert(std::make_pair("Ws",	          brKineWs));
    gstMap.insert(std::make_pair("x",	          brKineX));
    gstMap.insert(std::make_pair("y",	          brKineY));
    gstMap.insert(std::make_pair("t",	          brKineT));
    gstMap.insert(std::make_pair("Q2",	          brKineQ2));
    gstMap.insert(std::make_pair("W",	          brKineW));
    gstMap.insert(std::make_pair("Ev",	          brEv));
    gstMap.insert(std::make_pair("pxv",	          brPxv));
    gstMap.insert(std::make_pair("pyv",	          brPyv));
    gstMap.insert(std::make_pair("pzv",	          brPzv));
    gstMap.insert(std::make_pair("En",	          brEn));
    gstMap.insert(std::make_pair("pxn",	          brPxn));
    gstMap.insert(std::make_pair("pyn",	          brPyn));
    gstMap.insert(std::make_pair("pzn",	          brPzn));
    gstMap.insert(std::make_pair("pdgl",          brPdgl));
    gstMap.insert(std::make_pair("El",	          brEl));
    gstMap.insert(std::make_pair("KEl",	          brKEl));
    gstMap.insert(std::make_pair("pxl",	          brPxl));
    gstMap.insert(std::make_pair("pyl",	          brPyl));
    gstMap.insert(std::make_pair("pzl",	          brPzl));
    gstMap.insert(std::make_pair("nfp",	          brNfP));
    gstMap.insert(std::make_pair("nfn",	          brNfN));
    gstMap.insert(std::make_pair("nfpip",         brNfPip));
    gstMap.insert(std::make_pair("nfpim",         brNfPim));
    gstMap.insert(std::make_pair("nfpi0",         brNfPi0));
    gstMap.insert(std::make_pair("nfkp",          brNfKp));
    gstMap.insert(std::make_pair("nfkm",          brNfKm));
    gstMap.insert(std::make_pair("nfk0",          brNfK0));
    gstMap.insert(std::make_pair("nfem",          brNfEM));
    gstMap.insert(std::make_pair("nfother",       brNfOther));
    gstMap.insert(std::make_pair("nip",	          brNiP));
    gstMap.insert(std::make_pair("nin",	          brNiN));
    gstMap.insert(std::make_pair("nipip",         brNiPip));
    gstMap.insert(std::make_pair("nipim",         brNiPim));
    gstMap.insert(std::make_pair("nipi0",         brNiPi0));
    gstMap.insert(std::make_pair("nikp",          brNiKp));
    gstMap.insert(std::make_pair("nikm",          brNiKm));
    gstMap.insert(std::make_pair("nik0",          brNiK0));
    gstMap.insert(std::make_pair("niem",          brNiEM));
    gstMap.insert(std::make_pair("niother",       brNiOther));
    gstMap.insert(std::make_pair("ni",	          brNi));
    gstMap.insert(std::make_pair("pdgi",          brPdgi));
    gstMap.insert(std::make_pair("resc",          brResc));
    gstMap.insert(std::make_pair("Ei",	          brEi));
    gstMap.insert(std::make_pair("pxi",	          brPxi));
    gstMap.insert(std::make_pair("pyi",	          brPyi));
    gstMap.insert(std::make_pair("pzi",	          brPzi));
    gstMap.insert(std::make_pair("nf",	          brNf));
    gstMap.insert(std::make_pair("pdgf",          brPdgf));
    gstMap.insert(std::make_pair("Ef",	          brEf));
    gstMap.insert(std::make_pair("KEf",           brKEf));
    gstMap.insert(std::make_pair("pxf",	          brPxf));
    gstMap.insert(std::make_pair("pyf",	          brPyf));
    gstMap.insert(std::make_pair("pzf",	          brPzf));
    gstMap.insert(std::make_pair("vtxx",          brVtxX));
    gstMap.insert(std::make_pair("vtxy",          brVtxY));
    gstMap.insert(std::make_pair("vtxz",          brVtxZ));
    gstMap.insert(std::make_pair("vtxt",          brVtxT));
    gstMap.insert(std::make_pair("calresp0",      brCalResp0));
    gstMap.insert(std::make_pair("xsec",          xsec));
    gstMap.insert(std::make_pair("diffxsec",      diffxsec));
    gstMap.insert(std::make_pair("prob",          prob));
    gstMap.insert(std::make_pair("tgtmass",       tgtmass));
    
    return true;
};
