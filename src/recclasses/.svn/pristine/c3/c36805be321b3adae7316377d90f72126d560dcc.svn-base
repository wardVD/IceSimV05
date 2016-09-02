/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#include "I3TensorOfInertiaFitParamsConverter.h"

I3TableRowDescriptionPtr I3TensorOfInertiaFitParamsConverter::CreateDescription(const I3TensorOfInertiaFitParams& params) {
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());
    desc->AddField<double>("evalratio", "", "smallest eigenvalue / sum of all eigenvalues");
    desc->AddField<double>("mineval",   "", "smallest eigenvalue");
    desc->AddField<double>("eval2",     "", "second eigenvalue");
    desc->AddField<double>("eval3",     "", "third eigenvalue");
    return desc;
}
    
size_t I3TensorOfInertiaFitParamsConverter::FillRows(const I3TensorOfInertiaFitParams& params,
                                     I3TableRowPtr rows) {
    rows->Set<double>("evalratio", params.evalratio);
    rows->Set<double>("mineval", params.mineval);
    rows->Set<double>("eval2", params.eval2);
    rows->Set<double>("eval3", params.eval3);

    return 1;
}
