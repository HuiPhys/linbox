
/* tests/test-diagonal.C
 * Copyright (C) 2001, 2002 Bradford Hovinen
 *
 * Written by Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * Time-stamp: <02 Dec 15 11:45:00 Jean-Guillaume.Dumas@imag.fr>
 * --------------------------------------------------------
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 *
 */


/*! @file  tests/test-diagonal.C
 * @ingroup tests
 * @brief  no doc
 * @test NO DOC
 */



#include "linbox/linbox-config.h"

#include <iostream>
#include <fstream>

#include <cstdio>

#include "linbox/blackbox/diagonal.h"
#include "linbox/util/commentator.h"
//#include "linbox/field/archetype.h"
#include "linbox/ring/modular.h"
#include <givaro/givranditer.h>
//#include "linbox/solutions/minpoly.h"
//#include "linbox/solutions/rank.h"
//#include "linbox/vector/stream.h"

#include "test-blackbox.h"
//#include "test-generic.h"

using namespace LinBox;

#if 0
/* Test 1: Application of identity matrix onto random vectors
 *
 * Construct the identity matrix and a series of randomly-generated
 * vectors. Apply the identity to each vector and test whether the input and
 * output are equal.
 *
 * F - Field over which to perform computations
 * n - Dimension to which to make matrix
 *
 * Return true on success and false on failure
 */

template <class Field, class Vector>
static bool testIdentityApply (Field &F, VectorStream<Vector> &stream)
{
        typedef LinBox::Diagonal<Field> Blackbox;

	commentator().start ("Testing identity apply", "testIdentityApply", stream.m ());

	bool ret = true;
	// bool iter_passed = true;

	VectorDomain<Field> VD (F);
	Vector d(F);

	size_t i;

	VectorWrapper::ensureDim (d, stream.n ());

	for (i = 0; i < stream.n (); i++)
		F.assign(VectorWrapper::ref<Field> (d, i), F.one);

	Blackbox D (d);

	Vector v(F), w(F);

	VectorWrapper::ensureDim (v, stream.n ());
	VectorWrapper::ensureDim (w, stream.n ());

	while (stream) {
		commentator().startIteration ((unsigned)i);

		bool iter_passed = true;

		stream.next (v);

		ostream &report = commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION);
		report << "Input vector:  ";
		VD.write (report, v);
		report << endl;

		D.apply (w, v);

		report << "Output vector: ";
		VD.write (report, w);
		report << endl;

		if (!VD.areEqual (w, v))
			ret = iter_passed = false;

		if (!iter_passed)
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: Vectors are not equal" << endl;

		commentator().stop (MSG_STATUS (ret));
		commentator().progress ();
	}

	stream.reset ();

	commentator().stop (MSG_STATUS (ret), (const char *) 0, "testIdentityApply");

	return ret;
}

/* Test 2: Constant term in minimal polynomial of diagonal map
 *
 * Generates a random diagonal nonsingular matrix and computes its minimal
 * polynomial. Checks that the constant term thereof equals the product of the
 * entries on the diagonal.
 *
 * F - Field over which to perform computations
 * n - Dimension to which to make matrix
 * iterations - Number of random diagonal matrices to construct
 *
 * Return true on success and false on failure
 */

template <class Field, class Vector>
static bool testRandomMinpoly (Field &F, VectorStream<Vector> &stream)
{
	typedef BlasVector<Field> Polynomial;
	typedef LinBox::Diagonal <Field> Blackbox;

	commentator().start ("Testing random minpoly", "testRandomMinpoly", stream.m ());

	bool ret = true;

	size_t j;
	typename Field::Element pi;
	Polynomial m_D(F);
	VectorDomain<Field> VD (F);

	Vector d(F);

	VectorWrapper::ensureDim (d, stream.n ());

	while (stream) {
		commentator().startIteration ((unsigned)stream.j ());

		F.assign(pi, F.one);

		stream.next (d);

		ostream &report = commentator().report (Commentator::LEVEL_UNIMPORTANT, INTERNAL_DESCRIPTION);
		report << "Diagonal vector: ";
		VD.write (report, d);
		report << endl;

		for (j = 0; j < stream.n (); j++)
			F.mulin (pi, VectorWrapper::constRef<Field> (d, j));

		report << "Product: ";
		F.write (report, pi);
		report << endl;

		Blackbox D (d);
		minpoly (m_D, D);

		report << "Minimal polynomial: ";
		printPolynomial (F, report, m_D);

		if (!F.areEqual (m_D[0], pi)) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: m_D(0) != det(D)" << endl;
			ret = false;
		}

		commentator().stop (MSG_STATUS(ret));
		commentator().progress ();
	}

	stream.reset ();

	// try it with the random cstor of diagonal
	LinBox::Diagonal <Field> D(F, 10);
	unsigned long r;
	LinBox::rank(r, D, Method::Wiedemann());
	if (r != 10)
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: zeroes in random diagonal" << endl;
	ret = ret && r == 10;

	commentator().stop (MSG_STATUS (ret), (const char *) 0, "testRandomMinpoly");

	return ret;
}

/* Test 3: Random linearity
 *
 * Compute a random diagonal matrix and use the linearity test in test-generic.h
 * to ensure that the diagonal black box does indeed correspond to a linear
 * mapping.
 *
 * F - Field over which to perform computations
 * n - Dimension to which to make matrix
 * iterations - Number of random vectors to which to apply matrix
 *
 * Return true on success and false on failure
 */

template <class Field, class Vector>
static bool testRandomLinearity (Field &F,
				 VectorStream<BlasVector<Field> > &d_stream,
				 VectorStream<Vector> &stream1,
				 VectorStream<Vector> &stream2)
{
	typedef LinBox::Diagonal <Field> Blackbox;

	commentator().start ("Testing random transpose", "testRandomLinearity", stream1.m ());

	VectorDomain<Field> VD (F);

	BlasVector<Field> d(F);
	VectorWrapper::ensureDim (d, stream1.n ());

	d_stream.next (d);
	Blackbox D (d);

	ostream &report = commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION);

	report << "Diagonal vector: ";
	VD.write (report, d) << endl;

	bool ret = testLinearity (D, stream1, stream2);

	commentator().stop (MSG_STATUS (ret), (const char *) 0, "testRandomLinearity");

	return ret;
}

/* Test 3: Random transpose
 *
 * Compute a random diagonal matrix and use the transpose test in test-generic.h
 * to check consistency of transpose apply.
 *
 * F - Field over which to perform computations
 * n - Dimension to which to make matrix
 * iterations - Number of random vectors to which to apply matrix
 *
 * Return true on success and false on failure
 */

template <class Field, class Vector>
static bool testRandomTranspose (Field &F,
				 VectorStream<BlasVector<Field> > &d_stream,
				 VectorStream<Vector> &stream1,
				 VectorStream<Vector> &stream2)
{
	typedef LinBox::Diagonal <Field> Blackbox;

	commentator().start ("Testing random transpose", "testRandomTranspose", stream1.m ());

	VectorDomain<Field> VD (F);

	BlasVector<Field> d(F);
	VectorWrapper::ensureDim (d, stream1.n ());

	d_stream.next (d);
	Blackbox D (d);

	ostream &report = commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION);

	report << "Diagonal vector: ";
	VD.write (report, d) << endl;

	bool ret = testTranspose (F, D, stream1, stream2);

	commentator().stop (MSG_STATUS (ret), (const char *) 0, "testRandomTranspose");

	return ret;
}
#endif

int main (int argc, char **argv)
{
	bool pass = true;

	static size_t n = 10;
	static integer q = 65521U;
	static int iterations = 2; // was 100

	static Argument args[] = {
		{ 'n', "-n N", "Set dimension of test matrices to NxN.", TYPE_INT,     &n },
		{ 'q', "-q Q", "Operate over the \"field\" GF(Q) [1].", TYPE_INTEGER, &q },
		{ 'i', "-i I", "Perform each test for I iterations.", TYPE_INT,     &iterations },
		END_OF_ARGUMENTS
	};

	typedef Givaro::Modular<uint32_t> Field; //C.Pernet: avoids confusion with givaro::uint32_t
//	typedef BlasVector<Field> Vector;

	parseArguments (argc, argv, args);
	Field F (q);

	srand ((unsigned)time (NULL));

	commentator().start("Diagonal matrix black box test suite", "diagonal");

	// Make sure some more detailed messages get printed
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (3);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_UNIMPORTANT);

#if 0
    Field::RandIter gen(F);
	RandomDenseStream<Field, Vector> stream1 (F, gen, n, (unsigned int)iterations), stream2 (F, gen, n, (unsigned int)iterations), d_stream (F, gen, n, 1);
	RandomDenseStream<Field, Vector, Givaro::GeneralRingNonZeroRandIter<Field> >
		stream3 (F, Givaro::GeneralRingNonZeroRandIter<Field> (Field::RandIter (F)), n, (unsigned int)iterations);

	if (!testIdentityApply    (F, stream1)) pass = false;
	if (!testRandomMinpoly    (F, stream3)) pass = false;
	if (!testRandomLinearity  (F, d_stream, stream1, stream2)) pass = false;
	if (!testRandomTranspose  (F, d_stream, stream1, stream2)) pass = false;
#endif

        Field::RandIter iter(F);
	LinBox::Diagonal<Field> D(F, n, iter);
	pass = pass && testBlackbox(D);

	commentator().stop (MSG_STATUS (pass));

	return pass ? 0 : -1;
}

// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
