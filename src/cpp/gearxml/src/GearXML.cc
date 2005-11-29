
#include "gearxml/GearXML.h"
#include "gearxml/tinyxml.h"
#include "gearxml/XMLHandlerMgr.h"
#include "gearxml/GearParametersXML.h"
#include "gearxml/TPCParametersXML.h"
#include "gearxml/CalorimeterParametersXML.h"

#include "gearimpl/GearMgrImpl.h"

#include "gear/GEAR.h"
#include "gear/CalorimeterParameters.h"

//#include <algorithm>
#include <sstream>
#include <iostream>

namespace gear{


  GearXML::GearXML( const std::string& fileName ) : 
    _fileName( fileName ),
    _gearMgr(0) {

  } 


  void GearXML::createXMLFile( GearMgr* mgr, const std::string& fileName ) {

    if( mgr == 0 ){
      throw Exception("GearXML::createXMLFile: GearMgr dosn't exist");
    }

    TiXmlDocument doc( fileName )  ;

    TiXmlElement root("gear") ;

    TiXmlElement detectors("detectors") ;

    TiXmlComment rootComment ;
    rootComment.SetValue( "Gear XML file automatically created with GearXML::createXMLFile ...."  ) ;
    root.InsertEndChild ( rootComment) ;


    // --------- add TPC parameters -------------------

    try{
      
      TPCParametersXML handler ;

      TiXmlElement detector = handler.toXML( mgr->getTPCParameters() )  ;
      
      detector.SetAttribute( "name" , "TPC" ) ;
      detector.SetAttribute( "geartype" , GEAR::TPCPARAMETERS ) ;

      detectors.InsertEndChild( detector ) ;

    }
    catch( UnknownParameterException& e){
    }


    // --------- add EcalBarrel parameters -------------------
    try{
      
      CalorimeterParametersXML handler ;

      TiXmlElement detector = handler.toXML( mgr->getEcalBarrelParameters() )  ;
      
      detector.SetAttribute( "name" , "EcalBarrel" ) ;
      detector.SetAttribute( "geartype" , GEAR::CALORIMETERPARAMETERS ) ;

      detectors.InsertEndChild( detector ) ;

    }
    catch( UnknownParameterException& e){
    }

    // --------- add EcalEndcap parameters -------------------
    try{
      
      CalorimeterParametersXML handler ;

      TiXmlElement detector = handler.toXML( mgr->getEcalEndcapParameters() )  ;
      
      detector.SetAttribute( "name" , "EcalEndcap" ) ;
      detector.SetAttribute( "geartype" , GEAR::CALORIMETERPARAMETERS ) ;

      detectors.InsertEndChild( detector ) ;

    }
    catch( UnknownParameterException& e){
    }

    // --------- add HcalBarrel parameters -------------------
    try{
      
      CalorimeterParametersXML handler ;

      TiXmlElement detector = handler.toXML( mgr->getHcalBarrelParameters() )  ;
      
      detector.SetAttribute( "name" , "HcalBarrel" ) ;
      detector.SetAttribute( "geartype" , GEAR::CALORIMETERPARAMETERS ) ;

      detectors.InsertEndChild( detector ) ;

    }
    catch( UnknownParameterException& e){
    }

    // --------- add HcalEndcap parameters -------------------
    try{
      
      CalorimeterParametersXML handler ;

      TiXmlElement detector = handler.toXML( mgr->getHcalEndcapParameters() )  ;
      
      detector.SetAttribute( "name" , "HcalEndcap" ) ;
      detector.SetAttribute( "geartype" , GEAR::CALORIMETERPARAMETERS ) ;

      detectors.InsertEndChild( detector ) ;

    }
    catch( UnknownParameterException& e){
    }
    

    // ------- generic/user detector parameters -----------

    const std::vector<std::string>& keys = mgr->getGearParameterKeys() ;


    for( unsigned int i=0; i<keys.size(); i++ ){


      GearParametersXML handler ;

      TiXmlElement detector = handler.toXML( mgr->getGearParameters( keys[i] ) )  ;
      
      detector.SetAttribute( "name" , keys[i] ) ;
      detector.SetAttribute( "geartype" , GEAR::GEARPARAMETERS ) ;

      detectors.InsertEndChild( detector ) ;

    }



    // ---- now put everything together -------------

    root.InsertEndChild ( detectors ) ;

    doc.InsertEndChild ( root ) ;

    doc.SaveFile() ;

  }


  GearMgr* GearXML::createGearMgr() {

    if( _gearMgr != 0 ){
      
      return _gearMgr ;
      
    } else {
      
      _gearMgr = new GearMgrImpl ;
    }
    
    
    // parse the XML file
    
    TiXmlDocument* doc = new TiXmlDocument ;
    bool loadOkay = doc->LoadFile( _fileName  ) ;
    
    if( !loadOkay ) {

      std::stringstream str ;
      
      str  << "GearXML::createGearMgr error in file [" << _fileName 
	   << ", row: " << doc->ErrorRow() << ", col: " << doc->ErrorCol() << "] : "
	   << doc->ErrorDesc() ;
      
      throw ParseException( str.str() ) ;
    }
    
//     TiXmlHandle docHandle( doc );
    
    TiXmlElement* root = doc->RootElement();

    if( root == 0 ){
      throw ParseException( std::string( "GearXML::createGearMgr : no root tag found in  ") 
			   + _fileName  ) ;
    }
    
    TiXmlNode* detectors = root->FirstChild("detectors")  ;
    if( detectors == 0 ){
      throw ParseException( std::string( "GearXML::createGearMgr : no detectors tag found in  ") 
			   + _fileName  ) ;
    }

    
    TiXmlNode* det = 0 ;
    while( ( det = detectors->IterateChildren( "detector", det ) )  != 0  ){
      
      std::string name  =  getXMLAttribute( det, "name" )  ;

      std::string type("UNKOWN") ;

      try {

	type  =  getXMLAttribute( det, "geartype" )  ;

// 	std::cout << "GearXML::createGearMgr: reading detector " << name 
// 		  << " with \"geartype\" " << type << std::endl ;


      } catch( ParseException& e){

	std::cout << "GearXML::createGearMgr: igoring detector " << name 
		  << " with missing attribute \"geartype\" " << std::endl ;

	continue ;
      }
	
      const XMLHandler* handler = XMLHandlerMgr::instance()->getHandler( type ) ;
      
      if( handler == 0 ) {
	
	throw ParseException( std::string( "GearXML::createGearMgr : unknown geartype \"")  
			      + type + "\" in  file " + _fileName  ) ;
      }
      
      //       GearParameters* gp = 

      handler->fromXML( det->ToElement() , _gearMgr ) ; 

    }
    
    return _gearMgr ;
  }
  
  

}
