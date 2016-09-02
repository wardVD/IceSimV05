
#include "icetray/I3Units.h"
#include "photonics-service/I3PhotoSplineService.h"
#include "photonics-service/I3PhotonicsServiceCommons.h"
#include "photonics-service/I3PhotoSplineTable.h"
#include <cmath>   

/* Apply the chain rule to a gradient vector. */

void
convert_grad(const double matrix[6][6], const double grad[6], double out[6])
{
	int i, j;
	
	memset(out, 0, 6*sizeof(double));
	
	for (i = 0; i < 6; i++)
		for (j = 0; j < 6; j++)
			out[i] += matrix[i][j]*grad[j];
}

/* Apply the chain rule to a matrix of second derivatives:
 * 
 * {\partial^2 f \over \partial x_i x_j} =
 *     \sum_k \left[
 *       {\partial y_k \over\partial x_j}
 *       \sum_l \left(
 *           {\partial y_l \over \partial x_i} {\partial^2 f \over \partial y_l y_k}
 *       \right)
 *       + {\partial f \over \partial y_k} {\partial^2 y_k \over \partial x_i x_j}
 *    \right]
 */

void
convert_hessian(const double tensor[6][6][6], const double jacobian[6][6],
    const double hessian[6][6], const double gradient[6], double out[6][6])
{
	int i, j, k, l;
	double temp;
	
	memset(out, 0, 6*6*sizeof(double));
	
	for (i = 0; i < 6; i++) for (j = 0; j <= i; j++)
	    for (k = 0; k < 6; k++) {
		/* NB: by convention, tensor[j][i] is nonzero when j <= i */
		out[i][j] += gradient[k]*tensor[j][i][k];
		
		/* We only have the lower triangle of the raw Hessian. */
		temp = 0;
		for (l = 0; l < k; l++)
			temp += jacobian[i][l]*hessian[k][l];
		for (l = k; l < 6; l++)
			temp += jacobian[i][l]*hessian[l][k];
			
		out[i][j] += jacobian[j][k]*temp;
	}
	
}

void
insert_column(double matrix[6][6], unsigned col)
{
	unsigned i, j;
	
	/* Move the columns right */
	for (i = 0; i < 6; i++)
		for (j = 5; j > col; j--)
			matrix[i][j] = matrix[i][j-1];
	/* Move the rows down */
	for (i = 5; i > col; i--)
		for (j = 0; j < 6; j++)
			matrix[i][j] = matrix[i-1][j];
	/* Zero the new column and row */
	for (i = 0; i < 6; i++) {
		matrix[i][col] = 0;
		matrix[col][i] = 0;
	}
}

void
remove_column(double matrix[6][6], unsigned col)
{
	unsigned i, j;
	
	/* Move the columns left */
	for (i = 0; i < 6; i++)
		for (j = col; j < 5; j++)
			matrix[i][j] = matrix[i][j+1];
	/* Move the rows up */
	for (i = col; i < 5; i++)
		for (j = 0; j < 6; j++)
			matrix[i][j] = matrix[i+1][j];
	/* Zero the outer column and row */
	for (i = 0; i < 6; i++) {
		matrix[i][5] = 0;
		matrix[5][i] = 0;
	}
}

void
I3PhotoSplineService::ConvertMeanAmplitudeGradient(
    double raw_gradient[6], double gradient[6], const PhotonicsSource &source)
{
	double jacobian[6][6];
	
	I3PhotonicsCommons::fillJacobian(xOM_, yOM_, zOM_, r_, rho_, cosAzi_, l_,
	    source, jacobian, geometry_, parity_, nGroup_);
	
	raw_gradient[5] = raw_gradient[4];
	raw_gradient[4] = raw_gradient[3];
	raw_gradient[3] = 0.0; /* Amplitudes are time-independent */
	
	convert_grad(jacobian, raw_gradient, gradient);
	
	/* Move source zenith and azimuth back over */
	gradient[3] = gradient[4];
	gradient[4] = gradient[5];
}

void
I3PhotoSplineService::ScaleMeanAmplitudeGradient(double gradient[6],
    const double &rawYield, const double &meanPEs, const PhotonicsSource &source)
{
	int i;

	/* d/dx(f(E)*exp(mu)) = f(E)*exp(mu)*(dmu/dx) */
	for (i = 0; i < 5; i++)
		gradient[i] *= meanPEs;
		
	/* Get the gradient with respect to energy */
	gradient[5] = I3PhotonicsCommons::scaleLightYield(source, rawYield,
	    -1, 1);
}

bool
I3PhotoSplineService::GetMeanAmplitudeGradient(double gradient[6])
{
	int err;
	double tablecoordinates[5];
	double buffer[7];
	double *grad = &buffer[1]; /* buffer is value + 6 gradients */
	
	if (meanPEs_ < 0) {
		log_error("No light source selected! SelectSource() must be "
		    "called before GetTotalAmplitudeGradient().");
		return false;
	}
	
	log_trace("xOM_: %e yOM_: %e zOM_: %e r_: %e rho_: %e "
	    "cozAzi_: %e l_: %e geometry_: %d\n",xOM_,yOM_,zOM_,r_,rho_,cosAzi_,
	    l_, geometry_);
	
	FillTableCoordinates(tablecoordinates, false);
	
	err = amplitudeSplineTable_->EvalGradients(tablecoordinates, buffer);
	if (err)
		return false;
	
	ConvertMeanAmplitudeGradient(grad, gradient, lastSource_);
	ScaleMeanAmplitudeGradient(gradient, rawYield_, meanPEs_, lastSource_);

	return true;
}

bool
I3PhotoSplineService::GetMeanAmplitudeHessian(double gradient[6],double hessian[6][6])
{
	int i, j;
	double tablecoordinates[5];
	double jacobian[6][6];
	double hess_tensor[6][6][6];
	double grad_buffer[7];
	double *grad = &grad_buffer[1]; /* grad buffer is value + 6 derivatives */
	double hessian_buffer[6][6];

	if (meanPEs_ < 0) {
		log_error("No light source selected! SelectSource() must be "
		    "called before GetTotalAmplitudeGradient().");
		return false;
	}
	
	I3PhotonicsCommons::fillHessian(xOM_, yOM_, zOM_, r_, rho_, cosAzi_, l_,
	    lastSource_, jacobian, hess_tensor, geometry_, parity_);
	
	FillTableCoordinates(tablecoordinates, false);
	
	amplitudeSplineTable_->EvalGradients(tablecoordinates, grad_buffer);
	
	for (i = 0; i < 5; i++) {
		if (!std::isfinite(grad[i]))
			grad[i] = 0.;
	}
	
	amplitudeSplineTable_->EvalHessian(tablecoordinates, hessian_buffer);
	
	/* 
	 * Amplitudes are time-independent: scootch source zenith and depth
	 * one column to the right and add an entry for delay time before
	 * transforming the derivatives. 
	 */
	insert_column(hessian_buffer, 3);
	grad[5] = grad[4];
	grad[4] = grad[3];
	grad[3] = 0.0; 

	/* Drizzle generously with chain-rule sauce */
	convert_hessian(hess_tensor, jacobian, hessian_buffer, grad, hessian);
	convert_grad(jacobian, grad, gradient);	
	
	/* Move source zenith and azimuth back over */
	remove_column(hessian, 3);
	gradient[3] = gradient[4];
	gradient[4] = gradient[5];
	
	/* Get the gradient with respect to energy */
	gradient[5] = I3PhotonicsCommons::scaleLightYield(lastSource_,
	    rawYield_, -1, 1);

	/* 
	 * d^2/(dx_i dx_j) (f(E)*exp(mu)) = 
	 *    f(E)*exp(mu)*( (dmu/dx_i)(dmu/dx_j) + d^2mu/(dx_i dx_j) )
	 */
	for (i = 0; i < 5; i++)
		for (j = 0; j <= i; j++)
			hessian[i][j] = meanPEs_*(gradient[i]*gradient[j] + hessian[i][j]);
	
	/* d^2/(dx_i dE) (f(E)*exp(mu)) = exp(mu) (df/dE dmu/dx_i) */
	for (j = 0; j < 5; j++)
		hessian[5][j] = gradient[5]*gradient[j];
		
	/* Get the second derivative with respect to energy */
	hessian[5][5] = I3PhotonicsCommons::scaleLightYield(lastSource_,
	    rawYield_, -1, 2);
		
	/* d/dx(f(E)*exp(mu)) = f(E)*exp(mu)*(dmu/dx) */
	for (i = 0; i < 5; i++)
		gradient[i] *= meanPEs_;

	return true;
}

bool
I3PhotoSplineService::GetProbabilityQuantiles(double *time_edges,
    double t_0, double *amplitudes, double gradients[][6], size_t n_bins)
{
	int err;
	size_t i, j;
	bool retval = false;
	double jacobian[6][6];
	double buffer[7], prev_buffer[7], temp_grad;
	double *grad = &buffer[1];
	double *prev_grad = &prev_buffer[1];
	int (I3PhotoSplineTable::*evalThis)(double *, double *);
	bool use_gradients = (gradients != NULL);

	if (meanPEs_ < 0) {
		log_error("SelectSource must be called before "
		    "GetProbabilityQuantiles()!");
		return retval;
	}

	if (use_gradients) {
		I3PhotonicsCommons::fillJacobian(xOM_, yOM_, zOM_, r_, rho_,
		    cosAzi_, l_, lastSource_, jacobian, geometry_, parity_, nGroup_);
		evalThis = &I3PhotoSplineTable::EvalGradients;
	} else {
		evalThis = &I3PhotoSplineTable::Eval;
	}

	int timeindex;
	double tablecoordinates[6];

	/* Get coordinates for table lookup */
	switch (geotype_) {
	case POINTSOURCE:
		timeindex = 3;
		break;
	case INFINITEMUON:
		timeindex = 2;
		break;
	default:
		log_fatal("Unknown spline table geometry %d", geotype_);
	}

	FillTableCoordinates(tablecoordinates, true);
	tablecoordinates[timeindex] = time_edges[0] - t_0;

	const double *const timesupport = timingSplineTable_->GetTimeRange();
	
	/* 
	 * Check if we're in the region where the spline is guaranteed
	 * monotonic.
	 *
	 * NB: This strange syntax calls the member function pointed to by 
	 * evalThis with the raw pointer held by timingSplineTable_.
	 *
	 */

	if ((tablecoordinates[timeindex] >= timesupport[0]) &&
	    (tablecoordinates[timeindex] <= timesupport[1]))
		err = (timingSplineTable_.get()->*evalThis)(tablecoordinates,
		    prev_buffer);
	else
		err = 1;
	
	if (err || !std::isfinite(prev_buffer[0]) || prev_buffer[0] < 0)
		prev_buffer[0] = 0.;
	
	for (i = 0; i < 6; i++) {
		if (err || !std::isfinite(prev_grad[i]))
			prev_grad[i] = 0.;
	}

	for (i = 0; i < n_bins; i++) {
		bzero(buffer, sizeof(buffer));
		err = 0;

		tablecoordinates[timeindex] = time_edges[i+1] - t_0;
		if (tablecoordinates[timeindex] > timesupport[1]) 
			buffer[0] = 1.0;
		else if (tablecoordinates[timeindex] <= timesupport[0])
			buffer[0] = 0.0;	
		else
			err = (timingSplineTable_.get()->*evalThis)(
			    tablecoordinates, buffer);
		
		double bin_prob = buffer[0] - prev_buffer[0];
		if (bin_prob < 0)
			bin_prob = 0;
		if (buffer[0] < 0)
			buffer[0] = 0;
		
		if (err || !std::isfinite(bin_prob) || buffer[0] < 0) {
			bin_prob = 0;
			if (geotype_ == POINTSOURCE)
				log_warn("A PhotoSpline call failed at "
				    "[%.2e %.2e %.2e %.2e] %.2e -- %.2e "
				    "bin_prob: %e - %e = %e, set to 0",
				    tablecoordinates[0], tablecoordinates[1],
				    tablecoordinates[3], tablecoordinates[4],
				    time_edges[i]-t_0,
				    tablecoordinates[2], buffer[0],
				    prev_buffer[0], bin_prob);
			else
				log_warn("A PhotoSpline call failed at "
				    "[%.2e %.2e %.2e %.2e %.2e] %.2e -- %.2e "
				    "bin_prob: %e - %e = %e, set to 0",
				    tablecoordinates[0], tablecoordinates[1],
				    tablecoordinates[2], tablecoordinates[4],
				    tablecoordinates[5], time_edges[i]-t_0,
				    tablecoordinates[3], buffer[0],
				    prev_buffer[0], bin_prob);
		}

		prev_buffer[0] = buffer[0];
		amplitudes[i] = bin_prob;
		
		if (!use_gradients)
			continue;
		
		for (j = 0; j < 6; j++) {
			if (err || !std::isfinite(grad[j]))
				grad[j] = 0.;
			
			/* Subtract off the gradient at the left edge of the bin,
			 * and store the right edge for the next bin. */
			temp_grad = grad[j];
			grad[j] -= prev_grad[j];
			prev_grad[j] = temp_grad;	
		}
		
		/* Convert the gradient to detector coordinates */
		convert_grad(jacobian, grad, gradients[i]);
	}

	return true;
}

bool
I3PhotoSplineService::GetProbabilityQuantileGradients(double *time_edges,
    double t_0, double gradients[][6], size_t n_bins)
{
	double dummy_amp[n_bins];
	
	return GetProbabilityQuantiles(time_edges, t_0, dummy_amp, gradients,
	    n_bins);
}

bool
I3PhotoSplineService::GetProbabilityQuantileHessians(double *time_edges,
    double t_0, double values[], double gradients[][6], double hessians[][6][6],
    size_t n_bins)
{
	unsigned i;
	int j, k, err;
	double tablecoordinates[6];
	double jacobian[6][6];
	double hess_tensor[6][6][6];
	double grad_buffer[7], prev_grad_buffer[7];
	double *grad = &grad_buffer[1]; // grad buffer is value + 6 derivatives
	double *prev_grad = &prev_grad_buffer[1];
	double hessian_buffer[6][6], prev_hessian_buffer[6][6], temp;
	
	if (meanPEs_ < 0) {
		log_error("No light source selected! SelectSource() must be "
		    "called before GetTotalAmplitudeGradient().");
		return false;
	}
	
	I3PhotonicsCommons::fillHessian(xOM_, yOM_, zOM_, r_, rho_, cosAzi_, l_,
	    lastSource_, jacobian, hess_tensor, geometry_, parity_);
	
	FillTableCoordinates(tablecoordinates, true);
	tablecoordinates[3] = time_edges[0] - t_0;
	
	timingSplineTable_->EvalGradients(tablecoordinates,
	    prev_grad_buffer);
	timingSplineTable_->EvalHessian(tablecoordinates,
	    prev_hessian_buffer);
	
	for (i = 0; i < n_bins; i++) {
		tablecoordinates[3] = time_edges[i+1] - t_0;
		
		timingSplineTable_->EvalGradients(tablecoordinates,
		    grad_buffer);
		err = timingSplineTable_->EvalHessian(tablecoordinates,
		    hessian_buffer);
			
		values[i] = grad_buffer[0] - prev_grad_buffer[0];
		prev_grad_buffer[0] = grad_buffer[0];
		
		for (j = 0; j < 6; j++) {
			if (err || !std::isfinite(grad[j]))
				grad[j] = 0.;
			
			/* Subtract off the value at the left edge of the bin,
			 * and store the right edge for the next bin. */
			temp = grad[j];
			grad[j] -= prev_grad[j];
			prev_grad_buffer[j] = temp;	
		}
		
		for (j = 0; j < 6; j++) {
			for (k = 0; k <= j; k++) {
				temp = hessian_buffer[j][k];
				hessian_buffer[j][k] -=
				    prev_hessian_buffer[j][k];
				prev_hessian_buffer[j][k] = temp;
			}
		}
		
		/* Convert the gradient to detector coordinates */
		convert_grad(jacobian, grad, gradients[i]);
		/* Convert the Hessian to detector coordinates */
		convert_hessian(hess_tensor, jacobian, hessian_buffer,
		    &grad_buffer[1], hessians[i]);
	}

	return true;
}

