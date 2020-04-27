#include "Link.h"

float Link::CurrentLength() const
{
	return XMVectorGetX(XMVector3Length(rigidBody[0]->GetPosition() - rigidBody[1]->GetPosition()));
}
