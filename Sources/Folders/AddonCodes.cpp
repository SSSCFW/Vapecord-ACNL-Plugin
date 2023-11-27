#include "cheats.hpp"
#include "NonHacker.hpp"
#include "RegionCodes.hpp"
#include "Helpers/Game.hpp"
#include "Helpers/PlayerClass.hpp"
#include "Helpers/Player.hpp"
#include "Helpers/GameKeyboard.hpp"
#include "Helpers/Animation.hpp"
#include "Helpers/AnimData.hpp"
#include "Helpers/IDList.hpp"
#include "Helpers/Wrapper.hpp"
#include "Curl.hpp"
#include "Color.h"
#include "Files.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
 
std::vector<std::string> split(std::string str, char del) {
    int first = 0;
    int last = str.find_first_of(del);
 
    std::vector<std::string> result;
 
    while (first < str.size()) {
        std::string subStr(str, first, last - first);
 
        result.push_back(subStr);
 
        first = last + 1;
        last = str.find_first_of(del, first);
 
        if (last == std::string::npos) {
            last = str.size();
        }
    }
 
    return result;
}

/*
	// GameKeyboard

	bool GameKeyboard::Delete(void) {
		if(!GameKeyboard::IsOpen())
			return false;
		if(GameKeyboard::IsEmpty())
			return false;

		static Address DeleteFunc(0x523780, 0x5230D4, 0x5227C8, 0x5227C8, 0x5220D4, 0x5220D4, 0x521DCC, 0x521DCC);

		DeleteFunc.Call<void>(*(u32 *)Code::ChatPoint.addr, 0, 100);

		*(bool *)(*(u32 *)(Code::ChatPoint.addr) + 0x20) = 0; //unselects
		return true;
	}
*/

namespace CTRPluginFramework {

//日本版しか対応してないものが多いです。

	// Only JPN
	// かっぺいと会話中や駅にいるときだけゲーム速度を上昇させます。
	// Increase the game speed only when you are in conversation with Kappei or at the station.
	void OnlineFastGame(MenuEntry *entry) { 
		static const Address speed(0x54DDB4, 0x54D2CC, 0x54CDFC, 0x54CDFC, 0x54C6E8, 0x54C6E8, 0x54C40C, 0x54C40C);
		u32 value;
		Process::Read32(0x330A0AD8, value); //しずえ 41C00000
		if (value == 0x1 || value == 0x3F19999A || value == 0x41300000 || value == 0x41C00000){
			Process::Patch(speed.addr, GameHelper::GameSaving() ? 0xE59400A0 : 0xE3E004FF); 
		}
		else{
			Process::Patch(speed.addr, 0xE59400A0);
		}
	}

	// Only JPN
	// 強制帰還対策。乗っ取られたときにONにしてリロードする必要あり。常時ONだと帰れなくなる。
	void CanceledReturn(MenuEntry *entry) {
		if(entry->WasJustActivated()) 
			Process::Patch(0x00947E0C, 0x0);
			Process::Patch(0x00947E08, 0x0404FF00);
		//else if(!entry->IsActivated())
		//	Process::Patch(0x0032C2A4, 0xE92D43F8);
	}

	// 漢字(文字)変換
	// character conversion
	static std::map<std::string, std::string> kanji_list;
	static std::map<std::string, std::string> multi_list;
	void CharacterChange(MenuEntry *entry) {
		if(entry->WasJustActivated()) {
			File file(Character, File::READ);
			char del = ',';
			std::string line;
			LineReader reader(file);
			u32 lineNumber = 0;
			for(; reader(line); lineNumber++) {
				if(line.empty())
					continue;
				std::vector<std::string> oneline = split(line, del); //[0] = 漢字 [1] = ひらがな
				if (kanji_list.find(oneline[1]) == kanji_list.end()) { //kanji_listに指定キーが存在していなかったら
					kanji_list[oneline[1]] = oneline[0];
					multi_list[oneline[1]] = oneline[0];
				}
				else { //既に同じキーが存在していたら
					std::string read = kanji_list[oneline[1]];
					while (kanji_list.find(read) != kanji_list.end()) //重複キーの値がキーとして存在していたら
						read = kanji_list[kanji_list[oneline[1]]]; //重複キーの値(キー)の値
					kanji_list[read] = oneline[0];
					multi_list[oneline[1]] = oneline[0]; //キー:ひらがな 要素:漢字
				}
			}
			auto iter = multi_list.begin();
			while (iter != multi_list.end()) {
				kanji_list[iter->second] = iter->first;
				++iter;
			}
		}
				
		if(entry->Hotkeys[0].IsPressed()) {
			std::string Holder = "";
			if(GameKeyboard::CopySelected(Holder)) {
				if (kanji_list.find(Holder) != kanji_list.end()) {
					GameKeyboard::DeleteSelected();
					GameKeyboard::Write(kanji_list[Holder]);
				}
			}
		}
	}

	// Only JPN
	// Cstickのムーンジャンプ
	static int moon_value=0;
	void MoonJumpCpad(MenuEntry *entry) {
		u32 i = PlayerClass::GetInstance()->Offset(0x8C6);
		if (Controller::IsKeysDown(Key::ZL + Key::CStickLeft)) moon_value = 0;
		if (Controller::IsKeysDown(Key::ZL + Key::CStickUp)) moon_value = 1;
		if (Controller::IsKeysDown(Key::ZL + Key::CStickDown)) moon_value = 2;
		if (moon_value == 1) Process::Write32(i, 0x7FFFFF);
		if (moon_value == 2) Process::Write32(i, 0x19D5D);
	} 

	// ツールの機能変更
	// Only JPN
	void ToolsChanger(MenuEntry *entry)
	{
		if (Controller::IsKeysDown(Key::L + Key::CStickUp))
		{
			Process::Write32(0x33096734, 0x0000335B);//スコップ
			Process::Write32(0x3309A20C, 0x0000335B);
		}
		if (Controller::IsKeysDown(Key::L + Key::CStickDown))
		{
			Process::Write32(0x33096734, 0x0000334F);//オノ
			Process::Write32(0x3309A20C, 0x0000334F);
		}
		if (Controller::IsKeysDown(Key::L + Key::CStickRight))
		{
			Process::Write32(0x33096734, 0x00003353);//アミ
			Process::Write32(0x3309A20C, 0x00003353);
		}
		if (Controller::IsKeysDown(Key::L + Key::CStickLeft))
		{
			Process::Write32(0x33096734, 0x00003357);//つり
			Process::Write32(0x3309A20C, 0x00003357);
		}
		if (Controller::IsKeysDown(Key::R + Key::CStickUp))
		{
			Process::Write32(0x33096734, 0x00003363);//パチ
			Process::Write32(0x3309A20C, 0x00003363);
		}
		if (Controller::IsKeysDown(Key::R + Key::CStickDown))
		{
			Process::Write32(0x33096734, 0x00003365);//ハンマー
			Process::Write32(0x3309A20C, 0x00003365);
		}
	}
	
	// Only JPN
	// 室内にあるフラグをすべて削除する。主に破壊家具対策。オンラインでは対象を回収する必要あり。
	// Remove all flags in the room. Mainly for destructive furniture measures. On-line requires collection of the object.
	void RemoveRemakeFlag(MenuEntry *entry)
	{
		Keyboard key("家具フラグを消す家を選んでください。", { "P1", "P2", "P3", "P4" });

		int result = key.Open();
		if (result >= 0)
		{
			u32 offset = 0x31000000;
			u32 rooms[4][6] = {
				{0x3A0A6, 0x3B376, 0x3C646, 0x3D906, 0x3EBE6, 0x3FEB6}, 
				{0x41186, 0x42456, 0x43726, 0x449F6, 0x45CC6, 0x46F96},
				{0x48266, 0x49536, 0x4A806, 0x4BAD6, 0x4CDA6, 0x4E076},
				{0x4F346, 0x50616, 0x518E6, 0x52BB6, 0x53E86, 0x55156}
			};
			for (u32 room_address: rooms[result]){
				for (int i = 0; i < 0x78; i++) {
					Process::Write16(offset+room_address+(i*0x4), 0x0000);
				}
			}
		}
	}

	// オン島に誰も来なくなる。
	void IslandClosure(MenuEntry *entry) {
		if(entry->WasJustActivated())
		{
			Process::Patch(0x00947E0C, 0x00000001);
			Process::Patch(0x00947E08, 0x04000002);
		}
		else if(!entry->IsActivated())
		{
			Process::Patch(0x00947E0C, 0x00000000);
			Process::Patch(0x00947E08, 0x0404FF00);
		}
	}
	


//#################################################
	// チャットのメッセージをスクリーンに表示
	// Display chat messages on screen
	static std::vector<std::string> chat_text = { "", "", "", "", "" };

	void reset_message(void) {
		for (int i = 0; i<4; i++) {
			NonHacker *nHack = new NonHacker(i);
			nHack->ClearPlayerMessage();
			chat_text[i] = "";
			delete nHack;
		}
	}

	void GetMessage_p1(void) {	
		NonHacker *nHack1 = new NonHacker(0);
		std::string PlayerText1 = nHack1->GetPlayerMessage();
		if(PlayerText1.empty()) PlayerText1 = chat_text[0];
		
		chat_text[0] = (PlayerText1);
		delete nHack1;
	}

	void GetMessage_p2(void) {	
		NonHacker *nHack2 = new NonHacker(1);
		std::string PlayerText2 = nHack2->GetPlayerMessage();
		if(PlayerText2.empty()) PlayerText2 = chat_text[1];
		
		chat_text[1] = (PlayerText2);
		delete nHack2;
	}

	void GetMessage_p3(void) {	
		NonHacker *nHack3 = new NonHacker(2);
		std::string PlayerText3 = nHack3->GetPlayerMessage();
		if(PlayerText3.empty()) PlayerText3 = chat_text[2];
		
		chat_text[2] = (PlayerText3);
		delete nHack3;
	}

	void GetMessage_p4(void) {	
		NonHacker *nHack4 = new NonHacker(3);
		std::string PlayerText4 = nHack4->GetPlayerMessage();
		if(PlayerText4.empty()) PlayerText4 = chat_text[3];
		
		chat_text[3] = (PlayerText4);
		delete nHack4;
	}
	
	bool messageOSD(const Screen &screen) {
		if(!screen.IsTop)
			return 0;
		
		static constexpr u8 YPositions1[4] = { 189, 201, 213, 225 };

		screen.DrawSysfont(pColor[0] << "P1: " << Color(0xFFFFFFFF) << chat_text[0], 0, YPositions1[0]);
		screen.DrawSysfont(pColor[1] << "P2: " << Color(0xFFFFFFFF) << chat_text[1], 0, YPositions1[1]);
		screen.DrawSysfont(pColor[2] << "P3: " << Color(0xFFFFFFFF) << chat_text[2], 0, YPositions1[2]);
		screen.DrawSysfont(pColor[3] << "P4: " << Color(0xFFFFFFFF) << chat_text[3], 0, YPositions1[3]);

		return 1;
	}

	void ShowChatMessage(MenuEntry *entry) {

		if(entry->WasJustActivated()) {
			PluginMenu *menu = PluginMenu::GetRunningInstance();
			*menu += GetMessage_p1;
			*menu += GetMessage_p2;
			*menu += GetMessage_p3;
			*menu += GetMessage_p4;
			OSD::Run(messageOSD); 
		}
		else if(!entry->IsActivated()) {
			PluginMenu *menu = PluginMenu::GetRunningInstance();
			*menu -= GetMessage_p1;
			*menu -= GetMessage_p2;
			*menu -= GetMessage_p3;
			*menu -= GetMessage_p4;
			OSD::Stop(messageOSD); 
		}
	}
//#################################################
//#################################################
	// アニメidをスクリーンに表示
	// Animation id on screen
	static std::vector<std::string> anim_text = { "", "", "", "" };

	void GetAnim_p1(void) {	
		if(!PlayerClass::GetInstance()->GetCoordinates())
			return;
		anim_text[0] = (Utils::Format("%02X / %03X", *PlayerClass::GetInstance()->GetAnimation(), *PlayerClass::GetInstance()->GetSnake()));
	}

	void GetAnim_p2(void) {	
		if(!PlayerClass::GetInstance(1)->GetCoordinates())
			return;
		anim_text[1] = (Utils::Format("%02X / %03X", *PlayerClass::GetInstance(1)->GetAnimation(), *PlayerClass::GetInstance(1)->GetSnake()));
	}

	void GetAnim_p3(void) {	
		if(!PlayerClass::GetInstance(2)->GetCoordinates())
			return;
		anim_text[2] = (Utils::Format("%02X / %03X", *PlayerClass::GetInstance(2)->GetAnimation(), *PlayerClass::GetInstance(2)->GetSnake()));
	}

	void GetAnim_p4(void) {	
		if(!PlayerClass::GetInstance(3)->GetCoordinates())
			return;
		anim_text[3] = (Utils::Format("%02X / %03X", *PlayerClass::GetInstance(3)->GetAnimation(), *PlayerClass::GetInstance(3)->GetSnake()));
	}
	
	bool animOSD(const Screen &screen) {
		if(!screen.IsTop)
			return 0;
		
		static constexpr u8 YPositions2[4] = { 141, 153, 165, 177 };

		screen.DrawSysfont(pColor[0] << "P1: " << Color(0xFFFFFFFF) << anim_text[0], 0, YPositions2[0]);
		screen.DrawSysfont(pColor[1] << "P2: " << Color(0xFFFFFFFF) << anim_text[1], 0, YPositions2[1]);
		screen.DrawSysfont(pColor[2] << "P3: " << Color(0xFFFFFFFF) << anim_text[2], 0, YPositions2[2]);
		screen.DrawSysfont(pColor[3] << "P4: " << Color(0xFFFFFFFF) << anim_text[3], 0, YPositions2[3]);

		return 1;
	}

	void ShowAnimID(MenuEntry *entry) {

		if(entry->WasJustActivated()) {
			PluginMenu *menu = PluginMenu::GetRunningInstance();
			*menu += GetAnim_p1;
			*menu += GetAnim_p2;
			*menu += GetAnim_p3;
			*menu += GetAnim_p4;
			OSD::Run(animOSD); 
		}
		else if(!entry->IsActivated()) {
			PluginMenu *menu = PluginMenu::GetRunningInstance();
			*menu -= GetAnim_p1;
			*menu -= GetAnim_p2;
			*menu -= GetAnim_p3;
			*menu -= GetAnim_p4;
			OSD::Stop(animOSD); 
		}
	}
//#################################################

	//draw x+6 y+10
	// チャット ボタン表示関係
	int ButtonList[5][3] = 
		{
			{16, 5, 4}, //Copy
			{48, 5, 5}, //Paste
			{96, 5, 3}, //Cut
			{128, 5, 6}, //Delete
			{170, 5, 4}, //Send

		};

	int TouchButton(void)
	{
		static UIntRect CopyCoord(ButtonList[0][0], ButtonList[0][1], ButtonList[0][2]*6, 11);
		static UIntRect PasteCoord(ButtonList[1][0], ButtonList[1][1], ButtonList[1][2]*6, 11);
		static UIntRect CutCoord(ButtonList[2][0], ButtonList[2][1], ButtonList[2][2]*6, 11);
		static UIntRect DeleteCoord(ButtonList[3][0], ButtonList[3][1], ButtonList[3][2]*6, 11);
		static UIntRect SendCoord(ButtonList[4][0], ButtonList[4][1], ButtonList[4][2]*6, 11);
		if (CopyCoord.Contains(Touch::GetPosition())) return 1;
		if (PasteCoord.Contains(Touch::GetPosition())) return 2;
		if (CutCoord.Contains(Touch::GetPosition())) return 3;
		if (DeleteCoord.Contains(Touch::GetPosition())) return 4;
		if (SendCoord.Contains(Touch::GetPosition())) return 5;
		return 0;
	}

	bool DrawChatButton(const Screen &scr)
	{ // input 下 54
		if(!GameKeyboard::IsOpen()) return false;
		if ( scr.IsTop ) return false;
		scr.Draw( "Copy", ButtonList[0][0], ButtonList[0][1], Color::Yellow, Color::Black);
		scr.Draw( "Paste", ButtonList[1][0], ButtonList[1][1], Color::Yellow, Color::Black);
		scr.Draw( "Cut", ButtonList[2][0], ButtonList[2][1], Color::Yellow, Color::Black);
		scr.Draw( "Delete", ButtonList[3][0], ButtonList[3][1], Color::Yellow, Color::Black);
		scr.Draw( "Send", ButtonList[4][0], ButtonList[4][1], Color::Yellow, Color::Black);
		
		return true;
	}

	static std::string Holder = "";

	// チャットに便利なボタンを追加します。
	// Add a convenient button to chat.
	void ChatButton(MenuEntry *entry) {
		static Address callchat(0x52440C, 0x523D60, 0x523454, 0x523454, 0x522D40, 0x522D40, 0x522A48, 0x522A48);

		PluginMenu *menu = PluginMenu::GetRunningInstance();
		if(entry->WasJustActivated()) {
			//Process::Patch(Code::DisableChatRemoval.addr, 0xEA000000);

			OSD::Run(DrawChatButton); 
			if(!File::Exists(Utils::Format(PATH_CBOARD, regionName.c_str()))) 
				File::Create(Utils::Format(PATH_CBOARD, regionName.c_str()));

			Holder.clear();
			File f_board(Utils::Format(PATH_CBOARD, regionName.c_str()), File::READ);
			LineReader reader(f_board);
			reader(Holder);
			Holder.resize(65);
			Holder.shrink_to_fit();

			f_board.Flush();
			f_board.Close();
		}
		else if(!entry->IsActivated()) {
			//Process::Patch(Code::DisableChatRemoval.addr, 0xE5900000);
			OSD::Stop(DrawChatButton); 
		}
		if (Controller::IsKeyPressed(Touchpad) && GameKeyboard::IsOpen())
		{
			int ButtonID = TouchButton();
			switch (ButtonID) {
				case 1: // Copy
					{
						Holder.clear();
						if (GameKeyboard::CopySelected(Holder)) {
							File::Remove(Utils::Format(PATH_CBOARD, regionName.c_str()));
							File::Create(Utils::Format(PATH_CBOARD, regionName.c_str()));

							File f_board(Utils::Format(PATH_CBOARD, regionName.c_str()), File::WRITE);

							LineWriter writer(f_board);
							writer << Holder;
							writer.Flush();
							writer.Close();

							f_board.Flush();
							f_board.Close();
							OSD::Notify("Copied", Color(0xFF0077FF));
						}
					}
					break;
				case 2: // Paste
					{
						Holder.clear();
						File f_board(Utils::Format(PATH_CBOARD, regionName.c_str()), File::READ);
						LineReader reader(f_board);
						reader(Holder);
						Holder.resize(65);
						Holder.shrink_to_fit();

						f_board.Flush();
						f_board.Close();

						GameKeyboard::Write(Holder);
					}
					break;
				case 3: // Cut
					{
						Holder.clear();
						if (GameKeyboard::CopySelected(Holder)){
							GameKeyboard::DeleteSelected();

							File::Remove(Utils::Format(PATH_CBOARD, regionName.c_str()));
							File::Create(Utils::Format(PATH_CBOARD, regionName.c_str()));

							File f_board(Utils::Format(PATH_CBOARD, regionName.c_str()), File::WRITE);
							
							LineWriter writer(f_board);
							writer << Holder;
							writer.Flush();
							writer.Close();

							f_board.Flush();
							f_board.Close();

							OSD::Notify("Cut", Color(0x00FF6FFF));
						}
					}
					break;
				case 4: // Delete
					GameKeyboard::Delete();
					break;
				case 5: // Send
					callchat.Call<void>(1, 2);
					break;
				default:
					break;
			}
		}
	}

	u8 b_AnimID = 0x1;
	u8 a_AnimID2 = 0x9D;
	Item a_ItemID2 = {0x2001, 0};
	u16 a_SnakeID2 = 1;
	u8 a_EmoteID2 = 1;
	u16 a_SoundID2 = 0x660;
	u8 a_AppearanceID2[3] = {0, 0, 0};

	void anim_settings(MenuEntry *entry) {
		Keyboard key("Select anim", { "Before Anim", "After Anim", "Item ID" });

		int result = key.Open();
		if (result >= 0)
		{
			if (result == 0) {
				Wrap::KB<u8>("Before Anim", true, 2, b_AnimID, b_AnimID, AnimChange);
			}
			if (result == 1) {
				Wrap::KB<u8>("After Anim", true, 2, a_AnimID2, a_AnimID2, AnimChange);
			}
			if (result == 2) {
				Wrap::KB<u32>("Item ID", true, 8, *(u32 *)&a_ItemID2, *(u32 *)&a_ItemID2, ItemChange);
			}
		}
	}

	// 指定した動作をすると別の動作に置き換えます。全プレイヤー対象。
	// The specified action will be replaced by another action. For all players.
	void EveryoneAnimChange(MenuEntry *entry) {
		if(!PlayerClass::GetInstance()->IsLoaded())
			return;
		static u32 wX, wY;
		static u32 wX2, wY2;
		static u32 wX3, wY3;
		static u32 wX4, wY4;
		//(PlayerClass::GetInstance(pID)->GetWorldCoords(&x, &y))
		if (PlayerClass::GetInstance()->GetWorldCoords(&wX, &wY) && *PlayerClass::GetInstance()->GetAnimation() == b_AnimID) {
			Animation::ExecuteAnimationWrapper(0, IDList::AnimationValid(a_AnimID2) ? a_AnimID2 : 0x06, IDList::ItemValid(a_ItemID2) ? a_ItemID2 : Item{0x2001, 0}, a_EmoteID2, a_SnakeID2, a_SoundID2, 0, wX, wY, 1, a_AppearanceID2);
		}
		if (PlayerClass::GetInstance(1)->GetWorldCoords(&wX2, &wY2) && *PlayerClass::GetInstance(1)->GetAnimation() == b_AnimID) {
			Animation::ExecuteAnimationWrapper(1, IDList::AnimationValid(a_AnimID2) ? a_AnimID2 : 0x06, IDList::ItemValid(a_ItemID2) ? a_ItemID2 : Item{0x2001, 0}, a_EmoteID2, a_SnakeID2, a_SoundID2, 0, wX2, wY2, 1, a_AppearanceID2);
		}
		if (PlayerClass::GetInstance(2)->GetWorldCoords(&wX3, &wY3) && *PlayerClass::GetInstance(2)->GetAnimation() == b_AnimID) {
			Animation::ExecuteAnimationWrapper(2, IDList::AnimationValid(a_AnimID2) ? a_AnimID2 : 0x06, IDList::ItemValid(a_ItemID2) ? a_ItemID2 : Item{0x2001, 0}, a_EmoteID2, a_SnakeID2, a_SoundID2, 0, wX3, wY3, 1, a_AppearanceID2);
		}
		if (PlayerClass::GetInstance(3)->GetWorldCoords(&wX4, &wY4) && *PlayerClass::GetInstance(3)->GetAnimation() == b_AnimID) {
			Animation::ExecuteAnimationWrapper(3, IDList::AnimationValid(a_AnimID2) ? a_AnimID2 : 0x06, IDList::ItemValid(a_ItemID2) ? a_ItemID2 : Item{0x2001, 0}, a_EmoteID2, a_SnakeID2, a_SoundID2, 0, wX4, wY4, 1, a_AppearanceID2);
		}
	}

    //しずえスキップ関係
	bool TouchSkipButton(void)
	{
		static UIntRect skip_coord(13, 190, 5*6, 30);
		if (skip_coord.Contains(Touch::GetPosition())) return 1;
		return 0;
	}

	bool SkipButton(const Screen &scr)
	{
		if ( scr.IsTop ) return false;
		scr.Draw( "Skip", 16, 200, Color::Green, Color::White);
		
		return true;
	}

	bool on_run = false;

	void IsabelleSkip(MenuEntry *entry) {
		// FastGameSpeed
		static const Address speed(0x54DDB4, 0x54D2CC, 0x54CDFC, 0x54CDFC, 0x54C6E8, 0x54C6E8, 0x54C40C, 0x54C40C);

		u8 roomID = GameHelper::RoomCheck();
		if(!entry->IsActivated()) {
			if (on_run) OSD::Stop(SkipButton); 
			Process::Patch(speed.addr, 0xE59400A0);
			on_run = false;
		}
		else if (roomID == 0x63) { // Isabelle
			Process::Patch(speed.addr, GameHelper::GameSaving() ? 0xE59400A0 : 0xE3E004FF); 

			if (!on_run) OSD::Run(SkipButton); 
			
			on_run = true;
			if (Controller::IsKeyPressed(Touchpad) && TouchSkipButton()) {
				static Address roomfunc(0x304A60, 0x304C68, 0x304AEC, 0x304AEC, 0x304A94, 0x304A94, 0x304A3C, 0x304A3C);	
				roomfunc.Call<u32>(0, 1, 1, 0);
			}
		}
		else {
			if (on_run){
				OSD::Stop(SkipButton);
				Process::Patch(speed.addr, 0xE59400A0);
				on_run = false;
			}
		}
		
	}

	//直でDiscord API叩くより自作API経由したほうが軽い
	//std::string url = "https://discord.com/api/webhooks/";
	std::string url = "http://192.168.11.6:19569/discord/post/";

	void http_test(MenuEntry *entry) {
        std::string data = "{\"username\":\"CTRPF\",\"content\":\"aaa\"}";

        std::vector<u8> out;
        CURLcode res;
        if ((res = Curl::Post(url.c_str(), data.c_str(), out)) == CURLE_OK)
            MessageBox("success!")();
        else
            OSD::Notify(Utils::Format("failed %d", res));
        Sleep(Seconds(1));
        return;
	}
	
	void chat_send_discord(std::string text) {	
		std::string data = "{\"username\":\"とび森チャット\",\"content\":\""+text+"\"}";
        std::vector<u8> out;
        CURLcode res;
		Curl::Post(url.c_str(), data.c_str(), out);
	}

	static std::vector<std::string> discord_text = { "", "", "", "" };

	void GetMessage_p1_discord(void) {	
		NonHacker *nHack1 = new NonHacker(0);
		std::string PlayerText1 = nHack1->GetPlayerMessage();
		std::string PlayerName = nHack1->GetPlayerName();
		std::string text = PlayerName+": "+PlayerText1;
		if(!PlayerText1.empty() && discord_text[0] != text)
			chat_send_discord(text);
		discord_text[0] = text;
		delete nHack1;
	}

	void GetMessage_p2_discord(void) {	
		NonHacker *nHack2 = new NonHacker(1);
		std::string PlayerText2 = nHack2->GetPlayerMessage();
		std::string PlayerName = nHack2->GetPlayerName();
		std::string text = PlayerName+": "+PlayerText2;
		if(!PlayerText2.empty() && discord_text[1] != text)
			chat_send_discord(text);
		discord_text[1] = text;
		delete nHack2;
	}

	void GetMessage_p3_discord(void) {	
		NonHacker *nHack3 = new NonHacker(2);
		std::string PlayerText3 = nHack3->GetPlayerMessage();
		std::string PlayerName = nHack3->GetPlayerName();
		std::string text = PlayerName+": "+PlayerText3;
		if(!PlayerText3.empty() && discord_text[2] != text)
			chat_send_discord(text);
		discord_text[2] = text;
		delete nHack3;
	}

	void GetMessage_p4_discord(void) {	
		NonHacker *nHack4 = new NonHacker(3);
		std::string PlayerText4 = nHack4->GetPlayerMessage();
		std::string PlayerName = nHack4->GetPlayerName();
		std::string text = PlayerName+": "+PlayerText4;
		if(!PlayerText4.empty() && discord_text[3] != text)
			chat_send_discord(text);
		discord_text[3] = text;
		delete nHack4;
	}

	//discordにチャットを送信
	void SendDiscordMessage(MenuEntry *entry) {
		
		if(entry->WasJustActivated()) {
			PluginMenu *menu = PluginMenu::GetRunningInstance();
			*menu += GetMessage_p1_discord;
			*menu += GetMessage_p2_discord;
			*menu += GetMessage_p3_discord;
			*menu += GetMessage_p4_discord;
		}
		else if(!entry->IsActivated()) {
			PluginMenu *menu = PluginMenu::GetRunningInstance();
			*menu -= GetMessage_p1_discord;
			*menu -= GetMessage_p2_discord;
			*menu -= GetMessage_p3_discord;
			*menu -= GetMessage_p4_discord;
		}
	}

}
