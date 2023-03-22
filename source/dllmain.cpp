#include "plugin.h"
#include "common.h"
#include "CMenuManager.h"
#include "CSprite2d.h"
#include "CTheScripts.h"
#include "Utility.h"
#include "CText.h"
#include "imgui.h"
#include "imgui_impl_dx6.h"
#include "imgui_impl_win32.h"

#include "CPlayerPed.h"
#include "CPed.h"
#include "CGame.h"
#include "CObject.h"
#include "CVector.h"
#include "CCar.h"
#include "CPopulation.h"
#include "CWorld.h"
#include "CKeybrd.h"
#include "CGlobal.h"
#include "cDMAudio.h"

#include "extensions/ScriptCommands.h"

#include "CAudioManager.h"
#include "cSampleManager.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ddraw.lib")

#include "tVideo.h"
#include "CAudioManager.h"

using namespace plugin;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static WNDPROC wndProcOld = NULL;

static bool enableDebugCam = false;
static bool openDebugMenuBar = false;
static bool openDebugFlagsWindow = false;
static bool openPlayerWindow = false;
static bool openCarWindow = false;
static bool openCheatsWindow = false;
static bool openGameSettings = false;

struct tResList {
    int w;
    int h;
    char str[16];
};

std::vector<tResList> listOfRes;

class MyPlugin {
public:
    static LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
            return true;

        switch (uMsg) {
        case WM_SIZE:
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU)
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        }

        return CallWindowProc(wndProcOld, hWnd, uMsg, wParam, lParam);
    }

    static void MenuBar() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (GetGame()) {
                    if (ImGui::MenuItem("Save game")) {
                        GetTheScripts()->Save(gGlobal.saveName);
                        DMAudio.PlayVocal(VOCAL_HALLELUJAH);
                    }
                }
                if (ImGui::MenuItem("Exit game", "Alt+F4")) {
                    SendMessage(GetHWnd(), WM_DESTROY, 0, 0);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window")) {
                if (ImGui::MenuItem("Debug flags", "D", openDebugFlagsWindow)) {
                    openDebugFlagsWindow ^= 1;
                }
                if (ImGui::MenuItem("Player", "P", openPlayerWindow)) {
                    openPlayerWindow ^= 1;
                }
                if (ImGui::MenuItem("Car", "C", openCarWindow)) {
                    openCarWindow ^= 1;
                }
                if (ImGui::MenuItem("Cheats", "C", openCheatsWindow)) {
                    openCheatsWindow ^= 1;
                }
                if (ImGui::MenuItem("Game settings", "P", openGameSettings)) {
                    openGameSettings ^= 1;
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    static void DebugFlagsWindow() {
        ImGui::Begin("Debug flags", &openDebugFlagsWindow);   
        ImGui::Checkbox("play_replay", &play_replay);
        ImGui::Checkbox("constant_replay_save", &constant_replay_save);
        ImGui::Checkbox("skip_dummies", &skip_dummies);
        ImGui::Checkbox("do_test", &do_test);
        ImGui::Checkbox("skip_mission", &skip_mission);
        ImGui::Checkbox("show_cycle", &show_cycle);
        ImGui::Checkbox("do_brian_test", &do_brian_test);
        ImGui::Checkbox("do_iain_test", &do_iain_test);
        ImGui::Checkbox("skip_tiles", &skip_tiles);
        ImGui::Checkbox("do_show_counters", &do_show_counters);
        ImGui::Checkbox("do_show_camera", &do_show_camera);
        ImGui::Checkbox("do_show_input", &do_show_input);
        ImGui::Checkbox("do_show_timing", &do_show_timing);
        ImGui::Checkbox("do_show_collision_box", &do_show_collision_box);
        ImGui::Checkbox("do_show_physics", &do_show_physics);
        ImGui::Checkbox("do_show_imaginary", &do_show_imaginary);
        ImGui::Checkbox("skip_user", &skip_user);
        ImGui::Checkbox("skip_traffic_lights", &skip_traffic_lights);
        ImGui::Checkbox("skip_recycling", &skip_recycling);
        ImGui::Checkbox("log_collisions", &log_collisions);
        ImGui::Checkbox("do_show_traffic_lights_info", &do_show_traffic_lights_info);
        ImGui::Checkbox("limit_recycling", &limit_recycling);
        ImGui::Checkbox("no_annoying_chars", &no_annoying_chars);
        ImGui::Checkbox("skip_slopes", &skip_slopes);
        ImGui::Checkbox("skip_left", &skip_left);
        ImGui::Checkbox("skip_right", &skip_right);
        ImGui::Checkbox("skip_top", &skip_top);
        ImGui::Checkbox("skip_bottom", &skip_bottom);
        ImGui::Checkbox("skip_lid", &skip_lid);
        ImGui::Checkbox("log_routefinder", &log_routefinder);
        ImGui::Checkbox("do_mike", &do_mike);
        ImGui::Checkbox("skip_particles", &skip_particles);
        ImGui::Checkbox("show_hidden_faces", &show_hidden_faces);
        ImGui::Checkbox("get_all_weapons", &get_all_weapons);
        ImGui::Checkbox("do_exit_after_replay", &do_exit_after_replay);
        ImGui::Checkbox("dont_get_car_back", &dont_get_car_back);
        ImGui::Checkbox("do_show_instruments", &do_show_instruments);
        ImGui::Checkbox("skip_ambulance", &skip_ambulance);
        ImGui::Checkbox("skip_police", &skip_police);
        ImGui::Checkbox("skip_frontend", &skip_frontend);
        ImGui::Checkbox("do_invulnerable", &do_invulnerable);
        ImGui::Checkbox("show_all_arrows", &show_all_arrows);
        ImGui::Checkbox("do_show_horn", &do_show_horn);
        ImGui::Checkbox("keep_weapons_after_death", &keep_weapons_after_death);
        ImGui::Checkbox("skip_skidmarks", &skip_skidmarks);
        ImGui::Checkbox("do_show_junc_ids", &do_show_junc_ids);
        ImGui::Checkbox("do_corner_window", &do_corner_window);
        ImGui::Checkbox("do_infinite_lives", &do_infinite_lives);
        ImGui::Checkbox("do_load_savegame", &do_load_savegame);
        ImGui::Checkbox("skip_audio", &skip_audio);
        ImGui::Checkbox("do_debug_keys", &do_debug_keys);
        ImGui::Checkbox("log_random", &log_random);
        ImGui::Checkbox("log_random_extra", &log_random_extra);
        ImGui::Checkbox("log_input", &log_input);
        ImGui::Checkbox("log_directinput", &log_directinput);
        ImGui::Checkbox("ignore_replay_header", &ignore_replay_header);
        ImGui::Checkbox("skip_trains", &skip_trains);
        ImGui::Checkbox("skip_buses", &skip_buses);
        ImGui::Checkbox("skip_quit_confirm", &skip_quit_confirm);
        ImGui::Checkbox("do_sync_check", &do_sync_check);
        ImGui::Checkbox("skip_fire_engines", &skip_fire_engines);
        ImGui::Checkbox("show_brief_number", &show_brief_number);
        ImGui::Checkbox("skip_window_check", &skip_window_check);
        ImGui::Checkbox("skip_replay_sync_check", &skip_replay_sync_check);
        ImGui::Checkbox("do_show_object_ids", &do_show_object_ids);
        ImGui::Checkbox("do_kill_phones_on_answer", &do_kill_phones_on_answer);
        ImGui::Checkbox("do_miss_logging", &do_miss_logging);
        ImGui::Checkbox("do_text_id_test", &do_text_id_test);
        ImGui::Checkbox("do_police_1", &do_police_1);
        ImGui::Checkbox("do_police_2", &do_police_2);
        ImGui::Checkbox("do_police_3", &do_police_3);
        ImGui::Checkbox("skip_draw", &skip_draw);
        ImGui::Checkbox("do_free_shopping", &do_free_shopping);
        ImGui::Checkbox("do_release_replay", &do_release_replay);
        ImGui::Checkbox("do_blood", &do_blood);
        ImGui::Checkbox("do_3d_sound", &do_3d_sound);
        ImGui::Checkbox("french_game", &french_game);
        ImGui::Checkbox("show_player_names", &show_player_names);    
        ImGui::End();
    }

    static void CheatsWindow() {
        CGame* game = GetGame();

        if (game) {
            CPlayerPed* playa = game->m_pCurrentPlayer;

            ImGui::SetWindowSize({ 128.0f, 128.0f }, ImGuiCond_::ImGuiCond_FirstUseEver);
            ImGui::Begin("Cheats", &openCheatsWindow);

            static int car = 0;
            static const char* carNames[] = {
                "ALFA", "ALLARD", "AMDB4", "APC", "BANKVAN", "BMW", "BOXCAR", "BOXTRUCK", "BUG", "CAR9", "BUICK", "BUS", "COPCAR", "DART", "EDSEL", "CAR15", "FIAT", "FIRETRUK",
                "GRAHAM", "GT24640", "CAR20", "GTRUCK", "GUNJEEP", "HOTDOG", "HOTDOG_D1", "HOTDOG_D2", "HOTDOG_D3", "ICECREAM", "ISETLIMO", "ISETTA", "JEEP", "JEFFREY",
                "LIMO", "LIMO2", "MEDICAR", "MERC", "MESSER", "MIURA", "MONSTER", "MORGAN", "MORRIS", "PICKUP", "RTYPE", "CAR43", "SPIDER", "SPRITE", "STINGRAY", "STRATOS",
                "STRATOSB", "STRIPETB", "STYPE", "STYPECAB", "SWATVAN", "T2000GT", "TANK", "TANKER", "TAXI", "TBIRD", "TOWTRUCK", "TRAIN", "TRAINCAB", "TRAINFB", "TRANCEAM",
                "TRUKCAB1", "TRUKCAB2", "TRUKCONT", "TRUKTRNS", "TVVAN", "VAN", "VESPA", "VTYPE", "WBTWIN", "WRECK0", "WRECK1", "WRECK2", "WRECK3", "WRECK4", "WRECK5",
                "WRECK6", "WRECK7", "WRECK8", "WRECK9", "XK120", "ZCX5", "EDSELFBI", "HOTDOG_D4", "KRSNABUS"
            };

            ImGui::Combo("Vehicles", &car, carNames, IM_ARRAYSIZE(carNames));

            if (ImGui::Button("Spawn")) {
                if (car == MODEL_BOXCAR ||
                    car == MODEL_TRAIN ||
                    car == MODEL_TRAINCAB ||
                    car == MODEL_TRAINFB)
                    return;

                CEncodedVector pos = playa->m_pPed->m_vPosition;
                CVector p = pos.FromInt16();

                float a = 0.0f;
                short spriteRotation = 0.0f;
                if (playa->m_pPed->m_pObject) {
                    spriteRotation = playa->m_pPed->m_pObject->m_nRotation;
                    a = spriteRotation / 4.0f + 270.0f;
                }
                else if (playa->m_pPed->m_pCurrentCar) {
                    spriteRotation = playa->m_pPed->m_pCurrentCar->m_pPhysics->m_nRotation;
                    a = spriteRotation / 4.0f + 270.0f;

                }
                p.x += (cos(a * 0.017453f) * 1.0f);
                p.y -= (sin(a * 0.017453f) * 1.0f);

                pos = p.ToInt16();

                int result = 0;
                GetWorld()->FindGroundZForCoord(&result, pos.x, pos.y);
                GetPopulationManager()->SpawnCar(pos.x, pos.y, result, spriteRotation - 360, (char)car);
            }
            ImGui::Spacing();

            static int wep = 0;
            static const char* weapNames[] = {
                    "PISTOL",
                    "UZI",
                    "ROCKETLAUNCHER",
                    "ELECTROGUN",
                    "MOLOTOV",
                    "GRENADE",
                    "SHOTGUN",
                    "ELECTROBATON",
                    "FLAMETHROWER",
                    "SILENCEDUZI",
                    "DUALPISTOL",
            };

            ImGui::Combo("Weapons", &wep, weapNames, IM_ARRAYSIZE(weapNames));

            if (ImGui::Button("Get weapon")) {
                playa->m_pPed->GiveWeapon(wep, 999);
            }

            ImGui::Checkbox("God mode", &do_invulnerable);
            if (ImGui::Button("Give money")) {
                playa->GiveMoney(100000);
            }
            if (ImGui::Button("Take money")) {
                playa->GiveMoney(-100000);
            }

            ImGui::End();
        }
    }

    static void PlayerWindow() {
        CGame* game = GetGame();

        if (game) {
            CPlayerPed* playa = game->m_pCurrentPlayer;

            ImGui::SetWindowSize({ 128.0f, 256.0f }, ImGuiCond_::ImGuiCond_FirstUseEver);
            ImGui::Begin("Player", &openPlayerWindow);

            if (playa) {
                static float step = 1.0f;
                static float stepFast = 64.0f;
                static CVector pos = playa->GetPed()->GetPosition().FromInt16();
                static float x = pos.x;
                static float y = pos.y;
                static float z = pos.z;

                ImGui::InputFloat("x", &x, step, stepFast);
                ImGui::InputFloat("y", &y, step, stepFast);
                ImGui::InputFloat("z", &z, step, stepFast);

                if (ImGui::Button("Reset")) {
                    pos = playa->GetPed()->GetPosition().FromInt16();
                    x = pos.x;
                    y = pos.y;
                    z = pos.z;
                }

                if (ImGui::Button("Set Position")) {
                    pos = { x, y, z };

                    if (playa->GetPed()->GetCar())
                        playa->GetPed()->GetCar()->SetPosition(pos.ToInt16());
                    else
                        playa->GetPed()->GetSprite()->SetPosition(pos.ToInt16());
                }

                if (playa->m_pPed) {
                    static unsigned char remap = playa->m_pPed->m_nRemap;
                    ImGui::InputScalar("Remap", ImGuiDataType_::ImGuiDataType_U8, &remap);

                    if (ImGui::Button("Set remap")) {
                        playa->m_pPed->m_nRemap = remap;
                    }

                    static int wantedLevel = 0;
                    if (ImGui::Button("Increase Wanted Level") && wantedLevel < 6) {
                        wantedLevel++;
                        playa->m_pPed->SetWantedLevel(wantedLevel);
                    }
                    if (ImGui::Button("Decrease Wanted Level") && wantedLevel > 0) {
                        wantedLevel--;
                        playa->m_pPed->SetWantedLevel(wantedLevel);
                    }
                    if (ImGui::Button("Clear Wanted Level")) {
                        wantedLevel = 0;
                        playa->m_pPed->ClearWantedLevel();
                    }
                    if (playa->m_pPed->m_nWantedLevel < 1)
                        wantedLevel = 0;

                    static float maxSpeed = playa->GetPed()->m_fMaxDriveSpeed;
                    if (ImGui::InputFloat("MaxRunSpeed", &maxSpeed, step, stepFast)) {
                        playa->GetPed()->m_fMaxRunSpeed = maxSpeed;
                    }

                }
            }

            ImGui::End();
        }
    }

    static void CarWindow() {
        CGame* game = GetGame();

        if (game) {
            CPlayerPed* playa = game->FindPlayerPed(0);

            ImGui::Begin("Vehicle", &openCarWindow);

            if (playa) {
                CCar* car = playa->m_pPed->GetCar();

                if (car) {
                    static int remap = 0;
                    static const char* remapNames[] = {
                        "CAR_COLOR_BROWN",
                        "CAR_COLOR_BLACK",
                        "CAR_COLOR_BLACK_3",
                        "CAR_COLOR_BLUE",
                        "CAR_COLOR_BLUE_GREY",
                        "CAR_COLOR_BRIGHT_GREEN",
                        "CAR_COLOR_BRIGHT_RED",
                        "CAR_COLOR_BROWN_2",
                        "CAR_COLOR_BROWNER",
                        "CAR_COLOR_BRIGHT_SILVER",
                        "CAR_COLOR_BRIGHT_YELLOW",
                        "CAR_COLOR_CREAM",
                        "CAR_COLOR_CYAN",
                        "CAR_COLOR_DARK_BEIGE",
                        "CAR_COLOR_DARK_BLUE",
                        "CAR_COLOR_BLUE_2",
                        "CAR_COLOR_DARK_GREEN",
                        "CAR_COLOR_DARK_RED",
                        "CAR_COLOR_DARK_RUST",
                        "CAR_COLOR_GOLD",
                        "CAR_COLOR_GREEN",
                        "CAR_COLOR_GREY",
                        "CAR_COLOR_LIGHT_BROWN",
                        "CAR_COLOR_OLIVE",
                        "CAR_COLOR_ORANGE",
                        "CAR_COLOR_PALE_BLUE",
                        "CAR_COLOR_PINK_RED",
                        "CAR_COLOR_PURPLE",
                        "CAR_COLOR_RED",
                        "CAR_COLOR_RUST",
                        "CAR_COLOR_SILVER",
                        "CAR_COLOR_SKYBLUE",
                        "CAR_COLOR_TURQUOISE",
                        "CAR_COLOR_PALE_SILVER",
                        "CAR_COLOR_WHITER",
                        "CAR_COLOR_YELLOW",
                    };

                    if (ImGui::Combo("Color", &remap, remapNames, IM_ARRAYSIZE(remapNames)))
                        car->SetRemap(remap);
                }
            }
            ImGui::End();

        }
    }
    static void SetResolutionsList() {
        DEVMODE dm = {};
        dm.dmSize = sizeof(DEVMODE);
        for (auto i = 0; EnumDisplaySettings(NULL, i, &dm) != 0; i++) {
            if (dm.dmPelsWidth >= 640 && dm.dmPelsHeight >= 480 && dm.dmBitsPerPel == 32 && dm.dmDisplayFixedOutput == DMDFO_DEFAULT && dm.dmDisplayFrequency == 60) {
                tResList t;

                t.w = dm.dmPelsWidth;
                t.h = dm.dmPelsHeight;

                sprintf(t.str, "%dx%d", dm.dmPelsWidth, dm.dmPelsHeight);
                listOfRes.push_back(t);
            }
        }
    }

    static void GameSettingsWindow() {
        ImGui::SetWindowPos({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }, ImGuiCond_::ImGuiCond_Appearing);
        ImGui::SetNextWindowContentSize({ 8.0f, 8.0f });

        ImGui::Begin("Game settings", &openGameSettings);
        static int currentItem = 0;
        static const char* res = listOfRes.at(currentItem).str;
        if (ImGui::BeginCombo("resolution", res, ImGuiComboFlags_None)) {
            for (int i = 0; i < listOfRes.size(); i++) {
                ImGui::PushID(i);
                const bool item_selected = (i == currentItem);

                if (ImGui::Selectable(listOfRes.at(i).str, item_selected)) {
                    res = listOfRes.at(i).str;
                    currentItem = i;
                }
                if (item_selected)
                    ImGui::SetItemDefaultFocus();

                ImGui::PopID();
            }

            ImGui::EndCombo();
        }
        static bool screenType = start_mode;
        ImGui::Checkbox("windowed", &screenType);
        if (ImGui::Button("Set")) {
            window_width = listOfRes.at(currentItem).w;
            window_height = listOfRes.at(currentItem).h;
            start_mode = screenType;
            ResetVideoDevice();
        }

        ImGui::Checkbox("exploding_on", (bool*)&exploding_on);
        if (ImGui::Checkbox("lighting", (bool*)&lighting)) lightFlag = lighting ? 0x8000 : 0;
        ImGui::Checkbox("triple_buffer", (bool*)&triple_buffer);
        ImGui::NewLine();

        static int minVol = 0;
        static int maxVol = 127;
        static int sfx = AudioManager.m_nEffectsVolume;
        static int music = AudioManager.m_nMusicVolume;
        if (ImGui::SliderInt("sfx", &sfx, minVol, maxVol))
            DMAudio.SetEffectsMasterVolume(sfx);

        if (ImGui::SliderInt("music", &music, minVol, maxVol))
            DMAudio.SetMusicMasterVolume(music);

        if (GetTheText()) {
            static int lang = 0;
            static const char* languages[] = {
                "english",
                "french",
                "german",
                "italian",
                "spanish",
                "japanese",
            };
            if (ImGui::Combo("Language", &lang, languages, IM_ARRAYSIZE(languages))) {
                GetTheText()->language = languages[lang][0];
                GetTheText()->Unload();
                GetTheText()->Load();
            }
        }
        ImGui::End();
    }

    static void DoUI() {
        if (openDebugMenuBar) {
            MenuBar();
        }
        else
            return;

        if (openDebugFlagsWindow) {
            DebugFlagsWindow();
        }

        if (openPlayerWindow) {
            PlayerWindow();
        }

        if (openCarWindow) {
            CarWindow();
        }

        if (openCheatsWindow) {
            CheatsWindow();
        }

        if (openGameSettings) {
            GameSettingsWindow();
        }
    }

    static void OpenCloseMenus() {
        openDebugMenuBar ^= true;
    }

    static void Begin() {
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGui_ImplDx6_NewFrame();
    }

    static void End() {
        ImGui::EndFrame();
        ImGui::Render();

        ImGui_ImplDX6_RenderDrawData(ImGui::GetDrawData());
    }

    static void Draw() {
        if (ImGui::IsKeyDown(ImGuiKey_::ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_::ImGuiKey_M, false)) {
            OpenCloseMenus();

            ImGuiIO& io = ImGui::GetIO();
            io.MouseDrawCursor = openDebugMenuBar;
        }

        if (ImGui::IsKeyDown(ImGuiKey_::ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_::ImGuiKey_B, false))
            enableDebugCam ^= true;

        DoUI();
    }

    MyPlugin() {
#ifdef _DEBUG
       //AllocConsole();
       //freopen("conin$", "r", stdin);
       //freopen("conout$", "w", stdout);
       //freopen("conout$", "w", stderr);
       //std::setvbuf(stdout, NULL, _IONBF, 0);
#endif

        plugin::Events::initEngineEvent += []() {
            ImGui_ImplDX6_Init();
            ImGui_ImplWin32_Init(GetHWnd());
            ImGui::StyleColorsDark();

            wndProcOld = (WNDPROC)GetWindowLong(GetHWnd(), GWL_WNDPROC);
            SetWindowLong(GetHWnd(), GWL_WNDPROC, (LONG)WndProc);

            SetResolutionsList();
        };

        plugin::Events::shutdownEngineEvent += []() {
            ImGui_ImplWin32_Shutdown();
            ImGui_ImplDX6_Shutdown();
        };

        plugin::Events::d3dResetEvent += []() {
            ImGui_ImplDX6_Reset();
        };

        plugin::Events::menuDrawingEvent += [] {
            Begin();
            Draw();
            End();
        };     

        plugin::Events::drawHudEvent += [] {
            Begin();
            Draw();
            End();
        };
    };
} myPlugin;
