/* Copyright (C) LinBox
 *
 * Author: Zhendong Wan
 *
 *
 * ========LICENCE========
 * This file is part of the library LinBox.
 *
  * LinBox is free software: you can redistribute it and/or modify
 * it under the terms of the  GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 */



/*! @file  tests/test-rational-solver-adaptive.C
 * @ingroup tests
 * @brief  no doc
 * @test no doc.
 */



#include "linbox/linbox-config.h"
#include "givaro/zring.h"
#include "linbox/blackbox/diagonal.h"
#include "linbox/vector/blas-vector.h"
#include "linbox/algorithms/rational-solver-adaptive.h"
#include <iostream>
#include "test-common.h"
#include "linbox/vector/stream.h"
#include "linbox/util/commentator.h"
using namespace LinBox; // fragile

/// Testing Nonsingular Random Diagonal solve.
template <class Ring, class Field, class Vector>
bool testRandomSolve (const Ring& R,
		      const Field& f,
		      LinBox::VectorStream<Vector>& stream1,
		      LinBox::VectorStream<Vector>& stream2)
{


	// std::ostringstream str;



	commentator().start ("Testing Nonsingular Random Diagonal solve ", "testNonsingularRandomDiagonalSolve");

	bool ret = true;


	VectorDomain<Ring> VD (R);

	int n = (int)stream1.n();
	Vector d(R,n), b(R,n), X(R,n), y(R,n);

	// VectorWrapper::ensureDim (d, stream1.n ());
	// VectorWrapper::ensureDim (b, stream1.n ());
	// VectorWrapper::ensureDim (X, stream1.n ());
	// VectorWrapper::ensureDim (y, stream1.n ());


	while (stream1 && stream2) {

		commentator().startIteration ((unsigned)stream1.j ());

                //ActivityState state = commentator().saveActivityState ();

                bool iter_passed = true;

		bool zeroEntry;
		do {
		  stream1.next (d);
		  zeroEntry = false;
		  for (size_t i=0; i<stream1.n(); i++)
		    zeroEntry |= R.isZero(d[(size_t)i]);
		} while (zeroEntry);

        stream2.next (b);

        std::ostream &report = commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION);
        report << "Diagonal entries: ";
        VD.write (report, d);
        report << endl;

        report << "Right-hand side:  ";
        VD.write (report, b);
        report << endl;

        //Diagonal<Ring> D(R, d);

		BlasMatrix<Ring> D(R, (size_t) n, (size_t) n);

		for(size_t i = 0; i < (size_t)n; ++i) {
			typename Ring::Element x; R.init(x, d[i]);
			D.setEntry(i, i, x);
			//for(size_t j = 0; j < n; ++j) 
			//	if (j != i) D.setEntry(i,j,R.zero);
		}

		D.write(report<<"Matrix: \n");	

		typedef RationalSolverAdaptive RSolver;
		RSolver rsolver;

		BlasVector<Ring> num(R,(size_t)n);
		typename Ring::Element den;

		SolverReturnStatus solveResult = rsolver.solveNonsingular(num, den, D, b); //often 5 primes are not enough

        report << "Solution numerator: ";
        VD.write (report, num);
        report << ", denominator: " << den << endl;
#if 0
		typename Ring::Element lden;

		R. assign (lden, R.one);

		typename std::vector<std::pair<typename Ring::Element, typename Ring::Element> >::iterator p;

		for (p = answer.begin(); p != answer.end(); ++ p)
			R. lcm (lden, lden, p->second);
		typename Vector::iterator p_x;
		//typename Vector::iterator p_y;
#endif

		if (solveResult == SS_OK) {
#if 0
		  for (p = answer.begin(), p_x = X. begin();
		       p != answer.end();
		       ++ p, ++ p_x) {

		    R. mul (*p_x, p->first, lden);

		    R. divin (*p_x, p->second);

		  }

		  D. apply (y, X);
#endif
		  D. apply (y, num);

		  VD. mulin(b, den);

		  if (!VD.areEqual (y, b)) {
		    ret = iter_passed = false;
		    commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
		      << "ERROR: Computed solution is incorrect" << endl;
		  }
		}
		else {
		    ret = iter_passed = false;
		    commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
		      << "ERROR: Did not return OK solving status" << endl;
		}

		commentator().stop ("done");
                commentator().progress ();

	}


	stream1.reset ();
        stream2.reset ();

        commentator().stop (MSG_STATUS (ret), (const char *) 0, "testNonsingularRandomDiagonalSolve");

	return ret;
}

int main(int argc, char** argv)
{

	bool pass = true;

        static size_t n = 10;

	static int iterations = 1;

        static Argument args[] = {
                { 'n', "-n N", "Set order of test matrices to N.", TYPE_INT, &n },
		{ 'i', "-i I", "Perform each test for I iterations.", TYPE_INT, &iterations },
		END_OF_ARGUMENTS
        };


	parseArguments (argc, argv, args);

	typedef Givaro::Modular<int32_t> Field;
	// typedef Givaro::Modular<double> Field;

	typedef Givaro::ZRing<Integer>     Ring;

	Ring R; Ring::RandIter gen(R);

	Field F(101);

	RandomDenseStream<Ring,BlasVector<Ring> > s1 (R, gen, n, (unsigned int)iterations), s2 (R, gen, n, (unsigned int)iterations);



	if (!testRandomSolve(R, F, s1, s2)) pass = false;

	return pass ? 0 : -1;

}


// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,:0,t0,+0,=s
// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:

