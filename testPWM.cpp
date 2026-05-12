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

#include "testPWM.h"
#include <fstream>
#include <string>
#include <iostream>

#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "testPWM_gen.cpp"
#endif

DEFINE_FIRMWARE_FB(FORTE_testPWM, g_nStringIdtestPWM)

const CStringDictionary::TStringId FORTE_testPWM::scm_anDataInputNames[] = {g_nStringIdPIN, g_nStringIdFREQ, g_nStringIdDUTY};

const CStringDictionary::TStringId FORTE_testPWM::scm_anDataInputTypeIds[] = {g_nStringIdINT, g_nStringIdINT, g_nStringIdINT};

const CStringDictionary::TStringId FORTE_testPWM::scm_anDataOutputNames[] = {g_nStringIdSTATUS};

const CStringDictionary::TStringId FORTE_testPWM::scm_anDataOutputTypeIds[] = {g_nStringIdWSTRING};

const TForteInt16 FORTE_testPWM::scm_anEIWithIndexes[] = {-1, -1};
const CStringDictionary::TStringId FORTE_testPWM::scm_anEventInputNames[] = {g_nStringIdINIT, g_nStringIdREQ};

const TDataIOID FORTE_testPWM::scm_anEOWith[] = {0, 255};
const TForteInt16 FORTE_testPWM::scm_anEOWithIndexes[] = {-1, -1, 0};
const CStringDictionary::TStringId FORTE_testPWM::scm_anEventOutputNames[] = {g_nStringIdINITO, g_nStringIdCNF};

const SFBInterfaceSpec FORTE_testPWM::scm_stFBInterfaceSpec = {
  2,  scm_anEventInputNames,  0,  scm_anEIWithIndexes,
  2,  scm_anEventOutputNames,  scm_anEOWith, scm_anEOWithIndexes,  3,  scm_anDataInputNames, scm_anDataInputTypeIds,
  1,  scm_anDataOutputNames, scm_anDataOutputTypeIds,
  0, 0
};


void FORTE_testPWM::executeEvent(int pa_nEIID){
  std::string basePath = "/sys/class/pwm/pwmchip0/";
  
  switch(pa_nEIID){
    case scm_nEventINITID: {
      // Logic INIT: Export kenh PWM
      // PIN o day la chi so kenh (0 hoac 1) trong sysfs
      std::ofstream exportFile(basePath + "export");
      if(exportFile.is_open()){
        exportFile << std::to_string(PIN()); // PIN() lay gia tri tu dau vao PIN
        exportFile.close();
        STATUS() = "PWM Exported";
      } else {
        STATUS() = "Export Failed or Already Exported";
      }
      
      // Gui su kien dau ra INITO
      sendOutputEvent(scm_nEventINITOID);
      break;
    }

    case scm_nEventREQID: {
      // Logic REQ: Cap nhat Frequency va Duty Cycle
      // Tao duong dan toi thu muc pwm0 hoac pwm1
      std::string pwmPath = basePath + "pwm" + std::to_string(PIN()) + "/";
      
      try {
        // 1. Tinh toan Period (nanoseconds)
        // Vi du: 1000Hz -> 1,000,000,000 / 1000 = 1,000,000 ns
        if(FREQ() <= 0) {
            STATUS() = "Frequency must be > 0";
        } else {
            long period = 1000000000L / FREQ(); 
            
            // 2. Tinh toan Duty Cycle (nanoseconds)
            // Cong thuc: (Period * DutyPercent) / 100
            long duty = (period * DUTY()) / 100;

            // 3. Ghi vao Sysfs
            // Tat PWM de dam bao an toan khi thay doi thong so
            std::ofstream fEnable(pwmPath + "enable");
            fEnable << "0"; 
            fEnable.close();

            // Ghi Period truoc
            std::ofstream fPeriod(pwmPath + "period");
            fPeriod << std::to_string(period);
            fPeriod.close();

            // Ghi Duty Cycle sau (phai nho hon hoac bang Period)
            std::ofstream fDuty(pwmPath + "duty_cycle");
            fDuty << std::to_string(duty);
            fDuty.close();

            // Bat lai PWM
            fEnable.open(pwmPath + "enable");
            fEnable << "1"; 
            fEnable.close();

            STATUS() = "PWM Updated";
        }
      } catch (...) {
        STATUS() = "Update Error";
      }

      // Gui su kien xac nhan CNF
      sendOutputEvent(scm_nEventCNFID);
      break;
    }
  }
}