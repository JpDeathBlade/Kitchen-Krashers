#ifndef ISCENEOBJECT_H_
#define ISCENEOBJECT_H_

#include "IObject.h"

/*/////////////////////////////////////////
* NOTE: this isn't an abstract base class,
*	so why is it "ISceneObject"?
*								- Andrew
/////////////////////////////////////////*/

class ISceneObject : public IObject
{
public:
	void Render(double fDelta);
};

#endif