
/* Copyright (C) 2010 LinBox
 *
 * Time-stamp: <05 Apr 11 11:01:44 Jean-Guillaume.Dumas@imag.fr>
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

/*! @file tests/test-cradomain.C
 * @ingroup tests
 * @brief tests LinBox::ChineseRemainer
 * @test tests LinBox::ChineseRemainer (see \ref CRA)
 */


#include "linbox/ring/modular.h"
#include "linbox/matrix/dense-matrix.h"
#include "linbox/matrix/matrix-domain.h"
#include "linbox/algorithms/cra-domain.h"
#include "linbox/algorithms/cra-early-multip.h"
#include "linbox/algorithms/cra-full-multip.h"
#include "linbox/algorithms/cra-full-multip-fixed.h"
#include "linbox/algorithms/cra-givrnsfixed.h"
#include "linbox/randiter/random-prime.h"
#include "linbox/integer.h"

using namespace LinBox;

struct Interator {
	BlasVector<Givaro::ZRing<Integer> > _v;
	double maxsize;

	Interator(const BlasVector<Givaro::ZRing<Integer> >& v) :
		_v(v), maxsize(0.0)
	{
		for(BlasVector<Givaro::ZRing<Integer> > ::const_iterator it=_v.begin();
		    it != _v.end(); ++it) {
			//!@bug bb: *it < 0 ?
			double ds = Givaro::naturallog(*it);
			maxsize = (maxsize<ds?ds:maxsize);
		}
	}

	Interator(int n, int s) :
		_v(Givaro::ZRing<Integer>(),(size_t)n), maxsize(0.0)
	{
		for(BlasVector<Givaro::ZRing<Integer> >::iterator it=_v.begin();
		    it != _v.end(); ++it) {
			Integer::random<false>(*it, s);
			double ds = Givaro::naturallog(*it);
			maxsize = (maxsize<ds?ds:maxsize);
		}
	}

	const BlasVector<Givaro::ZRing<Integer> >& getVector()
	{
	       	return _v;
       	}
	double getLogSize() const
	{
	       	return maxsize;
	}

	template<typename Field>
	BlasVector<Field>& operator()(BlasVector<Field>& v,
				      const Field& F) const
	{
		v.resize(_v.size());
		BlasVector<Givaro::ZRing<Integer> >::const_iterator vit=_v.begin();
		typename BlasVector<Field>::iterator eit=v.begin();
		for( ; vit != _v.end(); ++vit, ++eit){
			F.init(*eit, *vit);
		}

		return v;
	}
};

struct InteratorIt;
namespace LinBox
{
	template<class Field>
	struct CRATemporaryVectorTrait<InteratorIt , Field> {
		typedef typename BlasVector<Field>::iterator Type_t;
	};
}

struct InteratorIt : public Interator {

	// could use BlasVector and changeField
	mutable std::vector<double> _vectC;

	InteratorIt(const BlasVector<Givaro::ZRing<Integer> >& v) :
		Interator(v), _vectC(v.size())
	{}
	InteratorIt(int n, int s) :
		Interator(n,s), _vectC((size_t)n)
	{}

	template<typename Iterator, typename Field>
	Iterator& operator()(Iterator& res, const Field& F) const
	{
		BlasVector<Givaro::ZRing<Integer> >::const_iterator vit=this->_v.begin();
		std::vector<double>::iterator eit=_vectC.begin();
		for( ; vit != _v.end(); ++vit, ++eit) {
			F.init(*eit, *vit);
		}

		return res=_vectC.begin();
	}

};

template<typename Field> struct InteratorBlas;

namespace LinBox
{
	template<class Field>
	struct CRATemporaryVectorTrait<InteratorBlas<Field>, Field > {
		typedef typename LinBox::BlasMatrix<Field>::pointer Type_t;
	};
}

template<typename Field>
struct InteratorBlas : public Interator {
	typedef typename Field::Element Element;
	typedef LinBox::BlasMatrix<Givaro::ZRing<Element> > Matrix;
	typedef typename Matrix::pointer Pointer;
	typename Givaro::ZRing<Element> _field;
	mutable Matrix _vectC;

	InteratorBlas(const BlasVector<Givaro::ZRing<Integer> >& v) :
		Interator(v),
		_field(),
		_vectC(_field,(int)v.size(), (int)1)
	{}

	InteratorBlas(int n, int s) :
		Interator(n,s),
		_field(),
		_vectC(_field,n,1) {}

	Pointer& operator()(Pointer& res, const Field& F) const
	{
		BlasVector<Givaro::ZRing<Integer> >::const_iterator vit=this->_v.begin();
		res = _vectC.getWritePointer();
		for( ; vit != _v.end(); ++vit, ++res)
			F.init(*res, *vit);

		return res=_vectC.getWritePointer();
	}

};

#include <typeinfo>


template<typename Builder, typename Iter, typename RandGen, typename BoundType>
bool TestOneCRA(std::ostream& report, Iter& iteration, RandGen& genprime, size_t N, const BoundType& bound)
{
	report << "ChineseRemainder<" << typeid(Builder).name() << ">(" << bound << ')' << std::endl;
	LinBox::ChineseRemainder< Builder > cra( bound );
	Givaro::ZRing<Integer> Z;
	BlasVector<Givaro::ZRing<Integer> > Res(Z,N);
	cra( Res, iteration, genprime);
	bool locpass = std::equal( Res.begin(), Res.end(), iteration.getVector().begin() );
	if (locpass) report << "ChineseRemainder<" << typeid(Builder).name() << ">(" << iteration.getLogSize() << ')' << ", passed."  << std::endl;
	else {
		report << "***ERROR***: ChineseRemainder<" << typeid(Builder).name() << ">(" << iteration.getLogSize() << ')' << "***ERROR***"  << std::endl;
		BlasVector<Givaro::ZRing<Integer> >::const_iterator Rit=Res.begin();
		BlasVector<Givaro::ZRing<Integer> >::const_iterator Oit=iteration.getVector().begin();
		for( ; Rit!=Res.end(); ++Rit, ++Oit)
			if (*Rit != *Oit)
				report << *Rit <<  " != " << * Oit << std::endl;

	}
	return locpass;
}

template<typename Builder, typename Iter, typename RandGen, typename BoundType>
bool TestOneCRAbegin(std::ostream& report, Iter& iteration, RandGen& genprime, size_t N, const BoundType& bound)
{
	Givaro::ZRing<Integer> Z;
	report << "ChineseRemainder<" << typeid(Builder).name() << ">(" << bound << ')' << std::endl;
	LinBox::ChineseRemainder< Builder > cra( bound );
	BlasVector<Givaro::ZRing<Integer> > Res(Z,N);
	BlasVector<Givaro::ZRing<Integer> >::iterator ResIT= Res.begin();
	cra( ResIT, iteration, genprime);
	bool locpass = std::equal( Res.begin(), Res.end(), iteration.getVector().begin() );
	if (locpass) report << "ChineseRemainder<" << typeid(Builder).name() << ">(" << iteration.getLogSize() << ')' << ", passed."  << std::endl;
	else {
		report << "***ERROR***: ChineseRemainder<" << typeid(Builder).name() << ">(" << iteration.getLogSize() << ')' << "***ERROR***"  << std::endl;
		BlasVector<Givaro::ZRing<Integer> >::const_iterator Rit=Res.begin();
		BlasVector<Givaro::ZRing<Integer> >::const_iterator Oit=iteration.getVector().begin();
		for( ; Rit!=Res.end(); ++Rit, ++Oit)
			if (*Rit != *Oit)
				report << *Rit <<  " != " << * Oit << std::endl;

	}
	return locpass;
}

template<typename Builder, typename Iter, typename RandGen, typename BoundType>
bool TestOneCRAWritePointer(std::ostream& report, Iter& iteration, RandGen& genprime, size_t N, const BoundType& bound)
{
	report << "ChineseRemainder<" << typeid(Builder).name() << ">(" << bound << ')' << std::endl;
	LinBox::ChineseRemainder< Builder > cra( bound );
	Givaro::ZRing<Integer> Z ;
	LinBox::BlasMatrix<Givaro::ZRing<Integer> > Res(Z, (int)N, (int)N);
	cra( Res.getWritePointer(), iteration, genprime);
	bool locpass = std::equal( iteration.getVector().begin(), iteration.getVector().end(), Res.getWritePointer() );

	if (locpass) {
		report << "ChineseRemainder<" << typeid(Builder).name() << ">(" << iteration.getLogSize() << ')' << ", passed."  << std::endl;
	}
	else {
		report << "***ERROR***: ChineseRemainder<" << typeid(Builder).name() << ">(" << iteration.getLogSize() << ')' << "***ERROR***"  << std::endl;
	}
	return locpass;
}


bool TestCra(size_t N, int S, size_t seed)
{

	std::ostream &report = LinBox::commentator().report (LinBox::Commentator::LEVEL_IMPORTANT,
							   INTERNAL_DESCRIPTION);
	// std::ostream &report = std::cout;
	Givaro::ZRing<Integer> Z;

	size_t new_seed = (seed?(seed):((size_t)BaseTimer::seed())) ;
	report << "TestCra(" << N << ',' << S << ',' << new_seed << ')' << std::endl;
	Integer::seeding(new_seed);

	Interator iteration((int)N, S);
	InteratorIt iterationIt(iteration.getVector());
	InteratorBlas<Givaro::Modular<double> > iterationBlas(iteration.getVector());
	LinBox::RandomPrimeIterator genprime( 24, new_seed );

	bool pass = true;

	pass &= TestOneCRA< LinBox::EarlyMultipCRA< Givaro::Modular<double> >,
	     Interator, LinBox::RandomPrimeIterator>(
						     report, iteration, genprime, N, 5);

	pass &= TestOneCRA< LinBox::EarlyMultipCRA< Givaro::Modular<double> >,
	     Interator, LinBox::RandomPrimeIterator>(
						     report, iteration, genprime, N, 15);

	pass &= TestOneCRA< LinBox::FullMultipCRA< Givaro::Modular<double> >,
	     Interator, LinBox::RandomPrimeIterator>(
						     report, iteration, genprime, N, iteration.getLogSize()+1);

	pass &= TestOneCRA< LinBox::FullMultipCRA< Givaro::Modular<double> >,
	     Interator, LinBox::RandomPrimeIterator>(
						     report, iteration, genprime, N, 3*iteration.getLogSize()+15);

#if 0
	pass &= TestOneCRAbegin<LinBox::FullMultipFixedCRA< Givaro::Modular<double> >,
	     InteratorIt, LinBox::RandomPrimeIterator>(
						       report, iterationIt, genprime, N, std::pair<size_t,double>(N,iteration.getLogSize()+1));

	pass &= TestOneCRAbegin<LinBox::FullMultipFixedCRA< Givaro::Modular<double> >,
	     InteratorIt, LinBox::RandomPrimeIterator>(
						       report, iterationIt, genprime, N, std::pair<size_t,double>(N,3*iteration.getLogSize()+15));


	pass &= TestOneCRAWritePointer<LinBox::FullMultipFixedCRA< Givaro::Modular<double> >,
	     InteratorIt, LinBox::RandomPrimeIterator>(
						       report, iterationIt, genprime, N, std::pair<size_t,double>(N,iterationIt.getLogSize()+1) );

	pass &= TestOneCRAWritePointer<LinBox::FullMultipFixedCRA< Givaro::Modular<double> >,
	     InteratorIt, LinBox::RandomPrimeIterator>(
						       report, iterationIt, genprime, N, std::pair<size_t,double>(N,3*iterationIt.getLogSize()+15) );

	pass &= TestOneCRAWritePointer<LinBox::FullMultipFixedCRA< Givaro::Modular<double> >,
	     InteratorBlas< Givaro::Modular<double> >,
	     LinBox::RandomPrimeIterator>(
					  report, iterationBlas, genprime, N, std::pair<size_t,double>(N,iterationIt.getLogSize()+1) );

	pass &= TestOneCRAWritePointer<LinBox::FullMultipFixedCRA< Givaro::Modular<double> >,
	     InteratorBlas< Givaro::Modular<double> >,
	     LinBox::RandomPrimeIterator>(
					  report, iterationBlas, genprime, N, std::pair<size_t,double>(N,3*iterationIt.getLogSize()+15) );
#endif


        BlasVector<Givaro::ZRing<Integer> >  PrimeSet(Z);
        double PrimeSize = 0.0;
        for( ; PrimeSize < (iterationIt.getLogSize()+1); ++genprime ) {
            if (std::find(PrimeSet.begin(), PrimeSet.end(), *genprime) == PrimeSet.end()) {
                PrimeSet.push_back( *genprime );
                PrimeSize += Givaro::naturallog(*genprime);
            }
        }

        BlasVector<Givaro::ZRing<Integer> > ::iterator psit = PrimeSet.begin();

	pass &= TestOneCRA<
            LinBox::GivaroRnsFixedCRA< Givaro::Modular<double> >,
            Interator,
            BlasVector<Givaro::ZRing<Integer> > ::iterator,
            BlasVector<Givaro::ZRing<Integer> >  >(
                 report, iteration, psit, N, PrimeSet);


	if (pass) report << "TestCra(" << N << ',' << S << ')' << ", passed." << std::endl;
	else
		report << "***ERROR***: TestCra(" << N << ',' << S << ')' << " ***ERROR***" << std::endl;

	return pass;
}

#include "test-common.h"
#include "linbox/util/timer.h"

int main (int argc, char **argv)
{
	static size_t n = 10;
	static size_t s = 30;
	static size_t seed = 0;
	static int iterations = 20;

	static Argument args[] = {
		{ 'n', "-n N", "Set dimension of test vectors to NxN.", TYPE_INT , &n },
		{ 's', "-s S", "Set size of test integers.", TYPE_INT , &s },
		{ 'z', "-z Z", "Set seed.", TYPE_INT , &seed },
		{ 'i', "-i I", "Perform each test for I iterations.",     TYPE_INT, &iterations },
		END_OF_ARGUMENTS
	};

	parseArguments (argc, argv, args);

	LinBox::commentator().start("CRA-Domain test suite", "CRADom");
	bool pass = true;

	for(int i=0; pass && i<iterations; ++i)
		pass &= TestCra((size_t)n,(int)s,seed);

	LinBox::commentator().stop(MSG_STATUS (pass), "CRA-Domain test suite");
	return pass ? 0 : -1;
}

// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
