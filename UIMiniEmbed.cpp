#pragma once

#include "Utils.cpp"
#include "Loop.cpp"
#include "UIVar.cpp"
#include "UINodeDesc.cpp"
#include "Parser.cpp"
#include "UIRenderDriverApi.cpp"
#include "UIInputMouse.cpp"
#include "UIStyle.cpp"

#include "BaseComponent/UIComponent.cpp"
#include "Components/MouseLogic.cpp"
#include "Components/Logic.cpp"
#include "Components/TextLine.cpp"
#include "Components/Sprite.cpp"

namespace UIMiniEmbed {

	struct T_createUINode {
		SP_UINodeDesc  spNodeDesc;
		SP_UIComponent node = nullptr;
			
		template< class T >
		void make(const std::string& name) {
			if ( spNodeDesc->getComponentName() == name )
				node = std::make_shared< T >();
		}
	};
	SP_UIComponent createUINode(SP_UINodeDesc spNodeDesc, SP_UIVarEnv spVarEnv, SP_UIComponent spParentNode) {
		if ( !spVarEnv )
			spVarEnv = UIVarEnv::create();
		
		auto un = T_createUINode{ spNodeDesc };
		un.make< UIComponentRoot                 >("Root");
		un.make< UIComponentContainer            >("Container");
		un.make< UIComponentContainerTransparent >("ContainerTransparent");

 
		un.make< UIComponent_TextLineDumpVar      >("TextLineDumpVar");
		un.make< UIComponent_TextLine             >("TextLine");
		un.make< UIComponent_Sprite               >("Sprite");
		un.make< UIComponent_SpriteFrameAnimation >("SpriteFrameAnimation");

		un.make< UIComponent_If                  >("if");
		un.make< UIComponent_IfNot               >("ifnot");
		
		un.make< UIComponent_Each                >("each");
		un.make< UIComponent_Cmp                 >("cmp");
		un.make< UIComponent_CmpI32              >("cmp_i32");
		un.make< UIComponent_Set                 >("set");
		un.make< UIComponent_Not                 >("not");
		un.make< UIComponent_Once                >("once");
		
		un.make< UIComponent_Timer               >("timer");
		un.make< UIComponent_Tweened             >("tweened");
		 
		
		un.make< UIComponent_Hover               >("hover");
		un.make< UIComponent_ActiveL             >("active");
		un.make< UIComponent_ClickL              >("lclick");

		if ( !un.node )
			un.node = std::make_shared< UIComponentContainer >();
		
		un.node->create(spNodeDesc, spVarEnv, spParentNode);
		
		return un.node;
	}
	
}