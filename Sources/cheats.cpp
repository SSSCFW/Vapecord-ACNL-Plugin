#include "cheats.hpp"
#include "Files.h"
#include "Helpers/PluginMenuData.hpp"
#include "Helpers/Game.hpp"
#include "RegionCodes.hpp"
#include "Color.h"
#include "csvc.h"

namespace CTRPluginFramework {
	bool turbo = false;
	bool fovbool = false;
	bool save = false;

	const std::string g_Empty = "-Empty-";

	const Color pColor[4] = {
		Color(pBlue), Color(pRed),		
		Color(pGreen), Color(pYellow),	
	};

	bool RainBowON = false;

/*
Randomizes colors of Menu Folders
*/
	void RainbowEntrys(Time time) {
		static Clock timer(time);
		if(timer.HasTimePassed(Milliseconds(500))) {  
			Color color[12] = {
				Color::Rainbow(), Color::Rainbow(), Color::Rainbow(), Color::Rainbow(),
				Color::Rainbow(), Color::Rainbow(), Color::Rainbow(), Color::Rainbow(),
				Color::Rainbow(), Color::Rainbow(), Color::Rainbow(), Color::Rainbow()
			};
			
			PluginMenuData::UpdateAll(color);
			timer.Restart();
		}
	}

	void OnNewFrameCallback(Time ttime) {
		SendPlayerData(ttime);

		if(PluginMenu::GetRunningInstance()->RainbowState())
			RainbowEntrys(ttime);
	}

	
	static u32 *socBuffer;
    constexpr u32 SOC_BUFFER_ADDR = 0x7500000;
    constexpr u32 SOC_BUFFER_SIZE = 0x100000;

	void soc_memory_init(void) {
		Result ret = 0;
		if (System::IsCitra())
		{
			socBuffer = (u32 *)aligned_alloc(0x1000, SOC_BUFFER_SIZE);
			if (socBuffer)
				ret = 0;
		}
		else
			ret = svcControlMemoryUnsafe((u32 *)&socBuffer, SOC_BUFFER_ADDR, SOC_BUFFER_SIZE, MemOp(MEMOP_REGION_SYSTEM | MEMOP_ALLOC), MemPerm(MEMPERM_READ | MEMPERM_WRITE));
		if (R_FAILED(ret))
			OSD::Notify("failed alloc");
		else
		{
			ret = socInit(socBuffer, SOC_BUFFER_SIZE);
			if (R_FAILED(ret))
			{
				OSD::Notify(Utils::Format("socInit: 0x%lX", ret));
				socExit();
				svcControlMemoryUnsafe((u32 *)&socBuffer, SOC_BUFFER_ADDR, SOC_BUFFER_SIZE, MEMOP_FREE, MemPerm(0));
			}
			else
				OSD::Notify("socInit success");
		}
	}

	void soc_memory(MenuEntry *entry) {
		soc_memory_init();
	}

	void soc_memory_exit(MenuEntry *entry) {
		OSD::Notify(Utils::Format("socInit: exit"));
		socExit();
		svcControlMemoryUnsafe((u32 *)&socBuffer, SOC_BUFFER_ADDR, SOC_BUFFER_SIZE, MEMOP_FREE, MemPerm(0));
	}
}