/* Copyright (C) LinBox
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


#ifndef __LINBOX_toeplitz_det_H
#define __LINBOX_toeplitz_det_H

namespace LinBox
{ // namespace in which all LinBox code resides.

#if 0
	template< class Field >
	void toeplitz_determinant( ZZ_p& res, const Toeplitz<Field>& A );
#endif

	template< class PField >
	typename PField::Coeff& toeplitz_determinant
	( const PField& F, typename PField::Coeff& res,
	  const typename PField::Element& T, size_t n )
	{
		short int sign = 1;
		typename PField::Coeff  temp;
		const typename PField::CoeffField& CField = F.getCoeffField();
		CField.assign(res,CField.one);
		typename PField::Element f1, f2( T ), fi;
		F.setCoeff( f1, 2*n - 1, CField.one );
		F.assign( fi, F.one );

		while( F.deg(f2) >= n ) {
			F.rem( fi, f1, f2 );
			CField.mulin
			( res, CField.powin( F.leadCoeff( temp, f2 ),
					     (long)(F.deg(f1) - F.deg(fi)) ) );
			if( !((F.deg(f2)-F.deg(f1))%2) && !((F.deg(f1)-n)%2) )
				sign = (short)-sign;
			f1 = f2;
			f2 = fi;
		}

		if( F.deg(f2) == (n-1) ) {
			CField.mulin
			( res, CField.powin( F.leadCoeff( temp, f2 ),
					     (long)(F.deg(f1) - F.deg(f2)) ) );
			if( sign == -1 ) {
				CField.mulin( res, CField.mOne);
			}
		}
		else CField.assign( res, CField.zero );

		return res;
	}

} // end of namespace LinBox

#endif //__LINBOX_toeplitz_det_H

// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,:0,t0,+0,=s
// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:

