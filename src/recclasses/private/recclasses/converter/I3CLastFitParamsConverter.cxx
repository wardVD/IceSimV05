/**
 * copyright  (C) 2011
 * The Icecube Collaboration
 *
 * $Id: I3CLastFitParamsConverter.cxx 2011-11-25 $
 *
 * @version $Revision: $
 * @date $ $
 * @author Lars Mohrmann <lars.mohrmann@desy.de> $LastChangedBy: $
 */

#include "I3CLastFitParamsConverter.h"

I3TableRowDescriptionPtr
I3CLastFitParamsConverter::CreateDescription(const I3CLastFitParams &params) {
  I3TableRowDescriptionPtr desc(new I3TableRowDescription());

  desc->AddField<double>("eval2", "number",
             "Eigenvalue of the tensor of inertia");
  desc->AddField<double>("eval3", "number",
             "Eigenvalue of the tensor of inertia");
  desc->AddField<double>("mineval", "number",
             "Smallest eigenvalue of the tensor of inertia");
  desc->AddField<double>("evalratio", "number",
             "Ratio of eigenvalues");
  return desc;
}

size_t I3CLastFitParamsConverter::FillRows(const I3CLastFitParams &params,
                      I3TableRowPtr rows) {
  
  rows->Set<double>("eval2", params.eval2);
  rows->Set<double>("eval3", params.eval3);
  rows->Set<double>("mineval", params.mineval);
  rows->Set<double>("evalratio", params.evalratio);
  
  return 1;
}
