#include <CTRPluginFramework.hpp>
#include "cheats.hpp"
#include "RegionCodes.hpp"
#include "TextFileParser.hpp"

extern "C" void PATCH_PartyPop(void);

u32 PartyPopperTool = 0x336A;

namespace CTRPluginFramework {
//Size Codes
	void sizecodes(MenuEntry *entry) {
		static const u32 player = Region::AutoRegion(0x1ACE00, 0x001AC848, 0x001ACE20, 0x001ACE20, 0x1ACD5C, 0x1ACD5C, 0x1ACD5C, 0x1ACD5C);
		static const u32 bug = Region::AutoRegion(0x1F557C, 0x1F4FC4, 0x1F559C, 0x1F559C, 0x1F54D8, 0x1F54D8, 0x1F54D8, 0x1F54D8);
		static const u32 npc = Region::AutoRegion(0x2042BC, 0x203D00, 0x2042DC, 0x2042DC, 0x2041FC, 0x2041FC, 0x2041C8, 0x2041C8);
		static const u32 effect = Region::AutoRegion(0x550A80, 0x54FF98, 0x550C0C, 0x550C0C, 0x54F3B4, 0x54F3B4, 0x54F0D8, 0x54F0D8);
		static const u32 shadow = Region::AutoRegion(0x28F3A4, 0x28EDE8, 0x28F3C4, 0x28F3C4, 0x28F2E4, 0x28F2E4, 0x28F2B0, 0x28F2B0);
		static const u32 town = Region::AutoRegion(0x52E9D0, 0x52E324, 0x52DA18, 0x52DA18, 0x52D304, 0x52D304, 0x52D028, 0x52D028);
		static const u32 horplayer = Region::AutoRegion(0x5680F8, 0x567610, 0x567140, 0x567140, 0x566A30, 0x566A30, 0x566750, 0x566750);
		static const u32 vertplayer = Region::AutoRegion(0x567FF4, 0x56750C, 0x56703C, 0x56703C, 0x56692C, 0x56692C, 0x56664C, 0x56664C);
		static const u32 head = Region::AutoRegion(0x568064, 0x56757C, 0x5670AC, 0x5670AC, 0x56699C, 0x56699C, 0x5666BC, 0x5666BC);
		static const u32 corrupt = Region::AutoRegion(0x47E3F0, 0x47DD68, 0x47D438, 0x47D438, 0x47D0D0, 0x47D0D0, 0x47CF90, 0x47CF90);
		
		static const std::vector<std::string> sizeopt = {
			Language->Get("VECTOR_SIZE_PLAYER"),
			Language->Get("VECTOR_SIZE_BUGFISH"),
			Language->Get("VECTOR_SIZE_NPC"),
			Language->Get("VECTOR_SIZE_EFFECT"),
			Language->Get("VECTOR_SIZE_SHADOW"),
			Language->Get("VECTOR_SIZE_TOWN"),
			Language->Get("VECTOR_SIZE_HORI"),
			Language->Get("VECTOR_SIZE_VERT"),
			Language->Get("VECTOR_SIZE_HEAD"),
			Language->Get("VECTOR_SIZE_CORRUPT"),
			Language->Get("VECTOR_SIZE_RESET")
		};
		
		std::vector<std::string> sizesopt = {
			Language->Get("VECTOR_SIZE_BIGGER"),
			Language->Get("VECTOR_SIZE_DEFAULT"),
			Language->Get("VECTOR_SIZE_SMALLER"),
			"Custom"
		};
		
		static const u32 sizer[10] = { player, bug, npc, effect, shadow, town, horplayer, vertplayer, head, corrupt };
		
		static constexpr u32 sizes[3] = { 0x40000000, 0x3F800000, 0x3F000000 };

		bool IsON;

		float size = 0.0;
		
		Keyboard optKb(Language->Get("KEY_CHOOSE_OPTION"));
		optKb.Populate(sizeopt);
		Sleep(Milliseconds(100));
		int op = optKb.Open();
		if(op == -1)
			return;
			
		if(op <= 9) {
			for(int i = 0; i < 3; ++i) {
				IsON = *(u32 *)sizer[op] == sizes[i];
				sizesopt[i] = IsON ? (Color(pGreen) << sizesopt[i]) : (Color(pRed) << sizesopt[i]);	
			}
			
			optKb.Populate(sizesopt);
			s8 op2 = optKb.Open();
			if(op2 < 0)
				return;

			if(op2 == 3) {
				if(optKb.Open(size, size) >= 0)
					Process::WriteFloat(sizer[op], size);
			}
			else
				Process::Patch(sizer[op], sizes[op2]);

			sizecodes(entry);
			return;
		}
		
		if(op == 10) 
			for(int i = 0; i < 10; i++) 
				Process::Patch(sizer[i], sizes[1]);
    }
//T-Pose
	void tposeentry(MenuEntry *entry) { 
		static const u32 tpose = Region::AutoRegion(0x73C290, 0x73B5D8, 0x73B298, 0x73B270, 0x73AA30, 0x73AA08, 0x73A5D8, 0x72EBD8);
		std::vector<std::string> cmnOpt =  { "" };

		bool IsON = *(u32 *)tpose == 0xE1A00000;

		cmnOpt[0] = IsON ? (Color(pGreen) << Language->Get("VECTOR_ENABLED")) : (Color(pRed) << Language->Get("VECTOR_DISABLED"));
		
		Keyboard optKb(Language->Get("KEY_CHOOSE_OPTION"));
		optKb.Populate(cmnOpt);
		Sleep(Milliseconds(100));
		s8 op = optKb.Open();
		if(op < 0)
			return;
			
		Process::Patch(tpose, IsON ? 0x0A000011 : 0xE1A00000);
		tposeentry(entry);
	}
//OSD For Take TPC Picture
	bool tpcoverlay(const Screen &screen) {
		if(screen.IsTop) {
			screen.DrawRect(154, 50, 90, 6, Color::White); //top bar
			screen.DrawRect(154, 50, 6, 130, Color::White); //left bar
			screen.DrawRect(154, 177, 90, 6, Color::White); //bottom bar
			screen.DrawRect(240, 50, 6, 133, Color::White); //right bar
		}
		return 1;
	}
//Take TPC Picture
	void freezeframe(MenuEntry *entry) {
		static const u32 freeze = Region::AutoRegion(0x54DBE8, 0x54D100, 0x54CC30, 0x54CC30, 0x54C51C, 0x54C51C, 0x54C240, 0x54C240);
		if(entry->Hotkeys[0].IsPressed()) {
			if(Player::GetSaveOffset(4) == 0) {
				OSD::Notify("Player needs to be loaded!", Color::Red);
				return;
			}

			static const u32 CreateTPC = Region::AutoRegion(0x5B3594, 0x5B2AAC, 0x5B25DC, 0x5B25DC, 0x5B1ECC, 0x5B1ECC, 0x5B1BA0, 0x5B1BA0);
			static const u32 TPCPoint = Region::AutoRegion(0x954F10, 0x953EF0, 0x953F08, 0x953F08, 0x94DF08, 0x94CF08, 0x94CF08, 0x94CF08);

			static FUNCT func(CreateTPC);
			func.Call<void>(*(u32 *)TPCPoint, PlayerPTR::Pointer(0x5734));
			GameHelper::PlaySound(0x4A7);
		}
	
		if(entry->Hotkeys[1].IsDown()) {
			if(entry->Hotkeys[1].IsPressed())
				Process::Patch(freeze, 0xE3A000FF);

			OSD::Run(tpcoverlay);
		}
		
		if(!entry->Hotkeys[1].IsDown()) {
			OSD::Stop(tpcoverlay);
			Process::Patch(freeze, 0xE0800001);
		}

		if(!entry->IsActivated())
			Process::Patch(freeze, 0xE0800001);
    }

//Max Turbo Presses
	void maxturbo(MenuEntry *entry) { 
		u32 maxT = *(u32 *)Code::max - 0x31C;

        Sleep(Seconds(0.0085F));
        Process::Write8(maxT, 0); //abxy
        Process::Write8(maxT + 2, 0); //dpad
    }

//Multi-presses
	void asmpresses(MenuEntry *entry) { 
		static const u32 press = Region::AutoRegion(0x5C5BEC, 0x5C511C, 0x5C4C34, 0x5C4C34, 0x5C4524, 0x5C4524, 0x5C41F8, 0x5C41F8);
		if(entry->WasJustActivated()) 
			Process::Patch(press, 0xE1A00000); 
		else if(!entry->IsActivated()) 
			Process::Patch(press, 0x0A000028); 
	}
//Ultimate Party Popper	
	void partypopper(MenuEntry *entry) {
		static const u32 PartySnakeSpeed = Region::AutoRegion(0x67F008, 0x67E530, 0x67E040, 0x67E040, 0x67DB00, 0x67DB00, 0x67D6A8, 0x67D6A8);
		static const u32 party2 = Region::AutoRegion(0x662D9C, 0x6622C4, 0x661DD4, 0x661DD4, 0x661894, 0x661894, 0x66143C, 0x66143C);
		static const u32 party3 = Region::AutoRegion(0x67BBC8, 0x67B0F0, 0x67AC00, 0x67AC00, 0x67A6C0, 0x67A6C0, 0x67A268, 0x67A268);
		static const u32 PartyItemID = Region::AutoRegion(0x671874, 0x670D9C, 0x6708AC, 0x6708AC, 0x67036C, 0x67036C, 0x66FF14, 0x66FF14);
		static const u32 PartyEffect = Region::AutoRegion(0x671880, 0x670DA8, 0x6708B8, 0x6708B8, 0x670378, 0x670378, 0x66FF20, 0x66FF20);
		
		static u16 PartyEffectID = 0x20A;

		Process::Write8(*(u32 *)Code::max - 0x31C, 0); //Multi Presses

		static Hook hook1, hook2;
		
		static const u32 PartyPop[3] = { PartySnakeSpeed, party2, party3 };
		
		static const u32 PartyPopPatch[2][3] = {
            { 0x50000000, 0x40000000, 0x40000000 },
            { 0x3F800000, 0x3F800000, 0x3F800000 }
        };

		if(entry->WasJustActivated()) {
			hook1.Initialize(PartyItemID - 0x1E0, (u32)PATCH_PartyPop);
			hook2.Initialize(PartyItemID - 0x1B4, (u32)PATCH_PartyPop);

			hook1.SetFlags(USE_LR_TO_RETURN);
			hook2.SetFlags(USE_LR_TO_RETURN);
		}

		if(Controller::IsKeysPressed(entry->Hotkeys[0].GetKeys())) 
			Wrap::KB<u16>(Language->Get("ULTIMATE_PARTY_POPPER_ENTER_EFFECT"), true, 3, PartyEffectID, PartyEffectID);

		if(Player::GetTool() == 0x336A) {
			Player::SetTool(0x336A);
			Process::Patch(PartyEffect, PartyEffectID);

			for(int i = 0; i < 3; ++i)
            	Process::Patch(PartyPop[i], PartyPopPatch[0][i]);

			hook1.Enable();
			hook2.Enable();
		}
		else {
			Process::Patch(PartyEffect, 0x20A);

			for(int i = 0; i < 3; ++i)
            	Process::Patch(PartyPop[i], PartyPopPatch[1][i]);

			hook1.Disable();
			hook2.Disable();
		}
   
		if(!entry->IsActivated()) {
			for(int i = 0; i < 3; ++i)
               Process::Patch(PartyPop[i], PartyPopPatch[1][i]);	
			
			Process::Patch(PartyEffect, 0x20A);

			hook1.Disable();
			hook2.Disable();
		}
	}
	
	void cameramod(MenuEntry *entry) {
    //pointers & addresses
		static const u32 cameraAsm = Region::AutoRegion(0x764504, 0x7634E8, 0x76350C, 0x7634E4, 0x762CA4, 0x762C7C, 0x76284C, 0x762824);
        static const u32 rotationAsm = Region::AutoRegion(0x1A3230, 0x1A2C78, 0x1A3250, 0x1A3250, 0x1A3190, 0x1A3190, 0x1A3190, 0x1A3190);
        static const u32 camerapan = Region::AutoRegion(0x1A2058, 0x1A1AA0, 0x1A2078, 0x1A2078, 0x1A1FB8, 0x1A1FB8, 0x1A1FB8, 0x1A1FB8);

    //variables
        static bool isPatched = false;
		static bool isOn = false;

		static const u16 OrigVal[2] = { *(u16 *)(Camera::GetInstance() + 0x12C), *(u16 *)(Camera::GetInstance() + 0x12E) };
		
		static Clock time;
        Time delta = time.Restart();

        float speed = 400.0f * delta.AsSeconds();
        u16 difference = 0x1000 * delta.AsSeconds();

		float *coord = Camera::GetCoordinates();

		if(entry->WasJustActivated()) 
			Process::Patch(camerapan, 0xE3A00000); //disables camera panning

		if(!entry->IsActivated()) {
			if(coord != nullptr)
				coord = PlayerClass::GetInstance()->GetCoordinates();

			Animation::ExecuteAnimationWrapper(4, 6, 0, 0, 0, 0, 0, 0, 0, 0); //idles player
			Process::Patch(cameraAsm, 0x2A000020);
			Process::Patch(rotationAsm, 0xE18020B4);
            Process::Write32(rotationAsm + 0xC, 0xE18020B4);
			Process::Patch(camerapan, 0xE3A00009);
			*(u16 *)(Camera::GetInstance() + 0x12C) = OrigVal[0];
			*(u16 *)(Camera::GetInstance() + 0x12E) = OrigVal[1];
			return;
		}
	
        if(Camera::GetInstance() != 0) {
        //check if you're outside
            if(!Player::IsIndoors()) {
                if(GameHelper::RoomCheck() == 1) {
                    Process::Patch(rotationAsm, 0xE18020B4);
                    Process::Patch(rotationAsm + 0xC, 0xE18020B4);
                }
                else {
                    Process::Patch(rotationAsm, 0xE1A00000);
                    Process::Patch(rotationAsm + 0xC, 0xE1A00000);
                }
            }
            else {
                Process::Patch(rotationAsm, 0xE18020B4);
                Process::Patch(rotationAsm + 0xC, 0xE18020B4);
            }
			
            if(Controller::IsKeyDown(Key::R)) {
                if(Controller::IsKeyDown(Key::CPadUp))
                    *(u16 *)(Camera::GetInstance() + 0x12C) += difference;
                if(Controller::IsKeyDown(Key::CPadDown))
                   *(u16 *)(Camera::GetInstance() + 0x12C) -= difference;
                if(Controller::IsKeyDown(Key::CPadLeft))
                    *(u16 *)(Camera::GetInstance() + 0x12E) += difference;
                if(Controller::IsKeyDown(Key::CPadRight))
                    *(u16 *)(Camera::GetInstance() + 0x12E) -= difference;
            }
		//Stop/follow camera from moving
            if(Controller::IsKeysPressed(Key::R + Key::X)) {
				switch(isPatched) {
					case 0: goto patch;
					case 1: goto unpatch;
				}
			}
			
		//lock/unlock player
            if(Controller::IsKeysPressed(Key::R + Key::Y)) {
				switch(isOn) {
					case 0: 
						OSD::Notify("Player: " << Color::Red << "Locked"); 
						Animation::ExecuteAnimationWrapper(4, 0xF, 0, 0, 0, 0, 0, 0, 0, 0);
						isOn = true;
					break;
					case 1: 
						OSD::Notify("Player: " << Color::Green << "Unlocked");
						Animation::ExecuteAnimationWrapper(4, 6, 0, 0, 0, 0, 0, 0, 0, 0);
						isOn = false;
					break;
				}
			}

			if(Controller::IsKeyDown(Key::B)) {
				if(!PlayerClass::GetInstance()->IsLoaded())
					return;
					
				if(coord == nullptr)
					return;

				if(Controller::IsKeyDown(Key::DPadUp)) {
					coord[2] -= speed;
					goto patch;
				}

				if(Controller::IsKeyDown(Key::DPadRight)) {
					coord[0] += speed;
					goto patch;
				}

				if(Controller::IsKeyDown(Key::DPadDown)) {
					coord[2] += speed;
					goto patch;
				}

				if(Controller::IsKeyDown(Key::DPadLeft)) {
					coord[0] -= speed;
					goto patch;
				}

				if(Controller::IsKeyDown(Key::L)) {
					coord[1] -= speed;
					goto patch;
				}

				if(Controller::IsKeyDown(Key::R)) {
					coord[1] += speed;
					goto patch;
				}
			}
            return;
        patch:
            if(!isPatched) {
            //disable camera following
				OSD::Notify("Camera following: " << Color::Red << "OFF"); 
                Process::Patch(cameraAsm, 0xEA000020);
                isPatched = true;
            }
            return;
        unpatch:
            if(isPatched) {
			//reenable camera followig
				OSD::Notify("Camera following: " << Color::Green << "ON"); 
                Process::Patch(cameraAsm, 0x2A000020);
                isPatched = false;
            }
        }
    }

//Growing 1 Trees	
	static const u16 Growing_Trees[79] { 
		0x22, 0x23, 0x24, 0x25, //tree
		0x27, 0x28, 0x29, 0x2A, //cedar
		0x2C, 0x2D, 0x2E, 0x2F, //coconut palm tree
		0x31, 0x32, 0x33, 0x34, //banana palm tree
		0x36, 0x37, 0x38,		//bamboo
		0x3A, 0x3B, 0x3C, 0x3D, //apple tree
		0x3F, 0x40, 0x41, 0x42, //orange tree
		0x44, 0x45, 0x46, 0x47, //pear tree
		0x49, 0x4A, 0x4B, 0x4C, //peaches tree
		0x4E, 0x4F, 0x50, 0x51, //cherry tree
		0x53, 0x54, 0x55, 0x56, //durian tree
		0x58, 0x59, 0x5A, 0x5B, //lemon tree
		0x5D, 0x5E, 0x5F, 0x60, //lychee tree
		0x62, 0x63, 0x64, 0x65, //mango tree
		0x67, 0x68, 0x69, 0x6A, //persimmons tree
		0x6C, 0x6D, 0x6E, 0x6F, //money tree
		0x80, 0x81, 			//holly bush
		0x83, 0x84, 			//pink azalea bush
		0x86, 0x87, 			//white azalea bush
		0x89, 0x8A, 			//blue hydrangea bush
		0x8C, 0x8D, 			//pink hydrangea bush
		0x8F, 0x90, 			//red hibiscus bush
		0x92, 0x93, 			//yellow hibiscus bush
		0x95, 0x96				//sweet olive bush
	};
	//549
	//550
	//581/583
	
//wand abilitys
	void wandability(MenuEntry *entry) {
	//If A is not pressed return
		if(!Controller::IsKeyPressed(Key::A)) 
			return;	

	//If player is not loaded return
		if(!PlayerClass::GetInstance()->IsLoaded())
			return;

	//If player is not idling or running return
		if(*PlayerClass::GetInstance()->GetAnimation() != 6 && *PlayerClass::GetInstance()->GetAnimation() != 0xD)
			return;
		
		switch(Player::GetTool()) {
			default: break;
		/*If Blue Wand*/
			case 0x3398: {

			} break;
		/*If Green Wand | Restores wilted flowers instantly*/
			case 0x3399: {
				if(!bypassing) 
					GameHelper::DropItemLock(true);

				u32 x, y;
			//If no World Coords found | Either Player not loaded or other error
				if(!PlayerClass::GetInstance()->GetWorldCoords(&x, &y))
					return;

			//If Item is 0 return | 0 means no item-offset
				u32 Item = (u32)GameHelper::GetItemAtWorldCoords(x, y);	
				if(Item == 0) 
					return;

				u32 PlaceItem = *(u32 *)Item;
				
			//If ID is not wilted flower return
				if(!IDList::ValidID((u16)PlaceItem, 0xCE, 0xF7))
					return;

			//Removes Wilted Flower	
				GameHelper::RemoveItems(true, x, y, 1, 1, false, false, false);
			//Some neat particles for a nice effect | do it 20 times to spam it	
				for(int i = 0; i <= 20; ++i) 
					GameHelper::Particles(0x10B, PlayerClass::GetInstance()->GetCoordinates(x, y));	

				//GameHelper::PlaySound(0);

			//Places Fixed Flower
				PlaceItem -= 0x2F; //Jumps to fixed flower 
				Dropper::PlaceItemWrapper(0xC, 0xFFFFFFFF, &PlaceItem, &PlaceItem, x, y, 0, 0, 0, 0, 0, 0x5C, 0xA5, false);

				if(!bypassing) 
					GameHelper::DropItemLock(false);
			} break;
		/*If Pink Wand*/
			case 0x339A: {

			} break;
		/*If Yellow Wand*/
			case 0x339B: {

			} break;
		/*If Flower Fairy Wand*/
			case 0x339C: {
				if(!bypassing) 
					GameHelper::DropItemLock(true);
				u32 x, y;
			//If no World Coords found | Either Player not loaded or other error
				if(!PlayerClass::GetInstance()->GetWorldCoords(&x, &y))
					return;
			//If Item is 0 return | 0 means no item-offset
				u32 Item = (u32)GameHelper::GetItemAtWorldCoords(x, y);	
				if(Item == 0) 
					return;
				
				u32 PlaceItem = *(u32 *)Item;
			//If no growing tree found return	
				if(!(std::find(std::begin(Growing_Trees), std::end(Growing_Trees), (PlaceItem & 0xFFFF)) != std::end(Growing_Trees)))
					return;
				
			//Removes growing tree
				GameHelper::RemoveItems(true, x, y, 1, 1, false, false, false);		
			//Some neat particles for a nice effect | do it 20 times to spam it	
				for(int i = 0; i <= 20; ++i) 
					GameHelper::Particles(0x19A, PlayerClass::GetInstance()->GetCoordinates(x, y));				
			//Places Grown Tree
				PlaceItem += 1; //Jumps to next growth level
				Dropper::PlaceItemWrapper(0xC, 0xFFFFFFFF, &PlaceItem, &PlaceItem, x, y, 0, 0, 0, 0, 0, 0x5C, 0xA5, false);

				if(!bypassing) 
					GameHelper::DropItemLock(false);
			} break;
		/*If Kiki and Lala Wand | Stores outfits for you and lets you select them*/
			case 0x339D: {

			} break;
		}
	}
}
