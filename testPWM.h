/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x!
 ***
 *** Name: testPWM
 *** Description: Service Interface Function Block Type
 *** Version: 
 ***     1.0: 2026-05-12/Tran Hung - null - null
 *************************************************************************/

#ifndef _TESTPWM_H_
#define _TESTPWM_H_

#include <funcbloc.h>
#include <forte_int.h>
#include <forte_wstring.h>

class FORTE_testPWM: public CFunctionBlock{
  DECLARE_FIRMWARE_FB(FORTE_testPWM)

private:
  static const CStringDictionary::TStringId scm_anDataInputNames[];
  static const CStringDictionary::TStringId scm_anDataInputTypeIds[];
  CIEC_INT &PIN() {
    return *static_cast<CIEC_INT*>(getDI(0));
  };

  CIEC_INT &FREQ() {
    return *static_cast<CIEC_INT*>(getDI(1));
  };

  CIEC_INT &DUTY() {
    return *static_cast<CIEC_INT*>(getDI(2));
  };

  static const CStringDictionary::TStringId scm_anDataOutputNames[];
  static const CStringDictionary::TStringId scm_anDataOutputTypeIds[];
  CIEC_WSTRING &STATUS() {
    return *static_cast<CIEC_WSTRING*>(getDO(0));
  };

  static const TEventID scm_nEventINITID = 0;
  static const TEventID scm_nEventREQID = 1;
  static const TForteInt16 scm_anEIWithIndexes[];
  static const CStringDictionary::TStringId scm_anEventInputNames[];

  static const TEventID scm_nEventINITOID = 0;
  static const TEventID scm_nEventCNFID = 1;
  static const TForteInt16 scm_anEOWithIndexes[];
  static const TDataIOID scm_anEOWith[];
  static const CStringDictionary::TStringId scm_anEventOutputNames[];

  static const SFBInterfaceSpec scm_stFBInterfaceSpec;

   FORTE_FB_DATA_ARRAY(2, 3, 1, 0);

  void executeEvent(int pa_nEIID);

public:
  FUNCTION_BLOCK_CTOR(FORTE_testPWM){
  };

  virtual ~FORTE_testPWM(){};

};

#endif //close the ifdef sequence from the beginning of the file

