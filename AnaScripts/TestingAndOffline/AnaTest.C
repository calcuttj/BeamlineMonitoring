#include "../BeamLineClasses/class_BeamLine.h"
#include "../BeamLineClasses/unpack.h"
#include "../BeamLineClasses/plotting.h"
#include <fstream>
#include <sstream>
#include <inttypes.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TROOT.h>


int main(int argc, char *argv[])
{
  gROOT->ProcessLine("#include <vector>");
  std::string s_OutDir = "";

  bool zeroSuppress = argv[1];
  std::vector<std::string> vec_DataPath;
  std::vector<std::string> vec_Detectors;

  for(unsigned int i = 2; i < argc; i++)
  {
    std::string s_Arg = argv[i];
    if(s_Arg.find("/") != std::string::npos)
    {
      vec_DataPath.push_back(s_Arg);
    }
    else
    {
      vec_Detectors.push_back(s_Arg);
    }
  }

  if(vec_DataPath.size()!=0)
  {
    std::cout << "\nDETECTORS: ANALYSING THE FOLLOWING DETECTORS:\n" << std::endl;

    std::map<std::string,Detector> map_Detector;
    for(unsigned int det = 0; det < vec_Detectors.size(); det++)
    {
      std::string s_DetName = vec_Detectors.at(det);
      std::string s_DetType = s_DetName.substr(0,4);

      if(s_DetName[0]=='G')
      {
        s_DetType = "XBTF"; 
      }
      else if(s_DetName[0]=='S')
      {
        s_DetType = "XBTF";
      }
      else
      {
        s_DetType = s_DetName.substr(0,4);
      }

      Detector detector(s_DetName, s_DetType, ((s_DetName[0]=='G' || s_DetName[0]=='S') ? "XBH4_"+s_DetType+"_"+s_DetName : "XBH4_"+s_DetType+"_022_"+s_DetName.substr(7)));
      map_Detector[s_DetName] = detector;
      detector.printDescription();
    }

    std::cout << "\nLOOKING AT DIP ROOT FILES IN: " << std::endl;
    for(unsigned int data = 0; data < vec_DataPath.size(); data++)
    {
      std::string s_DataPath = vec_DataPath.at(data);
      std::cout << s_DataPath << std::endl;
      unpackDIPROOT(map_Detector, s_DataPath, zeroSuppress);
    }

    std::map<std::string,std::vector<TH1I*>> map_NameToVecHist_I;
    std::map<std::string,std::vector<TH1D*>> map_NameToVecHist_D;
    std::map<std::string,std::vector<TH1D*>> map_NameToVecHistBeamLine_D;

    std::cout << "\nUNPACKING DONE! MAKING SINGLE DETECTOR PLOTS..." << std::endl;
    plot_dTAcq     (map_Detector, map_NameToVecHist_I);
    plot_AcqLevel  (map_Detector, map_NameToVecHist_I);
    plot_EventLevel(map_Detector, map_NameToVecHist_I, map_NameToVecHist_D);

    BeamLine beamline;
    //FINDS AND FILLS THE TF COINCIDENCE RECORD VARIABLE IN OUR INSTANCE OF BEAMLINE.
    std::cout << "\nSINGLE DETECTOR PLOTS DONE, FINDING COINCIDENCES IN TF" << std::endl;
    beamline.findTFCoincidences  (map_Detector);
    beamline.printTFCoincidencesUnique(map_Detector);
    //FINDS AND FILLS THE PROF COINCIDENCE RECORD VARIABLE IN OUR INSTANCE OF BEAMLINE.
    std::cout << "\nFINDING COINCIDENCES IN PROF" << std::endl;
    beamline.findPROFCoincidences(map_Detector);
    beamline.printPROFCoincidencesUnique(map_Detector);
    std::cout << "FINDING TF PROF COINCIDENCES" << std::endl;
    beamline.findTFPROFCoincidences(map_Detector, true);

    //beamline.printTFCoincidencesDegenerate(map_Detector);
    //beamline.printTFCoincidencesUnique(map_Detector);
    //beamline.printPROFCoincidencesDegenerate(map_Detector);
    //beamline.printPROFCoincidencesUnique(map_Detector);
    beamline.printTFPROFCoincidencesUnique(map_Detector);

    std::vector<PROFCoincidenceRecord::PROFCoincidence> vec_PROFCO_Degen = beamline.getPROFCoincidencesDegenerate();
    std::vector<PROFCoincidenceRecord::PROFCoincidence> vec_PROFCO_Uniq  = beamline.getPROFCoincidencesUnique();
    std::vector<TFCoincidenceRecord::TFCoincidence>     vec_TFCO_Degen   = beamline.getTFCoincidencesDegenerate();
    std::vector<TFCoincidenceRecord::TFCoincidence>     vec_TFCO_Uniq    = beamline.getTFCoincidencesUnique();

    std::vector<TH1D*> vec_PROFHist1D;
    std::vector<TH2D*> vec_PROFHist2D;
    std::vector<TH1D*> vec_TFHist1D;
    std::vector<TH2D*> vec_TFHist2D;

    std::cout << "\nFINDING DONE, MAKING PLOTS" << std::endl;
    //plotPROF(vec_PROFCO_Degen, vec_PROFHist1D, vec_PROFHist2D, "DEGEN"); 
    //plotPROF(vec_PROFCO_Uniq,  vec_PROFHist1D, vec_PROFHist2D, "UNIQ" ); 
    //plotTF  (vec_TFCO_Degen,     vec_TFHist1D,   vec_TFHist2D, "DEGEN");
    //plotTF  (vec_TFCO_Uniq,      vec_TFHist1D,   vec_TFHist2D, "UNIQ" );

    std::cout << "\nPLOTS MADE, DUMPING TO TREES" << std::endl;

    std::string s_FirstFile = vec_DataPath.at(0);
    std::string s_LastFile  = vec_DataPath.at(vec_DataPath.size()-1);
    std::string s_FirstTime = s_FirstFile.substr(s_FirstFile.length()-15,10);
    std::string s_LastTime  = s_LastFile .substr(s_LastFile .length()-15,10);

    //beamline.dumpBIData                    (map_Detector,(TString)s_OutDir+"Analyse_BIBL_Tree_"+(TString)s_FirstTime+"_"+(TString)s_LastTime+".root");
    //beamline.dumpTFCoincidencesDegenerate  (map_Detector,(TString)s_OutDir+"Analyse_BIBL_Tree_"+(TString)s_FirstTime+"_"+(TString)s_LastTime+".root");
    //beamline.dumpTFCoincidencesUnique      (map_Detector,(TString)s_OutDir+"Analyse_BIBL_Tree_"+(TString)s_FirstTime+"_"+(TString)s_LastTime+".root");
    //beamline.dumpPROFCoincidencesDegenerate(map_Detector,(TString)s_OutDir+"Analyse_BIBL_Tree_"+(TString)s_FirstTime+"_"+(TString)s_LastTime+".root");
    //beamline.dumpPROFCoincidencesUnique    (map_Detector,(TString)s_OutDir+"Analyse_BIBL_Tree_"+(TString)s_FirstTime+"_"+(TString)s_LastTime+".root");

    TFile *f_Out = new TFile((TString)s_OutDir+"Analyse_BIBL_Hist_"+(TString)s_FirstTime+"_"+(TString)s_LastTime+".root","RECREATE");
    for(auto detector : map_NameToVecHist_I)
    {
      TDirectory *dir = f_Out->mkdir((TString)detector.first);
      dir->cd();
      for(auto vec_Hist_I : detector.second)
      {
        vec_Hist_I->Write();
      }
    }

    for(auto detector : map_NameToVecHist_D)
    {
      TDirectory *dir = (TDirectory*)f_Out->Get((TString)detector.first);
      dir->cd();
      for(auto vec_Hist_D : detector.second)
      {
        vec_Hist_D->Write();
      }
    }

    TDirectory *dir_PROF = f_Out->mkdir("PROF");
    dir_PROF->cd();
    for(auto hist : vec_PROFHist1D){hist->Write();}
    for(auto hist : vec_PROFHist2D){hist->Write();}
    TDirectory *dir_TF = f_Out->mkdir("TF");
    dir_TF->cd();
    for(auto hist : vec_TFHist1D){hist->Write();}
    for(auto hist : vec_TFHist2D){hist->Write();}

    f_Out->Close();
    delete f_Out;

    std::ofstream latestFile;
    latestFile.open(s_OutDir+"latestTreeFile.txt");
    latestFile << s_OutDir+"Analyse_BIBL_Tree_"+s_FirstTime+"_"+s_LastTime+".root" << std::endl;
    latestFile.close();
    latestFile.open(s_OutDir+"latestHistFile.txt");
    latestFile << s_OutDir+"Analyse_BIBL_Hist_"+s_FirstTime+"_"+s_LastTime+".root" << std::endl;
    latestFile.close();
    latestFile.open(s_OutDir+"lxplus_HistFiles.txt",std::ios_base::app);
    latestFile << s_OutDir+"Analyse_BIBL_Hist_"+s_FirstTime+"_"+s_LastTime+".root ";
    latestFile.close();

    std::cout << "\nHISTOGRAMS AND TREES DONE!\n" << std::endl;
  }
  else
  {
    std::cout << "DETECTORS AND BEAMLINE: NO NEW DATA FILES!" << std::endl;
  }

  return 0;
}
