#ifndef IFRAME_H_
#define IFRAME_H_

#include <vector>
#include "../../Utilities/RJ_MathLibrary.hpp"

class IFrame
{
	protected:
		D3DXMATRIX m_LocalMatrix;
		D3DXMATRIX m_WorldMatrix;

		unsigned int m_uiFlags;

		IFrame* m_pParent;
		std::vector< IFrame* > m_vpChildren;

	public:
		enum { FRAME_CLEAN = 0, FRAME_DIRTY = 1 };

		IFrame(void) : m_uiFlags(0), m_pParent(NULL) { }
		virtual ~IFrame(void) { }

		IFrame* GetParent(void) { return m_pParent; }
		void SetParent( IFrame* pFrame ) { m_pParent = pFrame; }

		void AddChild( IFrame* pFrame )
		{
			m_vpChildren.push_back( pFrame );
		}

		IFrame *RemoveChild( IFrame* pFrame )
		{
			IFrame* pTemp = NULL;

			for( unsigned int i = 0; i < m_vpChildren.size(); ++i )
			{
				if( pFrame == m_vpChildren[i] )
				{
					pTemp = m_vpChildren[i];
					m_vpChildren.erase( m_vpChildren.begin() + i );
					break;
				}
			}

			return pTemp;
		}

		void Update(void)
		{
			if( (m_uiFlags & FRAME_DIRTY) == 0 )
			{
				m_uiFlags |= FRAME_DIRTY;
				for( unsigned int i = 0; i < m_vpChildren.size(); ++i )
					m_vpChildren[i]->Update();
			}
		}

		//D3DXMATRIX GetLocalMat(void) { return &m_LocalMatrix; }

		const D3DXMATRIX& GetWorldMat(void)
		{
			if( (m_uiFlags & FRAME_DIRTY) != 0 )
			{
				//if( m_pParent != NULL )
				//	//Multiply( m_WorldMatrix, m_LocalMatrix, m_pParent->m_WorldMatrix() );
				//else
				//	m_WorldMatrix = m_LocalMatrix;

				m_uiFlags &= ~FRAME_DIRTY;
			}

			return m_WorldMatrix;
		}

		void SetWorldMat(float* indices)
		{
			m_WorldMatrix._11 = indices[0];
			m_WorldMatrix._12 = indices[1];
			m_WorldMatrix._13 = indices[2];
			m_WorldMatrix._14 = indices[3];
			m_WorldMatrix._21 = indices[4];
			m_WorldMatrix._22 = indices[5];
			m_WorldMatrix._23 = indices[6];
			m_WorldMatrix._24 = indices[7];
			m_WorldMatrix._31 = indices[8];
			m_WorldMatrix._32 = indices[9];
			m_WorldMatrix._33 = indices[10];
			m_WorldMatrix._34 = indices[11];
			m_WorldMatrix._41 = indices[12];
			m_WorldMatrix._42 = indices[13];
			m_WorldMatrix._43 = indices[14];
			m_WorldMatrix._44 = indices[15];
		}
};

#endif