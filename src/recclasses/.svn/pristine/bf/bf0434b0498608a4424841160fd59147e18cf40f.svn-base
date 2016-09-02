/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Fabian Kislat <fabian.kislat@desy.de>, last changed by $LastChangedBy$
 */

#include "I3TopLateralFitParamsConverter.h"

I3TableRowDescriptionPtr I3TopLateralFitParamsConverter::CreateDescription(const I3TopLateralFitParams& params)
{
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());

    desc->AddField<double> ("xc_err", "m", "Uncertainty of the core X position");
    desc->AddField<double> ("yc_err", "m", "Uncertainty of the core Y position");
    desc->AddField<double> ("s125", "VEM", "Pulse height in 125m distance from the core (Fit parameter: log10(S125))");
    desc->AddField<double> ("log10_s125_err", "1", "Fit uncertainty of log10(S125/VEM)");
    desc->AddField<double> ("s50", "VEM", "Pulse height in 50m distance from the core");
    desc->AddField<double> ("s70", "VEM", "Pulse height in 70m distance from the core");
    desc->AddField<double> ("s80", "VEM", "Pulse height in 80m distance from the core");
    desc->AddField<double> ("s100", "VEM", "Pulse height in 100m distance from the core");
    desc->AddField<double> ("s150", "VEM", "Pulse height in 150m distance from the core");
    desc->AddField<double> ("s180", "VEM", "Pulse height in 180m distance from the core");
    desc->AddField<double> ("s250", "VEM", "Pulse height in 250m distance from the core");
    desc->AddField<double> ("s500", "VEM", "Pulse height in 500m distance from the core");
    desc->AddField<double> ("e_proton", "GeV", "Energy estimation under proton assumption");
    desc->AddField<double> ("e_iron", "GeV", "Energy estimation under iron assumption");
    desc->AddField<double> ("s_int", "VEM", "The integral over the lateral distribution function");
    desc->AddField<double> ("s_int_err", "VEM", "Uncertainty of the integral over the lateral distribution function");
    desc->AddField<double> ("beta", "", "The slope parameter of the lateral fit");
    desc->AddField<double> ("beta_err", "", "Uncertainty of Beta");
    desc->AddField<double> ("age", "", "The age parameter of the shower. In case of DLP function,"
                                       "this is converted as shown in the SAF report (s. Internal"
      	                               "Report Repository).");
    desc->AddField<double> ("dir_err", "", "Error of direction (from combined fit)");
    desc->AddField<double> ("llh", "", "The Log-Likelihood of the fit, divided by the degrees of freedom");
    desc->AddField<double> ("llh_silent", "", "The no-hit part of the likelihood");
    desc->AddField<double> ("chi2", "", "The Chi square, divided by the degrees of freedom");
    desc->AddField<double> ("chi2_time", "", "The Chi square, divided by the degrees of freedom, for the timing fit");
    desc->AddField<int32_t>("ndf", "", "The number of degrees of freedom (after all cuts)");
    desc->AddField<double> ("rlogl", "", "The reduced log-likelihood");
    desc->AddField<int32_t>("nmini", "", "The number of minimization steps required to converge");
    desc->AddField<double> ("sumVEM", "VEM", "Sum of all VEM in all available Tanks");
    desc->AddField<double> ("rmin", "m", "Core radius of the closest used pulse");
    desc->AddField<double> ("rmax", "m", "Core radius of most distant used pulse");
    desc->AddField<int32_t>("loudest_station", "", "Loudest Station in this event");
    desc->AddField<double> ("corr_xc_yc", "", "Correlation between core X and Y co-ordinates");
    desc->AddField<double> ("corr_xc_log10_s125", "", "Correlation between core X co-ordinate and log10(S125)");
    desc->AddField<double> ("corr_yc_log10_s125", "", "Correlation between core Y co-ordinate and log10(S125)");
    desc->AddField<double> ("corr_xc_beta", "", "Correlation between core X co-ordinate and Beta");
    desc->AddField<double> ("corr_yc_beta", "", "Correlation between core Y co-ordinate and Beta");
    desc->AddField<double> ("corr_log10_s125_beta", "", "Correlation between log10(S125) and Beta");
    desc->AddField<double> ("snowF","","Snow attenuation factor used for snow correction on signal, also called Snow Lambda"); 
    desc->AddField<double> ("snowF_err","","Error on Snow attenuation factor used for snow correction on signal, when left free in the fitting procedure");
    return desc;
}
    
size_t I3TopLateralFitParamsConverter::FillRows(const I3TopLateralFitParams& params,
						I3TableRowPtr rows)
{
    rows->Set<double> ("xc_err", params.Xc_err);
    rows->Set<double> ("yc_err", params.Yc_err);
    rows->Set<double> ("s125", params.S125);
    rows->Set<double> ("log10_s125_err", params.Log10_S125_err);
    rows->Set<double> ("s50", params.S50);
    rows->Set<double> ("s70", params.S70);
    rows->Set<double> ("s80", params.S80);
    rows->Set<double> ("s100", params.S100);
    rows->Set<double> ("s150", params.S150);
    rows->Set<double> ("s180", params.S180);
    rows->Set<double> ("s250", params.S250);
    rows->Set<double> ("s500", params.S500);
    rows->Set<double> ("e_proton", params.E_proton);
    rows->Set<double> ("e_iron", params.E_iron);
    rows->Set<double> ("s_int", params.Sint);
    rows->Set<double> ("s_int_err", params.Sint_err);
    rows->Set<double> ("beta", params.Beta);
    rows->Set<double> ("beta_err", params.Beta_err);
    rows->Set<double> ("age", params.Age);
    rows->Set<double> ("dir_err", params.Dir_err);
    rows->Set<double> ("llh", params.Llh);
    rows->Set<double> ("llh_silent", params.Llh_silent);
    rows->Set<double> ("chi2", params.Chi2);
    rows->Set<double> ("chi2_time", params.Chi2_time);
    rows->Set<int32_t>("ndf", params.Ndf);
    rows->Set<double> ("rlogl", params.RLogL);
    rows->Set<int32_t>("nmini", params.NMini);
    rows->Set<double> ("sumVEM", params.SumVEM);
    rows->Set<double> ("rmin", params.Rmin);
    rows->Set<double> ("rmax", params.Rmax);
    rows->Set<int32_t>("loudest_station", params.LoudestStation);
    rows->Set<double> ("corr_xc_yc", params.Corr_Xc_Yc);
    rows->Set<double> ("corr_xc_log10_s125", params.Corr_Xc_Log10_S125);
    rows->Set<double> ("corr_yc_log10_s125", params.Corr_Yc_Log10_S125);
    rows->Set<double> ("corr_xc_beta", params.Corr_Xc_Beta);
    rows->Set<double> ("corr_yc_beta", params.Corr_Yc_Beta);
    rows->Set<double> ("corr_log10_s125_beta", params.Corr_Log10_S125_Beta);
    rows->Set<double> ("snowF", params.SnowFactor);
    rows->Set<double> ("snowF_err", params.SnowFactor_err);

    return 1;
}
