#ifndef POSTA_RESOURCEBAG_UTILITY_H
#define POSTA_RESOURCEBAG_UTILITY_H
#include <vector>
#include <memory>

namespace posta {
	/// Use this class to hold all the assets in your project.
	/// For example: textures, meshes, rigidbodies, etc.
	/// When used, make sure to add all of your ResourceBags in the posta::App::resource_bag so the application is able to
	/// delete all the resources in the bags, this also implies that your resource bags are own by posta::App and not by you
	class ResourceBag
	{
	public:
		virtual ~ResourceBag() = default;

		/// Adds the bag to the array of bags and returns it.
		/// One way of using this function is by having a class called (for example) 'Resources' inside of one of your class
		/// and have a static pointer to the Resources class, that pointer is then created in the constructor of your application
		/// and the returning value is assigned to that static pointer.
		virtual ResourceBag* add_bag(ResourceBag* resource_bag) final;

		std::vector<std::unique_ptr<ResourceBag>> resource_bags;
	};
};

#endif // POSTA_RESOURCEBAG_UTILITY_H
