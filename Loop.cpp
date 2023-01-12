#pragma once

namespace UIMiniEmbed {
	
	uint32_t g_TimeMsec = 0;
	void loop_Update() {
		g_TimeMsec = GetTickCount();
	}
	uint32_t loop_GetTime() { return g_TimeMsec; }
	
}