// =================================================================== //
// SparseElimination rank calls
// Time-stamp: <11 Sep 08 13:26:46 Jean-Guillaume.Dumas@imag.fr> 
// =================================================================== //
#ifndef __GAUSS_RANK_INL
#define __GAUSS_RANK_INL

namespace LinBox 
{
    template <class _Field>
    template <class Matrix> unsigned long& 
    GaussDomain<_Field>::rankin(unsigned long &rank,
                                Matrix        &A,
                                unsigned long  Ni,
                                unsigned long  Nj,
                                SparseEliminationTraits::PivotStrategy   reord) 
    {
        Element determinant;
        if (reord == SparseEliminationTraits::PIVOT_NONE)
            return NoReordering(rank, determinant, A,  Ni, Nj);
        else
            return InPlaceLinearPivoting(rank, determinant, A,  Ni, Nj);
    }

   
    template <class _Field>
    template <class Matrix> unsigned long& 
    GaussDomain<_Field>::rankin(unsigned long &rank,
                                Matrix        &A,
                                SparseEliminationTraits::PivotStrategy   reord) 
    {
        return rankin(rank, A,  A.rowdim (), A.coldim (), reord);
    }

   

    template <class _Field>
    template <class Matrix> unsigned long& 
    GaussDomain<_Field>::rank(unsigned long &rank,
                              const Matrix        &A,
                              SparseEliminationTraits::PivotStrategy   reord) 
    {
        return rank(rank, A,  A.rowdim (), A.coldim (), reord);
    }

    template <class _Field>
    template <class Matrix> unsigned long& 
    GaussDomain<_Field>::rank(unsigned long &rank,
                              const Matrix        &A,
                              unsigned long  Ni,
                              unsigned long  Nj,
                              SparseEliminationTraits::PivotStrategy   reord) 
    {
        Matrix CopyA(Ni);
        for(unsigned long i = 0; i < Ni; ++i)
            CopyA[i] = A[i];
        return rankin(rank, CopyA, Ni, Nj, reord);
    }
} // namespace LinBox

#endif // __GAUSS_INL