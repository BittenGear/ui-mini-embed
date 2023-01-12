#pragma once

namespace UIMiniEmbed {
	
	class UIComponent_If : public UIComponentContainerTransparent {
		private:	
			UIVar _flag;
			bool  _flagReal = false;
		
		protected:
			virtual void _init_VarLink() override {
				_flag = getVar("flag");
			}

			virtual void _update_ChildNodeList(UIComponentList& outList,  SP_UINodeDesc& spNodeDesc) override {
				if ( _flag.getBool() == _flagReal ) return;
				_flagReal = _flag.getBool();

				outList.clear();
				if ( _flagReal )
					for(auto spChildNodeDesc : spNodeDesc->getChildNodesRef())
						outList.push_back( createChildNode( spChildNodeDesc ) );
			}
	};
	class UIComponent_IfNot : public UIComponentContainerTransparent {
		private:	
			UIVar _flag;
			bool  _flagReal = true;
		
		protected:
			virtual void _init_VarLink() override {
				_flag = getVar("flag");
			}

			virtual void _update_ChildNodeList(UIComponentList& outList,  SP_UINodeDesc& spNodeDesc) override {
				if ( _flag.getBool() == _flagReal ) return;
				_flagReal = _flag.getBool();

				outList.clear();
				if ( !_flagReal )
					for(auto spChildNodeDesc : spNodeDesc->getChildNodesRef())
						outList.push_back( createChildNode( spChildNodeDesc ) );
			}
	};

	class UIComponent_Each : public UIComponentContainerTransparent {
		private:	
			UIVar _list;
		
		protected:
			virtual void _init_VarLink() override {
				_list = getVar("list");
			}

			virtual void _update_ChildNodeList(UIComponentList& outList, SP_UINodeDesc& spNodeDesc) override {
				if ( !_list.readInvalidate() ) return;

				outList.clear();
				for(size_t i = 0; i != _list.list_GetSize(); i++) {
					auto varEnv = createChildVarEnv();
					auto item = varEnv->getVar("item");
					item.set( _list.list_Get(i) );
					for(auto spChildNodeDesc : spNodeDesc->getChildNodesRef())
						outList.push_back( createChildNode( spChildNodeDesc, varEnv ) );
				}
			}
	};

	class UIComponent_Cmp : public UIComponentContainerTransparent {
		private:	
			UIVar _in1;
			UIVar _in2;
			UIVar _cmp;
			UIVar _out;
			
			enum EnumCmpState {
				Equal,
				NotEqual,
				Less,
				LessEqual,
				Greater,
				GreaterEqual,
			};
			
			EnumCmpState _cmpState = Equal;
		
		protected:
			virtual void _init_VarLink() override {
				_in1 = getVar("in1");
				_in2 = getVar("in2");
				_cmp = getVar("cmp");
				_out = getVar("out");
			}
			
			virtual void _update_State() override {
				if ( _cmp.readInvalidate() ) {
					_cmpState = Equal;
					if ( _cmp.getString() == "!=" ) _cmpState = NotEqual;
					if ( _cmp.getString() == "<"  ) _cmpState = Less;
					if ( _cmp.getString() == "<=" ) _cmpState = LessEqual;
					if ( _cmp.getString() == "> " ) _cmpState = Greater;
					if ( _cmp.getString() == ">=" ) _cmpState = GreaterEqual;
				}
			
				switch( _cmpState ) {
					case Equal   : _out.setBool(  _in1.compare(_in2) ); break;
					case NotEqual: _out.setBool( !_in1.compare(_in2) ); break;
					
					case Less        : _out.setBool( _in1.getFloat() <  _in2.getFloat() ); break;
					case LessEqual   : _out.setBool( _in1.getFloat() <= _in2.getFloat() ); break;
					case Greater     : _out.setBool( _in1.getFloat() >  _in2.getFloat() ); break;
					case GreaterEqual: _out.setBool( _in1.getFloat() >= _in2.getFloat() ); break;
				}
			}
	};
	
	class UIComponent_CmpI32 : public UIComponentContainerTransparent {
		private:	
			UIVar _in1;
			UIVar _in2;
			UIVar _cmp;
			UIVar _out;
			
			enum EnumCmpState {
				Equal,
				NotEqual,
				Less,
				LessEqual,
				Greater,
				GreaterEqual,
			};
			
			EnumCmpState _cmpState = Equal;
		
		protected:
			virtual void _init_VarLink() override {
				_in1 = getVar("in1");
				_in2 = getVar("in2");
				_cmp = getVar("cmp");
				_out = getVar("out");
			}
			
			virtual void _update_State() override {
				if ( _cmp.readInvalidate() ) {
					_cmpState = Equal;
					if ( _cmp.getString() == "!=" ) _cmpState = NotEqual;
					if ( _cmp.getString() == "<"  ) _cmpState = Less;
					if ( _cmp.getString() == "<=" ) _cmpState = LessEqual;
					if ( _cmp.getString() == "> " ) _cmpState = Greater;
					if ( _cmp.getString() == ">=" ) _cmpState = GreaterEqual;
				}
			
				switch( _cmpState ) {
					case Equal   : _out.setBool( _in1.getI32() == _in2.getI32() ); break;
					case NotEqual: _out.setBool( _in1.getI32() != _in2.getI32() ); break;
					
					case Less        : _out.setBool( _in1.getI32() <  _in2.getI32() ); break;
					case LessEqual   : _out.setBool( _in1.getI32() <= _in2.getI32() ); break;
					case Greater     : _out.setBool( _in1.getI32() >  _in2.getI32() ); break;
					case GreaterEqual: _out.setBool( _in1.getI32() >= _in2.getI32() ); break;
				}
			}
	};
	
	class UIComponent_Set : public UIComponentContainerTransparent {
		private:	
			UIVar _in;
			UIVar _out;

		protected:
			virtual void _init_VarLink() override {
				_in  = getVar("in");
				_out = getVar("out");
			}
			
			virtual void _update_State() override {
				//if ( !_in.readInvalidate() && !_out.readInvalidate() )
				//	return;
				
				_out.setValue(_in);
			}
	};
	
	class UIComponent_Not : public UIComponentContainerTransparent {
		private:	
			UIVar _in;
			UIVar _out;

		protected:
			virtual void _init_VarLink() override {
				_in  = getVar("in");
				_out = getVar("out");
			}
			
			virtual void _update_State() override {
				_out.setBool( !_in.getBool() );
			}
	};

	class UIComponent_Once : public UIComponentContainerTransparent {
		private:	
			bool _once = true;

		protected:
			virtual void _update_ChildNodeList(UIComponentList& outList,  SP_UINodeDesc& spNodeDesc) override {
				outList.clear();
				
				if ( !_once ) return;
				_once = false;
				
				for(auto spChildNodeDesc : spNodeDesc->getChildNodesRef())
					outList.push_back( createChildNode( spChildNodeDesc ) );
			}
	};

	class UIComponent_Tweened : public UIComponentContainerTransparent {
		private:	
			UIVar _in;
			UIVar _out;
			UIVar _duration;

			float    _prevIn    = 0;
			double   _srcValue  = 0;
			double   _tarValue  = 0;
			double   _curValue  = 0;
	
			uint32_t _startTime = 0;
			uint32_t _endTime   = 0;

		protected:
			virtual void _init_VarLink() override {
				_in       = getVar("in");
				_out      = getVar("out");
				_duration = getVar("duration");

			
				
				_srcValue  = _in.getFloat();
				_tarValue  = _in.getFloat();
				_curValue  = _in.getFloat();
				
				_startTime = loop_GetTime();
				_endTime   = loop_GetTime();
			}

			virtual void _update_State() override {
				const uint32_t now = loop_GetTime();
				
				if ( _prevIn != _in.getFloat() ) {
					_prevIn = _in.getFloat();
					
					_srcValue = _curValue;
					_tarValue = _in.getFloat();
					
					_startTime = now;
					_endTime   = _startTime + ( (uint32_t)clamp(0, _duration.getFloat(), 60*1000) );
					//MessageBox(NULL, "Hello", "Caption", MB_OKCANCEL);
				}
				
				_curValue = _tarValue;
				
				if ( _startTime <= now && now < _endTime ) {
					const uint32_t duration = _endTime - _startTime;
					
					const uint32_t lTime = now - _startTime;
					const uint32_t rTime = _endTime - now;
					
					double frac = 1;
					if ( duration > 0 )
						frac = clampDouble( 0, ((double)lTime) / ((double)duration), 1 );
					
					_curValue = _srcValue + (_tarValue - _srcValue) * frac;
				}

				_out.setFloat( _curValue );
			}
	};


	class UIComponent_Timer : public UIComponentContainerTransparent {
		private:
			UIVar _out;
			UIVar _delay;

			uint32_t _startTime = 0;
			bool     _work = true;

		protected:
			virtual void _init_VarLink() override {
				_out   = getVar("out");
				_delay = getVar("delay");
				
				_startTime = loop_GetTime();
			}

			virtual void _update_State() override {
				if ( !_work )
					return;
				
				const uint32_t now = loop_GetTime();
				
				if ( _startTime + _delay.getI32() <= now ) {
					_out.setBool(true);
					_work = false;
				}
			}
	};


}
