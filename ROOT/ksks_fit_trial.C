//#ifndef __CINT__
#include "RooGlobalFunc.h"
//#endif
#include "RooRealVar.h"
#include "RooArgList.h"
#include "RooFormulaVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooChebychev.h"
#include "RooExponential.h"
#include "RooBifurGauss.h"
#include "RooAddModel.h"
#include "RooProdPdf.h"
#include "TCanvas.h"
#include "RooPlot.h"
#include "RooHist.h"
#include "RooCBShape.h"
#include "RooPolynomial.h"
#include "RooBinning.h"
#include "TH1.h"
#include "TH2.h"
#include "RooAddPdf.h"
#include "RooProdPdf.h"
#include "RooFitResult.h"
#include "RooGenericPdf.h"
#include "RooLandau.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "RooPlot.h"
#include "RooCategory.h"
#include "RooSuperCategory.h"
#include "RooSimultaneous.h"
#include "RooNLLVar.h"
#include "TFile.h"
#include "myRooJohnsonSU.cpp"
#include "myRooPolBG.cpp"

using namespace RooFit ;
using namespace std;

void ksks_fit_trial()
{
  //TFile *f = new TFile("forfit_proc11_bucket9to12_bcs.root");
  //TFile *f = new TFile("forfit_bucket9to12_bcs.root");
  //TFile *f = new TFile(" forfit_proc11_bcs.root");
  TFile *f = new TFile("forfit_mc13b_proc11_large.root");
  

  TTree *tree = (TTree*)f->Get("after_bcs");
  cout << "Entries = " << tree->GetEntries() << endl;
  
  RooRealVar deltaM("deltaM_after_bcs","", 0.139, 0.159, "");
  //RooRealVar deltaM("deltaM_prefit_after_bcs","#it{#Deltam}", 0.139, 0.159, "GeV/#it{c}^{2}");
  RooDataSet data("data","data", tree, RooArgSet(deltaM));
  
  //signal
  RooRealVar mean("#mu", "mean",0.1454, 0.14, 0.159);
  RooRealVar sigma("#sigma", "sigma", 0.0002, 0, 10);
  RooRealVar delta("delta", "delta", 1.0);
  RooRealVar gamma("gamma", "gamma", 0.0);
  //RooGaussian sig("sig", "signal component", deltaM, mean, sigma);
  myRooJohnsonSU sig("sig", "signal component", deltaM, mean, sigma, delta, gamma);

  //background
  RooRealVar mpi("mpi", "nominal pion mass", 0.13957039);
  RooRealVar alpha("#alpha", "alpha", 20, -100., 100.);
  RooRealVar beta("#beta", "beta", 0.);
  myRooPolBG bkg("bkg","background component", deltaM, mpi, alpha, beta);
  
  //total
  RooRealVar n_sig("N_{sig}", "n_{s}", 2000, 0, 10000);
  RooRealVar n_bkg("N_{bkg}", "n_{b}", 100, 0, 10000);
  RooAddPdf pdf("pdf", "two component model",RooArgList(sig, bkg), RooArgList(n_sig, n_bkg));
  
  //fit
  RooFitResult *fitresult = pdf.fitTo(data, Save(true), Strategy(2), Extended(true));
  
  RooPlot* deltaMplot = deltaM.frame();
  data.plotOn(deltaMplot);
  pdf.plotOn(deltaMplot);
  pdf.paramOn(deltaMplot);
  deltaMplot->SetYTitle("Candidates per 0.2 MeV/#it{c}^{2}");
  deltaMplot->GetXaxis()->SetLabelOffset(999);
  double nbins = 5*deltaMplot->chiSquare(5) / (deltaMplot->chiSquare(5) - deltaMplot->chiSquare());
  cout << "chi^2/ndf = " << deltaMplot->chiSquare()*nbins << "/" << nbins-5 << endl;
  
  RooHist* hpull = deltaMplot->pullHist();
  hpull->SetFillStyle(1001);
  hpull->SetFillColor(1);
  for(int i=0;i<hpull->GetN();++i) hpull->SetPointError(i,0.0,0.0,0.0,0.0);
  RooPlot* pullplot = deltaM.frame(Title(""));
  pullplot->addPlotable(hpull,"B");
  // pullplot->SetYTitle("Pull");
  pullplot->SetXTitle("#it{#Deltam} [GeV/c^{2}]");
  pullplot->SetMinimum(-4.);
  pullplot->SetMaximum(4.);
  pullplot->GetXaxis()->SetLabelSize(0.1);
  //pullplot->GetXaxis()->SetTitleSize(0.1);
  pullplot->GetYaxis()->SetLabelSize(0.07);
  
  
  TCanvas *canvas = new TCanvas("canvas","canvas", 700, 800);
  //TCanvas *canvas = new TCanvas();

  Double_t xlow, ylow, xup, yup;
  canvas->GetPad(0)->GetPadPar(xlow,ylow,xup,yup);
  canvas->Divide(1,2);

  TVirtualPad *upPad = canvas->GetPad(1);
  upPad->SetPad(xlow,ylow+0.25*(yup-ylow),xup,yup);
  
  TVirtualPad *dwPad = canvas->GetPad(2);
  dwPad->SetPad(xlow,ylow,xup,ylow+0.25*(yup-ylow));

  /*TPad *upPad = new TPad("Pad1", " ",0.0,0.25,1.0,1.0);
  upPad->SetPad(0.025,0.08, 0.1, 0.05);
  TPad *dwnPad = new TPad("Pad2", " ",0.0,0.0,1.0,0.25);
  dwnPad->SetPad(0.4,0.03,0.1,0.05);*/
  
    
  canvas->Update();
  canvas->cd(1);
  
  pdf.plotOn(deltaMplot,Components(RooArgSet(sig)),LineColor(kRed),LineStyle(kDashed));
  pdf.plotOn(deltaMplot,Components(RooArgSet(bkg)),LineColor(kBlue),LineStyle(kDashed));
  
  deltaMplot->Draw();
  canvas->cd(2);
  pullplot->Draw();

  
}
