/*
  c++ -o t_profile t_profile.cpp `root-config --glibs --cflags`
  ./t_profile 9.5cm
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>

#include "TF1.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TAxis.h"
#include "TApplication.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TMatrix.h"
#include "TMatrixDSym.h"


double funct_log (double * x, double * par)
  {
    return par[0] + par[1] * log(x[0]/(x[0]-par[2])) ;
  }

using namespace std ;

int main (int argc, char ** argv){

    gStyle->SetOptFit(1112);
    TApplication theApp("theApp", &argc, argv);

    int nbin = 100;
    double sigma_TDC1;
    double sigma_TDC2;
    double e_TDC;

    double m[] = {0.1221, 0.0008013}; // angular coefficient line of calibration TDC
    double q[] = {0.8727, 0.3014};    // offset line of calibration TDC

    vector<double> ADC1, ADC2, TDC_uncorrected, TDC_corrected, x1, e1, y1, ex1, x2, e2, ex2, y2;
    double adc1, adc2, tdc_s, tdc_c, maxADC1 = 0, minADC1 = 10000, maxADC2 = 0, minADC2 = 10000, bin1, bin2, peso1, peso2, sum1 = 0, sumweights1 = 0, sum2 = 0, sumweights2 = 0, i = 0, j = 0;
    double cutSupADC1, cutInfADC1, cutInfADC2, cutSupADC2;
    ifstream data;
    string fileData = "Data/Data_tdcadc_";
    string lenght = argv[1];
    string extension = ".txt";
    data.open((fileData+lenght+extension).c_str());


    if(stod(argv[1]) == 9.5){
      e_TDC = 10.45 ; //canali - 9.878
      cutInfADC1 = 180;
      cutSupADC1 = 300;
      cutInfADC2 = 120;
      cutSupADC2 = 240;
    }
    if(stod(argv[1]) == 38.2){
      e_TDC = 11.91; //canali - 11.25
      cutInfADC1 = 140;
      cutSupADC1 = 320;
      cutInfADC2 = 120;
      cutSupADC2 = 240;
    }
    if(stod(argv[1]) == 97.15){
      e_TDC = 16.39; //canali - 15.53 
      cutInfADC1 = 120;
      cutSupADC1 = 320;
      cutInfADC2 = 80;
      cutSupADC2 = 200;
    }
    if(stod(argv[1]) == 171.5){
      e_TDC = 19.41; //canali -17.84 
      cutInfADC1 = 120;
      cutSupADC1 = 320;
      cutInfADC2 = 80;
      cutSupADC2 = 200;
    }
    while (!data.eof())
    {

      data >> tdc_s;
      tdc_c = tdc_s * m[0] + q[0];

      data >> adc1;
      data >> adc2;
      if (adc1 >= cutInfADC1 && adc1 <= cutSupADC1 && adc2 >= cutInfADC2 && adc2 <= cutSupADC2){

        ADC1.push_back(adc1);
        ADC2.push_back(adc2);
        TDC_uncorrected.push_back(tdc_s);
        TDC_corrected.push_back(tdc_c);

        if (adc1 < minADC1)
          minADC1 = adc1;

        if (adc2 < minADC2)
          minADC2 = adc2;

        if (adc1 > maxADC1)
          maxADC1 = adc1;

        if (adc2 > maxADC2)
          maxADC2 = adc2;
      }
  }
  data.close();

  bin1=(maxADC1-minADC1)/nbin;
      
  bin2=(maxADC2-minADC2)/nbin;
      
  for(i=0;i<nbin;i++){
          
    for(j=0;j<ADC1.size();j++){
      if( ( ADC1.at(j)<(minADC1+(i+1)*bin1) )&&( ADC1.at(j)>(minADC1+i*bin1))){
        peso1 = 1/( pow(q[1], 2) + pow(e_TDC*m[0], 2) +pow(TDC_uncorrected.at(j)*m[1], 2) ); /* peso = 1/sigma quadra */
        sum1=sum1+TDC_corrected.at(j)*peso1;
        sumweights1=sumweights1+peso1;

      }
      if( ( ADC2.at(j)<(minADC2+(i+1)*bin2) )&&( ADC2.at(j)>(minADC2+i*bin2))){
        peso2 = 1/( pow(q[1], 2) + pow(e_TDC*m[0], 2) + pow(TDC_uncorrected.at(j)*m[1], 2) ); 
        sum2=sum2+TDC_corrected.at(j)*peso2;
        sumweights2=sumweights2+peso2;

      }
          
          
    }
          
    x1.push_back(minADC1+i*bin1+bin1/2);
    ex1.push_back(1);
    //cout<<x1.at(i)<<" \n "<<endl;

    y1.push_back(sum1/sumweights1);
    e1.push_back(1/sqrt(sumweights1));
    cout<<e1.at(i)<<" \n "<<endl;

    x2.push_back(minADC2+i*bin2+bin2/2);
    ex2.push_back(1);

    y2.push_back(sum2/sumweights2);
    e2.push_back(1/sqrt(sumweights2));
          
    sum1=0;
    sumweights1=0;
    sum2=0;
    sumweights2=0;

  }




  TGraphErrors funz (x1.size (), &x1[0], &y1[0], &ex1[0], &e1[0]) ;
  TGraphErrors funz2 (x2.size (), &x2[0], &y2[0], &ex2[0], &e2[0]) ;
  
  funz.SetMinimum(20);
  funz.SetMaximum(40);
  
  funz2.SetMinimum(20);
  funz2.SetMaximum(40);


  funz.SetMarkerStyle (105) ;
  funz.SetMarkerColor (4) ;
  funz2.SetMarkerStyle (105) ;
  funz2.SetMarkerColor (4) ;

  funz.SetTitle(" ");

  funz.GetXaxis()->SetTitle("ADC1 [canali]");
  funz.GetXaxis()->SetTitleSize(0.055);
  funz.GetXaxis()->SetLabelSize(0.045);
  funz.GetYaxis()->SetTitle("TDC [ns]");
  funz.GetYaxis()->SetTitleSize(0.055);
  funz.GetYaxis()->SetLabelSize(0.045);
 

  funz2.SetTitle(" ");

  funz2.GetXaxis()->SetTitle("ADC2 [canali]");
  funz2.GetXaxis()->SetTitleSize(0.055);
  funz2.GetXaxis()->SetLabelSize(0.045);

  funz2.GetYaxis()->SetTitle("TDC [ns]");
  funz2.GetYaxis()->SetTitleSize(0.055);
  funz2.GetYaxis()->SetLabelSize(0.045);

 


//fit energy 1

  TF1 f_fit ("f_fit", funct_log, minADC1-bin1 - 50, maxADC1+bin1, 3) ;
  f_fit.SetParName(0, "k");
  f_fit.SetParName(1, "tau");
  f_fit.SetParName(2, "Vs");

  /*
  9:     0 --> 31
         1 --> 0.4
         2 --> 18
  38,2:  0 --> 31 
         1 --> 0.4 
         2 --> 18
  97,15: 0 --> 31
         1 --> 0.4
         2 --> 18
  171,5: 0 --> 31
         1 --> 0.4
         2 --> 80

  */
  f_fit.SetParameter (0, 31);
  f_fit.SetParameter (1, 0.4);
  f_fit.SetParameter (2, 80); //perche i tagli tolgono la parte piu interessante del fit?(dove non è piu una retta)
  TFitResultPtr fit_result = funz.Fit (&f_fit, "SQ+") ;
  fit_result->PrintCovMatrix (cout) ;


  cout << endl ;
  cout.precision (3) ;
  cout << "risultato del fit: " << fit_result->IsValid () << endl ;
  cout << " k: " << f_fit.GetParameter (0) << "\t+- " << f_fit.GetParError (0) << endl ;
  cout << " tau: " << f_fit.GetParameter (1) << "\t+- " << f_fit.GetParError (1) << endl ;
  cout << " Vs: " << f_fit.GetParameter (2) << "\t+- " << f_fit.GetParError (2) << endl ;


  TCanvas c1 ("c1", "", 800, 800) ;
  c1.SetLeftMargin(0.15);
  c1.SetBottomMargin(0.15);
  funz.Draw ("AP") ;
  string fileGraphs = "Graphs/amplitude_walk_";
  string extensionPDF1 = "_ADC1.pdf";
  c1.Print ((fileGraphs+lenght+extensionPDF1).c_str(), "pdf");


// fit energy 2
  TF1 f_fit2 ("f_fit2", funct_log, minADC2-bin2 - 50, maxADC2+bin2, 3);
  f_fit2.SetParName(0, "k");
  f_fit2.SetParName(1, "tau");
  f_fit2.SetParName(2, "Vs");
  /*
  9:     0 --> 31
         1 --> -1.3
         2 --> 31
  38,2:  0 --> 31 
         1 --> -0.04
         2 --> 18
  97,15: 0 --> 31 
         1 --> -0.04
         2 --> 18
  171,5: 0 --> 31
         1 --> -0.04
         2 --> 18
  */
  f_fit2.SetParameter (0, 31); 
  f_fit2.SetParameter (1, -0.04);
  f_fit2.SetParameter (2, 18); 
  TFitResultPtr fit_result2 = funz2.Fit (&f_fit2, "SQ+") ;
  TMatrixDSym cov = fit_result2->GetCovarianceMatrix () ;
  // or TMatrixDSym cov = r->GetCorrelationMatrix();
  for (int i = 0; i < cov.GetNrows () ; ++i)
    {
      for (int j = 0; j < cov.GetNcols () ; ++j)
        {
          cout << cov(i,j) << "\t" ;
        }
      cout << "\n";
    }


  cout << endl ;
  cout.precision (3) ;
  cout << "risultato del fit: " << fit_result2->IsValid () << endl ;
  cout << " k: " << f_fit2.GetParameter (0) << "\t+- " << f_fit2.GetParError (0) << endl ;
  cout << " tau: " << f_fit2.GetParameter (1) << "\t+- " << f_fit2.GetParError (1) << endl ;
  cout << " Vs: " << f_fit2.GetParameter (2) << "\t+- " << f_fit2.GetParError (2) << endl ;

  ofstream OutFile;
  string fileParameters = "Data/corrected_parameters_";
  OutFile.open ((fileParameters+lenght+extension).c_str());
  
  if (!OutFile) {
    cout << "Error in the opening of the file" << endl; 
  } else {
  OutFile << "tau1 \t" << " e_tau1 \t" << " tau2 \t" << " e_tau2 \t" << " Vs1 \t" << " e_vs1 \t\t" << " Vs2 \t" << " e_vs2 " << endl;
  OutFile << f_fit.GetParameter (1) << " " << f_fit.GetParError (1) << " \t " << f_fit2.GetParameter (1)*(-1) << " " <<
    f_fit2.GetParError (1) << " \t "<< f_fit.GetParameter (2) << " " << f_fit.GetParError (2) << " \t " << f_fit2.GetParameter (2) << " " << f_fit2.GetParError (2) << " \n" << endl; 
  OutFile.close();
  }

  TCanvas c2 ("c2", "", 800, 800) ;
  c2.SetLeftMargin(0.15);
  c2.SetBottomMargin(0.15);
  funz2.Draw("AP") ;
  string extensionPDF2 = "_ADC2.pdf";
  c2.Print ((fileGraphs+lenght+extensionPDF2).c_str(), "pdf") ; 

  //theApp.Run();

    return 0 ;
  }
