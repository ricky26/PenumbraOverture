/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of Penumbra Overture.
 *
 * Penumbra Overture is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Penumbra Overture is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Penumbra Overture.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ButtonHandler.h"

#include "Init.h"
#include "Player.h"
#include "Inventory.h"
#include "Notebook.h"
#include "NumericalPanel.h"
#include "IntroStory.h"
#include "SaveHandler.h"
#include "MapHandler.h"
#include "DeathMenu.h"
#include "MapLoadText.h"
#include "PreMenu.h"
#include "Credits.h"
#include "DemoEndText.h"

#include "MainMenu.h"

struct cButtonHandlerAction
{
	tString msName;
	tString msType;
	int mlVal;
	bool mbConfig;
};

static tString gsLastPlayerAction = "GlowStick";
static cButtonHandlerAction gvDefaultActions[] = {
{"Forward","Keyboard",eKey_w,true},
{"Backward","Keyboard",eKey_s,true},
{"Left","Keyboard",eKey_a,true},
{"Right","Keyboard",eKey_d,true},

{"LeanLeft","Keyboard",eKey_q,true},
{"LeanRight","Keyboard",eKey_e,true},

{"Run","Keyboard",eKey_LSHIFT,true},
{"Jump","Keyboard",eKey_SPACE,true},
{"Crouch","Keyboard",eKey_LCTRL,true},

{"InteractMode","Keyboard",eKey_r,true},
{"LookMode","MouseButton",eMButton_Middle,true},

{"Holster","Keyboard",eKey_x,true},

{"Examine","MouseButton",eMButton_Right,true},
{"Interact","MouseButton",eMButton_Left,true},

{"Inventory","Keyboard",eKey_TAB,true},
{"NoteBook","Keyboard",eKey_n,true},
{"PersonalNotes","Keyboard",eKey_p,true},

{"WheelUp","MouseButton",eMButton_WheelUp,true},
{"WheelDown","MouseButton",eMButton_WheelDown,true},

{"Flashlight","Keyboard",eKey_f,true},
{"GlowStick","Keyboard",eKey_g,true},

{"Escape","Keyboard",eKey_ESCAPE,false},
{"Enter","Keyboard",eKey_RETURN,false},
{"MouseClick","MouseButton",eMButton_Left,false},
{"MouseClickRight","MouseButton",eMButton_Right,false},

{"RightClick","MouseButton",eMButton_Right,false},
{"LeftClick","MouseButton",eMButton_Left,false},

{"One","Keyboard",eKey_1,false},
{"Two","Keyboard",eKey_2,false},
{"Three","Keyboard",eKey_3,false},
{"Four","Keyboard",eKey_4,false},
{"Five","Keyboard",eKey_5,false},
{"Six","Keyboard",eKey_6,false},
{"Seven","Keyboard",eKey_7,false},
{"Eight","Keyboard",eKey_8,false},
{"Nine","Keyboard",eKey_9,false},

//Debug:
{"ResetGame","Keyboard",eKey_F1,false},
{"SaveGame","Keyboard",eKey_F4,false},
{"LoadGame","Keyboard",eKey_F5,false},
#ifdef __APPLE__
{"QuitGame","Keyboard",eKeyModifier_META | eKey_q,false},
#endif
//{"LockInput","Keyboard",eKey_k,false},
{"Screenshot","Keyboard",eKey_F12,false},

//{"Hit","Keyboard",eKey_h,false},
//{"Log","Keyboard",eKey_l,false},
//{"Taunt","Keyboard",eKey_t,false},
{"PrintLog","Keyboard",eKey_l,false},

{"","",0}
};

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

#define actionGroup_magic apInit->mpGame->GetInput()->GetActionGroup()
cButtonHandler::cButtonHandler(cInit *apInit)  : iUpdateable("ButtonHandler"),
	mActionQuit("Quit", actionGroup_magic),

	mActionForward("Forward", actionGroup_magic),
	mActionBackward("Backward", actionGroup_magic),
	mActionLeft("Left", actionGroup_magic),
	mActionRight("Right", actionGroup_magic),
	mActionLeanLeft("LeanLeft", actionGroup_magic),
	mActionLeanRight("LeanRight", actionGroup_magic),
	mActionLookUp("LookUp", actionGroup_magic),
	mActionLookDown("LookDown", actionGroup_magic),
	mActionLookLeft("LookLeft", actionGroup_magic),
	mActionLookRight("LookRight", actionGroup_magic),

	mActionLookX("LookX", actionGroup_magic),
	mActionLookY("LookY", actionGroup_magic),
	mActionMoveX("MoveX", actionGroup_magic),
	mActionMoveY("MoveY", actionGroup_magic),
	mActionLean("Lean", actionGroup_magic),

	mActionRun("Run", actionGroup_magic),
	mActionJump("Jump", actionGroup_magic),
	mActionCrouch("Crouch", actionGroup_magic),
	mActionInteractMode("InteractMode", actionGroup_magic),
	mActionLookMode("LookMode", actionGroup_magic),
	mActionHolster("Holster", actionGroup_magic),
	mActionExamine("Examine", actionGroup_magic),
	mActionInteract("Interact", actionGroup_magic),
	mActionInventory("Inventory", actionGroup_magic),
	mActionNotebook("Notebook", actionGroup_magic),
	mActionPersonalNotes("PersonalNotes", actionGroup_magic),
	mActionFlashlight("Flashlight", actionGroup_magic),
	mActionGlowStick("GlowStick", actionGroup_magic),
	mActionEnter("Enter", actionGroup_magic),
	mActionEscape("Escape", actionGroup_magic),
	mActionClick("Click", actionGroup_magic),
	mActionRightClick("RightClick", actionGroup_magic),
	mActionScrollUp("ScrollUp", actionGroup_magic),
	mActionScrollDown("ScrollDown", actionGroup_magic),
	mActionOne("One", actionGroup_magic),
	mActionTwo("Two", actionGroup_magic),
	mActionThree("Three", actionGroup_magic),
	mActionFour("Four", actionGroup_magic),
	mActionFive("Five", actionGroup_magic),
	mActionSix("Six", actionGroup_magic),
	mActionSeven("Seven", actionGroup_magic),
	mActionEight("Eight", actionGroup_magic),
	mActionNine("Nine", actionGroup_magic),
	mActionResetGame("ResetGame", actionGroup_magic),
	mActionSaveGame("SaveGame", actionGroup_magic),
	mActionLoadGame("LoadGame", actionGroup_magic),
	mActionScreenshot("Screenshot", actionGroup_magic),
	mActionPrintLog("PrintLog", actionGroup_magic)

#undef actionGroup_magic
{
	mpInit = apInit;

	mpInput = mpInit->mpGame->GetInput();
	iKeyboard *kbd = mpInput->GetKeyboard();
	iMouse *ms = mpInput->GetMouse();

	// MAGICAL HARD-CODED INPUT MAP! -- Ricky26
	//mActionQuit.Bind(kbd->GetButton(eKey_q));
	//mActionClick.Bind(ms->GetMouseButton(0));
	//mActionRightClick.Bind(ms->GetMouseButton(1));
	//mActionEscape.Bind(kbd->GetButton(eKey_ESCAPE));

	mpLowLevelGraphics = mpInit->mpGame->GetGraphics()->GetLowLevel();
	if(mpInit->mbHasHaptics)
		mpLowLevelHaptic = mpInit->mpGame->GetHaptic()->GetLowLevel();
	else
		mpLowLevelHaptic = NULL;

	
	mState = eButtonHandlerState_Game;

	// LOAD LAYOUT
	tWString sPersonalDir = GetSystemSpecialPath(eSystemPath_Personal);
	if(	cString::GetLastCharW(sPersonalDir) != _W("/") && 
		cString::GetLastCharW(sPersonalDir) != _W("\\"))
	{
		sPersonalDir += _W("/");
	}

	// CREATE NEEDED DIRS /////////////////////
	if(!mpInput->LoadLayout(sPersonalDir + PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME _W("layout.xml")))
	{
		mpInput->LoadLayout(_W("config/layout.xml"));
	}

	/*mlNumOfActions =0;

	// INIT ALL ACTIONS USED
	cButtonHandlerAction *pBHAction = &gvDefaultActions[0];
	while(pBHAction->msName != "")
	{
		tString sName = pBHAction->msName;
		tString sType = mpInit->mpConfig->GetString("Keys",sName+"_Type",pBHAction->msType);
		tString sVal = mpInit->mpConfig->GetString("Keys",sName+"_Val",cString::ToString(pBHAction->mlVal));

		iAction *pAction = ActionFromTypeAndVal(sName, sType, sVal);
		if(pAction)
		{
			mpInput->AddAction(pAction);
		}
		else
		{
			Warning("Couldn't create action from '%s' and %d\n",pBHAction->msType.c_str(),
																pBHAction->mlVal);
		}

		++pBHAction;
		++mlNumOfActions;
	}*/


	// LOAD SETTINGS
	mfMouseSensitivity = mpInit->mpConfig->GetFloat("Controls","MouseSensitivity",1.0f);
	mbInvertMouseY = mpInit->mpConfig->GetBool("Controls","InvertMouseY",false);
	mbToggleCrouch = mpInit->mpConfig->GetBool("Controls","ToggleCrouch",true);
}

//-----------------------------------------------------------------------

cButtonHandler::~cButtonHandler(void)
{
	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cButtonHandler::ChangeState(eButtonHandlerState aState)
{
	mState = aState;
}

//-----------------------------------------------------------------------

void cButtonHandler::OnStart()
{
	mpPlayer = mpInit->mpPlayer;
}

//-----------------------------------------------------------------------

void cButtonHandler::OnPostSceneDraw()
{
	if(mpInit->mpGame->GetGraphics()->GetRenderer3D()->GetDebugFlags() & eRendererDebugFlag_LogRendering)
	{
		Log("-------------- STOP RENDERING LOG ------------------------\n");
		mpInit->mpGame->GetGraphics()->GetRenderer3D()->SetDebugFlags(0);
	}
}

float float_clamp(float v, float a=-1, float b=1)
{
	if(v < a)
		return a;
	
	if(v > b)
		return b;

	return v;
}

void cButtonHandler::Update(float afTimeStep)
{
	static bool bLockState = true;
	///////////////////////////////////
	// GLOBAL Key Strokes
	///////////////////////////////////
	if(mActionQuit.BecameTriggered())
	{
		mpInit->mpGame->Exit();
	}
	if(mActionScreenshot.BecameTriggered())
	{
		int lCount = 1;
		tString sFileName = "screenshot000.bmp";
		while(FileExists(cString::To16Char(sFileName)))
		{
			sFileName = "screenshot";
            if(lCount < 10)sFileName+= "00";
			else if(lCount < 100)sFileName+= "0";
			sFileName += cString::ToString(lCount);
			sFileName += ".bmp";
			++lCount;
		}

		mpInit->mpGame->GetGraphics()->GetLowLevel()->SaveScreenToBMP(sFileName);
	}

	// TODO: WTH is this? -- Ricky26
	/*if(mpInput->BecameTriggerd("LockInput"))
	{
#ifndef WIN32
		bLockState = !bLockState;
		mpInit->mpGame->GetInput()->GetLowLevel()->LockInput(bLockState);
#endif
	}*/

	///////////////////////////////////
	// DEMO END TEXT
	///////////////////////////////////
	if(mState == eButtonHandlerState_DemoEndText)
	{
		if(mActionEscape.BecameTriggered())	
			mpInit->mpDemoEndText->OnButtonDown();
		if(mActionClick.BecameTriggered())
			mpInit->mpDemoEndText->OnMouseDown(eMButton_Left);
		if(mActionRightClick.BecameTriggered())
			mpInit->mpDemoEndText->OnMouseDown(eMButton_Right);
	}
	///////////////////////////////////
	// CREDITS STATE
	///////////////////////////////////
	else if(mState == eButtonHandlerState_Credits)
	{
		if(mActionEscape.BecameTriggered())	
			mpInit->mpCredits->OnButtonDown();
		if(mActionClick.BecameTriggered())
			mpInit->mpCredits->OnMouseDown(eMButton_Left);
		if(mActionRightClick.BecameTriggered())
			mpInit->mpCredits->OnMouseDown(eMButton_Right);
	}
	///////////////////////////////////
	// PRE MENU STATE
	///////////////////////////////////
	else if(mState == eButtonHandlerState_PreMenu)
	{
		if(mActionEscape.BecameTriggered())	
			mpInit->mpPreMenu->OnButtonDown();
		if(mActionClick.BecameTriggered())
			mpInit->mpPreMenu->OnMouseDown(eMButton_Left);
		if(mActionRightClick.BecameTriggered())
			mpInit->mpPreMenu->OnMouseDown(eMButton_Right);
	}
	///////////////////////////////////
	// MAP LOAD TEXT STATE
	///////////////////////////////////
	else if(mState == eButtonHandlerState_MapLoadText)
	{
		if(mActionEscape.BecameTriggered() ||
			mActionClick.BecameTriggered() ||
			mActionRightClick.BecameTriggered())
		{
			mpInit->mpMapLoadText->SetActive(false);
		}
	}
	///////////////////////////////////
	// MAIN MENU BUTTON STATE
	///////////////////////////////////
	else if(mState == eButtonHandlerState_MainMenu)
	{
		if(mActionEscape.BecameTriggered())
		{
			mpInit->mpMainMenu->Exit();
		}

		if(mActionRightClick.BecameTriggered())
		{
			mpInit->mpMainMenu->OnMouseDown(eMButton_Right);
			mActionExamine.Trigger();
		}
		if(mActionRightClick.WasTriggered())
		{
			mpInit->mpMainMenu->OnMouseUp(eMButton_Right);
		}
		if(mActionRightClick.DoubleTriggered(0.15f))
		{
			mpInit->mpMainMenu->OnMouseDoubleClick(eMButton_Right);
		}

		if(mActionClick.BecameTriggered())
		{
			mpInit->mpMainMenu->OnMouseDown(eMButton_Left);
			mActionInteract.Trigger();
		}
		if(mActionClick.WasTriggered())
		{
			mpInit->mpMainMenu->OnMouseUp(eMButton_Left);
		}
		if(mActionClick.DoubleTriggered(0.15f))
		{
			mpInit->mpMainMenu->OnMouseDoubleClick(eMButton_Left);
		}

		if(mActionLookUp.IsTriggered())
		{
			mpInit->mpMainMenu->AddMousePos(cVector2f(0, -10));
		}
		if(mActionLookDown.IsTriggered())
		{
			mpInit->mpMainMenu->AddMousePos(cVector2f(0, 10));
		}
		if(mActionLookLeft.IsTriggered())
		{
			mpInit->mpMainMenu->AddMousePos(cVector2f(-10, 0));
		}
		if(mActionLookRight.IsTriggered())
		{
			mpInit->mpMainMenu->AddMousePos(cVector2f(10, 0));
		}

		if(mpInit->mbHasHaptics)
		{
			mpInit->mpMainMenu->AddMousePos(mpLowLevelHaptic->GetRelativeVirtualMousePos() * mfMouseSensitivity);

			cVector2f vRel = mpInput->GetMouse()->GetRelPosition();
			mpInit->mpMainMenu->AddMousePos(vRel * mfMouseSensitivity);
		}
		else
		{
			/// Mouse Movement
			cVector2f vRel = mpInput->GetMouse()->GetRelPosition() + cVector2f(mActionLookX.TriggeredAmount(), mActionLookY.TriggeredAmount());
			mpInit->mpMainMenu->AddMousePos(vRel * mfMouseSensitivity);
		}
		
	}
	///////////////////////////////////
	// INTRO BUTTON STATE
	///////////////////////////////////
	else if(mState == eButtonHandlerState_Intro)
	{
		if(mActionEscape.BecameTriggered())
		{
			mpInit->mpIntroStory->Exit();
		}
	}
	///////////////////////////////////
	// GAME BUTTON STATE
	///////////////////////////////////
	else if(mState == eButtonHandlerState_Game)
	{
		///////////////////////////////////////
		// Global ////////////////////
		//if(mpInput->BecameTriggerd("ResetGame"))
		//{
		//	mpInit->ResetGame(true);
		//	mpInit->mpMapHandler->Load(	mpInit->msStartMap,mpInit->msStartLink);
		//}
		if(mpInit->mbAllowQuickSave)
		{
			if(mActionSaveGame.BecameTriggered())
			{
				mpInit->mpSaveHandler->AutoSave(_W("auto"),5);
			}
			if(mActionSaveGame.BecameTriggered())
			{
				mpInit->mpSaveHandler->AutoLoad(_W("auto"));
			}
		}
		if(mActionPrintLog.BecameTriggered())
		{
			Log("-------------- START RENDERING LOG ------------------------\n");
			mpInit->mpGame->GetGraphics()->GetRenderer3D()->SetDebugFlags(eRendererDebugFlag_LogRendering);
		}
		//Check if no jump is pressed always.
		bool bPlayerStateIsActive = false;
		///////////////////////////////////////
		// Death menu ////////////////////
		if(mpInit->mpDeathMenu->IsActive())
		{
			if(mActionEscape.BecameTriggered())
			{
				mpInit->mpGame->GetUpdater()->Reset();
				mpInit->mpMainMenu->SetActive(true);
			}

			if(mActionRightClick.BecameTriggered())
			{
				mpInit->mpDeathMenu->OnMouseDown(eMButton_Right);
				mActionExamine.Trigger();
			}

			if(mActionClick.BecameTriggered())
			{
				mpInit->mpDeathMenu->OnMouseDown(eMButton_Left);
				mActionInteract.Trigger();
			}
			if(mActionClick.WasTriggered())
			{
				mpInit->mpDeathMenu->OnMouseUp(eMButton_Left);
			}

			if(mActionLookUp.IsTriggered())
			{
				mpInit->mpDeathMenu->AddMousePos(cVector2f(0, -10));
			}
			if(mActionLookDown.IsTriggered())
			{
				mpInit->mpDeathMenu->AddMousePos(cVector2f(0, 10));
			}
			if(mActionLookLeft.IsTriggered())
			{
				mpInit->mpDeathMenu->AddMousePos(cVector2f(-10, 0));
			}
			if(mActionLookRight.IsTriggered())
			{
				mpInit->mpDeathMenu->AddMousePos(cVector2f(10, 0));
			}

			if(mpInit->mbHasHaptics)
			{
				mpInit->mpDeathMenu->AddMousePos(mpLowLevelHaptic->GetRelativeVirtualMousePos() * mfMouseSensitivity);
			}
			else
			{
				/// Mouse Movement
				cVector2f vRel = mpInput->GetMouse()->GetRelPosition() + cVector2f(mActionLookX.TriggeredAmount(), mActionLookY.TriggeredAmount());
				mpInit->mpDeathMenu->AddMousePos(vRel * mfMouseSensitivity);
			}
		}
		///////////////////////////////////////
		// Death ////////////////////
		else if(mpPlayer->IsDead())
		{
			if(mActionEscape.BecameTriggered())
			{
				mpInit->mpMainMenu->SetActive(true);
			}
		}
		///////////////////////////////////////
		// Numerical panel ////////////////////
		else if(mpInit->mpNumericalPanel->IsActive())
		{
			if(mActionInventory.BecameTriggered() || mActionEscape.BecameTriggered())
			{
				mpInit->mpNumericalPanel->OnExit();
			}
			if(mActionRightClick.BecameTriggered())
			{
				mpInit->mpNumericalPanel->OnExit();
			}

			if(mActionClick.BecameTriggered())
			{
				mpInit->mpNumericalPanel->OnMouseDown(eMButton_Left);
				mActionInteract.Trigger();
			}
			if(mActionClick.WasTriggered())
			{
				mpInit->mpNumericalPanel->OnMouseUp(eMButton_Left);
			}

			if(mActionLookUp.IsTriggered())
			{
				mpInit->mpNumericalPanel->AddMousePos(cVector2f(0, -10));
			}
			if(mActionLookDown.IsTriggered())
			{
				mpInit->mpNumericalPanel->AddMousePos(cVector2f(0, 10));
			}
			if(mActionLookLeft.IsTriggered())
			{
				mpInit->mpNumericalPanel->AddMousePos(cVector2f(-10, 0));
			}
			if(mActionLookRight.IsTriggered())
			{
				mpInit->mpNumericalPanel->AddMousePos(cVector2f(10, 0));
			}

			if(mpInit->mbHasHaptics)
			{
				mpInit->mpNumericalPanel->AddMousePos(mpLowLevelHaptic->GetRelativeVirtualMousePos() * mfMouseSensitivity);
			}
			else
			{
				/// Mouse Movement
				cVector2f vRel = mpInput->GetMouse()->GetRelPosition() + cVector2f(mActionLookX.TriggeredAmount(), mActionLookY.TriggeredAmount());
				mpInit->mpNumericalPanel->AddMousePos(vRel * mfMouseSensitivity);
			}
		}
		///////////////////////////////////////
		// Notebook ////////////////////
		else if(mpInit->mpNotebook->IsActive())
		{
			if(mActionInventory.BecameTriggered() || mActionEscape.BecameTriggered())
			{
				mpInit->mpNotebook->OnExit();
			}
			
			if(mActionClick.BecameTriggered())
			{
				mpInit->mpNotebook->OnMouseDown(eMButton_Left);

				mActionInteract.Trigger();
			}

			if(mActionNotebook.BecameTriggered())
			{
				mpInit->mpNotebook->OnExit();
			}
			if(mActionPersonalNotes.BecameTriggered())
			{
				cStateMachine *pStateMachine = mpInit->mpNotebook->GetStateMachine();
				if(pStateMachine->CurrentState()->GetId() == eNotebookState_TaskList)
				{
					pStateMachine->ChangeState(eNotebookState_Front);
					mpInit->mpNotebook->OnExit();
				}
				else
				{
					pStateMachine->ChangeState(eNotebookState_TaskList);
				}
			}

			if(mActionLookUp.IsTriggered())
			{
				mpInit->mpNotebook->AddMousePos(cVector2f(0, -10));
			}
			if(mActionLookDown.IsTriggered())
			{
				mpInit->mpNotebook->AddMousePos(cVector2f(0, 10));
			}
			if(mActionLookLeft.IsTriggered())
			{
				mpInit->mpNotebook->AddMousePos(cVector2f(-10, 0));
			}
			if(mActionLookRight.IsTriggered())
			{
				mpInit->mpNotebook->AddMousePos(cVector2f(10, 0));
			}
			
			if(mpInit->mbHasHaptics)
			{
				mpInit->mpNotebook->AddMousePos(mpLowLevelHaptic->GetRelativeVirtualMousePos() * mfMouseSensitivity);
			}
			else
			{
				/// Mouse Movement
				cVector2f vRel = mpInput->GetMouse()->GetRelPosition() + cVector2f(mActionLookX.TriggeredAmount(), mActionLookY.TriggeredAmount());
				mpInit->mpNotebook->AddMousePos(vRel * mfMouseSensitivity);
			}
		}
		///////////////////////////////////////
		// Inventory ////////////////////
		else if(mpInit->mpInventory->IsActive())
		{
			////////////////////////////
			//Normal Input
			if(mActionInventory.BecameTriggered() || mActionEscape.BecameTriggered())
			{
				mpInit->mpInventory->OnInventoryDown();
			}
			
			if(mActionClick.BecameTriggered())
			{
				mpInit->mpInventory->OnMouseDown(eMButton_Left);

				mActionInteract.Trigger();
			}
			
			if(mActionClick.DoubleTriggered(0.2f))
			{
				mpInit->mpInventory->OnDoubleClick(eMButton_Left);
			}
			if(mActionClick.WasTriggered())
			{
				mpInit->mpInventory->OnMouseUp(eMButton_Left);
			}

				
			if(mActionRightClick.BecameTriggered())
			{
				mpInit->mpInventory->OnMouseDown(eMButton_Right);
				
				mActionExamine.Trigger();
			}
			if(mActionRightClick.WasTriggered())
			{
				mpInit->mpInventory->OnMouseUp(eMButton_Right);
			}
			
			//////////////////////////////
			//Short cut keys
			if(mActionOne.BecameTriggered()) mpInit->mpInventory->OnShortcutDown(0);
			if(mActionTwo.BecameTriggered()) mpInit->mpInventory->OnShortcutDown(1);
			if(mActionThree.BecameTriggered()) mpInit->mpInventory->OnShortcutDown(2);
			if(mActionFour.BecameTriggered()) mpInit->mpInventory->OnShortcutDown(3);
			if(mActionFive.BecameTriggered()) mpInit->mpInventory->OnShortcutDown(4);
			if(mActionSix.BecameTriggered()) mpInit->mpInventory->OnShortcutDown(5);
			if(mActionSeven.BecameTriggered()) mpInit->mpInventory->OnShortcutDown(6);
			if(mActionEight.BecameTriggered()) mpInit->mpInventory->OnShortcutDown(7);
			if(mActionNine.BecameTriggered()) mpInit->mpInventory->OnShortcutDown(8);

			if(mActionLookUp.IsTriggered())
			{
				mpInit->mpInventory->AddMousePos(cVector2f(0, -10));
			}
			if(mActionLookDown.IsTriggered())
			{
				mpInit->mpInventory->AddMousePos(cVector2f(0, 10));
			}
			if(mActionLookLeft.IsTriggered())
			{
				mpInit->mpInventory->AddMousePos(cVector2f(-10, 0));
			}
			if(mActionLookRight.IsTriggered())
			{
				mpInit->mpInventory->AddMousePos(cVector2f(10, 0));
			}
			
			if(mpInit->mbHasHaptics)
			{
				cVector2f vRel = mpLowLevelHaptic->GetRelativeVirtualMousePos();
				mpInit->mpInventory->AddMousePos(vRel * mfMouseSensitivity);
			}
			else
			{
				/// Mouse Movement
				cVector2f vRel = mpInput->GetMouse()->GetRelPosition() + cVector2f(mActionLookX.TriggeredAmount(), mActionLookY.TriggeredAmount());
				mpInit->mpInventory->AddMousePos(vRel * mfMouseSensitivity);
			}
		}
		else
		{
			bPlayerStateIsActive = true;

			if(mActionEscape.BecameTriggered())
			{
				mpInit->mpMainMenu->SetActive(true);
			}
			//if(mpInput->BecameTriggerd("Hit"))
			//{
			//	mpInit->mpPlayer->Damage(20,ePlayerDamageType_BloodSplash);
			//}
			//if(mpInput->BecameTriggerd("Log"))
			//{
			//	mpInit->mpGame->GetPhysics()->SetDebugLog(!mpInit->mpGame->GetPhysics()->GetDebugLog());
			//}
			//if(mpInput->BecameTriggerd("Taunt"))
			//{
//				for(int i=0;i<10; ++i)
	//				mpInit->mpGame->GetSound()->GetSoundHandler()->PlayGui("gui_notebook_add_note1",
			//															false,0.01f);
				//cVector3f vPos = mpInit->mpPlayer->GetCharacterBody()->GetPosition();
				//cSoundEntity *pSound = mpInit->mpGame->GetScene()->GetWorld3D()->CreateSoundEntity("Taunt","interact_homer",true);
                //if(pSound)
				//{
				//	pSound->SetPosition(vPos);
				//}
		//	}
			
			if(mpPlayer->IsActive() || mpPlayer->GetState() == ePlayerState_Message)
			{
				if(mpPlayer->IsActive())
				{
					if(mActionInventory.BecameTriggered())
					{
						mpPlayer->StartInventory();
					}

					if(mActionNotebook.BecameTriggered())
					{
						mpInit->mpNotebook->SetActive(true);
					}
					if(mActionPersonalNotes.BecameTriggered())
					{
						mpInit->mpNotebook->SetActive(true);
						mpInit->mpNotebook->GetStateMachine()->ChangeState(eNotebookState_TaskList);
					}
	                
					if(mActionFlashlight.BecameTriggered())
					{
						mpPlayer->StartFlashLightButton();
					}

					if(mActionGlowStick.BecameTriggered())
					{
						mpPlayer->StartGlowStickButton();
					}

					///////////////////////////////////////
					// Player Movement ////////////////////

					float mul = mActionMoveY.TriggeredAmount();
					if(mActionForward.IsTriggered())
						mul += 1;
					if(mActionBackward.IsTriggered())
						mul -= 1;

					mpPlayer->MoveForwards(float_clamp(mul),afTimeStep);

					mul = mActionMoveX.TriggeredAmount();
					if(mActionLeft.IsTriggered())
						mul -= 1;
					if(mActionBackward.IsTriggered())
						mul += 1;

					mpPlayer->MoveSideways(float_clamp(mul),afTimeStep);

					mul = mActionLean.TriggeredAmount();
					if(mActionLeanLeft.IsTriggered())
						mul -= 1;
					if(mActionLeanRight.IsTriggered())
						mul += 1;

					mpPlayer->Lean(float_clamp(mul), afTimeStep);
					

					if(mActionJump.BecameTriggered())
					{
						mpPlayer->Jump();
					}
					if(mActionJump.IsTriggered())
					{
						mpPlayer->SetJumpButtonDown(true);
					}
					
					if(mActionRun.BecameTriggered())
					{
						mpPlayer->StartRun();
					}
					if(mActionRun.WasTriggered())
					{
						mpPlayer->StopRun();	
					}

					if(mActionCrouch.BecameTriggered())
					{
						mpPlayer->StartCrouch();
					}
					if(GetToggleCrouch())
					{
						if(mActionCrouch.WasTriggered())	mpPlayer->StopCrouch();	
					}
					else
					{
						if(!mActionCrouch.IsTriggered()) mpPlayer->StopCrouch();	
					}

					if(mActionInteractMode.BecameTriggered())
					{
						if(mpInit->mbHasHaptics==false)
						{
							mpPlayer->StartInteractMode();
						}
						else
						{
							//DO nothing for the time being.
						}
					}

					if(mActionLookUp.IsTriggered())
					{
						mpPlayer->AddPitch(-0.05f);
					}
					if(mActionLookDown.IsTriggered())
					{
						mpPlayer->AddPitch(0.05f);
					}
					if(mActionLookLeft.IsTriggered())
					{
						mpPlayer->AddYaw(-0.05f);
					}
					if(mActionLookRight.IsTriggered())
					{
						mpPlayer->AddYaw(0.05f);
					}
					
					//Get the mouse pos and convert it to 0 - 1
					if(mpInit->mbHasHaptics==false)
					{
						cVector2f vRel = mpInput->GetMouse()->GetRelPosition() + cVector2f(mActionLookX.TriggeredAmount(), mActionLookY.TriggeredAmount());
						vRel /= mpLowLevelGraphics->GetVirtualSize();

						mpPlayer->AddYaw(vRel.x * mfMouseSensitivity);
						mpPlayer->AddPitch(vRel.y * mfMouseSensitivity);
					}
				}

				///////////////////////////////////////
				// Player Interaction /////////////////
				if(mActionInteract.BecameTriggered())
				{
					mpPlayer->StartInteract();
					mActionClick.Trigger();
				}
				if(mActionInteract.WasTriggered())
				{
					mpPlayer->StopInteract();
				}
				if(mActionExamine.BecameTriggered())
				{
					mpPlayer->StartExamine();
				}
				if(mActionExamine.WasTriggered())
				{
					mpPlayer->StopExamine();
				}
				if(mActionHolster.BecameTriggered())
				{
					mpPlayer->StartHolster();
				}

				if(mpPlayer->IsActive())
				{
					if(mActionOne.BecameTriggered()) mpPlayer->StartInventoryShortCut(0);
					if(mActionTwo.BecameTriggered()) mpPlayer->StartInventoryShortCut(1);
					if(mActionThree.BecameTriggered()) mpPlayer->StartInventoryShortCut(2);
					if(mActionFour.BecameTriggered()) mpPlayer->StartInventoryShortCut(3);
					if(mActionFive.BecameTriggered()) mpPlayer->StartInventoryShortCut(4);
					if(mActionSix.BecameTriggered()) mpPlayer->StartInventoryShortCut(5);
					if(mActionSeven.BecameTriggered()) mpPlayer->StartInventoryShortCut(6);
					if(mActionEight.BecameTriggered()) mpPlayer->StartInventoryShortCut(7);
					if(mActionNine.BecameTriggered()) mpPlayer->StartInventoryShortCut(8);
				}
			}
		}
		if(!mActionJump.IsTriggered() || bPlayerStateIsActive==false)
		{
			mpPlayer->SetJumpButtonDown(false);
		}
	}
}

//-----------------------------------------------------------------------

void cButtonHandler::Reset()
{

}

//-----------------------------------------------------------------------

void cButtonHandler::OnExit()
{
	// SAVE SETTINGS
	Log("  Saving to config\n");
	mpInit->mpConfig->SetFloat("Controls","MouseSensitivity",mfMouseSensitivity);
	mpInit->mpConfig->SetBool("Controls","InvertMouseY",mbInvertMouseY);
	mpInit->mpConfig->SetBool("Controls","ToggleCrouch",mbToggleCrouch);

	/*// SAVE KEYS
	Log("  Saving keys\n");
	for(int i=0; i< mlNumOfActions; ++i)
	{
		//Log(" Action %s\n",gvDefaultActions[i].msName.c_str());
		
		iAction *pAction = mpInput->GetAction(gvDefaultActions[i].msName);
		tString sType="", sVal="";
		TypeAndValFromAction(pAction,&sType,&sVal);

		//Log(" type %s val: %s\n",sType.c_str(),sVal.c_str());
		
		mpInit->mpConfig->SetString("Keys",gvDefaultActions[i].msName+"_Type",sType);
		mpInit->mpConfig->SetString("Keys",gvDefaultActions[i].msName+"_Val",sVal);
	}*/
}

//-----------------------------------------------------------------------

void cButtonHandler::SetDefaultKeys()
{
	/*cButtonHandlerAction *pBHAction = &gvDefaultActions[0];
	while(pBHAction->msName != "")
	{
		tString sName = pBHAction->msName;
		tString sType = pBHAction->msType;
		tString sVal = cString::ToString(pBHAction->mlVal);
		
		iAction *pAction = ActionFromTypeAndVal(sName, sType, sVal);
		
		if(pAction)
		{
			//mpInput->DestroyAction(sName);
			//mpInput->AddAction(pAction);
		}
		else
		{
			Warning("Couldn't create action from '%s' and %d\n",pBHAction->msType.c_str(),
																	pBHAction->mlVal);
		}

		++pBHAction;
	}*/
}

//-----------------------------------------------------------------------

tString cButtonHandler::GetActionName(const tString &asInputName,const tString &asSkipAction)
{
	cButtonHandlerAction *pBHAction = &gvDefaultActions[0];
	while(pBHAction->msName != "")
	{
		tString sName = pBHAction->msName;
		tString sType = pBHAction->msType;
		tString sVal = cString::ToString(pBHAction->mlVal);

		//iAction *pAction = mpInput->GetAction(sName);
		
		//if(asSkipAction != sName && pAction && pAction->GetInputName() == asInputName) return sName;

		//If at last player action, skip the rest.
		if(sName == gsLastPlayerAction) return "";

		++pBHAction;
	}

	return "";
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

/*iAction* cButtonHandler::ActionFromTypeAndVal(const tString& asName,const tString& asType, const tString& asVal)
{
	//Log("Action %s from %s\n",asName.c_str(),asType.c_str());

	if(asType == "Keyboard")
	{
		return hplNew( cActionKeyboard, (asName,mpInit->mpGame->GetInput(),(eKey)cString::ToInt(asVal.c_str(),0)) );
	}
	else if(asType == "MouseButton" || asType == "HapticDeviceButton")
	{
		if(mpInit->mbHasHaptics && asName != "MouseClick")
		{
			int lNum = cString::ToInt(asVal.c_str(),0);
			if(lNum==2)lNum = 2; 
			else if(lNum==1)lNum = 1;
			return hplNew( cActionHaptic, (asName,mpInit->mpGame->GetHaptic(),lNum) );
		}
		else
		{
			return hplNew( cActionMouseButton, (asName,mpInit->mpGame->GetInput(),(eMButton)cString::ToInt(asVal.c_str(),0)) );
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------

void cButtonHandler::TypeAndValFromAction(iAction *apAction, tString *apType, tString *apVal)
{
	if(apAction)
	{
		*apType = apAction->GetInputType();

		if(apAction->GetInputType() == "Keyboard")
		{
			cActionKeyboard *pKeyAction = static_cast<cActionKeyboard*>(apAction);
			*apVal = cString::ToString((int)pKeyAction->GetKey() | (int)pKeyAction->GetModifier());
		}
		else if(apAction->GetInputType() == "MouseButton" ||
				apAction->GetInputType() == "HapticDeviceButton")
		{
			if(mpInit->mbHasHaptics && apAction->GetName() != "MouseClick")
			{
				cActionHaptic *pHapticAction = static_cast<cActionHaptic*>(apAction);
				*apVal = cString::ToString(pHapticAction->GetButton());
				if(*apVal=="2")*apVal = "2"; 
				else if(*apVal=="1")*apVal = "1";
			}
			else
			{
				cActionMouseButton *pMouseAction = static_cast<cActionMouseButton*>(apAction);
				*apVal = cString::ToString((int)pMouseAction->GetButton());
			}
		}
	}
	else
	{
		*apVal = "";
		*apType = "";
	}
}*/

//-----------------------------------------------------------------------
