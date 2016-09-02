/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3IceTopResponseService.cxx
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */

#include <topsimulator/interface/I3IceTopResponseService.h>
#include <icetray/I3Units.h>

#ifdef I3_USE_ROOT
#include <TH2F.h>
#include <TFile.h>
#endif

using namespace std;

I3IceTopResponseService::I3IceTopResponseService(const I3Context& context): I3ServiceBase(context),
  xbins_(0), xwidth_(0), xmin_(0), xmax_(0),
  ybins_(0), ywidth_(0), ymin_(0), ymax_(0),
  meanz_(NAN), gridSize_(NAN), samplRadius_(NAN)
{
  tankMap_ = I3TankResponseMapPtr(new I3TankResponseMap());
  ResetGrid(xmin_, xwidth_, xbins_, ymin_, ywidth_, ybins_);
  
  gridSize_ = 5.0;
  AddParameter("SearchGridSize",
	       "Pixel size of search grid in [m]",
	       gridSize_);

  samplRadius_ = 25.0;
  AddParameter("TankSamplingRadius",
	       "TankSamplingRadius in [m]",
	       samplRadius_);
}


void I3IceTopResponseService::Configure()
{
  log_info("Configuring I3IceTopResponseService:");
  
  GetParameter("SearchGridSize", gridSize_);
  gridSize_*=I3Units::m;
  log_info("+ Search grid pixel size: %.2f m", gridSize_/I3Units::m); 
  
  GetParameter("TankSamplingRadius", samplRadius_);
  samplRadius_*=I3Units::m;
  log_info("+ Tank sampling radius: %.2f m", samplRadius_/I3Units::m);
}


void I3IceTopResponseService::ResetGrid(double xmin, double xwidth, int xbins, double ymin, double ywidth, int ybins)
{
    xmin_   = xmin;
    xbins_  = xbins;
    xwidth_ = xwidth;
    xmax_   = xmin_ + xbins_*xwidth_;

    ymin_   = ymin;
    ybins_  = ybins;
    ywidth_ = ywidth;
    ymax_   = ymin_ + ybins_*xwidth_;
    
    fastGrid_.clear();
    fastGrid_.resize(xbins_*ybins_, I3TankResponseMapPtr());
}


void I3IceTopResponseService::InitializeGrid()
{
  // Determine the maximum dimensions of the grid
  double xmin = 0;
  double xmax = 0;
  double ymin = 0;
  double ymax = 0;
  double zsum = 0;
  
  I3TankResponseMap::const_iterator tank_iter;
  for(tank_iter=tankMap_->begin(); tank_iter!=tankMap_->end(); ++tank_iter)
  {
    double x = tank_iter->second->GetX();
    double y = tank_iter->second->GetY();
    
    if(tank_iter==tankMap_->begin())
    {
      xmin = x; 
      xmax = x;
      ymin = y;
      ymax = y;
    }
    else
    {
      xmin = min(xmin, x);
      xmax = max(xmax, x);
      ymin = min(ymin, y);
      ymax = max(ymax, y);
    }

    zsum += tank_iter->second->GetZ();
  }

  xmin -= (samplRadius_ + gridSize_);
  xmax += (samplRadius_ + gridSize_);
  ymin -= (samplRadius_ + gridSize_);
  ymax += (samplRadius_ + gridSize_);

  meanz_ = zsum/tankMap_->size();
  
  int xbins = static_cast<int>(ceil((xmax - xmin)/gridSize_));
  int ybins = static_cast<int>(ceil((ymax - ymin)/gridSize_));
  ResetGrid(xmin, gridSize_, xbins, ymin, gridSize_, ybins); 
  
  
  // Add tanks to grid
  for(tank_iter=tankMap_->begin(); tank_iter!=tankMap_->end(); ++tank_iter)
  {
    int xcBin = GetBinX(tank_iter->second->GetX());
    double xc = GetBinCenterX(xcBin);
    
    int ycBin = GetBinY(tank_iter->second->GetY());
    double yc = GetBinCenterY(ycBin);
    
    int yminBin = GetBinY(yc - samplRadius_);
    int ymaxBin = GetBinY(yc + samplRadius_);
    
    for(int ybin = yminBin; ybin<=ymaxBin; ybin++)
    {
      double dySqr = pow(yc - GetBinCenterY(ybin), 2);
      
      int xminBin = GetBinX(xc - sqrt(samplRadius_*samplRadius_ - dySqr));
      int xmaxBin = GetBinX(xc + sqrt(samplRadius_*samplRadius_ - dySqr));
    
      for(int xbin = xminBin; xbin<=xmaxBin; xbin++)
      {
	double x = GetBinCenterX(xbin);
	double y = GetBinCenterY(ybin);
      
	AddTank(x, y, tank_iter->first, tank_iter->second);
      }
    }
  }
  
  log_info("Registered %zu tanks in %zu bins (sampling radius = %.1f m):",
	   tankMap_->size(), fastGrid_.size(), samplRadius_/I3Units::m);
  log_info("+ Binning X: xmin=%.1f m, xmax=%.1f m, binwidth=%.1f m, nbins=%d",
	   xmin_/I3Units::m, xmax_/I3Units::m, xwidth_/I3Units::m, xbins_);
  log_info("+ Binning Y: ymin=%.1f m, ymax=%.1f m, binwidth=%.1f m, nbins=%d",
	   ymin_/I3Units::m, ymax_/I3Units::m, ywidth_/I3Units::m, ybins_);
  log_info("+ Mean z position of tanks: %.2f m", meanz_/I3Units::m);
}


I3TankResponsePtr I3IceTopResponseService::GetTankResponse(const TankKey& tankKey)
{
  I3TankResponseMap::iterator iter = tankMap_->find(tankKey);
  if(iter!=tankMap_->end()) return iter->second;
  else return I3TankResponsePtr();
}


int I3IceTopResponseService::GetBinX(double x)
{
    if(x < xmin_) return -1;
    if(x > xmax_) return -1;
    return static_cast<int>((x - xmin_)/xwidth_);
}


int I3IceTopResponseService::GetBinY(double y)
{
    if(y < ymin_) return -1;
    if(y > ymax_) return -1;
    return static_cast<int>((y - ymin_)/ywidth_);
}


int I3IceTopResponseService::GetBin(double x, double y)
{
    if(x < xmin_) return -1;
    if(x > xmax_) return -1;
    if(y < ymin_) return -1;
    if(y > ymax_) return -1;
    
    int xbin = static_cast<int>((x - xmin_)/xwidth_);
    int ybin = static_cast<int>((y - ymin_)/ywidth_);
    
    return ybin*xbins_ + xbin;
}


double I3IceTopResponseService::GetBinCenterX(int xbin)
{
    return xmin_ + (xbin + 0.5)*xwidth_;
}


double I3IceTopResponseService::GetBinCenterY(int ybin)
{
    return ymin_ + (ybin + 0.5)*ywidth_;
}


void I3IceTopResponseService::AddTank(double x, double y, const TankKey& tankKey, I3TankResponsePtr response)
{
    int bin =  GetBin(x, y);
    if(bin<0)
    {
      log_warn("Tank %s at position (%.1f, %.1f) lies outside boundaries!",
               tankKey.str().c_str(), x, y);
      return;
    }
    
    if(!fastGrid_.at(bin))
    {
      fastGrid_[bin] = I3TankResponseMapPtr(new I3TankResponseMap());
    }
    
    fastGrid_.at(bin)->insert(make_pair(tankKey, response));
}


I3TankResponseMapPtr I3IceTopResponseService::GetTankResponseMap(double x, double y)
{
    int bin = GetBin(x, y);
    if(bin<0) return I3TankResponseMapPtr();
    else      return fastGrid_.at(bin);
}


I3TankResponseMapPtr I3IceTopResponseService::GetTankResponseMap(const I3Particle& track)
{
  // Calculate intersection point of particle with plane
  // through the mean Z coordinate of all tank centers.
  double shift = (meanz_ - track.GetZ())/track.GetDir().GetZ();
  double x  = track.GetX() + shift*track.GetDir().GetX();
  double y  = track.GetY() + shift*track.GetDir().GetY();
  
  return GetTankResponseMap(x, y);
}


void I3IceTopResponseService::BeginEvent(const I3Particle& primary)
{
  I3TankResponseMap::const_iterator iter;
  for(iter=tankMap_->begin(); iter!=tankMap_->end(); ++iter) iter->second->BeginEvent(primary);
}


void I3IceTopResponseService::EndEvent(HitHistoCollection &hitHC, HitHistoCollection& cherHitCollection)
{
  I3TankResponseMap::const_iterator iter;
  for(iter=tankMap_->begin(); iter!=tankMap_->end(); ++iter) iter->second->EndEvent(hitHC, cherHitCollection);
}


void I3IceTopResponseService::Print()
{
  I3TankResponseMap::const_iterator iter;
  for(iter=tankMap_->begin(); iter!=tankMap_->end(); ++iter)
  {
    cout << iter->first
	 << ": x=" << iter->second->GetX()/I3Units::m
	 << " m, y=" << iter->second->GetY()/I3Units::m
	 << " m, z=" << iter->second->GetZ()/I3Units::m
	 << " m" << endl; 
  }
}


#ifdef I3_USE_ROOT
void I3IceTopResponseService::FillHistogram()
{
  TH2F* hist = new TH2F("hist","hist", xbins_, xmin_, xmax_, ybins_, ymin_, ymax_);
  for(int i=1; i<=hist->GetNbinsX(); i++)
  {
      double x = hist->GetXaxis()->GetBinCenter(i);
      for(int k=1; k<=hist->GetNbinsY(); k++)
      {
	double y = hist->GetYaxis()->GetBinCenter(k);
	I3TankResponseMapPtr respMap = GetTankResponseMap(x,y);
	
	if(respMap) hist->Fill(x,y, respMap->size());
      }
  }
  
  TFile* rootfile = new TFile("grid_test.root","RECREATE");
  hist->Write();
  delete rootfile;
}
#endif
