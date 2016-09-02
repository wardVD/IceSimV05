#include <tableio/converter/pybindings.h>

#include "recclasses/CramerRaoParams.h"
#include "../recclasses/converter/CramerRaoParamsConverter.h"

namespace bp = boost::python ;

void register_CramerRaoParams()
{
  {
    bp::scope in_cramer_rao_fitparams = bp::class_<CramerRaoParams, 
      bp::bases<I3FrameObject>, boost::shared_ptr<CramerRaoParams> >("CramerRaoParams")
      .def_readwrite("cramer_rao_theta"     ,&CramerRaoParams::cramer_rao_theta)
      .def_readwrite("cramer_rao_phi"       ,&CramerRaoParams::cramer_rao_phi)
      .def_readwrite("variance_theta"       ,&CramerRaoParams::variance_theta)
      .def_readwrite("variance_phi"         ,&CramerRaoParams::variance_phi)
      .def_readwrite("variance_x"           ,&CramerRaoParams::variance_x)
      .def_readwrite("variance_y"           ,&CramerRaoParams::variance_y)
      .def_readwrite("covariance_theta_phi" ,&CramerRaoParams::covariance_theta_phi)
      .def_readwrite("covariance_theta_x"   ,&CramerRaoParams::covariance_theta_x)
      .def_readwrite("covariance_theta_y"   ,&CramerRaoParams::covariance_theta_y)
      .def_readwrite("covariance_phi_x"     ,&CramerRaoParams::covariance_phi_x)
      .def_readwrite("covariance_phi_y"     ,&CramerRaoParams::covariance_phi_y)
      .def_readwrite("covariance_x_y"       ,&CramerRaoParams::covariance_x_y)
      .def_readwrite("cramer_rao_theta_corr",&CramerRaoParams::cramer_rao_theta_corr)
      .def_readwrite("cramer_rao_phi_corr"  ,&CramerRaoParams::cramer_rao_phi_corr)
      .def_readwrite("llh_est"              ,&CramerRaoParams::llh_est)
      .def_readwrite("rllh_est"             ,&CramerRaoParams::rllh_est)
      .def_readwrite("status"               ,&CramerRaoParams::status)

      ;

    bp::enum_<CramerRaoParams::CramerRaoStatus>("CramerRaoStatus")
      .value("NotSet",           CramerRaoParams::NotSet          )
      .value("OK",               CramerRaoParams::OK              )
      .value("MissingInput",     CramerRaoParams::MissingInput    )
      .value("SingularMatrix",   CramerRaoParams::SingularMatrix  )
      .value("InsufficientHits", CramerRaoParams::InsufficientHits)
      .value("OneStringEvent",   CramerRaoParams::OneStringEvent  )
      .value("OtherProblems",    CramerRaoParams::OtherProblems   )
      ;

  }
  
  register_pointer_conversions<CramerRaoParams>();
  
  I3CONVERTER_NAMESPACE(recclasses);
  I3CONVERTER_EXPORT_DEFAULT(CramerRaoParamsConverter, "Tabulator of CramerRaoParams");
}

