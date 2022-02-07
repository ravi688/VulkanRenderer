
namespace Showcase
{
	class ISystem
	{
	public:
		virtual void init() = 0;
		virtual void terminate() = 0;
		virtual bool isRunning() const = 0;
	};
}