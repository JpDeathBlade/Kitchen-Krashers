/**
 * 0===========================================================================0
 * | RJ_MathLibrary.h                                                          |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Matthew Walker                                          |
 * |   E-mail        : MattW1989@yahoo.com                                     |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 09:45 am (Eastern) on 06/09/2010                        |
 * |   Date Edited   : 00:00 pm (Eastern) on 00/00/2010                        |
 * 0=====0===============================================================0=====0
 *       |                                                               |      
 * 0=====0===============================================================0=====0
 * | Code Description:                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   The math library that houses important math functions.                  |
 * 0===========================================================================0
 */
#ifndef RJ_MATHLIBRARY_HPP
#define RJ_MATHLIBRARY_HPP

#include <d3d9.h>
#include <d3dx9.h>

namespace rj_mathlib
{
	inline D3DXVECTOR2 zeroVector2() { return D3DXVECTOR2(0.0f, 0.0f); }
	inline D3DXVECTOR3 zeroVector3() { return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	inline D3DXVECTOR4 zeroVector4() { return D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f); }
	inline D3DXVECTOR3 getXAxis(const D3DXMATRIX* in) { return D3DXVECTOR3(in->_11, in->_12, in->_13); }
	inline D3DXVECTOR3 getYAxis(const D3DXMATRIX* in) { return D3DXVECTOR3(in->_21, in->_22, in->_23); }
	inline D3DXVECTOR3 getZAxis(const D3DXMATRIX* in) { return D3DXVECTOR3(in->_31, in->_32, in->_33); }
	inline D3DXVECTOR3 getWAxis(const D3DXMATRIX* in) { return D3DXVECTOR3(in->_41, in->_42, in->_43); }

	inline void setXAxis(D3DXMATRIX* out, const D3DXVECTOR3* in)
	{
		out->_11 = in->x;
		out->_12 = in->y;
		out->_13 = in->z;
	}

	inline void setYAxis(D3DXMATRIX* out, const D3DXVECTOR3* in)
	{
		out->_21 = in->x;
		out->_22 = in->y;
		out->_23 = in->z;
	}

	inline void setZAxis(D3DXMATRIX* out, const D3DXVECTOR3* in)
	{
		out->_31 = in->x;
		out->_32 = in->y;
		out->_33 = in->z;
	}

	inline void setWAxis(D3DXMATRIX* out, const D3DXVECTOR3* in)
	{
		out->_41 = in->x;
		out->_42 = in->y;
		out->_43 = in->z;
	}

	inline D3DXMATRIX* translatePre(D3DXMATRIX* out, const D3DXVECTOR3* in)
	{
		D3DXMATRIX translate;
		D3DXMatrixIdentity(&translate);
		D3DXMatrixTranslation(&translate, in->x, in->y, in->z);
		D3DXMATRIX temp;
		D3DXMatrixMultiply(&temp, &translate, out);
		out = &temp;
		return out;
	}

	inline D3DXMATRIX* translatePre(D3DXMATRIX* out, float x, float y, float z)
	{
		D3DXMATRIX translate;
		D3DXMatrixIdentity(&translate);
		D3DXMatrixTranslation(&translate, x, y, z);
		D3DXMATRIX temp;
		D3DXMatrixMultiply(&temp, &translate, out);
		out = &temp;
		return out;
	}

	inline D3DXMATRIX* translatePost(D3DXMATRIX* out, const D3DXVECTOR3* in)
	{
		D3DXMATRIX translate;
		D3DXMatrixIdentity(&translate);
		D3DXMatrixTranslation(&translate, in->x, in->y, in->z);
		D3DXMATRIX temp;
		D3DXMatrixMultiply(&temp, out, &translate);
		out = &temp;
		return out;
	}

	inline D3DXMATRIX* translatePost(D3DXMATRIX* out, float x, float y, float z)
	{
		D3DXMATRIX translate;
		D3DXMatrixIdentity(&translate);
		D3DXMatrixTranslation(&translate, x, y, z);
		D3DXMATRIX temp;
		D3DXMatrixMultiply(&temp, out, &translate);
		out = &temp;
		return out;
	}

	inline D3DXMATRIX* rotateXPre(D3DXMATRIX* out, float angle)
	{
		D3DXMATRIX rotate;
		D3DXMatrixIdentity(&rotate);
		D3DXMatrixRotationX(&rotate, angle);
		D3DXMATRIX temp;
		D3DXMatrixMultiply(&temp, &rotate, out);
		out = &temp;
		return out;
	}

	inline D3DXMATRIX* rotateYPre(D3DXMATRIX* out, float angle)
	{
		D3DXMATRIX rotate;
		D3DXMatrixIdentity(&rotate);
		D3DXMatrixRotationY(&rotate, angle);
		D3DXMATRIX temp;
		D3DXMatrixMultiply(&temp, &rotate, out);
		out = &temp;
		return out;
	}

	inline D3DXMATRIX* rotateZPre(D3DXMATRIX* out, float angle)
	{
		D3DXMATRIX rotate;
		D3DXMatrixIdentity(&rotate);
		D3DXMatrixRotationZ(&rotate, angle);
		D3DXMATRIX temp;
		D3DXMatrixMultiply(&temp, &rotate, out);
		out = &temp;
		return out;
	}

	inline D3DXMATRIX* rotateXPost(D3DXMATRIX* out, float angle)
	{
		D3DXMATRIX rotate;
		D3DXMatrixIdentity(&rotate);
		D3DXMatrixRotationX(&rotate, angle);
		D3DXMATRIX temp;
		D3DXMatrixMultiply(&temp, out, &rotate);
		out = &temp;
		return out;
	}

	inline D3DXMATRIX* rotateYPost(D3DXMATRIX* out, float angle)
	{
		D3DXMATRIX rotate;
		D3DXMatrixIdentity(&rotate);
		D3DXMatrixRotationY(&rotate, angle);
		D3DXMATRIX temp;
		D3DXMatrixMultiply(&temp, out, &rotate);
		out = &temp;
		return out;
	}

	inline D3DXMATRIX* rotateZPost(D3DXMATRIX* out, float angle)
	{
		D3DXMATRIX rotate;
		D3DXMatrixIdentity(&rotate);
		D3DXMatrixRotationZ(&rotate, angle);
		D3DXMATRIX temp;
		D3DXMatrixMultiply(&temp, out, &rotate);
		out = &temp;
		return out;
	}

	inline D3DXVECTOR3 vecMult3x3Mat(D3DXVECTOR3 vec, D3DXVECTOR3* mat)
	{
		D3DXVECTOR3 out;
		out.x = (vec.x * mat[0].x) + (vec.y * mat[1].x) + (vec.z * mat[2].x);
		out.y = (vec.x * mat[0].y) + (vec.y * mat[1].y) + (vec.z * mat[2].y);
		out.z = (vec.x * mat[0].z) + (vec.y * mat[1].z) + (vec.z * mat[2].z);
		return out;
	}

	// Multiplies a Vec3 by a float[4][4]
	//	ignores the 4th row and column
	inline D3DXVECTOR3 vecMult3x3Mat(D3DXVECTOR3 vec, float mat[4][4])
	{
		D3DXVECTOR3 out;
		out.x = (vec.x * mat[0][0]) + (vec.y * mat[1][0]) + (vec.z * mat[2][0]);
		out.y = (vec.x * mat[0][1]) + (vec.y * mat[1][1]) + (vec.z * mat[2][1]);
		out.z = (vec.x * mat[0][2]) + (vec.y * mat[1][2]) + (vec.z * mat[2][2]);
		return out;
	}

	inline D3DXVECTOR3 vecMult3x3Mat(D3DXVECTOR3& v, D3DXMATRIX& m)
	{
		D3DXVECTOR3 newVec;

		newVec.x = m._11*v.x + m._12*v.y + m._13*v.z;
		newVec.y = m._21*v.x + m._22*v.y + m._23*v.z;
		newVec.z = m._31*v.x + m._32*v.y + m._33*v.z;

		return newVec;
	}

	inline float degToRad(float degree)
	{
		return ((D3DX_PI / 180.0f) * (degree));
	}
}

#endif