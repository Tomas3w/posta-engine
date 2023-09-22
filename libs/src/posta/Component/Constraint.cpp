#include "posta/App.h"
#include <posta/Component/Constraint.h>

using Constraint = posta::component::Constraint;
using Rigidbody = posta::component::Rigidbody;

Constraint::Constraint(btTypedConstraint* _constraint)
{
	constraint = _constraint;
	posta::App::app->physics->world->addConstraint(constraint);
}

Constraint::~Constraint()
{
	self_destruct();
}

void Constraint::add_rigidbody(Rigidbody* rigidbody)
{
	rigidbody->add_constraint(this);
	rigidbodies.push_back(rigidbody);
}

void Constraint::self_destruct()
{
	posta::App::app->physics->world->removeConstraint(constraint);
	delete constraint;
	constraint = nullptr;

	// Removing this from all the rigidbody it has been associated with
	for (auto body : rigidbodies)
		body->remove_constraint(this);
	rigidbodies.clear();
}

void Constraint::remove_rigidbodies()
{
	self_destruct();
}


