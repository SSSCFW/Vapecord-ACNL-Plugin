#include "cheats.hpp"
#include "Helpers/GameKeyboard.hpp"
#include "Helpers/Inventory.hpp"

//#include "items.hpp"

namespace CTRPluginFramework
{
	using FuncPointer = void(*);
	
	static bool CAN_USE_COMMAND = true;
	
	bool ChoiceMode;
	int page;

	std::vector<u16> hit_id;
	std::vector<std::string> hit_names;
	
	
	void CanUseCommand(bool use)
	{
		CAN_USE_COMMAND = use;
	}
	
	
	bool _slct_draw(const Screen &scr)
	{
		if ( scr.IsTop ) return false;
		scr.DrawSysfont( Color::Yellow << "Select", 270, 2 );
		
		return true;
	}
	
	std::pair<u16*, u32> get_item_name_addr(u16 item) {
		if( item >= 0x9F && item <= 0xCB ) {
			item += 0x2890;
		}
		else if( item >= 0xCE && item <= 0xF7 ) {
			item += 0x2861;
		}
		
		if( item >= 0x2000 && item <= 0x372B ) {
			u32 addr = 0x31724DC0 + *(u32*)( 0x31724DC0 + 4 + ((item - 0x2000) << 2));

			u32 i = 0;
	
			while( *(u16*)(addr + i) != 0 ) {
				i += 2;
			}
					
			return std::make_pair((u16*)addr, i >> 1);
		}

		return { };
	}

	
	// ---------------------------------------------------------------
	// 選択モードを有効にする
	// ---------------------------------------------------------------
	void EnableChoiceMode( void )
	{
		ChoiceMode = true;
		page = 0;
		GameKeyboard::Delete();
		GameKeyboard::Write( ":" << hit_names[page] );
		OSD::Run( _slct_draw );
	}
	
	
	// ---------------------------------------------------------------
	// 選択モードを無効にする
	// ---------------------------------------------------------------
	void DisableChoiceMode( void )
	{
		hit_id.clear();
		hit_names.clear();
		ChoiceMode = false;
		page = 0;
		OSD::Stop( _slct_draw );
	}
	
  	void find_item(std::string const& text) {

		static auto adjustStr = [] (std::u16string& s) -> void {
			for( auto& c : s ) {
				if( c >= 0x30A1 && c <= 0x30F3 )
				c -= 0x60;
				else if( c >= 0x61 && c <= 0x7A )
				c -= 0x20;
			}
		};

		u16 buf[0x100] { 0 };

		utf8_to_utf16(buf, (u8*)text.c_str(), sizeof(buf));

		std::u16string find = (char16_t*)buf;

		hit_id.clear();
		hit_names.clear();

		adjustStr(find);

		for( u16 id = 0x2000; id <= 0x372B; id++ ) {
			auto [name_addr, len] = get_item_name_addr(id);

			std::u16string name{ (char16_t*)name_addr, len };

			adjustStr(name);

			if( name.find(find) != std::string::npos ) {
				std::string str;
				Process::ReadString((u32)name_addr, str, len * sizeof(u16), StringFormat::Utf16);

				hit_id.emplace_back(id);
				hit_names.emplace_back(str);
			}
		}
		// Load Undefined item
		for(int i = 0; i < UndefItemFileLenght; ++i) {
			if(UndefItemList->Name[i].find(text) != std::string::npos ) {
				hit_id.emplace_back(UndefItemList->ID[i].ID);
				hit_names.emplace_back(UndefItemList->Name[i]);
			}
		}

		if( !hit_id.empty() ) {
			EnableChoiceMode();
		}
  }

	
	
	// ---------------------------------------------------------------
	// コマンドでチート実行
	// ---------------------------------------------------------------
	void TextToCheats(MenuEntry *entry)
	{
		if(!GameKeyboard::IsOpen()){
			return;
		}
		
		
		if ( ChoiceMode )
		{
			// X + → で決定する。
			if ( Controller::IsKeyDown(X) && Controller::IsKeyPressed(DPadRight) )
			{
				GameKeyboard::Delete();
				GameKeyboard::Write( "決定: " << hit_names[page] );
				u32 id = 0x7FFE;
				Inventory::WriteSlot( 3, hit_id[page] );
				
				DisableChoiceMode();
			}
			
			if	( Controller::IsKeyPressed( Touchpad ) && ChoiceMode )
			{
				DisableChoiceMode();
			}
			
			
			
		}
		
		if ( Controller::IsKeyDown(B) )
		{
			std::string command = "";
			GameKeyboard::Copy(command,0,20);
			
			
			if ( Controller::IsKeyPressed(R) )
			{
				if ( !ChoiceMode )
				{
					/*if ( command == "test" )
					{
						//Chat::Write("test chat");
					}*/
					if ( command.length() > 0 )
					{
						find_item( command );
					}
					
					
				}
				else // ChoiceMode == true
				{
					page++;
					if ( page >= (int)(hit_names.size()) ) page = 0;
					GameKeyboard::Delete();
					GameKeyboard::Write( ":" << hit_names[page] );
				}	
			}
			
			if ( Controller::IsKeyPressed(L) )
			{
				if ( ChoiceMode )
				{
					page--;
					if ( page < 0 ) page = (int)(hit_names.size()) - 1;
					GameKeyboard::Delete();
					GameKeyboard::Write( ":" << hit_names[page] );
				}	
			}	
		}
	}
	

}


