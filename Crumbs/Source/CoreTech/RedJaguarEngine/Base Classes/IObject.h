/**
 * 0===========================================================================0
 * | RJ_IObject.h                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Matthew Walker                                          |
 * |   E-mail        : MattW1989@yahoo.com                                     |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 11:00 am (Eastern) on 06/14/2010                        |
 * |   Date Edited   : 00:00 pm (Eastern) on 00/00/2010                        |
 * 0=====0===============================================================0=====0
 *       |                                                               |      
 * 0=====0===============================================================0=====0
 * | Code Description:                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Is object                                                               |
 * 0===========================================================================0
 */
#ifndef IOBJECT_H_
#define IOBJECT_H_
#include "IFrame.h"
#include "../../../RJ_Config.h"
#include "../Scene/ObjectID.h"

class IObject : public IFrame
{
	protected:
		D3DXVECTOR3 m_vCenter;
		bool m_bTransparent;
		float m_fDistance;
		char m_szName[64];
		RJ_Config::RJ_Object_Types m_eObjectType;
		bool m_bActive;
		unsigned int m_uiUniqueID;
		unsigned int m_uiGlobalID;
		float m_fRandomAdditive;

	public:
		int m_nRef;

		IObject(bool bTransparent = false, D3DXVECTOR3 vCenter = D3DXVECTOR3(1.0f, 1.0f, 1.0f), float fDistance = 0.0f,char* szName = "default", RJ_Config::RJ_Object_Types eObjectType = RJ_Config::RJ_BASE, 
			bool bActive = true, unsigned int uiUniqueID = 0)
		{
			m_bTransparent = bTransparent;
			m_vCenter = vCenter;
			m_fDistance = fDistance;
			setName(szName);
			m_eObjectType = eObjectType;
			m_bActive = bActive;
			m_uiUniqueID = uiUniqueID;
			m_uiGlobalID = ObjectID::GetInstanceUpdate()->CheckOutID();

			m_fRandomAdditive = (rand()%100) / 50.0f;
		};
		virtual ~IObject(void) {ObjectID::GetInstanceUpdate()->ReturnID(m_uiGlobalID);}

		bool GetIsActive()					{ return m_bActive; }
		void SetIsActive(bool b)			{ m_bActive = b; }
		void SetID(unsigned int id)			{ m_uiUniqueID = id; }
		void SetCenter(D3DXVECTOR3& c)		{ m_vCenter = c; }

		unsigned int GetGlobalID()				{ return m_uiGlobalID; }
		void SetGlobalID(unsigned int id)		{ m_uiGlobalID = id; }

		virtual void Initialize() {}
		virtual void Shutdown() {}
		virtual void Update(double fDelta) {};
		virtual void Render(double fDelta) = 0;

		unsigned int GetID()		{ return m_uiUniqueID; }
		float GetRandomAdditive()	{ return m_fRandomAdditive; }
		const char* getName()		{ return m_szName; }
		const bool isTransparent()	{ return m_bTransparent; }
		const float getDistance()	{ return m_fDistance; }
		D3DXVECTOR3 getCenter() { return m_vCenter; }

		void setName(char* _name)
		{
			unsigned int i = 0;
			while ( _name[i] != 0 )
			{
				m_szName[i] = _name[i];
				i++;
			}
			m_szName[i] = 0;
		}
		void setTransparent(bool _isTransparent) { m_bTransparent = _isTransparent; }
		void setDistance(float _distance) { m_fDistance = _distance; }

		RJ_Config::RJ_Object_Types GetObjectType() { return m_eObjectType; }
		void SetObjectType( RJ_Config::RJ_Object_Types e) { m_eObjectType = e; }
};

#endif