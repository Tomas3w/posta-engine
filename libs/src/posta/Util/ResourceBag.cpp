#include <posta/Util/ResourceBag.h>

using posta::ResourceBag;

ResourceBag* ResourceBag::add_bag(ResourceBag *resource_bag)
{
	resource_bags.emplace_back(resource_bag);
	return resource_bag;
}

