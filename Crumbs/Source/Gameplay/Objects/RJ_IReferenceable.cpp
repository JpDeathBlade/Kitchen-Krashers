#include "RJ_IReferenceable.h"

IReferenceable::IReferenceable()
{
	m_sReferences = 0;
}

IReferenceable::IReferenceable(const IReferenceable& _ref)
{
	m_sReferences = 0;
}

IReferenceable& IReferenceable::operator= (const IReferenceable& _ref)
{
	m_sReferences = 0;
	return *this;
}

IReferenceable::~IReferenceable()
{

}

void IReferenceable::AcquireReference()
{
	m_sReferences++;
}

void IReferenceable::ReleaseReference()
{
	if(m_sReferences <= 1)
		delete this;
	else
		m_sReferences--;
}