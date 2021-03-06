#include <xAODAnaHelpers/ElectronHists.h>
#include <sstream>

#include "xAODAnaHelpers/HelperFunctions.h"

ANA_MSG_SOURCE(msgElectronHists, "ElectronHists")

ElectronHists :: ElectronHists (std::string name, std::string detailStr) :
  IParticleHists(name, detailStr, "electron", "electron"),
  m_infoSwitch(new HelperClasses::ElectronInfoSwitch(m_detailStr))
{ }

ElectronHists :: ~ElectronHists () {
  if(m_infoSwitch) delete m_infoSwitch;
}

StatusCode ElectronHists::initialize() {
  using namespace msgElectronHists;
  ANA_CHECK( IParticleHists::initialize());

  // isolation
  if( m_infoSwitch->m_isolation ) {
    if(m_debug) Info("ElectronHists::initialize()", "adding isolation plots");

    for (auto& isol : m_infoSwitch->m_isolWPs) {
      if (isol.empty() && isol == "NONE") continue;

      m_isIsolated[isol] = book(m_name, "isIsolated_" + isol,   "isIsolated_" + isol, 3, -1.5, 1.5);
    }

    m_ptcone20     = book(m_name, "ptcone20",     "ptcone20",     101, -0.2, 20);
    m_ptcone30     = book(m_name, "ptcone30",     "ptcone30",     101, -0.2, 20);
    m_ptcone40     = book(m_name, "ptcone40",     "ptcone40",     101, -0.2, 20);
    m_ptvarcone20  = book(m_name, "ptvarcone20",  "ptvarcone20",  101, -0.2, 20);
    m_ptvarcone30  = book(m_name, "ptvarcone30",  "ptvarcone30",  101, -0.2, 20);
    m_ptvarcone40  = book(m_name, "ptvarcone40",  "ptvarcone40",  101, -0.2, 20);
    m_topoetcone20 = book(m_name, "topoetcone20", "topoetcone20", 101, -0.2, 20);
    m_topoetcone30 = book(m_name, "topoetcone30", "topoetcone30", 101, -0.2, 20);
    m_topoetcone40 = book(m_name, "topoetcone40", "topoetcone40", 101, -0.2, 20);

    m_ptcone20_rel     = book(m_name, "ptcone20_rel",     "ptcone20_rel",     110, -0.2, 2);
    m_ptcone30_rel     = book(m_name, "ptcone30_rel",     "ptcone30_rel",     110, -0.2, 2);
    m_ptcone40_rel     = book(m_name, "ptcone40_rel",     "ptcone40_rel",     110, -0.2, 2);
    m_ptvarcone20_rel  = book(m_name, "ptvarcone20_rel",  "ptvarcone20_rel",  110, -0.2, 2);
    m_ptvarcone30_rel  = book(m_name, "ptvarcone30_rel",  "ptvarcone30_rel",  110, -0.2, 2);
    m_ptvarcone40_rel  = book(m_name, "ptvarcone40_rel",  "ptvarcone40_rel",  110, -0.2, 2);
    m_topoetcone20_rel = book(m_name, "topoetcone20_rel", "topoetcone20_rel", 110, -0.2, 2);
    m_topoetcone30_rel = book(m_name, "topoetcone30_rel", "topoetcone30_rel", 110, -0.2, 2);
    m_topoetcone40_rel = book(m_name, "topoetcone40_rel", "topoetcone40_rel", 110, -0.2, 2);
}

  // PID
  if (m_infoSwitch->m_PID || m_infoSwitch->m_quality) {
    for (auto& PID : m_infoSwitch->m_PIDWPs) {
      if (PID.empty()) continue;

      m_PID[PID] = book(m_name, PID, PID, 3, -1.5, 1.5);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode ElectronHists::execute( const xAOD::Electron* electron, float eventWeight, const xAOD::EventInfo* eventInfo ) {
  return execute(static_cast<const xAOD::IParticle*>(electron), eventWeight, eventInfo);
}

StatusCode ElectronHists::execute( const xAOD::IParticle* particle, float eventWeight, const xAOD::EventInfo* eventInfo ) {
  using namespace msgElectronHists;
  ANA_CHECK( IParticleHists::execute(particle, eventWeight, eventInfo));

  if(m_debug) std::cout << "ElectronHists: in execute " <<std::endl;

  const xAOD::Electron* electron=dynamic_cast<const xAOD::Electron*>(particle);
  if(electron==0)
    {
      ANA_MSG_ERROR( "Cannot convert IParticle to Electron" );
      return StatusCode::FAILURE;
    }

  // isolation
  if ( m_infoSwitch->m_isolation ) {
    static std::map< std::string, SG::AuxElement::Accessor<char> > accIsol;

    for (auto& isol : m_infoSwitch->m_isolWPs) {
      if (!isol.empty() && isol != "NONE") {
        std::string isolWP = "isIsolated_" + isol;
        accIsol.insert( std::pair<std::string, SG::AuxElement::Accessor<char> > ( isol , SG::AuxElement::Accessor<char>( isolWP ) ) );
        
        if (accIsol.at(isol).isAvailable(*electron)) {
            m_isIsolated[isol]->Fill(accIsol.at(isol)(*electron), eventWeight);
        } else {
            m_isIsolated[isol]->Fill(-1 , eventWeight);
        }
      }
    }

    m_ptcone20    ->Fill( electron->isolation( xAOD::Iso::ptcone20    ) / 1e3, eventWeight );
    m_ptcone30    ->Fill( electron->isolation( xAOD::Iso::ptcone30    ) / 1e3, eventWeight );
    m_ptcone40    ->Fill( electron->isolation( xAOD::Iso::ptcone40    ) / 1e3, eventWeight );
    m_ptvarcone20 ->Fill( electron->isolation( xAOD::Iso::ptvarcone20 ) / 1e3, eventWeight );
    m_ptvarcone30 ->Fill( electron->isolation( xAOD::Iso::ptvarcone30 ) / 1e3, eventWeight );
    m_ptvarcone40 ->Fill( electron->isolation( xAOD::Iso::ptvarcone40 ) / 1e3, eventWeight );
    m_topoetcone20->Fill( electron->isolation( xAOD::Iso::topoetcone20) / 1e3, eventWeight );
    m_topoetcone30->Fill( electron->isolation( xAOD::Iso::topoetcone30) / 1e3, eventWeight );
    m_topoetcone40->Fill( electron->isolation( xAOD::Iso::topoetcone40) / 1e3, eventWeight );

    float electronPt = electron->pt();
    m_ptcone20_rel     ->Fill( electron->isolation( xAOD::Iso::ptcone20 )     / electronPt,  eventWeight );
    m_ptcone30_rel     ->Fill( electron->isolation( xAOD::Iso::ptcone30 )     / electronPt,  eventWeight );
    m_ptcone40_rel     ->Fill( electron->isolation( xAOD::Iso::ptcone40 )     / electronPt,  eventWeight );
    m_ptvarcone20_rel  ->Fill( electron->isolation( xAOD::Iso::ptvarcone20 )  / electronPt,  eventWeight );
    m_ptvarcone30_rel  ->Fill( electron->isolation( xAOD::Iso::ptvarcone30 )  / electronPt,  eventWeight );
    m_ptvarcone40_rel  ->Fill( electron->isolation( xAOD::Iso::ptvarcone40 )  / electronPt,  eventWeight );
    m_topoetcone20_rel ->Fill( electron->isolation( xAOD::Iso::topoetcone20 ) / electronPt,  eventWeight );
    m_topoetcone30_rel ->Fill( electron->isolation( xAOD::Iso::topoetcone30 ) / electronPt,  eventWeight );
    m_topoetcone40_rel ->Fill( electron->isolation( xAOD::Iso::topoetcone40 ) / electronPt,  eventWeight );

  }

  return StatusCode::SUCCESS;
}



StatusCode ElectronHists::execute( const xAH::Electron* elec, float eventWeight, const xAH::EventInfo* eventInfo ) {
  return execute(static_cast<const xAH::Particle*>(elec), eventWeight, eventInfo);
}


StatusCode ElectronHists::execute( const xAH::Particle* particle, float eventWeight, const xAH::EventInfo* /*eventInfo*/  ) {
  using namespace msgElectronHists;
  ANA_CHECK( IParticleHists::execute(particle, eventWeight));

  if(m_debug) std::cout << "ElectronHists: in execute " <<std::endl;

  const xAH::Electron* elec=dynamic_cast<const xAH::Electron*>(particle);
  if(elec==0)
    {
      ANA_MSG_ERROR( "Cannot convert IParticle to Electron" );
      return StatusCode::FAILURE;
    }

  // isolation
  if ( m_infoSwitch->m_isolation ) {
    for (auto& isol : m_infoSwitch->m_isolWPs) {
      if (isol.empty() && isol == "NONE") continue;

      m_isIsolated[isol]->Fill(elec->isIsolated.at(isol), eventWeight);
    }

    m_ptcone20    ->Fill( elec->ptcone20    , eventWeight );
    m_ptcone30    ->Fill( elec->ptcone30    , eventWeight );
    m_ptcone40    ->Fill( elec->ptcone40    , eventWeight );
    m_ptvarcone20 ->Fill( elec->ptvarcone20 , eventWeight );
    m_ptvarcone30 ->Fill( elec->ptvarcone30 , eventWeight );
    m_ptvarcone40 ->Fill( elec->ptvarcone40 , eventWeight );
    m_topoetcone20->Fill( elec->topoetcone20, eventWeight );
    m_topoetcone30->Fill( elec->topoetcone30, eventWeight );
    m_topoetcone40->Fill( elec->topoetcone40, eventWeight );


    float elecPt = elec->p4.Pt();
    m_ptcone20_rel     ->Fill( elec->ptcone20/elecPt        ,  eventWeight );
    m_ptcone30_rel     ->Fill( elec->ptcone30/elecPt        ,  eventWeight );
    m_ptcone40_rel     ->Fill( elec->ptcone40/elecPt        ,  eventWeight );
    m_ptvarcone20_rel  ->Fill( elec->ptvarcone20 /elecPt    ,  eventWeight );
    m_ptvarcone30_rel  ->Fill( elec->ptvarcone30 /elecPt    ,  eventWeight );
    m_ptvarcone40_rel  ->Fill( elec->ptvarcone40 /elecPt    ,  eventWeight );
    m_topoetcone20_rel ->Fill( elec->topoetcone20/elecPt    ,  eventWeight );
    m_topoetcone30_rel ->Fill( elec->topoetcone30/elecPt    ,  eventWeight );
    m_topoetcone40_rel ->Fill( elec->topoetcone40/elecPt    ,  eventWeight );
  }


  if ( m_infoSwitch->m_PID || m_infoSwitch->m_quality ) {
    for (auto& PID : m_infoSwitch->m_PIDWPs) {
      if (PID.empty()) continue;

      m_PID[PID]->Fill(elec->PID.at(PID), eventWeight);
    }
  }

  return StatusCode::SUCCESS;
}
