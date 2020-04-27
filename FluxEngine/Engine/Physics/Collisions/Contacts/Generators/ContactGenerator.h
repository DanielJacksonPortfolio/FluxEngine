#pragma once
#include "../Contact.h"

class ContactGenerator
{
public:
	virtual unsigned AddContact(Contact* contact, unsigned limit) const = 0;
private:
};

