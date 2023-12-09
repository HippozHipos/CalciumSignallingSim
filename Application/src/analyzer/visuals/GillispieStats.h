#include "../Simulator.h"
#include "Cyclopes.h"

class GillispieStats : public cyc::Layer
{
public:
	GillispieStats(const csim::Simulator* const sim);
	void OnImGuiRender() override;

private:
	const csim::Simulator* const sim;
};