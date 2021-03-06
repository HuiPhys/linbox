/* Copyright (C) LinBox
 *
 * using generic testBlackbox  -bds
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

/*! @file  tests/test-quad-matrix.C
 * @ingroup tests
 * @brief no doc.
 * @test no doc.
 */




#include "linbox/linbox-config.h"

#include <iostream>
#include <fstream>

#include <set>

#include "linbox/ring/modular.h"
#include "linbox/blackbox/quad-matrix.h"

#include "test-blackbox.h"

using namespace LinBox;

int main (int argc, char **argv)
{
	// ofstream report;

	bool pass = true;

	static size_t n = 100000;
	static integer q = 101;
	static int iterations = 1;

	static Argument args[] = {
		{ 'n', "-n N", "Set dimension of test matrices to NxN.", TYPE_INT,     &n },
		{ 'q', "-q Q", "Operate over the \"field\" GF(Q) [1].", TYPE_INTEGER, &q },
		{ 'i', "-i I", "Perform each test for I iterations.", TYPE_INT,     &iterations },
		END_OF_ARGUMENTS
	};

	parseArguments (argc, argv, args);

	srand ((unsigned int)time (NULL));

	typedef Givaro::Modular<uint32_t> Field;
	typedef ZOQuad <Field> BlackBox;

	Field F (q);
	Field::Element d;
	F.assign(d, F.one);

       	//ScalarMatrix<Field> A (F, n, d); // a small identity.
        //BlackBox AA(A);
	//pass = pass && testBlackbox(AA);

	size_t *rows, *cols, i;
	const size_t npr = n / 10000;
	rows = new size_t[npr * n];
	cols = new size_t[npr * n];

	/*
	// "arrow" matrix
	for(i = 0; i < n; i++) { rows[i] = 0; cols[i] = i; }
	for(i = 0; i < n - 1; i++) { rows[n+2*i] = i + 1; cols[n+2*i] = 0; rows[n+2*i+1] = i + 1; cols[n+2*i+1] = i + 1; }
	ZeroOne<Field> B(F, rows, cols, n, n, 3 * n - 2);
	*/

	// random 3 per row matrix
	for(i = 0; i < n; i++)
		{
			set<size_t> a;
			while( a.size() < npr )
				a.insert((size_t)rand()%n);
			size_t j = 0;
			for(set<size_t>::iterator iter = a.begin(); j < npr; ++j, ++iter)
				{
					rows[npr*i+j] = i;
					cols[npr*i+j] = *iter;
					//std::cout << rows[npr*i+j] << ", ";
				}
			//std::cout << std::endl;
		}
	ZeroOne<Field> B(F, rows, cols, n, n, npr * n );

	/*
	ZeroOne<Field> B(F);
	//ifstream mat_in("data/m133.b3.200200x200200.sms");
	//ifstream mat_in("data/n4c6.b9.186558x198895.sms");
	//ifstream mat_in("data/small21x21.sms");
	//ifstream mat_in("data/iso333");
	ifstream mat_in("../examples/iso334");
	B.read(mat_in);
	*/

	//std::cout << " -- main: " << B.rowdim() << " " << B.coldim() << " " << B.nnz() << std::endl;
	//std::cout << " -- main: ZOQuad matrix blackbox test suite" << std::endl;

	BlackBox BB(B);

	//BB.write(cout) << endl; //just writes the sizes of the strips.

	std::cout << " *** Warning ***, no read-write test" << std::endl;
	pass = pass && testBlackbox(BB, false);

	return pass ? 0 : -1;
}


// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
