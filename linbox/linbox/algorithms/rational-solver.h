/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* linbox/algorithms/lifting-container.h
 * Copyright (C) 2004 Zhendong Wan, Pascal Giorgi
 *
 * Written by Zhendong Wan  <wan@mail.eecis.udel.edu> 
 *         and Pascal Giorgi <pascal.giorgi@ens-lyon.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __LINBOX_RATIONAL_SOLVER_H
#define __LINBOX_RATIONAL_SOLVER_H


#include <linbox/solutions/methods.h>
#include <linbox/blackbox/archetype.h>
#include <linbox/blackbox/lambda-sparse.h>
#include <linbox/blackbox/compose.h>
#define DEFAULT_PRIMESIZE 14

namespace LinBox {
	
#define SINGULARITY_THRESHOLD 5
#define BAD_PRECONTITIONER_THRESHOLD 5
#define DEFAULT_MAXPRIMES 5
	
	/** _Ring integer ring
	 *  _Field, finite field for lifting
	 */

 	template<class Ring, class Field,class RandomPrime, class MethodTraits = DixonTraits>		
 	class RationalSolver {};


	/* RationalSolver for linears systems over a Ring
	 * using p-adic lifting and Wiedemann algorithm.
	 */
	template<class Ring, class Field,class RandomPrime>		
	class RationalSolver<Ring, Field, RandomPrime, WiedemannTraits> {
	
	public:          

		typedef typename Ring::Element                Integer;
		typedef typename Field::Element               Element;
		typedef typename RandomPrime::Prime_Type        Prime;
		typedef std::vector<Element>              FPolynomial;

	protected:
		
		Ring                    _R;
		RandomPrime      _genprime;
		mutable Prime       _prime;
		WiedemannTraits    _traits;
    	
	public:

		enum ReturnStatus {
			OK, FAILED, SINGULAR, INCONSISTENT, BAD_PRECONDITIONER
		};
    
		/* Constructor
		 * @param r   , a Ring, set by default
		 * @param rp  , a RandomPrime generator, set by default		 
		 */
		RationalSolver (const Ring& r = Ring(), const RandomPrime& rp = RandomPrime(DEFAULT_PRIMESIZE), const WiedemannTraits& traits=WiedemannTraits()) : 
			_R(r), _genprime(rp), _traits(traits){_prime=_genprime.randomPrime();}
    
		/* Constructor with a prime
		 * @param p   , a Prime
		 * @param r   , a Ring, set by default
		 * @param rp  , a RandomPrime generator, set by default		 
		 */
		RationalSolver (const Prime& p, const Ring& r = Ring(), const RandomPrime& rp = RandomPrime(DEFAULT_PRIMESIZE), const WiedemannTraits& traits=WiedemannTraits()) : 
			_R(r), _genprime(rp), _prime(p), _traits(traits){}
    
		/** Solve a linear system Ax=b over quotient field of a ring		 
		 * giving a random solution if the system is singular and consistent.
		 * giving the unique solution if the system is non-singular.
		 *
		 * @param A    , Matrix of linear system
		 * @param x    , Vector in which to store solution
		 * @param b    , Right-hand side of system
		 * @param maxPrimes , maximum number of moduli to try
		 *
		 * @return status of solution
		 */
		template<class IMatrix, class Vector1, class Vector2>
		ReturnStatus solve(Vector1& answer, const IMatrix& A, const Vector2& b,const bool, int maxPrimes = DEFAULT_MAXPRIMES) const;
    
		/** Solve a nonsingular linear system Ax=b over quotient field of a ring.
		 * giving the unique solution of the system.
		 *
		 * @param A   , Matrix of linear system
		 * @param x   , Vector in which to store solution
		 * @param b   , Right-hand side of system
		 * @param maxPrimes , maximum number of moduli to try
		 *
		 * @return status of solution
		 */
		template<class IMatrix, class Vector1, class Vector2>
		ReturnStatus solveNonsingular(Vector1& answer, const IMatrix& A, const Vector2& b, int maxPrimes = DEFAULT_MAXPRIMES) const;         

		/** Solve a singular linear system Ax=b over quotient field of a ring.
		 * giving a random solution if the system is singular and consistent.
		 *
		 * @param A   , Matrix of linear system
		 * @param x   , Vector in which to store solution
		 * @param b   , Right-hand side of system
		 * @param maxPrimes , maximum number of moduli to try
		 *
		 * @return status of solution
		 */	
		template<class IMatrix, class Vector1, class Vector2>
		ReturnStatus solveSingular(Vector1& answer, const IMatrix& A, const Vector2& b, int maxPrimes = DEFAULT_MAXPRIMES) const;	


		template <class IMatrix, class FMatrix, class IVector>
		void sparseprecondition (const Field&,
					 const IMatrix* ,
					 Compose< LambdaSparseMatrix<Ring>,Compose<IMatrix, LambdaSparseMatrix<Ring> > > *&,
					 const FMatrix*,
					 Compose<LambdaSparseMatrix<Field>,Compose<FMatrix,LambdaSparseMatrix<Field> > > *&,
					 const IVector&,
					 IVector&,
					 LambdaSparseMatrix<Ring> *&,
					 LambdaSparseMatrix<Ring> *&,
					 LambdaSparseMatrix<Field> *&,
					 LambdaSparseMatrix<Field> *&) const;


 
		template <class IMatrix, class FMatrix, class IVector, class FVector>
		void precondition (const Field&,
				   const IMatrix&,
				   BlackboxArchetype<IVector>*&,
				   const FMatrix*,
				   BlackboxArchetype<FVector>*&,
				   const IVector&,				   
				   IVector&,
				   BlackboxArchetype<IVector>*&,
				   BlackboxArchetype<IVector>*&) const; 
			

	}; // end of specialization for the class RationalSover with Wiedemann traits


	/* RationalSolver for linears systems over a Ring
	 * using p-adic lifting and Dixon algorithm.
	 */
	template<class Ring, class Field,class RandomPrime>		
	class RationalSolver<Ring, Field, RandomPrime, DixonTraits> {
	
	public:          
		
		typedef typename Ring::Element             Integer;
		typedef typename Field::Element             Element;
		typedef typename RandomPrime::Prime_Type     Prime;
				
	protected:
		
		Ring                    _R;
		RandomPrime      _genprime;
		mutable Prime       _prime;
		
	public:

		enum ReturnStatus {
			OK, FAILED, SINGULAR, INCONSISTENT, BAD_PRECONDITIONER
		};
    
		/* Constructor
		 * @param r   , a Ring, set by default
		 * @param rp  , a RandomPrime generator, set by default		 
		 */
		
		RationalSolver (const Ring& r = Ring(), const RandomPrime& rp = RandomPrime(DEFAULT_PRIMESIZE)) : 
			_R(r), _genprime(rp) {_prime=_genprime.randomPrime();}
    
		
		/* Constructor with a prime
		 * @param p   , a Prime
		 * @param r   , a Ring, set by default
		 * @param rp  , a RandomPrime generator, set by default		 
		 */
		RationalSolver (const Prime& p, const Ring& r = Ring(), const RandomPrime& rp = RandomPrime(DEFAULT_PRIMESIZE)) : 
			_R(r), _genprime(rp), _prime(p) {}
    
		
		/** Solve a linear system Ax=b over quotient field of a ring
		 * giving a random solution if the system is singular and consistent.
		 * giving the unique solution if the system is non-singular.
		 *
		 * @param A    , Matrix of linear system
		 * @param x    , Vector in which to store solution
		 * @param b    , Right-hand side of system
		 * @param maxPrimes , maximum number of moduli to try
		 *
		 * @return status of solution. OK means the solution is 
		 *   certifiably OK, while 'inconsistent' means that the
		 *   system was inconsistent for at least one choice of prime
		 */
		template<class IMatrix, class Vector1, class Vector2>
		ReturnStatus solve(Vector1& x, const IMatrix& A, const Vector2& b,const bool = false, int maxPrimes = DEFAULT_MAXPRIMES) const;
		
		template <class IMatrix, class Vector1, class Vector2>	
		typename RationalSolver<Ring,Field,RandomPrime,DixonTraits>::ReturnStatus 
		RationalSolver<Ring,Field,RandomPrime,DixonTraits>::solve (Vector1& answer,
									   const IMatrix& A,
									   const Vector2& b,
									   int maxPrimes) const {
			return solve (answer, A, b, false, maxPrimes);
		}


		/** Solve a nonsingular linear system Ax=b over quotient field of a ring.
		 * giving the unique solution of the system.
		 *
		 * @param A   , Matrix of linear system
		 * @param x   , Vector in which to store solution
		 * @param b   , Right-hand side of system
		 * @param maxPrimes , maximum number of moduli to try
		 *
		 * @return status of solution
		 */
		template<class IMatrix, class Vector1, class Vector2>
		ReturnStatus solveNonsingular(Vector1& x, const IMatrix& A, const Vector2& b, bool, int maxPrimes = DEFAULT_MAXPRIMES) const;         

		/** Solve a singular linear system Ax=b over quotient field of a ring.
		 * giving a solution if the system is singular and consistent.
		 *
		 * @param A   , Matrix of linear system
		 * @param x   , Vector in which to store solution
		 * @param b   , Right-hand side of system
		 * @param maxPrimes , maximum number of moduli to try
		 *
		 * @return status of solution
		 */	
		template<class IMatrix, class Vector1, class Vector2>
		ReturnStatus solveSingular(Vector1& x, const IMatrix& A, const Vector2& b, int maxPrimes = DEFAULT_MAXPRIMES) const;



		/** find a random solution of the linear system Ax=b over quotient field of a ring.
		 * giving a random solution if the system is singular and consistent.
		 *
		 * @param A   , Matrix of linear system
		 * @param x   , Vector in which to store solution
		 * @param b   , Right-hand side of system
		 * @param maxPrimes , maximum number of moduli to try
		 *
		 * @return status of solution
		 */	
		template<class IMatrix, class Vector1, class Vector2>
		ReturnStatus findRandomSolution(Vector1& x, const IMatrix& A, const Vector2& b, int maxPrimes = DEFAULT_MAXPRIMES) const;
		
		

	}; // end of specialization for the class RationalSover with Dixon traits



}
#include <linbox/algorithms/rational-solver.inl>

#endif
