/**
 *@file I3PhotonicsServiceCommons.cxx
 *@brief implementations of I3PhotonicsServiceCommons.h
 *@author Eike Middell
 *(c) the IceCube Collaboration
 *
 */

#include "photonics-service/I3PhotonicsServiceCommons.h"
#include "photonics-service/I3MPConstants.h"

#include <string>
#include <cmath>
#include <float.h>
#include "icetray/I3Units.h"
#include "dataclasses/I3Constants.h"

double
I3PhotonicsCommons::scaleLightYield(const PhotonicsSource &s,
    double rawYield, double energy, unsigned derivative)
{
	if (energy < 0)
		energy = s.E;

	if ( energy < 0.01 ) { 
		log_trace( "Energy %f<10 MeV, using 10 MeV (lowest possible) "
		    "for light factor calculation", energy);
		energy = 0.01;
	}

	/** 
	 * Cerenkov photons per meter in 300 - 600 nm range 
	 * assuming beta = 1 and a wavelength dependent index of refraction
	 */
	double lightFactor(32582.0);
	double beta_n, mp_amp;

	/* Compute effective track length */
	switch (s.type) {
		case 1: /* EM Shower: see AMANDA-IR/20020803 Table 2.1 */
			lightFactor *= 5.21;
			switch (derivative) {
				case 0:
					lightFactor *= energy;
					break;
				case 1: /* first derivative */
					break;
				default: /* higher derivatives */
					lightFactor = 0;
			}
			break;
		case 2: /* Hadronic Shower */
			lightFactor *= 0.860*4.076;
			switch (derivative) {
				case 0:
					lightFactor *= energy;
					break;
				case 1: /* first derivative */
					break;
				default: /* higher derivatives */
					lightFactor = 0;
			}
			break;
		case 13: /* Sub-cherenkov monopole */
			lightFactor = 0;
			break;
		case 9: /* Monopole: like a muon, but more */
		case 10:
		case 11:
		case 12:
			beta_n = I3Constants::n_ice_phase * s.speed;
			mp_amp = 1.-(1./pow(beta_n,2));
			if (mp_amp < 0) {
				lightFactor = 0;
				break;
			}
			lightFactor *= I3MPConst::MP_FACTOR * mp_amp * s.length;
			// Fall through
		default: /* Muon */
			switch (derivative) {
				case 0:
					lightFactor *= (1.172 + 0.0324*log(energy));
					break;
				case 1: /* first derivative */
					lightFactor *= 0.0324/energy;
					break;
				case 2:
					lightFactor *= -0.0324/(energy*energy);
					break;
				default: /* higher derivatives */
					lightFactor = 0;
					log_error("scaleLightYield() doesn't"
					    "know about derivates of order "
					    "greater than 2 (you asked for "
					    "order %ud)", derivative);
			}
			break;
	}

	log_trace("Scaling amplitude %e amplitude with light factor %lf",
	    rawYield, lightFactor);
	return rawYield*lightFactor;
}

void
I3PhotonicsCommons::fillJacobian(const double& xOM, const double& yOM, const double& zOM,
    const double& r, const double& rho, const double& cosAzi, const double& l, 
    const PhotonicsSource &source, double jac[6][6], const geo_type geometry,
    const parity_type parity, const double n_group)
{	
	if (geometry == CUBIC)
		log_fatal("Ironically enough, transformations from"
		    "cubic geometries are not supported!");
	
	const double dx = xOM - source.x;
	const double dy = yOM - source.y;
	const double dz = zOM - source.z;
		
	/* 
	 * columns: r/rho, azi, costheta/l, t_d, theta_s, z 
	 * rows:    x,     y,   z,          t_s, zenith,  azimuth
	 */
	
	/* The projective coordinates are useful even if we're
	 * working in spherical coordinates.
	 */
	double drho_dx(1), drho_dy(1), drho_dz(1), drho_dtheta(0), drho_dphi(0);
	
	const double dl_dtheta = -dx*source.costheta*source.cosphi
	    - dy*source.costheta*source.sinphi + dz*source.sintheta;
	const double dl_dphi = -dx*source.sintheta*source.sinphi
	    + dy*source.sintheta*source.cosphi;
	
	if (rho > 0) {
		drho_dx = (-dx + l*source.dirx)/rho;
		drho_dy = (-dy + l*source.diry)/rho;
		drho_dz = (-dz + l*source.dirz)/rho;
		
		drho_dtheta = -l*dl_dtheta/rho;
		drho_dphi = -l*dl_dphi/rho;
	}

	memset(jac, 0, 6*6*sizeof(double));
	
	/* r / rho */
	if (geometry == SPHERICAL) {
		if (r > 0) {
			jac[0][0] = -dx/r;
			jac[1][0] = -dy/r;
			jac[2][0] = -dz/r;
		} else {
			jac[0][0] = jac[1][0] = jac[2][0] = 1.0;
		}
	} else if (geometry == CYLINDRICAL) {
		/* rho == 0 case already handled above */
		jac[0][0] = drho_dx;
		jac[1][0] = drho_dy;
		jac[2][0] = drho_dz;
		
		jac[4][0] = drho_dtheta;
		jac[5][0] = drho_dphi;	
	}
	
	/* azi */
	/* XXX FIXME: the fallback calculation is wrong in a very subtle way! */
	if (true || (rho > 0 && 1-fabs(cosAzi) > 1e-2)) {
		/* \vec{dx} . \vec{p} / rho^2 */
		const double xpr = (dx*source.perpx + dy*source.perpy
		    + dz*source.perpz)/(rho*rho);
		const double acos_scale = ((parity == ODD) ? 1 : -1) / 
		    ((sqrt(1 - cosAzi*cosAzi)*I3Units::degree));
		
		jac[0][1] = acos_scale*(-source.perpx/rho - xpr*drho_dx);
		jac[1][1] = acos_scale*(-source.perpy/rho - xpr*drho_dy);
		jac[2][1] = acos_scale*(-source.perpz/rho - xpr*drho_dz);
		
		jac[4][1] = acos_scale*(-l/rho - xpr*drho_dtheta);
		jac[5][1] = acos_scale*((dx*source.costheta*source.sinphi
		    - dy*source.costheta*source.cosphi)/rho - xpr*drho_dphi);
	} else if (rho > 0) {
		/*
		 * A slightly more stable version near photonics azimuth 0 and 180.
		 */
		/* \vec{dx} . \vec{q} / rho^2 */
		const double xqr = (dx*source.sinphi - dy*source.cosphi)/(rho*rho);
		/* The direction of \vec{q} */
		double sign = xqr > 0 ? -1 : 1;
		if (parity == ODD)
			sign *= -1;
		/*
		 * NB: the lack of abs(cosAzi) is intentional, as the sign
		 * effectively reflects the result of arcsin around pi/2
		 */
		const double asin_scale = sign/(cosAzi*I3Units::degree);
		
		jac[0][1] = asin_scale*(-source.sinphi/rho - xqr*drho_dx);
		jac[1][1] = asin_scale*(source.cosphi/rho - xqr*drho_dy);
		jac[2][1] = -asin_scale*xqr*drho_dz;
		
		jac[4][1] = -asin_scale*xqr*drho_dtheta;
		jac[5][1] = asin_scale*((dx*source.cosphi
		    + dy*source.sinphi)/rho - xqr*drho_dphi);
	}
	
	/* costheta / l */
	if (geometry == SPHERICAL && r > 0) {
		const double lrrr = l/(r*r*r);
		
		jac[0][2] = -source.dirx/r + lrrr*dx;
		jac[1][2] = -source.diry/r + lrrr*dy;
		jac[2][2] = -source.dirz/r + lrrr*dz;
		
		jac[4][2] = dl_dtheta/r;
		jac[5][2] = dl_dphi/r;
	} else if (geometry == CYLINDRICAL) {
		jac[0][2] = -source.dirx;
		jac[1][2] = -source.diry;
		jac[2][2] = -source.dirz;
		
		jac[4][2] = dl_dtheta;
		jac[5][2] = dl_dphi;
	}
	
	/* delay time */
	/* FIXME: add an option to change the time reference (for level2) */
	if (r > 0) {
		const double tscale = n_group / 
		    (r*I3PhotonicsCommons::cVacuum);
		
		jac[0][3] = dx*tscale;
		jac[1][3] = dy*tscale;
		jac[2][3] = dz*tscale;
		
	}
	jac[3][3] = -1.0;
	
	/* source zenith */
	jac[4][4] = I3Units::radian/I3Units::degree;
	if (parity == ODD)
		jac[4][4] *= -1.0;
	
	/* source z */
	jac[2][5] = 1;
}

void
I3PhotonicsCommons::fillHessian(const double& xOM, const double& yOM, const double& zOM,
    const double& r, const double& rho, const double& cosAzi, const double& l, 
    const PhotonicsSource &source, double jac[6][6], double hess[6][6][6],
    const geo_type geometry, const parity_type parity, const double n_group)
{	
	if (geometry == CUBIC)
		log_fatal("Ironically enough, transformations from"
		    "cubic geometries are not supported!");
	
	const double dx = xOM - source.x;
	const double dy = yOM - source.y;
	const double dz = zOM - source.z;
	
	/* some handy derivatives of the sourceward vector */
	const double dnx_dtheta = -source.costheta*source.cosphi;
	const double dny_dtheta = -source.costheta*source.sinphi;
	const double dnz_dtheta = source.sintheta;
	
	const double dnx_dphi = -source.sintheta*source.sinphi;
	const double dny_dphi = source.sintheta*source.cosphi;
	/* dnz_dphi == 0 */
 
	const double dnx_dtheta_dtheta = -source.sintheta*source.cosphi;
	const double dny_dtheta_dtheta = -source.sintheta*source.sinphi;
	const double dnz_dtheta_dtheta = -source.costheta;
 
	const double dnx_dphi_dphi = -source.sintheta*source.cosphi;
	const double dny_dphi_dphi = -source.sintheta*source.sinphi;
	/* dnz_dphi_dphi == 0 */
 
	const double dnx_dtheta_dphi = source.costheta*source.sinphi;
	const double dny_dtheta_dphi = -source.costheta*source.cosphi;
	/* dnz_dtheta_dphi == 0 */
	
	const double dl_dtheta = dx*dnx_dtheta + dy*dny_dtheta + dz*dnz_dtheta;
	const double dl_dphi = dx*dnx_dphi + dy*dny_dphi;
	
	const double dl_dtheta_dtheta = dx*dnx_dtheta_dtheta + dy*dny_dtheta_dtheta + dz*dnz_dtheta_dtheta;
	const double dl_dtheta_dphi = dx*dnx_dtheta_dphi + dy*dny_dtheta_dphi;
	const double dl_dphi_dphi = dx*dnx_dphi_dphi + dy*dny_dphi_dphi;
 
	const double drho_dx = (-dx + l*source.dirx)/rho;
	const double drho_dy = (-dy + l*source.diry)/rho;
	const double drho_dz = (-dz + l*source.dirz)/rho;
 
	const double drho_dtheta = -l*dl_dtheta/rho;
	const double drho_dphi = -l*dl_dphi/rho;

	const double drho_dx_dx = (1 - source.dirx*source.dirx - drho_dx*drho_dx)/rho; /* drho / dx dx */
	const double drho_dy_dy = (1 - source.diry*source.diry - drho_dy*drho_dy)/rho; /* drho / dy dy */
	const double drho_dz_dz = (1 - source.dirz*source.dirz - drho_dz*drho_dz)/rho; /* drho / dz dz */

	const double drho_dx_dy = (-source.dirx*source.diry - drho_dx*drho_dy)/rho;
	const double drho_dy_dz = (-source.diry*source.dirz - drho_dy*drho_dz)/rho;
	const double drho_dx_dz = (-source.dirx*source.dirz - drho_dx*drho_dz)/rho;

	const double drho_dx_dtheta = -(l*source.dirx - dx)*drho_dtheta/(rho*rho) + (source.dirx*dl_dtheta + l*dnx_dtheta)/rho;
	const double drho_dy_dtheta = -(l*source.diry - dy)*drho_dtheta/(rho*rho) + (source.diry*dl_dtheta + l*dny_dtheta)/rho;
	const double drho_dz_dtheta = -(l*source.dirz - dz)*drho_dtheta/(rho*rho) + (source.dirz*dl_dtheta + l*dnz_dtheta)/rho;

	const double drho_dx_dphi = -(l*source.dirx - dx)*drho_dphi/(rho*rho) + (source.dirx*dl_dphi + l*dnx_dphi)/rho;
	const double drho_dy_dphi = -(l*source.diry - dy)*drho_dphi/(rho*rho) + (source.diry*dl_dphi + l*dny_dphi)/rho;
	const double drho_dz_dphi = -(l*source.dirz - dz)*drho_dphi/(rho*rho) + (source.dirz*dl_dphi)/rho;

	const double drho_dtheta_dtheta = l*dl_dtheta*drho_dtheta/(rho*rho) - (dl_dtheta*dl_dtheta + l*dl_dtheta_dtheta)/rho;
	const double drho_dtheta_dphi = l*dl_dtheta*drho_dphi/(rho*rho) - (dl_dtheta*dl_dphi + l*dl_dtheta_dphi)/rho;
	const double drho_dphi_dphi = l*dl_dphi*drho_dphi/(rho*rho) - (dl_dphi*dl_dphi + l*dl_dphi_dphi)/rho;

	/* \vec{x_om - x_s} \cdot \vec{p} */
	const double xp = dx*source.perpx + dy*source.perpy + dz*source.perpz;
	
	fillJacobian(xOM, yOM, zOM, r, rho, cosAzi, l, source, jac, geometry, parity, n_group);
	
	memset(hess, 0, 6*6*6*sizeof(double));
	
	if (geometry == SPHERICAL) { /* r */
		const double rrr = r*r*r;
		
		hess[0][0][0] = 1/r - dx*dx/rrr; /* dr / dx dx */
		hess[1][1][0] = 1/r - dy*dy/rrr; /* dr / dy dy */
		hess[2][2][0] = 1/r - dz*dz/rrr; /* dr / dz dz */
	
		hess[0][1][0] = -dx*dy/rrr; /* dr / dx dy */
		hess[1][2][0] = -dy*dz/rrr; /* dr / dy dz */
		hess[0][2][0] = -dx*dz/rrr; /* dr / dx dz */
		
		/* NB: remaining dr / da db terms are zero. */
	} else { /* rho */
		hess[0][0][0] = drho_dx_dx;
		hess[1][1][0] = drho_dy_dy;
		hess[2][2][0] = drho_dz_dz;
	                    
		hess[0][1][0] = drho_dx_dy;
		hess[1][2][0] = drho_dy_dz;
		hess[0][2][0] = drho_dx_dz;
	
		/* and so on in that fashion until all are eaten... */

		hess[0][4][0] = drho_dx_dtheta;
		hess[1][4][0] = drho_dy_dtheta;
		hess[2][4][0] = drho_dz_dtheta;
		
		hess[0][5][0] = drho_dx_dphi;
		hess[1][5][0] = drho_dy_dphi;
		hess[2][5][0] = drho_dz_dphi;
		
		hess[4][4][0] = drho_dtheta_dtheta;
		hess[4][5][0] = drho_dtheta_dphi;
		hess[5][5][0] = drho_dphi_dphi;
	}
	
	{ /* phi */
		double a;
		const double dphi_dx = jac[0][1];
		const double dphi_dy = jac[1][1];
		const double dphi_dz = jac[2][1];
		
		const double dphi_dtheta = jac[4][1];
		const double dphi_dphi = jac[5][1];
		
		const double sign = (parity == EVEN) ? 1 : -1;

		/* 
		 * NB: this will blow the hell up at phi = 0,180
		 * FIXME: add numerically stable verion for cosAzi =~ +-1
		 */

		const double sin2Azi = 1 - cosAzi*cosAzi;
		const double sr2d = sqrt(sin2Azi)*rho*rho*(I3Units::degree/I3Units::radian);

		const double dpx_dtheta = -source.sintheta*source.cosphi;
		const double dpy_dtheta = -source.sintheta*source.sinphi;
		const double dpz_dtheta = -source.costheta;

		const double dpx_dphi = source.costheta*source.sinphi;
		const double dpy_dphi = -source.costheta*source.cosphi;
		/* dpz_dphi == 0 */

		const double dxp_dtheta = dx*dpx_dtheta + dy*dpy_dtheta + dz*dpz_dtheta;
		const double dxp_dphi = dx*dpx_dphi + dy*dpy_dphi;
		const double dxp_dphi_dphi = -dx*source.perpx - dy*source.perpy;

		a = -(cosAzi/sin2Azi)*(source.perpx/rho + xp*drho_dx/(rho*rho));

		/* dphi / dx dx */
		hess[0][0][1] = sign*(dphi_dx*a +
		    (xp*drho_dx_dx - 2*xp*drho_dx*drho_dx/rho - 2*source.perpx*drho_dx)/sr2d);
		/* dphi / dx dy */
		hess[0][1][1] = sign*(dphi_dy*a + \
		    (xp*drho_dx_dy - 2*xp*drho_dx*drho_dy/rho - source.perpx*drho_dy - source.perpy*drho_dx)/sr2d);
		/* dphi / dx dz */
		hess[0][2][1] = sign*(dphi_dz*a + \
		    (xp*drho_dx_dz - 2*xp*drho_dx*drho_dz/rho - source.perpx*drho_dz - source.perpz*drho_dx)/sr2d);
		/* dphi / dx dtheta */
		hess[0][4][1] = sign*(dphi_dtheta*a + \
		    (rho*dpx_dtheta - source.perpx*drho_dtheta - 2*xp*drho_dx*drho_dtheta/rho + \
		    drho_dx*dxp_dtheta + xp*drho_dx_dtheta)/sr2d);
		/* dphi / dx dphi */
		hess[0][5][1] = sign*(dphi_dphi*a + \
		    (rho*dpx_dphi - source.perpx*drho_dphi - 2*xp*drho_dx*drho_dphi/rho + \
		    drho_dx*dxp_dphi + xp*drho_dx_dphi)/sr2d);

		a = -(cosAzi/sin2Azi)*(source.perpy/rho + xp*drho_dy/(rho*rho));

		/* dphi / dy dy */
		hess[1][1][1] = sign*(dphi_dy*a + \
		    (xp*drho_dy_dy - 2*xp*drho_dy*drho_dy/rho - 2*source.perpy*drho_dy)/sr2d);
		/* dphi / dy dz */
		hess[1][2][1] = sign*(dphi_dz*a + \
		    (xp*drho_dy_dz - 2*xp*drho_dy*drho_dz/rho - source.perpy*drho_dz - source.perpz*drho_dy)/sr2d);
		/* dphi / dy dtheta */
		hess[1][4][1] = sign*(dphi_dtheta*a + \
		    (rho*dpy_dtheta - source.perpy*drho_dtheta - 2*xp*drho_dy*drho_dtheta/rho + \
		    drho_dy*dxp_dtheta + xp*drho_dy_dtheta)/sr2d);
		/* dphi / dy dphi */
		hess[1][5][1] = sign*(dphi_dphi*a + \
		    (rho*dpy_dphi - source.perpy*drho_dphi - 2*xp*drho_dy*drho_dphi/rho + \
		    drho_dy*dxp_dphi + xp*drho_dy_dphi)/sr2d);

		a = -(cosAzi/sin2Azi)*(source.perpz/rho + xp*drho_dz/(rho*rho));

		/* dphi / dz dz */
		hess[2][2][1] = sign*(dphi_dz*a + \
		    (xp*drho_dz_dz - 2*xp*drho_dz*drho_dz/rho - 2*source.perpz*drho_dz)/sr2d);
		/* dphi / dz dtheta */
		hess[2][4][1] = sign*(dphi_dtheta*a + \
		    (rho*dpz_dtheta - source.perpz*drho_dtheta - 2*xp*drho_dz*drho_dtheta/rho + \
		    drho_dz*dxp_dtheta + xp*drho_dz_dtheta)/sr2d);
		/* dphi / dz dphi */
		hess[2][5][1] = sign*(dphi_dphi*a + \
		    (-source.perpz*drho_dphi - 2*xp*drho_dz*drho_dphi/rho + \
		    drho_dz*dxp_dphi + xp*drho_dz_dphi)/sr2d);

		a = -(cosAzi/sin2Azi)*(l/rho + xp*drho_dtheta/(rho*rho));

		/* dphi / dtheta dtheta */
		hess[4][4][1] = sign*(dphi_dtheta*a + \
		    (rho*dl_dtheta - 2*xp*drho_dtheta*drho_dtheta/rho -\
		    l*drho_dtheta + drho_dtheta*dxp_dtheta + xp*drho_dtheta_dtheta)/sr2d);
		/* dphi / dtheta phi */
		hess[4][5][1] = sign*(dphi_dphi*a + \
		    (rho*dl_dphi - 2*xp*drho_dtheta*drho_dphi/rho -\
		    l*drho_dphi + drho_dtheta*dxp_dphi + xp*drho_dtheta_dphi)/sr2d);

		a = -(cosAzi/sin2Azi)*(-dxp_dphi/rho + xp*drho_dphi/(rho*rho));
		
		/* dphi / dphi phi */
		hess[5][5][1] = sign*(dphi_dphi*a + \
		    (dxp_dphi*drho_dphi - 2*xp*drho_dphi*drho_dphi/rho - \
		    rho*dxp_dphi_dphi + dxp_dphi*drho_dphi + xp*drho_dphi_dphi)/sr2d);
	}	
	
	if (geometry == SPHERICAL) { /* cos(polar angle) */
		const double rrr = r*r*r;
		double a;
		
		a = (source.dirx - 3*l*dx/(r*r))/(r*r);
		
		hess[0][0][2] = -dx*a/r - (source.dirx*dx + l)/rrr; /* dc / dx dx */
		hess[0][1][2] = -dy*a/r - (source.diry*dx)/rrr;     /* dc / dx dy */
		hess[0][2][2] = -dz*a/r - (source.dirz*dx)/rrr ;    /* dc / dx dz */
		
		a = (source.diry - 3*l*dy/(r*r))/(r*r);
		
		hess[1][1][2] = -dy*a/r - (source.diry*dy + l)/rrr; /* dc / dy dy */
		hess[1][2][2] = -dz*a/r - (source.dirz*dy)/rrr;     /* dc / dy dz */
		
		a = (source.dirz - 3*l*dz/(r*r))/(r*r);
		
		hess[2][2][2] = -dz*a/r - (source.dirz*dz + l)/rrr; /* dc / dz dz */
		
		hess[0][4][2] = -dnx_dtheta/r + dx*dl_dtheta/rrr;   /* dc / dx dtheta */
		hess[0][5][2] = -dnx_dphi/r   + dx*dl_dphi  /rrr;   /* dc / dx dphi */
		hess[1][4][2] = -dny_dtheta/r + dy*dl_dtheta/rrr;   /* dc / dy dtheta */
		hess[1][5][2] = -dny_dphi/r   + dy*dl_dphi  /rrr;   /* dc / dy dphi */
		hess[2][4][2] = -dnz_dtheta/r + dz*dl_dtheta/rrr;   /* dc / dz dtheta */
		hess[2][5][2] = dz*dl_dphi/rrr;                     /* dc / dz dphi */
		
		hess[4][4][2] = dl_dtheta_dtheta/r;                 /* dc / dtheta dtheta */
		hess[4][5][2] = dl_dtheta_dphi/r;                   /* dc / dtheta dphi */
		hess[5][5][2] = dl_dphi_dphi/r;                     /* dc / dphi dphi */
	} else { /* l */
		/* NB: l is linear in the spatial coordinates */
		hess[0][4][2] = -dnx_dtheta;
		hess[1][4][2] = -dny_dtheta;
		hess[2][4][2] = -dnz_dtheta;
		
		hess[0][5][2] = -dnx_dphi;
		hess[1][5][2] = -dny_dphi;
		/* dl / dz dphi = -dnz_dphi == 0 */
		
		hess[4][4][2] = dl_dtheta_dtheta;
		hess[4][5][2] = dl_dtheta_dphi;
		hess[5][5][2] = dl_dphi_dphi;
	}
	
	{ /* delay time */
		const double timescale = n_group / (r*cVacuum);

		hess[0][0][3] = timescale*(dx*dx/(r*r) - 1);  /* dt / dx dx */
		hess[0][1][3] = timescale*(dx*dy/(r*r));      /* dt / dx dy */
		hess[0][2][3] = timescale*(dx*dz/(r*r));      /* dt / dx dz */

		hess[1][1][3] = timescale*(dy*dy/(r*r) - 1);  /* dt / dy dy */
		hess[1][2][3] = timescale*(dy*dz/(r*r));      /* dt / dy dz */

		hess[2][2][3] = timescale*(dz*dz/(r*r) - 1);  /* dt / dz dz */
	}
	
	/* NB: remaining coords depend linearly or not at all on source coords */
}
