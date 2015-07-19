#ifndef  REDJAGUAR_JS_IREFERENCEABLE_H_
#define  REDJAGUAR_JS_IREFERENCEABLE_H_

class IReferenceable
{
	private:
		unsigned short m_sReferences;

	public:
		IReferenceable();
		IReferenceable(const IReferenceable& _ref);
		virtual IReferenceable& operator= (const IReferenceable& _ref);
		virtual ~IReferenceable();

		virtual void AcquireReference();
		virtual void ReleaseReference();
};

#endif // end of REDJAGUAR_JS_IREFERENCEABLE_H_