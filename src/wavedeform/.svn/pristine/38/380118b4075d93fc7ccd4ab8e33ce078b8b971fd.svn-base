/*
 * Copyright (c) 2010-2011 IceCube Collaboration. All rights reserved.
 *
 * Authors:
 *    Nathan Whitehorn <nwhitehorn@icecube.wisc.edu>
 *    Jakob van Santen <jvansanten@icecube.wisc.edu>
 *    Jim Braun	 <jbraun@icecube.wisc.edu>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <cholmod.h>
#include <SuiteSparseQR_C.h>

/* Solve Ap(x) = y by transforming to normal equations */
cholmod_dense *
solve_by_normaleq(cholmod_sparse *Ap, cholmod_dense *y,
    int nP, cholmod_common *c);

/*
 * Lawson-Hanson NNLS
 *
 * Algorithm NNLS from "Solving Least Squares Problems", Charles Lawson and
 *  Richard Hanson. Prentice-Hall, 1974.
 */
cholmod_dense *
nnls_lawson_hanson(cholmod_sparse *A, cholmod_dense *y, double tolerance,
    unsigned min_iterations, unsigned max_iterations, unsigned npos,
    int normaleq, int solve_with_normaleq, int verbose, cholmod_common *c)
    __attribute__((visibility("hidden")));

cholmod_dense *
nnls_lawson_hanson(cholmod_sparse *A, cholmod_dense *y, double tolerance,
    unsigned min_iterations, unsigned max_iterations, unsigned npos,
    int normaleq, int solve_with_normaleq, int verbose, cholmod_common *c)
{
	cholmod_dense *x, *w, *p, *yp;
	cholmod_sparse *Ap;
	long *P = malloc(A->ncol * sizeof(long));
	long *Z = malloc(A->ncol * sizeof(long));
	int nP, nZ;
	double wmax, wpmin, alpha, qtemp;
	int i, j, n, t, qmax;
	int last_freed = -1;

	/* By default, all coefficients are positive */
	if (npos == 0 || npos > A->ncol)
		npos = A->ncol;

	/* Step 1: All elements actively constrained */
	nP = A->ncol - npos; nZ = npos;
	for (i = 0; i < nZ; i++)
		Z[i] = i;
	for (i = 0; i < nP; i++)
		P[i] = npos + i;

	/* Initialize coefficient and negative gradient vectors */
	x = cholmod_l_zeros(A->ncol, 1, CHOLMOD_REAL, c);
	w = cholmod_l_zeros(A->ncol, 1, CHOLMOD_REAL, c);

	for (n = 0; n < max_iterations || max_iterations == 0; n++) {
		/* Step 2: compute the negative gradient of the residuals, w */
		if (normaleq) {
			/* If the normal equations are pre-formulated,
			 * w = y - Ax. */
			double alpha[2] = {1.0, 0.0}, beta[2] = {-1.0, 0.0};

			memcpy(w->x, y->x, sizeof(double)*A->ncol);
			cholmod_l_sdmult(A, 0 /* no transpose */, beta, alpha,
			    x, w, c);
		} else {
			/* Otherwise, take the derivative: w = At(y - Ax) */
			cholmod_dense *wtemp;
			double alpha[2] = {1.0, 0.0}, beta[2] = {-1.0, 0.0};

			wtemp = cholmod_l_copy_dense(y, c);
			cholmod_l_sdmult(A, 0 /* no transpose */, beta, alpha,
			    x, wtemp, c);
			beta[0] = 0;
			cholmod_l_sdmult(A, 1 /* transpose */, alpha, beta,
			    wtemp, w, c);
			cholmod_l_free_dense(&wtemp, c);
		}

		/* Step 3a: Check for completion */
		if (nZ == 0)
			break;

		/* Steps 3b,4: Find maximum w, complete if <= 0 */
		wmax = ((double *)(w->x))[Z[0]];
		t = 0;
		for (i = 1; i < nZ; i++) {
			if (((double *)(w->x))[Z[i]] > wmax &&
			    last_freed != Z[i]) {
				t = i;
				wmax = ((double *)(w->x))[Z[t]];
			}
		}

		if (wmax <= 0)
			break;

		/* See if we might be within fit tolerance */
		if (wmax < tolerance && n >= min_iterations) {
			/* Check if any passive coefficients need to be reduced
			 * due to clipped ringing */
			if (nP == 0)
				break;

			wpmin = ((double *)(w->x))[P[0]];
			for (i = 1; i < nP; i++) {
				if (((double *)(w->x))[P[i]] < wpmin)
					wpmin = ((double *)(w->x))[P[i]];
			}

			if (-wpmin < tolerance)
				break;
		}

		/* Step 5: Move coefficient Z[t] into the passive set P */
		if (verbose)
			printf("Freeing coefficient %ld (active: %d, "
			    "passive: %d, wmax: %e)\n", Z[t], nZ, nP, wmax);
		last_freed = Z[t];
		alpha = -1;
		P[nP++] = Z[t];
		nZ--;
		for (i = t; i < nZ; i++)
			Z[i] = Z[i+1];

		/*
		 * Steps 6-11: Move coefficients from the passive to the
		 * active set
		 */
		while (1) {
			/*
			 * Step 6: Solve the least-squares subproblem for the
			 * columns of A in the passive set P.
			 */
			if (normaleq) {
				Ap = cholmod_l_submatrix(A, P, nP, P, nP, 1,
				    1, c);
				yp = cholmod_l_allocate_dense(nP, 1, nP,
				    CHOLMOD_REAL, c);
				for (i = 0; i < nP; i++)
					((double *)(yp->x))[i] =
					    ((double *)(y->x))[P[i]];
				p = SuiteSparseQR_C_backslash_default(Ap, yp, c);
				cholmod_l_free_sparse(&Ap, c);
				cholmod_l_free_dense(&yp, c);
			} else {
				Ap = cholmod_l_submatrix(A, NULL, -1, P, nP,
				    1, 1, c);
				p = NULL;
				if (nP < 15 && solve_with_normaleq) {
					/* Solve by forming normal equations.
					 * This is not numerically stable, but
					 * at double precision it is sufficient
					 * for almost all practical purposes and
					 * is much faster at least up to 15x15.
					 */
					p = solve_by_normaleq(Ap, y, nP, c);
				}
				if (p == NULL) {
					/* Solve by QR factorization */
					p = SuiteSparseQR_C_backslash_default(
					    Ap, y, c);
				}
				cholmod_l_free_sparse(&Ap, c);
			}

			/*
			 * Step 7: Check if any coefficients need be constrained
			 */
			for (i = 0; i < nP; i++)
				if (P[i] < npos && ((double *)(p->x))[i] <= 0)
					break;

			if (i == nP) {
				/*
				 * All were positive. Cycle back for the next.
				 */
				memset(x->x, '\0', sizeof(double)*x->nrow);
				for (i = 0; i < nP; i++)
					((double *)(x->x))[P[i]] =
					    ((double *)(p->x))[i];
				cholmod_l_free_dense(&p, c);
				break; /* Break loop to step 2 */
			}

			/*
			 * Step 8-9: Compute q, alpha
			 */
			alpha = 2; /* All computed values <= 1 */
			qmax = -1;
			for (i = 0; i < nP; i++) {
				if (P[i] >= npos || ((double *)(p->x))[i] > 0)
					continue;

				qtemp = ((double *)(x->x))[P[i]] /
				    (((double *)(x->x))[P[i]] -
				    ((double *)(p->x))[i]);

				if (qtemp < alpha && qtemp != 0) {
					qmax = P[i];
					alpha = qtemp;
				} else if (last_freed == P[i]) {
					/* Anti-cycling advice from LH */
					alpha = 0;
					qmax = P[i];
					break;
				}
			}

			if (qmax < 0) {
				fprintf(stderr, "%s line %d: Math has failed\n",
				    __FILE__, __LINE__);
				exit(1);
			}

			/*
			 * Step 10: Recompute x
			 */
			for (i = 0; i < nP; i++)
				((double *)(x->x))[P[i]] += alpha*
				    (((double *)(p->x))[i] -
				    ((double *)(x->x))[P[i]]);

			/* Avoid rounding errors above */
			((double *)(x->x))[qmax] = 0;
			cholmod_l_free_dense(&p, c);

			/*
			 * Step 11: Move coefficients equal to zero to the
			 * active set.
			 */
			for (i = 0; i < nP; i++) {
				if (P[i] >= npos || ((double *)(x->x))[P[i]] > 0)
					continue;

				if (verbose)
					printf("\tConstraining coefficient %ld "
					    "(active: %d, passive: %d, "
					    "value: %e)\n", P[i], nZ, nP,
					    ((double *)(x->x))[P[i]]);

				((double *)(x->x))[P[i]] = 0;
				Z[nZ++] = P[i];
				nP--;
				for (j = i; j < nP; j++)
					P[j] = P[j+1];
				i--;
			}

			/* If alpha = 0, we've reached equilibrium */
			if (alpha == 0)
				break;
		}

		/* Exit to the caller in equilibrium */
		if (alpha == 0)
			break;
	}

	/* Step 12: return */
	cholmod_l_free_dense(&w, c);
	free(P);
	free(Z);
	return (x);
}

cholmod_dense *
solve_by_normaleq(cholmod_sparse *Ap, cholmod_dense *y,
    int nP, cholmod_common *c) {

	cholmod_dense *p = NULL;
	if (nP == 1) {
		/* Solution to Ap^t(y) = Ap^t(Ap(x)) is trivial
		 * x[P[0]] = [(Ap)^T * y] / [(Ap)^T * Ap]
		 */
		double alpha[2] = {1.0, 0.0}, beta[2] = {0.0, 0.0};
		p = cholmod_l_zeros(1, 1, CHOLMOD_REAL, c);
		cholmod_dense *pr = cholmod_l_sparse_to_dense(Ap, c);
		cholmod_l_sdmult(Ap, 1, alpha, beta, pr, p, c);
		cholmod_l_free_dense(&pr, c);
		double denom = ((double *)(p->x))[0];
		if (denom <= 0.) {
			cholmod_l_free_dense(&p, c);
			p = NULL;
		} else {
			cholmod_l_sdmult(Ap, 1, alpha, beta, y, p, c);
			((double *)(p->x))[0] /= denom;
		}
		return p;
	}

	cholmod_sparse *Apt = cholmod_l_transpose(Ap, 1, c);
	cholmod_sparse *dp = cholmod_l_aat(Apt, NULL, 0, 1, c);
	double alpha[2] = {1.0, 0.0}, beta[2] = {0.0, 0.0};
	cholmod_dense *yp = cholmod_l_allocate_dense(nP, 1, nP, CHOLMOD_REAL, c);
	cholmod_l_sdmult(Apt, 0, alpha, beta, y, yp, c);

	if (nP == 2) {

		/* Apply Cramer's rule to solve the 2x2 system */
		double *m = (double *)(yp->x);
		p = cholmod_l_zeros(2, 1, CHOLMOD_REAL, c);
		// Read A^tA from the sparse matrix.	This is painful.
		double a[2][2] = {{0., 0.}, {0., 0.}};
		int i;
		for (i = 0; i < 2; ++i) {
			long idx;
			for (idx = ((long*)(dp->p))[i];
			    idx < ((long*)(dp->p))[i+1]; ++idx) {
				a[i][((long*)(dp->i))[idx]] =
				    ((double*)(dp->x))[idx];
			}
		}
		double d = (a[0][0]*a[1][1] - a[0][1]*a[1][0]);
		if (d <= 0.) {
			cholmod_l_free_dense(&p, c);
			p = NULL;
		} else {
			d = 1. / d;
			((double *)(p->x))[0] = (m[0]*a[1][1]-m[1]*a[0][1])*d;
			((double *)(p->x))[1] = (m[1]*a[0][0]-m[0]*a[1][0])*d;
		}
	} else {
		/*
		 *	Solve with Cholesky factorization. Ap^t(Ap) will
		 *	always be positive-definite.
		 */
		dp->stype = 1; //< Symmetric flag for Ap^tAp.
		cholmod_factor *L = cholmod_l_analyze(dp, c);
		c->status = CHOLMOD_OK;
		cholmod_l_factorize(dp, L, c);
		if (c->status != CHOLMOD_NOT_POSDEF) {
			p = cholmod_l_solve(CHOLMOD_A, L, yp, c);
		}
		cholmod_l_free_factor(&L, c);
	}

	cholmod_l_free_sparse(&Apt, c);
	cholmod_l_free_sparse(&dp, c);
	cholmod_l_free_dense(&yp, c);
	return p;
}

