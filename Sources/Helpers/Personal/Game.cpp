#include "Helpers/Game.hpp"
#include "Helpers/PlayerClass.hpp"
#include "Helpers/IDList.hpp"
#include "Helpers/Save.hpp"
#include "Helpers/Player.hpp"
#include "Helpers/PlayerPTR.hpp"
#include "Helpers/Animation.hpp"
#include "Helpers/Dropper.hpp"
#include "RegionCodes.hpp"

namespace CTRPluginFramework {
	Keyboard optKb("");
//Writes 4Bit
	bool Set::WriteNibble(u32 address, u8 value, bool right_side) {
		if(!Process::CheckAddress(address)) //checks if address is valid
			return false;
	//right side of the byte
		if(right_side) {
			*(u8 *)address &= ~0xF; //Clears 4bit first
			*(u8 *)address |= (value & 0xF); //Write 4bit then
			return true;
		}
	//left side of the byte
		*(u8 *)address &= ~(0xF << 4); //Clears 4bit first
		*(u8 *)address |= ((value & 0xF) << 4); //Write 4bit then
		return true;
	}
//Reads 4Bit
	bool Set::ReadNibble(u32 address, u8 &value, bool right_side) {
		if(!Process::CheckAddress(address)) //checks if address is valid
			return false;
	//right side of the byte
		if(right_side) {
			value = *(u8 *)(address) & 0xF;
			return true;
		}
	//left side of the byte	
		value = (*(u8 *)(address) >> 4) & 0xF;
		return true;
	}

	c_RGBA Set::ToRGBA(int hexValue) {	
		c_RGBA rgb;
		rgb.R = ((hexValue >> 24) & 0xFF) / 255.0;
		rgb.G = ((hexValue >> 16) & 0xFF) / 255.0;
		rgb.B = ((hexValue >> 8) & 0xFF) / 255.0;
		rgb.A = ((hexValue) & 0xFF) / 255.0;
		return rgb;
	}

	u32 Set::ToRGB(int r, int g, int b) {
		return(r << 16 | g << 8 | b);
	}

	u32 Set::ToRGBA_U32(int r, int g, int b, int a) {
		return(r << 24 | g << 16 | b << 8 | a);
	}

	void GameHelper::PlaySound(u16 soundID) {
		static Address soundfunc(0x58DE9C, 0x58D3B4, 0x58CEE4, 0x58CEE4, 0x58C7D4, 0x58C7D4, 0x58C4A8, 0x58C4A8);
		soundfunc.Call<void>(0x1000000 + soundID);
	}

	void GameHelper::ResetValueDisplay(void) {
	//TODO: port addresses
		static const Address reset(0x951722, 0, 0, 0, 0, 0, 0, 0); 
		Process::Write8(reset.addr, 0);
	}

	bool GameHelper::SetValueDisplay(u8 type) {
	//TODO: port addresses
		static Address disp(0x2912B8, 0, 0, 0, 0, 0, 0, 0); 
		return disp.Call<bool>(type);
	}

//returns current grass state of coordinates
	u8 GrassState(u8 wX, u8 wY) {	
		static Address gState(0x6C92FC, 0x6C87AC, 0x6C8334, 0x6C830C, 0x6C7C2C, 0x6C7C04, 0x6C77D4, 0x6C77AC); 
		return gState.Call<u8>(wX, wY);
	}
//Water flower
	bool GameHelper::WaterFlower(u8 wX, u8 wY) {
		if(!PlayerClass::GetInstance()->IsLoaded())
			return 0;
		
		static Address WFlower(0x765B14, 0x764AF8, 0x764B1C, 0x764AF4, 0x7642B4, 0x76428C, 0x763E5C, 0x763E34); 
		WFlower.Call<void>(PlayerClass::GetInstance()->Offset(), wX, wY);
		return 1;
	}
//reload room
	void GameHelper::ReloadRoom() {
		static const Address u0Data(0x976C0E, 0x975BEE, 0x975C06, 0x975C06, 0x96FC06, 0x96EC06, 0x96EC06, 0x96EC06);

		static Address ReloadFunc(0x5B6660, 0x5B5B78, 0x5B56A8, 0x5B56A8, 0x5B4F98, 0x5B4F98, 0x5B4C6C, 0x5B4C6C); 
		ReloadFunc.Call<void>(GetRoomData(), RoomCheck(), PlayerClass::GetInstance()->GetCoordinates(), (u32 *)u0Data.addr, 6, 0, 0, 1, 1);
	}
	
//Get Room Reload Data
	u32 GameHelper::GetRoomData() {
		static Address RoomDat(0x308154, 0x308340, 0x3081F4, 0x3081F4, 0x308110, 0x308110, 0x308198, 0x308198); 
		return RoomDat.Call<u32>();
	}
//check if save screen is active 
	bool GameHelper::GameSaving() {
		static Address saving(0x126568, 0x125E54, 0x12658C, 0x12658C, 0x126554, 0x126554, 0x126554, 0x126554); 
		return saving.Call<bool>();
	}
	
	/*u32 GameHelper::GetExhibition() {
		static const Address d_exhibition(0x9B4AC0, 0x9B3AC0, 0x9B3AC0, 0x9B3AC0, 0x9ADAA0, 0x9ACAA0, 0x9ACAC0, 0x9ACAC0);
		return *(u32 *)d_exhibition.addr;
	}

	u32 GameHelper::GetFriend() {
		static const Address d_friend(0x983038, 0x982018, 0x982030, 0x982030, 0x97C030, 0x97B030, 0x97B030, 0x97B030);
		return *(u32 *)d_friend.addr;
	}

	u32 GameHelper::GetDesign() {
		static const Address d_design(0x983088, 0x982068, 0x982080, 0x982080, 0x97C080, 0x97B080, 0x97B080, 0x97B080);
		return *(u32 *)d_design.addr;
	}

	u32 GameHelper::GetMail() {
		static const Address d_mail(0x983050, 0x982030, 0x982048, 0x982048, 0x97C048, 0x97B048, 0x97B048, 0x97B048);
		return *(u32 *)d_mail.addr;
	}*/
//get GameType
	u8 GameHelper::GetGameType() {
		static Address gtype(0x305ED8, 0x305F2C, 0x305ED8, 0x305ED8, 0x305F54, 0x305F54, 0x305F24, 0x305F24);
		return gtype.Call<u8>();
	}
//Change GameType
	void GameHelper::ChangeGameType(u8 GameType) {
		static Address gtype(0x625B88, 0x6250B0, 0x624BC0, 0x624BC0, 0x624680, 0x624680, 0x624228, 0x624228);
		gtype.Call<void>(GameType);
	}
//call menu
	void GameHelper::OpenMenu(u8 menuID, bool NoMenCall) {
		static Address SetupMenu(0x5C5398, 0x5C48C8, 0x5C43E0, 0x5C43E0, 0x5C3CD0, 0x5C3CD0, 0x5C39A4, 0x5C39A4);
		SetupMenu.Call<void>(1);

		if(NoMenCall)
			return;
		
		static Address OpenMenu(0x6D3F8C, 0x6D343C, 0x6D2FC4, 0x6D2F9C, 0x6D2ADC, 0x6D2AB4, 0x6D2AE8, 0x6D2AC0); 
		OpenMenu.Call<void>(menuID, 0, 0);
	}

//move building
	void GameHelper::MoveBuilding() {
		if(!PlayerClass::GetInstance()->IsLoaded()) {
			OSD::Notify("Error: Player Needs To Be Loaded!", Color::Red);
			return;
		}
		
		if(!GameHelper::IsInRoom(0)) {
			OSD::Notify("Error: Only Works In Town!", Color::Red);
			return;
		}
		
		if(GameHelper::GetOnlinePlayerCount() != 0) {
			OSD::Notify("Error: Only Works In Your Own Town!", Color::Red);
			return;
		}
		
		Sleep(Milliseconds(100));
		std::vector<u8> index;
		s8 val;
		std::vector<std::string> buildingOpt;
		u32 building, targetPlayer;
		u32 buildingremoval = Save::GetInstance()->Address(0x4BE88);
		u32 x, y;
		
		buildingOpt.clear();
		for(building = 0; building < 56; building++) { 
			targetPlayer = buildingremoval + building * 4;
			if (*(u8 *)(targetPlayer) != 0xFC) {
				index.push_back(building);
				buildingOpt.push_back(IDList::GetBuildingName(*(u8 *)targetPlayer));
			}
		}
		
		optKb.Populate(buildingOpt);
		val = optKb.Open();
		if(val < 0) 
			return;
		
		PlayerClass::GetInstance()->GetWorldCoords(&x, &y);
		Process::Write8(buildingremoval + index.at(val) * 4 + 2, x & 0xFF);
		Process::Write8(buildingremoval + index.at(val) * 4 + 3, y & 0xFF);
		Sleep(Milliseconds(20));
		GameHelper::ReloadRoom();
	}
//remove building
	void GameHelper::RemoveBuilding() {
		if(!PlayerClass::GetInstance()->IsLoaded()) {
			OSD::Notify("Error: Player Needs To Be Loaded!", Color::Red);
			return;
		}
		
		if(!GameHelper::IsInRoom(0)) {
			OSD::Notify("Error: Only Works In Town!", Color::Red);
			return;
		}
		
		if(GameHelper::GetOnlinePlayerCount() != 0) {
			OSD::Notify("Error: Only Works In Your Own Town!", Color::Red);
			return;
		}
		
		Sleep(Milliseconds(100));
		std::vector<u8> index;
		s8 val;
		std::vector<std::string> buildingOpt;
		u32 building, targetPlayer;
		u32 buildingremoval = Save::GetInstance()->Address(0x4BE88);
		
		buildingOpt.clear();
		for(building = 0; building < 56; building++) {
			targetPlayer = buildingremoval + building * 4;
			if(*(u8 *)(targetPlayer) != 0xFC) {
				index.push_back(building);
				
				buildingOpt.push_back(IDList::GetBuildingName(*(u8 *)targetPlayer));
			}
		}
		
		optKb.Populate(buildingOpt);
		val = optKb.Open();
		if(val < 0) 
			return;
		
		if(!IDList::BuildingValid(*(u8 *)(buildingremoval + index.at(val) * 4) & 0xFF)) {
			OSD::Notify("Error: You can not remove that building!", Color::Red);
			return;
		}
		
		Process::Write32(buildingremoval + index.at(val) * 4, 0xFC);
		*(u8 *)(buildingremoval - 4) = *(u8 *)(buildingremoval - 4) - 1;
		Sleep(Milliseconds(20));
		GameHelper::ReloadRoom();
	}
//check for free building place
	bool BuildingSpotFree() {	
		if(!PlayerClass::GetInstance()->IsLoaded()) 
			return 0;
		
		int Bslot = 0;
		while(true) { 
			u32 Building = Save::GetInstance()->Address(0x4BE88 + (0x4 * Bslot));
			if(0xFC == *(u8 *)Building) //If empty building slot was found
				return 1;
			
			Bslot++; //goto next slot
			
			if(56 < Bslot) //no empty building slot found
				return 0;			
		}
	}

//place building
	void GameHelper::PlaceBuilding(u8 buildingID) {
		if(!PlayerClass::GetInstance()->IsLoaded()) {
			OSD::Notify("Error: Player Needs To Be Loaded!", Color::Red);
			return;
		}
		
		if(!GameHelper::IsInRoom(0)) {
			OSD::Notify("Error: Only Works In Town!", Color::Red);
			return;
		}
		
		if(!IDList::BuildingValid(buildingID)) {
			OSD::Notify("Error: Building Invalid!", Color::Red);
			return;
		}
		
		if(GameHelper::GetOnlinePlayerCount() != 0) {
			OSD::Notify("Error: Only Works In Your Own Town!", Color::Red);
			return;
		}
		
		if(!BuildingSpotFree()) {
			OSD::Notify("Error: No Building Slot free!", Color::Red);
			return;
		}
	
		u32 x, y;	
		PlayerClass::GetInstance()->GetWorldCoords(&x, &y);
		buildingID &= 0xFFFF;
		GameHelper::PlaceBuildingUpdateCollisions(x, y, buildingID);
		Sleep(Milliseconds(20));
		GameHelper::ReloadRoom();	
	}
//is in room
	bool GameHelper::IsInRoom(u8 room) {
		return GameHelper::RoomCheck() == room;
	} 
//set played hours	
	void GameHelper::SetPlayedHours(u32 hours) {
		int intminutes = hours * 60;
		int intseconds = intminutes * 60;
		u32 sethours = intseconds;
		Save::GetInstance()->Write<u32>(0x621B0, sethours);
	}
//set played days	
	void GameHelper::SetPlayedDays(u16 days) {
		Save::GetInstance()->Write<u16>(0x6223E, days);
	}
	
//set current time
	void GameHelper::SetCurrentTime(bool forward, int Minutes, int Hours, int Days, int Months, int Years) {
		u64 SMinute = (Minutes * 60000000000);
		u64 SHour = (Hours * 60 * 60000000000);
		u64 SDay = (Days * 24 * 60 * 60000000000);
		u64 SMonth = (Months * 30 * 24 * 60 * 60000000000);
		u64 SYear = (Years * 365 * 24 * 60 * 60000000000);
		
		u64 Time = SMinute + SHour + SDay + SMonth + SYear;	

	  //static const u32 RealTime = FollowPointer(0x300000BC, 0x2E0, -1) + 0x18;
		static const Address RealTime(0x95D508, 0x95C4E8, 0x95C500, 0x95C500, 0x956500, 0x955500, 0x955500, 0x955500);
	//makes time negative
		if(!forward) 
			Time = Time * (-1);

		*(u64 *)Save::GetInstance()->Address(0x621A0) += Time;
		*(u64 *)RealTime.addr += Time;
	}
	
//get money(decrypt it) 
	int GameHelper::GetMoney(u64 *position) {
		if(Player::GetSaveOffset(4) == 0) 
			return 0;
		
		static Address moneyget(0x3037DC, 0x30366C, 0x303870, 0x303870, 0x30353C, 0x30353C, 0x3035F8, 0x3035F8);
		return moneyget.Call<int>(position);
	}
//set money	
	void GameHelper::SetMoney(u32 position, int moneyamount) {
		if(Player::GetSaveOffset(4) == 0) 
			return;
		
		static Address moneyset(0x3036A4, 0x303534, 0x303738, 0x303738, 0x303404, 0x303404, 0x3034C0, 0x3034C0); 
		moneyset.Call<void>(position, (u32)moneyamount);
	}

	void InjectTouchCallBack(void) {
		static PluginMenu *menu = PluginMenu::GetRunningInstance();
		if(!menu->IsOpen()) {
			Controller::InjectTouch(300, 200); //lazy way to properly update badges lol
			*menu -= InjectTouchCallBack;
		}
	}

//set badges	
	void GameHelper::SetBadges(u8 badge, u8 type) {
		if(Player::GetSaveOffset(4) == 0) 
			return;
		
		Process::Write8(PlayerPTR::Pointer(0x569C) + badge, type);

		return; //for now!

		if(GameHelper::BaseInvPointer() == 0)
			return;

	//if badge menu is not opened
		if(*(u8 *)(*(u32 *)(GameHelper::BaseInvPointer() + 0xC) + 0x19E8) != 0xC) 
			return;

		static PluginMenu *menu = PluginMenu::GetRunningInstance();

		static Address LoadBadges(0x6AD730);
		LoadBadges.Call<void>(*(u32 *)(GameHelper::BaseInvPointer() + 0xC) + 0x800, PlayerPTR::Pointer(0x569C)); //32DC50B8, 31F2C6BC
		*menu += InjectTouchCallBack;
	}
//if game is ACNL
	bool GameHelper::IsACNL() {
		u32 address = Address(1, 1, 1, 1, 1, 1, 1, 1).addr;
		return (bool)address; //will return 1 if acnl game, 0 if not
	}
//get country name
	std::string GameHelper::GetCountryName() {
		static Address countryfunc(0x350AE8, 0x3504FC, 0x34FC30, 0x34FC30, 0x34F8C8, 0x34F8C8, 0x34F78C, 0x34F78C);
		u8 country = countryfunc.Call<u8>();
		return IDList::SetCountryName(country);
	}
//if item is outdoor only
	bool GameHelper::IsOutdoorItem(u16 item) {
		return (item <= 0xFD && item >= 0);
	}
//converts flower to outdoor flower
	void GameHelper::ToOutdoorFlowers(u32& input) {
		input = (input - 0x2890);
	}
//converts flower to indoor flower
	void GameHelper::ToIndoorFlowers(u32& input) {
		input = (input + 0x2890);
	}

//catalog function
	void GameHelper::Catalog(bool directcall) {
		if(!PlayerClass::GetInstance()->IsLoaded()) 
			return;

		if(!directcall) {
			Animation::Idle();
			GameHelper::OpenMenu(0, true);
		}
		
		static Address cfunction(0x6D33D8, 0x6D2888, 0x6D2410, 0x6D23E8, 0x6D1BBC, 0x6D1B94, 0x6D1764, 0x6D173C); 
		cfunction.Call<void>(0);
	}
//get base inventory pointer
	u32 GameHelper::BaseInvPointer() {
		static const Address InvMenu(0x98D500, 0x98C500, 0x98C500, 0x98C500, 0x986500, 0x985500, 0x985500, 0x985500);
		return *(u32 *)InvMenu.addr; 
	}
//room function
	u32 GameHelper::RoomFunction(u8 room, bool u0, bool u1, bool u2) {
		if(!PlayerClass::GetInstance()->IsLoaded())
			return -1;

		if(GameHelper::GetOnlinePlayerCount() != 0) 
			return -2;

		static Address roomfunc(0x304A60, 0x304C68, 0x304AEC, 0x304AEC, 0x304A94, 0x304A94, 0x304A3C, 0x304A3C);	
		return roomfunc.Call<u32>(room, u0, u1, u2);
	}
//get room ID
	u8 GameHelper::RoomCheck() {
		static Address GetRoom(0x2F7384, 0x2F74C8, 0x2F73AC, 0x2F73AC, 0x2F75CC, 0x2F75CC, 0x2F7488, 0x2F7488);
		return GetRoom.Call<u8>();
	}
//get next loaded room ID
	u8 GameHelper::NextRoomCheck() {
		static Address GetNextRoom(0x5B4C08, 0x5B4120, 0x5B3C50, 0x5B3C50, 0x5B3540, 0x5B3540, 0x5B3214, 0x5B3214);
		return GetNextRoom.Call<u8>();
	}
//If loading screen is active
	bool GameHelper::LoadRoomBool() {
		static const Address LoadCheck(0x94F451, 0x94E441, 0x94E451, 0x94E451, 0x948451, 0x947451, 0x947451, 0x947451);
		return *(bool *)LoadCheck.addr;
		return 0;
	}
//get map boolen pointer	
	bool GameHelper::MapBoolCheck() {
		return *(bool *)Code::MapBool.addr;
	}
//Get online index	
	u8 GameHelper::GetOnlinePlayerIndex() {
		return Code::a_GetOnlinePlayerIndex.Call<u8>();
	}
//Get actual index	
	u8 GameHelper::GetActualPlayerIndex() {
		u8 index = *(u8 *)((*(u32 *)Code::GamePointer.addr) + 0x13268);
		if(index >= 4) 
			return 0;
		
		return index;
	}
//Get player count
	u8 GameHelper::GetOnlinePlayerCount() {
		static Address getplayer2(0x75EFF8, 0x75DFDC, 0x75E000, 0x75DFD8, 0x75D798, 0x75D770, 0x75D340, 0x75D340);
		return getplayer2.Call<u8>(*(u32 *)Code::GamePointer.addr);
	}
//Get item at world coords
	u32 *GameHelper::GetItemAtWorldCoords(u32 x, u32 y) {
		static Address WorlditemCoords(0x2FEF9C, 0x2FEB00, 0x2FEF0C, 0x2FEF0C, 0x2FEE38, 0x2FEE38, 0x2FECF0, 0x2FECF0);
		return WorlditemCoords.Call<u32 *>(GetCurrentMap(), x, y, 0);
	}
//Get current map
	u32 GameHelper::GetCurrentMap(void) {
		static Address Currentmap(0x6A690C, 0x6A5E34, 0x6A5944, 0x6A5944, 0x6A53DC, 0x6A53DC, 0x6A4F84, 0x6A4F84);
		return Currentmap.Call<u32>();
	}
//Building Update Collision	
	void GameHelper::PlaceBuildingUpdateCollisions(u32 x, u32 y, u16 buildingID) {
		static Address PlaceBuilding(0x2425D8, 0x24201C, 0x2425D4, 0x2425D4, 0x2424F4, 0x2424F4, 0x2424C0, 0x2424C0);
		PlaceBuilding.Call<void>(x, y, buildingID);
	}
//Remove items with trample
	bool GameHelper::RemoveItems(bool trample, u8 wX, u8 wY, u8 width, u8 length, bool allowAbort, bool removeEverything, bool counting) {
		bool res = false;
		u8 x = wX, y = wY;
		u32 count = 0;
		u32 Empty = 0x7FFE;

		if(!PlayerClass::GetInstance()->IsLoaded()) 
			return res;
		
		static const Address rem1(0x597F54, 0x59746C, 0x596F9C, 0x596F9C, 0x59688C, 0x59688C, 0x596560, 0x596560);
		static const Address rem2(0x597F38, 0x5995DC, 0x596F80, 0x596F80, 0x596870, 0x596870, 0x5986D0, 0x5986D0);
		static const Address rem3(0x597FAC, 0x5974C4, 0x596FF4, 0x596FF4, 0x5968E4, 0x5968E4, 0x5965B8, 0x5965B8);
		
		if(removeEverything) {
			if(!IsIndoorsBool) {
				x = 0x10;
				y = 0x10;
			}
			else {
				x = 0;
				y = 0;
			}
		}
		res = true;
		if(trample) {
			Process::Patch(rem1.addr, 0xE1A00000);
			Process::Patch(rem2.addr, 0xE1A00000);
			Process::Patch(rem3.addr, 0xE1A00000);
		}
		
		while(res && (x - wX < width || removeEverything)) {
			while(res && (y - wY < length || removeEverything)) {
				if((u32)GameHelper::GetItemAtWorldCoords(x, y) != 0) {
					if(*GameHelper::GetItemAtWorldCoords(x, y) != 0x7FFE) {
						count++;
						if(count % 300 == 0) 
							Sleep(Milliseconds(50));

						if(trample) 
							GameHelper::TrampleAt(x, y);
						else 
							Dropper::PlaceItemWrapper(6, 0xFFFFFFFF, &Empty, &Empty, x, y, 0, 0, 0, 0, 0, 0x3C, 0xA5);

						Controller::Update();
						if(Controller::IsKeyPressed(Key::B) && allowAbort) {
							OSD::Notify("Aborted.");
							goto end;
						}
					}
				}
				else 
					res = false;

				y++;
			}
			res = true;
			
			if(removeEverything) 
				y = !IsIndoorsBool ? 0x10 : 0;
			else 
				y = wY;

			x++;
			if((u32)GameHelper::GetItemAtWorldCoords(x, y) == 0) 
				res = false;
		}
	end:
		if(trample) {
			Process::Patch(rem3.addr, 0x0A000039);
			Process::Patch(rem2.addr, 0x0A000056);
			Process::Patch(rem1.addr, 0x0A00004F);
		}
		
		if(counting)
			OSD::Notify(std::to_string(count) << " items removed!");
		
		return true;
	}
//Spawn Particles
	void GameHelper::Particles(u32 particleID, float *floats) {
		static Address particleclass(0x207B90, 0x2075D4, 0x207BB0, 0x207BB0, 0x207AD0, 0x207AD0, 0x207A9C, 0x207A9C);
		static const Address u0(0x976C0E, 0x975BEE, 0x975C06, 0x975C06, 0x96FC06, 0x96EC06, 0x96EC06, 0x96EC06);
		static const Address u1(0xAE6870, 0xAE5664, 0xAE5870, 0xAE5870, 0xADF870, 0xADE870, 0xADE870, 0xADE870);
		if(floats == nullptr) 
			return;

		u32 pInstance = PlayerClass::GetInstance()->Offset();
		if(pInstance == 0)
			return;

		if(*(u32 *)(pInstance + 0x1B4) == 0)
			return;

		particleclass.Call<void>(particleID, floats, u0.addr, u1.addr);
	}
//Clear a locked spot	
	void GameHelper::ClearLockedSpot(u8 wX, u8 wY, u8 roomID, u32 param_4) {
		static Address clearLocked(0x5A1278, 0x5A0790, 0x5A02C0, 0x5A02C0, 0x59E634, 0x59E634, 0x59F884, 0x59F884);
		clearLocked.Call<void>(wX, wY, roomID, param_4);
	}
//Create a locked spot
	u32 GameHelper::CreateLockedSpot(u8 DropID, u8 wX, u8 wY, u8 roomID, bool sendPkt) {
		static Address createLocked(0x5A13C4, 0x5A08DC, 0x5A040C, 0x5A040C, 0x59FCFC, 0x59FCFC, 0x59F9D0, 0x59F9D0);
		static const Address lockspot1(0x5A13C8, 0x5A08E0, 0x5A0410, 0x5A0410, 0x59FD00, 0x59FD00, 0x59F9D4, 0x59F9D4);
		u32 lockspot2 = lockspot1.addr + 4;
		u32 index;
		
		if(*(u32 *)lockspot1.addr != 0xE3A00000) 
			return createLocked.Call<u32>(DropID, wX, wY, roomID, sendPkt);
		
		Process::Patch(lockspot1.addr, 0xE24DD01C);
		Process::Patch(lockspot2, 0xE1A07001);
		index = createLocked.Call<u32>(DropID, wX, wY, roomID, sendPkt);
		Process::Patch(lockspot1.addr, 0xE3A00000);
		Process::Patch(lockspot2, 0xE8BD83F0);
		return index;
	}
//Get index for locked spot
	u32 GameHelper::GetLockedSpotIndex(u8 wX, u8 wY, u8 roomID) {
		static Address getlocked(0x5A11BC, 0x5A06D4, 0x5A0204, 0x5A0204, 0x59FAF4, 0x59FAF4, 0x59F7C8, 0x59F7C8);
		return getlocked.Call<u32>(wX, wY, roomID);
	}
//Trample at specific position	
	void GameHelper::TrampleAt(u8 wX, u8 wY) {		
		u32 pItem = (u32)GameHelper::GetItemAtWorldCoords(wX, wY);
		u32 Empty = 0x7FFE;
		
		if(pItem == 0) 
			return;
		
		u8 room = Player::GetRoom(GameHelper::GetActualPlayerIndex()); 
		if(GameHelper::GetOnlinePlayerCount() != 0) {	
			TramplePkt data { *(u32 *)pItem, room, wX, wY, 0 };
			
			static Address trample(0x625488, 0x6249B0, 0x6244C0, 0x6244C0, 0x623F80, 0x623F80, 0x623B28, 0x623B28);
			trample.Call<void>(0x47, 4, &data, 8);
		}
		
		static Address trample1(0x168E20, 0x168868, 0x168E40, 0x168E40, 0x168E08, 0x168E08, 0x168E08, 0x168E08); 
		trample1.Call<void>(wX, wY, 0, room, &Empty);

		static Address trample3(0x59F144, 0x59E65C, 0x59E18C, 0x59E18C, 0x59DA7C, 0x59DA7C, 0x59D750, 0x59D750);
		trample3.Call<void>(wX, wY, 1);
	}
//checks if inv is full
	bool GameHelper::InvFull() {
		static Address fullinv(0x7631EC, 0x7621D0, 0x7621F4, 0x7621CC, 0x76198C, 0x761964, 0x761534, 0x76150C);
		return fullinv.Call<bool>(PlayerClass::GetInstance()->Offset());
	}
//sets first empty slot
	bool GameHelper::SetItem(u32 *item) {		
		if(Player::GetSaveOffset(4) == 0) 
			return 0;
		
		if(!IDList::ItemValid(*item, false)) 
			return 0;

		static Address writeitem(0x64FDEC, 0x64F314, 0x64EE24, 0x64EE24, 0x64E8E4, 0x64E8E4, 0x64E48C, 0x64E48C);
		return writeitem.Call<bool>(PlayerClass::GetInstance()->Offset(), item);
	}

//Get instance of camera
	u32 Camera::GetInstance() {
		static const Address camera1(0x951884, 0x950874, 0x950880, 0x950880, 0x94A880, 0x949880, 0x949880, 0x949880);
		return *(u32 *)camera1.addr;
	}
	
	float* Camera::GetCoordinates() {
		static const Address camcoord(0x9866F4, 0x9856F4, 0x9856F4, 0x9856F4, 0x97F6F4, 0x97E6F4, 0x97E6F4, 0x97E6F4);
		return (float *)camcoord.addr;
	}

//add float to x	
	void Camera::AddToX(float val) {
		*(float *)(Camera::GetInstance() + 4) += val;
	}

//add float to y	
	void Camera::AddToY(float val) {
		*(float *)(Camera::GetInstance() + 8) += val;
	}

//add float to z	
	void Camera::AddToZ(float val) {
		*(float *)(Camera::GetInstance() + 0xC) += val;
	}
	
//add u16 to y rotation
	void Camera::AddToYRotation(u16 val) {
		*(u16 *)(Camera::GetInstance() + 0x1C) += val;
	}		
}
