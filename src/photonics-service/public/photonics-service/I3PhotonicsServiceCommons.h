/**
 *@file I3PhotonicsServiceCommons.h
 *@brief static methods that are common to several classes
 *@author Eike Middell
 *(c) the IceCube Collaboration
 *
 */

#ifndef I3PHOTONICSSERVICECOMMONS_H
#define I3PHOTONICSSERVICECOMMONS_H

#include "icetray/IcetrayFwd.h"
#include "photonics-service/I3PhotonicsService.h"


namespace I3PhotonicsCommons {
    /* Common constants */
    static const double nPhase = 1.3195; 
    static const double nGroup = 1.35634;
    static const double cVacuum = 0.299792458;

   /**
    * @brief Light yield parametrization from ChW. Copied from photonics code.
    */
    double scaleLightYield(const PhotonicsSource &s, double rawYield,
        double energy = -1, unsigned derivative = 0);

    /*
     * @brief Fill a Jacobian matrix for converting between the Photonics coordinates
     * corresponding to an OM position and the coordinates of the source.
     */
    void fillJacobian(const double& xOM, const double& yOM, const double& zOM,
        const double& r, const double& rho, const double& cosAzi, const double& l, 
        const PhotonicsSource &source, double jac[6][6],
        const geo_type geometry = CYLINDRICAL, const parity_type parity = EVEN,
        const double n_group = nGroup);

    void fillHessian(const double& xOM, const double& yOM, const double& zOM,
        const double& r, const double& rho, const double& cosAzi, const double& l, 
        const PhotonicsSource &source, double jac[6][6], double hess[6][6][6],
        const geo_type geometry = CYLINDRICAL, const parity_type parity = EVEN,
        const double n_group = nGroup);

}

#endif
