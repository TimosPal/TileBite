#include <iostream>

#include <resources/ResourceManager.hpp>
#include <resources/Resource.hpp>

class ResourceT : public Engine::Resource<ResourceT>
{ 
public:
	ResourceT() {}
protected:
	bool createImplementation() override { return true; }
	bool destroyImplementation() override { return false; }
};

int main()
{
	Engine::ResourceManager<ResourceT> manager;
	manager.addResource("test", ResourceT());

	auto t = manager.getResource("test");
	t.isValid();

	return 0;
}
