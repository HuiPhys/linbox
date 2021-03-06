/* Copyright (C) 2007 LinBox
 * written by JG Dumas
 *
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

/*! @file algorithms/cra-early-single.h
 * @ingroup algorithms
 * @brief NO DOC
 */


#ifndef __LINBOX_cra_early_single_H
#define __LINBOX_cra_early_single_H

#include "linbox/util/timer.h"
#include <stdlib.h>
#include "linbox/integer.h"
#include "linbox/solutions/methods.h"
#include "linbox/algorithms/cra-domain.h"
#include <vector>
#include <utility>

namespace LinBox
{

	/*!  @brief NO DOC
	 * @ingroup CRA
	 *
	 */
	template<class Domain_Type>
	struct EarlySingleCRA {
		typedef Domain_Type			Domain;
		typedef typename Domain::Element DomainElement;
		typedef EarlySingleCRA<Domain> Self_t;

	protected:
		// PrimeProd*nextM_ is the modulus
		Integer 	primeProd_;
		Integer		nextM_;
		Integer 	residue_; 	// remainder to be reconstructed
		unsigned int    occurency_;	// number of equalities

		const unsigned int    EARLY_TERM_THRESHOLD;

#ifdef _LB_CRATIMING
		mutable Timer tInit, tIteration, tImaging, tIRecon, tOther;
		mutable CRATimer totalTime;
        mutable size_t IterCounter_;
#endif

	public:

		EarlySingleCRA(const unsigned long EARLY=DEFAULT_EARLY_TERM_THRESHOLD) :
			primeProd_(1U),
			nextM_(1U),
			occurency_(0U),
			EARLY_TERM_THRESHOLD((unsigned)EARLY-1)
#ifdef _LB_CRATIMING
            , IterCounter_(0)
#endif
		{
#ifdef _LB_CRATIMING
			clearTimers();
			totalTime.clear();
#endif
		}

#if 0
		EarlySingleCRA(const Self_t& c) :
			primeProd_(c.primeProd_),
			nextM_(c.nextM_),
			residue_(c.residue_),
			occurency_(c.occurency_),
			EARLY_TERM_THRESHOLD(c.EARLY_TERM_THRESHOLD)
		{}
#endif

		virtual void initialize (const Integer& D, const Integer& e)
		{
#ifdef _LB_CRATIMING
			tInit.clear();
			tInit.start();
            IterCounter_=1;
#endif
			primeProd_ = D;
			nextM_ = 1U;
			residue_ = e;
			occurency_ = 1;
#ifdef _LB_CRATIMING
			tInit.stop();
			totalTime.ttInit += tInit;
#endif
		}

		virtual void initialize (const Domain& D, const DomainElement& e)
		{
#ifdef _LB_CRATIMING
			tInit.clear();
			tInit.start();
            IterCounter_=1;
#endif
			D.characteristic( primeProd_ );
			nextM_ = 1U;
			D.convert( residue_, e);
			occurency_ = 1;
#ifdef _LB_CRATIMING
			tInit.stop();
			totalTime.ttInit += tInit;
#endif
		}

		virtual Integer& result(Integer& d)
		{
			return d=residue_;
		}

		// virtual Integer& result(Integer& n, Integer& d) = 0 ; // rational pure virtual

		virtual Integer& getResidue(Integer& r )
		{
			return r= residue_;
		}

		virtual Integer& getModulus(Integer& m)
		{

#ifdef _LB_CRATIMING
			tOther.clear();
			tOther.start();
#endif
			m = primeProd_ * nextM_;
#ifdef _LB_CRATIMING
			tOther.stop();
			totalTime.ttOther += tOther;
#endif
			return m;
		}

		virtual bool terminated()
		{
			return occurency_>EARLY_TERM_THRESHOLD;
		}

		virtual void progress (const Integer& D, const Integer& e)
		{
			// Precondition : initialize has been called once before
			// linbox_check(occurency_ > 0);
#ifdef _LB_CRATIMING
			tIRecon.clear();
			tIRecon.start();
            ++IterCounter_;
#endif
			primeProd_ *= nextM_;
			nextM_ =D;
			Integer u0 = residue_ % D;//0
			Integer u1 = e % D;//e
			Integer m0 = primeProd_;//1
			if (u0 == u1) {
				++occurency_;
			}
			else {
				occurency_ = 1;
				inv(m0, m0, D); // res <-- m0^{-1} mod m1//1
				u0 = u1 - u0;           // tmp <-- (u1-u0)//e
				u0 *= m0;       // res <-- (u1-u0)( m0^{-1} mod m1 )//e
				u0 %= D;
				Integer tmp(u0);//e
				if (u0 < 0)
					tmp += D;//e+D
				else
					tmp -= D;//e-D
				if (absCompare(u0,tmp) > 0) u0 = tmp;
				u0 *= primeProd_;          // res <-- (u1-u0)( m0^{-1} mod m1 ) m0       and res <= (m0m1-m0)
				residue_ += u0;   // res <-- u0 + (u1-u0)( m0^{-1} mod m1 ) m0  and res <  m0m1
			}
#ifdef _LB_CRATIMING
			tIRecon.stop();
			totalTime.ttIRecon += tIRecon;
#endif
		}

		virtual void progress (const Domain& D, const DomainElement& e)
		{
			// Precondition : initialize has been called once before
			// linbox_check(occurency_ > 0);
#ifdef _LB_CRATIMING
			tIRecon.clear();
			tIRecon.start();
            ++IterCounter_;
#endif
			primeProd_ *= nextM_;
			D.characteristic( nextM_ );

			DomainElement u0;
			if (D.areEqual( D.init(u0, residue_), e)) {
				++occurency_;
			}
			else {
				occurency_ = 1;

				D.negin(u0);       	// u0  <-- -u0
				D.addin(u0, e);    	// u0  <-- e-u0

				DomainElement m0;
				D.init(m0, primeProd_);
				D.invin(m0);       	// m0  <-- m0^{-1} mod nextM_
				D.mulin(u0, m0);   	// u0  <-- (e-u0)( m0^{-1} mod nextM_ )

				Integer res;
				D.convert(res, u0);	// res <-- (e-u0)( m0^{-1} mod nextM_ )
				// and res < nextM_

				Integer tmp(res);
				tmp -= nextM_;
				if (absCompare(res,tmp)>0) res = tmp; // Normalize

				res *= primeProd_;	// res <-- (e-u0)( m0^{-1} mod nextM_ ) m0
				// and res <= (m0.nextM_-m0)

				residue_ += res;	// <-- u0 + (e-u0)( m0^{-1} mod nextM_ ) m0
				// and res <  m0.nextM_
			}
#ifdef _LB_CRATIMING
			tIRecon.stop();
			totalTime.ttIRecon += tIRecon;
#endif
		}

		virtual bool noncoprime(const Integer& i) const
		{
			Integer g;
			return ( (gcd(g, i, nextM_) != 1) || (gcd(g, i, primeProd_) != 1) );
		}


		virtual ~EarlySingleCRA() {}

#ifdef _LB_CRATIMING
		void clearTimers() const
		{
			tInit.clear();
			//tIteration.clear();
			//tImaging.clear();
			tIRecon.clear();
			tOther.clear();
		}
	public:
		virtual inline std::ostream& printTime(const Timer& timer, const char* title, std::ostream& os, const char* pref = "") const
		{
			if (timer.count() > 0) {
				os << pref << title;
				for (int i=strlen(title)+strlen(pref); i<28; i++)
					os << ' ';
				return os << timer << std::endl;
			}
			else
				return os;
		}

		virtual inline std::ostream& printCRATime(const CRATimer& timer, const char* title, std::ostream& os) const
		{
			printTime(timer.ttInit, " Init: ", os, title);
			//printTime(timer.ttImaging, "Imaging", os, title);
			//printTime(timer.ttIteration, "Iteration", os, title);
			printTime(timer.ttIRecon, " Integer reconstruction: ", os, title);
			printTime(timer.ttOther, " Other: ", os, title);
			return os;
		}

		virtual std::ostream& reportTimes(std::ostream& os) const
		{
            os <<  "CRA Iterations:" << IterCounter_ << "\n";
			printCRATime(totalTime, "CRA Time", os);
			return os;
		}
#endif

	};


}

#endif //__LINBOX_cra_early_single_H


// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,:0,t0,+0,=s
// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:

